// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2025 Solomina Elle, a.rainman on gmail point com
 */
 
// GUI.cpp : Defines the class behaviors for the application.
//

#include "pch.h"

#include "lexer.h"
#include "parser.h"
#include "issue_manager.h"
#include "formatter.h"
#ifdef CALC_TESTS_ENABLED
#include "tests.h"
#endif

// ================= GUI ===============

#ifdef _WIN32

#include <vector>
#include <optional>
#include <cstdint>
#include <immintrin.h>
#include <algorithm>

//#define CALC_SUPPORT_WINDOWS_XP // deprecated by Windows SDK
//#define CALC_SUPPORT_WINDOWS_7_8_81 // deprecated by Windows SDK since version 18 of the MSVC compiler.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // Ensure that we using min/max from std algorithms.

#include "targetver.h"


#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <imm.h>
#include <shellapi.h>
#include "resource.h"		// main symbols

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "imm32.lib")
#pragma comment(lib, "Version.lib") // needed for GetFileVersionInfo*

// ---------- configuration ----------
static constexpr std::string_view kRegKey = "Software\\HedgehogInTheCPP\\Calc";

// ANSI multiline EDIT max is 64 KiB for classic Edit control
static std::array<char, 64 * 1024> g_input; // 65535 for symbols and 1 for null C string API terminator

// baseline DPI
static constexpr UINT kBaselineDpi = 96u;

// ---------- anchors ----------
struct Anchor {
    int id;
    RECT rcBaseline; // client coords at baseline DPI
    bool left, top, right, bottom;
};
static std::vector<Anchor> g_anchors;
static UINT g_minW = 260u; // minimal window width (pixels)
static UINT g_minH = 180u; // minimal window height
static UINT g_baseClientW = g_minW;
static UINT g_baseClientH = g_minH;
static UINT g_currentDpi = kBaselineDpi;
static HFONT g_hFontDialog = nullptr;

// ---------- registry helpers ----------
static void RegWriteDword(HKEY root, std::string_view subkey, std::string_view name, DWORD value) {
    HKEY hKey [[indeterminate]];
    if (RegCreateKeyExA(root, subkey.data(), 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, name.data(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&value), sizeof(value));
        RegCloseKey(hKey);
    }
}

