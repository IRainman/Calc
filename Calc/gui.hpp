/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef GUI_HPP
#define GUI_HPP

/**
 * gui.hpp : Contains wrapers and other usable helpers to work with GUI, process
 * user input, write program output, work with system database, etc.
 */

namespace GUI {

#ifdef _WIN32

/// Explicitely enable C++ support by Windows SDK to use Unicode on systems
/// before Windows Vista or Server 2008
#define WINDOWS_ENABLE_CPLUSPLUS

/// Disable warnings about unsafe functions in Windows API
#define _CRT_SECURE_NO_WARNINGS

/// Speed up build time by excluding rarely-used stuff from Windows headers
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "targetver.h"

/**
 * Check Windows version support for Calc GUI application. The minimal supported
 * version is Windows 2000, but some features are not available in older
 * versions below Windows 10 or Server 2016.
 */

#if (_WIN32_WINNT < _WIN32_WINNT_WIN2K)
#error                                                                         \
    "Calc is the dialog based application. Windows 2000 is the minimal posiible version to compile."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WINXP)
#warning                                                                       \
    "The homepage link in the about box working from XP, before it's not working at all because system API isn't exist"
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WS03)
#warning                                                                       \
    "From Windows XP 64 bit and Server 2003 we can use increased user input, before only 32k symbols is possible."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WIN6)
#warning                                                                       \
    "Restart manager and different DPI scaling supported from Vista and Server 2008."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WIN10)
#warning                                                                       \
    "In Windows 10 and Server 2016 the HiDPI supported and Dialog based applications resized automatically when DPI changed."
#endif

#if (NTDDI_VERSION < NTDDI_WIN10_19H1)
#warning "Dark mode available from Windows 10 1903."
#endif

/**
 * Disable rarely-used stuff from Windows headers
 */

// #define NOAPISET // MultiByteToWideChar, WideCharToMultiByte
#define NODDEMLSPY
#define NO_COMMCTRL_DA
#define NOWINBASEINTERLOCK
#define NOIME
#define NORESOURCE
#define NODESKTOP
#define NOWINDOWSTATION
#define NOSECURITY
// #define NOMSG
#define NONCMESSAGES
#define NOMDI
#define NOSYSPARAMSINFO
#ifndef CALC_SUPPORT_DPI_CHANGES
#define NOWINABLE
#endif
#define NO_STATE_FLAGS

#define NOGDICAPMASKS -CC_ *, LC_ *, PC_ *, CP_ *, TC_ *, RC_ *
#define NOVIRTUALKEYCODES -VK_ *
// #define NOWINMESSAGES -WM_ *, EM_ *, LB_ *, CB_ *
#define NOWINSTYLES -WS_ *, CS_ *, ES_ *, LBS_ *, SBS_ *, CBS_ *
#define NOSYSMETRICS -SM_ *
// #define NOMENUS -MF_ *
#define NOICONS -IDI_ *
#define NOKEYSTATES -MK_ *
#define NOSYSCOMMANDS -SC_ *
#define NORASTEROPS -Binary and Tertiary raster ops
// #define NOSHOWWINDOW -SW_ *
#define OEMRESOURCE -OEM Resource values
#define NOATOM -Atom Manager routines
#define NOCLIPBOARD -Clipboard routines
#ifndef CALC_SUPPORT_DARK_MODE
#define NOCOLOR -Screen colors
#endif
//  #define NOCTLMGR -Control and Dialog routines
#define NODRAWTEXT -DrawText() and DT_ *
#if !defined(CALC_SUPPORT_DARK_MODE) && !defined(CALC_SUPPORT_DPI_CHANGES)
#define NOGDI -All GDI defines and routines
#endif
#define NOKERNEL -All KERNEL defines and routines
// #define NOUSER -All USER defines and routines
// #define NONLS -All NLS defines and routines
#define NOMB -MB_ *and MessageBox()
#define NOMEMMGR -GMEM_ *, LMEM_ *, GHND, LHND, associated routines
#define NOMETAFILE -typedef METAFILEPICT
#define NOMINMAX -Macros min(a, b) and max(a, b)
// #define NOMSG -typedef MSG and associated routines
#define NOOPENFILE -OpenFile(), OemToAnsi, AnsiToOem, and OF_ *
#define NOSCROLL -SB_ *and scrolling routines
#define NOSERVICE -All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND -Sound driver routines
#define NOTEXTMETRIC -typedef TEXTMETRIC and associated routines
#define NOWH -SetWindowsHook and WH_ *
// #define NOWINOFFSETS -GWL_ *, GCL_ *, associated routines
#define NOCOMM -COMM driver routines
#define NOKANJI -Kanji support stuff.
#define NOHELP -Help engine interface.
#define NOPROFILER -Profiler interface.
// #define NODEFERWINDOWPOS -DeferWindowPos routines
#define NOMCX -Modem Configuration Extensions

#define NOTOOLBAR Customizable bitmap - button toolbar control.
#define NOUPDOWN Up and Down arrow increment / decrement control.
#define NOSTATUSBAR Status bar control.
#define NOMENUHELP APIs to help manage menus, especially with a status bar.
#define NOTRACKBAR Customizable column - width tracking control.
#define NODRAGLIST APIs to make a listbox source and sink drag &drop actions.
#define NOPROGRESS Progress gas gauge.
#define NOHOTKEY HotKey control
#define NOHEADER Header bar control.
#ifndef CALC_SUPPORT_DARK_MODE
#define NOIMAGEAPIS ImageList apis.
#endif
#define NOLISTVIEW ListView control.
#define NOTREEVIEW TreeView control.
#define NOTABCONTROL Tab control.
#define NOANIMATE Animate control.
#define NOBUTTON Button control.
#define NOSTATIC Static control.
#ifndef CALC_SUPPORT_EXTENDENT_STYLES
#define NOEDIT Edit control.
#endif
#define NOLISTBOX Listbox control.
#define NOCOMBOBOX Combobox control.
#define NOSCROLLBAR Scrollbar control.
#define NOTASKDIALOG Task Dialog.

#define NOMUI
#define NOTRACKMOUSEEVENT
#define NOFLATSBAPIS
#define NONATIVEFONTCTL
#define NOPAGESCROLLER
#define NOIPADDRESS
#define NODATETIMEPICK
#define NOMONTHCAL
#define NOUSEREXCONTROLS
#define NOTOOLTIPS
#define NOREBAR

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

/**
 * @brief Enable CRT debug leak detection in debug builds so leaking allocations
 *        are reported with call stacks at process exit. This mirrors the usual
 *        pattern of calling _CrtSetDbgFlag and _CrtSetReportMode early in
 *        startup.
 *
 * @warning Should be called before WinMain!
 */
static void __cdecl _setup_crt_leak_check() noexcept {
  // Report to output window and perform leak check at exit.
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

  flags |= _CRTDBG_ALLOC_MEM_DF;  // Turn on debug allocation
  flags |= _CRTDBG_LEAK_CHECK_DF; // Perform leak check at program exit

  // Don't enable _CRTDBG_CHECK_CRT_DF here!

  _CrtSetDbgFlag(flags);

  // Ensure reports go to debugger output
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
}
#endif

