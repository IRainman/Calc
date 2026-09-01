/*
 * Copyright 2009-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef GUI_HPP
#define GUI_HPP

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
 * Check that unicode is enabled
 */

#if !defined(_UNICODE)
#error                                                                         \
    "Because _UNICODE isn't enabled we can't effectively proccess user input and user can't use mathematical symbols."
#endif

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
#ifndef CALC_SUPPORT_DARK_MODE
#define NOCOLOR -Screen colors
#endif
//  #define NOCTLMGR -Control and Dialog routines
#define NODRAWTEXT -DrawText() and DT_ *
#ifndef CALC_SUPPORT_DARK_MODE
#define NOGDI -All GDI defines and routines
#endif
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

__pragma(warning(push));
__pragma(warning(disable : 5039));
__pragma(warning(disable : 4865));
#include <windows.h>
#ifdef CALC_SUPPORT_LINK_WINDOW
#include <commctrl.h>
#include <shellapi.h>
#endif
#ifdef CALC_SUPPORT_DARK_MODE
#include <dwmapi.h>
#include <uxtheme.h>
#endif
__pragma(warning(pop));

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
      // Win32 multiline EDIT max is 64 KiB for classic Edit control:
      (64 * 1024) * sizeof(WCHAR);
#else
      // Win32 multiline EDIT max is 32 KiB for classic Edit control:
      (32 * 1024) * sizeof(WCHAR);
#endif
  static constexpr UINT input_max_text_length = input_max_data_size / sizeof(WCHAR);
};

/**
 * The normalization needed for preprocessing and converting mathematical
 * values, like 𝜋 -> pi i.e to it's ANSI representation that Calc understand.
 * TODO: this function not transform any syntax construction!
 */
