/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

// gui.cpp : Defines the user iteraction behaviors for the application.
//

#include "pch.hpp"

#include "formatter.hpp"
#include "issue_manager.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#ifdef CALC_TESTS_ENABLED
#include "tests.hpp"
#endif

#ifdef CALC_DONT_USE_SUBNORMALS
#include <immintrin.h> // TODO: Move to the core!
#endif

// ================= GUI ===============

#include "gui.hpp"

#ifdef _WIN32

/**
 * Buffer for the GUI used for equasion processing
 */
static std::array<char, CalcConfiguration::input_max_data_size> input
    [[indeterminate]];

class CalcWindow {
public:
  CalcWindow() noexcept = default;
  CalcWindow(const CalcWindow &) = delete;
  CalcWindow(CalcWindow &&) = delete;
  ~CalcWindow() = default;

  /**
   * Perform calculation from the GUI
   */
  void perform_calculation() {
    const std::string_view equation(
        input.data(), static_cast<size_t>(GetWindowTextA(
                          layout.get_constraints_handle(0), input.data(),
                          static_cast<int>(input.size()))));

    Lexer l(equation);
    Parser p(l);
    const auto value = p.parse();

#if defined(CALC_USE_ERROR_TOKEN)
    const bool has_errors = xxx;
    if (has_errors) {
      yyy;
    }
#else
    const auto has_errors = IssueManager::has_errors();

    if (has_errors) [[unlikely]] {
      Formatter::Summary summary [[indeterminate]];
      const auto summary_text_end = Formatter::create_summary(summary);
      IssueManager::clear();

      *summary_text_end = '\0'; // because of LPCSTR in the SetWindowTextA
      SetWindowTextA(layout.get_constraints_handle(1), summary.data());
    }
#endif
    else [[likely]] {
      Formatter::Result result [[indeterminate]];
      const auto result_text_end = Formatter::format(value, result);

      *result_text_end = '\0'; // because of LPCSTR in the SetWindowTextA
      SetWindowTextA(layout.get_constraints_handle(1), result.data());
    }
  }

  /**
   * Save user data from GUI.
   */
  void save_user_data(const HWND window) const { save_window_data(window); }

  /**
   * Close Calc GUI.
   */
  void close(const HWND window) const { EndDialog(window, FALSE); }

  /**
   * Initialize Calc GUI and load user data into it.
   */
  void init(const HWND window, const HINSTANCE instance) {
    add_about_menu_to_system_menu(window);
    set_window_icons(window, instance);
#ifdef CALC_SUPPORT_DPI_CHANGES
    dpi = get_window_dpi(window);
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    init_min_sizes(window, to_physical(CalcConfiguration::min_width, dpi),
                   to_physical(CalcConfiguration::min_height, dpi), dpi);
#else
    init_min_sizes(window, to_physical(CalcConfiguration::min_width, dpi),
                   to_physical(CalcConfiguration::min_height, dpi));
#endif
#else
    init_min_sizes(window, CalcConfiguration::min_width,
                   CalcConfiguration::min_height);
#endif
    layout_init(window);
    load_window_data(window);

#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
    SendMessageA(layout.get_constraints_handle(0), EM_LIMITTEXT,
                 static_cast<WPARAM>(CalcConfiguration::input_max_symbols), 0);
#endif
    SetWindowTextA(layout.get_constraints_handle(0),
#ifdef CALC_TESTS_ENABLED
                   calc_tests().data()
#else
                   input.data()
#endif
    );
    SendMessageA(layout.get_constraints_handle(0), EM_SETSEL,
                 static_cast<WPARAM>(0), static_cast<LPARAM>(-1));
    SendMessageA(layout.get_constraints_handle(0), EM_SCROLLCARET, 0, 0);
  }

  /**
   * Resize Calc window.
   */
  inline void resize(const WORD new_width, const WORD new_height) {
    layout.resize(new_width, new_height);
  }

