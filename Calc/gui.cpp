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

namespace GUI {

#ifdef _WIN32

/**
 * Calc GUI window also it's application itself.
 */
class CalcGUI {
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

  /**
   * GUI helper to handle Calc user input.
   */
  class EquasionHandler {
  public:
    EquasionHandler() = delete;
    EquasionHandler(EquasionHandler const &) = delete;
    EquasionHandler &operator=(EquasionHandler const &) = delete;

    explicit constexpr EquasionHandler(EditView &edit, std::string &equasion)
        : _equasion(equasion), _edit(edit) {
      _position = normalize_equasion(edit.text(), edit.length(), _equasion);
    }

    constexpr ~EquasionHandler() noexcept { _equasion.clear(); }

    [[nodiscard]] constexpr bool failed() const noexcept {
      // TODO Need to check _position in the edit control. And higlight
      // _position in the edit control if it's not the end.

      // TODO this check isn't complet, because ANSI string can be longer
      // because of Unicode characters.
      return _position != _equasion.length();
    }

    [[nodiscard]] constexpr const auto &data() const noexcept {
      return _equasion;
    }

  private:
    [[no_unique_address]] std::string &_equasion;
    [[no_unique_address]] EditView &_edit;
    [[no_unique_address]] UINT _position;

    /**
     * The normalization needed for preprocessing and converting mathematical
     * values, like ℯ -> e, 𝜋 -> pi i.e to it's ANSI representation.
     *
     * TODO: this function not transform any syntax construction!
     * CALC_ALLOW_UNICODE_IN_GUI
     */
    [[nodiscard]] UINT normalize_equasion(const LPCWSTR input, const UINT size,
                                          std::string &output) noexcept {
      UINT position = 0;
      for (; position != size; ++position) [[likely]] {
        switch (input[position])
          [[likely]] {
          // All string separation, formatting, and control characters:
          case WCHAR('\t'): // TAB
          case WCHAR('\n'): // LF
          case WCHAR('\v'): // VT
          case WCHAR('\f'): // FF
          case WCHAR('\r'): // CR
          case WCHAR(' '):  // SPACE
          case u'\u00A0':   // NO-BREAK SPACE
          case u'\u2000':   // EN QUAD
          case u'\u2001':   // EM QUAD
          case u'\u2002':   // EN SPACE
          case u'\u2003':   // EM SPACE
          case u'\u2004':   // THREE-PER-EM SPACE
          case u'\u2005':   // FOUR-PER-EM SPACE
          case u'\u2006':   // SIX-PER-EM SPACE
          case u'\u2007':   // FIGURE SPACE
          case u'\u2008':   // PUNCTUATION SPACE
          case u'\u2009':   // THIN SPACE
          case u'\u200A':   // HAIR SPACE
          case u'\u200B':   // ZERO WIDTH SPACE
          case u'\u200C':   // ZERO WIDTH NON-JOINER
          case u'\u200D':   // ZERO WIDTH JOINER
          case u'\u2060':   // WORD JOINER
          case u'\u202F':   // NARROW NO-BREAK SPACE
          case u'\u205F':   // MEDIUM MATHEMATICAL SPACE
          case u'\u3000':   // IDEOGRAPHIC SPACE
          case u'\u2028':   // LINE SEPARATOR
          case u'\u2029':   // PARAGRAPH SEPARATOR
          case u'\uFEFF':   // ZERO WIDTH NO-BREAK SPACE
            output.push_back(' ');
            break;

          // Fullwidth, superscript, subscript digits.
          case u'\uFF10': // ０
          case u'\u2070': // ⁰
          case u'\u2080': // ₀
            output.push_back('0');
            break;
          case u'\uFF11': // １
          case u'\u00B9': // ¹
          case u'\u2081': // ₁
            output.push_back('1');
            break;
          case u'\uFF12': // ２
          case u'\u00B2': // ²
          case u'\u2082': // ₂
            output.push_back('2');
            break;
          case u'\uFF13': // ３
          case u'\u00B3': // ³
          case u'\u2083': // ₃
            output.push_back('3');
            break;
          case u'\uFF14': // ４
          case u'\u2074': // ⁴
          case u'\u2084': // ₄
            output.push_back('4');
            break;
          case u'\uFF15': // ５
          case u'\u2075': // ⁵
          case u'\u2085': // ₅
            output.push_back('5');
            break;
          case u'\uFF16': // ６
          case u'\u2076': // ⁶
          case u'\u2086': // ₆
            output.push_back('6');
            break;
          case u'\uFF17': // ７
          case u'\u2077': // ⁷
          case u'\u2087': // ₇
            output.push_back('7');
            break;
          case u'\uFF18': // ８
          case u'\u2078': // ⁸
          case u'\u2088': // ₈
            output.push_back('8');
            break;
          case u'\uFF19': // 9
          case u'\u2079': // ⁹
          case u'\u2089': // ₉
            output.push_back('9');
            break;

          // Fullwidth, superscript, subscript and alternative
          // operators/punctuation.
          case u'\uFF0B': // ＋
          case u'\u207A': // ⁺
          case u'\u208A': // ₊
            output.push_back('+');
            break;

          case u'\uFF0D': // －
          case u'\u207B': // ⁻
          case u'\u208B': // ₋
          case u'\u2010': // ‐
          case u'\u2011': // -
          case u'\u2012': // ‒
          case u'\u2013': // –
          case u'\u2014': // —
          case u'\u2212': // −
          case u'\uFE63': // ﹣
            output.push_back('-');
            break;

          case u'\uFF0A': // ＊
          case u'\u00B7': // ·
          case u'\u00D7': // ×
          case u'\u2217': // ∗
          case u'\u2219': // ∙
          case u'\u22C5': // ⋅
          case u'\u204E': // ⁎
          case u'\u2A2F': // ⨯
            output.push_back('*');
            break;

          case u'\u00F7': // ÷
          case u'\u2044': // ⁄
          case u'\u2215': // ∕
          case u'\uFF0F': // ／
            output.push_back('/');
            break;

          case u'\uFF08': // （
          case u'\u207D': // ⁽
          case u'\u208D': // ₍
          case u'\uFE59': // ﹙
            output.push_back('(');
            break;

          case u'\uFF09': // ）
          case u'\u207E': // ⁾
          case u'\u208E': // ₎
          case u'\uFE5A': // ﹚
            output.push_back(')');
            break;

          case u'\uFF0C': // ，
          case u'\uFE50': // ﹐
            output.push_back(',');
            break;

          case u'\uFF05': // ％
            output.push_back('%');
            break;

#ifdef CALC_ALLOW_UNICODE_IN_GUI
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

        | Unicode | Calc | | ------- |
        ------------------------------------------------------------ | | `∞` |
        `inf`                                                        | | `π` |
        `pi`                                                         | | `ϖ` |
        `pi`                                                         | | `e` |
        `e`                                                          | | `ℯ` |
        `e`                                                          | | `φ` |
        `phi`                                                        | | `ϕ` |
        `phi`                                                        | | `γ` |
        `e_gamma`                                                    | | `ℇ` |
        `e`                                                          | Γ → gamma

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
          default:
            // All ASCII chars converted directly
            if (input[position] <= WCHAR(0x7F)) [[likely]] {
              output.push_back(static_cast<char>(input[position]));
              break;
            }

            // Any other non-ASCII character is not part of the current Calc
            // language and should be rejected here.
            [[unlikely]] return position;
          }
      }
      // All characters processed successfully.
      assert(position == size);
      [[likely]] return position;
    }
  };

public:
  constexpr CalcGUI() noexcept = default;

