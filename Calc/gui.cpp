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

#include "gui.hpp"

#ifdef _WIN32

/**
 * Calc GUI window also it's application itself.
 */
class CalcWindow {
public:
  constexpr CalcWindow() noexcept = default;

  CalcWindow(const CalcWindow &) = delete;
  CalcWindow(CalcWindow &&) = delete;

  constexpr ~CalcWindow() noexcept = default;

  /**
   * Perform calculation from the GUI
   */
  void perform_calculation() noexcept {
#ifdef CALC_USED_EDIT_VIEW
    EditTextView text(layout.get_handle(0));
    Lexer l(text.get_view());
#else
    Lexer l({input.data(), get_user_input()});
#endif
    Parser p(l);
    const auto value = p.parse();

#if defined(CALC_USE_ERROR_TOKEN)
    const bool has_errors = xxx;
    if (has_errors) {
      yyy;
    }
#endif
    if (IssueManager::has_errors()) [[unlikely]] {
      Formatter::Summary summary [[indeterminate]];
      set_result(summary.data(), Formatter::create_summary(summary));
    } else [[likely]] {
      Formatter::Result result [[indeterminate]];
      set_result(result.data(), Formatter::format(value, result));
    }
  }

  /**
   * Save user data from GUI.
   */
  void save_user_data(const HWND window) noexcept { save_window_data(window); }

  /**
   * Close Calc GUI.
   */
  void close(const HWND window) const noexcept { EndDialog(window, FALSE); }

  /**
   * Initialize Calc GUI and load user data into it.
   */
  void init(const HWND window, const HINSTANCE instance) noexcept {

    set_window_icons(window, instance);

    add_about_menu_to_system_menu(window);

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
    set_window_text_limit(layout.get_handle(0),
#ifdef CALC_USED_EDIT_VIEW
                          CalcConfiguration::input_max_symbols
#else
                          input.max_size()
#endif
    );
#endif

#ifdef CALC_SUPPORT_EXTENDENT_STYLES
    edit_set_extended_style(layout.get_handle(0), ES_EX_ALLOWEOL_ALL,
                            ES_EX_ALLOWEOL_ALL);
#endif

#ifdef CALC_TESTS_ENABLED
    auto tests = calc_tests();
    set_window_text(layout.get_handle(0), tests.data(),
                    tests.data() + tests.size());
#else
    set_window_text(layout.get_handle(0), input.data(),
                    input.data() + input.size());
#endif

    goto_end_of_window_text(layout.get_handle(0));
  }

  /**
   * Resize Calc window.
   */
  inline void resize(const WORD new_width, const WORD new_height) noexcept {

    layout.resize(new_width, new_height);
  }