  /**
   * Return to the system minimal sizes for Calc window.
   */
  inline void get_minmaxinfo(const LPMINMAXINFO lpMMI) const {
    lpMMI->ptMinTrackSize.x = layout.get_min_x();
    lpMMI->ptMinTrackSize.y = layout.get_min_y();
  }
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  void set_dpi(const HWND window, const WORD new_dpi, Rect_ptr new_rect) {
    // Store new DPI
    dpi = new_dpi;

    // Init min sizes
    init_min_sizes(window, to_physical(CalcConfiguration::min_width, dpi),
                   to_physical(CalcConfiguration::min_height, dpi), dpi);

    // Rescale main window to suggestion and set the window position
    SetWindowPos(window, nullptr, new_rect->get_x(), new_rect->get_y(),
                 new_rect->get_width(), new_rect->get_heigth(),
                 SWP_NOZORDER | SWP_NOACTIVATE);
  }
#endif
  /**
   * Initialize layout helper for resizing.
   */
  void layout_init(const HWND window) {
    layout.init_window(window);
    layout.init_anchor(window, 0, IDC_EDIT_INPUT,
                       Anchor{HorizontalMode::Stretch, VerticalMode::Stretch});
    layout.init_anchor(window, 1, IDC_EDIT_RESULT,
                       Anchor{HorizontalMode::Right, VerticalMode::Bottom});
    layout.init_anchor(window, 2, IDC_BUTTON_CALC,
                       Anchor{HorizontalMode::Right, VerticalMode::Bottom});
    static_assert(3 == CalcConfiguration::elements);
  }

private:
#ifdef CALC_SUPPORT_DPI_CHANGES
  static LONG to_physical(LONG value, LONG dpi) {
    return std::lroundf(static_cast<float>(value) * static_cast<float>(dpi) /
                        static_cast<float>(USER_DEFAULT_SCREEN_DPI));
  }

  static LONG to_logical(LONG value, LONG dpi) {

    return std::lroundf(static_cast<float>(value) *
                        static_cast<float>(USER_DEFAULT_SCREEN_DPI) /
                        static_cast<float>(dpi));
  };
#endif

#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  void init_min_sizes(const HWND window, const LONG requested_min_width,
                      const LONG requested_min_height, UINT new_dpi) {
#else
  void init_min_sizes(const HWND window, const LONG requested_min_width,
                      const LONG requested_min_height) {
#endif
    // Convert minimum client area size to window (outer) size so the user can't
    // resize window smaller than the intended client area. WM_GETMINMAXINFO
    // expects window dimensions.
    Rect requiredClient(0, 0, requested_min_width, requested_min_height);

    // Retrieve window styles to adjust for non-client area.
    const auto style = static_cast<DWORD>(GetWindowLongPtrA(window, GWL_STYLE));
    const auto exStyle =
        static_cast<DWORD>(GetWindowLongPtrA(window, GWL_EXSTYLE));

    // AdjustWindowRectEx will expand the rectangle so that the resulting outer
    // window will have the requested client size.
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    AdjustWindowRectExForDpi(&requiredClient, style, FALSE, exStyle, new_dpi);
#else
    AdjustWindowRectEx(&requiredClient, style, FALSE, exStyle);
#endif
    layout.init_min_sizes(requiredClient.get_width(),
                          requiredClient.get_heigth());
  }

  void load_window_data(const HWND window) {

    const RegRead reg(HKEY_CURRENT_USER, CalcConfiguration::reg_key);
#ifndef CALC_TESTS_ENABLED
    input[reg.read("input", reinterpret_cast<LPBYTE>(input.data()),
                   static_cast<DWORD>(input.size()))] = '\0';
#endif

    const auto flags = reg.read("flags");
    const auto show = reg.read("showCmd");

    const auto left = reg.read("left");
    const auto top = reg.read("top");
    const auto right = reg.read("right");
    const auto bottom = reg.read("bottom");
#ifdef CALC_SUPPORT_DPI_CHANGES
    const auto sSavedDpi = reg.read("savedDpi");
#endif
    WINDOWPLACEMENT wp [[indeterminate]];
    wp.length = sizeof(wp);
    wp.flags = static_cast<UINT>(flags ? *flags : 0);
    wp.showCmd = static_cast<UINT>(show ? *show : SW_SHOWNORMAL);

    if (left && top && right && bottom) {
      // -> use saved position
#ifdef CALC_SUPPORT_DPI_CHANGES
      if (sSavedDpi) {
        // -> use with saved dpi
        const auto saved_dpi = *sSavedDpi;
        wp.rcNormalPosition.left = to_physical(*left, saved_dpi);
        wp.rcNormalPosition.top = to_physical(*top, saved_dpi);
        wp.rcNormalPosition.right = to_physical(*right, saved_dpi);
        wp.rcNormalPosition.bottom = to_physical(*bottom, saved_dpi);
      }
#else
      // -> use as is
      wp.rcNormalPosition.left = *left;
      wp.rcNormalPosition.top = *top;
      wp.rcNormalPosition.right = *right;
      wp.rcNormalPosition.bottom = *bottom;
#endif
    } else {
      // -> use default position
#ifdef CALC_SUPPORT_DPI_CHANGES
      const auto shift = to_physical(CalcConfiguration::default_shift_px, dpi);
#else
      const auto &shift = CalcConfiguration::default_shift_px;
#endif
      wp.rcNormalPosition.left = shift;
      wp.rcNormalPosition.top = shift;
      wp.rcNormalPosition.right = layout.get_min_width() + shift;
      wp.rcNormalPosition.bottom = layout.get_min_height() + shift;
    }
    SetWindowPlacement(window, &wp);
  }

  void save_window_data(const HWND hWnd) const {
    const RegWrite reg(HKEY_CURRENT_USER, CalcConfiguration::reg_key);
#ifndef CALC_TESTS_ENABLED
    reg.write("input", reinterpret_cast<BYTE *>(input.data()),
              static_cast<DWORD>(
                  GetWindowTextA(layout.get_constraints_handle(0), input.data(),
                                 static_cast<int>(input.size()))));
#endif

    WINDOWPLACEMENT wp [[indeterminate]];
    wp.length = sizeof(wp);
    GetWindowPlacement(hWnd, &wp);
    reg.write("flags", wp.flags);
    reg.write("showCmd", wp.showCmd);
#ifdef CALC_SUPPORT_DPI_CHANGES
    reg.write("left", to_logical(wp.rcNormalPosition.left, dpi));
    reg.write("top", to_logical(wp.rcNormalPosition.top, dpi));
    reg.write("right", to_logical(wp.rcNormalPosition.right, dpi));
    reg.write("bottom", to_logical(wp.rcNormalPosition.bottom, dpi));
    reg.write("savedDpi", dpi);
#else
    reg.write("left", wp.rcNormalPosition.left);
    reg.write("top", wp.rcNormalPosition.top);
    reg.write("right", wp.rcNormalPosition.right);
    reg.write("bottom", wp.rcNormalPosition.bottom);
#endif
  }

  Layout<CalcConfiguration::elements> layout [[indeterminate]];

#ifdef CALC_SUPPORT_DPI_CHANGES
  DWORD dpi [[indeterminate]];
#endif
};

