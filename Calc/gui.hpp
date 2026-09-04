/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef GUI_HPP
#define GUI_HPP

namespace GUI {

#ifdef _WIN32

// Explicitely enble C++ support by Windows SDK to use Unicode on systems before
// Windows Vista or Windows 2008
#define WINDOWS_ENABLE_CPLUSPLUS

// Disable warnings about unsafe functions in Windows API
#define _CRT_SECURE_NO_WARNINGS

// Speed up build time by excluding rarely-used stuff from Windows headers
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "targetver.h"

/**
 * Check Windows version support for Calc GUI application. The minimal supported
 * version is Windows 2000, but some features are not available in older
 * versions below Windows 10 or Windows Server 2016.
 */

#if (_WIN32_WINNT < _WIN32_WINNT_WIN2K)
#error                                                                         \
    "Calc is the dialog based application. Windows 2000 is the minimal version."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WINXP)
#warning "The homepage link in the about box working from XP."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WS03)
#warning "From Server 2003 we can use increased input up to 64k symbols."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WIN6)
#warning                                                                       \
    "Restart manager and different DPI scaling supported from Vista or Server 2008."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WIN10)
#warning                                                                       \
    "In Windows 10 or Server 2016 the HiDPI supported and Dialog based applications resized automatically when DPI changed. Also auto dark mode only available for Windows 10+."
#endif

/**
 * Disable rarely-used stuff from Windows headers
 */

// #define NOAPISET // MultiByteToWideChar
#define NODDEMLSPY
#define NO_COMMCTRL_DA
#define NOWINBASEINTERLOCK
#ifndef CALC_DISABLE_IME
#define NOIME
#endif
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
 * Enable CRT debug leak detection in debug builds so leaking allocations are
 * reported with call stacks at process exit. This mirrors the usual pattern of
 * calling _CrtSetDbgFlag and _CrtSetReportMode early in startup.
 *
 * Should be called before WinMain!
 */
static void __cdecl _setup_crt_leak_check() {
  // Report to output window and perform leak check at exit.
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

  flags |= _CRTDBG_ALLOC_MEM_DF;  // Turn on debug allocation
  flags |= _CRTDBG_LEAK_CHECK_DF; // Perform leak check at program exit

  // Don't enable _CRTDBG_CHECK_CRT_DF !

  _CrtSetDbgFlag(flags);

  // Ensure reports go to debugger output
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
  _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
  _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
}
#else
static void __cdecl _setup_crt_leak_check() {}
#endif

// Disable warnings for GUI code:
// clang-format off
__pragma(warning(disable : 5039)); // potentially throwing function passed to extern C
__pragma(warning(disable : 4865)); // vector<bool> is never constructed with a non-constant size
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

#include "resource.h" // GUI symbols

/**
 * Dialog procedure type.
 */
typedef INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

/**
 * GUI helper to process user input. Don't need to copy Unicode user input from
 * Edit control. This can'be usable only if Edit is multiline and for Dialog
 * local edit option is enabled.
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
    _data = static_cast<LPCWSTR>(LocalLock(_handle));
    assert(_handle && _data);
  }

  ~EditView() noexcept { LocalUnlock(_handle); }

  /**
   * Length in characters of the text in the edit control.
   */
  [[nodiscard]]
  constexpr auto length() const noexcept {
    return _length;
  }

  [[nodiscard]] constexpr auto text() const noexcept { return _data; }

  /**
   * Return ANSI text from the edit control to the ouput paramether.
   */
  constexpr void ansi(std::string &output) const noexcept {
    assert(_length <= output.capacity());
    output.resize(output.capacity());
    output.resize(WideCharToMultiByte(CP_ACP, 0, _data, _length, output.data(),
                                      output.capacity(), NULL, NULL));
  }

  /**
   * Size in bytes of the data in the edit control.
   */
  [[nodiscard]]
  constexpr auto size() const noexcept {
    return static_cast<UINT>(_length * sizeof(WCHAR));
  }

  [[nodiscard]] constexpr auto data() const noexcept {
    return reinterpret_cast<const BYTE *>(_data);
  }

  [[nodiscard]]
  constexpr bool empty() const noexcept {
    return _length == 0;
  }

