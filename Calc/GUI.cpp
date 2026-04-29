/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */
 
// GUI.cpp : Defines the user iteraction behaviors for the application.
//

#include "pch.hpp"

#include "lexer.hpp"
#include "parser.hpp"
#include "issue_manager.hpp"
#include "formatter.hpp"
#ifdef CALC_TESTS_ENABLED
#include "tests.hpp"
#endif

// ================= GUI ===============

#ifdef _WIN32

#ifdef CALC_DONT_USE_SUBNORMALS
#include <immintrin.h>// TODO: Move to the core!
#endif

#include <optional>

#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers

#define WIN32_LEAN_AND_MEAN

#define NOGDICAPMASKS - CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES - VK_ *
//#define NOWINMESSAGES - WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES - WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS - SM_ *
//#define NOMENUS - MF_ *
#define NOICONS - IDI_ *
#define NOKEYSTATES - MK_ *
#define NOSYSCOMMANDS - SC_ *
#define NORASTEROPS - Binary and Tertiary raster ops
//#define NOSHOWWINDOW - SW_ *
#define OEMRESOURCE - OEM Resource values
#define NOATOM - Atom Manager routines
#define NOCLIPBOARD - Clipboard routines
#define NOCOLOR - Screen colors
//#define NOCTLMGR - Control and Dialog routines
#define NODRAWTEXT - DrawText() and DT_ *
//#define NOGDI - All GDI defines and routines
#define NOKERNEL - All KERNEL defines and routines
//#define NOUSER - All USER defines and routines
#define NONLS - All NLS defines and routines
#define NOMB - MB_ * and MessageBox()
#define NOMEMMGR - GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE - typedef METAFILEPICT
#define NOMINMAX - Macros min(a, b) and max(a, b)
//#define NOMSG - typedef MSG and associated routines
#define NOOPENFILE - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL - SB_ * and scrolling routines
#define NOSERVICE - All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND - Sound driver routines
#define NOTEXTMETRIC - typedef TEXTMETRIC and associated routines
#define NOWH - SetWindowsHook and WH_ *
//#define NOWINOFFSETS - GWL_*, GCL_*, associated routines
#define NOCOMM - COMM driver routines
#define NOKANJI - Kanji support stuff.
#define NOHELP - Help engine interface.
#define NOPROFILER - Profiler interface.
//#define NODEFERWINDOWPOS - DeferWindowPos routines
#define NOMCX - Modem Configuration Extensions

#include "targetver.h"

#include <windows.h>
#include <commctrl.h>
#include <imm.h> // CALC_USE_IME or ImmDisableIME
#include <shellapi.h>
#include "resource.h" // main symbols

class CalcWindowState {
    // Configuration
    static constexpr std::string_view kRegKey = "Software\\HedgehogInTheCPP\\Calc";
    struct Baseline {
#ifdef CALC_SUPPORT_DPI_CHANGES
        constexpr static UINT dpi = USER_DEFAULT_SCREEN_DPI;
        constexpr static WORD point_size = 10; // matches RC font 10 pt in resource template
#endif
        constexpr static LONG min_width = 292; // matches RC
        constexpr static LONG min_height = 357; // matches RC
        constexpr static size_t elements_count = 4; // matches RC
        
        // Win32 ANSI multiline EDIT max is 64 KiB for classic Edit control:
        constexpr static size_t input_max_data_size = 64 * 1024;
        // because the number above including C zero terminator maximum symbols are:
        constexpr static size_t input_max_symbols = input_max_data_size - 1;
    };
    // ~Configuration
public:
    constexpr static Baseline baseline;

    struct Rectangle : public RECT {
        auto get_x() const noexcept {
            return left;
        }
        auto get_y() const noexcept {
            return top;
        }
        auto get_width() const noexcept {
            return right - left;
        }
        auto get_heigth() const noexcept {
            return bottom - top;
        }
    };
    using Rectangle_ptr = Rectangle*;

private:

    class LayoutManager {
    public:
        enum class HMode : BYTE { Left, Right, Stretch };
        enum class VMode : BYTE { Top, Bottom, Stretch };

        struct Anchor {
            HMode h [[indeterminate]];
            VMode v [[indeterminate]];
        };

        struct Constraint {
            int id [[indeterminate]];

            // Margins relative to parent client rect at init time
            LONG left [[indeterminate]];
            LONG top [[indeterminate]];
            LONG right [[indeterminate]];
            LONG bottom [[indeterminate]];

            LONG width [[indeterminate]];
            LONG height [[indeterminate]];

            Anchor anchor [[indeterminate]];
        };

