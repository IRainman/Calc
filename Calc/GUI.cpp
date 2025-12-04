// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2025 Solomina Elle, a.rainman on gmail point com
 */
 
// GUI.cpp : Defines the class behaviors for the application.
//

#include "pch.h"

#include "lexer.h"
#include "parser.h"
#include "issue_manager.h"
#include "formatter.h"
#ifdef CALC_TESTS_ENABLED
#include "tests.h"
#endif

// ================= GUI ===============

#ifdef _WIN32

//#define CALC_SUPPORT_WINDOWS_XP // deprecated by Windows SDK
//#define CALC_SUPPORT_WINDOWS_7_8_81 // deprecated by Windows SDK since version 18 of the MSVC compiler.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // Ensure that we using min/max from std algorithms.

#include "targetver.h"

#include <immintrin.h>

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <imm.h>
#include <shellapi.h>
#include "resource.h"		// main symbols

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "imm32.lib")

// ---------- configuration ----------
static constexpr std::string_view kRegKey = "Software\\HedgehogInTheCPP\\Calc";

// ------------------------------------------------------------------
// registry helper
// ------------------------------------------------------------------
static void RegWriteDword(const HKEY root, const std::string_view subkey, const std::string_view name, const DWORD value) {
    HKEY hKey [[indeterminate]];
    if (RegCreateKeyExA(root, subkey.data(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, name.data(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
        RegCloseKey(hKey);
    }
}

// ------------------------------------------------------------------
// registry helper
// ------------------------------------------------------------------
static std::optional<DWORD> RegReadDword(const HKEY root, const std::string_view subkey, const std::string_view name) {
    HKEY hKey [[indeterminate]];
    if (RegOpenKeyExA(root, subkey.data(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) return std::nullopt;

    DWORD out [[indeterminate]]; DWORD outSize = sizeof(out); DWORD type [[indeterminate]];
    if (RegQueryValueExA(hKey, name.data(), nullptr, &type, reinterpret_cast<LPBYTE>(&out), &outSize) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return std::nullopt;
    }
    RegCloseKey(hKey);
    if (type != REG_DWORD) {
        return std::nullopt;
    }
    return out;
}

// ANSI multiline EDIT max is 64 KiB for classic Edit control
static std::array<char, 64 * 1024> g_input; // 65535 for symbols and 1 for null C string API terminator

class CalcWindowState {
    struct Baseline {
        constexpr static UINT dpi = 96;
        constexpr static UINT min_height = 280;
        constexpr static UINT min_width = 240;
        constexpr static UINT point_size = 10; // matches RC font 10 pt in resource template
    };
    constexpr static Baseline baseline;

    struct Anchor {
        int id;
        RECT rc;
        const bool left, top, right, bottom;
        auto get_x() const {
            return static_cast<UINT>(rc.left);
		}
        auto get_y() const {
            return static_cast<UINT>(rc.top);
        }
        auto get_width() const {
            return static_cast<UINT>(rc.right - rc.left);
		}
        auto get_height() const {
            return static_cast<UINT>(rc.bottom - rc.top);
        }
    };
    std::vector<Anchor> anchors;

    HFONT font;
    UINT dpi [[indeterminate]];
    UINT min_width [[indeterminate]];
    UINT width [[indeterminate]];
    UINT min_heigth [[indeterminate]];
    UINT heigth [[indeterminate]];

public:
    CalcWindowState() noexcept : font(nullptr) { }
    CalcWindowState(const CalcWindowState&) = delete;
    CalcWindowState(CalcWindowState&&) = delete;
    ~CalcWindowState() {
		cleanup_font();
    }
    auto get_font() const {
        return font;
	}
    auto get_dpi() const {
        return dpi;
    }
    auto get_min_width() const {
        return min_width;
	}
    auto get_min_heigth() const {
        return min_heigth;
    }
    auto get_min_x() const {
        return get_min_width();
    }
    auto get_min_y() const {
        return get_min_heigth();
    }
    void close(const HWND hWnd) const {
		save_window_placement(hWnd);
        EndDialog(hWnd, 0);
    }
    void initialize(const HWND hWnd) {
        RECT wr [[indeterminate]];
        const auto wr_ok = GetWindowRect(hWnd, &wr);
        dpi = get_window_dpi(hWnd);
        width = wr_ok ? static_cast<UINT>(wr.right - wr.left) : baseline.min_width;
        heigth = wr_ok ? static_cast<UINT>(wr.bottom - wr.top) : baseline.min_height;

        set_dpi(hWnd, dpi);
        init_anchors(hWnd);
		load_window_placement(hWnd);

#ifdef CALC_TESTS_ENABLED
        SetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, calc_tests().c_str());
#endif
        const auto hInput = GetDlgItem(hWnd, IDC_EDIT_INPUT);
        if (hInput) {
            // IME: associate default context for multiline edit
            ImmAssociateContextEx(hInput, nullptr, IACE_DEFAULT);
            SetFocus(hInput);
        }
        add_about_menu_to_system_menu(hWnd);
        set_window_icons(hWnd);
    }
    static UINT get_window_dpi(const HWND hWnd) {
        // Try GetDpiForWindow (Windows 10+). Dynamically resolve to keep compatibility.
        const HMODULE user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
            auto fn = reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(user32, "GetDpiForWindow"));
            if (fn) {
                const UINT dpi = fn(hWnd);
                if (dpi > 0) return dpi;
            }
        }
        // Fallback: get device caps from nearest monitor / device context.
        const HDC hdc = GetDC(hWnd);
        if (hdc) {
            const int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
            ReleaseDC(hWnd, hdc);
            if (dpiY > 0) return static_cast<UINT>(dpiY);
        }
        // Last fallback, query primary screen
        const HDC screen = GetDC(nullptr);
        if (screen) {
            const int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
            ReleaseDC(nullptr, screen);
            if (dpiY > 0) return static_cast<UINT>(dpiY);
        }
        return baseline.dpi;
    }
    void set_dpi(const HWND dlg, const UINT new_dpi) {
        if (new_dpi == baseline.dpi) {
            min_width = baseline.min_width;
            min_heigth = baseline.min_height;
        }
        else {
            min_width = scale(baseline.min_width, baseline.dpi, new_dpi);
            min_heigth = scale(baseline.min_height, baseline.dpi, new_dpi);
        }
        const auto scale = static_cast<double>(new_dpi) / static_cast<double>(dpi);
        rescale_anchors(scale);
        dpi = new_dpi;
        //==============
        cleanup_font();
        set_font(dlg);
    }
    void update_controls(const HWND hDlg) const {
        for (const auto& a : anchors) {
            const HWND hCtrl = GetDlgItem(hDlg, a.id);
            if (hCtrl) {
                MoveWindow(hCtrl, a.get_x(), a.get_y(), a.get_width(), a.get_height(), TRUE);
            }
        }
    }
    void resize(UINT clientW, UINT clientH) {
        clientW = std::max(clientW, get_min_width());
        clientH = std::max(clientH, get_min_heigth());

        const auto w = width - clientW;
        const auto h = heigth - clientH;

        for (auto& a : anchors) {
			a.rc.bottom = a.bottom ? (a.rc.bottom - h) : a.rc.bottom;
			a.rc.left = a.left ? a.rc.left : (a.rc.left - w);
			a.rc.top = a.top ? a.rc.top : (a.rc.top - h);
			a.rc.right = a.right ? (a.rc.right - w) : a.rc.right;
        }

		width = clientW;
        heigth = clientH;
    }
private:
    static UINT scale(const UINT base_px, const UINT base_dpi, const UINT target_dpi) {
		return base_dpi == target_dpi ? base_px : static_cast<UINT>(std::lround(static_cast<double>(base_px) * static_cast<double>(target_dpi) / static_cast<double>(base_dpi)));
    }
    void init_anchors(const HWND dlg) {
        const auto add = [&](const int id, const bool left, const bool top, const bool right, const bool bottom) {
            const auto h = GetDlgItem(dlg, id);
            if (!h) return;

            RECT wr [[indeterminate]]; if (!GetWindowRect(h, &wr)) return;
            POINT p{ wr.left, wr.top }; ScreenToClient(dlg, &p);
            RECT rcClient{ p.x, p.y, p.x + (wr.right - wr.left), p.y + (wr.bottom - wr.top) };
            anchors.emplace_back(Anchor{ id, rcClient, left, top, right, bottom });
            };

        add(IDC_EDIT_INPUT, true, true, true, true);    // flexible input
        add(IDC_EDIT_RESULT, true, false, true, true);  // bottom anchored
        add(IDC_EDIT_MESSAGE, true, false, true, true); // bottom anchored
        add(IDC_BUTTON_CALC, false, false, true, true); // button bottom-right
    }
    void rescale_anchors(const double scale) {
        for (auto& a : anchors) {
            // scale to target DPI
            a.rc.left = std::lround(a.rc.left * scale);
            a.rc.top = std::lround(a.rc.top * scale);
            a.rc.right = std::lround(a.rc.right * scale);
            a.rc.bottom = std::lround(a.rc.bottom * scale);
        }
    }
    void set_font(const HWND dlg) {
        // Height in logical units: negative value means character height; formula:
        // height = -MulDiv(pointSize, dpi, 72)
        constexpr int pointSize = 10; // matches RC font 10 pt in resource template
        const auto height = -MulDiv(pointSize, static_cast<int>(dpi), 72);
        font = CreateFontA(height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");

        if (font) {
            SendMessageA(dlg, WM_SETFONT, reinterpret_cast<WPARAM>(font), MAKELPARAM(TRUE, 0));
            constexpr int controlIds[] = { IDC_EDIT_INPUT, IDC_EDIT_RESULT, IDC_EDIT_MESSAGE, IDC_BUTTON_CALC };
            for (int id : controlIds) {
                const HWND h = GetDlgItem(dlg, id);
                if (h) SendMessageA(h, WM_SETFONT, reinterpret_cast<WPARAM>(font), MAKELPARAM(TRUE, 0));
            }
        }
        else {
            // error
		}
	}
    void cleanup_font() {
        if (font) {
            DeleteObject(font);
			font = nullptr;
        }
	}
    static void center_window_on_monitor(const HWND hWnd, const HMONITOR hMon) {
        RECT wr [[indeterminate]]; if (!GetWindowRect(hWnd, &wr)) { SetWindowPos(hWnd, nullptr, 100, 100, 0, 0, SWP_NOZORDER | SWP_NOSIZE); return; }
        MONITORINFO mi [[indeterminate]]; mi.cbSize = sizeof(mi);
        if (!GetMonitorInfo(hMon, &mi)) SystemParametersInfoW(SPI_GETWORKAREA, 0, &mi.rcWork, 0);
        const auto w = wr.right - wr.left, h = wr.bottom - wr.top;
        const auto x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - w) / 2;
        const auto y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - h) / 2;
        SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    void load_window_placement(const HWND hWnd) const {
        const auto sShow = RegReadDword(HKEY_CURRENT_USER, kRegKey, "showCmd");
        const auto sFlags = RegReadDword(HKEY_CURRENT_USER, kRegKey, "flags");
        const auto sLeft = RegReadDword(HKEY_CURRENT_USER, kRegKey, "left");
        const auto sTop = RegReadDword(HKEY_CURRENT_USER, kRegKey, "top");
        const auto sRight = RegReadDword(HKEY_CURRENT_USER, kRegKey, "right");
        const auto sBottom = RegReadDword(HKEY_CURRENT_USER, kRegKey, "bottom");

        WINDOWPLACEMENT wp [[indeterminate]]; wp.length = sizeof(wp);
        wp.showCmd = static_cast<int>(sShow ? *sShow : SW_SHOWNORMAL);
        wp.flags = static_cast<UINT>(sFlags ? *sFlags : 0u);
        wp.rcNormalPosition.left = static_cast<LONG>(sLeft ? *sLeft : 100);
        wp.rcNormalPosition.top = static_cast<LONG>(sTop ? *sTop : 100);
        wp.rcNormalPosition.right = static_cast<LONG>(sRight ? *sRight : get_min_width() + 100);
        wp.rcNormalPosition.bottom = static_cast<LONG>(sBottom ? *sBottom : get_min_width() + 100);

        const auto hMon = MonitorFromRect(&wp.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi [[indeterminate]]; mi.cbSize = sizeof(mi);
        if (!GetMonitorInfoA(hMon, &mi)) {
            POINT pt [[indeterminate]];
            GetCursorPos(&pt);
            const auto hMon2 = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            center_window_on_monitor(hWnd, hMon2);
            return;
        }
        const RECT& work = mi.rcWork;
        if (wp.rcNormalPosition.right <= work.left || wp.rcNormalPosition.left >= work.right || wp.rcNormalPosition.bottom <= work.top || wp.rcNormalPosition.top >= work.bottom) {
            center_window_on_monitor(hWnd, hMon);
            return;
        }
        SetWindowPlacement(hWnd, &wp);
    }
    void save_window_placement(const HWND hWnd) const {
        WINDOWPLACEMENT wp [[indeterminate]]; wp.length = sizeof(wp);
        if (GetWindowPlacement(hWnd, &wp)) {
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "showCmd", static_cast<DWORD>(wp.showCmd));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "flags", static_cast<DWORD>(wp.flags));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "left", static_cast<DWORD>(wp.rcNormalPosition.left));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "top", static_cast<DWORD>(wp.rcNormalPosition.top));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "right", static_cast<DWORD>(wp.rcNormalPosition.right));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "bottom", static_cast<DWORD>(wp.rcNormalPosition.bottom));
        }
    }
    // ---------- Create menu ----------
    static void add_about_menu_to_system_menu(const HWND hDlg) {
        // Add "About..." menu item to system menu.
        // IDM_ABOUTBOX must be in the system command range.
        static_assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
        static_assert(IDM_ABOUTBOX < 0xF000);

        const auto hSys = GetSystemMenu(hDlg, FALSE);
        if (hSys) {
            AppendMenuA(hSys, MF_SEPARATOR, 0, nullptr);
            AppendMenuA(hSys, MF_STRING, IDM_ABOUTBOX, "&About...");
        }
    }
    // ------------------------------------------------------------------
    // Utility: set dialog icon (small & big) from IDR_MAINFRAME
    // ------------------------------------------------------------------
    static void set_window_icons(const HWND hDlg) {
        // Set icons (large and small)
        const auto icon = LoadIconA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDR_MAINFRAME));
        if (icon) {
            SendMessageA(hDlg, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
            SendMessageA(hDlg, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
        }
    }
};
static CalcWindowState CalcWindow [[indeterminate]];

