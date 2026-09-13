/*
 * Copyright 2026-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef EQUASION_NORMALIZATON_HPP
#define EQUASION_NORMALIZATON_HPP

#ifdef CALC_ALLOW_UNICODE_IN_GUI

namespace GUI {

#ifdef CALC_TESTS_ENABLED
// clang-format off
/**
 * Test cases for GUI Unicode normalization.
 *
 * Each test case contains:
 *   - input: UTF-8 encoded source text as std::string_view.
 *   - failed: expected Normalizer::failed() result.
 *   - output: expected normalized ASCII text.
 *
 * The input strings are UTF-8 because these tests exercise the same text
 * representation used by the GUI test adapter before it is converted to
 * UTF-16 for Normalizer. ASCII input may be written directly; non-ASCII
 * characters should remain UTF-8 encoded in the source file, or use explicit
 * UTF-8 byte escapes where source-encoding ambiguity matters.
 *
 * A failed test case means normalization must stop at the first unsupported
 * UTF-16 code unit. The expected output therefore contains only the portion
 * successfully normalized before that character.
 *
 * Test groups are organized by normalization feature so that a failing case
 * identifies the affected mapping or state-machine rule:
 *   - Separators: whitespace and Unicode space normalization.
 *   - Fullwidth forms: compatibility forms mapped to ASCII.
 *   - Operators: arithmetic operator variants.
 *   - Parentheses and punctuation: compatibility punctuation mappings.
 *   - Fractions: vulgar fractions and fraction numerator forms.
 *   - Constants and Greek letters: mathematical and physical symbols.
 *   - Mathematical functions: roots, logarithms, and implicit arguments.
 *   - Decimal exponent notation: the U+23E8 exponent symbol.
 *   - Astronomical symbols: celestial-body names and astronomical units.
 *   - Subscripts: numeric and supported letter subscripts.
 *   - Superscripts: individual mappings and superscript state transitions.
 *   - Superscript parentheses: parenthesized superscript syntax.
 *   - Combinations: interaction between independent normalization features.
 *   - ASCII pass-through: printable ASCII and separator handling.
 *   - Unsupported characters: early termination and partial output.
 *
 * Keep the cases deterministic and focused: each test should verify one
 * normalization rule or a small, intentional combination of rules.
 */
