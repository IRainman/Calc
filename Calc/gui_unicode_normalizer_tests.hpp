/*
 * Copyright 2026-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef EQUASION_NORMALIZATON_HPP
#define EQUASION_NORMALIZATON_HPP

#ifdef CALC_ALLOW_UNICODE_IN_GUI

namespace GUI {

#ifdef CALC_TESTS_ENABLED

/**
 * Test cases for GUI
 */
// clang-format off
static auto normalizer_tests = std::to_array<std::pair<std::string_view, std::string_view>>({
    { "½∜(7π÷2)", "1/2qdrt(7pi/2)" },
    { "½∜(7π÷3)R☉×c * (⅒+ ⅟₉)⏨₉  +ℯ⁶", "1/2qdrt(7pi/3)R_sun*c * (1/10+ 1/9)*10^(9)  +e^(6)" },

    // Separators.
    { "\t\n\v\f\r ", "      " },
    { "\u00A0", " " },
    { "\u2000\u2001\u2002\u2003\u2004\u2005\u2006\u2007\u2008\u2009\u200A\u200B\u200C\u200D\u200E\u200F\u2010\u2011\u2012\u2013\u2014\u2015\u2016\u2017\u2018\u2019\u201A\u201B\u201C\u201D\u201E\u201F\u2020\u2021\u2022\u2023\u2024\u2025\u2026\u2027\u2028\u2029\u202A\u202B\u202C\u202D\u202E\u202F\u3000", "                                                 " },

    // Fullwidth forms.
    { "ＡＢＣＸＹＺａｂｃｘｙｚ０１２３４５６７８９＋－＊／（）＝＜＞＠＃％＆", "ABCXYZabcxyz0123456789+-*/()=<>@#%&" },
    { "！＂＃＄％＆＇（）＊＋，－．／０１２３４５６７８９：；＜＝＞？＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～", "!\\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~" },
    { "！＂＃＄％＆＇（）＊＋，－．／", "!\"#$%&'()*+,-./" },
    { "０１２３４５６７８９：；＜＝＞？＠", "0123456789:;<=>?@" },
    { "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ", "ABCDEFGHIJKLMNOPQRSTUVWXYZ" },
    { "ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ", "abcdefghijklmnopqrstuvwxyz" },
    { "［＼］＾＿｀｛｜｝～", "[]\\^_`{|}~" },

    // Operators.
    { "+-*/", "+-*/" },
    { "·×÷", "***" },
    { "⁄∕⧸", "///" },
    { "⁎∗⋅﹡", "****" },
    { "₊﹢₋−﹣", "++---" },

    // Parentheses and punctuation.
    { "()₍₎﹙﹚", "()()()" },
    { "﹐", "," },
    { "﹒", "." },

    // Fractions.
    { "¼½¾", "1/41/23/4" },
    { "⅐⅑⅒⅓⅔⅕⅖⅗⅘", "1/71/91/101/32/31/52/53/54/5" },
    { "⅙⅚⅛⅜⅝⅞⅟↉", "1/65/61/83/85/87/81/00/3" },
    { "3⅟₉", "31/9" },

    // Mathematical constants and Greek letters.
    { "πτφϕⅇΓαζμµσϐ∞ℯ", "pi(2*pi)phiphie_atomicgammaalphariemann_zetamumumusigmabetainfe" },
    { "Γαζμµσεϐ∞", "gammaalphariemann_zetamumumusigmaepsilonbetainf" },
    { "ℯℇƵ", "eE" }, // Ƶ is intentionally unsupported after "ℯℇ".

    // Physical constants.
    { "ℎℏ", "hhbar" },
    { "Ɛℇ", "EE" },

    // Mathematical functions.
    { "√x", "sqrt(x)" },
    { "∛x", "cbrt(x)" },
    { "∜x", "qdrt(x)" },
    { "√(x)", "sqrt(x)" },
    { "∛(x)", "cbrt(x)" },
    { "∜(x)", "qdrt(x)" },
    { "√x+1", "sqrt(x)+1" },
    { "∛x−1", "cbrt(x)-1" },
    { "∜x×2", "qdrt(x)*2" },
    { "√x y", "sqrt(x) y" },
    { "√x,y", "sqrt(x),y" },
    { "√√x", "sqrt(sqrt(x))" },
    { "√∛x", "sqrt(cbrt(x))" },
    { "∛∜x", "cbrt(qdrt(x))" },
    { "㏑x", "ln(x)" },
    { "㏒x", "log(x)" },
    { "㏑x+㏒y", "ln(x)+log(y)" },
    { "√x²", "sqrt(x^(2))" },

    // Decimal exponent notation.
    { "1⏨3", "1*10^(3)" },
    { "1⏨−3", "1*10^(-3)" },
    { "1⏨(3)", "1*10^(3)" },
    { "⏨₉", "*10^(9)" },
    { "2⏨³", "2*10^(3)" },

    // Astronomical symbols.
    { "⊕♁", "_earth_earth" },
    { "⊙☉☼", "_sun_sun_sun" },
    { "☽☾", "_moon_moon" },
    { "☿", "_mercury" },
    { "♀", "_venus" },
    { "♂", "_mars" },
    { "♃", "_jupiter" },
    { "♄", "_saturn" },
    { "♅", "_uranus" },
    { "♆", "_neptune" },
    { "♇", "_pluto" },
    { "㍳", "*au" },
    { "㍶", "*pc" },
    { "R☉×c", "R_sun*c" },

    // Subscript digits and letters.
    { "₀₁₂₃₄₅₆₇₈₉", "0123456789" },
    { "ₐₑₒₓₕₖₗₘₙₚₛₜ", "aeoxhklmnpst" },
    { "x₂₃₄₅₆₇₈₉", "x23456789" },
    { "x₄", "" }, // U+2084 is not a subscript letter and is handled as a digit.
    { "ₔ", "" },  // U+2094 is explicitly excluded from the subscript-letter lookup.

    // Superscripts: individual characters.
    { "x⁰", "x^(0)" },
    { "x¹", "x^(1)" },
    { "x²", "x^(2)" },
    { "x³", "x^(3)" },
    { "x⁴", "x^(4)" },
    { "x⁵", "x^(5)" },
    { "x⁶", "x^(6)" },
    { "x⁷", "x^(7)" },
    { "x⁸", "x^(8)" },
    { "x⁹", "x^(9)" },
    { "xⁱ", "x^(i)" },
    { "xⁿ", "x^(n)" },
    { "x⁺", "x^(+)" },
    { "x⁻", "x^(-)" },
    { "x⁼", "x^(=)" },

    // Superscript grouping/state machine.
    { "x²³", "x^(23)" },
    { "x⁴⁵⁶⁷⁸⁹", "x^(456789)" },
    { "xⁱⁿ", "x^(in)" },
    { "x²⁺⁻⁼", "x^(2+-=)" },
    { "x²+1", "x^(2)+1" },
    { "x²)", "x^(2))" },
    { "x² y", "x^(2) y" },
    { "²", "^(2)" },
    { "ⁿ", "^(n)" },
    { "²³⁴⁵⁶⁷⁸⁹", "^(23456789)" },

    // Superscript parentheses. These intentionally lock in the current state-machine behavior.
    { "⁽", "^()" },
    { "⁾", "^))" },
    { "⁽⁾", "^())" },
    { "x⁽⁾", "x^())" },

    // Combinations of normalization features.
    { "½π²", "1/2pi^(2)" },
    { "α+β−γ×μ÷σ", "alpha+beta-gamma*mu/sigma" },
    { "√(1−½)²", "sqrt(1-1/2)^(2)" },
    { "⅓×π+⅔×π", "1/3*pi+2/3*pi" },
    { "R☉=cℏ", "R_sun=c hbar" },
    { "㏑(1)+㏒(10)", "ln(1)+log(10)" },
    { "１２３．４５６", "123.456" },
    { "﹣９﹒０９", "-9.09" },

    // ASCII pass-through and separator boundaries.
    { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" },
    { "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~", "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~" },
    { "  abc\tdef\nxyz ", "  abc def xyz " },

    // Unsupported characters: normalization must stop at the first unsupported UTF-16 code unit.
    { "A😀B", "A" },
    { "abc©def", "abc" },
    { "abcₔdef", "abc" },
    { "abc⁲def", "abc" },
    { "﹣₉﹒₀₉⏨₉", "-9.09*10^(9)" },

    { "12³×(231² - 1)³ + 744 - 0.00000000000075", "12^(3)*(231^(2) - 1)^(3) + 744 - 0.00000000000075" },
	// TODO: user constants and variables support:
	// Also I ask ChatGPT https://chatgpt.com/c/6a89e39f-5610-83eb-8182-d914dc425042
	// Which syntax we should use?
	{ "x=π2;", "x=pi2;" },
	// Intervals needed:
	{ "[0...π]", "[0...pi]" },
	{ "[0...π, step]", "[0...pi, step]" },
	// blocks?
	{ "{x= 150 ⁄ 12 + 1 }", "{x= 150 / 12 + 1 }" },
	// TODO: equation support, for input test https://xrjunque.nom.es/AllInOne:
	{ "{x=((−b ? √(b²−4⋅a⋅c))⁄(2a))π}", "{x=((-b ? sqrt(b^(2)-4*a*c))/(2a))pi}" },

    { "|—J│Оa╓Чn", "| J"},
    { "±", "" },

	// convert MathML to plain before processing (needs another library):
    { "<math><mrow><mi>x</mi><mo>=</mo><mfrac><mrow><mo ... skip this! >−</mo><mi>b</mi><mo>?</mo><msqrt><mrow><msup><mi>b</mi><mn>2</mn></msup><mo>−</mo><mn>4</mn><mi>a</mi><mi>c</mi></mrow></msqrt></mrow><mrow><mn>2</mn><mi>a</mi></mrow></mfrac><mo>⋅</mo><mi>π</mi></mrow></math>",
      // It's the simple normalization to ASCII.
      "<math><mrow><mi>x</mi><mo>=</mo><mfrac><mrow><mo ... skip this! >-</mo><mi>b</mi><mo>?</mo><msqrt><mrow><msup><mi>b</mi><mn>2</mn></msup><mo>-</mo><mn>4</mn><mi>a</mi><mi>c</mi></mrow></msqrt></mrow><mrow><mn>2</mn><mi>a</mi></mrow></mfrac><mo>*</mo><mi>pi</mi></mrow></math>" },
});
// clang-format on
#endif

/**
 * @class Normalizer
 * @brief Transforms Unicode and ANSI mathematical notation into
 *        ASCII-compatible representation.
 *
 * This class handles normalization for preprocessing mathematical expressions,
 * converting specialized mathematical symbols to their standard ASCII
 * equivalents. Supports extensive Unicode character mappings including:
 *
 *   - Mathematical constants: π → "pi", φ → "phi", ℯ → "e"
 *   - Mathematical functions: √ → "sqrt(", ∛ → "cbrt(", ∜ → "qdrt("
 *   - Vulgar fractions: ¼ → "1/4", ⅕ → "1/5", etc.
 *   - Greek letters: α → "alpha", β → "beta", γ → "gamma", etc.
 *   - Superscripts: ² → "^(2)", ⁵ → "^(5)", ⁿ → "^(n)"
 *   - Subscripts: ₀₋₉ → direct digit mapping
 *   - Operators: × → "*", ÷ → "/", − → "-", etc.
 *   - Fullwidth characters: normalized to ASCII range
 *   - Whitespace variants: normalized to ASCII space
 *   - Astronomical symbols: ⊕ → "_earth", ☉ → "_sun"
 *   - Physical constants: ℏ → "hbar", ℎ → "h"
 *
 * The normalizer is designed for constexpr evaluation, supporting both
 * compile-time and runtime processing. It is strict about memory requirements
 * and uses reference semantics to minimize overhead via the
 * [[no_unique_address]] attribute.
 *
 * @note Non-copyable and non-default-constructible utility class.
 * @note The output buffer must have capacity of at least input.length() * 4
 * bytes since some Unicode characters expand to multiple ASCII characters.
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
   * Initiates the normalization process by calling the Operation functor with
   * the equation buffer. The normalization is performed immediately within the
   * constructor via resize_and_overwrite.
   *
   * @param edit Reference to the Edit containing the input Unicode text to
   * normalize
   * @param equasion Reference to a std::string buffer that will contain the
   * normalized output
   *
   * @pre equasion.capacity() must be >= edit.length() * 4 to accommodate
   * expanded characters
   * @post equasion contains the normalized ANSI representation up to the failed
   * character or end of input if successful
   *
   * @note The constructor performs the actual normalization work through
   * resize_and_overwrite
   * @warning Behavior is undefined if the precondition is not met
   */
  explicit constexpr Normalizer(EditView &edit, std::string &equasion) noexcept
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
   * @return true if normalization failed (incomplete), false if all input was
   * normalized
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
  [[no_unique_address]] EditView &_edit;
  /// Count of normalized input chars
  [[no_unique_address]] uint32_t _normalized;

  /**
   * @struct Operation
   * @brief Functor that performs the actual Unicode and ANSI normalization.
   *
   * This struct is the core normalization engine, implementing a state machine
   * that processes UTF-16 input character-by-character and writes normalized
   * ASCII output. It handles multiple parsing states:
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
    LPWSTR begin;
    /// Length of input string in characters
    LPCWSTR end;

    /**
     * @brief Copies a string literal to the output buffer and returns new
     * position.
     *
     * Template specialization on string length allows compile-time computation
     * of the copy size. Uses memcpy for efficiency and skips the null
     * terminator.
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
     * @brief Classifies if a character is a superscript digit, operator, or
     * variable.
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
     *   - ASCII control: tab, newline, vertical tab, form feed, carriage return
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
     * @brief Classifies operator characters.
     *
     * Includes basic arithmetic operators and mathematical symbols:
     *   - ASCII: +, -, *, /
     *   - ANSI: · (0xB7), × (0xD7), ÷ (0xF7)
     *   - Unicode: All characters in mathematical operators range
     * (0x2200-0x22FF)
     *
     * @param c Character to test
     * @return true if c is an operator
     */
    [[nodiscard]] inline static constexpr bool is_operator(WCHAR c) noexcept {
      return (c == L'+' || c == L'-' || c == L'*' || c == L'/' || // ASCII
              c == 0xB7 || c == 0xD7 || c == 0xF7 ||              // ANSI
              c >= 0x2200 && c <= 0x22FF                          // Unicode
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
     *   - A separator space, tab, etc.
     *   - An operator +, -, *, /, etc.
     *   - A parenthesis (, )
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
     * Unicode-to-ASCII conversions and state machine management. This is the
     * primary normalization algorithm.
     *
     * **Algorithm Overview:**
     * 1. Initialize state machines (superscript flag, function nesting counter)
     * 2. Iterate through each input character:
     *    a. Check if exiting superscript context and append closing ')'
     *    b. Check if exiting function context and append closing ')'
     *    c. Process character:
     *       - Printable ASCII → copy directly
     *       - Separator → replace with ASCII space
     *       - Fullwidth (0xFF01-0xFF5E) → map to ASCII via subtraction
     *       - Subscript digits (0x2080-0x2089) → map ASCII to digits
     *       - Subscript letters (0x2090-0x209C) → lookup in letter array
     *       - Switch statement (400+ mappings) for special symbols:
     *         * Operators: ×→*, ÷→/, etc.
     *         * Fractions: ½→"1/2", ⅖→"2/5", etc.
     *         * Constants: π→"pi", ℏ→"hbar", etc.
     *         * Functions: √→"sqrt(", ∛→"cbrt(", etc.
     *         * Superscripts: wrap in "^(...)" notation
     *       - Unknown character → stop processing
     * 3. Finalize: close any open parenthesis from superscript/function modes
     *
     * **State Machines:**
     * - **Superscript mode (bool in_superscript)**:
     *   Activated by superscript characters, generates "^(...)" wrapper.
     *   Closed when a non-superscript character is encountered.
     *
     * - **Function mode (uint8_t in_function)**:
     *   Incremented when function names (sqrt, ln, log, etc.) are encountered.
     *   Tracks nesting depth (though typically 0 or 1 in practice).
     *   Decremented when function_end condition is met.
     *
     * **Performance Characteristics:**
     * - Single-pass algorithm: O(n) where n = input length
     * - No backtracking or lookahead (except next-char check for functions)
     * - Direct character-by-character output: minimal memory operations
     * - Uses switch statement (compiler jump table vs if-chain)
     * - Memcpy for multi-character replacements
     *
     * @param buffer Output buffer where normalized ASCII string will be written
     * @param bufferCapacity Size of output buffer (unused, determined by
     * caller)
     * @return Length of normalized output in bytes (not including null
     * terminator)
     *
     * @note This function signature matches std::string::resize_and_overwrite's
     *       operation callback, allowing it to be used as a callable in that
     * API
     * @note The 'normalized' reference is updated to track input position
     * during processing
     * @note Returns the final output buffer position minus the starting
     * position
     *
     * @warning Assumes buffer has capacity >= input.length() * 8
     * @warning Will stop processing and return partial result on unsupported
     * character
     */
    [[nodiscard]] inline unsigned int operator()(char *buffer,
                                                 unsigned int) noexcept {
      normalized = 0;

      char *output = buffer;

      // Tracks if we are currently in a superscript block
      bool in_superscript = false;

      // Tracks nesting depth of implicit function argument parentheses
      uint8_t in_function = 0;

      // Main processing loop - processes all input characters
      while (begin != end) [[likely]] {
        const auto c = *begin;

        // Exit superscript mode if encountering a non-superscript character
        if (in_superscript && !is_superscript(c)) {
          in_superscript = false;
          *output++ = ')';
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

#if 0
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

          // PLUS-MINUS SIGN' (U+00B1) ±
          // MINUS-OR-PLUS SIGN' (U+2213) ∓
#endif

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

        case 0x03D0: // β GREEK BETA SYMBOL
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
  if (begin + 1 != end && *(begin + 1) != L'(') {                              \
    ++in_function;                                                             \
    *output++ = '(';                                                           \
  }

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

        case 0x23E8: // ⏨ DECIMAL EXPONENT SYMBOL
          output = append(output, "*10^");
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
  if (!in_superscript) {                                                       \
    in_superscript = true;                                                     \
    output = append(output, "^(");                                             \
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

        // Superscript Unicode parentheses (⁽, ⁾)
        // Special handling: these can appear in pairs, so use ^ prefix only
        // once
        case 0x207D: // ⁽ SUPERSCRIPT LEFT PARENTHESIS
        case 0x207E: // ⁾ SUPERSCRIPT RIGHT PARENTHESIS
          if (!in_superscript) {
            in_superscript = true;
            *output++ = '^';
          }
          *output++ = c == 0x207D ? '(' : ')';
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
        ++begin;
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