[[nodiscard]] static bool normalize_equasion(LPCWSTR input, const UINT size,
                                             std::string &output) noexcept {

  for (const auto end = input + size; input != end; ++input) [[likely]] {
    const auto &c = *input;

    // Process ANSI part:
    switch (c) {
    // Cleanup all string separation and formatting:
    case WCHAR('\t'):
    case WCHAR('\n'):
    case WCHAR('\v'):
    case WCHAR('\f'):
    case WCHAR('\r'):
    case WCHAR(' '):
      // output.push_back(' ');
      break;
    default:
      break;
    }

    // All other ANSI chars converted directly
    if (c <= WCHAR(0x7F)) {
      output.push_back(static_cast<char>(c));
      continue;
    }

    // Process Unicode part:
    switch (c) {
    // Whitespace accepted by mathematical input.
    case u'\u00A0': // NBSP
    case u'\u2000': // EN QUAD
    case u'\u2001': // EM QUAD
    case u'\u2002': // EN SPACE
    case u'\u2003': // EM SPACE
    case u'\u2004':
    case u'\u2005':
    case u'\u2006':
    case u'\u2007':
    case u'\u2008':
    case u'\u2009': // THIN SPACE
    case u'\u200A': // HAIR SPACE
    case u'\u202F': // NARROW NBSP
    case u'\u205F': // MEDIUM MATHEMATICAL SPACE
    case u'\u3000': // IDEOGRAPHIC SPACE
      // output.push_back(' ');
      break;

    // Fullwidth digits.
    case u'\uFF10':
      output.push_back('0');
      break;
    case u'\uFF11':
      output.push_back('1');
      break;
    case u'\uFF12':
      output.push_back('2');
      break;
    case u'\uFF13':
      output.push_back('3');
      break;
    case u'\uFF14':
      output.push_back('4');
      break;
    case u'\uFF15':
      output.push_back('5');
      break;
    case u'\uFF16':
      output.push_back('6');
      break;
    case u'\uFF17':
      output.push_back('7');
      break;
    case u'\uFF18':
      output.push_back('8');
      break;
    case u'\uFF19':
      output.push_back('9');
      break;

    // Fullwidth operators/punctuation.
    case u'\uFF0B':
      output.push_back('+');
      break; // ＋
    case u'\uFF0D':
      output.push_back('-');
      break; // －
    case u'\uFF0A':
      output.push_back('*');
      break; // ＊
    case u'\uFF0F':
      output.push_back('/');
      break; // ／
    case u'\uFF08':
      output.push_back('(');
      break; // （
    case u'\uFF09':
      output.push_back(')');
      break; // ）
    case u'\uFF0C':
      output.push_back(',');
      break; // ，
    case u'\uFF05':
      output.push_back('%');
      break; // ％

    // Alternative minus characters.
    case u'\u2010': // ‐
    case u'\u2011': // -
    case u'\u2012': // ‒
    case u'\u2013': // –
    case u'\u2014': // —
    case u'\u2212': // −
    case u'\uFE63': // ﹣
      output.push_back('-');
      break;

    // Multiplication.
    case u'\u00B7': // ·
    case u'\u00D7': // ×
    case u'\u2217': // ∗
    case u'\u2219': // ∙
    case u'\u22C5': // ⋅
    case u'\u204E': // ⁎
    case u'\u2A2F': // ⨯
      output.push_back('*');
      break;

    // Division.
    case u'\u00F7': // ÷
    case u'\u2044': // ⁄
    case u'\u2215': // ∕
      output.push_back('/');
      break;

    // Parentheses.
    case u'\uFE59': // ﹙
      output.push_back('(');
      break;

    case u'\uFE5A': // ﹚
      output.push_back(')');
      break;

    // Comma.
    case u'\uFE50': // ﹐
      output.push_back(',');
      break;

    // Mathematical constants.
    case u'\u03C0': // π
    case u'\u03D6': // ϖ
      output.append("pi");
      break;

    case u'\u03C6': // φ
    case u'\u03D5': // ϕ
      output.append("phi");
      break;

    case u'\u03B3': // γ
      output.append("e_gamma");
      break;

    case u'\u03B1': // α
      output.append("alpha");
      break;

    case u'\u03C3': // σ
      output.append("sigma");
      break;

    case u'\u03BC': // μ
      output.append("mu");
      break;

    case u'\u221E': // ∞
      output.append("inf");
      break;

    case u'\u212F': // ℯ
      output.push_back('e');
      break;

    // Physical constants.
    case u'\u210F': // ℏ
      output.append("hbar");
      break;

    // Roots.
    case u'\u221A': // √
      output.append("sqrt");
      break;

    case u'\u221B': // ∛
      output.append("cbrt");
      break;

    // Superscript/subscript signs and digits.
    case u'\u207A': // ⁺
    case u'\u208A': // ₊
      output.push_back('+');
      break;

    case u'\u207B': // ⁻
    case u'\u208B': // ₋
      output.push_back('-');
      break;

    case u'\u207D': // ⁽
    case u'\u208D': // ₍
      output.push_back('(');
      break;

    case u'\u207E': // ⁾
    case u'\u208E': // ₎
      output.push_back(')');
      break;

    case u'\u2070': // ⁰
    case u'\u2080': // ₀
      output.push_back('0');
      break;

    case u'\u00B9': // ¹
    case u'\u2081': // ₁
      output.push_back('1');
      break;

    case u'\u00B2': // ²
    case u'\u2082': // ₂
      output.push_back('2');
      break;

    case u'\u00B3': // ³
    case u'\u2083': // ₃
      output.push_back('3');
      break;

    case u'\u2074': // ⁴
    case u'\u2084': // ₄
      output.push_back('4');
      break;

    case u'\u2075': // ⁵
    case u'\u2085': // ₅
      output.push_back('5');
      break;

    case u'\u2076': // ⁶
    case u'\u2086': // ₆
      output.push_back('6');
      break;

    case u'\u2077': // ⁷
    case u'\u2087': // ₇
      output.push_back('7');
      break;

    case u'\u2078': // ⁸
    case u'\u2088': // ₈
      output.push_back('8');
      break;

    case u'\u2079': // ⁹
    case u'\u2089': // ₉
      output.push_back('9');
      break;

      /*

  TODO: mapping and converting functionality

  ∑ → sum
  ∏ → prod
  ∫ → integral

  | Unicode | Calc |
  | ------- | ---- |
  | `-`     | `-`  |
  | `−`     | `-`  |
  | `‐`     | `-`  |
  | `-`     | `-`  |
  | `‒`     | `-`  |
  | `–`     | `-`  |
  | `—`     | `-`  |
  | `﹣`     | `-`  |
  | `－`     | `-`  |



  | Unicode | Calc |
  | ------- | ---- |
  | `+`     | `+`  |
  | `＋`     | `+`  |


  × ⋅ · ∙ ∗ ⨯ → *

  | Unicode | Calc                                                         |
  | ------- | ------------------------------------------------------------ |
  | `∞`     | `inf`                                                        |
  | `π`     | `pi`                                                         |
  | `ϖ`     | `pi`                                                         |
  | `e`     | `e`                                                          |
  | `ℯ`     | `e`                                                          |
  | `φ`     | `phi`                                                        |
  | `ϕ`     | `phi`                                                        |
  | `γ`     | `e_gamma`                                                    |
  | `ℇ`     | `e`                                                          |
  Γ → gamma

  | Unicode | Calc |
  | ------- | ---- |
  | `/`     | `/`  |
  | `÷`     | `/`  |
  | `∕`     | `/`  |
  | `⁄`     | `/`  |
  | `／`     | `/`  |

  | Unicode | Calc  |
  | ------- | ----- |
  | `%`     | `%`   |
  | `％`     | `%`   |
  | `mod`   | `mod` |


  | Unicode | Calc |
  | ------- | ---- |
  | `(`     | `(`  |
  | `（`     | `(`  |
  | `﹙`     | `(`  |
  | `)`     | `)`  |
  | `）`     | `)`  |
  | `﹚`     | `)`  |
  | `,`     | `,`  |
  | `，`     | `,`  |
  | `﹐`     | `,`  |

  ⟮ → (
  ⟯ → )

  ∜x → pow(x, 1/4)
  | Unicode | Calc      |
  | ------- | --------- |
  | `√`     | `sqrt`    |
  | `√x`    | `sqrt(x)` |
  | `∛`     | `cbrt`    |
  | `∛x`    | `cbrt(x)` |
  | `∜`     | `pow`*    |
  | `√`     | `sqrt`    |
  | `⎷`     | `sqrt`    |
  | `⏥`     | `sqrt`*   |



  | Unicode  | Calc     |
  | -------- | -------- |
  | `arcsin` | `arcsin` |
  | `asin`   | `arcsin` |
  | `sin⁻¹`  | `arcsin` |
  | `cos⁻¹`  | `arccos` |
  | `acos`   | `arccos` |
  | `tan⁻¹`  | `arctan` |
  | `atan`   | `arctan` |


  asin(x)    → arcsin(x)
  acos(x)    → arccos(x)
  atan(x)    → arctan(x)

  |  notation | Calc    |
  | ------------------ | ------- |
  | `sinh`             | `sh`    |
  | `cosh`             | `ch`    |
  | `tanh`             | `tanh`  |
  | `arsinh`           | `asinh` |
  | `arcsinh`          | `asinh` |
  | `arsinh`           | `asinh` |
  | `arcosh`           | `acosh` |
  | `arctanh`          | `atanh` |



  | Unicode    | Calc       |
  | ---------- | ---------- |
  | `㏑`        | `ln`       |
  | `㏒`        | `log`      |
  | `log₁₀`    | `log10`    |
  | `log₁₀(x)` | `log10(x)` |
  | `log₂`     | `log2`     |
  | `log₂(x)`  | `log2(x)`  |

  | Unicode / notation | Calc       |
  | ------------------ | ---------- |
  | `eˣ`               | `exp`*     |
  | `exp`              | `exp`      |
  | `e^x`              | `e^x`      |
  | `eˣ`               | `exp(x)`*  |
  | `expm1`            | `expm1`    |
  | `2ˣ`               | `exp2(x)`* |


  | Unicode notation | Calc      |   |           |
  | ---------------- | --------- | - | --------- |
  | `                | x         | ` | `abs(x)`* |
  | `‖x‖`            | `abs(x)`* |   |           |
  | `abs(x)`         | `abs(x)`  |   |           |


  | Unicode / notation | Calc            |
  | ------------------ | --------------- |
  | `n!`               | `factorial(n)`* |
  | `n‼`               | `factorial(n)`* |
  | `P(n,r)`           | `P(n,r)`        |
  | `C(n,r)`           | `C(n,r)`        |
  | `nPr`              | `P(n,r)`*       |
  | `nCr`              | `C(n,r)`*       |
  | `nP r`             | `P(n,r)`*       |
  | `nC r`             | `C(n,r)`*       |


  | Unicode    | Calc       |
  | ---------- | ---------- |
  | `min`      | `min`      |
  | `minimum`  | `min`      |
  | `min(x,y)` | `min(x,y)` |
  | `max`      | `max`      |
  | `maximum`  | `max`      |
  | `max(x,y)` | `max(x,y)` |


  | Unicode / notation | Calc        |
  | ------------------ | ----------- |
  | `⌊x⌋`              | `floor(x)`* |
  | `⌈x⌉`              | `ceil(x)`*  |
  | `⌊x⌋`              | `floor(x)`* |
  | `⌈x⌉`              | `ceil(x)`*  |
  | `round(x)`         | `round(x)`  |
  | `trunc(x)`         | `trunc(x)`  |



  | Unicode    | Calc             |
  | ---------- | ---------------- |
  | `Γ(x)`     | `gamma(x)`       |
  | `Γ`        | `gamma`*         |
  | `ln Γ(x)`  | `log(gamma(x))`* |
  | `log Γ(x)` | `lgamma(x)`*     |

  γ → e_gamma
  Γ(x) → gamma(x)


  | Unicode / notation | Calc      |
  | ------------------ | --------- |
  | `erf`              | `erf`     |
  | `erfc`             | `erfc`    |
  | `erf(x)`           | `erf(x)`  |
  | `erfc(x)`          | `erfc(x)` |


  | Notation     | Calc                |
  | ------------ | ------------------- |
  | `√(x²+y²)`   | existing expression |
  | `‖(x,y)‖`    | `hypot(x,y)`*       |
  | `hypot(x,y)` | `hypot(x,y)`        |
  | `d(p,q)`     | `distance(...)`*    |


  90° → 90 * pi / 180
  | Unicode   | Calc               |
  | --------- | ------------------ |
  | `°`       | `deg`*             |
  | `º`       | `deg`*             |
  | `rad`     | `rad`              |
  | `radian`  | `rad`              |
  | `radians` | `rad`              |
  | `deg`     | `deg`              |
  | `degree`  | `deg`              |
  | `degrees` | `deg`              |
  | `grad`    | `grad_to_radians`* |
  | `gon`     | `grad_to_radians`* |
  | `turn`    | `turn_to_radians`* |
  | `τ`       | `2*pi`*            |


  | Unicode | Calc        |
  | ------- | ----------- |
  | `c`     | `c`         |
  | `ℏ`     | `hbar`      |
  | `ħ`     | `hbar`      |
  | `h`     | `h`         |
  | `e`     | `e_charge`* |
  | `e₀`    | `epsilon0`* |
  | `ε₀`    | `epsilon0`  |
  | `μ₀`    | `mu0`       |
  | `Z₀`    | `Z0`        |
  | `k_B`   | `kB`        |
  | `N_A`   | `NA`        |
  | `F`     | `F`         |
  | `R`     | `R`         |
  | `α`     | `alpha`     |
  | `G`     | `G`         |
  | `g₀`    | `g0`        |

  ℯ → e
  e → e
  qₑ → e_charge


  | Unicode | Calc        |
  | ------- | ----------- |
  | `a₀`    | `a0`        |
  | `a.u.`  | `au`        |
  | `AU`    | `au`        |
  | `ly`    | `ly`        |
  | `pc`    | `pc`        |
  | `M☉`    | `m_sun`     |
  | `M⊕`    | `m_earth`   |
  | `M♃`    | `m_jupiter` |
  | `R☉`    | `r_sun`     |
  | `R⊕`    | `r_earth`   |


  | Unicode | Calc     |
  | ------- | -------- |
  | `mₑ`    | `m_e`    |
  | `mₚ`    | `mp`     |
  | `mₙ`    | `mn`     |
  | `mᵤ`    | `mu`     |
  | `m_d`   | `md`     |
  | `m_α`   | `malpha` |
  | `μ_B`   | `muB`    |
  | `μ_N`   | `muN`    |
  | `μₑ`    | `mue`    |
  | `μₚ`    | `mup`    |
  | `μₙ`    | `mun`    |
  | `μ_d`   | `mud`    |
  | `rₑ`    | `re`     |


  | Unicode | Calc  |
  | ------- | ----- |
  | `ℓ_P`   | `l_P` |
  | `l_P`   | `l_P` |
  | `m_P`   | `m_P` |
  | `t_P`   | `t_P` |
  | `q_P`   | `q_P` |
  | `T_P`   | `T_P` |
  | `E_P`   | `E_P` |


  | Unicode / notation | Calc  |
  | ------------------ | ----- |
  | `a₀`               | `a0`  |
  | `mₑ`               | `m_e` |
  | `E_h`              | `Eh`  |
  | `𝜇_B`             | `muB` |
  | `𝜇_N`             | `muN` |



  | Unicode notation | Calc           |   |          |
  | ---------------- | -------------- | - | -------- |
  | `√x`             | `sqrt(x)`      |   |          |
  | `∛x`             | `cbrt(x)`      |   |          |
  | `                | x              | ` | `abs(x)` |
  | `⌊x⌋`            | `floor(x)`     |   |          |
  | `⌈x⌉`            | `ceil(x)`      |   |          |
  | `x!`             | `factorial(x)` |   |          |
  | `Γ(x)`           | `gamma(x)`     |   |          |
  | `sin⁻¹(x)`       | `arcsin(x)`    |   |          |
  | `cos⁻¹(x)`       | `arccos(x)`    |   |          |
  | `tan⁻¹(x)`       | `arctan(x)`    |   |          |
  | `log₁₀(x)`       | `log10(x)`     |   |          |
  | `log₂(x)`        | `log2(x)`      |   |          |
  | `eˣ`             | `exp(x)`       |   |          |
  | `2ˣ`             | `exp2(x)`*     |   |          |
  | `‖(x,y)‖`        | `hypot(x,y)`   |   |          |



  | Unicode | ASCII |
  | ------- | ----- |
  | `⁰`     | `0`   |
  | `¹`     | `1`   |
  | `²`     | `2`   |
  | `³`     | `3`   |
  | `⁴`     | `4`   |
  | `⁵`     | `5`   |
  | `⁶`     | `6`   |
  | `⁷`     | `7`   |
  | `⁸`     | `8`   |
  | `⁹`     | `9`   |
  | `⁺`     | `+`   |
  | `⁻`     | `-`   |
  | `⁽`     | `(`   |
  | `⁾`     | `)`   |



  | Unicode | ASCII |
  | ------- | ----- |
  | `₀`     | `0`   |
  | `₁`     | `1`   |
  | `₂`     | `2`   |
  | `₃`     | `3`   |
  | `₄`     | `4`   |
  | `₅`     | `5`   |
  | `₆`     | `6`   |
  | `₇`     | `7`   |
  | `₈`     | `8`   |
  | `₉`     | `9`   |
  | `₊`     | `+`   |
  | `₋`     | `-`   |
  | `₍`     | `(`   |
  | `₎`     | `)`   |


  | Unicode | Calc      |
  | ------- | --------- |
  | `π`     | `pi`      |
  | `φ`     | `phi`     |
  | `ϕ`     | `phi`     |
  | `γ`     | `e_gamma` |
  | `α`     | `alpha`   |



  | Unicode | Calc            |
  | ------- | --------------- |
  | `α`     | `alpha`         |
  | `μ`     | `mu`            |
  | `μ₀`    | `mu0`           |
  | `μ_B`   | `muB`           |
  | `μ_N`   | `muN`           |
  | `λₑ`    | `lambda_e`      |
  | `λ̄ₑ`   | `lambda_bar_e`  |
  | `σ`     | `sigma`         |
  | `σₑ`    | `sigmae`        |
  | `ε₀`    | `epsilon0`      |
  | `φ₀`    | `phi0`          |
  | `Γ`     | `gamma`*        |
  | `ζ`     | `riemann_zeta`* |


  | Unicode / notation | Calc           |
  | ------------------ | -------------- |
  | `√`                | `sqrt`         |
  | `∛`                | `cbrt`         |
  | `Γ`                | `gamma`        |
  | `ζ`                | `riemann_zeta` |
  | `η`                | `?`            |
  | `β`                | `beta`         |
  | `Hₙ`               | `hermite`*     |
  | `Lₙ`               | `laguerre`*    |
  | `Pₙ`               | `legendre`*    |



  | Unicode | Tempting mapping | Status            |
  | ------- | ---------------- | ----------------- |
  | `∑`     | `sum`            | **not supported** |
  | `Σ`     | `sum`            | **not supported** |
  | `∏`     | `prod`           | **not supported** |
  | `Π`     | `prod`           | **not supported** |
  | `∫`     | `integral`       | **not supported** |
  | `∬`     | `integral`       | **not supported** |
  | `∭`     | `integral`       | **not supported** |
  | `∮`     | `integral`       | **not supported** |
  | `∂`     | `partial`        | **not supported** |
  | `∇`     | `nabla`          | **not supported** |
  | `∆`     | `delta`          | **not supported** |
  | `∀`     | `forall`         | **not supported** |
  | `∃`     | `exists`         | **not supported** |
  | `∈`     | `in`             | **not supported** |
  | `∉`     | `notin`          | **not supported** |
  | `⊂`     | `subset`         | **not supported** |
  | `⊆`     | `subseteq`       | **not supported** |
  | `∩`     | `intersection`   | **not supported** |
  | `∪`     | `union`          | **not supported** |


  | Unicode | Potential ASCII |
  | ------- | --------------- |
  | `=`     | `=`             |
  | `≠`     | `!=`            |
  | `≤`     | `<=`            |
  | `≥`     | `>=`            |
  | `<`     | `<`             |
  | `>`     | `>`             |
  | `≡`     | `==`            |
  | `≈`     | `~=`            |
  | `≃`     | `~=`            |
  | `≅`     | `~=`            |



  First step:


  ∞  → inf

  π  → pi
  ϖ  → pi
  φ  → phi
  ϕ  → phi
  γ  → e_gamma
  α  → alpha

  √  → sqrt
  ∛  → cbrt

  ×  → *
  ⋅  → *
  ·  → *
  ∙  → *
  ∗  → *
  ⨯  → *

  ÷  → /
  ∕  → /
  ⁄  → /

  −  → -
  ‐  → -
  -  → -
  ‒  → -
  –  → -
  —  → -

  ＋  → +
  －  → -

  ％  → %

  （  → (
  ）  → )
  ，  → ,

  ℏ  → hbar
  ħ  → hbar
  ε₀ → epsilon0
  μ₀ → mu0
  Z₀ → Z0
  N_A → NA
  k_B → kB

  mₑ → m_e
  mₚ → mp
  mₙ → mn
  μ_B → muB
  μ_N → muN
  rₑ → re

  ℓ_P → l_P
  a₀ → a0

  M☉ → m_sun
  M⊕ → m_earth
  M♃ → m_jupiter
  R☉ → r_sun
  R⊕ → r_earth


  */

    // Any other non-ASCII character is not part of the current Calc
    // language and should be rejected here.
    default:
      [[unlikely]] return false;
    }
  }

  [[likely]] return true;
}