// ------------------------------------------------------------------
// Calc!
// ------------------------------------------------------------------
static inline void ExecuteCalculation(const HWND hDlg) {
    const auto copied = GetDlgItemTextA(hDlg, IDC_EDIT_INPUT, g_input.data(), static_cast<int>(g_input.size()));
    if (!copied) {
        SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, "");
        SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, "");
    }
    else {
        const std::string_view sv{ g_input.data(), copied };

        Lexer l(sv);
        Parser p(l);
        const auto result = p.parse();

#ifndef CALC_USE_ERROR_TOKEN
        const auto hasErr = IssueManager::has_errors();
#endif

        if (hasErr) {
            SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, "");
            const auto summary = Formatter::create_summary();
            SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, summary.data());
#ifndef CALC_USE_ERROR_TOKEN
            IssueManager::clear();
#endif
        }
        else {
            const auto out = Formatter::format(result);
            SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, out.data());
            SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, "");
        }
    }
    SetFocus(GetDlgItem(hDlg, IDC_EDIT_INPUT));
}

// ------------------------------------------------------------------
// About dialog proc (resource-based).
// ------------------------------------------------------------------
static INT_PTR CALLBACK AboutDlgProc(const HWND hDlg, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
    switch (uMsg) {
    //case WM_INITDIALOG: {
    //    return TRUE;
    //}
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, 0); return TRUE;
        }
        return FALSE;
    case WM_NOTIFY: {
        const auto nm = reinterpret_cast<LPNMHDR>(lParam);
        if (nm && nm->idFrom == IDC_LINK_HOMEPAGE && nm->code == NM_CLICK) {
            const auto link = reinterpret_cast<NMLINK*>(lParam);
            if (link && link->item.szUrl) {
                ShellExecuteW(nullptr, L"open", link->item.szUrl, nullptr, nullptr, SW_SHOWNORMAL);
            }
            return TRUE;
        }
        return FALSE;
    }
    }
    return FALSE;
}