static std::optional<DWORD> RegReadDword(HKEY root, std::string_view subkey, std::string_view name) {
    HKEY hKey [[indeterminate]];
    if (RegOpenKeyExA(root, subkey.data(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) return std::nullopt;
    DWORD out [[indeterminate]]; DWORD outSize = sizeof(out); DWORD type [[indeterminate]];
    if (RegQueryValueExA(hKey, name.data(), nullptr, &type, reinterpret_cast<LPBYTE>(&out), &outSize) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return std::nullopt;
    }
    RegCloseKey(hKey);
    if (type != REG_DWORD) {
        return std::nullopt;
    }
    return out;
}

// ---------- version string reading (uses Version APIs) ----------
static std::string ReadFileVersionString() {
    const auto module = GetModuleHandleA(nullptr);
    if (!module) return {};

    std::string path; path.resize(MAX_PATH);
    const auto len = GetModuleFileNameA(module, path.data(), static_cast<DWORD>(path.size()));
    if (len == 0) return {};
    if (len >= path.size()) {
        // filename longer than MAX_PATH: resize and try again (rare)
        path.resize(len + 1);
        const auto len2 = GetModuleFileNameA(module, path.data(), static_cast<DWORD>(path.size()));
        if (len2 == 0 || len2 >= path.size()) return {};
    }
    // null-terminate properly
    path.resize(len); path.push_back('\0');

    const DWORD verSize = GetFileVersionInfoSizeA(path.data(), nullptr);
    if (verSize == 0) return {};

    std::string buffer;
    buffer.resize(verSize);
    if (!GetFileVersionInfoA(path.data(), 0, verSize, buffer.data())) return {};

    struct LANGANDCODEPAGE { WORD wLanguage; WORD wCodePage; };
    LANGANDCODEPAGE* trans [[indeterminate]]; UINT transLen [[indeterminate]];
    if (!VerQueryValueA(buffer.data(), "\\VarFileInfo\\Translation", reinterpret_cast<LPVOID*>(&trans), &transLen)) {
        // fallback to usual key
        LPVOID data [[indeterminate]]; UINT dataLen [[indeterminate]];
        if (VerQueryValueA(buffer.data(), "\\StringFileInfo\\040904b0\\FileVersion", &data, &dataLen) && dataLen) {
            return std::string(static_cast<const char*>(data), static_cast<std::size_t>(dataLen));
        }
        return {};
    }
    if (transLen < sizeof(LANGANDCODEPAGE)) return {};
    // build query key using std::format and then query
    const auto key = fmt::format("\\StringFileInfo\\{:04x}{:04x}\\FileVersion", trans->wLanguage, trans->wCodePage);
    LPVOID verData [[indeterminate]]; UINT verLen [[indeterminate]];
    if (!VerQueryValueA(buffer.data(), key.c_str(), &verData, &verLen) || verLen == 0) return {};
    return std::string(static_cast<const char*>(verData), static_cast<std::size_t>(verLen));
}

// ---------- DPI helpers ----------
using SetProcessDpiAwarenessContext_t = BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT);
static SetProcessDpiAwarenessContext_t pSetProcessDpiAwarenessContext = nullptr;
using SetProcessDpiAwareness_t = HRESULT(WINAPI*)(int);
static SetProcessDpiAwareness_t pSetProcessDpiAwareness = nullptr;

static void TryEnableDpiAwareness() {
    const auto user32 = GetModuleHandleA("user32.dll");
    if (user32) {
        pSetProcessDpiAwarenessContext = reinterpret_cast<SetProcessDpiAwarenessContext_t>(GetProcAddress(user32, "SetProcessDpiAwarenessContext"));
        if (pSetProcessDpiAwarenessContext) {
#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
#endif
            if (pSetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) return;
        }
    }
    const auto shcore = GetModuleHandleA("shcore.dll");
    if (shcore) {
        pSetProcessDpiAwareness = reinterpret_cast<SetProcessDpiAwareness_t>(GetProcAddress(shcore, "SetProcessDpiAwareness"));
        if (pSetProcessDpiAwareness) { pSetProcessDpiAwareness(2); return; } // PROCESS_PER_MONITOR_DPI_AWARE
    }
    if (user32) {
        using SetProcessDPIAware_t = BOOL(WINAPI*)();
        const auto fn = reinterpret_cast<SetProcessDPIAware_t>(GetProcAddress(user32, "SetProcessDPIAware"));
        if (fn) fn();
    }
}

static void TryEnableDarkModeForWindow(HWND hWnd, const BOOL enable) {
    const auto hDwm = GetModuleHandleA("dwmapi.dll");
    if (!hDwm) return;
    using DwmSetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);
    const auto pDwmSetWindowAttribute = reinterpret_cast<DwmSetWindowAttribute_t>(GetProcAddress(hDwm, "DwmSetWindowAttribute"));
    if (!pDwmSetWindowAttribute) return;
    constexpr int attrs[] = { 20, 19 };
    for (const auto a : attrs) {
        (void)pDwmSetWindowAttribute(hWnd, static_cast<DWORD>(a), &enable, sizeof(enable));
    }
}

static HFONT CreateDialogFontForDpi(UINT dpi) {
    const int pointSize = 10;
    const int height = -MulDiv(pointSize, static_cast<int>(dpi), 72);
    return CreateFontA(height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, "Segoe UI");
}

// ---------- Anchors ----------
static void InitAnchors(HWND hDlg) {
    g_anchors.clear();
    RECT client [[indeterminate]]; if (!GetClientRect(hDlg, &client)) return;
    g_baseClientW = static_cast<UINT>(client.right - client.left);
    g_baseClientH = static_cast<UINT>(client.bottom - client.top);

    const auto add = [&](int id, bool l, bool t, bool r, bool b) {
        const auto h = GetDlgItem(hDlg, id);
        if (!h) return;

        RECT wr [[indeterminate]]; if (!GetWindowRect(h, &wr)) return;
        POINT p{ wr.left, wr.top }; ScreenToClient(hDlg, &p);
        RECT rcClient{ p.x, p.y, p.x + (wr.right - wr.left), p.y + (wr.bottom - wr.top) };
        g_anchors.push_back({ id, rcClient, l, t, r, b });
    };

    add(IDC_EDIT_INPUT, true, true, true, true);    // flexible input
    add(IDC_EDIT_RESULT, true, false, true, true);  // bottom anchored
    add(IDC_EDIT_MESSAGE, true, false, true, true); // bottom anchored
    add(IDC_BUTTON_CALC, false, false, true, true); // button bottom-right
}

