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

#include <optional>

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // Ensure that we using min/max from std algorithms.

#include "targetver.h"

#include <immintrin.h>// TODO: Move to the core!

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <imm.h>
#include <shellapi.h>
#include "resource.h"        // main symbols

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "imm32.lib")

class CalcWindowState {
    // ---------- configuration ----------
    static constexpr std::string_view kRegKey = "Software\\HedgehogInTheCPP\\Calc";
    struct Baseline {
#ifdef CALC_SUPPORT_DPI_CHANGES
        constexpr static int dpi = USER_DEFAULT_SCREEN_DPI;
        constexpr static int point_size = 10; // matches RC font 10 pt in resource template
#endif
        constexpr static int min_width = 292;
        constexpr static int min_heigth = 357;
    };
    constexpr static Baseline baseline;

#ifdef CALC_SUPPORT_DPI_CHANGES
    class Font {
    public:
        Font() noexcept {
            font = nullptr;
        }
        [[nodiscard]] Font(const UINT dpi) noexcept {
            font = CreateFontA(-MulDiv(baseline.point_size, dpi, 72), // negative height for character height
                0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
        }
        // non-copyable
        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;
        // movable ownership
        Font(Font&& other) noexcept : font(other.font) { other.font = nullptr; }
        Font& operator=(Font&& other) noexcept {
            if (this != &other) {
                if (font) DeleteObject(font);
                font = other.font;
                other.font = nullptr;
            }
            return *this;
        }
        ~Font() {
            if (font) {
                DeleteObject(font);
            }
        }
        auto get_font() const {
            return font;
        }
    private:
        HFONT font;
    };
    Font font;

    int dpi;
#endif

    int min_width;
    int min_heigth;
    int width [[indeterminate]];
    int heigth [[indeterminate]];

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