// Disable warnings for GUI code:
// clang-format off
#pragma warning(disable : 4267) // 'argument': conversion from 'type_x' to 'type_y', possible loss of data
#pragma warning(disable : 4865) // the underlying type of 'x' will change when '/Zc:enumTypes' is specified on the command line
#pragma warning(disable : 5039) // 'x': pointer or reference to potentially throwing function passed to 'extern "C"' function under -EHc. Undefined behavior may occur if this function throws an exception.
#pragma warning(disable : 4820) // 'x': 'n' bytes padding added after data member 'y'
// clang-format on
#include <windows.h>
#ifdef CALC_SUPPORT_LINK_WINDOW
#include <commctrl.h>
#include <shellapi.h>
#endif
#ifdef CALC_SUPPORT_DARK_MODE
#include <dwmapi.h>
#include <uxtheme.h>
#endif

/**
 * Dialog procedure type.
 */
typedef INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM) noexcept;

#ifdef CALC_SUPPORT_EXTENDENT_STYLES
/**
 * Set window extended styles.
 */
constexpr static void set_extended_style(const HWND window,
                                         const DWORD style) noexcept {
  PostMessageA(window, EM_SETEXTENDEDSTYLE, static_cast<WPARAM>(style),
               static_cast<LPARAM>(style));
}
#endif

/**
 * Set text to window. Shoul be zero terminated!
 * @see Edit
 */
constexpr static void set_text(const HWND window, LPCSTR text) noexcept {
  SetWindowTextA(window, text);
}

/**
 * Set text to window (end is for C string terminator).
 * @see Edit
 */
constexpr static void set_text(const HWND window, LPCSTR text,
                               LPSTR const text_end) noexcept {
  *text_end = '\0'; // because of C string
  set_text(window, text);
}

#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
/**
 * Set window text limit
 * @see Edit
 */
constexpr static void set_text_limit(const HWND window,
                                     WPARAM max_symbols) noexcept {
  PostMessageA(window, EM_LIMITTEXT, max_symbols, 0);
}
#endif

/**
 * Goto end of text in window and scroll caret to it
 */
constexpr static void goto_end_of_text(const HWND window) noexcept {
  PostMessageA(window, EM_SETSEL, static_cast<WPARAM>(0),
               static_cast<LPARAM>(-1));
  PostMessageA(window, EM_SCROLLCARET, 0, 0);
}

/**
 * @brief GUI helper to use Edit control.
 *
 * @warning This can'be usable only after Edit called one time!
 * @warning This can'be usable only if Edit is multiline and for Dialog
 *          local edit option is enabled.
 * @see Edit
 */
class EditView {
public:
  EditView() = delete;
  EditView(EditView const &) = delete;
  EditView &operator=(EditView const &) = delete;

  constexpr explicit EditView(const HWND edit) noexcept
      : _length(static_cast<UINT>(SendMessageA(edit, WM_GETTEXTLENGTH, 0, 0))) {
    assert(_length);
    _handle = reinterpret_cast<HLOCAL>(SendMessageA(edit, EM_GETHANDLE, 0, 0));
    _text = static_cast<LPCWSTR>(LocalLock(_handle));
    assert(_handle && _text);
  }

  constexpr ~EditView() noexcept { LocalUnlock(_handle); }

  /**
   * @return Length in UTF-16 characters of the text.
   * @see text()
   */
  [[nodiscard]] constexpr auto length() const noexcept { return _length; }

  /**
   * @return Pointer (LPCWSTR) to the UTF-16 text.
   * @see length()
   */
  [[nodiscard]] constexpr auto text() const noexcept { return _text; }

  /**
   * @return UTF-8 text to the ouput paramether.
   */
  constexpr void read(std::string &output) const noexcept {
    assert(_length <= output.capacity());
    output.resize(output.capacity());
    output.resize(WideCharToMultiByte(CP_UTF8, 0, text(), length(),
                                      output.data(), output.capacity(), NULL,
                                      NULL));
  }

  /**
   * @return Size in bytes of the data.
   * @see data()
   */
  [[nodiscard]] constexpr auto size() const noexcept {
    return static_cast<UINT>(_length * sizeof(WCHAR));
  }

  /**
   * @return BYTE pointer to the the data.
   * @see size()
   */
  [[nodiscard]] constexpr auto data() const noexcept {
    return reinterpret_cast<const BYTE *>(_text);
  }

private:
  [[no_unique_address]] HLOCAL _handle;
  [[no_unique_address]] LPCWSTR _text;
  [[no_unique_address]] const UINT _length;
};

/**
 * @brief GUI helper to use Edit control.
 * @warning This relocate memory.
 * @warning This can'be usable only if Edit is multiline and for Dialog
 *          local edit option is enabled.
 * @see EditView
 */
class Edit {
public:
  Edit() = delete;
  Edit(Edit const &) = delete;
  Edit &operator=(Edit const &) = delete;

  constexpr explicit Edit(const HWND edit, const UINT max_length) noexcept
      : _edit(edit), _max_size(max_length * sizeof(WCHAR)) {
    _handle = reinterpret_cast<HLOCAL>(SendMessageA(edit, EM_GETHANDLE, 0, 0));
    assert(max_length);
    assert(_handle);
    _handle = LocalReAlloc(_handle, _max_size, LMEM_MOVEABLE);
    assert(_handle);
    _text = static_cast<LPWSTR>(LocalLock(_handle));
    assert(_text);
  }

  constexpr ~Edit() noexcept {
    LocalUnlock(_handle);
    SendMessageA(_edit, EM_SETHANDLE, reinterpret_cast<WPARAM>(_handle), 0);
  }

  /**
   * @return Length in UTF-16 characters of the text.
   * @see text()
   */
  [[nodiscard]] constexpr UINT length() const noexcept {
    return _max_size / sizeof(WCHAR);
  }

  /**
   * @return Pointer (LPWSTR) to the UTF-16 text.
   * @see length()
   */
  [[nodiscard]] constexpr auto text() noexcept { return _text; }

  /**
   * Set end of the text.
   * @see text(), length()
   */
  constexpr void set_text_end(LPWSTR end) noexcept {
    assert(end >= _text && UINT(end - _text) <= length());
    *end = L'\0'; // because of C string
  }

  /**
   * Set length in characters of the text.
   * @see text(), length()
   */
  constexpr void set_length(const UINT len) noexcept {
    assert(len <= length());
    set_text_end(_text + len);
  }

  /**
   * Write UTF-8 text.
   */
  constexpr void write(const char *text, const int len) noexcept {
    assert(unsigned(len) <= length());
    set_length(MultiByteToWideChar(CP_UTF8, 0, text, len, _text, len));
  }

  /**
   * Write UTF-8 text.
   */
  constexpr void write(const char *text, const char *text_end) noexcept {
    write(text, static_cast<UINT>(text_end - text));
  }

  /**
   * @return UTF-8 text to the ouput paramether.
   */
  constexpr auto &read(std::string &output) noexcept {
    assert(length() <= output.capacity());
    output.resize(output.capacity());
    output.resize(WideCharToMultiByte(CP_UTF8, 0, text(), length(),
                                      output.data(), output.capacity(), NULL,
                                      NULL));
    return output;
  }

