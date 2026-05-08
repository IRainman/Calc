/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef GUI_HPP
#define GUI_HPP

#define _CRT_SECURE_NO_WARNINGS

#ifdef _WIN32

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

#include "resource.h" // main symbols

struct CalcConfiguration {
  static constexpr const char *reg_key = "Software\\HedgehogInTheCPP\\Calc";
  constexpr static LONG min_width = 292;  // matches RC
  constexpr static LONG min_height = 357; // matches RC
  constexpr static size_t elements = 4;   // matches RC

  constexpr static UINT default_shift_px = 100;

  // https://learn.microsoft.com/windows/win32/controls/em-limittext
  constexpr static size_t input_max_data_size =
#ifdef CALC_SUPPORT_SET_LIMIT_TEXT
      // Win32 ANSI multiline EDIT max is 64 KiB for classic Edit control:
      64 * 1024;
#else
      // Win32 ANSI multiline EDIT max is 32 KiB for classic Edit control:
      32 * 1024;
#endif
  // because the number above including C zero terminator maximum symbols are:
  constexpr static size_t input_max_symbols = input_max_data_size - 1;
};

struct RegWrite {
  RegWrite() = delete;

  RegWrite(const HKEY root, const char *subkey) noexcept {
    RegCreateKeyExA(root, subkey, 0, nullptr, REG_OPTION_NON_VOLATILE,
                    KEY_WRITE, nullptr, &key, nullptr);
  }

  ~RegWrite() { RegCloseKey(key); }

  void write(const char *name, const DWORD value) const {
    RegSetValueExA(key, name, 0, REG_DWORD,
                   reinterpret_cast<const BYTE *>(&value), sizeof(value));
  }

  void write(const char *name, const BYTE *data, const DWORD size) const {
    RegSetValueExA(key, name, 0, REG_BINARY, data, size);
  }

private:
  HKEY key;
};

struct RegRead {
  RegRead() = delete;