/**
 * GUI helper to process user input. Don't need to copy Unicode user input from
 * Edit control. This can'be usable only if Edit is multiline and for Dialog
 * local edit option is enabled.
 */
class EditTextView {
public:
  EditTextView() = delete;
  EditTextView(EditTextView const &) = delete;
  EditTextView &operator=(EditTextView const &) = delete;

  explicit EditTextView(const HWND edit) noexcept {
    _size = static_cast<UINT>(SendMessageA(edit, WM_GETTEXTLENGTH, 0, 0));
    if (!empty()) [[likely]] {
      _handle =
          reinterpret_cast<HLOCAL>(SendMessageA(edit, EM_GETHANDLE, 0, 0));
      _data = static_cast<LPCWSTR>(LocalLock(_handle));
    }
  }

  ~EditTextView() noexcept {
    if (!empty()) [[likely]] {
      LocalUnlock(_handle);
    }
  }

  [[nodiscard]]
  constexpr bool empty() const noexcept {
    return _size == 0;
  }

  /**
   * length in characters of the text in the edit control. This is useful for
   * text processing and validation.
   */
  [[nodiscard]]
  constexpr auto length() const noexcept {
    return _size;
  }

  [[nodiscard]] constexpr auto text() const noexcept { return _data; }

  /**
   * size in bytes of the text in the edit control. This is useful for writing
   * to the system database.
   */
  [[nodiscard]]
  constexpr auto size() const noexcept {
    return static_cast<DWORD>(_size * sizeof(WCHAR));
  }