  /**
   * @return Size in bytes of the data.
   * @see data()
   */
  [[nodiscard]] constexpr auto size() const noexcept { return _max_size; }

  /**
   * @return BYTE pointer to the the data.
   * @see size()
   */
  [[nodiscard]] constexpr auto data() noexcept {
    return reinterpret_cast<BYTE *>(_text);
  }

  /**
   * Set size in bytes of the data.
   * @see data(), size()
   */
  constexpr void set_size(const UINT size) noexcept {
    set_length(size / sizeof(WCHAR));
  }

private:
  [[no_unique_address]] const HWND _edit;
  [[no_unique_address]] HLOCAL _handle [[indeterminate]];
  [[no_unique_address]] LPWSTR _text [[indeterminate]];
  [[no_unique_address]] const UINT _max_size;
};

/**
 * Helper to write data to system database.
 */
struct RegWrite {
  RegWrite() = delete;
  RegWrite(const RegWrite &) = delete;
  RegWrite(RegWrite &&) = delete;

  constexpr explicit RegWrite(const HKEY root, const char *subkey) noexcept {
    RegCreateKeyExA(root, subkey, FALSE, nullptr, REG_OPTION_NON_VOLATILE,
                    KEY_WRITE, nullptr, &key, nullptr);
  }

  constexpr ~RegWrite() noexcept { RegCloseKey(key); }

  constexpr void write(const char *name, const DWORD value) const noexcept {
    RegSetValueExA(key, name, FALSE, REG_DWORD,
                   reinterpret_cast<const BYTE *>(&value), sizeof(value));
  }

  constexpr void write(const char *name, const BYTE *data,
                       const DWORD size) const noexcept {
    RegSetValueExA(key, name, FALSE, REG_BINARY, data, size);
  }

private:
  HKEY key;
};

/**
 * Helper for read data from system database.
 */
struct RegRead {
  RegRead() = delete;
  RegRead(const RegRead &) = delete;
  RegRead(RegRead &&) = delete;

  constexpr explicit RegRead(const HKEY root, const char *subkey) noexcept {
    RegOpenKeyExA(root, subkey, FALSE, KEY_READ, &key);
  }

  constexpr ~RegRead() noexcept { RegCloseKey(key); }

  [[nodiscard]] constexpr std::optional<DWORD>
  read(const char *name) const noexcept {
    DWORD type [[indeterminate]];
    DWORD out [[indeterminate]];
    DWORD outSize = sizeof(out);
    if (RegQueryValueExA(key, name, nullptr, &type,
                         reinterpret_cast<LPBYTE>(&out),
                         &outSize) == ERROR_SUCCESS) {
      if (type == REG_DWORD) {
        return out;
      }
    }
    return std::nullopt;
  }

  [[nodiscard]] constexpr UINT read(const char *name, LPBYTE out,
                                    DWORD out_size) const noexcept {
    DWORD type [[indeterminate]];
    if (RegQueryValueExA(key, name, nullptr, &type, out, &out_size) ==
        ERROR_SUCCESS) {
      if (type == REG_BINARY) {
        return static_cast<UINT>(out_size);
      }
    }
    return 0;
  }

private:
  HKEY key;
};

#ifdef CALC_SUPPORT_DPI_CHANGES
/**
 * @return dpi for window
 */
[[nodiscard]] constexpr static UINT dpi(const HWND window) noexcept {
#ifdef CALC_SUPPORT_DPI_FOR_WINDOW
  // Use per-window DPI
  return GetDpiForWindow(window);
#else
  // Get device caps from device context.
  const auto dc = GetDC(window);
  const auto dpiY = GetDeviceCaps(dc, LOGPIXELSY);
  ReleaseDC(window, dc);
  return dpiY;
#endif
}

/**
 * Convert coordinates from logical to physical.
 * @return physical coordiantes.
 */
[[nodiscard]] constexpr static LONG physical(LONG logical, LONG dpi) noexcept {
  return std::lroundf(static_cast<float>(logical) * static_cast<float>(dpi) /
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI));
}

/**
 * Convert coordinates from physical to logical.
 * @return logical coordiantes.
 */
[[nodiscard]] constexpr static LONG logical(LONG physical, LONG dpi) noexcept {
  return std::lroundf(static_cast<float>(physical) *
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI) /
                      static_cast<float>(dpi));
};
#endif

#ifdef CALC_SUPPORT_DARK_MODE
// clang-format off
/**
 * Dynamic colors API for Windows 10 1903+.
 *
 * Uses the undocumented uxtheme exports:
 *   #104 RefreshImmersiveColorPolicyState
 *   #135 SetPreferredAppMode
 *   #136 FlushMenuThemes
 *
 * And define PreferredAppMode used by SetPreferredAppMode.
 *
 * The menu itself remains owned by system. No owner-draw, no WM_DRAWITEM,
 * and no custom menu colors are required.
 */
enum class PreferredAppMode : int {
  Default = 0,
  AllowDark = 1,
  ForceDark = 2,
  ForceLight = 3,
  Max = 4
};

using SetPreferredAppModeFn = PreferredAppMode(WINAPI *)(PreferredAppMode) noexcept;
static SetPreferredAppModeFn SetPreferredAppMode [[indeterminate]];

using RefreshImmersiveColorPolicyStateFn = void(WINAPI *)() noexcept;
static RefreshImmersiveColorPolicyStateFn RefreshImmersiveColorPolicyState [[indeterminate]];

using FlushMenuThemesFn = void(WINAPI *)() noexcept;
static FlushMenuThemesFn FlushMenuThemes [[indeterminate]];
// clang-format on

/**
 * Init theming: get real adresses of uxtheme functions.
 *
 * @warning Should be called before WinMain!
 */
constexpr static void init_uxtheme_callers() noexcept {
  auto uxtheme = GetModuleHandleA("uxtheme.dll");
  // clang-format off
#pragma warning(push)
#pragma warning(disable : 4191) // allow FARPROC -> function pointer casts here
  RefreshImmersiveColorPolicyState = reinterpret_cast<RefreshImmersiveColorPolicyStateFn>(
      GetProcAddress(uxtheme, MAKEINTRESOURCEA(104)));
  SetPreferredAppMode = reinterpret_cast<SetPreferredAppModeFn>(
      GetProcAddress(uxtheme, MAKEINTRESOURCEA(135)));
  FlushMenuThemes = reinterpret_cast<FlushMenuThemesFn>(
      GetProcAddress(uxtheme, MAKEINTRESOURCEA(136)));
#pragma warning(pop)
  // clang-format on
}

/**
 * Application theme management.
 *
 * Provides a comprehensive theme management system for the calculator
 * application, supporting both light and dark modes. The theme system
 * automatically detects the user's Windows theme preference and applies
 * consistent styling to all UI elements, including the application window,
 * dialogs, menus, and interactive controls.
 *
 * Features:
 * - Automatic Windows theme detection (light/dark mode)
 * - Theme application to BUTTON, SCROLLBAR, COMBOBOX, LISTBOX, LISTVIEW,
 *   TREEVIEW, TAB, PROGRESSBAR, and TRACKBAR controls
 * - Customizable brushes for background and field colors
 * - Immersive dark mode support for title bars and application frames
 *
 * Usage:
 *   Theme theme;
 *   theme.init(application_main_window); // Initialize before any windows are
 *                                        // created
 *   theme.apply(some_window); // Apply theme to a window
 */