static auto normalizer_tests = std::to_array< std::pair< std::string_view, 
    std::pair< bool, std::string_view > > >({
    // Separators
    { "\t\n\v\f\r ",  { false, "      " }}, // ASCII

    // Separators
    { "\xC2\xA0",     { false, " " } }, // ANSI 0xA0, U+00A0 NO-BREAK SPACE

    // Separators
    { "\xE3\x80\x80", { false, " " } }, // U+3000 IDEOGRAPHIC SPACE

    // Separators from 0x2000 to 0x202F
    { "\xE2\x80\x80", { false, " " } }, // U+2000 EN QUAD
    { "\xE2\x80\x81", { false, " " } }, // U+2001 EM QUAD
    { "\xE2\x80\x82", { false, " " } }, // U+2002 EN SPACE
    { "\xE2\x80\x83", { false, " " } }, // U+2003 EM SPACE
    { "\xE2\x80\x84", { false, " " } }, // U+2004 THREE-PER-EM SPACE
    { "\xE2\x80\x85", { false, " " } }, // U+2005 FOUR-PER-EM SPACE
    { "\xE2\x80\x86", { false, " " } }, // U+2006 SIX-PER-EM SPACE
    { "\xE2\x80\x87", { false, " " } }, // U+2007 FIGURE SPACE
    { "\xE2\x80\x88", { false, " " } }, // U+2008 PUNCTUATION SPACE
    { "\xE2\x80\x89", { false, " " } }, // U+2009 THIN SPACE
    { "\xE2\x80\x8A", { false, " " } }, // U+200A HAIR SPACE
    { "\xE2\x80\x8B", { false, " " } }, // U+200B ZERO WIDTH SPACE
    { "\xE2\x80\x8C", { false, " " } }, // U+200C ZERO WIDTH NON-JOINER
    { "\xE2\x80\x8D", { false, " " } }, // U+200D ZERO WIDTH JOINER
    { "\xE2\x80\x8E", { false, " " } }, // U+200E LEFT-TO-RIGHT MARK
    { "\xE2\x80\x8F", { false, " " } }, // U+200F RIGHT-TO-LEFT MARK
    { "\xE2\x80\x90", { false, " " } }, // U+2010 HYPHEN
    { "\xE2\x80\x91", { false, " " } }, // U+2011 NON-BREAKING HYPHEN
    { "\xE2\x80\x92", { false, " " } }, // U+2012 FIGURE DASH
    { "\xE2\x80\x93", { false, " " } }, // U+2013 EN DASH
    { "\xE2\x80\x94", { false, " " } }, // U+2014 EM DASH
    { "\xE2\x80\x95", { false, " " } }, // U+2015 HORIZONTAL BAR
    { "\xE2\x80\x96", { false, " " } }, // U+2016 DOUBLE VERTICAL LINE
    { "\xE2\x80\x97", { false, " " } }, // U+2017 DOUBLE LOW LINE
    { "\xE2\x80\x98", { false, " " } }, // U+2018 LEFT SINGLE QUOTATION MARK
    { "\xE2\x80\x99", { false, " " } }, // U+2019 RIGHT SINGLE QUOTATION MARK
    { "\xE2\x80\x9A", { false, " " } }, // U+201A SINGLE LOW-9 QUOTATION MARK
    { "\xE2\x80\x9B", { false, " " } }, // U+201B SINGLE HIGH-REVERSED-9 QUOTATION MARK
    { "\xE2\x80\x9C", { false, " " } }, // U+201C LEFT DOUBLE QUOTATION MARK
    { "\xE2\x80\x9D", { false, " " } }, // U+201D RIGHT DOUBLE QUOTATION MARK
    { "\xE2\x80\x9E", { false, " " } }, // U+201E DOUBLE LOW-9 QUOTATION MARK
    { "\xE2\x80\x9F", { false, " " } }, // U+201F DOUBLE HIGH-REVERSED-9 QUOTATION MARK
    { "\xE2\x80\xA0", { false, " " } }, // U+2020 DAGGER
    { "\xE2\x80\xA1", { false, " " } }, // U+2021 DOUBLE DAGGER
    { "\xE2\x80\xA2", { false, " " } }, // U+2022 BULLET
    { "\xE2\x80\xA3", { false, " " } }, // U+2023 TRIANGULAR BULLET
    { "\xE2\x80\xA4", { false, " " } }, // U+2024 ONE DOT LEADER
    { "\xE2\x80\xA5", { false, " " } }, // U+2025 TWO DOT LEADER
    { "\xE2\x80\xA6", { false, " " } }, // U+2026 HORIZONTAL ELLIPSIS
    { "\xE2\x80\xA7", { false, " " } }, // U+2027 HYPHENATION POINT
    { "\xE2\x80\xA8", { false, " " } }, // U+2028 LINE SEPARATOR
    { "\xE2\x80\xA9", { false, " " } }, // U+2029 PARAGRAPH SEPARATOR
    { "\xE2\x80\xAA", { false, " " } }, // U+202A LEFT-TO-RIGHT EMBEDDING
    { "\xE2\x80\xAB", { false, " " } }, // U+202B RIGHT-TO-LEFT EMBEDDING
    { "\xE2\x80\xAC", { false, " " } }, // U+202C POP DIRECTIONAL FORMATTING
    { "\xE2\x80\xAD", { false, " " } }, // U+202D LEFT-TO-RIGHT OVERRIDE
    { "\xE2\x80\xAE", { false, " " } }, // U+202E RIGHT-TO-LEFT OVERRIDE
    { "\xE2\x80\xAF", { false, " " } }, // U+202F NARROW NO-BREAK SPACE
    
    // Fullwidth forms.
    {          "ＡＢＣＸＹＺａｂｃｘｙｚ０１２３４５６７８９＋－＊／（）＝＜＞＠＃％＆",
      { false, "ABCXYZabcxyz0123456789+-*/()=<>@#%&" }},
    {          "！＂＃＄％＆＇（）＊＋，－．／０１２３４５６７８９：；＜＝＞？＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～",
      { false, "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" }},
    {          "！＂＃＄％＆＇（）＊＋，－．／",
      { false, "!\"#$%&'()*+,-./" }},
    {          "０１２３４５６７８９：；＜＝＞？＠", 
      { false, "0123456789:;<=>?@" }},
    {          "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ",
      { false, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" }},
    {          "ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ",
      { false, "abcdefghijklmnopqrstuvwxyz" }},
    {          "［＼］＾＿｀｛｜｝～", 
      { false, "[\\]^_`{|}~" }},
   
    // Operators.
    { "+-*/", { false, "+-*/" }},
    { "·×÷", { false, "**/" }},
    { "⁄∕⧸", { false, "///" }},
    { "⁎∗⋅﹡", { false, "****" }},
    { "₊﹢₋−﹣", { false, "++---" }},

    // Parentheses and punctuation.
    { "()₍₎﹙﹚", { false, "()()()" }},
    { "﹐", { false, "," }},
    { "﹒", { false, "." }},

    // Fractions.
    { "¼½¾", { false, "1/41/23/4" }},
    { "⅐⅑⅒⅓⅔⅕⅖⅗⅘", { false, "1/71/91/101/32/31/52/53/54/5" }},
    { "⅙⅚⅛⅜⅝⅞⅟↉", { false, "1/65/61/83/85/87/81/0/3" }},
    { "3⅟₉", { false, "31/9" }},

    // Mathematical constants and Greek letters.
    { "πτφϕⅇΓαζμµσϐ∞ℯ", { false, "pi(2*pi)phiphie_atomicgammaalphariemann_zetamumusigmabetainfe" }},
    { "Γγαζμµσεβϐ∞", { false, "gammagammaalphariemann_zetamumusigmaepsilonbetabetainf" }},
    { "Γαζμµσεϐ∞", { false, "gammaalphariemann_zetamumusigmaepsilonbetainf" }},
    { "ℯℇƵ", { true, "eE" }}, // Ƶ is intentionally unsupported after "ℯℇ".
  
    // Physical constants.
    { "ℎℏ", { false, "hhbar" }},
    { "Ɛℇ", { false, "EE" }},

    // Mathematical functions.
    { "√x",   { false, "sqrt(x)" }},
    { "∛x",   { false, "cbrt(x)" }},
    { "∜x",   { false, "qdrt(x)" }},
    { "√(x)", { false, "sqrt(x)" }},
    { "∛(x)", { false, "cbrt(x)" }},
    { "∜(x)", { false, "qdrt(x)" }},
    { "√x+1", { false, "sqrt(x)+1" }},
    { "∛x−1", { false, "cbrt(x)-1" }},
    { "∜x×2", { false, "qdrt(x)*2" }},
    { "√x y", { false, "sqrt(x) y" }},
    { "√x,y", { false, "sqrt(x),y" }},
    { "⎷x",   { false, "sqrt(x)" }},
    { "√√x",  { false, "sqrt(sqrt(x))" }},
    { "√∛x",  { false, "sqrt(cbrt(x))" }},
    { "∛∜x",  { false, "cbrt(qdrt(x))" }},
    { "㏑x",     { false, "ln(x)" }},
    { "㏒x",     { false, "log(x)" }},
    { "㏑x+㏒y", { false, "ln(x)+log(y)" }},
    { "√x²",  { false, "sqrt(x^(2))" }},
    { "√😀", { true, "sqrt()" }},

    // Decimal exponent notation.
    { "1⏨3", { false, "1e3" }},
    { "1⏨−3", { false, "1e-3" }},
    { "⏨₉", { false, "e9" }},
    { "2⏨³", { false, "2e3" }},
    { "⏨⁹", { false, "e9" }},
    { "(2)⏨3", { false, "(2)*10^(3)" }},
    { "(2)⏨(3)", { false, "(2)*10^(3)" }},

    // Astronomical symbols.
    { "⊕♁", { false, "_earth_earth" }},
    { "⊙☉☼", { false, "_sun_sun_sun" }},
    { "☽☾", { false, "_moon_moon" }},
    { "☿", { false, "_mercury" }},
    { "♀", { false, "_venus" }},
    { "♂", { false, "_mars" }},
    { "♃", { false, "_jupiter" }},
    { "♄", { false, "_saturn" }},
    { "♅", { false, "_uranus" }},
    { "♆", { false, "_neptune" }},
    { "♇", { false, "_pluto" }},
    { "㍳", { false, "*au" }},
    { "㍶", { false, "*pc" }},
    { "R☉×c", { false, "R_sun*c" }},

    // Subscript digits and letters.
    { "₀₁₂₃₄₅₆₇₈₉", { false, "0123456789" }},
    { "ₐₑₒₓₕₖₗₘₙₚₛₜ", { false, "aeoxhklmnpst" }},
    { "x₂₃₄₅₆₇₈₉", { false, "x23456789" }},
    { "₊₋₍₎", { false, "+-()" }},
    { "ₔ", { true, "" }},  // U+2094 is explicitly excluded from the subscript-letter lookup.

    // Superscripts: individual characters.
    { "x⁰", { false, "x^(0)" }},
    { "x¹", { false, "x^(1)" }},
    { "x²", { false, "x^(2)" }},
    { "x³", { false, "x^(3)" }},
    { "x⁴", { false, "x^(4)" }},
    { "x⁵", { false, "x^(5)" }},
    { "x⁶", { false, "x^(6)" }},
    { "x⁷", { false, "x^(7)" }},
    { "x⁸", { false, "x^(8)" }},
    { "x⁹", { false, "x^(9)" }},
    { "xⁱ", { false, "x^(i)" }},
    { "xⁿ", { false, "x^(n)" }},
    { "x⁺", { false, "x^(+)" }},
    { "x⁻", { false, "x^(-)" }},
    { "x⁼", { false, "x^(=)" }},
    
    // Superscript grouping/state machine.
    { "x²³", { false, "x^(23)" }},
    { "x⁴⁵⁶⁷⁸⁹", { false, "x^(456789)" }},
    { "xⁱⁿ", { false, "x^(in)" }},
    { "x²⁺⁻⁼", { false, "x^(2+-=)" }},
    { "x²+1", { false, "x^(2)+1" }},
    { "x²)", { false, "x^(2))" }},
    { "x² y", { false, "x^(2) y" }},
    { "²", { false, "2" }},
    { "ⁿ", { false, "n" }},
    { "²³⁴⁵⁶⁷⁸⁹", { false, "23456789" }},

    // Superscript parentheses.
    { "⁽", { false, "(" }},
    { "⁾", { false, ")" }},
    { "⁽⁾", { false, "()" }},
    { "x⁽⁾", { false, "x^(())" }},
    { "x⁽ⁿ⁾", { false, "x^((n))" }},
    { "x²😀", { true, "x^(2)" }},
    
    // Combinations of normalization features.
    { "½π²", { false, "1/2pi^(2)" }},
    { "(½π)²", { false, "(1/2pi)^(2)" }},
    { "α+β−γ×μ÷σ", { false, "alpha+beta-gamma*mu/sigma" }},
    { "√(1−½)²", { false, "sqrt(1-1/2)^(2)" }},
    { "⅓×π+⅔×π", { false, "1/3*pi+2/3*pi" }},
    { "⅓π+⅔π", { false, "1/3pi+2/3pi" }},
    { "R☉=cℏ", { false, "R_sun=chbar" }},
    { "㏑(1)+㏒(10)", { false, "ln(1)+log(10)" }},
    { "１２３．４５６", { false, "123.456" }},
    { "﹣９﹒０９", { false, "-9.09" }},
    
    // ASCII pass-through and separator boundaries.
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", { false, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" }},
    { "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", { false, "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~" }},
    { "  abc\tdef\nxyz ", { false, "  abc def xyz " }},

    // Unsupported characters: normalization must stop at the first unsupported UTF-16 code unit.
    { "A😀B", { true, "A" }},
    { "abc©def", { true, "abc" }},
    { "abcₔdef", { true, "abc" }},
    { "abc⁲def", { true, "abc" }},
    { "|—J│Оa╓Чn", { true, "| J" }},
    { "±", { true, "" }},
    
    { "½∜(7π÷2)", { false, "1/2qdrt(7pi/2)" }},
    { "½∜(7π÷3)R☉×c * (⅒+ ⅟₉)⏨₉  +ℯ⁶", { false ,"1/2qdrt(7pi/3)R_sun*c * (1/10+ 1/9)*10^(9)  +e^(6)" }},
    { "﹣₉﹒₀₉⏨₉", { false, "-9.09e9" }},
    { "12³×(231² - 1)³ + 744 - 0.00000000000075", { false, "12^(3)*(231^(2) - 1)^(3) + 744 - 0.00000000000075" }},
	// TODO: user constants and variables support:
	// Also I ask ChatGPT https://chatgpt.com/c/6a89e39f-5610-83eb-8182-d914dc425042
	// Which syntax we should use?
	{ "x=π2;", { false, "x=pi2;" }},
	// Intervals needed:
	{ "[0...π]", { false, "[0...pi]" }},
	{ "[0...π, step]", { false, "[0...pi, step]" }},
	// blocks?
	{ "{x= 150 ⁄ 12 + 1 }", { false, "{x= 150 / 12 + 1 }" }},
	// TODO: equation support, for input test https://xrjunque.nom.es/AllInOne:
	{ "{x=((−b ? √(b²−4⋅a⋅c))⁄(2a))π}", { false, "{x=((-b ? sqrt(b^(2)-4*a*c))/(2a))pi}" }},
});
// clang-format on
#endif