  [[nodiscard]] constexpr auto bytes() const noexcept {
    return reinterpret_cast<const BYTE *>(_data);
  }

private:
  [[no_unique_address]] HLOCAL _handle [[indeterminate]];
  [[no_unique_address]] LPCWSTR _data [[indeterminate]];
  [[no_unique_address]] UINT _size;
};

/**
 * GUI helper to write data to the GUI and restore user data from system
 * database.
 */
class EditTextWriter {
public:
  EditTextWriter() = delete;
  EditTextWriter(EditTextWriter const &) = delete;
  EditTextWriter &operator=(EditTextWriter const &) = delete;

  explicit EditTextWriter(const HWND edit, const UINT max_size) noexcept
      : _edit(edit), _max_size(max_size) {

    const auto orig_memory =
        reinterpret_cast<HLOCAL>(SendMessageA(edit, EM_GETHANDLE, 0, 0));

    _handle = LocalReAlloc(orig_memory, _max_size, LMEM_MOVEABLE);

    if (valid()) [[likely]] {
      _data = static_cast<LPWSTR>(LocalLock(_handle));
    }
  }

  ~EditTextWriter() noexcept {
    if (valid()) [[likely]] {
      LocalUnlock(_handle);
      SendMessageA(_edit, EM_SETHANDLE, reinterpret_cast<WPARAM>(_handle), 0);
    }
  }