struct Theme {
  /// Dark window background
  static constexpr auto _dark_bakground_window_color = RGB(32, 32, 32);
  /// Slightly lighter gray than window for fields
  static constexpr auto _dark_bakground_field_color = RGB(45, 45, 45);
  /// Slightly darker text than pure light
  static constexpr auto _dark_bakground_text_color = RGB(240, 240, 240);

  [[no_unique_address]] const HBRUSH _dark_bakground_window_brush =
      CreateSolidBrush(_dark_bakground_window_color);
  [[no_unique_address]] const HBRUSH _dark_bakground_field_brush =
      CreateSolidBrush(_dark_bakground_field_color);

  /**
   * Initializes the application theme system.
   *
   * Must be called before any window is initialized. This function:
   * - Sets the application's preferred appearance mode to allow dark themes
   * - Applies the theme to the main application window
   *
   * @param application_main_window The HWND of the main application window.
   * @warning Must be called during application initialization, before window
   *          creation.
   */
  constexpr void init(const HWND application_main_window) noexcept {
    SetPreferredAppMode(PreferredAppMode::AllowDark);
    apply(application_main_window, false, true);
  }

  /**
   * Applies the theme to a window and all its child controls.
   *
   * Applies theme to application title bar, application frame, menus, and for
   * interactive controls that repaints itself based on its current state.
   * Supported controls: BUTTON, SCROLLBAR, COMBOBOX, LISTBOX, LISTVIEW,
   * TREEVIEW, TAB, PROGRESSBAR, TRACKBAR.
   *
   * @param window The HWND of the window to theme.
   * @param redraw If true, redraws the window and all child windows after
   *               applying the theme. Default is false.
   * @param is_main_window If true, detects the system's dark mode preference
   *                       from Windows Registry and applies it. Only set to
   *                       true for the main application window. Default is
   *                       false.
   *
   * @note Child windows are automatically enumerated and themed recursively.
   * @see dialog_background(), static_control(), edit()
   */
  constexpr void apply(const HWND window, const bool redraw = false,
                       const bool is_main_window = false) noexcept {
    theme(window, is_dark_mode(is_main_window));
    EnumChildWindows(window, theme, is_dark_mode());
    menus();
    title_bar_and_frame(window);
    if (redraw) {
      RedrawWindow(window, nullptr, nullptr,
                   RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    }
  }

  /**
   * Gets the appropriate background brush for DIALOG controls.
   *
   * Returns the correct brush based on the current theme (light or dark mode).
   * In dark mode, returns a dark background brush; in light mode, returns the
   * system default window background brush.
   *
   * @return An INT_PTR representing the HBRUSH to use for dialog backgrounds.
   * @note Use this in WM_CTLCOLORDLG message handling to properly theme
   * dialogs.
   */
  [[nodiscard]] constexpr INT_PTR dialog_background() const noexcept {
    return reinterpret_cast<INT_PTR>(is_dark_mode()
                                         ? _dark_bakground_window_brush
                                         : GetSysColorBrush(COLOR_WINDOW));
  }

  /**
   * Applies theme to STATIC control.
   *
   * Configures the text and background colors for static controls based on
   * the current theme. In dark mode, uses custom dark theme colors; in light
   * mode, uses system default colors.
   *
   * @param wPhdc The WPARAM value from WM_CTLCOLORSTATIC, which contains the
   *              device context handle (HBRUSH cast to WPARAM).
   * @return An INT_PTR representing the HBRUSH to use for the control
   * background.
   * @note Use this in WM_CTLCOLORSTATIC message handling. The returned brush
   *       paints the static control background, while text and background
   * colors are set on the provided device context.
   * @see _dark_bakground_text_color, _dark_bakground_window_color
   */
  [[nodiscard]] constexpr INT_PTR
  static_control(const WPARAM wPhdc) const noexcept {
    auto hdc = reinterpret_cast<HDC>(wPhdc);
    if (is_dark_mode()) {
      SetTextColor(hdc, _dark_bakground_text_color);
      SetBkColor(hdc, _dark_bakground_window_color);
      return reinterpret_cast<INT_PTR>(_dark_bakground_window_brush);
    } else {
      SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
      SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
      return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOW));
    }
  }

  /**
   * Applies theme to EDIT controls.
   *
   * Configures the text and background colors for edit controls based on
   * the current theme. In dark mode, uses custom dark theme colors for input
   * fields. In light mode, defers to default system behavior.
   *
   * @param wPhdc The WPARAM value from WM_CTLCOLOREDIT, which contains the
   *              device context handle (HBRUSH cast to WPARAM).
   * @return An INT_PTR representing the HBRUSH to use for the control
   * background, or FALSE to use default light mode behavior.
   * @note Use this in WM_CTLCOLOREDIT message handling (e.g., for text input
   *       fields in the calculator display).
   * @see _dark_bakground_text_color, _dark_bakground_field_color
   */
  [[nodiscard]] constexpr INT_PTR edit(const WPARAM wPhdc) const noexcept {
    if (is_dark_mode()) {
      auto hdc = reinterpret_cast<HDC>(wPhdc);
      SetTextColor(hdc, _dark_bakground_text_color);
      SetBkColor(hdc, _dark_bakground_field_color);
      return reinterpret_cast<INT_PTR>(_dark_bakground_field_brush);
    } else {
      // Fall back to default light behavior
      return FALSE;
    }
  }

