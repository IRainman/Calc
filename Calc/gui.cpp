/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

/**
 * gui.cpp : Defines the user iteraction behaviors for the Calc.
 */

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

#include "resource.h" // GUI symbols

/**
 * @brief Calc GUI window also it's application itself.
 *
 * @warning [[indeterminate]] is used because before init window isn't usable.
 *          That is the system API requirements.
 */
class CalcApp {
  /**
   * Calc GUI configuration: matches RC and system internals for correct work.
   */
  struct CalcConfiguration {
    static constexpr const char *reg_key = "Software\\HedgehogInTheCPP\\Calc";

    static constexpr LONG min_width = 345;

    static constexpr LONG min_height = 170;

    static constexpr BYTE elements = 3;

    static constexpr BYTE default_shift_px = 100;

    /// @see https://learn.microsoft.com/windows/win32/controls/em-limittext
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
  /*
   * This is the basic initialization of the application.
   *
   * @warning Should be called before WinMain!
   * @see create(), init()
   */
  constexpr explicit CalcApp() noexcept {
#ifdef _DEBUG
    _setup_crt_leak_check();
#endif
#ifdef CALC_SUPPORT_DARK_MODE
    init_uxtheme_callers();
#endif
#ifdef CALC_SUPPORT_DPI_CHANGES
    /**
     * Calc is system DPI aware. It for the DPI value once at start. If the DPI
     * changes application will be automatically scaled up or down by the
     * system. #ifdef CALC_SUPPORT_DPI_FOR_WINDOW
     * SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
     */
    SetProcessDPIAware();
#endif
#ifdef CALC_SUPPORT_AUTO_RESTART
    /**
     * Calc automatically restarted after a crash, system shutdown or reboot or
     * user session end.
     */
    RegisterApplicationRestart(nullptr, FALSE);
#endif
  }

  CalcApp(CalcApp const &) = delete;
  CalcApp &operator=(CalcApp const &) = delete;
  CalcApp(CalcApp &&) = delete;

  constexpr ~CalcApp() noexcept = default;

