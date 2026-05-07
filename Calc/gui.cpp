/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */
 
// gui.cpp : Defines the user iteraction behaviors for the application.
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

#ifdef CALC_DONT_USE_SUBNORMALS
#include <immintrin.h>// TODO: Move to the core!
#endif

#include "gui.hpp"

#ifdef _WIN32

std::array<char, CalcConfiguration::input_max_data_size> input [[indeterminate]];

class CalcWindow {
public:
    CalcWindow() noexcept = default;
    CalcWindow(const CalcWindow&) = delete;
    CalcWindow(CalcWindow&&) = delete;
    ~CalcWindow() = default;
    // ------------------------------------------------------------------
    // Calc!
    // ------------------------------------------------------------------
    void perform_calculation() {
        const std::string_view equation(input.data(), static_cast<size_t>(GetWindowTextA(layout.get_constraints_handle(0), input.data(), static_cast<int>(input.size()))));

        Lexer l(equation);
        Parser p(l);
        const auto result = p.parse();

#if defined(CALC_USE_ERROR_TOKEN)
        const bool has_errors = xxx;
        if (has_errors) {
            yyy;
        }
#else
        const auto has_errors = IssueManager::has_errors();

        if (has_errors) {
            Formatter::Summary summary [[indeterminate]];
            Formatter::create_summary(summary);
            IssueManager::clear();
            SetWindowTextA(layout.get_constraints_handle(2), summary.data());
            SetWindowTextA(layout.get_constraints_handle(1), nullptr);
        }
#endif
        else {
            Formatter::Result result_text [[indeterminate]];
            Formatter::format(result, result_text);
            SetWindowTextA(layout.get_constraints_handle(1), result_text.data());
            SetWindowTextA(layout.get_constraints_handle(2), nullptr);
        }
    }
    void close(const HWND dlg) const {
        save_window_data(dlg);
        EndDialog(dlg, 0);
    }
    void init(const HWND dlg) {
        add_about_menu_to_system_menu(dlg);
        set_window_icons(dlg);
        load_window_data(dlg);
#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
        // set text limit to input edit control
        SendMessageA(layout.get_constraints_handle(0), EM_LIMITTEXT, static_cast<WPARAM>(CalcConfiguration::input_max_symbols), 0);
#endif
        SetWindowTextA(layout.get_constraints_handle(0),
#ifdef CALC_TESTS_ENABLED
            calc_tests().data()
#else
            input.data()
#endif
        );
        SendMessageA(layout.get_constraints_handle(0), EM_SETSEL, static_cast<WPARAM>(0), static_cast<LPARAM>(-1));
        SendMessageA(layout.get_constraints_handle(0), EM_SCROLLCARET, 0, 0);
    }