static CalcWindow calc;

/**
 * About dialog callback processing (resource-based).
 */
static INT_PTR CALLBACK AboutDlgProc(const HWND dlg, const UINT msg,
                                     const WPARAM wParam,
                                     const LPARAM
#ifdef CALC_SUPPORT_LINK_WINDOW
                                         lParam
#endif
) {
  switch (msg) {
  case WM_COMMAND:
    if (LOWORD(wParam) == IDCANCEL) {
      EndDialog(dlg, FALSE);
      return TRUE;
    }
    return FALSE;
#ifdef CALC_SUPPORT_LINK_WINDOW
  case WM_NOTIFY: {
    const auto nm = reinterpret_cast<LPNMHDR>(lParam);
    if (nm->idFrom == IDC_LINK_HOMEPAGE && nm->code == NM_CLICK) {
      const auto link = reinterpret_cast<NMLINK *>(lParam);
      ShellExecuteW(nullptr, L"open", link->item.szUrl, nullptr, nullptr,
                    SW_SHOWNORMAL);
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

/**
 * Calc dialog callback processing (resource-based).
 */
static INT_PTR CALLBACK CalcDialogProc(const HWND window, const UINT msg,
                                       const WPARAM wParam,
                                       const LPARAM lParam) {
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  static bool dpi_change_in_progress = false;
#endif
  switch (msg) {
  case WM_COMMAND: {
    if (LOWORD(wParam) == IDC_BUTTON_CALC && HIWORD(wParam) == BN_CLICKED) {
      calc.perform_calculation();
      return TRUE;
    }
    return FALSE;
  }
  case WM_SYSCOMMAND: {
    if ((wParam & 0xFFF0) == IDM_ABOUTBOX) {
      DialogBoxParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDD_ABOUTBOX),
                      window, AboutDlgProc, FALSE);
      return TRUE;
    }
    return FALSE;
  }
  case WM_GETMINMAXINFO: {
    calc.get_minmaxinfo(reinterpret_cast<LPMINMAXINFO>(lParam));
    return TRUE;
  }
  case WM_SIZE: {
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    if (dpi_change_in_progress) {
      calc.layout_init(window);
      return FALSE;
    }
#endif
    calc.resize(LOWORD(lParam), HIWORD(lParam));
    return TRUE;
  }
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  case WM_GETDPISCALEDSIZE: {
    // https://learn.microsoft.com/en-us/windows/win32/hidpi/wm-getdpiscaledsize
    dpi_change_in_progress = true;
    return FALSE;
  }
#endif
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  case WM_DPICHANGED: {
    calc.set_dpi(window, HIWORD(wParam),
                 reinterpret_cast<const Rect_ptr>(lParam));
    dpi_change_in_progress = false;
    return TRUE;
  }
#endif
  case WM_INITDIALOG: {
    calc.init(window, reinterpret_cast<HINSTANCE>(lParam));
    return TRUE;
  }
#ifdef CALC_SUPPORT_AUTO_RESTART
  case WM_ENDSESSION: {
    if (wParam) {
      calc.save_user_data(window);
    }
    return TRUE;
  }
#endif
  case WM_CLOSE: {
    calc.save_user_data(window);
    calc.close(window);
    return TRUE;
  }
  case WM_DESTROY: {
    PostQuitMessage(EXIT_SUCCESS);
    return FALSE;
  }
  default: {
    return FALSE;
  }
  }
}

/**
 * Calc entrypoint in Win32 GUI
 */
int WINAPI WinMain(const HINSTANCE instance, const HINSTANCE /*prev_instance*/,
                   const LPSTR /*cmd_line*/, const int /*cmd_show*/) {
#ifdef CALC_DONT_USE_SUBNORMALS
  // TODO: move this code to the core.
  /* Applications that generate floating point underflow in vector registers can
   * benefit from setting the flush-to-zero mode rather than generating
   * subnormal numbers in case of underflow:*/
  /* It is strongly recommended to set the flush-to-zero mode unless you have
   * special reasons to use subnormal numbers. You may, in addition, set the
   * denormals-are-zero mode if vector regsiters are available:*/
  _mm_setcsr(_mm_getcsr() | _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);
#endif
#ifdef CALC_SUPPORT_DPI_CHANGES
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  // https://learn.microsoft.com/en-us/windows/win32/hidpi/dpi-awareness-context
  //  Per monitor DPI aware. This window checks for the DPI when it is created
  //  and adjusts the scale factor whenever the DPI changes. These processes are
  //  not automatically scaled by the system.
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#else
  // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setprocessdpiaware
  // System DPI aware. This window does not scale for DPI changes. It will query
  // for the DPI once and use that value for the lifetime of the process. If the
  // DPI changes, the process will not adjust to the new DPI value. It will be
  // automatically scaled up or down by the system when the DPI changes from the
  // system value.
  // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
  SetProcessDPIAware();
#endif
#endif
#ifdef CALC_SUPPORT_AUTO_RESTART
  RegisterApplicationRestart(nullptr, FALSE);
#endif

  // Disable IME completely because Calc use only ANSI input in GUI
  ImmDisableIME(FALSE);

  return static_cast<int>(
      DialogBoxParamA(instance, MAKEINTRESOURCEA(IDD_CALC_DIALOG), nullptr,
                      CalcDialogProc, reinterpret_cast<LPARAM>(instance)));
}

#if defined _M_IX86
#pragma comment(                                                               \
    linker,                                                                    \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(                                                               \
    linker,                                                                    \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(                                                               \
    linker,                                                                    \
    "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "user32.lib")

#else

class MyApp : public wxApp {
public:
  bool OnInit() override;
};

// This defines the equivalent of main() for the current platform.
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
#ifdef CALC_DONT_USE_SUBNORMALS
  // TODO: move this code to the core.
  /* Applications that generate floating point underflow in vector registers can
   * benefit from setting the flush-to-zero mode rather than generating
   * subnormal numbers in case of underflow:*/
  /* It is strongly recommended to set the flush-to-zero mode unless you have
   * special reasons to use subnormal numbers. You may, in addition, set the
   * denormals-are-zero mode if vector regsiters are available:*/
  _mm_setcsr(_mm_getcsr() | _MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON);
#endif
  CalcFrame *frame = new CalcFrame(nullptr);
  frame->Show();
  return true;
}

void CalcFrame::CalcFrameOnClose(class wxCloseEvent &e) {
  // FIXME: Not working properly.
  Close(true);
}

void CalcFrame::m_textCtrlInputOnTextEnter(class wxCommandEvent &e) {}

void CalcFrame::m_buttonEnterOnButtonClick(class wxCommandEvent &e) {}

#endif