private:
  /**
   * Applies theme styling to a window via SetWindowTheme.
   *
   * Callback function used with EnumChildWindows to recursively apply the
   * theme to all child windows. Sets the window theme based on whether dark
   * mode is enabled, using either "DarkMode_Explorer" or "Explorer" theme.
   *
   * @param window The HWND of the window to theme.
   * @param dark The LPARAM indicating whether dark mode is enabled (non-zero
   *             for dark, zero for light).
   * @return TRUE to continue enumeration of child windows.
   * @note This is a static callback function for use with EnumChildWindows().
   */
  constexpr static BOOL CALLBACK theme(const HWND window,
                                       const LPARAM dark) noexcept {
    SetWindowTheme(window, dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
    return TRUE;
  }

  /**
   * Checks whether dark mode is currently enabled.
   *
   * Returns the cached dark mode setting for the application.
   *
   * @return true if dark mode is enabled, false otherwise.
   * @note This reflects the cached state; use is_dark_mode(is_main_window)
   *       to detect the current system preference.
   */
  [[nodiscard]] constexpr bool is_dark_mode() const noexcept {
    return _dark_mode;
  }

  /**
   * Determines if dark mode should be used (system preference detection).
   *
   * For the main application window, reads the Windows Registry to detect the
   * user's preferred application theme and caches the result in _dark_mode.
   * For other windows, this function is a no-op and returns the cached value.
   *
   * @param is_main_window If true, reads the Windows Personalize registry key
   *                       to detect the user's dark mode preference
   *                       (HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\
   *                       CurrentVersion\\Themes\\Personalize\\AppsUseLightTheme).
   *                       If false, uses the cached _dark_mode value.
   * @return true if dark mode is enabled, false for light mode.
   * @note Registry value: AppsUseLightTheme = 0 means dark mode is enabled,
   *       non-zero means light mode is enabled.
   * @see init()
   */
  [[nodiscard]] constexpr bool
  is_dark_mode(const bool is_main_window) noexcept {
    if (is_main_window) {
      // clang-format off
      const RegRead personalize(HKEY_CURRENT_USER,
                  "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");
      const auto light = personalize.read("AppsUseLightTheme");
      _dark_mode = light && *light == 0;
      // clang-format on
    }
    return is_dark_mode();
  }

  /**
   * Applies dark mode theme to the application title bar and frame.
   *
   * Uses the Windows Desktop Window Manager (DWM) API to apply immersive dark
   * mode styling to the window's title bar and frame, creating a cohesive dark
   * theme appearance.
   *
   * @param window The HWND of the window to apply title bar theming to.
   * @note This uses DwmSetWindowAttribute with DWMWA_USE_IMMERSIVE_DARK_MODE
   *       to enable Windows 10 1903+ native dark mode for window frames.
   */
  constexpr void title_bar_and_frame(const HWND window) const noexcept {
    BOOL value = is_dark_mode() ? TRUE : FALSE;
    DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &value,
                          sizeof(value));
  }

  /**
   * Refreshes and applies dark mode theme to menus.
   *
   * Updates the application's menu appearance to match the current theme by
   * refreshing the immersive color policy state and flushing menu themes.
   *
   * @note Typically called as part of the theme application process to ensure
   *       menus reflect the current dark/light mode setting.
   */
  constexpr static void menus() noexcept {
    RefreshImmersiveColorPolicyState();
    FlushMenuThemes();
  }

  /**
   * Cached dark mode setting for the application.
   *
   * Stores whether dark mode is currently enabled. This value is determined
   * from the Windows Registry during initialization (in is_dark_mode with
   * is_main_window=true) and reused for performance to reduce registry reads.
   *
   * @note Marked with [[indeterminate]] to indicate the value is not
   * initialized until is_dark_mode(true) is called during init().
   */
  [[no_unique_address]] bool _dark_mode [[indeterminate]];
};
#endif

/**
 * Helper to work with points in window layout
 */
struct Point : tagPOINT {
  constexpr Point() noexcept = default;

  constexpr Point(const LONG _x, const LONG _y) noexcept {
    x = _x;
    y = _y;
  }
};

/**
 * Helper to work with rectangles in window layout
 */
struct Rect : tagRECT {
  constexpr Rect() noexcept = default;

  Rect(const Rect &) = delete;

  constexpr Rect(const LONG x, const LONG y, const LONG width,
                 const LONG heigth) noexcept {
    left = x;
    right = x + width;
    top = y;
    bottom = y + heigth;
  }

  [[nodiscard]] constexpr auto x() const noexcept { return left; }

  [[nodiscard]] constexpr auto y() const noexcept { return top; }

  [[nodiscard]] constexpr auto width() const noexcept { return right - left; }

  [[nodiscard]] constexpr auto height() const noexcept { return bottom - top; }
};

/**
 * Helper for resize Layout
 */
enum class Anchor : BYTE {
  None = 0,
  HorizontalStretch = 1 << 0,
  Left = 1 << 1,
  Right = 1 << 2,
  VerticalStretch = 1 << 3,
  Top = 1 << 4,
  Bottom = 1 << 5
};