  /**
   * Return to the system minimal sizes for Calc window.
   */
  inline void get_minmaxinfo(const LPMINMAXINFO lpMMI) const noexcept {
    lpMMI->ptMinTrackSize.x = layout.get_min_x();
    lpMMI->ptMinTrackSize.y = layout.get_min_y();
  }
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  void set_dpi(const HWND window, const WORD new_dpi,
               Rect_ptr new_rect) noexcept {
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
  void layout_init(const HWND window) noexcept {
    layout.init_window(window);
    layout.init_anchor(window, 0, IDC_EDIT_INPUT,
                       Anchor{HorizontalMode::Stretch, VerticalMode::Stretch});
    layout.init_anchor(window, 1, IDC_EDIT_RESULT,
                       Anchor{HorizontalMode::Right, VerticalMode::Bottom});
    layout.init_anchor(window, 2, IDC_BUTTON_CALC,
                       Anchor{HorizontalMode::Right, VerticalMode::Bottom});
    static_assert(3 == CalcConfiguration::elements);
  }

#ifdef CALC_SUPPORT_DARK_MODE
  Theme &theme() { return _theme; }
#endif

private:
#ifndef CALC_USED_EDIT_VIEW
  /**
   * Read the user input from GUI to input buffer and return its size.
   */
  [[nodiscard]] UINT get_user_input() noexcept {
    input.set_size(
        get_window_text(layout.get_handle(0), input.data(), input.max_size()));
    return input.size();
  }
#endif

  /**
   * Set the result text in the GUI.
   */
  void set_result(const char *result_text,
                  char *result_text_end) const noexcept {
    set_window_text(layout.get_handle(1), result_text, result_text_end);
  }

  void init_min_sizes(const HWND window, const LONG requested_min_width,
                      const LONG requested_min_height
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
                      ,
                      UINT new_dpi
#endif
                      ) noexcept {
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

  void load_window_data(const HWND window) noexcept {

    const RegRead reg(HKEY_CURRENT_USER, CalcConfiguration::reg_key);
#ifndef CALC_TESTS_ENABLED
    input.set_size(reg.read("input", reinterpret_cast<LPBYTE>(input.data()),
                            input.max_size()));
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

  void save_window_data(const HWND hWnd) noexcept {
    const RegWrite reg(HKEY_CURRENT_USER, CalcConfiguration::reg_key);
#ifndef CALC_TESTS_ENABLED
    reg.write("input", reinterpret_cast<const BYTE *>(input.data()),
              get_user_input());
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

  [[no_unique_address]] Layout<CalcConfiguration::elements> layout
      [[indeterminate]];

#ifdef CALC_SUPPORT_DARK_MODE
  [[no_unique_address]] Theme _theme [[indeterminate]];
#endif
#ifdef CALC_SUPPORT_DPI_CHANGES
  [[no_unique_address]] UINT dpi [[indeterminate]];
#endif
#ifndef CALC_USED_EDIT_VIEW
  // Should be always as last member, has a big size for handling user input.
  [[no_unique_address]] UserInput input [[indeterminate]];
#endif
};

static CalcWindow calc [[indeterminate]];

/**
 * About dialog callback processing (resource-based).
 */
static INT_PTR CALLBACK AboutDlgProc(const HWND window, const UINT msg,
                                     const WPARAM wParam,
                                     const LPARAM
#ifdef CALC_SUPPORT_LINK_WINDOW
                                         lParam
#endif
                                     ) noexcept {
  switch (msg) {
  case WM_COMMAND: {
    if (LOWORD(wParam) == IDCANCEL) {
      EndDialog(window, FALSE);
      return TRUE;
    }
    break;
  }
#ifdef CALC_SUPPORT_LINK_WINDOW
  case WM_NOTIFY: {
    const auto nm = reinterpret_cast<LPNMHDR>(lParam);
    if (nm->idFrom == IDC_LINK_HOMEPAGE && nm->code == NM_CLICK) {
      const auto link = reinterpret_cast<NMLINK *>(lParam);
      ShellExecuteW(window, L"open", link->item.szUrl, nullptr, nullptr,
                    SW_SHOWNORMAL);
      return TRUE;
    }
    break;
  }
#endif
#ifdef CALC_SUPPORT_DARK_MODE
  case WM_INITDIALOG: {
    calc.theme().apply(window);
    return TRUE;
  }
#ifdef CALC_SUPPORT_DARK_MODE_WITHOUT_WIN32_HELPER
  case WM_CTLCOLORDLG:
  case WM_CTLCOLORSTATIC: {
    return calc.theme().apply(wParam);
  }
#endif
  case WM_SYSCOLORCHANGE: {
    calc.theme().apply(window, true);
    return TRUE;
  }
#endif
  }
  return FALSE;
}

/**
 * Calc dialog callback processing (resource-based).
 */
static INT_PTR CALLBACK CalcDialogProc(const HWND window, const UINT msg,
                                       const WPARAM wParam,
                                       const LPARAM lParam) noexcept {
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  static bool dpi_change_in_progress = false;
#endif
  switch (msg) {
  case WM_COMMAND: {
    if (LOWORD(wParam) == IDC_BUTTON_CALC && HIWORD(wParam) == BN_CLICKED) {
      calc.perform_calculation();
      return TRUE;
    }
    break;
  }
  case WM_SYSCOMMAND: {
    if ((wParam & 0xFFF0) == IDM_ABOUTBOX) {
      DialogBoxParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDD_ABOUTBOX),
                      window, AboutDlgProc, FALSE);
      return TRUE;
    }
    break;
  }
  case WM_GETMINMAXINFO: {
    calc.get_minmaxinfo(reinterpret_cast<LPMINMAXINFO>(lParam));
    return TRUE;
  }
  case WM_SIZE: {
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    if (dpi_change_in_progress) {
      calc.layout_init(window);
      break;
    }
#endif
    calc.resize(LOWORD(lParam), HIWORD(lParam));
    return TRUE;
  }
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  case WM_GETDPISCALEDSIZE: {
    // https://learn.microsoft.com/en-us/windows/win32/hidpi/wm-getdpiscaledsize
    dpi_change_in_progress = true;
    break;
  }
  case WM_DPICHANGED: {
    calc.set_dpi(window, HIWORD(wParam),
                 reinterpret_cast<const Rect_ptr>(lParam));
    dpi_change_in_progress = false;
    return TRUE;
  }
#endif
  case WM_INITDIALOG: {
#ifdef CALC_SUPPORT_DARK_MODE
    calc.theme().init(window);
#endif
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
#ifdef CALC_SUPPORT_DARK_MODE
#ifdef CALC_SUPPORT_DARK_MODE_WITHOUT_WIN32_HELPER
  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORDLG: {
    return calc.theme().apply(wParam);
  }
#endif
  case WM_SYSCOLORCHANGE: {
    calc.theme().apply(window, true, true);
    return TRUE;
  }
#endif
  case WM_CLOSE: {
    calc.save_user_data(window);
    calc.close(window);
    return TRUE;
  }
  }
  return FALSE;
}

/**
 * Calc entrypoint in Win32 GUI
 */
int WINAPI WinMain(const HINSTANCE instance, const HINSTANCE /*prev_instance*/,
                   const LPSTR /*cmd_line*/, const int /*cmd_show*/) {
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

  DialogBoxParamA(instance, MAKEINTRESOURCEA(IDD_CALC_DIALOG), nullptr,
                  CalcDialogProc, reinterpret_cast<LPARAM>(instance));

  return EXIT_SUCCESS;
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

#ifdef CALC_SUPPORT_LINK_WINDOW
#pragma comment(lib, "comctl32.lib")
#endif
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "user32.lib")
#ifdef CALC_SUPPORT_DARK_MODE
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "uxtheme.lib")
#endif

#else

// TODO Qt

// for now, just print a message and exit with failure code
int main(int /*argc*/, char * /*argv[]*/) noexcept {
  fmt::print("Calc: GUI is not implemented for this platform yet.\n");
  return EXIT_FAILURE;
}

#endif