// ------------------------------------------------------------------
// Calc dialog proc (resource-based).
// ------------------------------------------------------------------
static INT_PTR CALLBACK CalcDialogProc(const HWND hDlg, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND: {
        const auto id = static_cast<UINT>(LOWORD(wParam));
        const auto notify = static_cast<UINT>(HIWORD(wParam));
        if (id == IDC_BUTTON_CALC && notify == BN_CLICKED) {
            ExecuteCalculation(hDlg);
            return TRUE;
        }
        return FALSE;
    }
    case WM_SIZE: {
        const auto clientW = static_cast<UINT>(LOWORD(lParam));
        const auto clientH = static_cast<UINT>(HIWORD(lParam));
        CalcWindow.resize(clientW, clientH);
        CalcWindow.update_controls(hDlg);
        return TRUE;
    }
    case WM_GETMINMAXINFO: {
        const auto lpMMI = reinterpret_cast<LPMINMAXINFO>(lParam);
        if (lpMMI) {
            lpMMI->ptMinTrackSize.x = static_cast<LONG>(CalcWindow.get_min_x());
            lpMMI->ptMinTrackSize.y = static_cast<LONG>(CalcWindow.get_min_y());
        }
        return FALSE;
    }
    case WM_SYSCOMMAND: {
        const auto cmd = static_cast<int>(wParam & 0xFFF0);
        if (cmd == IDM_ABOUTBOX) {
            DialogBoxParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDD_ABOUTBOX), hDlg, AboutDlgProc, 0);
            return TRUE;
        }
        return FALSE;
    }
    case WM_DISPLAYCHANGE: {
		const auto new_dpi = CalcWindow.get_window_dpi(hDlg);
        if (CalcWindow.get_dpi() != new_dpi) {
			CalcWindow.set_dpi(hDlg, new_dpi);
            CalcWindow.update_controls(hDlg);
		}
        return TRUE;
    }
    case WM_DPICHANGED: {
        const UINT newDpi = HIWORD(wParam);
        if (newDpi) {
			CalcWindow.set_dpi(hDlg, newDpi);
            CalcWindow.update_controls(hDlg);
            return TRUE;
        }
		return FALSE;
    }
    case WM_INITDIALOG: {
        CalcWindow.initialize(hDlg);
        return FALSE;
    }
    case WM_CLOSE: {
        CalcWindow.close(hDlg);
        return TRUE;
    }
    }
    return FALSE;
}