/// Bitwise OR for Anchor
constexpr static Anchor operator|(Anchor lhs, Anchor rhs) noexcept {
  using T = std::underlying_type_t<Anchor>;
  return static_cast<Anchor>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

/// Bitwise AND for Anchor
constexpr static Anchor operator&(Anchor lhs, Anchor rhs) noexcept {
  using T = std::underlying_type_t<Anchor>;
  return static_cast<Anchor>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

/// Helper to check if a flag is set for Anchor
constexpr static bool has_flag(Anchor value, Anchor flag) noexcept {
  return static_cast<bool>(value & flag);
}

/**
 * @class Layout
 * @brief Template-based dynamic window layout manager with anchor-based
 * resizing.
 *
 * @tparam elements Compile-time constant specifying the number of child
 * controls managed by this layout. This determines the fixed size of all
 *                  internal arrays (handles, widths, heights, margins,
 *                  anchors).
 *
 * A sophisticated layout management system for Windows dialog-based
 * applications that automatically handles the repositioning and resizing of
 * child controls when the parent window is resized. The system uses an
 * anchor-based approach similar to WinForms and other UI frameworks, where each
 * control can specify how it should adapt to size changes.
 *
 * **Key Features:**
 * - **Anchor-based positioning**: Each control specifies anchor constraints
 * that determine how it moves and resizes relative to window edges
 * - **Deferred window updates**: Uses Windows BeginDeferWindowPos /
 * DeferWindowPos / EndDeferWindowPos for efficient batched window updates
 * - **Compile-time constexpr support**: All initialization methods are
 * constexpr, allowing compile-time configuration when possible
 * - **Minimal memory overhead**: Uses [[no_unique_address]] to minimize memory
 *   footprint of layout constraints
 * - **Minimum size enforcement**: Supports minimum window dimensions to prevent
 *   controls from becoming too small
 *
 * **Anchor Behaviors:**
 * The Anchor enum flags control how a control responds to parent window
 * resizes:
 *
 *   - **Left**: Control's left edge stays fixed distance from parent's left
 *               edge
 *   - **Right**: Control's right edge stays fixed distance from parent's right
 *                edge
 *   - **HorizontalStretch**: Control stretches/shrinks horizontally with the
 *                            window
 *
 *   - **Top**: Control's top edge stays fixed distance from parent's top edge
 *   - **Bottom**: Control's bottom edge stays fixed distance from parent's
 *                 bottom edge
 *   - **VerticalStretch**: Control stretches/shrinks vertically with the window
 *   - **None**: Control does not resize or move with the window
 *
 * **Usage Example:**
 * ```cpp
 * // Create a layout for a dialog with 3 controls
 * Layout<3> layout;
 *
 * // Initialize with the main dialog window
 * layout.init_window(dialog_hwnd);
 * layout.init_min_sizes(400, 300); // Minimum 400x300 pixels
 *
 * // Register controls with their resize behavior
 * layout.init_anchor(dialog_hwnd, 0, IDC_EDIT_MAIN,
 *                    Anchor::Left | Anchor::Top | Anchor::HorizontalStretch);
 * layout.init_anchor(dialog_hwnd, 1, IDC_BTN_OK,
 *                    Anchor::Right | Anchor::Bottom);
 * layout.init_anchor(dialog_hwnd, 2, IDC_LIST_RESULTS,
 *                    Anchor::Left | Anchor::Top | Anchor::HorizontalStretch
 *                    | Anchor::VerticalStretch);
 *
 * // Handle WM_SIZE in dialog procedure
 * case WM_SIZE:
 *     layout.resize(LOWORD(lParam), HIWORD(lParam));
 *     break;
 * ```
 *
 * @note Non-copyable and non-move-constructible for safety
 * @note All initialization must occur before the first resize call
 * @see Anchor, Constraints, Point, Rect
 */
template <typename BYTE elements> class Layout {
public:
  /**
   * @brief Default constructor.
   *
   * Creates an uninitialized layout. Must call init_window(), init_min_sizes(),
   * and multiple init_anchor() calls before using resize().
   */
  constexpr Layout() noexcept = default;

  Layout(Layout const &) = delete;
  Layout &operator=(Layout const &) = delete;
  Layout(Layout &&) = delete;

protected:
  /**
   * @struct Constraints
   * @brief Stores sizing and positioning constraints for all managed controls.
   *
   * This nested structure maintains the complete set of constraints needed to
   * properly reposition and resize each child control. It uses fixed-size
   * arrays indexed by control index (0 to elements-1).
   *
   * **Data Members:**
   * - handles: Array of HWND for each managed child control
   * - widths: Array of original widths for each control
   * - heights: Array of original heights for each control
   * - margins: Array of margin information (distance from edges) for each
   *            control
   * - anchors: Array of Anchor flags specifying resize behavior for each
   *            control
   *
   * @tparam elements Number of controls managed by the parent Layout
   */
  template <typename BYTE elements> struct Constraints {
    /**
     * @brief Initializes constraints for a single control.
     *
     * Called by Layout::init_anchor() to capture the current size and position
     * of a control, along with its anchor settings. The margins are calculated
     * as the distance from each edge of the client area.
     *
     * **Calculation Details:**
     * - left margin = control's left position relative to client origin
     * - right margin = (layout width) - (control's right edge)
     * - top margin = control's top position relative to client origin
     * - bottom margin = (layout height) - (control's bottom edge)
     *
     * These margins form the basis for all future resize calculations.
     *
     * @param layout Pointer to the parent Layout instance (for querying
     * dimensions)
     * @param index Zero-based index in the constraints arrays [0, elements)
     * @param handle The HWND of the child control being registered
     * @param client Reference to Rect containing the control's current position
     *               and size in client coordinates relative to the parent
     * @param anchor Anchor flags specifying how this control should resize
     *
     * @note This method should only be called by Layout::init_anchor()
     */
    constexpr void init(const Layout *layout, const BYTE index,
                        const HWND handle, const Rect &client,
                        const Anchor anchor) noexcept {
      handles[index] = handle;

      margins[index].left = client.left;
      margins[index].right = layout->width() - client.right;

      margins[index].top = client.top;
      margins[index].bottom = layout->height() - client.bottom;

      widths[index] = client.width();
      heights[index] = client.height();

      anchors[index] = anchor;
    }

    // clang-format off
    /// Window handles for all managed controls
    [[no_unique_address]] std::array<HWND, elements> handles [[indeterminate]];
    
    /// Original widths of all controls
    [[no_unique_address]] std::array<LONG, elements> widths [[indeterminate]];
    
    /// Original heights of all controls
    [[no_unique_address]] std::array<LONG, elements> heights [[indeterminate]];
    
    /// Margin information (distances from edges) for all controls
    [[no_unique_address]] std::array<Rect, elements> margins [[indeterminate]];
    
    /// Anchor flags specifying resize behavior for all controls
    [[no_unique_address]] std::array<Anchor, elements> anchors [[indeterminate]];
    // clang-format on
  };

public:
  /**
   * @brief Initializes the layout with the current window dimensions.
   *
   * Retrieves the client area rectangle of the specified window and stores its
   * dimensions. This establishes the baseline size against which future resize
   * operations are calculated. Should be called before registering any
   * controls.
   *
   * @param window The HWND of the parent window whose client area dimensions
   *               will be captured
   *
   * @post _width and _height contain the current client dimensions
   * @note Call this before init_anchor() and resize()
   */
  constexpr void init_window(const HWND window) noexcept {
    Rect client [[indeterminate]];
    GetClientRect(window, &client);
    _width = client.right;
    _height = client.bottom;
  }

  /**
   * @brief Sets the minimum allowable window dimensions.
   *
   * Specifies the smallest size to which the window can be resized. When
   * calculating new control positions during a resize operation, these minimums
   * can be used to prevent controls from becoming too small or overlapping.
   *
   * @param window The HWND of the parent dialog window
   * @param min_width Minimum width in pixels (typically used in
   * WM_GETMINMAXINFO)
   * @param min_height Minimum height in pixels (typically used in
   * WM_GETMINMAXINFO)
   *
   * @post _min_width and _min_height are set for later reference
   * @note These values are stored but not enforced by the Layout class;
   *       they are typically used by the dialog procedure for WM_GETMINMAXINFO
   */
  constexpr void init_min_sizes(const HWND window, const LONG min_width,
                                const LONG min_height) noexcept {
    Rect client(0, 0, min_width, min_height);
    const auto style = static_cast<DWORD>(GetWindowLongPtrA(window, GWL_STYLE));
    const auto exStyle =
        static_cast<DWORD>(GetWindowLongPtrA(window, GWL_EXSTYLE));
    AdjustWindowRectEx(&client, style, FALSE, exStyle);
    _min_width = min_width;
    _min_height = min_height;
  }

  /**
   * @brief Registers a child control with its anchor constraints.
   *
   * Captures the current size and position of a dialog control and registers it
   * with the layout manager under the specified index. The control's position
   * and size are recorded relative to the parent window's client area, along
   * with its anchor settings that determine how it should resize.
   *
   * **Processing Steps:**
   * 1. Retrieves the control's HWND using GetDlgItem()
   * 2. Gets its screen coordinates via GetWindowRect()
   * 3. Converts screen coordinates to client coordinates of the parent
   * 4. Calls Constraints::init() to store margins and dimensions
   *
   * @param parent The HWND of the parent dialog window
   * @param index Zero-based index for this control in the constraints arrays
   *              Must be < elements
   * @param id The dialog control ID (passed to GetDlgItem)
   * @param anchor Combination of Anchor flags specifying resize behavior
   *
   * @pre init_window() must be called first to establish layout dimensions
   * @pre index must be in range [0, elements)
   *
   * @post Control is registered in _constraints at the given index
   * @post Control's current position/size and anchor settings are recorded
   *
   * @warning Windows API calls are synchronous and may block if window is busy
   *
   * @see Anchor
   */
  constexpr void init_anchor(const HWND parent, const BYTE index, const int id,
                             const Anchor anchor) noexcept {
    assert(index < elements);
    // Retrieve the control's window handle from the dialog
    const auto handle = GetDlgItem(parent, id);

    // Get the control's coordinates in screen space
    Rect window [[indeterminate]];
    GetWindowRect(handle, &window);

    // Convert the top-left corner from screen to client coordinates
    Point client_point(window.x(), window.y());
    ScreenToClient(parent, &client_point);

    // Create a Rect representing the control's position/size in client space
    const Rect client_area(client_point.x, client_point.y, window.width(),
                           window.height());

    // Store the constraint information
    _constraints.init(this, index, handle, client_area, anchor);
  }

  /**
   * @brief Resizes all managed controls based on the new window dimensions.
   *
   * The primary method called in response to WM_SIZE messages. Recalculates the
   * position and size of each registered control based on its anchor
   * constraints and the new window dimensions. Uses deferred window positioning
   * for efficiency.
   *
   * **Algorithm:**
   * 1. Early exit if size hasn't changed
   * 2. Update stored window dimensions
   * 3. For each registered control:
   *    a. Calculate new position/size based on anchor flags:
   *       - **Left anchor**: left edge fixed from layout left + margin
   *       - **Right anchor**: right edge fixed from layout right - margin
   *       - **HorizontalStretch**: both edges move with margins
   *       - **Top anchor**: top edge fixed from layout top + margin
   *       - **Bottom anchor**: bottom edge fixed from layout bottom - margin
   *       - **VerticalStretch**: both edges move with margins
   *    b. Queue the window update via DeferWindowPos
   * 4. Apply all updates atomically via EndDeferWindowPos
   *
   * **Anchor Logic:**
   * For horizontal positioning (priority: Left > Right > HorizontalStretch):
   * - Left only: rect.left = margin.left; rect.right = margin.left + width
   * - Right only: rect.right = new_width - margin.right; rect.left = rect.right
   *                            - width
   * - HorizontalStretch: rect.left = margin.left; rect.right = new_width -
   *                      margin.right
   *
   * For vertical positioning (priority: Top > Bottom > VerticalStretch):
   * - Top only: rect.top = margin.top; rect.bottom = margin.top + height
   * - Bottom only: rect.bottom = new_height - margin.bottom; rect.top =
   *                              rect.bottom - height
   * - VerticalStretch: rect.top = margin.top; rect.bottom = new_height -
   *                               margin.bottom
   *
   * @param width New client area width in pixels
   * @param height New client area height in pixels
   *
   * @pre Controls must be registered via init_anchor() before calling
   * @post All managed controls are repositioned and resized atomically
   * @post Internal _width and _height are updated to new dimensions
   *
   * @note Uses BeginDeferWindowPos/EndDeferWindowPos for batched updates,
   *       minimizing window redraws and providing atomic-like behavior
   * @note The deferred position update uses SWP_NOZORDER | SWP_NOACTIVATE flags
   *
   * @warning Should only be called from WM_SIZE message handler
   * @see WM_SIZE message
   */
  constexpr void resize(const LONG width, const LONG height) noexcept {
    _width = width;
    _height = height;

    // clang-format off
    // Start batching window position updates
    auto hdwp = BeginDeferWindowPos(static_cast<int>(_constraints.handles.size()));
    
    // Process each registered control
    for (BYTE i = 0; i != _constraints.handles.size(); ++i) {
      Rect rect [[indeterminate]];

      // Calculate new horizontal position based on anchor flags
      if (has_flag(_constraints.anchors[i], Anchor::Left)) {
        // Left-anchored: maintain fixed distance from left edge
        rect.right = _constraints.margins[i].left + _constraints.widths[i];
        rect.left = _constraints.margins[i].left;

      } else if (has_flag(_constraints.anchors[i], Anchor::Right)) {
        // Right-anchored: maintain fixed distance from right edge
        rect.right = width - _constraints.margins[i].right;
        rect.left = rect.right - _constraints.widths[i];

      } else if (has_flag(_constraints.anchors[i], Anchor::HorizontalStretch)) {
        // Horizontally stretched: expand/contract with window width
        rect.right = width - _constraints.margins[i].right;
        rect.left = _constraints.margins[i].left;
      }

      // Calculate new vertical position based on anchor flags
      if (has_flag(_constraints.anchors[i], Anchor::Top)) {
        // Top-anchored: maintain fixed distance from top edge
        rect.bottom = _constraints.margins[i].top + _constraints.heights[i];
        rect.top = _constraints.margins[i].top;

      } else if (has_flag(_constraints.anchors[i], Anchor::Bottom)) {
        // Bottom-anchored: maintain fixed distance from bottom edge
        rect.bottom = height - _constraints.margins[i].bottom;
        rect.top = rect.bottom - _constraints.heights[i];

      } else if (has_flag(_constraints.anchors[i], Anchor::VerticalStretch)) {
        // Vertically stretched: expand/contract with window height
        rect.bottom = height - _constraints.margins[i].bottom;
        rect.top = _constraints.margins[i].top;
      }

      // Queue the window position update
      hdwp = DeferWindowPos(hdwp, _constraints.handles[i], nullptr, rect.left, rect.top, rect.width(), rect.height(), SWP_NOZORDER | SWP_NOACTIVATE);
    }
    
    // Apply all queued updates atomically
    EndDeferWindowPos(hdwp);
    // clang-format on
  }

  // clang-format off
  /**
   * @brief Returns the current width of the layout in pixels.
   * @return Current width of the parent window's client area
   */
  [[nodiscard]] constexpr auto width() const noexcept  { return _width; }

  /**
   * @brief Returns the current height of the layout in pixels.
   * @return Current height of the parent window's client area
   */
  [[nodiscard]] constexpr auto height() const noexcept { return _height; }

  /**
   * @brief Returns the minimum width specified for this layout.
   * @return Minimum width in pixels (set via init_min_sizes)
   */
  [[nodiscard]] constexpr auto min_width() const noexcept { return _min_width; }

  /**
   * @brief Returns the minimum width as a dimension (alias for min_width()).
   * Useful for WM_GETMINMAXINFO where both min_x and min_y represent dimensions.
   * @return Minimum width in pixels
   */
  [[nodiscard]] constexpr auto min_x() const noexcept     { return _min_width; }

  /**
   * @brief Returns the minimum height specified for this layout.
   * @return Minimum height in pixels (set via init_min_sizes)
   */
  [[nodiscard]] constexpr auto min_height() const noexcept { return _min_height; }

  /**
   * @brief Returns the minimum height as a dimension (alias for min_height()).
   * Useful for WM_GETMINMAXINFO where both min_x and min_y represent dimensions.
   * @return Minimum height in pixels
   */
  [[nodiscard]] constexpr auto min_y() const noexcept      { return _min_height; }

  /**
   * @brief Retrieves the HWND of a managed control by its index.
   * @param index Zero-based index [0, elements) of the control
   * @return HWND of the control at the specified index
   * @pre index must be < elements
   */
  [[nodiscard]] constexpr auto handle(const BYTE index) const noexcept {
    assert(index < elements);
    return _constraints.handles[index];
  }

  // clang-format on

private:
  /// Current width of the parent window's client area in pixels
  [[no_unique_address]] LONG _width [[indeterminate]];

  /// Current height of the parent window's client area in pixels
  [[no_unique_address]] LONG _height [[indeterminate]];

  /// Minimum allowable width for the window in pixels
  [[no_unique_address]] LONG _min_width [[indeterminate]];

  /// Minimum allowable height for the window in pixels
  [[no_unique_address]] LONG _min_height [[indeterminate]];

  /// Storage for all control constraints (handles, sizes, margins, anchors)
  [[no_unique_address]] Constraints<elements> _constraints [[indeterminate]];
};

#ifdef CALC_SUPPORT_DARK_MODE_TEST_WIN32_HELPER_REALIZATION
namespace Colors {

/**
 * All COLOR_* indices accepted by GetSysColor()/SetSysColors().
 *
 * COLOR_SCROLLBAR .. COLOR_MENUBAR are the standard system-color indices.
 *
 * The array intentionally contains the actual indices instead of assuming that
 * every integer in the range is a unique semantic color.
 *
 * COLOR_DESKTOP == COLOR_BACKGROUND, so it is intentionally omitted.
 */
constexpr static const auto indexes =
    std::to_array<INT>({COLOR_SCROLLBAR,
                        COLOR_BACKGROUND,
                        COLOR_ACTIVECAPTION,
                        COLOR_INACTIVECAPTION,
                        COLOR_MENU,
                        COLOR_WINDOW,
                        COLOR_WINDOWFRAME,
                        COLOR_MENUTEXT,
                        COLOR_WINDOWTEXT,
                        COLOR_CAPTIONTEXT,
                        COLOR_ACTIVEBORDER,
                        COLOR_INACTIVEBORDER,
                        COLOR_APPWORKSPACE,
                        COLOR_HIGHLIGHT,
                        COLOR_HIGHLIGHTTEXT,
                        COLOR_BTNFACE,
                        COLOR_BTNSHADOW,
                        COLOR_GRAYTEXT,
                        COLOR_BTNTEXT,
                        COLOR_INACTIVECAPTIONTEXT,
                        COLOR_BTNHIGHLIGHT,
                        COLOR_3DDKSHADOW,
                        COLOR_3DLIGHT,
                        COLOR_INFOTEXT,
                        COLOR_INFOBK,
                        COLOR_HOTLIGHT,
                        COLOR_GRADIENTACTIVECAPTION,
                        COLOR_GRADIENTINACTIVECAPTION,
                        COLOR_MENUHILIGHT,
                        COLOR_MENUBAR});

/**
 * Complete snapshot of system colors.
 *
 * The values are indexed by the corresponding COLOR_* value:
 *
 *   colors[COLOR_WINDOW]
 *   colors[COLOR_WINDOWTEXT]
 *
 * etc.
 */
class Colors final {
  [[no_unique_address]] std::array<COLORREF, indexes.size()> _values
      [[indeterminate]];

public:
  /**
   * Get a system color by its COLOR_* index.
   */
  [[nodiscard]] constexpr const COLORREF &
  operator[](const INT index) const noexcept {
    return _values[index];
  }

  /**
   * Get a mutable system color by its COLOR_* index.
   */
  [[nodiscard]] constexpr COLORREF &operator[](const INT index) noexcept {
    return _values[index];
  }

  [[nodiscard]] constexpr LPCOLORREF data() const noexcept {
    return _values.data();
  }

  [[nodiscard]] constexpr INT count() const noexcept { return _values.size(); }
};

/**
 * Get all system colors.
 *
 * system provides GetSysColor() only as a scalar API, so the complete
 * system-color table is collected with one GetSysColor() call per entry.
 */
[[nodiscard]] constexpr static Colors colors() noexcept {
  Colors result [[indeterminate]];

  for (const auto i : indexes) {
    result[i] = GetSysColor(i);
  }

  return result;
}

/**
 * Set all system colors.
 *
 * SetSysColors() accepts an array of COLOR_* indices and an array of COLORREF
 * values. System broadcasts WM_SYSCOLORCHANGE after a successful change and
 * repaints affected visible windows.
 */
[[nodiscard]]
constexpr static bool set_colors(Colors &&new_values) noexcept {
  return SetSysColors(static_cast<INT>(indexes.size()), indexes.data(),
                      new_values.data()) != FALSE;
}

/**
 * Windows/DWM colors.
 *
 * There is no single global Windows 10/11 "color palette" exposed through DWM.
 *
 * DWM provides:
 *
 *   - a global colorization/accent color and opaque;
 *   - per-window caption color;
 *   - per-window caption text color;
 *   - per-window border color.
 */
struct DWMColors final {
  /**
   * Global DWM colorization color in 0xAARRGGBB format.
   *
   * This is NOT a COLORREF.
   */
  [[no_unique_address]] DWORD colorization_argb [[indeterminate]];

  [[no_unique_address]] BOOL colorization_opaque [[indeterminate]];

  /**
   * DWM window border color.
   */
  [[no_unique_address]] COLORREF border [[indeterminate]];

  /**
   * DWM window caption/title-bar color.
   */
  [[no_unique_address]] COLORREF caption [[indeterminate]];

  /**
   * DWM window caption text color.
   */
  [[no_unique_address]] COLORREF text [[indeterminate]];

  /**
   * True when DwmGetColorizationColor() succeeded.
   */
  [[no_unique_address]] bool has_window_colors [[indeterminate]];

  [[no_unique_address]] bool has_colorization [[indeterminate]];
};

/**
 * Get DWM colors for Windows 10/11.
 *
 * DwmGetColorizationColor() is queried independently because it is a global DWM
 * value.
 *
 * DWMWA_BORDER_COLOR, DWMWA_CAPTION_COLOR and DWMWA_TEXT_COLOR are per-window
 * attributes introduced for Windows 11 build 22000.
 *
 * On unsupported Windows versions the corresponding values remain zero and
 * has_window_colors stays false.
 */
[[nodiscard]]
constexpr static DWMColors dwm_colors(const HWND window) noexcept {
  DWMColors result;

  /**
   * Global DWM colorization/accent color.
   */
  if (SUCCEEDED(DwmGetColorizationColor(&result.colorization_argb,
                                        &result.colorization_opaque))) {
    result.has_colorization = true;
  }

  if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_BORDER_COLOR,
                                      &result.border, sizeof(result.border))) &&
      SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CAPTION_COLOR,
                                      &result.caption,
                                      sizeof(result.caption))) &&
      SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_TEXT_COLOR, &result.text,
                                      sizeof(result.text)))) {
    result.has_window_colors = true;
  }

  return result;
}