  RegRead(const HKEY root, const char *subkey) noexcept {
    RegOpenKeyExA(root, subkey, 0, KEY_READ, &key);
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

// ------------------------------------------------------------------
// Utility: add "About..." menu item to system menu.
// ------------------------------------------------------------------
static void add_about_menu_to_system_menu(const HWND window) {
  // IDM_ABOUTBOX must be in the system command range.
  static_assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  static_assert(IDM_ABOUTBOX < 0xF000);

  const auto system_menu = GetSystemMenu(window, FALSE);
  AppendMenuA(system_menu, MF_SEPARATOR, 0, nullptr);
  AppendMenuA(system_menu, MF_STRING, IDM_ABOUTBOX, "&About...");
}

// ------------------------------------------------------------------
// Utility: set window icon (small & big) from IDR_MAINFRAME
// ------------------------------------------------------------------
static void set_window_icons(const HWND window) {
  const auto icon =
      LoadIconA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDR_MAINFRAME));
  SendMessageA(window, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
  SendMessageA(window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
}

#ifdef CALC_SUPPORT_DPI_CHANGES
// ------------------------------------------------------------------
// Utility:
// ------------------------------------------------------------------
static UINT get_window_dpi(const HWND window) {
#ifdef CALC_SUPPORT_PER_WINDOW_DPI
  // Use per-window DPI if available.
  return GetDpiForWindow(window);
#else
  // Get device caps from nearest monitor / device context.
  const auto dc = GetDC(window);
  const auto dpiY = GetDeviceCaps(dc, LOGPIXELSY);
  ReleaseDC(window, dc);
  return dpiY;
#endif
}
#endif

struct Point : tagPOINT {
  Point() = default;

  constexpr Point(const LONG _x, const LONG _y) noexcept {
    x = _x;
    y = _y;
  }

  constexpr auto get_x() const noexcept { return x; }

  constexpr auto get_y() const noexcept { return y; }
};

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

#ifdef CALC_SUPPORT_MONITOR_API
// ------------------------------------------------------------------
// Utility: center window on monitor
// ------------------------------------------------------------------
static void center_window_on_monitor(const HWND window,
                                     const HMONITOR monitor) {
  Rect wr [[indeterminate]];
  if (GetWindowRect(window, &wr)) {

    MONITORINFO mi [[indeterminate]];
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoA(monitor, &mi)) {
      SystemParametersInfoW(SPI_GETWORKAREA, 0, &mi.rcWork, 0);
    }
    const auto x = mi.rcWork.left +
                   ((mi.rcWork.right - mi.rcWork.left) - wr.get_width()) / 2;
    const auto y = mi.rcWork.top +
                   ((mi.rcWork.bottom - mi.rcWork.top) - wr.get_heigth()) / 2;
    SetWindowPos(window, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
  } else {
    SetWindowPos(window, nullptr, CalcConfiguration::default_shift_px,
                 CalcConfiguration::default_shift_px, 0, 0,
                 SWP_NOZORDER | SWP_NOSIZE);
  }
}
#endif

class Layout {
public:
  enum class HMode : BYTE { Left, Right, Stretch };
  enum class VMode : BYTE { Top, Bottom, Stretch };

  struct Anchor {
    HMode horizontal [[indeterminate]];
    VMode vertical [[indeterminate]];
  };

  struct Constraint {
    constexpr void init(const Layout *layout, const HWND _handle,
                        const Rect &client, const Anchor _anchor) noexcept {
      handle = _handle;

      relative_margins.left = client.left;
      relative_margins.right = layout->width - client.right;

      relative_margins.top = client.top;
      relative_margins.bottom = layout->height - client.bottom;

      width = client.get_width();
      height = client.get_heigth();

      anchor = _anchor;
    }

    HWND handle [[indeterminate]];
    Rect relative_margins [[indeterminate]];
    LONG width [[indeterminate]];
    LONG height [[indeterminate]];
    Anchor anchor [[indeterminate]];
  };

  void init_window(const HWND window) {
    Rect client [[indeterminate]];
    GetClientRect(window, &client);

    width = client.right;
    height = client.bottom;
  }

  void init_min_sizes(const HWND window, const LONG requested_min_width,
                      const LONG requested_min_height
#ifdef CALC_SUPPORT_PER_WINDOW_DPI
                      ,
                      UINT dpi
#endif
  ) {
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
#ifdef CALC_SUPPORT_PER_WINDOW_DPI
    AdjustWindowRectExForDpi(&requiredClient, style, FALSE, exStyle, dpi);
#else
    AdjustWindowRectEx(&requiredClient, style, FALSE, exStyle);
#endif
    min_width = requiredClient.get_width();
    min_height = requiredClient.get_heigth();
  }

  void init_anchor(const HWND parent, const size_t index, const int id,
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
    for (auto &c : constraints) {
      Rect rect [[indeterminate]];

      switch (c.anchor.horizontal) {
      case HMode::Left:
        rect.right = c.relative_margins.left + c.width;
        rect.left = c.relative_margins.left;
        break;

      case HMode::Right:
        rect.right = width - c.relative_margins.right;
        rect.left = rect.right - c.width;
        break;

      case HMode::Stretch:
        rect.right = width - c.relative_margins.right;
        rect.left = c.relative_margins.left;
        break;

      default:
        std::unreachable();
      }

      switch (c.anchor.vertical) {
      case VMode::Top:
        rect.bottom = c.relative_margins.top + c.height;
        rect.top = c.relative_margins.top;
        break;

      case VMode::Bottom:
        rect.bottom = height - c.relative_margins.bottom;
        rect.top = rect.bottom - c.height;
        break;

      case VMode::Stretch:
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

  constexpr auto get_min_width() const { return min_width; }

  constexpr auto get_min_x() const { return min_width; }

  constexpr auto get_min_height() const { return min_height; }

  constexpr auto get_min_y() const { return min_height; }

  constexpr auto get_constraints_handle(const size_t index) const {
    return constraints[index].handle;
  }

private:
  std::array<Constraint, CalcConfiguration::elements> constraints
      [[indeterminate]];

  LONG width [[indeterminate]];
  LONG height [[indeterminate]];

  LONG min_width [[indeterminate]];
  LONG min_height [[indeterminate]];
};

#else

#include <Calc.h>
#include <wx/wx.h>

#endif
#endif