  /**
   * length in characters of the text in the edit control. This is useful for
   * setting text.
   */
  constexpr void set_length(const UINT length) noexcept {
    _data[length] = L'\0'; // because of C string
  }

  [[nodiscard]] constexpr auto text() const noexcept { return _data; }

  /**
   * size in bytes of the text in the edit control. This is useful for loading
   * data from the system database.
   */
  constexpr void set_size(const UINT size) noexcept {
    set_length(size / sizeof(WCHAR));
  }

  [[nodiscard]] constexpr auto bytes() noexcept {
    return reinterpret_cast<BYTE *>(_data);
  }

  [[nodiscard]]
  constexpr bool valid() const noexcept {
    return _handle != nullptr;
  }

  [[nodiscard]]
  constexpr bool writable() const noexcept {
    return _data != nullptr;
  }

private:
  [[no_unique_address]] const HWND _edit;
  [[no_unique_address]] HLOCAL _handle [[indeterminate]];
  [[no_unique_address]] LPWSTR _data [[indeterminate]];
  [[no_unique_address]] const UINT _max_size;
};

/**
 * GUI helper structure to handle Calc user input.
 */
class EquasionHandler {
public:
  EquasionHandler() = delete;
  EquasionHandler(EquasionHandler const &) = delete;
  EquasionHandler &operator=(EquasionHandler const &) = delete;