  /**
   * Perform calculation from the GUI
   *      System UTF-16 input (EditView)
   *              ↓
   *      Unicode normalizer (Normalizer)
   *              ↓
   *       ASCII expression (_equasion)
   *              ↓
   *            Lexer
   *              ↓
   *            Parser
   *              ↓
   *            Token
   *              ↓
   *            Result
   *              ↓
   *    ASCII to system UTF-16 (set_result)
   *              ↓
   *     System UTF-16 result
   */
  [[nodiscard]] constexpr BOOL calc(const WPARAM wP) noexcept {
    if (LOWORD(wP) == IDC_BUTTON_CALC && HIWORD(wP) == BN_CLICKED) {
      const EditView input(_layout.handle(0));

      Normalizer to_ascii(input, _equasion);

      Token token [[indeterminate]];
      if (to_ascii.failed()) {
        token = issue(token, to_ascii.normalized(), Issue::unparsable);
      } else {
        Lexer lexer(_equasion);

        Parser parser(lexer);

        token = parser.result();
      }
      Result text [[indeterminate]];
      set_result(text.data(), result(token, text));
    }
    return TRUE;
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
  [[nodiscard]] constexpr static auto close(const HWND window) noexcept {
    return EndDialog(window, FALSE);
  }

  /**
   * Create Calc GUI.
   *
   * @warning call init befor any usage of any functionality!
   * @see init()
   */
  [[nodiscard]] constexpr static auto create(const HINSTANCE instance,
                                             DlgProc main_proc) noexcept {
    if (SUCCEEDED(DialogBoxParamA(instance, MAKEINTRESOURCEA(IDD_CALC_DIALOG),
                                  nullptr, main_proc,
                                  reinterpret_cast<LPARAM>(instance)))) {
      return EXIT_SUCCESS;
    } else {
      return EXIT_FAILURE;
    }
  }

  /**
   * Initialize Calc GUI and load user data into it.
   *
   * @warning call create first!
   * @see create()
   */
  [[nodiscard]] constexpr BOOL init(const HWND window, const LPARAM lP) {
    set_icons(window, reinterpret_cast<HINSTANCE>(lP));

    About::add_menu_to_system_menu(window);

    layout_init(window);
#ifdef CALC_SUPPORT_EXTENDENT_STYLES
    set_extended_style(_layout.handle(1), ES_EX_ALLOWEOL_ALL);
    set_extended_style(_layout.handle(0), ES_EX_ALLOWEOL_ALL);
#endif
#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
    set_text_limit(_layout.handle(0), cfg::input_max_text_length);
#endif
    // Allocate user input memory nearby
    _equasion.reserve(cfg::input_max_text_length * 4);
#ifdef CALC_TESTS_ENABLED
    gui_tests();
#endif
    load_window_data(window);

    goto_end_of_text(_layout.handle(0));

    return TRUE;
  }

  /**
   * Resize Calc window.
   */
  [[nodiscard]] constexpr BOOL resize(const LPARAM lP) noexcept {
    _layout.resize(LOWORD(lP), HIWORD(lP));
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
    // clang-format off
#ifdef CALC_SUPPORT_DPI_CHANGES
    _dpi = dpi(window);

    _layout.init_min_sizes(window, physical(cfg::min_width, _dpi), physical(cfg::min_height, _dpi));
#else
    _layout.init_min_sizes(window, cfg::min_width, cfg::min_height);
#endif
    _layout.init_window(window);
    _layout.init_anchor(window, 0, IDC_EDIT_INPUT,  Anchor::HorizontalStretch | Anchor::VerticalStretch);
    _layout.init_anchor(window, 1, IDC_EDIT_RESULT, Anchor::Right | Anchor::Bottom);
    _layout.init_anchor(window, 2, IDC_BUTTON_CALC, Anchor::Right | Anchor::Bottom);
                  static_assert(3 == cfg::elements);
    // clang-format on
  }
#ifdef CALC_SUPPORT_DARK_MODE
  [[nodiscard]] constexpr inline auto &theme() noexcept { return _theme; }
#endif
  [[nodiscard]] constexpr inline auto &about() noexcept { return _about; }

private:
#ifdef CALC_TESTS_ENABLED
  /**
   * @see normalizer_tests, Normalizer, Edit, EditView
   */
  constexpr void gui_tests() {
    std::string &output = _equasion;

    auto output_end = output.data();

    const auto start = std::chrono::steady_clock::now();
#ifdef CALC_TESTS_DEV_ENABLED // Development
    unsigned int failed = 0;
#else // Performance
    constexpr unsigned int count = 100;
    for (unsigned int i = count; --i != 0;)
#endif
    for (const auto &test : normalizer_tests) {
      { // Write UTF-8 text to the input Edit
        const auto &text = test.first;

        Edit input(_layout.handle(0), text.length());

        input.write(text.data(), text.size());
      }

      {
        // Read UTF-16 text from the input EditView
        const EditView input(_layout.handle(0));

        // Convert UTF-16 text to ASCII representation
        Normalizer normalizer(input, _equasion);

        const auto &[test_failed, test_equasion] = test.second;

        // If result unexpected:
        if (test_failed != normalizer.failed() || test_equasion != _equasion) {
          set_result(_equasion.data(), _equasion.size());
#ifdef CALC_TESTS_DEV_ENABLED // Development
          ++failed;
#endif
        }
      }
    }

    const auto end = std::chrono::steady_clock::now();
#ifdef CALC_TESTS_DEV_ENABLED // Development
    output_end = fmt::format_to(
        output_end,
        FMT_COMPILE("Tests:\n"
                    " passed: {},\n failed: {}\n"
                    " time is {}µs per case."),
        normalizer_tests.size() - static_cast<size_t>(failed), failed,
        std::chrono::duration_cast<std::chrono::microseconds>(
            (end - start) / normalizer_tests.size())
            .count());
#else // Performance
    output_end =
        fmt::format_to(output_end, FMT_COMPILE("GUI is {}µs per case."),
                       std::chrono::duration_cast<std::chrono::microseconds>(
                           (end - start) / (normalizer_tests.size() * count))
                           .count());
#endif
    set_result(output.data(), output_end);
  }
#endif
  /**
   *
   */
  constexpr void set_result(const char *text,
                            const char *text_end) const noexcept {
    set_text(_layout.handle(1), text, const_cast<char *>(text_end));
  }

  /**
   *
   */
  constexpr void set_result(const char *text, size_t size) const noexcept {
    set_result(text, text + size);
  }

  /**
   * Load Calc data from the system database
   */
  constexpr inline void load_window_data(const HWND window) {
    Edit input(_layout.handle(0), cfg::input_max_text_length);
    const RegRead reg(HKEY_CURRENT_USER, cfg::reg_key);
#ifndef CALC_TESTS_ENABLED
    input.set_size(reg.read("input", input.data(), input.size()));
#else
    const auto tests = calc_tests();
    input.write(tests.data(), static_cast<int>(tests.size()));
#endif
    const auto flags = reg.read("flags");
    const auto show = reg.read("showCmd");

    const auto left = reg.read("left");
    const auto top = reg.read("top");
    const auto right = reg.read("right");
    const auto bottom = reg.read("bottom");
#ifdef CALC_SUPPORT_DPI_CHANGES
    const auto saved_dpi = reg.read("savedDpi");
#endif
    WINDOWPLACEMENT wp [[indeterminate]];
    wp.length = sizeof(wp);
    wp.flags = static_cast<UINT>(flags ? *flags : 0);
    wp.showCmd = static_cast<UINT>(show ? *show : SW_SHOWNORMAL);

    if (left && top && right && bottom) {
      // -> use saved position
#ifdef CALC_SUPPORT_DPI_CHANGES
      if (saved_dpi) {
        // -> use with saved dpi
        const auto dpi = *saved_dpi;
        wp.rcNormalPosition.left = physical(*left, dpi);
        wp.rcNormalPosition.top = physical(*top, dpi);
        wp.rcNormalPosition.right = physical(*right, dpi);
        wp.rcNormalPosition.bottom = physical(*bottom, dpi);
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

  /**
   * Save Calc data to the system database
   */
  constexpr void save_window_data(const HWND hWnd) const noexcept {
    const RegWrite reg(HKEY_CURRENT_USER, cfg::reg_key);
#ifndef CALC_TESTS_ENABLED
    const EditView input(_layout.handle(0));
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

  /**
   * About window helpers
   */
  struct About {
    /**
     * Add "About..." menu item to system menu for window.
     */
    static constexpr void add_menu_to_system_menu(const HWND window) noexcept {
      // IDM_ABOUTBOX must be in the system command range.
      static_assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
      static_assert(IDM_ABOUTBOX < 0xF000);

      const auto system_menu = GetSystemMenu(window, FALSE);
      AppendMenuA(system_menu, MF_SEPARATOR, FALSE, nullptr);
      AppendMenuA(system_menu, MF_STRING, IDM_ABOUTBOX, "&About...");
    }

    /**
     * Create about window
     */
    [[nodiscard]] static constexpr BOOL create(const HWND parent, WPARAM wParam,
                                               DlgProc about_proc) noexcept {
      if ((wParam & 0xFFF0) == IDM_ABOUTBOX &&
          SUCCEEDED(DialogBoxParamA(GetModuleHandleA(nullptr),
                                    MAKEINTRESOURCEA(IDD_ABOUTBOX), parent,
                                    about_proc, FALSE))) {
        return TRUE;
      } else {
        return FALSE;
      }
    }
#ifdef CALC_SUPPORT_LINK_WINDOW
    /**
     * Helper to open homepage of the Calc
     */
    [[nodiscard]] static constexpr BOOL
    open_homepage(const HWND window, const LPARAM lParam) noexcept {
      const auto nm = reinterpret_cast<LPNMHDR>(lParam);
      if (nm->idFrom == IDC_LINK_HOMEPAGE && nm->code == NM_CLICK) {
        const auto l = reinterpret_cast<NMLINK *>(lParam);
        ShellExecuteW(window, L"open", l->item.szUrl, nullptr, nullptr,
                      SW_SHOWNORMAL);
        return TRUE;
      } else {
        return FALSE;
      }
    }
#endif
    /**
     * Close about window
     */
    [[nodiscard]] static constexpr BOOL close(const HWND window,
                                              const WPARAM wParam) noexcept {
      if (LOWORD(wParam) == IDCANCEL) {
        return EndDialog(window, FALSE);
      } else {
        return FALSE;
      }
    }
  };

  /**
   * Set window icons (small and big).
   */
  static constexpr void set_icons(const HWND window,
                                  const HINSTANCE app) noexcept {
    // clang-format off
    PostMessageA(window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(LoadIconA(app, MAKEINTRESOURCEA(IDR_MAINFRAME_SMALL))));
    PostMessageA(window, WM_SETICON, ICON_BIG,   reinterpret_cast<LPARAM>(LoadIconA(app, MAKEINTRESOURCEA(IDR_MAINFRAME_BIG))));
    // clang-format on
  }

  [[no_unique_address]] std::string _equasion [[indeterminate]];

  [[no_unique_address]] Layout<cfg::elements> _layout [[indeterminate]];
#ifdef CALC_SUPPORT_DARK_MODE
  [[no_unique_address]] Theme _theme [[indeterminate]];
#endif
#ifdef CALC_SUPPORT_DPI_CHANGES
  [[no_unique_address]] UINT _dpi [[indeterminate]];
#endif
  [[no_unique_address]] About _about [[indeterminate]];
};

static CalcApp gui;

/**
 * About dialog callback processing (resource-based).
 */
constexpr static INT_PTR CALLBACK about_proc(const HWND window,
                                             const UINT message,
                                             const WPARAM wParam,
                                             const LPARAM lParam) noexcept {
  switch (message) {
  case WM_COMMAND:
    return gui.about().close(window, wParam);
#ifdef CALC_SUPPORT_LINK_WINDOW
  case WM_NOTIFY:
    return gui.about().open_homepage(window, lParam);
#endif
#ifdef CALC_SUPPORT_DARK_MODE
  case WM_INITDIALOG:
    gui.theme().apply(window);
    return TRUE;
  case WM_CTLCOLORDLG:
    return gui.theme().dialog_background();
  case WM_CTLCOLORSTATIC:
    return gui.theme().static_control(wParam);
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
constexpr static INT_PTR CALLBACK main_proc(const HWND window,
                                            const UINT message,
                                            const WPARAM wParam,
                                            const LPARAM lParam) noexcept {
  switch (message) {
  case WM_COMMAND:
    return gui.calc(wParam);
  case WM_SYSCOMMAND:
    return gui.about().create(window, wParam, about_proc);
  case WM_GETMINMAXINFO:
    return gui.minmaxinfo(lParam);
  case WM_SIZE:
    return gui.resize(lParam);
  case WM_INITDIALOG:
#ifdef CALC_SUPPORT_DARK_MODE
    gui.theme().init(window);
#endif
    return gui.init(window, lParam);
#ifdef CALC_SUPPORT_AUTO_RESTART
  case WM_ENDSESSION:
    if (wParam) {
      gui.save_user_data(window);
    }
    return TRUE;
#endif
#ifdef CALC_SUPPORT_DARK_MODE
  case WM_CTLCOLORDLG:
    return gui.theme().dialog_background();
  case WM_CTLCOLOREDIT:
    return gui.theme().edit(wParam);
  case WM_CTLCOLORSTATIC:
    return gui.theme().static_control(wParam);
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
  return gui.create(instance, main_proc);
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