    struct Anchor {
        int id;
        RECT rc;
        const bool left, top, right, bottom;
        auto get_x() const {
            return rc.left;
        }
        auto get_y() const {
            return rc.top;
        }
        auto get_width() const {
            return rc.right - rc.left;
        }
        auto get_heigth() const {
            return rc.bottom - rc.top;
        }
    };
    std::vector<Anchor> anchors;

public:
	CalcWindowState() noexcept :
#ifdef CALC_SUPPORT_DPI_CHANGES
        dpi(baseline.dpi),
#endif
		min_width(baseline.min_width),
		min_heigth(baseline.min_heigth),
		width(baseline.min_width),
		heigth(baseline.min_heigth)
    {
    }
    CalcWindowState(const CalcWindowState&) = delete;
    CalcWindowState(CalcWindowState&&) = delete;
    ~CalcWindowState() = default;
    void close(const HWND hWnd) const {
        save_window_placement(hWnd);
        EndDialog(hWnd, 0);
    }
    void initialize(const HWND hWnd) {

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
    void resize(const HWND dlg, const INT clientW, const INT clientH) {
        if (width != clientW || heigth != clientH) {
            // compute delta between previous client size and new client size
            const INT deltaW = width - clientW;
            const INT deltaH = heigth - clientH;

            // store new client size
            width = clientW;
            heigth = clientH;

            for (auto& a : anchors) {
                // if anchored to bottom, move bottom edge by deltaH
                a.rc.bottom = a.bottom ? (a.rc.bottom - deltaH) : a.rc.bottom;
                // if not anchored to left, shift left coordinate by deltaW
                a.rc.left = a.left ? a.rc.left : (a.rc.left - deltaW);
                // if not anchored to top, shift top coordinate by deltaH
                a.rc.top = a.top ? a.rc.top : (a.rc.top - deltaH);
                // if anchored to right, move right edge by deltaW
                a.rc.right = a.right ? (a.rc.right - deltaW) : a.rc.right;
                // move the control
                const auto hCtrl = GetDlgItem(dlg, a.id);
                if (hCtrl) {
                    MoveWindow(hCtrl, a.get_x(), a.get_y(), a.get_width(), a.get_heigth(), TRUE);
                }
            }
        }
    }
    void get_minmaxinfo(const HWND hDlg, const LPMINMAXINFO lpMMI) const {
        if (lpMMI) {
            // Convert minimum client area size to window (outer) size so the user can't resize window
            // smaller than the intended client area. WM_GETMINMAXINFO expects window dimensions.
            RECT requiredClient{ 0, 0, get_min_x(), get_min_y() };

            // Retrieve window styles to adjust for non-client area.
            const auto stylePtr = GetWindowLongPtrA(hDlg, GWL_STYLE);
            const auto exStylePtr = GetWindowLongPtrA(hDlg, GWL_EXSTYLE);
            const auto style = static_cast<DWORD>(stylePtr);
            const auto exStyle = static_cast<DWORD>(exStylePtr);

            // AdjustWindowRectEx will expand the rectangle so that the resulting outer window
            // will have the requested client size.
            if (AdjustWindowRectEx(&requiredClient, style, FALSE, exStyle)) {
                lpMMI->ptMinTrackSize.x = requiredClient.right - requiredClient.left;
                lpMMI->ptMinTrackSize.y = requiredClient.bottom - requiredClient.top;
            }
        };
    }
#ifdef CALC_SUPPORT_DPI_CHANGES
    void set_dpi(const HWND dlg, const INT new_dpi) {
        if (dpi != new_dpi) {
            // Compute scale factor relative to current dpi
            const auto factor = static_cast<double>(new_dpi) / static_cast<double>(dpi);

            dpi = new_dpi;

            // Rescale minimum sizes
            min_width = std::lround(static_cast<double>(baseline.min_width) * factor);
            min_heigth = std::lround(static_cast<double>(baseline.min_heigth) * factor);

            // Rescale current sizes (width/heigth are client-area sizes)
            width = std::lround(static_cast<double>(width) * factor);
            heigth = std::lround(static_cast<double>(heigth) * factor);

            // Recreate font for new DPI (move-assign safely)
            font = Font(dpi);

            // Rescale dialog window
            SendMessageA(dlg, WM_SETFONT, reinterpret_cast<WPARAM>(font.get_font()), MAKELPARAM(TRUE, 0));
            RECT wr [[indeterminate]];
            if (GetWindowRect(dlg, &wr)) {
                MoveWindow(dlg, wr.left, wr.top,
                    std::lround(static_cast<double>(wr.right - wr.left) * factor),
                    std::lround(static_cast<double>(wr.bottom - wr.top) * factor),
                    TRUE);
            }

            // Rescale anchors
            for (auto& a : anchors) {
                a.rc.left = std::lround(a.rc.left * factor);
                a.rc.top = std::lround(a.rc.top * factor);
                a.rc.right = std::lround(a.rc.right * factor);
                a.rc.bottom = std::lround(a.rc.bottom * factor);
                // move the control
                const auto hCtrl = GetDlgItem(dlg, a.id);
                if (hCtrl) {
                    SendMessageA(hCtrl, WM_SETFONT, reinterpret_cast<WPARAM>(font.get_font()), MAKELPARAM(TRUE, 0));
                    MoveWindow(hCtrl, a.get_x(), a.get_y(), a.get_width(), a.get_heigth(), TRUE);
                }
            }
        }
    }
    static int get_window_dpi(const HWND hWnd) {
#if(_WIN32_WINNT >= 0x0605)
		// Preferred: use per-window DPI if available.
        const auto dpi = GetDpiForWindow(hWnd);
        if (dpi > 0) return dpi;
#else
        // Try GetDpiForWindow (Windows 10+). Dynamically resolve to keep compatibility.
        const auto user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            using GetDpiForWindow_t = int(WINAPI*)(HWND);
            auto fn = reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(user32, "GetDpiForWindow"));
            if (fn) {
                const auto dpi = fn(hWnd);
                if (dpi > 0) return dpi;
            }
        }
        // Fallback: get device caps from nearest monitor / device context.
        const auto hdc = GetDC(hWnd);
        if (hdc) {
            const auto dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
            ReleaseDC(hWnd, hdc);
            if (dpiY > 0) return dpiY;
        }
        // Last fallback, query primary screen
        const auto screen = GetDC(nullptr);
        if (screen) {
            const auto dpiY = GetDeviceCaps(screen, LOGPIXELSY);
            ReleaseDC(nullptr, screen);
            if (dpiY > 0) return dpiY;
        }
#endif
        return baseline.dpi;
    }
#endif
private:
    static void center_window_on_monitor(const HWND hWnd, const HMONITOR hMon) {
        RECT wr [[indeterminate]];
        if (GetWindowRect(hWnd, &wr)) {

            MONITORINFO mi [[indeterminate]]; mi.cbSize = sizeof(mi);
            if (!GetMonitorInfo(hMon, &mi)) {
                SystemParametersInfoW(SPI_GETWORKAREA, 0, &mi.rcWork, 0);
            }
            const auto w = wr.right - wr.left, h = wr.bottom - wr.top;
            const auto x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - w) / 2;
            const auto y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - h) / 2;
            SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        }
        else {
            SetWindowPos(hWnd, nullptr, 100, 100, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        }
    }
    __declspec(noinline) static void RegWriteDword(const HKEY root, const std::string_view subkey, const std::string_view name, const DWORD value) {
        HKEY hKey [[indeterminate]];
        if (RegCreateKeyExA(root, subkey.data(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
            RegSetValueExA(hKey, name.data(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
            RegCloseKey(hKey);
        }
    }
    __declspec(noinline) static std::optional<DWORD> RegReadDword(const HKEY root, const std::string_view subkey, const std::string_view name) {
        HKEY hKey [[indeterminate]];
        if (RegOpenKeyExA(root, subkey.data(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            return std::nullopt;
        }
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
    void load_window_placement(const HWND hWnd) {
        // IMPORTANT: use client rect here. WM_SIZE provides client area sizes.
        RECT cr [[indeterminate]];
        if (GetClientRect(hWnd, &cr)) {
            width = (cr.right - cr.left);
            heigth = (cr.bottom - cr.top);
        }

        const auto add = [&](const int id, const bool left, const bool top, const bool right, const bool bottom) {
            const auto h = GetDlgItem(hWnd, id);
            if (!h) return;

            RECT wr [[indeterminate]]; if (!GetWindowRect(h, &wr)) return;
            POINT p{ wr.left, wr.top }; ScreenToClient(hWnd, &p);
            RECT rcClient{ p.x, p.y, p.x + (wr.right - wr.left), p.y + (wr.bottom - wr.top) };
            anchors.emplace_back(Anchor{ id, rcClient, left, top, right, bottom });
            };

        add(IDC_EDIT_INPUT, true, true, true, true);    // flexible input
        add(IDC_EDIT_RESULT, true, false, true, true);  // bottom anchored
        add(IDC_EDIT_MESSAGE, true, false, true, true); // bottom anchored
        add(IDC_BUTTON_CALC, false, false, true, true); // button bottom-right

        // Read stored placement. New behaviour: stored coords are logical (96 DPI base).
        // If a savedDpi key exists we assume values were written by the new code (logical),
        // otherwise treat registry values as legacy raw pixels.
        const auto sShow = RegReadDword(HKEY_CURRENT_USER, kRegKey, "showCmd");
        const auto sFlags = RegReadDword(HKEY_CURRENT_USER, kRegKey, "flags");
        const auto sLeft = RegReadDword(HKEY_CURRENT_USER, kRegKey, "left");
        const auto sTop = RegReadDword(HKEY_CURRENT_USER, kRegKey, "top");
        const auto sRight = RegReadDword(HKEY_CURRENT_USER, kRegKey, "right");
        const auto sBottom = RegReadDword(HKEY_CURRENT_USER, kRegKey, "bottom");
#ifdef CALC_SUPPORT_DPI_CHANGES
        const auto sSavedDpi = RegReadDword(HKEY_CURRENT_USER, kRegKey, "savedDpi");
#endif

        WINDOWPLACEMENT wp [[indeterminate]]; wp.length = sizeof(wp);
        wp.showCmd = static_cast<int>(sShow ? *sShow : SW_SHOWNORMAL);
        wp.flags = static_cast<UINT>(sFlags ? *sFlags : 0u);

#ifdef CALC_SUPPORT_DPI_CHANGES
        const auto currentDpi = get_window_dpi(hWnd);

        const auto to_physical = [&](const std::optional<DWORD>& val) -> LONG {
            // If savedDpi exists we assume stored value is logical (96-based), convert to current DPI.
            // If savedDpi absent: legacy raw pixels are left unchanged.
            if (sSavedDpi) {
                return std::lround(static_cast<double>(*val) * static_cast<double>(currentDpi) / static_cast<double>(baseline.dpi));
            }
            else {
                return *val;
            }
            };

		min_width = to_physical(baseline.min_width);
		min_heigth = to_physical(baseline.min_heigth);

        dpi = currentDpi;
#else
        const auto to_physical = [&](const std::optional<DWORD>& val) -> LONG {
            return *val;
			};
#endif

        wp.rcNormalPosition.left = sLeft ? to_physical(sLeft) : 100;
        wp.rcNormalPosition.top = sTop ? to_physical(sTop) : 100;
        wp.rcNormalPosition.right = sRight ? to_physical(sRight) : static_cast<LONG>(get_min_width() + 100);
        wp.rcNormalPosition.bottom = sBottom ? to_physical(sBottom) : static_cast<LONG>(get_min_heigth() + 100);

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
#ifdef CALC_SUPPORT_DPI_CHANGES
            // Persist placement in baseline.dpi so saved placement survives DPI changes.
            const auto to_logical = [&](LONG phys) -> DWORD {
                return static_cast<DWORD>(std::lround(static_cast<double>(phys) * static_cast<double>(baseline.dpi) / static_cast<double>(dpi)));
                };
#else
            const auto to_logical = [&](LONG phys) -> DWORD {
                return static_cast<DWORD>(phys);
				};
#endif

            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "showCmd", static_cast<DWORD>(wp.showCmd));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "flags", static_cast<DWORD>(wp.flags));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "left", to_logical(wp.rcNormalPosition.left));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "top", to_logical(wp.rcNormalPosition.top));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "right", to_logical(wp.rcNormalPosition.right));
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "bottom", to_logical(wp.rcNormalPosition.bottom));
#ifdef CALC_SUPPORT_DPI_CHANGES
            // Write savedDpi marker so loader knows values are logical units (new format).
            RegWriteDword(HKEY_CURRENT_USER, kRegKey, "savedDpi", static_cast<DWORD>(dpi));
#endif
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
static CalcWindowState CalcWindow;

// ------------------------------------------------------------------
// Calc!
// ------------------------------------------------------------------
static void perform_calculation(const HWND hDlg) {
    // ANSI multiline EDIT max is 64 KiB for classic Edit control
    std::array<char, 64 * 1024> input [[indeterminate]]; // 65535 for symbols and 1 for null C string API terminator

    const auto copied = GetDlgItemTextA(hDlg, IDC_EDIT_INPUT, input.data(), static_cast<int>(input.size()));
    if (!copied) {
        SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, "");
        SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, "");
    }
    else {
        const std::string_view sv{ input.data(), copied };

        Lexer l(sv);
        Parser p(l);
        const auto result = p.parse();

#if defined(CALC_USE_ERROR_TOKEN)
        const bool hasErr = 
#else
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
        if (LOWORD(wParam) == IDC_BUTTON_CALC && HIWORD(wParam) == BN_CLICKED) {
            perform_calculation(hDlg);
            return TRUE;
        }
        return FALSE;
    }
    case WM_GETMINMAXINFO: {
		CalcWindow.get_minmaxinfo(hDlg, reinterpret_cast<LPMINMAXINFO>(lParam));
        return TRUE;
    }
    case WM_SIZE: {
        CalcWindow.resize(hDlg, LOWORD(lParam), HIWORD(lParam));
        return TRUE;
    }
    case WM_SYSCOMMAND: {
        if ((wParam & 0xFFF0) == IDM_ABOUTBOX) {
            DialogBoxParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDD_ABOUTBOX), hDlg, AboutDlgProc, 0);
            return TRUE;
        }
        return FALSE;
    }
#ifdef CALC_SUPPORT_DPI_CHANGES
    case WM_DPICHANGED: {
        CalcWindow.set_dpi(hDlg, HIWORD(wParam));
        return TRUE;
    }
#endif
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

#ifdef CALC_SUPPORT_DPI_CHANGES
    SetProcessDPIAware();
#endif

    INITCOMMONCONTROLSEX InitCtrls [[indeterminate]];
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    const auto ret = DialogBoxParamA(hInstance, MAKEINTRESOURCEA(IDD_CALC_DIALOG), nullptr, CalcDialogProc, 0);
    if (ret == -1) {
        MessageBoxA(nullptr, "Failed to create main dialog", "Startup Error", MB_OK | MB_ICONERROR);
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