private:
  [[no_unique_address]] HLOCAL _handle [[indeterminate]];
  [[no_unique_address]] LPCWSTR _data [[indeterminate]];
  [[no_unique_address]] const UINT _length;
};

/**
 * GUI helper to write data to the GUI and restore user data from system
 * database.
 */
class Edit {
public:
  Edit() = delete;
  Edit(Edit const &) = delete;
  Edit &operator=(Edit const &) = delete;

  explicit Edit(const HWND edit, const UINT max_length) noexcept
      : _edit(edit), _max_size(max_length * sizeof(WCHAR)) {
    _handle = reinterpret_cast<HLOCAL>(SendMessageA(edit, EM_GETHANDLE, 0, 0));
    assert(_handle);
    if (_max_size) {
      _handle = LocalReAlloc(_handle, _max_size, LMEM_MOVEABLE);
      assert(_handle);
    }
    _data = static_cast<LPWSTR>(LocalLock(_handle));
    assert(_data);
  }

  ~Edit() noexcept {
    LocalUnlock(_handle);
    if (_max_size) {
      SendMessageA(_edit, EM_SETHANDLE, reinterpret_cast<WPARAM>(_handle), 0);
    }
  }

  /**
   * Length in characters of the text in the edit control.
   */
  [[nodiscard]] constexpr UINT length() const noexcept {
    return _max_size / sizeof(WCHAR);
  }

  [[nodiscard]] constexpr auto text() const noexcept { return _data; }

  constexpr void set_text_end(LPWSTR end) const noexcept {
    assert(end >= _data && UINT(end - _data) <= length());
    *end = L'\0'; // because of C string
  }

  /**
   * Set length in characters of the text in the edit control.
   */
  constexpr void set_length(const UINT length) const noexcept {
    assert(length <= _max_size / sizeof(WCHAR));
    set_text_end(_data + length);
  }

  /**
   * Write ANSI text to the edit control.
   */
  void write(const char *text, const int length) noexcept {
    assert(unsigned(length) <= _max_size / sizeof(WCHAR));
    set_length(MultiByteToWideChar(CP_ACP, 0, text, length, _data, length));
  }

  /**
   * Write ANSI text to the edit control.
   */
  void write(const char *text, const char *text_end) noexcept {
    write(text, static_cast<UINT>(text_end - text));
  }

  /**
   * Size in bytes of the data in the edit control.
   */
  [[nodiscard]] constexpr auto data() noexcept {
    return reinterpret_cast<BYTE *>(_data);
  }

  /**
   * Set size in bytes of the data in the edit control.
   */
  constexpr void set_size(const UINT size) noexcept {
    set_length(size / sizeof(WCHAR));
  }

private:
  [[no_unique_address]] const HWND _edit;
  [[no_unique_address]] HLOCAL _handle [[indeterminate]];
  [[no_unique_address]] LPWSTR _data [[indeterminate]];
  [[no_unique_address]] const UINT _max_size;
};

/**
 * Utility: helper to write data to system database.
 */
struct RegWrite {
  RegWrite() = delete;
  RegWrite(const RegWrite &) = delete;
  RegWrite(RegWrite &&) = delete;

  explicit RegWrite(const HKEY root, const char *subkey) noexcept {
    RegCreateKeyExA(root, subkey, FALSE, nullptr, REG_OPTION_NON_VOLATILE,
                    KEY_WRITE, nullptr, &key, nullptr);
  }

  ~RegWrite() noexcept { RegCloseKey(key); }

  void write(const char *name, const DWORD value) const noexcept {
    RegSetValueExA(key, name, FALSE, REG_DWORD,
                   reinterpret_cast<const BYTE *>(&value), sizeof(value));
  }

  void write(const char *name, const BYTE *data,
             const DWORD size) const noexcept {
    RegSetValueExA(key, name, FALSE, REG_BINARY, data, size);
  }

private:
  HKEY key;
};