  CalcGUI(const CalcGUI &) = delete;
  CalcGUI(CalcGUI &&) = delete;

  constexpr ~CalcGUI() noexcept = default;

  /**
   * Perform calculation from the GUI
   */
  void calc() noexcept {
    EditView input(_layout.handle(0));
    EquasionHandler equasion(input, _equasion);
    Lexer l(equasion.data());
    Parser p(l);
    const auto value = p.parse();
#if defined(CALC_USE_ERROR_TOKEN)
    if (value.type == Token::Type::RESULT) [[likely]] {
      Formatter::Result result [[indeterminate]];
      set_result(result.data(), Formatter::format(value, result));
    } else if (value.type == Token::Type::ERROR) {
        set_result(value.error_text, value.error_text + value.error_text_size));
    } else [[unlikely]] {
      constexpr std::string_view err = "parser internal error";
      set_result(err.data(), err.data() + err.size());
    }
#else
    if (IssueManager::has_errors()) [[unlikely]] {
      Formatter::Summary summary [[indeterminate]];
      set_result(summary.data(), Formatter::create_summary(summary));
    } else [[likely]] {
      Formatter::Result result [[indeterminate]];
      set_result(result.data(), Formatter::format(value, result));
    }
#endif
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
  constexpr void close(const HWND window) const noexcept {
    EndDialog(window, FALSE);
  }

  /**
   * Initialize Calc GUI and load user data into it.
   */
  constexpr void init(const HWND window, const HINSTANCE instance) noexcept {
    set_icons(window, instance);
    add_about_menu_to_system_menu(window);
#ifdef CALC_SUPPORT_DPI_CHANGES
    _dpi = dpi(window);
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    init_min_sizes(window, physical(cfg::min_width, _dpi),
                   physical(cfg::min_height, _dpi), _dpi);
#else
    init_min_sizes(window, physical(cfg::min_width, _dpi),
                   physical(cfg::min_height, _dpi));
#endif
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
  }

