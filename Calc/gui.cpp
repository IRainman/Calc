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
#include "gui_unicode_normalizer.hpp"

namespace GUI {

#ifdef _WIN32

/**
 * Calc GUI window also it's application itself.
 */
class CalcApp {
  /**
   * Calc GUI configuration: matches RC and system internals for correct work.
   */
  struct CalcConfiguration {
    static constexpr const char *reg_key = "Software\\HedgehogInTheCPP\\Calc";
    static constexpr LONG min_width = 232;  // matches RC
    static constexpr LONG min_height = 158; // matches RC
    static constexpr BYTE elements = 3;     // matches RC

    static constexpr BYTE default_shift_px = 100;

    // https://learn.microsoft.com/windows/win32/controls/em-limittext
    static constexpr UINT input_max_text_length =
#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
        128 * 1024;
#else
        // max is 32k symbols for Edit control and can't be changed:
        32 * 1024;
#endif
    static constexpr UINT input_max_data_size =
        input_max_text_length * sizeof(WCHAR);
  };

  using cfg = CalcConfiguration;

public:
  constexpr CalcApp() noexcept {
    _setup_crt_leak_check();
    init_uxtheme_callers();
#ifdef CALC_SUPPORT_DPI_CHANGES
    // This application is system DPI aware. It for the DPI value once at start.
    // If the DPI changes application will be automatically scaled up or down by
    // the system. #ifdef CALC_SUPPORT_DPI_FOR_WINDOW
    // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
    SetProcessDPIAware();
#endif
#ifdef CALC_SUPPORT_AUTO_RESTART
    RegisterApplicationRestart(nullptr, FALSE);
#endif
#ifdef CALC_DISABLE_IME
    ImmDisableIME(FALSE);
#endif
  }

  CalcApp(const CalcApp &) = delete;
  CalcApp(CalcApp &&) = delete;

  constexpr ~CalcApp() noexcept = default;

  /**
   * Perform calculation from the GUI
   */
  [[nodiscard]] constexpr BOOL calc(const WPARAM wP) noexcept {
    if (LOWORD(wP) == IDC_BUTTON_CALC && HIWORD(wP) == BN_CLICKED) {
      EditView input(_layout.handle(0));
#ifdef CALC_ALLOW_UNICODE_IN_GUI
      Normalizer to_ansi(input, _equasion);
      if (to_ansi.failed())
#else
      input.ansi(_equasion);
      if (_equasion.length() != input.length())
#endif
          [[unlikely]] {
#if defined(CALC_USE_ERROR_TOKEN)
        set_result("Only ANSI symbols supported, Unicode isn't supported");
#elif defined(CALC_ALLOW_UNICODE_IN_GUI)
        IssueManager::report_error(
            to_ansi.normalized(),
            "this Unicode character is not part of the Calc language");
#else
        IssueManager::report_error(
            0, "Only ANSI symbols supported, Unicode isn't supported");
#endif
      }
      Lexer l(_equasion);
      Parser p(l);
      const auto value = p.parse();
#if defined(CALC_USE_ERROR_TOKEN)
      switch (value.type) {
      case Token::Type::RESULT [[likely]]:
        Formatter::Result result [[indeterminate]];
        set_result(result.data(), Formatter::format(value, result));
        return TRUE;
      case Token::Type::ERROR:
        set_result(value.error_text, value.error_text + value.error_text_size));
        break;
        default [[unlikely]] : set_result("parser internal error");
        break;
      }
#else
      if (IssueManager::has_errors()) {
        Formatter::Summary summary [[indeterminate]];
        set_result(summary.data(), Formatter::create_summary(summary));
      } else [[likely]] {
        Formatter::Result result [[indeterminate]];
        set_result(result.data(), Formatter::format(value, result));
        return TRUE;
      }
#endif
    }
    return FALSE;
  }

  /**
   * Save user data from GUI.
   */
  constexpr void save_user_data(const HWND window) noexcept {

    save_window_data(window);
  }

  /**
   * Close Calc GUI.
   */
  [[nodiscard]] constexpr auto close(const HWND window) const noexcept {
    return EndDialog(window, FALSE);
  }