        void initialize(const HWND parent) {
            Rectangle client [[indeterminate]];
            GetClientRect(parent, &client);

            width = client.right;
            height = client.bottom;
        }
        void initAnchor(const HWND parent, const size_t idx, const int id, const Anchor a) {
            HWND h = GetDlgItem(parent, id);
            Rectangle wr [[indeterminate]];
            GetWindowRect(h, &wr);

            POINT p{ wr.left, wr.top };
            ScreenToClient(parent, &p);

            Rectangle cr [[indeterminate]];
            cr.left = p.x;
            cr.top = p.y;
            cr.right = p.x + wr.get_width();
            cr.bottom = p.y + wr.get_heigth();

            Constraint& c = constraints[idx];
            c.id = id;
            c.left = cr.left;
            c.top = cr.top;
            c.right = width - cr.right;
            c.bottom = height - cr.bottom;
            c.width = cr.get_width();
            c.height = cr.get_heigth();
            c.anchor = a;
        }
        void resize(const HWND parent, const int newW, const int newH) {
            width = newW;
            height = newH;

            HDWP hdwp = BeginDeferWindowPos((int)constraints.size());
            for (auto& c : constraints) {
                HWND h = GetDlgItem(parent, c.id);
                if (!h) continue;

                Rectangle r [[indeterminate]];

                // Horizontal
                switch (c.anchor.h) {
                case HMode::Left:
                    r.left = c.left;
                    r.right = c.left + c.width;
                    break;

                case HMode::Right:
                    r.right = newW - c.right;
                    r.left = r.right - c.width;
                    break;

                case HMode::Stretch:
                    r.left = c.left;
                    r.right = newW - c.right;
                    break;
                }

                // Vertical
                switch (c.anchor.v) {
                case VMode::Top:
                    r.top = c.top;
                    r.bottom = c.top + c.height;
                    break;

                case VMode::Bottom:
                    r.bottom = newH - c.bottom;
                    r.top = r.bottom - c.height;
                    break;

                case VMode::Stretch:
                    r.top = c.top;
                    r.bottom = newH - c.bottom;
                    break;
                }

                hdwp = DeferWindowPos(
                    hdwp,
                    h,
                    nullptr,
                    r.left,
                    r.top,
                    r.get_width(),
                    r.get_heigth(),
                    SWP_NOZORDER | SWP_NOACTIVATE
                );
            }
            EndDeferWindowPos(hdwp);
        }

        LONG min_width [[indeterminate]];
        LONG min_height [[indeterminate]];

    private:
        LONG width [[indeterminate]];
        LONG height [[indeterminate]];

        std::array<Constraint, baseline.elements_count> constraints [[indeterminate]];
    };

    LayoutManager layout [[indeterminate]];

#ifdef CALC_SUPPORT_DPI_CHANGES
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    class Font {
    public:
        Font() noexcept {
            font = nullptr;
        }
        [[nodiscard]] Font(const WORD dpi) noexcept {
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
                cleanup();
                font = other.font;
                other.font = nullptr;
            }
            return *this;
        }
        ~Font() {
            cleanup();
        }
        auto get() const {
            return font;
        }
    private:
        void cleanup() const {
            if (font) {
                DeleteObject(font);
            }
        }
        HFONT font;
    };
    Font font;
#endif
    UINT dpi;
#endif

    auto get_min_width() const {
        return layout.min_width;
    }
    auto get_min_heigth() const {
        return layout.min_height;
    }
    auto get_min_x() const {
        return get_min_width();
    }
    auto get_min_y() const {
        return get_min_heigth();
    }

public:
    CalcWindowState() noexcept = default;
    CalcWindowState(const CalcWindowState&) = delete;
    CalcWindowState(CalcWindowState&&) = delete;
    ~CalcWindowState() = default;
    void close(const HWND hWnd) const {
        save_window_placement(hWnd);
        EndDialog(hWnd, 0);
    }
    void init(const HWND hWnd) {
        load_window_placement(hWnd);

        add_about_menu_to_system_menu(hWnd);
        set_window_icons(hWnd);

#ifdef CALC_TESTS_ENABLED
        SetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, calc_tests().c_str());
