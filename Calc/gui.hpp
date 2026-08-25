/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef GUI_HPP
#define GUI_HPP

#ifdef _WIN32

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
    "Because Calc is the Dialog based application version below Windows 2000 isn't supported."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WINXP)
#warning                                                                       \
    "In Windows before Windows XP the link in the About box isn't working bacause system API isn't exist."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WS03)
#warning                                                                       \
    "In Windows before Windows Server 2003 the max input is limited to 32767 symbols."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WIN6)
#warning                                                                       \
    "Restart manager and different DPI scaling isn't supported before Windows Vista or Windows Server 2008."
#endif

#if (_WIN32_WINNT < _WIN32_WINNT_WIN10)
#warning                                                                       \
    "In Windows before Windows 10 or Windows Server 2016 the HiDPI isn't supported and Dialog based applications not resized automatically when DPI changed."
#endif

#ifdef UNICODE
// TODO: don't copy data from Win32 Edit control to the buffer. This can'be
// usable only with UNICODE and only if multiline and local edit enabled in the
// edit control. This is needs to be enable after preprocessing and converting
// mathematical values, like 𝜋 -> pi i.e to it's representatin.
#define CALC_USED_EDIT_VIEW
#endif

/**
 * Disable rarely-used stuff from Windows headers
 */

#define NOAPISET

#define NODDEMLSPY
#define NO_COMMCTRL_DA
#define NOWINBASEINTERLOCK
// #define NOIME // For ImmDisableIME
#define NORESOURCE
#define NODESKTOP
#define NOWINDOWSTATION
#define NOSECURITY
#define NOMSG
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
#define NOCOLOR -Screen colors
// #define NOCTLMGR -Control and Dialog routines
#define NODRAWTEXT -DrawText() and DT_ *
#define NOGDI -All GDI defines and routines
#define NOKERNEL -All KERNEL defines and routines
// #define NOUSER -All USER defines and routines
#define NONLS -All NLS defines and routines
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
#define NOIMAGEAPIS ImageList apis.
#define NOLISTVIEW ListView control.
#define NOTREEVIEW TreeView control.
#define NOTABCONTROL Tab control.
#define NOANIMATE Animate control.
#define NOBUTTON Button control.
#define NOSTATIC Static control.
#define NOEDIT Edit control.
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

#include <windows.h>

#ifdef CALC_SUPPORT_LINK_WINDOW
#include <commctrl.h>

#include <shellapi.h>
#endif

#include "resource.h" // GUI symbols

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
  static constexpr UINT input_max_data_size =
#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
      // Win32 ANSI multiline EDIT max is 64 KiB for classic Edit control:
      64 * 1024;
#else
      // Win32 ANSI multiline EDIT max is 32 KiB for classic Edit control:
      32 * 1024;
#endif
  // because the number above including C zero terminator maximum symbols are:
  static constexpr UINT input_max_symbols = input_max_data_size - 1;
};

#ifdef CALC_USED_EDIT_VIEW
/**
 * GUI helper structure to process user input.
 */
class EditTextView {
public:
  explicit EditTextView(HWND edit) noexcept {
    _size = SendMessageA(edit, WM_GETTEXTLENGTH, 0, 0);
    if (is_present()) {
      _handle =
          reinterpret_cast<HLOCAL>(SendMessageA(edit, EM_GETHANDLE, 0, 0));
      _data = static_cast<char const *>(LocalLock(_handle));
    }
  }

  ~EditTextView() noexcept {
    if (is_present()) {
      LocalUnlock(_handle);
    }
  }

  EditTextView(EditTextView const &) = delete;
  EditTextView &operator=(EditTextView const &) = delete;

  [[nodiscard]]
  bool is_present() const noexcept {
    return _size != 0;
  }

  [[nodiscard]]
  std::string_view get_view() const noexcept {
    return {_data, _size};
  }

private:
  HLOCAL _handle [[indeterminate]];
  char const *_data [[indeterminate]];
  UINT _size;
};
#else
/**
 * GUI helper structure to handle user input.
 */
struct UserInput {
  [[nodiscard]] auto size() const noexcept { return _size; }

  void set_size(UINT s) noexcept { _size = s; }

  [[nodiscard]] auto max_size() const noexcept { return _max_size; }

  [[nodiscard]] auto data() const noexcept { return _data; }