/**
 * @class Normalizer
 * @brief Transforms Unicode mathematical notation into an ASCII-compatible
 *        representation.
 *
 * This class handles normalization for preprocessing mathematical
 * expressions, converting selected Unicode mathematical notation to the
 * ASCII syntax expected by the calculator. It supports:
 *
 *   - Mathematical constants and symbols: π → "pi", τ → "(2*pi)", φ/ϕ → "phi",
 *     ∞ → "inf", ℯ → "e", ⅇ → "e_atomic"
 *   - Mathematical functions: √/⎷ → "sqrt", ∛ → "cbrt", ∜ → "qdrt",
 *     ㏑ → "ln", ㏒ → "log"
 *   - Vulgar fractions: ¼, ½, ¾ and the supported Unicode fraction forms
 *   - Selected Greek letters: α, β, γ/Γ, ε, ζ, μ/µ and σ
 *   - Superscripts: supported digits/operators/letters are grouped as "^(...)"
 *     after ordinary input; a superscript-only leading input is emitted
 *     without the wrapper
 *   - Subscripts: ₀₋₉ and the supported Unicode subscript letters
 *   - Operators and punctuation: common Unicode variants normalized to ASCII
 *   - Fullwidth forms: U+FF01-U+FF5E mapped directly to U+0021-U+007E
 *   - Whitespace/separators: supported Unicode separator characters mapped
 *     to ASCII space
 *   - Astronomical identifiers: Earth, Sun, Moon and the supported planets
 *   - Astronomical units: ㍳ → "*au", ㍶ → "*pc"
 *   - Physical constants/symbols: ℏ → "hbar", ℎ → "h", Ɛ/ℇ → "E"
 *
 * Normalization is a single-pass runtime operation used through
 * std::string::resize_and_overwrite(). References are stored with
 * [[no_unique_address]].
 *
 * @note Non-copyable and non-default-constructible utility class.
 * @note The current mappings have a maximum expansion of 12 ASCII bytes per
 *       input UTF-16 code unit (ζ → "riemann_zeta"). A capacity based on
 *       input.length() * 12 is therefore sufficient for the currently supported
 *       mappings, although the constructor assertion currently checks only
 *       input.length() * 4.
 *
 * Usage:
 * ```cpp
 * std::string equation(input_text.length() * 4);
 * auto normalizer = Normalizer(edit_view, equation);
 * if (normalizer.failed()) {
 *     // Handle unsupported character
 * }
 * // equation now contains normalized ASCII representation
 * ```
 *
 * @see EditView
 */