/**
 * Utility: helper for read data from system database.
 */
struct RegRead {
  RegRead() = delete;
  RegRead(const RegRead &) = delete;
  RegRead(RegRead &&) = delete;

  explicit RegRead(const HKEY root, const char *subkey) noexcept {
    RegOpenKeyExA(root, subkey, FALSE, KEY_READ, &key);
  }

  ~RegRead() noexcept { RegCloseKey(key); }

  [[nodiscard]] std::optional<DWORD> read(const char *name) const noexcept {
    DWORD type [[indeterminate]];
    DWORD out [[indeterminate]];
    DWORD outSize = sizeof(out);
    if (RegQueryValueExA(key, name, nullptr, &type,
                         reinterpret_cast<LPBYTE>(&out),
                         &outSize) == ERROR_SUCCESS) [[likely]] {
      if (type == REG_DWORD) [[likely]] {
        return out;
      }
    }
    return std::nullopt;
  }

  [[nodiscard]] UINT read(const char *name, LPBYTE out,
                          DWORD out_size) const noexcept {
    DWORD type [[indeterminate]];
    if (RegQueryValueExA(key, name, nullptr, &type, out, &out_size) ==
        ERROR_SUCCESS) [[likely]] {
      if (type == REG_BINARY) [[likely]] {
        return static_cast<UINT>(out_size);
      }
    }
    return 0;
  }

private:
  HKEY key;
};

/**
 * Utility: add "About..." menu item to system menu for window.
 */
static void add_about_menu_to_system_menu(const HWND window) noexcept {
  // IDM_ABOUTBOX must be in the system command range.
  static_assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  static_assert(IDM_ABOUTBOX < 0xF000);

  const auto system_menu = GetSystemMenu(window, FALSE);
  AppendMenuA(system_menu, MF_SEPARATOR, FALSE, nullptr);
  AppendMenuA(system_menu, MF_STRING, IDM_ABOUTBOX, "&About...");
}

/**
 * Utility: set window icons (small and big)
 */
static void set_icons(const HWND window, const HINSTANCE app) noexcept {
  PostMessageA(window, WM_SETICON, ICON_SMALL,
               reinterpret_cast<LPARAM>(
                   LoadIconA(app, MAKEINTRESOURCEA(IDR_MAINFRAME_SMALL))));
  PostMessageA(window, WM_SETICON, ICON_BIG,
               reinterpret_cast<LPARAM>(
                   LoadIconA(app, MAKEINTRESOURCEA(IDR_MAINFRAME_BIG))));
}

#ifdef CALC_SUPPORT_EXTENDENT_STYLES
/**
 * Utility: set window extended styles.
 */
static void set_extended_style(const HWND window, const DWORD style) noexcept {
  PostMessageA(window, EM_SETEXTENDEDSTYLE, static_cast<WPARAM>(style),
               static_cast<LPARAM>(style));
}
#endif

/**
 * Set text to window. Shoul be zero terminated!
 */
static void set_text(const HWND window, LPCSTR text) noexcept {
  SetWindowTextA(window, text);
}

/**
 * Set text to window (end is for C string terminator).
 */
static void set_text(const HWND window, LPCSTR text,
                     LPSTR const text_end) noexcept {
  *text_end = '\0'; // because of C string
  set_text(window, text);
}

#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
/**
 * Set window text limit
 */
static void set_text_limit(const HWND window, WPARAM max_symbols) noexcept {
  PostMessageA(window, EM_LIMITTEXT, max_symbols, 0);
}
#endif

/**
 * Goto end of text in window and scroll caret to it
 */
static void goto_end_of_text(const HWND window) noexcept {
  PostMessageA(window, EM_SETSEL, static_cast<WPARAM>(0),
               static_cast<LPARAM>(-1));
  PostMessageA(window, EM_SCROLLCARET, 0, 0);
}

#ifdef CALC_SUPPORT_DPI_CHANGES
/**
 * Get dpi for window
 */