// ---------- WinMain ----------
int WINAPI WinMain(const HINSTANCE hInstance, [[maybe_unused]] const HINSTANCE hPrev, [[maybe_unused]] const LPSTR lpCmdLine, const int) { // OK

    // TODO: move this code to the core.
    /* Applications that generate floating point underflow in vector registers can benefit from setting the flush-to-zero mode rather than generating subnormal numbers in case of underflow:*/
    /* It is strongly recommended to set the flush-to-zero mode unless you have special reasons to use subnormal numbers. You may, in addition, set the denormals-are-zero mode if vector regsiters are available:*/
    // Set flush-to-zero and denormals-are-zero mode (SSE2):
    _mm_setcsr(_mm_getcsr() | 0x8040);

    // TODO: move this code to the core.
#if !defined(CALC_USE_ERROR_TOKEN) && !defined(CALC_USING_STATIC_VECTOR)
    IssueManager::speedup();
#endif

    INITCOMMONCONTROLSEX InitCtrls [[indeterminate]];
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    const auto ret = DialogBoxParamA(hInstance, MAKEINTRESOURCEA(IDD_CALC_DIALOG), nullptr, CalcDialogProc, 0);
    if (ret == -1) {
        MessageBoxA(nullptr, "Failed to create main dialog.", "Startup Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    return 0;
}

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#else

// add other platforms here.

#endif // _WIN32