#endif
        const auto hInput = GetDlgItem(hWnd, IDC_EDIT_INPUT);

        SendMessageA(hInput, EM_LIMITTEXT, static_cast<WPARAM>(baseline.input_max_symbols), 0);

		// set focus to input edit control
        SetFocus(hInput);
    }
    void resize(HWND dlg, WORD w, WORD h) {
        layout.resize(dlg, w, h);
    }
    void get_minmaxinfo(const HWND hDlg, const LPMINMAXINFO lpMMI) const {
        // Convert minimum client area size to window (outer) size so the user can't resize window
        // smaller than the intended client area. WM_GETMINMAXINFO expects window dimensions.
        Rectangle requiredClient{ 0, 0, get_min_x(), get_min_y() };

        // Retrieve window styles to adjust for non-client area.
        const auto style = static_cast<DWORD>(GetWindowLongPtrA(hDlg, GWL_STYLE));
        const auto exStyle = static_cast<DWORD>(GetWindowLongPtrA(hDlg, GWL_EXSTYLE));

        // AdjustWindowRectEx will expand the rectangle so that the resulting outer window
        // will have the requested client size.
        if (AdjustWindowRectEx(&requiredClient, style, FALSE, exStyle)) {
            lpMMI->ptMinTrackSize.x = requiredClient.get_width();
            lpMMI->ptMinTrackSize.y = requiredClient.get_heigth();
        }
    }
#ifdef CALC_SUPPORT_DPI_CHANGES
    void set_dpi(const HWND dlg, const WORD new_dpi, Rectangle_ptr new_rect) {
        if (dpi != new_dpi) {
            // Rescale main window to suggestion and move the window to the position
            MoveWindow(dlg, new_rect->get_x(), new_rect->get_y(), new_rect->get_width(), new_rect->get_heigth(), TRUE);
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
            // Recreate font for new DPI (move-assign safely)
            font = Font(new_dpi);
            SendMessageA(dlg, WM_SETFONT, reinterpret_cast<WPARAM>(font.get()), MAKELPARAM(TRUE, 0));

            // Compute scale factor relative to current dpi
            const auto factor = static_cast<float>(new_dpi) / static_cast<float>(dpi);

            // Rescale minimum sizes
            min_width = std::lroundf(static_cast<float>(baseline.min_width) * factor);
            min_heigth = std::lroundf(static_cast<float>(baseline.min_heigth) * factor);

            // Rescale current sizes (width/heigth are client-area sizes)
            width = std::lroundf(static_cast<float>(width) * factor);
            heigth = std::lroundf(static_cast<float>(heigth) * factor);

            for (auto& a : anchors) {
                a.rc.left = std::lroundf(a.rc.left * factor);
                a.rc.top = std::lroundf(a.rc.top * factor);
                a.rc.right = std::lroundf(a.rc.right * factor);
                a.rc.bottom = std::lroundf(a.rc.bottom * factor);
                // move the control
                const auto hCtrl = GetDlgItem(dlg, a.id);
                SendMessageA(hCtrl, WM_SETFONT, reinterpret_cast<WPARAM>(font.get()), MAKELPARAM(TRUE, 0));
                MoveWindow(hCtrl, a.get_x(), a.get_y(), a.get_width(), a.get_heigth(), TRUE);
            }
#endif
            // Store new DPI
            dpi = new_dpi;
        }
    }
    static UINT get_window_dpi(const HWND hWnd) {
#ifdef CALC_SUPPORT_PER_WINDOW_DPI
		// Preferred: use per-window DPI if available.
        return GetDpiForWindow(hWnd);
#else
        // Try GetDpiForWindow (Windows 10+). Dynamically resolve to keep compatibility.
        const auto user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            using GetDpiForWindow_t = int(WINAPI*)(HWND);
            const auto fn = reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(user32, "GetDpiForWindow"));
            if (fn) {
                return fn(hWnd);
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
        return baseline.dpi;
#endif
    }
#endif
private:
#ifdef CALC_SUPPORT_MONITOR_API
    static void center_window_on_monitor(const HWND hWnd, const HMONITOR hMon) {
        Rectangle wr [[indeterminate]];
        if (GetWindowRect(hWnd, &wr)) {

            MONITORINFO mi [[indeterminate]]; mi.cbSize = sizeof(mi);
            if (!GetMonitorInfoA(hMon, &mi)) {
                SystemParametersInfoW(SPI_GETWORKAREA, 0, &mi.rcWork, 0);
            }
            const auto x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - wr.get_width()) / 2;
            const auto y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - wr.get_heigth()) / 2;
            SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        }
        else {
            SetWindowPos(hWnd, nullptr, 100, 100, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
        }
    }