static inline int ScaleForDpi(int baselinePx, UINT targetDpi) {
    return MulDiv(baselinePx, static_cast<int>(targetDpi), static_cast<int>(kBaselineDpi));
}

static void ResizeAnchoredControls(HWND hDlg, UINT clientW, UINT clientH, UINT dpi) {
    if (g_anchors.empty()) return;

    const auto baseW = (g_baseClientW ? g_baseClientW : 1u);
    const auto baseH = (g_baseClientH ? g_baseClientH : 1u);

    const auto dpiScaledClientW = ScaleForDpi(static_cast<int>(baseW), dpi);
    const auto dpiScaledClientH = ScaleForDpi(static_cast<int>(baseH), dpi);

    for (const auto& a : g_anchors) {
        const RECT& rc = a.rcBaseline;
        const int leftB = rc.left, topB = rc.top, rightB = rc.right, bottomB = rc.bottom;
        const int widthB = rightB - leftB, heightB = bottomB - topB;

        const auto leftS = ScaleForDpi(leftB, dpi);
        const auto topS = ScaleForDpi(topB, dpi);
        const auto rightS = ScaleForDpi(rightB, dpi);
        const auto bottomS = ScaleForDpi(bottomB, dpi);

        auto newLeft = leftS, newTop = topS, newRight = rightS, newBottom = bottomS;

        const auto deltaW = static_cast<int>(clientW) - dpiScaledClientW;
        const auto deltaH = static_cast<int>(clientH) - dpiScaledClientH;

        // horizontal
        if (a.left && a.right) {
            newRight = rightS + deltaW; newLeft = leftS;
        }
        else if (!a.left && a.right) {
            const auto distRight = dpiScaledClientW - rightS;
            newRight = static_cast<int>(clientW) - distRight;
            newLeft = newRight - ScaleForDpi(widthB, dpi);
        }
        else {
            newLeft = leftS; newRight = leftS + ScaleForDpi(widthB, dpi);
        }

        // vertical
        if (a.top && a.bottom) {
            newBottom = bottomS + deltaH; newTop = topS;
        }
        else if (!a.top && a.bottom) {
            const auto distBottom = dpiScaledClientH - bottomS;
            newBottom = static_cast<int>(clientH) - distBottom;
            newTop = newBottom - ScaleForDpi(heightB, dpi);
        }
        else {
            newTop = topS; newBottom = topS + ScaleForDpi(heightB, dpi);
        }

        if (newLeft < 0) newLeft = 0;
        if (newTop < 0) newTop = 0;
        if (newRight < newLeft + 4) newRight = newLeft + 4;
        if (newBottom < newTop + 4) newBottom = newTop + 4;

        const auto hCtrl = GetDlgItem(hDlg, a.id);
        if (hCtrl) MoveWindow(hCtrl, newLeft, newTop, newRight - newLeft, newBottom - newTop, TRUE);
    }
}

// ---------- placement ----------
static void SaveWindowPlacement(HWND hWnd) {
    WINDOWPLACEMENT wp [[indeterminate]]; wp.length = sizeof(wp);
    if (!GetWindowPlacement(hWnd, &wp)) return;
    RegWriteDword(HKEY_CURRENT_USER, kRegKey, "showCmd", static_cast<DWORD>(wp.showCmd));
    RegWriteDword(HKEY_CURRENT_USER, kRegKey, "flags", static_cast<DWORD>(wp.flags));
    RegWriteDword(HKEY_CURRENT_USER, kRegKey, "left", static_cast<DWORD>(wp.rcNormalPosition.left));
    RegWriteDword(HKEY_CURRENT_USER, kRegKey, "top", static_cast<DWORD>(wp.rcNormalPosition.top));
    RegWriteDword(HKEY_CURRENT_USER, kRegKey, "right", static_cast<DWORD>(wp.rcNormalPosition.right));
    RegWriteDword(HKEY_CURRENT_USER, kRegKey, "bottom", static_cast<DWORD>(wp.rcNormalPosition.bottom));
}