  [[nodiscard]] auto data() noexcept { return _data; }

private:
  static constexpr UINT _max_size = CalcConfiguration::input_max_symbols;
  UINT _size [[indeterminate]];
  char _data[CalcConfiguration::input_max_data_size];
};
#endif

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
                         &outSize) == ERROR_SUCCESS) {
      if (type == REG_DWORD) {
        return out;
      }
    }
    return std::nullopt;
  }

  [[nodiscard]] DWORD read(const char *name, LPBYTE out,
                           DWORD out_size) const noexcept {
    DWORD type [[indeterminate]];
    if (RegQueryValueExA(key, name, nullptr, &type, out, &out_size) ==
        ERROR_SUCCESS) {
      if (type == REG_BINARY) {
        return out_size;
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
static void set_window_icons(const HWND window,
                             const HINSTANCE instance) noexcept {
  SendMessageA(window, WM_SETICON, ICON_SMALL,
               reinterpret_cast<LPARAM>(
                   LoadIconA(instance, MAKEINTRESOURCEA(IDR_MAINFRAME_SMALL))));
  SendMessageA(window, WM_SETICON, ICON_BIG,
               reinterpret_cast<LPARAM>(
                   LoadIconA(instance, MAKEINTRESOURCEA(IDR_MAINFRAME_BIG))));
}

/**
 * Utility: set text to window from begin to end (end is not included).
 */
static void set_window_text(const HWND hWnd, LPCSTR text,
                            LPSTR const text_end) noexcept {
  *text_end = '\0'; // because of C string
  SetWindowTextA(hWnd, text);
}

#ifndef CALC_USED_EDIT_VIEW
/**
 * Utility: get text from window and return its size.
 */
static [[nodiscard]] UINT get_window_text(const HWND hWnd, CHAR *text,
                                          const int max_size) noexcept {
  return GetWindowTextA(hWnd, text, max_size);
}
#endif

#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
/**
 * Utility: set window text limit
 */
static void set_window_text_limit(const HWND hWnd,
                                  WPARAM max_symbols) noexcept {
  SendMessageA(hWnd, EM_LIMITTEXT, max_symbols, 0);
}
#endif

/**
 * Utility: goto end of text in window and scroll caret to it
 */
static void goto_end_of_window_text(const HWND hWnd) noexcept {
  SendMessageA(hWnd, EM_SETSEL, static_cast<WPARAM>(0),
               static_cast<LPARAM>(-1));
  SendMessageA(hWnd, EM_SCROLLCARET, 0, 0);
}

#ifdef CALC_SUPPORT_DPI_CHANGES
/**
 * Utility: return dpi for window
 */
static [[nodiscard]] UINT get_window_dpi(const HWND window) noexcept {
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
 * Utility: coordinate convertor from logical to physical for window
 */
static [[nodiscard]] LONG to_physical(LONG value, LONG dpi) noexcept {
  return std::lroundf(static_cast<float>(value) * static_cast<float>(dpi) /
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI));
}

/**
 * Utility: coordinate convertor from physical to logical for window
 */
static [[nodiscard]] LONG to_logical(LONG value, LONG dpi) noexcept {
  return std::lroundf(static_cast<float>(value) *
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI) /
                      static_cast<float>(dpi));
};
#endif

/**
 * Utility: helper to work with points in window layout
 */
struct Point : tagPOINT {
  Point() = default;

  constexpr Point(const LONG _x, const LONG _y) noexcept {
    x = _x;
    y = _y;
  }

  [[nodiscard]] constexpr auto get_x() const noexcept { return x; }

  [[nodiscard]] constexpr auto get_y() const noexcept { return y; }
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

  [[nodiscard]] constexpr auto get_x() const noexcept { return left; }

  [[nodiscard]] constexpr auto get_y() const noexcept { return top; }

  [[nodiscard]] constexpr auto get_width() const noexcept {
    return right - left;
  }

  [[nodiscard]] constexpr auto get_heigth() const noexcept {
    return bottom - top;
  }
};

using Rect_ptr = Rect *;

enum class HorizontalMode : BYTE { Stretch, Left, Right };
enum class VerticalMode : BYTE { Stretch, Top, Bottom };

struct Anchor {
  [[no_unique_address]] HorizontalMode horizontal [[indeterminate]];
  [[no_unique_address]] VerticalMode vertical [[indeterminate]];
};

/**
 * Utility: helper to work with window layout
 */
template <typename UINT elements> class Layout {
public:
  struct Constraint {
    constexpr void init(const Layout *layout, const HWND _handle,
                        const Rect &client, const Anchor _anchor) noexcept {
      handle = _handle;

      relative_margins.left = client.left;
      relative_margins.right = layout->get_width() - client.right;

      relative_margins.top = client.top;
      relative_margins.bottom = layout->get_height() - client.bottom;

      width = client.get_width();
      height = client.get_heigth();

      anchor = _anchor;
    }

    HWND handle [[indeterminate]];
    Rect relative_margins [[indeterminate]];
    LONG width [[indeterminate]];
    LONG height [[indeterminate]];
    [[no_unique_address]] Anchor anchor [[indeterminate]];
  };

  void init_window(const HWND window) noexcept {
    Rect client [[indeterminate]];
    GetClientRect(window, &client);

    width = client.right;
    height = client.bottom;
  }

  constexpr void init_min_sizes(const LONG _min_width,
                                const LONG _min_height) noexcept {
    min_width = _min_width;
    min_height = _min_height;
  }

  void init_anchor(const HWND parent, const uint8_t index, const int id,
                   const Anchor anchor) noexcept {
    const auto handle = GetDlgItem(parent, id);

    Rect window [[indeterminate]];
    GetWindowRect(handle, &window);

    Point client_point(window.get_x(), window.get_y());
    ScreenToClient(parent, &client_point);
    const Rect client(client_point.get_x(), client_point.get_y(),
                      window.get_width(), window.get_heigth());

    constraints[index].init(this, handle, client, anchor);
  }

  void resize(const LONG new_width, const LONG new_height) noexcept {
    if (width == new_width && height == new_height) {
      return;
    }
    width = new_width;
    height = new_height;

    HDWP hdwp = BeginDeferWindowPos(static_cast<int>(constraints.size()));
    for (const auto &c : constraints) {
      Rect rect [[indeterminate]];

      switch (c.anchor.horizontal) {
      case HorizontalMode::Left:
        rect.right = c.relative_margins.left + c.width;
        rect.left = c.relative_margins.left;
        break;

      case HorizontalMode::Right:
        rect.right = width - c.relative_margins.right;
        rect.left = rect.right - c.width;
        break;

      case HorizontalMode::Stretch:
        rect.right = width - c.relative_margins.right;
        rect.left = c.relative_margins.left;
        break;

      default:
        std::unreachable();
      }

      switch (c.anchor.vertical) {
      case VerticalMode::Top:
        rect.bottom = c.relative_margins.top + c.height;
        rect.top = c.relative_margins.top;
        break;

      case VerticalMode::Bottom:
        rect.bottom = height - c.relative_margins.bottom;
        rect.top = rect.bottom - c.height;
        break;

      case VerticalMode::Stretch:
        rect.bottom = height - c.relative_margins.bottom;
        rect.top = c.relative_margins.top;
        break;

      default:
        std::unreachable();
      }

      hdwp = DeferWindowPos(hdwp, c.handle, nullptr, rect.left, rect.top,
                            rect.get_width(), rect.get_heigth(),
                            SWP_NOZORDER | SWP_NOACTIVATE);
    }
    EndDeferWindowPos(hdwp);
  }

  [[nodiscard]] constexpr auto get_width() const noexcept { return width; }

  [[nodiscard]] constexpr auto get_height() const noexcept { return height; }

  [[nodiscard]] constexpr auto get_min_width() const noexcept {
    return min_width;
  }

  [[nodiscard]] constexpr auto get_min_x() const noexcept { return min_width; }

  [[nodiscard]] constexpr auto get_min_height() const noexcept {
    return min_height;
  }

  [[nodiscard]] constexpr auto get_min_y() const noexcept { return min_height; }

  [[nodiscard]] constexpr auto get_handle(const BYTE index) const noexcept {
    return constraints[index].handle;
  }

private:
  [[no_unique_address]] std::array<Constraint, elements> constraints
      [[indeterminate]];

  LONG width [[indeterminate]];
  LONG height [[indeterminate]];

  LONG min_width [[indeterminate]];
  LONG min_height [[indeterminate]];
};

#else

// TODO Qt

#endif
#endif
