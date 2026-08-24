/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef GUI_HPP
#define GUI_HPP

#ifdef _WIN32

#define WINDOWS_ENABLE_CPLUSPLUS
#define _CRT_SECURE_NO_WARNINGS

// Exclude rarely-used stuff from Windows headers
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include "targetver.h"

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

#define NOGDICAPMASKS -CC_ *, LC_ *, PC_ *, CP_ *, TC_ *, RC_
#define NOVIRTUALKEYCODES -VK_ *
#define NOWINSTYLES -WS_ *, CS_ *, ES_ *, LBS_ *, SBS_ *, CBS_ *
#define NOSYSMETRICS -SM_ *
#define NOICONS -IDI_ *
#define NOKEYSTATES -MK_ *
#define NOSYSCOMMANDS -SC_ *
#define NORASTEROPS -Binary and Tertiary raster ops
#define OEMRESOURCE -OEM Resource values
#define NOATOM -Atom Manager routines
#define NOCLIPBOARD -Clipboard routines
#define NOCOLOR -Screen colors
#define NODRAWTEXT -DrawText() and DT_ *
#define NOKERNEL -All KERNEL defines and routines
#define NONLS -All NLS defines and routines
#define NOMB -MB_ *and MessageBox()
#define NOMEMMGR -GMEM_ *, LMEM_ *, GHND, LHND, associated routines
#define NOMETAFILE -typedef METAFILEPICT
#define NOMINMAX -Macros min(a, b) and max(a, b)
#define NOOPENFILE -OpenFile(), OemToAnsi, AnsiToOem, and OF_ *
#define NOSCROLL -SB_ *and scrolling routines
#define NOSERVICE -All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND -Sound driver routines
#define NOTEXTMETRIC -typedef TEXTMETRIC and associated routines
#define NOWH -SetWindowsHook and WH_ *
#define NOCOMM -COMM driver routines
#define NOKANJI -Kanji support stuff.
#define NOHELP -Help engine interface.
#define NOPROFILER -Profiler interface.
#define NOMCX -Modem Configuration Extensions

#include <windows.h>

#include <commctrl.h>
#include <imm.h>
#include <shellapi.h>

#include "resource.h" // GUI symbols

/**
 * Calc GUI configuration: matches RC and system internals for correct work.
 */
struct CalcConfiguration {
  static constexpr const char *reg_key = "Software\\HedgehogInTheCPP\\Calc";
  static constexpr LONG min_width = 232;  // matches RC
  static constexpr LONG min_height = 158; // matches RC
  static constexpr UINT elements = 3;     // matches RC

  static constexpr UINT default_shift_px = 100;

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

/**
 * Calc GUI helper structure to handle user input.
 */
struct CalcEquation {
  const auto size() const { return _size; }

  void set_size(UINT s) { _size = s; }

  auto max_size() const { return _max_size; }

  auto data() const { return _data; }

  auto data() { return _data; }

private:
  static constexpr UINT _max_size = CalcConfiguration::input_max_symbols;
  UINT _size [[indeterminate]];
  char _data[CalcConfiguration::input_max_data_size];
};

/**
 * Utility: helper to write data to system database.
 */
struct RegWrite {
  RegWrite() = delete;

  RegWrite(const HKEY root, const char *subkey) noexcept {
    RegCreateKeyExA(root, subkey, FALSE, nullptr, REG_OPTION_NON_VOLATILE,
                    KEY_WRITE, nullptr, &key, nullptr);
  }

  ~RegWrite() { RegCloseKey(key); }

  void write(const char *name, const DWORD value) const {
    RegSetValueExA(key, name, FALSE, REG_DWORD,
                   reinterpret_cast<const BYTE *>(&value), sizeof(value));
  }

  void write(const char *name, const BYTE *data, const DWORD size) const {
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

  RegRead(const HKEY root, const char *subkey) noexcept {
    RegOpenKeyExA(root, subkey, FALSE, KEY_READ, &key);
  }

  ~RegRead() { RegCloseKey(key); }

  std::optional<DWORD> read(const char *name) const {
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

  DWORD read(const char *name, LPBYTE out, DWORD out_size) const {
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
void add_about_menu_to_system_menu(const HWND window) {
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
void set_window_icons(const HWND window, const HINSTANCE instance) {
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
void set_window_text(const HWND hWnd, LPCSTR text,
                            LPSTR const text_end) {
  *text_end = '\0'; // because of C string
  SetWindowTextA(hWnd, text);
}

/**
 * Utility: get text from window and return its size.
 */
[[nodiscard]] UINT get_window_text(const HWND hWnd, CHAR *text,
                                          const int max_size) {
  return GetWindowTextA(hWnd, text, max_size);
}

#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
/**
 * Utility: set window text limit
 */
void set_window_text_limit(const HWND hWnd, WPARAM max_symbols) {
  SendMessageA(hWnd, EM_LIMITTEXT, max_symbols, 0);
}
#endif

/**
 * Utility: goto end of text in window and scroll caret to it
 */
void goto_end_of_window_text(const HWND hWnd) {
  SendMessageA(hWnd, EM_SETSEL, static_cast<WPARAM>(0),
               static_cast<LPARAM>(-1));
  SendMessageA(hWnd, EM_SCROLLCARET, 0, 0);
}

#ifdef CALC_SUPPORT_DPI_CHANGES
/**
 * Utility: return dpi for window
 */
UINT get_window_dpi(const HWND window) {
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
LONG to_physical(LONG value, LONG dpi) {
  return std::lroundf(static_cast<float>(value) * static_cast<float>(dpi) /
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI));
}

/**
 * Utility: coordinate convertor from physical to logical for window
 */
LONG to_logical(LONG value, LONG dpi) {
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

  constexpr auto get_x() const noexcept { return x; }

  constexpr auto get_y() const noexcept { return y; }
};

/**
 * Utility: helper to work with rectangles in window layout
 */
struct Rect : tagRECT {
  Rect() = default;

  constexpr Rect(const LONG x, const LONG y, const LONG width,
                 const LONG heigth) noexcept {
    left = x;
    right = x + width;
    top = y;
    bottom = y + heigth;
  }

  constexpr auto get_x() const noexcept { return left; }

  constexpr auto get_y() const noexcept { return top; }

  constexpr auto get_width() const noexcept { return right - left; }

  constexpr auto get_heigth() const noexcept { return bottom - top; }
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

  void init_window(const HWND window) {
    Rect client [[indeterminate]];
    GetClientRect(window, &client);

    width = client.right;
    height = client.bottom;
  }

  constexpr void init_min_sizes(const LONG _min_width, const LONG _min_height) {
    min_width = _min_width;
    min_height = _min_height;
  }

  void init_anchor(const HWND parent, const uint8_t index, const int id,
                   const Anchor anchor) {
    const auto handle = GetDlgItem(parent, id);

    Rect window [[indeterminate]];
    GetWindowRect(handle, &window);

    Point client_point(window.get_x(), window.get_y());
    ScreenToClient(parent, &client_point);
    const Rect client(client_point.get_x(), client_point.get_y(),
                      window.get_width(), window.get_heigth());

    constraints[index].init(this, handle, client, anchor);
  }

  void resize(const LONG new_width, const LONG new_height) {
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

  constexpr auto get_width() const { return width; }

  constexpr auto get_height() const { return height; }

  constexpr auto get_min_width() const { return min_width; }

  constexpr auto get_min_x() const { return min_width; }

  constexpr auto get_min_height() const { return min_height; }

  constexpr auto get_min_y() const { return min_height; }

  constexpr auto get_handle(const UINT index) const {
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