static void CenterWindowOnMonitor(HWND hWnd, HMONITOR hMon) {
    RECT wr [[indeterminate]]; if (!GetWindowRect(hWnd, &wr)) { SetWindowPos(hWnd, nullptr, 100, 100, 0, 0, SWP_NOZORDER | SWP_NOSIZE); return; }
    MONITORINFO mi [[indeterminate]]; mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(hMon, &mi)) SystemParametersInfoW(SPI_GETWORKAREA, 0, &mi.rcWork, 0);
    const auto w = wr.right - wr.left, h = wr.bottom - wr.top;
    const auto x = mi.rcWork.left + ((mi.rcWork.right - mi.rcWork.left) - w) / 2;
    const auto y = mi.rcWork.top + ((mi.rcWork.bottom - mi.rcWork.top) - h) / 2;
    SetWindowPos(hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

static void RestoreWindowPlacement(HWND hWnd, [[maybe_unused]] UINT dpi) {
    const auto sShow = RegReadDword(HKEY_CURRENT_USER, kRegKey, "showCmd");
    const auto sFlags = RegReadDword(HKEY_CURRENT_USER, kRegKey, "flags");
    const auto sLeft = RegReadDword(HKEY_CURRENT_USER, kRegKey, "left");
    const auto sTop = RegReadDword(HKEY_CURRENT_USER, kRegKey, "top");
    const auto sRight = RegReadDword(HKEY_CURRENT_USER, kRegKey, "right");
    const auto sBottom = RegReadDword(HKEY_CURRENT_USER, kRegKey, "bottom");

    if (!sLeft || !sTop || !sRight || !sBottom) {
        POINT pt [[indeterminate]]; GetCursorPos(&pt);
        const auto hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        CenterWindowOnMonitor(hWnd, hMon);
        return;
    }

    WINDOWPLACEMENT wp [[indeterminate]]; wp.length = sizeof(wp);
    wp.showCmd = static_cast<int>(sShow ? *sShow : SW_SHOWNORMAL);
    wp.flags = static_cast<UINT>(sFlags ? *sFlags : 0u);
    wp.rcNormalPosition.left = static_cast<LONG>(*sLeft);
    wp.rcNormalPosition.top = static_cast<LONG>(*sTop);
    wp.rcNormalPosition.right = static_cast<LONG>(*sRight);
    wp.rcNormalPosition.bottom = static_cast<LONG>(*sBottom);

    const auto hMon = MonitorFromRect(&wp.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi [[indeterminate]]; mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoA(hMon, &mi)) { POINT pt; GetCursorPos(&pt); const auto hMon2 = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST); CenterWindowOnMonitor(hWnd, hMon2); return; }
    const RECT& work = mi.rcWork;
    if (wp.rcNormalPosition.right <= work.left || wp.rcNormalPosition.left >= work.right || wp.rcNormalPosition.bottom <= work.top || wp.rcNormalPosition.top >= work.bottom) {
        CenterWindowOnMonitor(hWnd, hMon);
        return;
    }

    const auto width = static_cast<UINT>(wp.rcNormalPosition.right - wp.rcNormalPosition.left);
    const auto height = static_cast<UINT>(wp.rcNormalPosition.bottom - wp.rcNormalPosition.top);
    if (width < g_minW || height < g_minH) {
        SetWindowPos(hWnd, nullptr, work.left + static_cast<int>((work.right - work.left - g_minW) / 2), work.top + static_cast<int>((work.bottom - work.top - g_minH) / 2), static_cast<int>(g_minW), static_cast<int>(g_minH), SWP_NOZORDER | SWP_NOACTIVATE);
        return;
    }
    SetWindowPlacement(hWnd, &wp);
}

// ---------- Create menu (used) ----------
static void CreateAndAttachMenu(HWND hWnd) {
    // Add "About..." menu item to system menu.
    // IDM_ABOUTBOX must be in the system command range.
    static_assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    static_assert(IDM_ABOUTBOX < 0xF000);

    const auto hMenu = CreateMenu();
    if (!hMenu) return;
    const auto hFile = CreatePopupMenu();
    if (!hFile) { DestroyMenu(hMenu); return; }
    AppendMenuA(hFile, MF_STRING, IDM_ABOUTBOX, "&About...");
    AppendMenuA(hFile, MF_SEPARATOR, 0, nullptr);
    AppendMenuA(hFile, MF_STRING, ID_CANCEL, "E&xit");
    AppendMenuA(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hFile), "File");
    SetMenu(hWnd, hMenu);

    /*
    const auto pSysMenu = GetSystemMenu(hWnd, FALSE);
    AppendMenuA(pSysMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuA(pSysMenu, MF_STRING, IDM_ABOUTBOX, "&About...");
    AppendMenuA(pSysMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuA(pSysMenu, MF_STRING, ID_CANCEL, "E&xit");
    */

    //auto m_hIcon = LoadIconA(reinterpret_cast<HINSTANCE>(GetWindowLongPtrA(hWnd, GWLP_HINSTANCE)), MAKEINTRESOURCEA(IDR_MAINFRAME));

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    //SetIconA(m_hIcon, TRUE);			// Set big icon
    //SetIconA(m_hIcon, FALSE);		// Set small icon
}