#endif
    static void RegWriteDword(const HKEY root, const std::string_view subkey, const std::string_view name, const DWORD value) {
        HKEY hKey [[indeterminate]];
        RegCreateKeyExA(root, subkey.data(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
        RegSetValueExA(hKey, name.data(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
        RegCloseKey(hKey);
    }
    static std::optional<DWORD> RegReadDword(const HKEY root, const std::string_view subkey, const std::string_view name) {
        HKEY hKey [[indeterminate]];
        if (RegOpenKeyExA(root, subkey.data(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            return std::nullopt;
        }
        DWORD out [[indeterminate]]; DWORD outSize = sizeof(out); DWORD type [[indeterminate]];
        if (RegQueryValueExA(hKey, name.data(), nullptr, &type, reinterpret_cast<LPBYTE>(&out), &outSize) != ERROR_SUCCESS || type != REG_DWORD) {
            RegCloseKey(hKey);
            return std::nullopt;
        }
        RegCloseKey(hKey);
        return out;
    }
    void load_window_placement(const HWND hWnd) {
        layout.initialize(hWnd);
        layout.initAnchor(hWnd, 0, IDC_EDIT_INPUT, LayoutManager::Anchor{ LayoutManager::HMode::Stretch, LayoutManager::VMode::Stretch });
        layout.initAnchor(hWnd, 1, IDC_EDIT_RESULT, LayoutManager::Anchor{ LayoutManager::HMode::Stretch, LayoutManager::VMode::Bottom });
        layout.initAnchor(hWnd, 2, IDC_EDIT_MESSAGE, LayoutManager::Anchor{ LayoutManager::HMode::Stretch, LayoutManager::VMode::Bottom });
        layout.initAnchor(hWnd, 3, IDC_BUTTON_CALC, LayoutManager::Anchor{ LayoutManager::HMode::Right,   LayoutManager::VMode::Bottom });
        static_assert(          4 == baseline.elements_count);

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
                return std::lroundf(static_cast<float>(*val) * static_cast<float>(currentDpi) / static_cast<float>(baseline.dpi));
            }
            else {
                return *val;
            }
            };

        dpi = currentDpi;
#else
        const auto to_physical = [&](const std::optional<DWORD>& val) -> LONG {
            return *val;
			};
#endif
        layout.min_width = to_physical(baseline.min_width);
        layout.min_height = to_physical(baseline.min_height);

        wp.rcNormalPosition.left = sLeft ? to_physical(sLeft) : 100;
        wp.rcNormalPosition.top = sTop ? to_physical(sTop) : 100;
        wp.rcNormalPosition.right = sRight ? to_physical(sRight) : static_cast<LONG>(get_min_width() + 100);
        wp.rcNormalPosition.bottom = sBottom ? to_physical(sBottom) : static_cast<LONG>(get_min_heigth() + 100);

#ifdef CALC_SUPPORT_MONITOR_API
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
#endif

        SetWindowPlacement(hWnd, &wp);
    }
    void save_window_placement(const HWND hWnd) const {
        WINDOWPLACEMENT wp [[indeterminate]]; wp.length = sizeof(wp);
        if (GetWindowPlacement(hWnd, &wp)) {
#ifdef CALC_SUPPORT_DPI_CHANGES
            // Persist placement in baseline.dpi so saved placement survives DPI changes.
            const auto to_logical = [&](LONG phys) -> DWORD {
                return static_cast<DWORD>(std::lroundf(static_cast<float>(phys) * static_cast<float>(baseline.dpi) / static_cast<float>(dpi)));
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
        AppendMenuA(hSys, MF_SEPARATOR, 0, nullptr);
        AppendMenuA(hSys, MF_STRING, IDM_ABOUTBOX, "&About...");
    }
    // ------------------------------------------------------------------
    // Utility: set dialog icon (small & big) from IDR_MAINFRAME
    // ------------------------------------------------------------------
    static void set_window_icons(const HWND hDlg) { // OK
        // Set icons (large and small)
        const auto icon = LoadIconA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDR_MAINFRAME));
        SendMessageA(hDlg, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
        SendMessageA(hDlg, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
    }
};

using CalcWindowState_ptr = CalcWindowState*;

static CalcWindowState_ptr GetState(const HWND hDlg) {
    return reinterpret_cast<CalcWindowState_ptr>(GetWindowLongPtrA(hDlg, GWLP_USERDATA));
}

// ------------------------------------------------------------------
// Calc!
// ------------------------------------------------------------------
static void perform_calculation(const HWND hDlg) { // OK
    std::array<char, CalcWindowState::baseline.input_max_data_size> input [[indeterminate]];

    if (const auto copied = GetDlgItemTextA(hDlg, IDC_EDIT_INPUT, input.data(), static_cast<int>(input.size()))) {
        const std::string_view sv{ input.data(), copied };

        Lexer l(sv);
        Parser p(l);
        const auto result = p.parse();

#if defined(CALC_USE_ERROR_TOKEN)
        const bool hasErr = xxx;
        if (hasErr) {
            yyy;
        }
#else
        const auto hasErr = IssueManager::has_errors();

        if (hasErr) {
            SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, "");
            Formatter::Summary summary [[indeterminate]];
            Formatter::create_summary(summary);
            IssueManager::clear();
            SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, summary.data());
        }
#endif
        else {
            Formatter::Result result_text [[indeterminate]];
            Formatter::format(result, result_text);
            SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, result_text.data());
            SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, "");
        }
    }
}

// ------------------------------------------------------------------
// About dialog proc (resource-based).
// ------------------------------------------------------------------
static INT_PTR CALLBACK AboutDlgProc(const HWND hDlg, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) { // OK
    switch (uMsg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, 0);
            return TRUE;
        }
        return FALSE;
#ifdef CALC_SUPPORT_LINK_WINDOW
    case WM_NOTIFY: {
        const auto nm = reinterpret_cast<LPNMHDR>(lParam);
        if (nm->idFrom == IDC_LINK_HOMEPAGE && nm->code == NM_CLICK) {
            const auto link = reinterpret_cast<NMLINK*>(lParam);
            ShellExecuteW(nullptr, L"open", link->item.szUrl, nullptr, nullptr, SW_SHOWNORMAL);
            return TRUE;
        }
        return FALSE;
    }
#endif
    }
    return FALSE;
}

