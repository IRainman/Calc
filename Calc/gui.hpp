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

#ifdef CALC_TEST_UNICODE
// TODO: don't copy data from Win32 Edit control to the buffer. This can'be
// usable only with UNICODE and only if multiline and local edit enabled in the
// edit control. This is needs to be enable after preprocessing and converting
// mathematical values, like 𝜋 -> pi i.e to it's representatin.
#define CALC_USED_EDIT_VIEW

[[nodiscard]] static bool normalize(std::u16string_view input,
                                    std::string &output) noexcept {
  output.clear();
  output.reserve(input.size());

  for (const char16_t c : input) {
    if (c <= Token::ERROR) {
      output.push_back(static_cast<char>(c));
      continue;
    }

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
      output.push_back(' ');
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

    // Any other non-ASCII character is not part of the current Calc
    // language and should be rejected here.
    default:
      return false;
    }
  }

  return true;
}

[[nodiscard]] static const auto &normalize(std::u16string_view input) {
  static std::string output;
  if (!normalize(input, output)) {
    output.clear();
  }
  return output;
}

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
#ifndef CALC_SUPPORT_THEMING
#define NOCOLOR -Screen colors
#endif
// #define NOCTLMGR -Control and Dialog routines
#define NODRAWTEXT -DrawText() and DT_ *
#ifndef CALC_SUPPORT_THEMING
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
#ifndef CALC_SUPPORT_THEMING
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
#ifdef CALC_SUPPORT_THEMING
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
  std::u16string_view get_view() const noexcept {
    return {_data, _size};
  }

private:
  HLOCAL _handle [[indeterminate]];
  LPCWSTR _data [[indeterminate]];
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
  [[no_unique_address]] UINT _size [[indeterminate]];
  [[no_unique_address]] char _data[CalcConfiguration::input_max_data_size];
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

#ifndef CALC_USED_EDIT_VIEW
/**
 * Utility: get text from window and return its size.
 */
[[nodiscard]] static UINT get_window_text(const HWND hWnd, CHAR *text,
                                          const UINT max_size) noexcept {
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
#ifdef CALC_SUPPORT_THEMING
struct Theme {
  /**
   * initialization
   */
  void init(const HWND window) noexcept {

    apply_dark_mode(window, false, true);

#if (_WIN32_WINNT >= 0x0600)
    HMODULE uxtheme = GetModuleHandleA("uxtheme.dll");

    pSetPreferredAppMode = reinterpret_cast<SetPreferredAppModeFn>(
        GetProcAddress(uxtheme, MAKEINTRESOURCEA(135)));

    if (pSetPreferredAppMode) {
      pSetPreferredAppMode(PreferredAppMode::AllowDark);
    }

    pRefreshImmersiveColorPolicyState =
        reinterpret_cast<RefreshImmersiveColorPolicyStateFn>(
            GetProcAddress(uxtheme, MAKEINTRESOURCEA(104)));

    pFlushMenuThemes = reinterpret_cast<FlushMenuThemesFn>(
        GetProcAddress(uxtheme, MAKEINTRESOURCEA(136)));

    apply_dark_mode();

    apply_title_bar_and_frame(window);
#endif
  }

  /**
   * Utility: apply dark/light appearance title bar, frame and non interactive
   * controls: BUTTON, SCROLLBAR, COMBOBOX, LISTBOX, LISTVIEW, TREEVIEW, TAB,
   * PROGRESSBAR, TRACKBAR.
   */
  void apply_dark_mode(const HWND window, const bool redraw = false,
                       const bool is_main_window = false) noexcept {

    apply_theme(window, is_dark_mode(is_main_window));

    EnumChildWindows(window, apply_theme, is_dark_mode(false));

    if (redraw) {
      RedrawWindow(window, nullptr, nullptr,
                   RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    }

#if (_WIN32_WINNT >= 0x0600)
    apply_dark_mode();

    apply_title_bar_and_frame(window);
#endif
  }

  /**
   * Utility: apply dark/light appearance to interactive controls:
   * EDIT, STATIC, DIALOG
   */
  [[nodiscard]] INT_PTR apply_dark_mode(const WPARAM dc) noexcept {
    const HDC hdc = reinterpret_cast<HDC>(dc);
    if (is_dark_mode(false)) {
      SetBkColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
      SetTextColor(hdc, GetSysColor(COLOR_WINDOW));
      return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_WINDOWTEXT));
    }
    return FALSE;
  }

private:
  /**
   * Utility: apply dark/light theme to chaild windows.
   */
  static BOOL CALLBACK apply_theme(const HWND child,
                                   const LPARAM dark) noexcept {
    SetWindowTheme(child, dark ? L"DarkMode_Explorer" : L"Explorer", nullptr);
    return TRUE;
  }

  /**
   * Utility: determine whether applications uses the dark app theme.
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

#if (_WIN32_WINNT >= 0x0600)
  /**
   * Utility: apply dark/light theme to a application bar and frame. Win32 API
   * for Windows 6+ with working DWM.
   */
  void apply_title_bar_and_frame(const HWND hwnd) const noexcept {
    BOOL value = dark_mode_enabled ? TRUE : FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value,
                          sizeof(value));
  }

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

  /** Menu dark-mode support for Win32 API for Windows 6+ with working DWM.
   *
   * Uses the undocumented uxtheme exports:
   *   #132 ShouldAppsUseDarkMode
   *   #135 SetPreferredAppMode
   *   #136 FlushMenuThemes
   *
   * The menu itself remains owned by Windows. No owner-draw, no WM_DRAWITEM,
   * and no custom menu colors are required.
   */
  using SetPreferredAppModeFn = PreferredAppMode(WINAPI *)(PreferredAppMode);

  [[no_unique_address]] SetPreferredAppModeFn pSetPreferredAppMode
      [[indeterminate]];

  using RefreshImmersiveColorPolicyStateFn = void(WINAPI *)();

  [[no_unique_address]] RefreshImmersiveColorPolicyStateFn
      pRefreshImmersiveColorPolicyState [[indeterminate]];

  using FlushMenuThemesFn = void(WINAPI *)();

  [[no_unique_address]] FlushMenuThemesFn pFlushMenuThemes [[indeterminate]];

  /**
   * for menus only is DWM enabled
   */
  void apply_dark_mode() const noexcept {
    if (pRefreshImmersiveColorPolicyState) {
      pRefreshImmersiveColorPolicyState();
    }
    if (pFlushMenuThemes) {
      pFlushMenuThemes();
    }
  }
#endif
  [[no_unique_address]] bool dark_mode_enabled [[indeterminate]];
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
  constexpr Layout() = default;
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
    [[no_unique_address]] Rect relative_margins [[indeterminate]];
    [[no_unique_address]] LONG width [[indeterminate]];
    [[no_unique_address]] LONG height [[indeterminate]];
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

  [[no_unique_address]] LONG width [[indeterminate]];
  [[no_unique_address]] LONG height [[indeterminate]];

  [[no_unique_address]] LONG min_width [[indeterminate]];
  [[no_unique_address]] LONG min_height [[indeterminate]];
};

#else

// TODO Qt

#endif
#endif