[[nodiscard]] static UINT dpi(const HWND window) noexcept {
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
 */
[[nodiscard]] static LONG physical(LONG logical, LONG dpi) noexcept {
  return std::lroundf(static_cast<float>(logical) * static_cast<float>(dpi) /
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI));
}

/**
 * Convert coordinates from physical to logical.
 */
[[nodiscard]] static LONG logical(LONG physical, LONG dpi) noexcept {
  return std::lroundf(static_cast<float>(physical) *
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI) /
                      static_cast<float>(dpi));
};
#endif

#ifdef CALC_SUPPORT_DARK_MODE
/**
 * Preferred application theme mode used by the undocumented UxTheme API.
 */
enum class PreferredAppMode : int {
  Default = 0,
  AllowDark = 1,
  ForceDark = 2,
  ForceLight = 3,
  Max = 4
};

/**
 * Menu dynamic colors API for Windows 6+ with DWM.
 *
 * Uses the undocumented uxtheme exports:
 *   #104 RefreshImmersiveColorPolicyState
 *   #135 SetPreferredAppMode
 *   #136 FlushMenuThemes
 *
 * The menu itself remains owned by system. No owner-draw, no WM_DRAWITEM,
 * and no custom menu colors are required.
 */
// clang-format off
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
 * Should be called before WinMain!
 */
constexpr static void init_uxtheme_callers() noexcept {
  HMODULE uxtheme = GetModuleHandleA("uxtheme.dll");
  // clang-format off
  __pragma(warning(push))
  __pragma(warning(disable : 4191)) // allow FARPROC -> function pointer casts here
  RefreshImmersiveColorPolicyState = reinterpret_cast<RefreshImmersiveColorPolicyStateFn>(
      GetProcAddress(uxtheme, MAKEINTRESOURCEA(104)));
  SetPreferredAppMode = reinterpret_cast<SetPreferredAppModeFn>(
      GetProcAddress(uxtheme, MAKEINTRESOURCEA(135)));
  FlushMenuThemes = reinterpret_cast<FlushMenuThemesFn>(
      GetProcAddress(uxtheme, MAKEINTRESOURCEA(136)));
  __pragma(warning(pop))
  // clang-format on
}

/**
 * Application theme management.
 */
struct Theme {
  /**
   * Init application theme.
   *
   * Should be called before any window is initialized!
   */
  constexpr void init(const HWND application_main_window) noexcept {
    SetPreferredAppMode(PreferredAppMode::AllowDark);
    apply(application_main_window, false, true);
  }

  /**
   * Apply theme to application title bar, aplication frame, menus and for
   * interactive controls that repaints itself based on it's current state:
   * BUTTON, SCROLLBAR, COMBOBOX, LISTBOX, LISTVIEW, TREEVIEW, TAB, PROGRESSBAR,
   * TRACKBAR.
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

#ifdef CALC_SUPPORT_DARK_MODE_WITHOUT_WIN32_HELPER
  /**
   * Apply theme to controls that needs external repainting, they don't using
   * theming and get global colors from system settings: EDIT, STATIC, DIALOG
   */
  [[nodiscard]] constexpr INT_PTR apply(const WPARAM wP) const noexcept {
    const HDC hdc = reinterpret_cast<HDC>(wP);
    SetBkColor(hdc, GetSysColor(_background_index));
    SetTextColor(hdc, GetSysColor(_text_index));
    return reinterpret_cast<INT_PTR>(GetSysColorBrush(_background_index));
  }
#endif

private:
  /**
   * Apply theme to window based on a global theme.
   */
  constexpr static BOOL CALLBACK theme(const HWND window,
                                       const LPARAM dark) noexcept {
    SetWindowTheme(window, dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
    return TRUE;
  }

  [[nodiscard]] constexpr bool is_dark_mode() const noexcept {
    return _dark_mode;
  }

  /**
   * For main application window only: determine uses of the dark app theme.
   */
  [[nodiscard]] constexpr bool
  is_dark_mode(const bool is_main_window) noexcept {
    if (is_main_window) {
      // clang-format off
      const RegRead personalize(HKEY_CURRENT_USER,
                  "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");
      const auto light = personalize.read("AppsUseLightTheme");
      _dark_mode = light && light.value() == 0;

      _background_index = is_dark_mode() ? COLOR_WINDOWTEXT : COLOR_WINDOW;
      _text_index =       is_dark_mode() ? COLOR_WINDOW : COLOR_WINDOWTEXT;
      // clang-format on
    }
    return is_dark_mode();
  }

  /**
   * Apply theme to a application bar and frame. Windows 6+ with DWM.
   */
  constexpr void title_bar_and_frame(const HWND hwnd) const noexcept {
    BOOL value = is_dark_mode() ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value,
                          sizeof(value));
  }