  /**
   * Resize Calc window.
   */
  constexpr inline void resize(const WORD width, const WORD height) noexcept {
    _layout.resize(width, height);
  }

  /**
   * Return to the system minimal sizes for Calc window.
   */
  constexpr inline void minmaxinfo(const LPMINMAXINFO lpMMI) const noexcept {
    lpMMI->ptMinTrackSize.x = _layout.min_x();
    lpMMI->ptMinTrackSize.y = _layout.min_y();
  }
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  constexpr void set_dpi(const HWND window, const WORD dpi,
                         Rect_ptr rect) noexcept {
    _dpi = dpi;
    init_min_sizes(window, physical(cfg::min_width, _dpi),
                   physical(cfg::min_height, _dpi), _dpi);
    SetWindowPos(window, nullptr, rect->x(), rect->y(), rect->width(),
                 rect->heigth(), SWP_NOZORDER | SWP_NOACTIVATE);
  }
#endif
  /**
   * Initialize layout helper for resizing.
   */
  constexpr void layout_init(const HWND window) noexcept {
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
  [[nodiscard]] constexpr auto &theme() { return _theme; }
#endif

private:
  /**
   * Set the result text in the GUI.
   */
  constexpr void set_result(const char *text, char *text_end) const noexcept {
    set_text(_layout.handle(1), text, text_end);
  }

  constexpr void init_min_sizes(const HWND window, const LONG min_width,
                                const LONG min_height
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
                                ,
                                UINT new_dpi
#endif
                                ) noexcept {
    // Convert minimum client area size to window (outer) size so the user can't
    // resize window smaller than the intended client area. WM_GETMINMAXINFO
    // expects window dimensions.
    Rect client(0, 0, min_width, min_height);

    // Retrieve window styles to adjust for non-client area.
    const auto style = static_cast<DWORD>(GetWindowLongPtrA(window, GWL_STYLE));
    const auto exStyle =
        static_cast<DWORD>(GetWindowLongPtrA(window, GWL_EXSTYLE));

    // AdjustWindowRectEx will expand the rectangle so that the resulting outer
    // window will have the requested client size.
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    AdjustWindowRectExForDpi(&client, style, FALSE, exStyle, new_dpi);
#else
    AdjustWindowRectEx(&client, style, FALSE, exStyle);
#endif
    _layout.init_min_sizes(client.width(), client.height());
  }

  constexpr void load_window_data(const HWND window) noexcept {
    const RegRead reg(HKEY_CURRENT_USER, cfg::reg_key);
    Edit input(_layout.handle(0), cfg::input_max_text_length);
#ifndef CALC_TESTS_ENABLED
    input.set_size(reg.read("input", input.bytes(), cfg::input_max_data_size));
#else
    auto tests = calc_tests();
    input.write(tests.data(), tests.size());
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
    reg.write("input", input.bytes(), input.size());
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

  [[no_unique_address]] std::string _equasion;

  [[no_unique_address]] Layout<cfg::elements> _layout [[indeterminate]];
#ifdef CALC_SUPPORT_DARK_MODE
  [[no_unique_address]] Theme _theme [[indeterminate]];
#endif
#ifdef CALC_SUPPORT_DPI_CHANGES
  [[no_unique_address]] UINT _dpi [[indeterminate]];
#endif
};

static CalcGUI gui;

/**
 * About dialog callback processing (resource-based).
 */
constexpr static INT_PTR CALLBACK AboutDlgProc(const HWND window,
                                               const UINT msg,
                                               const WPARAM wParam,
                                               const LPARAM
#ifdef CALC_SUPPORT_LINK_WINDOW
                                                   lParam
#endif
                                               ) noexcept {
  switch (msg) {
  case WM_COMMAND: {
    if (LOWORD(wParam) == IDCANCEL) {
      EndDialog(window, FALSE);
      return TRUE;
    }
    break;
  }
#ifdef CALC_SUPPORT_LINK_WINDOW
  case WM_NOTIFY: {
    const auto nm = reinterpret_cast<LPNMHDR>(lParam);
    if (nm->idFrom == IDC_LINK_HOMEPAGE && nm->code == NM_CLICK) {
      const auto link = reinterpret_cast<NMLINK *>(lParam);
      ShellExecuteW(window, L"open", link->item.szUrl, nullptr, nullptr,
                    SW_SHOWNORMAL);
      return TRUE;
    }
    break;
  }
#endif
#ifdef CALC_SUPPORT_DARK_MODE
  case WM_INITDIALOG: {
    gui.theme().apply(window);
    return TRUE;
  }
#ifdef CALC_SUPPORT_DARK_MODE_WITHOUT_WIN32_HELPER
  case WM_CTLCOLORDLG:
  case WM_CTLCOLORSTATIC: {
    return gui.theme().apply(reinterpret_cast<HDC>(wParam));
  }
#endif
  case WM_SYSCOLORCHANGE: {
    gui.theme().apply(window, true);
    return TRUE;
  }
#endif
  }
  return FALSE;
}

/**
 * Calc dialog callback processing (resource-based).
 */
constexpr static INT_PTR CALLBACK CalcDialogProc(const HWND window,
                                                 const UINT msg,
                                                 const WPARAM wParam,
                                                 const LPARAM lParam) noexcept {
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  static bool dpi_change_in_progress = false;
#endif
  switch (msg) {
  case WM_COMMAND: {
    if (LOWORD(wParam) == IDC_BUTTON_CALC && HIWORD(wParam) == BN_CLICKED) {
      gui.calc();
      return TRUE;
    }
    break;
  }
  case WM_SYSCOMMAND: {
    if ((wParam & 0xFFF0) == IDM_ABOUTBOX) {
      DialogBoxParamA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDD_ABOUTBOX),
                      window, AboutDlgProc, FALSE);
      return TRUE;
    }
    break;
  }
  case WM_GETMINMAXINFO: {
    gui.minmaxinfo(reinterpret_cast<LPMINMAXINFO>(lParam));
    return TRUE;
  }
  case WM_SIZE: {
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
    if (dpi_change_in_progress) {
      gui.layout_init(window);
      break;
    }
#endif
    gui.resize(LOWORD(lParam), HIWORD(lParam));
    return TRUE;
  }
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  case WM_GETDPISCALEDSIZE: {
    // https://learn.microsoft.com/en-us/windows/win32/hidpi/wm-getdpiscaledsize
    dpi_change_in_progress = true;
    break;
  }
  case WM_DPICHANGED: {
    gui.set_dpi(window, HIWORD(wParam),
                reinterpret_cast<const Rect_ptr>(lParam));
    dpi_change_in_progress = false;
    return TRUE;
  }
#endif
  case WM_INITDIALOG: {
#ifdef CALC_SUPPORT_DARK_MODE
    gui.theme().init(window);
#endif
    gui.init(window, reinterpret_cast<HINSTANCE>(lParam));
    return TRUE;
  }
#ifdef CALC_SUPPORT_AUTO_RESTART
  case WM_ENDSESSION: {
    if (wParam) {
      gui.save_user_data(window);
    }
    return TRUE;
  }
#endif
#ifdef CALC_SUPPORT_DARK_MODE
#ifdef CALC_SUPPORT_DARK_MODE_WITHOUT_WIN32_HELPER
  case WM_CTLCOLORSTATIC:
  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORDLG: {
    return gui.theme().apply(reinterpret_cast<HDC>(wParam));
  }
#endif
  case WM_SYSCOLORCHANGE: {
    gui.theme().apply(window, true, true);
    return TRUE;
  }
#endif
  case WM_CLOSE: {
    gui.save_user_data(window);
    gui.close(window);
    return TRUE;
  }
  }
  return FALSE;
}

/**
 * Calc entrypoint in Win32 GUI
 */
int WINAPI WinMain(const HINSTANCE instance, const HINSTANCE /*prev_instance*/,
                   const LPSTR /*cmd_line*/, const int /*cmd_show*/) {
#ifdef _DEBUG
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF |
                 _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF |
                 _CRTDBG_CHECK_EVERY_1024_DF);
#endif

#ifdef CALC_SUPPORT_DARK_MODE
  init_uxtheme_callers();
#endif

#ifdef CALC_SUPPORT_DPI_CHANGES
#ifdef CALC_SUPPORT_DPI_CHANGES_WITHOUT_RESTART
  // https://learn.microsoft.com/en-us/windows/win32/hidpi/dpi-awareness-context
  //  Per monitor DPI aware. This window checks for the DPI when it is created
  //  and adjusts the scale factor whenever the DPI changes. These processes are
  //  not automatically scaled by the system.
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#else
  // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setprocessdpiaware
  // System DPI aware. This window does not scale for DPI changes. It will query
  // for the DPI once and use that value for the lifetime of the process. If the
  // DPI changes, the process will not adjust to the new DPI value. It will be
  // automatically scaled up or down by the system when the DPI changes from the
  // system value.
  // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
  SetProcessDPIAware();
#endif
#endif

#ifdef CALC_SUPPORT_AUTO_RESTART
  RegisterApplicationRestart(nullptr, FALSE);
#endif

#ifdef CALC_DISABLE_IME
  // Disable IME completely because Calc use only ANSI input in GUI
  ImmDisableIME(FALSE);
#endif

  DialogBoxParamA(instance, MAKEINTRESOURCEA(IDD_CALC_DIALOG), nullptr,
                  CalcDialogProc, reinterpret_cast<LPARAM>(instance));

  return EXIT_SUCCESS;
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