/**
 * Convert a DWM 0xAARRGGBB color to a COLORREF.
 *
 * DWM uses:
 *
 *   0xAARRGGBB
 *
 * while COLORREF uses:
 *
 *   0x00BBGGRR
 */
[[nodiscard]] constexpr static inline COLORREF
argb_to_colorref(const DWORD value) noexcept {
  return RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
}

} // namespace Colors

#ifdef CALC_SUPPORT_WINDOWS_VERSION_CHECK
constexpr static... check_system_version(...) noexcept {
  auto ntdll = GetModuleHandleA("ntdll.dll");
  // clang-format off
  typedef void (WINAPI* pfnRtlGetNtVersionNumbers)(ULONG*, ULONG*, ULONG*) noexcept;
#pragma warning(push)
#pragma warning(disable : 4191) // allow FARPROC -> function pointer casts here
  auto RtlGetNtVersionNumbers = (pfnRtlGetNtVersionNumbers)GetProcAddress(ntdll, "RtlGetNtVersionNumbers");
#pragma warning(pop)
  // clang-format on

  ULONG major, minor, build;

  /**
   * Note: Windows 11 and Windows 10 both return Major 10.
   * Differentiate by build number (Windows 11 is Build >= 22000).
   */
  RtlGetNtVersionNumbers(&major, &minor, &build);

  // Mask out the build number flags
  build &= 0xFFFF;

  if (major == 10 && build >= 22000) {
    // Windows 11
  }

  if (major == 10 && build >= 18362) {
    // Windows 10 1903 and later supports dark mode
  }
}
#endif

#endif

#else

// TODO Qt

#endif

} // namespace GUI

#endif