class Normalizer {
public:
  Normalizer() = delete;
  Normalizer(Normalizer const &) = delete;
  Normalizer &operator=(Normalizer const &) = delete;
  Normalizer(Normalizer &&) = delete;

  /**
   * @brief Constructs a Normalizer and performs the normalization.
   *
   * Initiates the normalization process by calling the Operation functor
   * with the equation buffer. The normalization is performed immediately
   * within the constructor via resize_and_overwrite.
   *
   * @param edit Reference to the Edit containing the input Unicode text to
   * normalize
   * @param equasion Reference to a std::string buffer that will contain the
   * normalized output
   *
   * @pre equasion.capacity() must be large enough for the complete expanded
   *      ASCII result. For the current mappings, edit.length() * 12 is a
   *      sufficient worst-case bound. The implementation currently asserts
   *      only capacity() > edit.length() * 4.
   * @post equasion contains the normalized ASCII representation up to the
   * failed character or end of input if successful
   *
   * @note The constructor performs the actual normalization work through
   *       resize_and_overwrite.
   * @warning Behavior is undefined if the output buffer is too small.
   */
  explicit constexpr Normalizer(const EditView &edit, std::string &equasion) noexcept
      : _equasion(equasion), _edit(edit) {
    assert(_equasion.capacity() > _edit.length() * 4);
    _equasion.resize_and_overwrite(
        _equasion.capacity(),
        Operation{_normalized, _edit.text(), _edit.text() + _edit.length()});
  }

  /**
   * @brief Destructor that clears the normalized output.
   */
  constexpr ~Normalizer() noexcept { _equasion.clear(); }

  /**
   * @brief Checks if normalization encountered an unsupported character.
   *
   * Returns true if the normalization process stopped before processing all
   * input characters, indicating an unsupported or invalid character was
   * encountered.
   *
   * @return true if normalization failed (incomplete), false if all input
   * was normalized
   *
   * @see normalized()
   */
  [[nodiscard]] constexpr auto failed() const noexcept {
    return _normalized != _edit.length();
  }

  /**
   * @brief Returns the count of successfully normalized characters.
   *
   * @return Number of input characters that were successfully normalized
   */
  [[nodiscard]] constexpr auto normalized() const noexcept {
    return _normalized;
  }

private:
  /// Reference to output buffer
  [[no_unique_address]] std::string &_equasion;
  /// Reference to input view
  [[no_unique_address]] const EditView &_edit;
  /// Count of normalized input chars
  [[no_unique_address]] uint32_t _normalized;

  /**
   * @struct Operation
   * @brief Functor that performs the actual Unicode and ANSI normalization.
   *
   * This struct is the core normalization engine, implementing a state
   * machine that processes UTF-16 input character-by-character and writes
   * normalized ASCII output. It handles multiple parsing states:
   *
   *   - **Superscript mode**: Tracks when superscript characters are
   * encountered and wraps them in "^(...)" notation
   *   - **Function mode**: Automatically inserts parentheses for function
   * symbols like sqrt, cbrt, etc., tracking nesting depth
   *
   * The normalization logic uses a combination of:
   *   - Direct ASCII pass-through for printable ASCII (0x21-0x7E)
   *   - Lookup tables for subscript/superscript mappings
   *   - Comprehensive switch statement for many Unicode and ANSI mappings
   *   - Inline helper functions for character classification
   *
   * @note Uses [[nodiscard]] on all classification methods to prevent
   * accidental misuse
   * @note The operator() is designed for single-pass processing with no
   * backtracking
   */
  struct Operation {
    /// Reference to counter tracking processed input chars
    uint32_t &normalized;
    /// UTF-16 input string pointer
    LPCWSTR begin;
    /// Length of input string in characters
    LPCWSTR end;