  explicit constexpr EquasionHandler(const EditTextView &edit,
                                     std::string &buffer)
      : _data(buffer) {
    if (!normalize_equasion(edit.text(), edit.length(), buffer)) [[unlikely]] {
      _data.clear();
    }
  }

  constexpr ~EquasionHandler() noexcept { _data.clear(); }

  [[nodiscard]] constexpr const auto &data() const noexcept { return _data; }

private:
  [[no_unique_address]] std::string &_data;
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
                         &outSize) == ERROR_SUCCESS) {
      if (type == REG_DWORD) {
        return out;
      }
    }
    return std::nullopt;
  }

  [[nodiscard]] UINT read(const char *name, LPBYTE out,
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

#ifdef CALC_SUPPORT_EXTENDENT_STYLES
/**
 * Utility: set Edit control extended styles.
 */
static void edit_set_extended_style(const HWND edit, const DWORD mask,
                                    const DWORD style) noexcept {
  SendMessageA(edit, EM_SETEXTENDEDSTYLE, static_cast<WPARAM>(mask),
               static_cast<LPARAM>(style));
}
#endif

/**
 * Utility: set text to window from begin to end (end is not included).
 */
static void set_window_text(const HWND hWnd, LPCSTR text,
                            LPSTR const text_end) noexcept {
  *text_end = '\0'; // because of C string
  SetWindowTextA(hWnd, text);
}

#if defined(_MSC_VER)
__pragma(warning(push))
    __pragma(warning(disable
                     : 4505)) // allow unused static function in release build
#endif

    /**
     * Utility: set text to window from begin to end (end is not included).
     */
    static void set_window_text(const HWND hWnd, LPCWSTR text,
                                LPWSTR const text_end) noexcept {
  *text_end = L'\0'; // because of C string
  SetWindowTextW(hWnd, text);
}

/**
 * Utility: get text from window and return its size.
 */
[[nodiscard]] static UINT get_window_text(const HWND hWnd, CHAR *text,
                                          const UINT max_size) noexcept {
  return GetWindowTextA(hWnd, text, max_size);
}

#if defined(_MSC_VER)
__pragma(warning(pop))
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
[[nodiscard]] static UINT get_window_dpi(const HWND window) noexcept {
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
[[nodiscard]] static LONG to_physical(LONG value, LONG dpi) noexcept {
  return std::lroundf(static_cast<float>(value) * static_cast<float>(dpi) /
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI));
}

/**
 * Utility: coordinate convertor from physical to logical for window
 */
[[nodiscard]] static LONG to_logical(LONG value, LONG dpi) noexcept {
  return std::lroundf(static_cast<float>(value) *
                      static_cast<float>(USER_DEFAULT_SCREEN_DPI) /
                      static_cast<float>(dpi));
};
#endif

#ifdef CALC_SUPPORT_DARK_MODE
/**
 * Utility: helper to work with theming
 */
struct Theme {
  /**
   * init application theming. Should be called before any window/dialog/menu is
   * created.
   */
  void init(const HWND application_main_window) noexcept {

    init_uxtheme_callers();

    SetPreferredAppMode(PreferredAppMode::AllowDark);

    apply(application_main_window, false, true);
  }

  /**
   * apply dark/light appearance to application title bar and aplication frame.
   * Also this update theme settings for interactive controls that repaints
   * itself based on it's current state and application theme: BUTTON,
   * SCROLLBAR, COMBOBOX, LISTBOX, LISTVIEW, TREEVIEW, TAB, PROGRESSBAR,
   * TRACKBAR.
   */
  void apply(const HWND window, const bool redraw = false,
             const bool is_main_window = false) noexcept {

    apply_theme(window, is_dark_mode(is_main_window));

    EnumChildWindows(window, apply_theme, is_dark_mode(false));

    apply_menus();

    apply_title_bar_and_frame(window);

    if (redraw) {
      RedrawWindow(window, nullptr, nullptr,
                   RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    }
  }

#ifdef CALC_SUPPORT_DARK_MODE_WITHOUT_WIN32_HELPER
  /**
   * apply dark/light appearance to controls that needs external repainting,
   * they don't using theming and get global colors from Win32 settings, but we
   * can simply swap colors to use dark theme for them: EDIT, STATIC, DIALOG
   */
  [[nodiscard]] INT_PTR apply(const WPARAM dc) noexcept {
    const HDC hdc = reinterpret_cast<HDC>(dc);
    if (is_dark_mode(false)) {
      SetBkColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
      SetTextColor(hdc, GetSysColor(COLOR_APPWORKSPACE));
      return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOWTEXT));
    }
    return FALSE;
  }
#endif

private:
  /**
   * apply dark/light theme to window based on a global theme.
   */
  static BOOL CALLBACK apply_theme(const HWND window,
                                   const LPARAM dark) noexcept {
    SetWindowTheme(window, dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
    return TRUE;
  }

  /**
   * determine whether applications uses the dark app theme.
   */
  [[nodiscard]] bool is_dark_mode(const bool is_main_window) noexcept {
    if (is_main_window) {
      const RegRead personalization(
          HKEY_CURRENT_USER,
          "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");

      const auto app_uses_light_theme =
          personalization.read("AppsUseLightTheme");
      dark_mode_enabled =
          app_uses_light_theme && app_uses_light_theme.value() == 0;
    }
    return dark_mode_enabled;
  }

  /**
   * apply dark/light theme to a application bar and frame. Win32 API for
   * Windows 6+ with working DWM.
   */
  void apply_title_bar_and_frame(const HWND hwnd) const noexcept {
    BOOL value = dark_mode_enabled ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value,
                          sizeof(value));
  }

  // DLL hell begin()

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
   * Menu dynamic colors for Win32 API for Windows 6+ with working DWM.
   *
   * Uses the undocumented uxtheme exports:
   *   #135 SetPreferredAppMode
   *   #104 RefreshImmersiveColorPolicyState
   *   #136 FlushMenuThemes
   *
   * The menu itself remains owned by Windows. No owner-draw, no WM_DRAWITEM,
   * and no custom menu colors are required.
   */
  using SetPreferredAppModeFn = PreferredAppMode(WINAPI *)(PreferredAppMode);

  SetPreferredAppModeFn SetPreferredAppMode [[indeterminate]];

  using RefreshImmersiveColorPolicyStateFn = void(WINAPI *)();

  RefreshImmersiveColorPolicyStateFn RefreshImmersiveColorPolicyState
      [[indeterminate]];

  using FlushMenuThemesFn = void(WINAPI *)();

  FlushMenuThemesFn FlushMenuThemes [[indeterminate]];

  /**
   * technical helper for dll hell ^^
   */
  void init_uxtheme_callers() noexcept {
    HMODULE uxtheme = GetModuleHandleA("uxtheme.dll");

#if defined(_MSC_VER)
    __pragma(warning(push)) __pragma(
        warning(disable : 4191)) // allow FARPROC -> function pointer casts here
#endif

        SetPreferredAppMode = reinterpret_cast<SetPreferredAppModeFn>(
            GetProcAddress(uxtheme, MAKEINTRESOURCEA(135)));

    RefreshImmersiveColorPolicyState =
        reinterpret_cast<RefreshImmersiveColorPolicyStateFn>(
            GetProcAddress(uxtheme, MAKEINTRESOURCEA(104)));

    FlushMenuThemes = reinterpret_cast<FlushMenuThemesFn>(
        GetProcAddress(uxtheme, MAKEINTRESOURCEA(136)));

#if defined(_MSC_VER)
    __pragma(warning(pop))
#endif
  }

  // DLL hell end()

  /**
   * apply theme to menus. Win32 API for Windows 6+ with working DWM.
   */
  void apply_menus() const noexcept {
    RefreshImmersiveColorPolicyState();
    FlushMenuThemes();
  }

  [[no_unique_address]] bool dark_mode_enabled [[indeterminate]];
};
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
constexpr bool hasFlag(Anchor value, Anchor flag) noexcept {
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

    [[no_unique_address]] HWND handle [[indeterminate]];
    [[no_unique_address]] LONG width [[indeterminate]];
    [[no_unique_address]] LONG height [[indeterminate]];
    [[no_unique_address]] Rect relative_margins [[indeterminate]];
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

  void init_anchor(const HWND parent, const BYTE index, const int id,
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

      if (hasFlag(c.anchor, Anchor::Left)) {
        rect.right = c.relative_margins.left + c.width;
        rect.left = c.relative_margins.left;
      } else if (hasFlag(c.anchor, Anchor::Right)) {
        rect.right = width - c.relative_margins.right;
        rect.left = rect.right - c.width;
      } else if (hasFlag(c.anchor, Anchor::HorizontalStretch)) {
        rect.right = width - c.relative_margins.right;
        rect.left = c.relative_margins.left;
      }

      if (hasFlag(c.anchor, Anchor::Top)) {
        rect.bottom = c.relative_margins.top + c.height;
        rect.top = c.relative_margins.top;
      } else if (hasFlag(c.anchor, Anchor::Bottom)) {
        rect.bottom = height - c.relative_margins.bottom;
        rect.top = rect.bottom - c.height;
      } else if (hasFlag(c.anchor, Anchor::VerticalStretch)) {
        rect.bottom = height - c.relative_margins.bottom;
        rect.top = c.relative_margins.top;
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
  [[no_unique_address]] LONG width [[indeterminate]];
  [[no_unique_address]] LONG height [[indeterminate]];

  [[no_unique_address]] LONG min_width [[indeterminate]];
  [[no_unique_address]] LONG min_height [[indeterminate]];
  [[no_unique_address]] std::array<Constraint, elements> constraints
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
constexpr static const auto colors_indexes =
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
  [[no_unique_address]] std::array<COLORREF, colors_indexes.size()> values
      [[indeterminate]];

public:
  /**
   * Get a system color by its COLOR_* index.
   */
  [[nodiscard]]
  const COLORREF &operator[](const INT index) const noexcept {
    return values[index];
  }

  /**
   * Get a mutable system color by its COLOR_* index.
   */
  [[nodiscard]]
  COLORREF &operator[](const INT index) noexcept {
    return values[index];
  }

  [[nodiscard]] LPCOLORREF data() noexcept { return values.data(); }
};

/**
 * Get all system colors.
 *
 * Win32 provides GetSysColor() only as a scalar API, so the complete
 * system-color table is collected with one GetSysColor() call per entry.
 */
[[nodiscard]]
Colors colors() noexcept {
  Colors result [[indeterminate]];

  for (const auto i : colors_indexes) {
    result[i] = GetSysColor(i);
  }

  return result;
}

/**
 * Set all classic Win32 system colors.
 *
 * SetSysColors() accepts an array of COLOR_* indices and an array of COLORREF
 * values. Windows broadcasts WM_SYSCOLORCHANGE after a successful change and
 * repaints affected visible windows.
 */
[[nodiscard]]
inline bool set_colors(Colors &&new_values) noexcept {
  return SetSysColors(static_cast<INT>(colors_indexes.size()),
                      colors_indexes.data(), new_values.data()) != FALSE;
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
inline DWMColors dwm_colors(const HWND window) noexcept {
  DWMColors result;

  /**
   * Global DWM colorization/accent color.
   */
  if (SUCCEEDED(DwmGetColorizationColor(&result.colorization_argb,
                                        &result.colorization_opaque))) {
    result.has_colorization = true;
  }

  if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_BORDER_COLOR,
                                      &result.border, sizeof(result.border)))) {
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CAPTION_COLOR,
                                        &result.caption,
                                        sizeof(result.caption)))) {
      if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_TEXT_COLOR,
                                          &result.text, sizeof(result.text)))) {
        result.has_window_colors = true;
      }
    }
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
#endif