    inline void resize(WORD w, WORD h) {
        layout.resize(w, h);
    }
    inline void get_minmaxinfo(const LPMINMAXINFO lpMMI) const {
        lpMMI->ptMinTrackSize.x = layout.get_min_x();
        lpMMI->ptMinTrackSize.y = layout.get_min_y();
    }
#ifdef CALC_SUPPORT_DPI_CHANGES_SIGNAL
    void set_dpi(const HWND window, const WORD new_dpi, Rect_ptr new_rect) {
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
        // Reinit min sizes
        layout.init_min_sizes(window, CalcConfiguration::min_width, CalcConfiguration::min_height, new_dpi);
#endif
        // Rescale main window to suggestion and set the window position
        SetWindowPos(window, nullptr, new_rect->get_x(), new_rect->get_y(), new_rect->get_width(), new_rect->get_heigth(), SWP_NOZORDER | SWP_NOACTIVATE);

        // Store new DPI
        dpi = new_dpi;
    }
#endif

private:
    void load_window_data(const HWND hWnd) {
        // Read stored data:
        const RegRead reg(HKEY_CURRENT_USER, CalcConfiguration::reg_key);
#ifndef CALC_TESTS_ENABLED
        input[reg.read("input", reinterpret_cast<LPBYTE>(input.data()), static_cast<DWORD>(input.size()))] = '\0';
#endif
        
#ifdef CALC_SUPPORT_DPI_CHANGES
        const auto sSavedDpi = reg.read("savedDpi");
        dpi = get_window_dpi(hWnd);
        const auto to_physical = [&](const std::optional<DWORD>& val) -> LONG {
            if (sSavedDpi) {
                // If savedDpi exists we assume stored value is logical (96-based), convert to current DPI.
                return std::lroundf(static_cast<float>(*val) * static_cast<float>(dpi) / static_cast<float>(USER_DEFAULT_SCREEN_DPI));
            }
            else {
                // If savedDpi absent: legacy raw pixels are left unchanged.
                return *val;
            }
            };
#else
        const auto to_physical = [&](const std::optional<DWORD>& val) -> LONG {
            return *val;
            };
#endif
        //
        layout.init_window(hWnd);
        layout.init_min_sizes(hWnd, CalcConfiguration::min_width, CalcConfiguration::min_height
#ifdef CALC_SUPPORT_PER_WINDOW_DPI
            , dpi
#endif
        );

        layout.init_anchor(hWnd, 0, IDC_EDIT_INPUT,   Layout::Anchor{ Layout::HMode::Stretch, Layout::VMode::Stretch });
        layout.init_anchor(hWnd, 1, IDC_EDIT_RESULT,  Layout::Anchor{ Layout::HMode::Stretch, Layout::VMode::Bottom });
        layout.init_anchor(hWnd, 2, IDC_EDIT_MESSAGE, Layout::Anchor{ Layout::HMode::Stretch, Layout::VMode::Bottom });
        layout.init_anchor(hWnd, 3, IDC_BUTTON_CALC,  Layout::Anchor{ Layout::HMode::Right,   Layout::VMode::Bottom });
        static_assert(4 == CalcConfiguration::elements);

        //
        const auto flags = reg.read("flags");
        const auto show = reg.read("showCmd");
        const auto left = reg.read("left");
        const auto top = reg.read("top");
        const auto right = reg.read("right");
        const auto bottom = reg.read("bottom");
        WINDOWPLACEMENT wp [[indeterminate]]; wp.length = sizeof(wp);
        wp.flags = static_cast<UINT>(flags ? *flags : 0);
        wp.showCmd = static_cast<UINT>(show ? *show : SW_SHOWNORMAL);
        if (left && top && right && bottom) {
            // If saved values exists: use it.
            wp.rcNormalPosition.left = to_physical(left);
            wp.rcNormalPosition.top = to_physical(top);
            wp.rcNormalPosition.right = to_physical(right);
            wp.rcNormalPosition.bottom = to_physical(bottom);
        }
        else {
            // If not: set window to the default.
            const auto shift = to_physical(CalcConfiguration::default_shift_px);
            wp.rcNormalPosition.left = shift;
            wp.rcNormalPosition.top = shift;
            wp.rcNormalPosition.right = layout.get_min_width() + shift;
            wp.rcNormalPosition.bottom = layout.get_min_height() + shift;
        }

        SetWindowPlacement(hWnd, &wp);

#ifdef CALC_SUPPORT_MONITOR_API
        const auto mon = MonitorFromRect(&wp.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi [[indeterminate]]; mi.cbSize = sizeof(mi);
        GetMonitorInfoA(mon, &mi);
        const RECT& work = mi.rcWork;
        if (wp.rcNormalPosition.right <= work.left || wp.rcNormalPosition.left >= work.right || wp.rcNormalPosition.bottom <= work.top || wp.rcNormalPosition.top >= work.bottom) {
            center_window_on_monitor(hWnd, mon);
            return;
        }
#endif
    }
    void save_window_data(const HWND hWnd) const {
        const RegWrite reg(HKEY_CURRENT_USER, CalcConfiguration::reg_key);
#ifndef CALC_TESTS_ENABLED
        reg.write("input", reinterpret_cast<BYTE*>(input.data()), static_cast<DWORD>(GetWindowTextA(layout.get_constraints_handle(0), input.data(), static_cast<int>(input.size()))));
#endif
#ifdef CALC_SUPPORT_DPI_CHANGES
        reg.write("savedDpi", dpi);
        const auto to_logical = [&](LONG phys) -> DWORD {
            return static_cast<DWORD>(std::lroundf(static_cast<float>(phys) * static_cast<float>(USER_DEFAULT_SCREEN_DPI) / static_cast<float>(dpi)));
            };
#else
        const auto to_logical = [&](LONG phys) -> DWORD {
            return static_cast<DWORD>(phys);
            };
#endif

        WINDOWPLACEMENT wp [[indeterminate]]; wp.length = sizeof(wp);
        GetWindowPlacement(hWnd, &wp);
        reg.write("flags", wp.flags);
        reg.write("showCmd", wp.showCmd);
        reg.write("left", to_logical(wp.rcNormalPosition.left));
        reg.write("top", to_logical(wp.rcNormalPosition.top));
        reg.write("right", to_logical(wp.rcNormalPosition.right));
        reg.write("bottom", to_logical(wp.rcNormalPosition.bottom));
    }
    Layout layout [[indeterminate]];

#ifdef CALC_SUPPORT_DPI_CHANGES
    DWORD dpi;
#endif
};

static CalcWindow calc_window;

// ------------------------------------------------------------------
// About dialog proc (resource-based).
// ------------------------------------------------------------------
static INT_PTR CALLBACK AboutDlgProc(const HWND dlg, const UINT msg, const WPARAM wParam, const LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(dlg, 0);
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
    default: {
        return FALSE;
    }
    }
}