// ------------------------------------------------------------------
// Calc dialog proc (resource-based).
// ------------------------------------------------------------------
static INT_PTR CALLBACK CalcDialogProc(const HWND hDlg, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) { // OK
    switch (uMsg) {
    case WM_COMMAND: {
        if (LOWORD(wParam) == IDC_BUTTON_CALC && HIWORD(wParam) == BN_CLICKED) {
            perform_calculation(hDlg);
            return TRUE;
        }
        return FALSE;
    }
    case WM_SYSCOMMAND: {
        if ((wParam & 0xFFF0) == IDM_ABOUTBOX) {
            DialogBoxParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDD_ABOUTBOX), hDlg, AboutDlgProc, 0);
            return TRUE;
        }
        return FALSE;
    }
    case WM_GETMINMAXINFO: {
        GetState(hDlg)->get_minmaxinfo(hDlg, reinterpret_cast<LPMINMAXINFO>(lParam));
        return TRUE;
    }
    case WM_SIZE: {
        GetState(hDlg)->resize(hDlg, LOWORD(lParam), HIWORD(lParam));
        return TRUE;
    }
#ifdef CALC_SUPPORT_DPI_CHANGES
    case WM_DPICHANGED: {
        GetState(hDlg)->set_dpi(hDlg, HIWORD(wParam), reinterpret_cast<const CalcWindowState::Rectangle_ptr>(lParam));
        return TRUE;
    }
#endif
    case WM_INITDIALOG: {
        CalcWindowState_ptr state = new CalcWindowState();
        SetWindowLongPtrA(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(state));

        state->init(hDlg);
        return FALSE;
    }
    case WM_CLOSE: {
        GetState(hDlg)->close(hDlg);

        delete reinterpret_cast<CalcWindowState_ptr>(SetWindowLongPtrA(hDlg, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr)));
        return TRUE;
    }
    }
    return FALSE;
}
// ------------------------------------------------------------------
// WinMain - entrypoint
// ------------------------------------------------------------------
int WINAPI WinMain(const HINSTANCE hInstance, const HINSTANCE /*hPrev*/ , const LPSTR /*lpCmdLine*/, const int) { // OK

#ifdef CALC_DONT_USE_SUBNORMALS
    // TODO: move this code to the core.
    /* Applications that generate floating point underflow in vector registers can benefit from setting the flush-to-zero mode rather than generating subnormal numbers in case of underflow:*/
    /* It is strongly recommended to set the flush-to-zero mode unless you have special reasons to use subnormal numbers. You may, in addition, set the denormals-are-zero mode if vector regsiters are available:*/
    _mm_setcsr(_mm_getcsr() | _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);
#endif

#ifdef CALC_SUPPORT_IME
    ImmDisableIME(0);
#endif

#ifdef CALC_SUPPORT_DPI_CHANGES
#ifdef CALC_SUPPORT_PER_WINDOW_DPI
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
#else
    SetProcessDPIAware();
#endif
#endif

    DialogBoxParamA(hInstance, MAKEINTRESOURCEA(IDD_CALC_DIALOG), nullptr, CalcDialogProc, 0);

    return EXIT_SUCCESS;
}

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "user32.lib")

#else

// add other platforms here.

#endif // _WIN32