    /**
     * @brief Copies a string literal to the output buffer and returns new
     * position.
     *
     * The array bound gives the exact literal size to copy. Uses memcpy for
     * efficiency and skips the null terminator.
     *
     * @tparam N Deduced string literal size including null terminator
     * @param out Output buffer position
     * @param str String literal to append
     * @return Pointer to the position after copied content
     *
     * Example: append(out, "pi") copies 2 bytes and returns out+2
     */
    template <uint8_t N>
    [[nodiscard]] inline static constexpr char *
    append(char *out, const char (&str)[N]) noexcept {
      std::memcpy(out, str, N - 1);
      return out + N - 1;
    }

    /**
     * @brief Classifies whether a character belongs to the supported
     *        superscript code-point set.
     *
     * Used by the superscript state machine to determine if the current
     * character should continue the superscript sequence or trigger its
     * closure.
     *
     * Recognized superscripts:
     *   - ANSI: ² (0xB2), ³ (0xB3), ¹ (0xB9)
     *   - Unicode: ⁰⁻⁹ (0x2070-0x2079), ⁺⁻⁼⁽⁾ (0x207A-0x207E), ⁱⁿ (0x2071,
     * 0x207F)
     *
     * @param c Character to test
     * @return true if c is in the superscript Unicode range
     */
    [[nodiscard]] inline static constexpr bool
    is_superscript(WCHAR c) noexcept {
      return (c == 0xB2 || c == 0xB3 || c == 0xB9 || // ANSI ²³¹
              c >= 0x2070 && c <= 0x207F             // Unicode ⁰ⁱ⁴⁵⁶⁷⁸⁹⁺⁻⁼⁽⁾ⁿ
      );
    }

    /**
     * @brief Classifies whitespace and separator characters.
     *
     * Normalized to single ASCII space (0x20). Includes:
     *   - ASCII control: tab, newline, vertical tab, form feed, carriage
     * return
     *   - ASCII space (0x20)
     *   - ANSI non-breaking space (0xA0)
     *   - Unicode spaces: general punctuation (0x2000-0x202F), fullwidth
     * (0x3000)
     *
     * @param c Character to test
     * @return true if c is a separator/whitespace character
     */
    [[nodiscard]] inline static constexpr bool is_separator(WCHAR c) noexcept {
      return (c >= L'\t' && c <= L'\r' ||   // ASCII control characters
              c == L' ' ||                  // ASCII space
              c == 0xA0 ||                  // ANSI NO-BREAK SPACE
              c >= 0x2000 && c <= 0x202F || // Unicode general punctuation
              c == 0x3000                   // Unicode fullwidth space
      );
    }

    /**
     * @brief Classifies characters that terminate an implicit function
     *        argument.
     *
     * Includes ASCII arithmetic operators and comma, legacy single-byte
     * mathematical operator values, and broad Unicode mathematical-operator
     * ranges. U+221A-U+221C (√, ∛, ∜) are intentionally excluded from these
     * ranges because they are handled as function symbols.
     *
     * @param c Character to test
     * @return true if c is an operator
     */
    [[nodiscard]] inline static constexpr bool is_operator(WCHAR c) noexcept {
      return (c == L'+' || c == L'-' || c == L'*' || c == L'/' || // ASCII
              c == L',' ||                                        // ASCII
              c == 0xB7 || c == 0xD7 || c == 0xF7 ||              // ANSI
              c >= 0x2200 && c <= 0x2219 || // Unicode before roots
              c >= 0x221E && c <= 0x22FF    // Unicode after roots

      );
    }

    /**
     * @brief Classifies parenthesis and bracket characters.
     *
     * Includes:
     *   - ASCII: (, )
     *   - Unicode subscript: ₍ (0x208D), ₎ (0x208E)
     *   - Unicode small: ﹙ (0xFE59), ﹚ (0xFE5A)
     *
     * @param c Character to test
     * @return true if c is a parenthesis
     */
    [[nodiscard]] inline static constexpr bool is_parentesis(WCHAR c) noexcept {
      return (c == L'(' || c == L')' ||     // ASCII
              c == 0x208D || c == 0x208E || // Unicode ₍ ₎
              c == 0xFE59 || c == 0xFE5A    // Unicode﹙﹚
      );
    }

    /**
     * @brief Classifies printable ASCII characters.
     *
     * ASCII range 0x21-0x7E (! through ~) that can pass through unchanged.
     *
     * @param c Character to test
     * @return true if c is in the printable ASCII range
     */
    [[nodiscard]] inline static constexpr bool
    is_printable_ascii(WCHAR c) noexcept {
      return c >= 0x21 && c <= 0x7E; // ASCII ! to ~
    }

    /**
     * @brief Detects the boundary of an implicit function argument.
     *
     * Used by the function state machine to determine when to close the
     * automatically inserted opening parenthesis after function names
     * like "sqrt", "cbrt", "ln", etc.
     *
     * A function argument ends when encountering:
     *   - A separator such as space, tab, newline, or a supported Unicode space
     *   - An operator, comma, or supported mathematical-operator code point
     *   - A parenthesis
     *
     * @param c Character to test
     * @return true if c marks the end of a function's implicit argument
     */
    [[nodiscard]] inline static constexpr bool
    is_function_end(WCHAR c) noexcept {
      return is_separator(c) || is_operator(c) || is_parentesis(c);
    }