// ------------------------------------------------------------------
// Calc dialog proc (resource-based).
// ------------------------------------------------------------------
static INT_PTR CALLBACK CalcDialogProc(const HWND dlg, const UINT msg, const WPARAM wParam, const LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND: {
        if (LOWORD(wParam) == IDC_BUTTON_CALC && HIWORD(wParam) == BN_CLICKED) {
            calc_window.perform_calculation();
            return TRUE;
        }
        return FALSE;
    }
    case WM_SYSCOMMAND: {
        if ((wParam & 0xFFF0) == IDM_ABOUTBOX) {
            DialogBoxParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDD_ABOUTBOX), dlg, AboutDlgProc, 0);
            return TRUE;
        }
        return FALSE;
    }
    case WM_GETMINMAXINFO: {
        calc_window.get_minmaxinfo(reinterpret_cast<LPMINMAXINFO>(lParam));
        return TRUE;
    }
    case WM_SIZE: {
        calc_window.resize(LOWORD(lParam), HIWORD(lParam));
        return TRUE;
    }
#ifdef CALC_SUPPORT_PER_WINDOW_DPI
    //case WM_GETDPISCALEDSIZE: {
    //}
#endif
#ifdef CALC_SUPPORT_DPI_CHANGES_SIGNAL
    case WM_DPICHANGED: {
        calc_window.set_dpi(dlg, HIWORD(wParam), reinterpret_cast<const Rect_ptr>(lParam));
        return TRUE;
    }
#endif
    case WM_INITDIALOG: {
        calc_window.init(dlg);
        return TRUE;
    }
    case WM_CLOSE: {
        calc_window.close(dlg);
        return TRUE;
    }
    default: {
        return FALSE;
    }
    }
}
// ------------------------------------------------------------------
// WinMain - entrypoint
// ------------------------------------------------------------------
int WINAPI WinMain(const HINSTANCE hInstance, const HINSTANCE /*hPrevInstance*/, const LPSTR /*pCmdLine*/, const int /*nCmdShow*/) {

#ifdef CALC_DONT_USE_SUBNORMALS
    // TODO: move this code to the core.
    /* Applications that generate floating point underflow in vector registers can benefit from setting the flush-to-zero mode rather than generating subnormal numbers in case of underflow:*/
    /* It is strongly recommended to set the flush-to-zero mode unless you have special reasons to use subnormal numbers. You may, in addition, set the denormals-are-zero mode if vector regsiters are available:*/
    _mm_setcsr(_mm_getcsr() | _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);
#endif

    // Disable IME completely because Calc use only ANSI input in GUI
    ImmDisableIME(0);

    
#ifdef CALC_SUPPORT_DPI_CHANGES
    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setprocessdpiawarenesscontext
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    //SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#else
    //SetProcessDPIAware();
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

#include <wx/wx.h>
#include <Calc.h>

class MyApp : public wxApp
{
public:
    bool OnInit() override;

};

// This defines the equivalent of main() for the current platform.
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
#ifdef CALC_DONT_USE_SUBNORMALS
    // TODO: move this code to the core.
    /* Applications that generate floating point underflow in vector registers can benefit from setting the flush-to-zero mode rather than generating subnormal numbers in case of underflow:*/
    /* It is strongly recommended to set the flush-to-zero mode unless you have special reasons to use subnormal numbers. You may, in addition, set the denormals-are-zero mode if vector regsiters are available:*/
    _mm_setcsr(_mm_getcsr() | _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);
#endif

    CalcFrame* frame = new CalcFrame(nullptr);
    frame->Show();
    return true;
}

void CalcFrame::CalcFrameOnClose(class wxCloseEvent& e)
{
	// Not working properly.
    Close(true);
}

void CalcFrame::m_textCtrlInputOnTextEnter(class wxCommandEvent& e)
{

}

void CalcFrame::m_buttonEnterOnButtonClick(class wxCommandEvent& e)
{

}

#endif