  /**
   * Apply theme to menus. API for Windows 6+ with working DWM.
   */
  constexpr void menus() const noexcept {
    RefreshImmersiveColorPolicyState();
    FlushMenuThemes();
  }

  /**
   * Used to reduce registry reads and colors calls.
   */
  [[no_unique_address]] uint8_t _background_index [[indeterminate]];
  [[no_unique_address]] uint8_t _text_index [[indeterminate]];
  [[no_unique_address]] bool _dark_mode [[indeterminate]];
};
#else
void init_uxtheme_callers() {};
#endif

/**
 * Utility: helper to work with points in window layout
 */
struct Point : tagPOINT {
  constexpr Point() noexcept = default;

  constexpr Point(const LONG _x, const LONG _y) noexcept {
    x = _x;
    y = _y;
  }
};

/**
 * Utility: helper to work with rectangles in window layout
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

using Rect_ptr = Rect *;

/**
 * Utility: helper for resize Layout
 */
enum class Anchor : uint8_t {
  None = 0,
  HorizontalStretch = 1 << 0,
  Left = 1 << 1,
  Right = 1 << 2,
  VerticalStretch = 1 << 3,
  Top = 1 << 4,
  Bottom = 1 << 5
};

// Enable bitwise OR
constexpr Anchor operator|(Anchor lhs, Anchor rhs) noexcept {
  using T = std::underlying_type_t<Anchor>;
  return static_cast<Anchor>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

// Enable bitwise AND
constexpr Anchor operator&(Anchor lhs, Anchor rhs) noexcept {
  using T = std::underlying_type_t<Anchor>;
  return static_cast<Anchor>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

// Helper to check if a flag is set
constexpr bool has_flag(Anchor value, Anchor flag) noexcept {
  return static_cast<bool>(value & flag);
}

/**
 * Utility: helper to work with window layout
 */
template <typename UINT elements> class Layout {
public:
  constexpr Layout() noexcept = default;
  Layout(const Layout &) = delete;
  Layout(Layout &&) = delete;

  struct Constraint {
    constexpr void init(const Layout *layout, const HWND handle,
                        const Rect &client, const Anchor anchor) noexcept {
      _handle = handle;

      _relative_margins.left = client.left;
      _relative_margins.right = layout->width() - client.right;

      _relative_margins.top = client.top;
      _relative_margins.bottom = layout->height() - client.bottom;

      _width = client.width();
      _height = client.height();

      _anchor = anchor;
    }

    [[no_unique_address]] HWND _handle [[indeterminate]];
    [[no_unique_address]] LONG _width [[indeterminate]];
    [[no_unique_address]] LONG _height [[indeterminate]];
    [[no_unique_address]] Rect _relative_margins [[indeterminate]];
    [[no_unique_address]] Anchor _anchor [[indeterminate]];
  };

  constexpr void init_window(const HWND window) noexcept {
    Rect client [[indeterminate]];
    GetClientRect(window, &client);
    _width = client.right;
    _height = client.bottom;
  }

  constexpr void init_min_sizes(const LONG min_width,
                                const LONG min_height) noexcept {
    _min_width = min_width;
    _min_height = min_height;
  }

  constexpr void init_anchor(const HWND parent, const BYTE index, const int id,
                             const Anchor anchor) noexcept {
    const auto handle = GetDlgItem(parent, id);

    Rect window [[indeterminate]];
    GetWindowRect(handle, &window);

    Point client_point(window.x(), window.y());
    ScreenToClient(parent, &client_point);
    const Rect client(client_point.x, client_point.y, window.width(),
                      window.height());

    _constraints[index].init(this, handle, client, anchor);
  }

  constexpr void resize(const LONG width, const LONG height) noexcept {
    if (_width == width && _height == height) [[unlikely]] {
      return;
    }
    _width = width;
    _height = height;

    HDWP hdwp = BeginDeferWindowPos(static_cast<int>(_constraints.size()));
    for (const auto &c : _constraints) {
      Rect rect [[indeterminate]];

      if (has_flag(c._anchor, Anchor::Left)) {
        rect.right = c._relative_margins.left + c._width;
        rect.left = c._relative_margins.left;
      } else if (has_flag(c._anchor, Anchor::Right)) {
        rect.right = width - c._relative_margins.right;
        rect.left = rect.right - c._width;
      } else if (has_flag(c._anchor, Anchor::HorizontalStretch)) {
        rect.right = width - c._relative_margins.right;
        rect.left = c._relative_margins.left;
      }

      if (has_flag(c._anchor, Anchor::Top)) {
        rect.bottom = c._relative_margins.top + c._height;
        rect.top = c._relative_margins.top;
      } else if (has_flag(c._anchor, Anchor::Bottom)) {
        rect.bottom = height - c._relative_margins.bottom;
        rect.top = rect.bottom - c._height;
      } else if (has_flag(c._anchor, Anchor::VerticalStretch)) {
        rect.bottom = height - c._relative_margins.bottom;
        rect.top = c._relative_margins.top;
      }

      hdwp = DeferWindowPos(hdwp, c._handle, nullptr, rect.left, rect.top,
                            rect.width(), rect.height(),
                            SWP_NOZORDER | SWP_NOACTIVATE);
    }
    EndDeferWindowPos(hdwp);
  }

  [[nodiscard]] constexpr auto width() const noexcept { return _width; }

  [[nodiscard]] constexpr auto height() const noexcept { return _height; }

  [[nodiscard]] constexpr auto min_width() const noexcept { return _min_width; }

  [[nodiscard]] constexpr auto min_x() const noexcept { return _min_width; }

  [[nodiscard]] constexpr auto min_height() const noexcept {
    return _min_height;
  }

  [[nodiscard]] constexpr auto min_y() const noexcept { return _min_height; }

  [[nodiscard]] constexpr auto handle(const BYTE index) const noexcept {
    return _constraints[index]._handle;
  }

private:
  [[no_unique_address]] LONG _width [[indeterminate]];
  [[no_unique_address]] LONG _height [[indeterminate]];

  [[no_unique_address]] LONG _min_width [[indeterminate]];
  [[no_unique_address]] LONG _min_height [[indeterminate]];
  [[no_unique_address]] std::array<Constraint, elements> _constraints
      [[indeterminate]];
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
  [[nodiscard]]
  const COLORREF &operator[](const INT index) const noexcept {
    return _values[index];
  }

  /**
   * Get a mutable system color by its COLOR_* index.
   */
  [[nodiscard]]
  COLORREF &operator[](const INT index) noexcept {
    return _values[index];
  }

  [[nodiscard]] LPCOLORREF data() noexcept { return _values.data(); }
};

/**
 * Get all system colors.
 *
 * system provides GetSysColor() only as a scalar API, so the complete
 * system-color table is collected with one GetSysColor() call per entry.
 */
[[nodiscard]]
constexpr static Colors colors() noexcept {
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
 * Convert a DWM 0xAARRGGBB color to a Win32 COLORREF.
 *
 * DWM uses:
 *
 *   0xAARRGGBB
 *
 * while COLORREF uses:
 *
 *   0x00BBGGRR
 */
[[nodiscard]]
constexpr static inline COLORREF argb_to_colorref(const DWORD value) noexcept {
  return RGB((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
}

} // namespace Colors

#endif

#else

// TODO Qt

#endif

} // namespace GUI

#endif