  /**
   * Create Calc GUI.
   */
  constexpr auto create(const HINSTANCE instance,
                        DlgProc CalcDialogProc) const noexcept {
    if (SUCCEEDED(DialogBoxParamA(instance, MAKEINTRESOURCEA(IDD_CALC_DIALOG),
                                  nullptr, CalcDialogProc,
                                  reinterpret_cast<LPARAM>(instance)))) {
      return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
  }

  /**
   * Initialize Calc GUI and load user data into it.
   */
  [[nodiscard]] constexpr BOOL init(const HWND window,
                                    const LPARAM lP) noexcept {
    const HINSTANCE instance = reinterpret_cast<HINSTANCE>(lP);
    set_icons(window, instance);
    add_about_menu_to_system_menu(window);
#ifdef CALC_SUPPORT_DPI_CHANGES
    _dpi = dpi(window);
    init_min_sizes(window, physical(cfg::min_width, _dpi),
                   physical(cfg::min_height, _dpi));
#else
    init_min_sizes(window, cfg::min_width, cfg::min_height);
#endif
    layout_init(window);
    load_window_data(window);
#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
    set_text_limit(_layout.handle(0), cfg::input_max_text_length);
#endif
#ifdef CALC_SUPPORT_EXTENDENT_STYLES
    set_extended_style(_layout.handle(0), ES_EX_ALLOWEOL_ALL);
#endif
    goto_end_of_text(_layout.handle(0));
    return TRUE;
  }

  /**
   * Resize Calc window.
   */
  [[nodiscard]] constexpr inline BOOL resize(const LPARAM lP) noexcept {
    const WORD width = LOWORD(lP);
    const WORD height = HIWORD(lP);
    _layout.resize(width, height);
    return TRUE;
  }

  /**
   * Return to the system minimal sizes for Calc window.
   */
  [[nodiscard]] constexpr inline BOOL
  minmaxinfo(const LPARAM lP) const noexcept {
    const auto lpMMI = reinterpret_cast<LPMINMAXINFO>(lP);
    lpMMI->ptMinTrackSize.x = _layout.min_x();
    lpMMI->ptMinTrackSize.y = _layout.min_y();
    return TRUE;
  }

  /**
   * Initialize layout helper for resizing.
   */
  constexpr inline void layout_init(const HWND window) noexcept {
    _layout.init_window(window);
    _layout.init_anchor(window, 0, IDC_EDIT_INPUT,
                        Anchor::HorizontalStretch | Anchor::VerticalStretch);
    _layout.init_anchor(window, 1, IDC_EDIT_RESULT,
                        Anchor::Right | Anchor::Bottom);
    _layout.init_anchor(window, 2, IDC_BUTTON_CALC,
                        Anchor::Right | Anchor::Bottom);
    static_assert(3 == cfg::elements);
  }

#ifdef CALC_SUPPORT_DARK_MODE
  [[nodiscard]] constexpr inline auto &theme() noexcept { return _theme; }
#endif

  [[nodiscard]] constexpr inline auto &about() noexcept { return _about; }

private:
  constexpr inline void set_result(const char *text,
                                   char *text_end) const noexcept {
    set_text(_layout.handle(1), text, text_end);
  }

  constexpr inline void set_result(const char *text) const noexcept {
    set_text(_layout.handle(1), text);
  }

  constexpr void init_min_sizes(const HWND window, const LONG min_width,
                                const LONG min_height) noexcept {
    Rect client(0, 0, min_width, min_height);
    const auto style = static_cast<DWORD>(GetWindowLongPtrA(window, GWL_STYLE));
    const auto exStyle =
        static_cast<DWORD>(GetWindowLongPtrA(window, GWL_EXSTYLE));
    AdjustWindowRectEx(&client, style, FALSE, exStyle);
    _layout.init_min_sizes(client.width(), client.height());
  }

  constexpr inline void load_window_data(const HWND window) noexcept {
    _equasion.reserve(cfg::input_max_text_length);
    Edit input(_layout.handle(0), cfg::input_max_text_length);
    const RegRead reg(HKEY_CURRENT_USER, cfg::reg_key);
#ifndef CALC_TESTS_ENABLED
    input.set_size(reg.read("input", input.data(), cfg::input_max_data_size));
#else
    {
      const auto tests = calc_tests();
      input.write(tests.data(), static_cast<int>(tests.size()));
    }
#endif
    const auto flags = reg.read("flags");
    const auto show = reg.read("showCmd");

    const auto left = reg.read("left");
    const auto top = reg.read("top");
    const auto right = reg.read("right");
    const auto bottom = reg.read("bottom");
#ifdef CALC_SUPPORT_DPI_CHANGES
    const auto dpi = reg.read("savedDpi");
#endif
    WINDOWPLACEMENT wp [[indeterminate]];
    wp.length = sizeof(wp);
    wp.flags = static_cast<UINT>(flags ? *flags : 0);
    wp.showCmd = static_cast<UINT>(show ? *show : SW_SHOWNORMAL);

    if (left && top && right && bottom) {
      // -> use saved position
#ifdef CALC_SUPPORT_DPI_CHANGES
      if (dpi) {
        // -> use with saved dpi
        const auto saved_dpi = *dpi;
        wp.rcNormalPosition.left = physical(*left, saved_dpi);
        wp.rcNormalPosition.top = physical(*top, saved_dpi);
        wp.rcNormalPosition.right = physical(*right, saved_dpi);
        wp.rcNormalPosition.bottom = physical(*bottom, saved_dpi);
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
      const auto shift = physical(cfg::default_shift_px, _dpi);
#else
      const auto &shift = cfg::default_shift_px;
#endif
      wp.rcNormalPosition.left = shift;
      wp.rcNormalPosition.top = shift;
      wp.rcNormalPosition.right = _layout.min_width() + shift;
      wp.rcNormalPosition.bottom = _layout.min_height() + shift;
    }
    SetWindowPlacement(window, &wp);
  }

  constexpr void save_window_data(const HWND hWnd) noexcept {
    const RegWrite reg(HKEY_CURRENT_USER, cfg::reg_key);
#ifndef CALC_TESTS_ENABLED
    EditView input(_layout.handle(0));
    reg.write("input", input.data(), input.size());
#endif

    WINDOWPLACEMENT wp [[indeterminate]];
    wp.length = sizeof(wp);
    GetWindowPlacement(hWnd, &wp);
    reg.write("flags", wp.flags);
    reg.write("showCmd", wp.showCmd);
#ifdef CALC_SUPPORT_DPI_CHANGES
    reg.write("left", logical(wp.rcNormalPosition.left, _dpi));
    reg.write("top", logical(wp.rcNormalPosition.top, _dpi));
    reg.write("right", logical(wp.rcNormalPosition.right, _dpi));
    reg.write("bottom", logical(wp.rcNormalPosition.bottom, _dpi));
    reg.write("savedDpi", _dpi);
#else
    reg.write("left", wp.rcNormalPosition.left);
    reg.write("top", wp.rcNormalPosition.top);
    reg.write("right", wp.rcNormalPosition.right);
    reg.write("bottom", wp.rcNormalPosition.bottom);
#endif
  }

  struct About {
    [[nodiscard]] static constexpr BOOL create(const HWND parent, WPARAM wP,
                                               DlgProc AboutDlgProc) noexcept {
      if ((wP & 0xFFF0) == IDM_ABOUTBOX &&
          SUCCEEDED(DialogBoxParamA(GetModuleHandleA(nullptr),
                                    MAKEINTRESOURCEA(IDD_ABOUTBOX), parent,
                                    AboutDlgProc, FALSE))) {
        return TRUE;
      } else {
        return FALSE;
      }
    }

    [[nodiscard]] static constexpr BOOL open_homepage(const HWND window,
                                                      LPARAM lP) noexcept {
      const auto nm = reinterpret_cast<LPNMHDR>(lP);
      if (nm->idFrom == IDC_LINK_HOMEPAGE && nm->code == NM_CLICK) {
        const auto l = reinterpret_cast<NMLINK *>(lP);
        ShellExecuteW(window, L"open", l->item.szUrl, nullptr, nullptr,
                      SW_SHOWNORMAL);
        return TRUE;
      } else {
        return FALSE;
      }
    }

    [[nodiscard]] static constexpr BOOL close(const HWND window,
                                              const WPARAM wP) noexcept {
      if (LOWORD(wP) == IDCANCEL) {
        return EndDialog(window, FALSE);
      } else {
        return FALSE;
      }
    }
  };

  [[no_unique_address]] std::string _equasion;

  [[no_unique_address]] Layout<cfg::elements> _layout [[indeterminate]];
#ifdef CALC_SUPPORT_DPI_CHANGES
  [[no_unique_address]] UINT _dpi [[indeterminate]];
#endif
#ifdef CALC_SUPPORT_DARK_MODE
  [[no_unique_address]] Theme _theme [[indeterminate]];
#endif
  [[no_unique_address]] About _about;
};

static CalcApp gui;

/**
 * About dialog callback processing (resource-based).
 */
constexpr static INT_PTR CALLBACK AboutDlgProc(const HWND window,
                                               const UINT msg, const WPARAM wP,
                                               const LPARAM
#ifdef CALC_SUPPORT_LINK_WINDOW
                                                   lP
#endif
                                               ) noexcept {
  switch (msg) {
  case WM_COMMAND:
    return gui.about().close(window, wP);
#ifdef CALC_SUPPORT_LINK_WINDOW
  case WM_NOTIFY:
    return gui.about().open_homepage(window, lP);
#endif
#ifdef CALC_SUPPORT_DARK_MODE
  case WM_INITDIALOG:
    gui.theme().apply(window);
    return TRUE;
#ifdef CALC_SUPPORT_DARK_MODE_WITHOUT_WIN32_HELPER
  case WM_CTLCOLORDLG:
  case WM_CTLCOLORSTATIC:
    return gui.theme().apply(wP);
#endif
  case WM_SYSCOLORCHANGE:
    gui.theme().apply(window, true);
    return TRUE;
#endif
  default:
    return FALSE;
  }
}

/**
 * Calc dialog callback processing (resource-based).
 */
constexpr static INT_PTR CALLBACK CalcDialogProc(const HWND window,
                                                 const UINT msg,
                                                 const WPARAM wP,
                                                 const LPARAM lP) noexcept {
  switch (msg) {
  case WM_COMMAND:
    return gui.calc(wP);
  case WM_SYSCOMMAND:
    return gui.about().create(window, wP, AboutDlgProc);
  case WM_GETMINMAXINFO:
    return gui.minmaxinfo(lP);
  case WM_SIZE:
    return gui.resize(lP);
  case WM_INITDIALOG:
#ifdef CALC_SUPPORT_DARK_MODE
    gui.theme().init(window);
#endif
    return gui.init(window, lP);
#ifdef CALC_SUPPORT_AUTO_RESTART
  case WM_ENDSESSION:
    if (wP) {
      gui.save_user_data(window);
    }
    return TRUE;
#endif
#ifdef CALC_SUPPORT_DARK_MODE
#ifdef CALC_SUPPORT_DARK_MODE_WITHOUT_WIN32_HELPER
  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORDLG:
    return gui.theme().apply(wP);
#endif
  case WM_SYSCOLORCHANGE:
    gui.theme().apply(window, true, true);
    return TRUE;
#endif
  case WM_CLOSE:
    gui.save_user_data(window);
    return gui.close(window);
  default:
    return FALSE;
  }
}

/**
 * Calc entrypoint in Win32 GUI
 */
int WINAPI WinMain(const HINSTANCE instance, const HINSTANCE /*prev_instance*/,
                   const LPSTR /*cmd_line*/, const int /*cmd_show*/) {
  return gui.create(instance, CalcDialogProc);
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

#ifdef CALC_DISABLE_IME
#pragma comment(lib, "imm32.lib")
#endif

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

} // namespace GUI