// ---------- RunCalcOnce ----------
static void RunCalcOnce(HWND hDlg) {
    const auto copied = GetDlgItemTextA(hDlg, IDC_EDIT_INPUT, g_input.data(), static_cast<int>(g_input.size()));
    if (!copied) {
        SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, "");
        SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, "");
    }
    else {
        const std::string_view sv{ g_input.data(), copied };

        Lexer l(sv);
        Parser p(l);
        const auto result = p.parse();

#ifndef CALC_USE_ERROR_TOKEN
        const auto hasErr = IssueManager::has_errors();
#endif

        if (hasErr) {
            SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, "");
            const auto summary = Formatter::create_summary();
            SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, summary.data());
#ifndef CALC_USE_ERROR_TOKEN
            IssueManager::clear();
#endif
        }
        else {
            const auto out = Formatter::format(result);
            SetDlgItemTextA(hDlg, IDC_EDIT_RESULT, out.data());
            SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, "");
        }
    }

    SetFocus(GetDlgItem(hDlg, IDC_EDIT_INPUT));
}

// ---------- Dialog proc ----------
static INT_PTR CALLBACK CalcDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG: {
        TryEnableDpiAwareness();

        // detect initial DPI
        auto dpi = kBaselineDpi;
        const auto user32 = GetModuleHandleA("user32.dll");
        if (user32) {
            using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
            const auto pGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(user32, "GetDpiForWindow"));
            if (pGetDpiForWindow) dpi = pGetDpiForWindow(hDlg);
        }
        g_currentDpi = dpi;

        // DPI scaled font
        g_hFontDialog = CreateDialogFontForDpi(g_currentDpi);
        if (g_hFontDialog) {
            const auto apply = [&](int id) {
                const auto h = GetDlgItem(hDlg, id);
                if (h) SendMessageA(h, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFontDialog), TRUE);
                };
            apply(IDC_EDIT_INPUT); apply(IDC_EDIT_RESULT); apply(IDC_EDIT_MESSAGE); apply(IDC_BUTTON_CALC); apply(IDC_LINK_HOMEPAGE);
        }

        // IME: associate default context for multiline edit
        const auto hInput = GetDlgItem(hDlg, IDC_EDIT_INPUT);
        if (hInput) ImmAssociateContextEx(hInput, nullptr, IACE_DEFAULT);

        // anchors and min size
        InitAnchors(hDlg);
        RECT wr [[indeterminate]]; if (GetWindowRect(hDlg, &wr)) { g_minW = static_cast<UINT>(wr.right - wr.left); g_minH = static_cast<UINT>(wr.bottom - wr.top); }

        // create menu
        CreateAndAttachMenu(hDlg);

        // restore placement (center if no stored)
        RestoreWindowPlacement(hDlg, g_currentDpi);

        // try dark mode
        TryEnableDarkModeForWindow(hDlg, TRUE);

#ifdef CALC_TESTS_ENABLED
        SetDlgItemTextA(hDlg, IDC_EDIT_MESSAGE, calc_tests().c_str());