    /**
     * @brief Normalization functor - the main processing engine.
     *
     * Processes the entire input string character-by-character, performing
     * Unicode-to-ASCII conversions and state machine management. This is
     * the primary normalization algorithm.
     *
     * **Algorithm Overview:**
     * 1. Initialize superscript and implicit-function state.
     * 2. Iterate through each UTF-16 code unit:
     *    a. Close active superscript/function state when the current character
     *       marks a boundary.
     *    b. Pass through printable ASCII unchanged.
     *    c. Normalize separators, fullwidth forms, and subscripts.
     *    d. Dispatch special symbols through the switch statement.
     *    e. Stop at the first unsupported code unit.
     * 3. Finalize by closing any still-active superscript/function state.
     *
     * **State Machines:**
     * - **Superscript mode (bool in_superscript):**
     *   Activated by superscripts after ordinary input, generating a "^(...)"
     *   wrapper. Closed when a non-superscript character is encountered.
     *
     * - **Initial-superscript mode (bool only_superscript):**
     *   Keeps a leading superscript-only input unwrapped. It is cleared as soon
     *   as a non-superscript code unit is encountered.
     *
     * - **Function mode (uint8_t in_function):**
     *   Counts implicit argument parentheses opened by sqrt, cbrt, qdrt, ln,
     *   log, or the parenthesized decimal-exponent form following a ')' .
     *   It is decremented when a separator, operator, or parenthesis ends the
     *   implicit argument.
     *
     * **Performance Characteristics:**
     * - Single-pass algorithm: O(n) where n = input length
     * - No backtracking or lookahead (except next-char check for functions)
     * - Direct character-by-character output: minimal memory operations
     * - Uses switch statement (compiler jump table vs if-chain)
     * - Memcpy for multi-character replacements
     *
     * @param buffer Output buffer where normalized ASCII string will be
     * written
     * @param bufferCapacity Size of output buffer (unused, determined by
     * caller)
     * @return Length of normalized output in bytes (not including null
     * terminator)
     *
     * @note This function signature matches
     * std::string::resize_and_overwrite's operation callback, allowing it
     * to be used as a callable in that API
     * @note The 'normalized' reference is updated to track input position
     * during processing
     * @note Returns the final output buffer position minus the starting
     * position
     *
     * @warning The buffer capacity must be sufficient for the complete expanded
     *          output. For the current mappings, input.length() * 12 is a
     *          sufficient worst-case bound. The constructor assertion currently
     *          checks only capacity() > input.length() * 4.
     * @warning Processing stops at the first unsupported UTF-16 code unit and
     *          returns the successfully normalized prefix.
     */
    [[nodiscard]] inline unsigned int operator()(char *buffer,
                                                 unsigned int) noexcept {
      normalized = 0;

      auto input = begin;

      auto output = buffer;

      // Tracks if we are currently in a superscript block
      bool in_superscript = false;

      // Tracks if we have input started from superscript
      bool only_superscript = false;

      // Tracks nesting depth of implicit function argument parentheses
      uint8_t in_function = 0;

      // Main processing loop - processes all input characters
      while (input != end) [[likely]] {
        const auto c = *input;

        // Exit superscript mode if encountering a non-superscript character
        if (!is_superscript(c)) {
          only_superscript = false;
          if (in_superscript) {
            in_superscript = false;
            *output++ = ')';
          }
        }

        // Exit function mode when encountering function argument boundary
        if (in_function && is_function_end(c)) {
          --in_function;
          *output++ = ')';
        }

        // Pass through printable ASCII characters unchanged
        if (is_printable_ascii(c)) {
          *output++ = static_cast<char>(c);
          goto normalization_of_current_end;
        }

        // Replace all separators with ASCII space
        if (is_separator(c)) {
          *output++ = ' '; // ASCII 0x20
          goto normalization_of_current_end;
        }

        // Map fullwidth forms (U+FF01-U+FF5E) directly to ASCII (0x21-0x7E)
        if (c >= 0xFF01 && c <= 0xFF5E) {
          *output++ = static_cast<char>(c - 0xFEE0);
          goto normalization_of_current_end;
        }

        // Map subscript digits (₀-₉) directly to ASCII digits (0-9)
        if (c >= 0x2080 && c <= 0x2089) {
          *output++ = static_cast<char>(c - 0x2050);
          goto normalization_of_current_end;
        }

        // Map subscript letters (ₐₑₒₓ ₕₖₗₘₙₚₛₜ) via lookup table
        if (c >= 0x2090 && c <= 0x209C && c != 0x2094) {
          *output++ = "aeox hklmnpst"[c - 0x2090];
          goto normalization_of_current_end;
        }

        // Dispatch to appropriate handler for special characters
        switch (c) {

        // Operators - ANSI and Unicode (Small and Subscripts)

        // Plus sign variants
        case 0x208A: // ₊ SUBSCRIPT PLUS SIGN
        case 0xFE62: // ﹢ SMALL PLUS SIGN
          *output++ = '+';
          break;

        // Minus sign variants
        case 0x208B: // ₋ SUBSCRIPT MINUS SIGN
        case 0x2212: // − MINUS SIGN
        case 0xFE63: // ﹣ SMALL HYPHEN-MINUS
          *output++ = '-';
          break;

        // Division sign variants
        case 0x2044: // ⁄ FRACTION SLASH
        case 0x2215: // ∕ DIVISION SLASH
        case 0x29F8: // ⧸ BIG SOLIDUS
        case 0xF7:   // ÷ ANSI DIVISION SIGN
          *output++ = '/';
          break;

        // Multiplication sign variants
        case 0x204E: // ⁎ LOW ASTERISK
        case 0x2217: // ∗ ASTERISK OPERATOR
        case 0x22C5: // ⋅ DOT OPERATOR
        case 0xFE61: // ﹡ SMALL ASTERISK
        case 0xB7:   // · ANSI MIDDLE DOT
        case 0xD7:   // × ANSI MULTIPLICATION SIGN
          *output++ = '*';
          break;

        // TODO https://www.fileformat.info/info/unicode/category/Sm/list.htm
        // Relational Operators
        case 0x2260: // ≠
          output = append(output, "!=");
          break;

        case 0x2264: // ≤
          output = append(output, "<=");
          break;

        case 0x2265: // ≥
          output = append(output, ">=");
          break;

        case 0x2261: // ≡
          output = append(output, "==");
          break;

        case 0x2248: // ≈
        case 0x2243: // ≃
        case 0x2245: // ≅
          output = append(output, "~=");
          break;

        // case PLUS-MINUS SIGN' (U+00B1) ±
        // case MINUS-OR-PLUS SIGN' (U+2213) ∓

        // Fractions - ANSI (¼, ½, ¾)
        case 0xBC: // ¼
          output = append(output, "1/4");
          break;

        case 0xBD: // ½
          output = append(output, "1/2");
          break;

        case 0xBE: // ¾
          output = append(output, "3/4");
          break;

        // Fractions - Unicode (comprehensive coverage)
        case 0x2150: // ⅐ VULGAR FRACTION ONE SEVENTH
          output = append(output, "1/7");
          break;

        case 0x2151: // ⅑ VULGAR FRACTION ONE NINTH
          output = append(output, "1/9");
          break;

        case 0x2152: // ⅒ VULGAR FRACTION ONE TENTH
          output = append(output, "1/10");
          break;

        case 0x2153: // ⅓ VULGAR FRACTION ONE THIRD
          output = append(output, "1/3");
          break;

        case 0x2154: // ⅔ VULGAR FRACTION TWO THIRDS
          output = append(output, "2/3");
          break;

        case 0x2155: // ⅕ VULGAR FRACTION ONE FIFTH
          output = append(output, "1/5");
          break;

        case 0x2156: // ⅖ VULGAR FRACTION TWO FIFTHS
          output = append(output, "2/5");
          break;

        case 0x2157: // ⅗ VULGAR FRACTION THREE FIFTHS
          output = append(output, "3/5");
          break;

        case 0x2158: // ⅘ VULGAR FRACTION FOUR FIFTHS
          output = append(output, "4/5");
          break;

        case 0x2159: // ⅙ VULGAR FRACTION ONE SIXTH
          output = append(output, "1/6");
          break;

        case 0x215A: // ⅚ VULGAR FRACTION FIVE SIXTHS
          output = append(output, "5/6");
          break;

        case 0x215B: // ⅛ VULGAR FRACTION ONE EIGHTH
          output = append(output, "1/8");
          break;

        case 0x215C: // ⅜ VULGAR FRACTION THREE EIGHTHS
          output = append(output, "3/8");
          break;

        case 0x215D: // ⅝ VULGAR FRACTION FIVE EIGHTHS
          output = append(output, "5/8");
          break;

        case 0x215E: // ⅞ VULGAR FRACTION SEVEN EIGHTHS
          output = append(output, "7/8");
          break;

        case 0x215F: // ⅟ FRACTION NUMERATOR ONE
          output = append(output, "1/");
          break;

        case 0x2189: // ↉ VULGAR FRACTION ZERO THIRDS
          output = append(output, "0/3");
          break;

        // Parentheses - Unicode subscript and small forms
        case 0x208D: // ₍ SUBSCRIPT LEFT PARENTHESIS
        case 0xFE59: // ﹙ SMALL LEFT PARENTHESIS
          *output++ = '(';
          break;

        case 0x208E: // ₎ SUBSCRIPT RIGHT PARENTHESIS
        case 0xFE5A: // ﹚ SMALL RIGHT PARENTHESIS
          *output++ = ')';
          break;

        // Comma variants
        case 0xFE50: // ﹐ SMALL COMMA
          *output++ = ',';
          break;

        // Full stop variants
        case 0xFE52: // ﹒ SMALL FULL STOP
          *output++ = '.';
          break;

        // Mathematical constants - Greek letters and special forms
        case 0x03C0: // π GREEK SMALL LETTER PI
          output = append(output, "pi");
          break;

        case 0x03C4: // τ GREEK SMALL LETTER TAU
          output = append(output, "(2*pi)");
          break;

        case 0x03C6: // φ GREEK SMALL LETTER PHI
        case 0x03D5: // ϕ GREEK PHI SYMBOL
          output = append(output, "phi");
          break;

        case 0x2147: // ⅇ Double-struck italic e (charge)
          output = append(output, "e_atomic");
          break;

        case 0x0393: // Γ GREEK CAPITAL LETTER GAMMA
        case 0x03B3: // γ GREEK SMALL LETTER GAMMA
          output = append(output, "gamma");
          break;

        case 0x03B1: // α GREEK SMALL LETTER ALPHA
          output = append(output, "alpha");
          break;

        case 0x03B6: // ζ GREEK SMALL LETTER ZETA
          output = append(output, "riemann_zeta");
          break;

        // Mu - Unicode and ANSI
        case 0x03BC: // μ GREEK SMALL LETTER MU
        case 0xB5:   // µ ANSI MICRO SIGN
          output = append(output, "mu");
          break;

        case 0x03C3: // σ GREEK SMALL LETTER SIGMA
          output = append(output, "sigma");
          break;

        case 0xA7B5: // β LATIN SMALL LETTER BETA
        case 0x03D0: // ϐ GREEK BETA SYMBOL
        case 0x03B2: // β GREEK SMALL LETTER BETA
          output = append(output, "beta");
          break;

        case 0x221E: // ∞ INFINITY
          output = append(output, "inf");
          break;

        case 0x212F: // ℯ SCRIPT SMALL E
          *output++ = 'e';
          break;

        // Physical constants - Unicode
        case 0x0190: // Ɛ LATIN CAPITAL LETTER OPEN E
        case 0x2107: // ℇ EULER CONSTANT
          output = append(output, "E");
          break;

        case 0x210E: // ℎ PLANCK CONSTANT
          *output++ = 'h';
          break;

        case 0x210F: // ℏ PLANCK CONSTANT OVER TWO PI (H-BAR)
          output = append(output, "hbar");
          break;

        case 0x03B5: // ε GREEK SMALL LETTER EPSILON
          output = append(output, "epsilon");
          break;

#define START_FUNCTION                                                         \
  if (input + 1 != end && *(input + 1) != L'(') {                              \
    ++in_function;                                                             \
    *output++ = '(';                                                           \
  }
        // Special case
        case 0x23E8: // ⏨ DECIMAL EXPONENT SYMBOL
          if (begin <= input - 1 && *(input - 1) == L')') {
            output = append(output, "*10^");
            START_FUNCTION
          } else {
            *output++ = 'e';
          }
          break;

        // Unicode mathematical function symbols
        case 0x221A: // √ SQUARE ROOT
        case 0x23B7: // ⎷ RADICAL SYMBOL BOTTOM
          output = append(output, "sqrt");
          START_FUNCTION
          break;

        case 0x221B: // ∛ CUBE ROOT
          output = append(output, "cbrt");
          START_FUNCTION
          break;

        case 0x221C: // ∜ FOURTH ROOT
          output = append(output, "qdrt");
          START_FUNCTION
          break;

        // CJK compatibility functions - Square symbol notations
        case 0x33D1: // ㏑ SQUARE LN
          output = append(output, "ln");
          START_FUNCTION
          break;

        case 0x33D2: // ㏒ SQUARE LOG
          output = append(output, "log");
          START_FUNCTION
          break;

#undef START_FUNCTION

        // Astronomical symbols - Planets and celestial bodies
        case 0x2295: // ⊕ CIRCLED PLUS OPERATOR
        case 0x2641: //  ♁ EARTH
          output = append(output, "_earth");
          break;

        case 0x2299: // ☉ CIRCLED DOT OPERATOR
        case 0x2609: // ☉ SUN
        case 0x263C: // ☼ WHITE SUN WITH RAYS
          output = append(output, "_sun");
          break;

        case 0x263D: // ☽ FIRST QUARTER MOON
        case 0x263E: // ☾ LAST QUARTER MOON
          output = append(output, "_moon");
          break;

        case 0x263F: // ☿ MERCURY
          output = append(output, "_mercury");
          break;

        case 0x2640: // ♀ FEMALE SIGN
          output = append(output, "_venus");
          break;

        case 0x2642: // ♂ MALE SIGN
          output = append(output, "_mars");
          break;

        case 0x2643: // ♃ JUPITER
          output = append(output, "_jupiter");
          break;

        case 0x2644: // ♄ SATURN
          output = append(output, "_saturn");
          break;

        case 0x2645: // ♅ URANUS
          output = append(output, "_uranus");
          break;

        case 0x2646: // ♆ NEPTUNE
          output = append(output, "_neptune");
          break;

        case 0x2647: // ♇ PLUTO
          output = append(output, "_pluto");
          break;

        // CJK compatibility astronomical units
        case 0x3373: // ㍳ SQUARE AU (Astronomical Unit)
          output = append(output, "*au");
          break;

        case 0x3376: // ㍶ SQUARE PC (Parsec)
          output = append(output, "*pc");
          break;

          // Superscript - ANSI digits and Unicode variants

#define START_SUPERSCRIPT                                                      \
  if (!only_superscript) {                                                     \
    if (begin == input) {                                                      \
      only_superscript = true;                                                 \
    }                                                                          \
    if (!only_superscript && !in_superscript &&                                \
        *(input - 1) != 0x23E8 /*⏨*/) {                                        \
      in_superscript = true;                                                   \
      output = append(output, "^(");                                           \
    }                                                                          \
  }

        // Superscript ANSI digits (², ³, ¹)
        case 0xB2: // ² SUPERSCRIPT TWO
          START_SUPERSCRIPT
          *output++ = '2';
          break;

        case 0xB3: // ³ SUPERSCRIPT THREE
          START_SUPERSCRIPT
          *output++ = '3';
          break;

        case 0xB9: // ¹ SUPERSCRIPT ONE
          START_SUPERSCRIPT
          *output++ = '1';
          break;

        // Superscript Unicode letter
        case 0x2071: // ⁱ SUPERSCRIPT LATIN SMALL LETTER I
          START_SUPERSCRIPT
          *output++ = 'i';
          break;

        // case 0x2072: // not assigened.
        // case 0x2073: // not assigened.

        // Superscript Unicode digits (⁰, ⁴-⁹)
        case 0x2070: // ⁰ SUPERSCRIPT ZERO
        case 0x2074: // ⁴ SUPERSCRIPT FOUR
        case 0x2075: // ⁵ SUPERSCRIPT FIVE
        case 0x2076: // ⁶ SUPERSCRIPT SIX
        case 0x2077: // ⁷ SUPERSCRIPT SEVEN
        case 0x2078: // ⁸ SUPERSCRIPT EIGHT
        case 0x2079: // ⁹ SUPERSCRIPT NINE
          START_SUPERSCRIPT
          *output++ = '0' + static_cast<char>(c - 0x2070);
          break;

        // Superscript Unicode arithmetic operators (⁺, ⁻, ⁼)
        case 0x207A: // ⁺ SUPERSCRIPT PLUS SIGN
          START_SUPERSCRIPT
          *output++ = '+';
          break;

        case 0x207B: // ⁻ SUPERSCRIPT MINUS SIGN
          START_SUPERSCRIPT
          *output++ = '-';
          break;

        case 0x207C: // ⁼ SUPERSCRIPT EQUALS SIGN
          START_SUPERSCRIPT
          *output++ = '=';
          break;

        // Superscript Unicode parentheses (⁽, ⁾). Should be in pairs
        case 0x207D: // ⁽ SUPERSCRIPT LEFT PARENTHESIS
          START_SUPERSCRIPT
          *output++ = '(';
          break;

        case 0x207E: // ⁾ SUPERSCRIPT RIGHT PARENTHESIS
          START_SUPERSCRIPT
          *output++ = ')';
          break;

        // Superscript Unicode letter (ⁿ)
        case 0x207F: // ⁿ SUPERSCRIPT LATIN SMALL LETTER N
          START_SUPERSCRIPT
          *output++ = 'n';
          break;

#undef START_SUPERSCRIPT

        // Unknown or unsupported character - abort processing
        default:
          goto processing_end;
        }

      normalization_of_current_end:
        ++input;
        ++normalized;
      }

    processing_end:
      // Close any unclosed parentheses from active state machines
      if (in_superscript) {
        *output++ = ')';
      }
      while (in_function) {
        --in_function;
        *output++ = ')';
      }

      return static_cast<uint32_t>(output - buffer);
    }
  };
};
} // namespace GUI
#endif
#endif