#endif

        SetFocus(hInput);
        return FALSE;
    }

    case WM_DPICHANGED: {
        const UINT newDpi = HIWORD(wParam);
        if (newDpi == 0) break;
        g_currentDpi = newDpi;
        if (lParam) {
            const auto prc = reinterpret_cast<RECT*>(lParam);
            SetWindowPos(hDlg, nullptr, prc->left, prc->top, prc->right - prc->left, prc->bottom - prc->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (g_hFontDialog) { DeleteObject(g_hFontDialog); g_hFontDialog = nullptr; }
        g_hFontDialog = CreateDialogFontForDpi(g_currentDpi);
        if (g_hFontDialog) {
            const auto apply = [&](int id) {
                const auto h = GetDlgItem(hDlg, id);
                if (h) SendMessageA(h, WM_SETFONT, reinterpret_cast<WPARAM>(g_hFontDialog), TRUE);
                };
            apply(IDC_EDIT_INPUT); apply(IDC_EDIT_RESULT); apply(IDC_EDIT_MESSAGE); apply(IDC_BUTTON_CALC); apply(IDC_LINK_HOMEPAGE);
        }
        RECT client [[indeterminate]]; GetClientRect(hDlg, &client);
        ResizeAnchoredControls(hDlg, static_cast<UINT>(client.right - client.left), static_cast<UINT>(client.bottom - client.top), g_currentDpi);
        return TRUE;
    }

    case WM_SIZE: {
        const auto newW = static_cast<UINT>(LOWORD(lParam));
        const auto newH = static_cast<UINT>(HIWORD(lParam));
        // enforce minimal size
        const auto enforcedW = std::max(newW, g_minW);
        const auto enforcedH = std::max(newH, g_minH);
        ResizeAnchoredControls(hDlg, enforcedW, enforcedH, g_currentDpi);
        return TRUE;
    }

    case WM_GETMINMAXINFO: {
        const auto lpMMI = reinterpret_cast<LPMINMAXINFO>(lParam);
        if (lpMMI) {
            lpMMI->ptMinTrackSize.x = static_cast<LONG>(g_minW);
            lpMMI->ptMinTrackSize.y = static_cast<LONG>(g_minH);
        }
        return 0;
    }

    case WM_COMMAND: {
        const auto id = static_cast<int>(LOWORD(wParam));
        const auto notify = static_cast<int>(HIWORD(wParam));
        
        if (id == IDC_BUTTON_CALC && notify == BN_CLICKED) {
            RunCalcOnce(hDlg);
            return TRUE;
        }
        else if (id == IDM_ABOUTBOX) {
            const auto ver = ReadFileVersionString();
            std::string about;
            if (!ver.empty()) { about = "Calc "; about += ver; }
            else about = "Calc";
            about += "\r\nHedgehogInTheCPP";
            MessageBoxA(hDlg, about.c_str(), "About", MB_OK | MB_ICONINFORMATION);
            return TRUE;
        }
        else if (id == IDC_LINK_HOMEPAGE && (notify == NM_CLICK || notify == NM_RETURN)) {
            const auto link = reinterpret_cast<NMLINK*>(lParam);
            if (link && link->item.szUrl) {
                ShellExecuteW(nullptr, L"open", link->item.szUrl, nullptr, nullptr, SW_SHOWNORMAL);
            }
            return TRUE;
        }
        else if (id == ID_CANCEL) {
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;
    }

    case WM_CLOSE:
        SaveWindowPlacement(hDlg);
        EndDialog(hDlg, 0);
        return TRUE;

    case WM_DESTROY:
        if (g_hFontDialog) { DeleteObject(g_hFontDialog); g_hFontDialog = nullptr; }
        return FALSE;
    }

    return FALSE;
}

// ---------- WinMain ----------
int WINAPI WinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrev, [[maybe_unused]] LPSTR lpCmdLine, int) {

    // TODO: move this code to the core.
    /* Applications that generate floating point underflow in vector registers can benefit from setting the flush-to-zero mode rather than generating subnormal numbers in case of underflow:*/
    /* It is strongly recommended to set the flush-to-zero mode unless you have special reasons to use subnormal numbers. You may, in addition, set the denormals-are-zero mode if vector regsiters are available:*/
    // Set flush-to-zero and denormals-are-zero mode (SSE2):
    _mm_setcsr(_mm_getcsr() | 0x8040);

    // TODO: move this code to the core.
#if !defined(CALC_USE_ERROR_TOKEN) && !defined(CALC_USING_STATIC_VECTOR)
    IssueManager::speedup();
#endif

    TryEnableDpiAwareness();

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls [[indeterminate]];
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_STANDARD_CLASSES | ICC_LINK_CLASS | ICC_NATIVEFNTCTL_CLASS | ICC_PAGESCROLLER_CLASS | ICC_COOL_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    const auto ret = DialogBoxParamA(hInstance, MAKEINTRESOURCEA(IDD_CALC_DIALOG), nullptr, CalcDialogProc, 0);
    if (ret == -1) {
        MessageBoxA(nullptr, "Failed to create main dialog.", "Startup Error", MB_OK | MB_ICONERROR);
        return -1;
    }
    return 0;
}

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#else

// add other platforms here.

#endif // _WIN32
