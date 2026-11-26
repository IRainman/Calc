/*
 * Copyright 2026-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef EQUASION_NORMALIZATON_HPP
#define EQUASION_NORMALIZATON_HPP

#ifdef CALC_SUPPORT_UNICODE_INPUT

namespace GUI {

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
 *   - Superscripts: consecutive supported superscript characters are grouped
 *     as "^(...)" after ordinary input. An explicitly supplied superscript
 *     left parenthesis (⁽) replaces the automatically inserted opening
 *     parenthesis. A leading superscript sequence is emitted without the
 *     generated wrapper.
 *   - Decimal exponent notation: ⏨ is normalized to "*10^" after an existing
 *     expression. Its exponent is handled as an implicit argument, unless an
 *     explicit opening parenthesis is already present.
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
 *
 * Usage:
 * ```cpp
 * EditView view(...);
 * // Preallocate enough space for normalized output
 * std::string equation(input_text.length() * 4);
 * Normalizer normalizer(view.text(), view.length(), equation);
 * if (normalizer.failed()) {
 *     // Handle unsupported input or an incomplete implicit argument
 * }
 * // equation now contains normalized ASCII representation
 * ```
 *
 * @see std::string::resize_and_overwrite()
 */
template <typename CharT, typename Length> class Normalizer {
  static_assert(sizeof(CharT) == sizeof(char16_t));
  static_assert(std::is_integral_v<CharT>);

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
   * @param text Pointer to the UTF-16 text that needs to be normalized.
   * @param length Number of UTF-16 chars in text.
   * @param equasion Reference to a std::string buffer that will contain the
   *        normalized output
   *
   * @pre equasion.capacity() must be large enough for the complete expanded
   *      ASCII result.
   *
   * @post equasion contains the normalized ASCII representation up to the
   *       failed character or end of input if successful
   *
   * @note The constructor performs the actual normalization work through
   *       resize_and_overwrite.
   *
   * @warning Behavior is undefined if the output buffer is too small.
   */
  constexpr explicit inline Normalizer(const CharT *const text,
                                       const Length length,
                                       std::string &equasion)
      : _length(length) {
    equasion.resize_and_overwrite(equasion.capacity(),
                                  Operation{_normalized, text, text + _length});
  }

  constexpr inline ~Normalizer() noexcept = default;

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
  [[nodiscard]] constexpr inline auto failed() const noexcept {
    return _normalized != _length;
  }

  /**
   * @brief Returns the count of successfully normalized characters.
   *
   * @return Number of input characters that were successfully normalized
   */
  [[nodiscard]] constexpr inline auto normalized() const noexcept {
    return _normalized;
  }

private:
  /// Count of the UTF-16 input text chars
  [[no_unique_address]] const Length _length;
  /// Count of succesfully normalized UTF-16 input chars
  [[no_unique_address]] Length _normalized;

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
    /// Reference to counter of succesfully normalized input chars
    Length &_normalized;
    /// UTF-16 input string pointer
    const CharT *const _begin;
    /// UTF-16 input end string pointer
    const CharT *const _end;

    /**
     * @brief Copies a string literal to the output buffer and returns new
     *        position.
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
    append(char *__restrict out, const char (&str)[N]) noexcept {
      std::memcpy(out, str, N - 1);
      return out + N - 1;
    }

    /**
     * @brief Classifies supported superscript characters.
     *
     * Recognized characters are explicitly listed in the implementation.
     * Unassigned and conditionally unsupported code points are excluded.
     *
     * @param c Character to test
     * @return true if c is a supported superscript character
     */
    [[nodiscard]] inline static constexpr bool
    is_superscript(const CharT c) noexcept {
      return (c == 0x00B2 || // U+00B2 ² SUPERSCRIPT TWO
              c == 0x00B3 || // U+00B3 ³ SUPERSCRIPT THREE
              c == 0x00B9 || // U+00B9 ¹ SUPERSCRIPT ONE
              c == 0x2070 || // U+2070 ⁰ SUPERSCRIPT ZERO
#ifdef CALC_TEST_EQUATION_SOLVER
              c == 0x2071 || // U+2071 ⁱ SUPERSCRIPT LATIN SMALL LETTER I
#endif
              // case 0x2072: // not assigned.
              // case 0x2073: // not assigned.
              c >= 0x2074 && c <= 0x207B || // U+2074-U+207B ⁴-⁹⁺⁻
#ifdef CALC_USED_EQUALS_OPERATORS
              c == 0x207C || // U+207C ⁼ SUPERSCRIPT EQUALS SIGN
#endif
              c == 0x207D || // U+207D ⁽ SUPERSCRIPT LEFT PARENTHESIS
              c == 0x207E    // U+207E ⁾ SUPERSCRIPT RIGHT PARENTHESIS
#ifdef CALC_TEST_EQUATION_SOLVER
              || c == 0x207F // U+207F ⁿ SUPERSCRIPT LATIN SMALL LETTER N
#endif
      );
    }

    /**
     * @brief Classifies whitespace and separator characters.
     *
     * Includes:
     *   - ASCII control: tab, newline, etc
     *   - ASCII space
     *   - ANSI non-breaking space
     *   - Unicode: general punctuation and fullwidth space
     *
     * @param c Character to test
     * @return true if c is a separator/whitespace character
     */
    [[nodiscard]] inline static constexpr bool
    is_separator(const CharT c) noexcept {
      return (
          c >= CharT('\t') && c <= CharT('\r') || // ASCII control characters
          c == CharT(' ') ||                      // ASCII space
          c == 0xA0 ||                            // ANSI NO-BREAK SPACE
          c == 0x2028 ||                          // LINE SEPARATOR
          c == 0x2029 ||                          // PARAGRAPH SEPARATOR
          c == 0x202F ||                          // NARROW NO-BREAK SPACE
          c == 0x205F ||                          // MEDIUM MATHEMATICAL SPACE
          c == 0x3000                             // IDEOGRAPHIC SPACE
      );
    }

    /**
     * @brief Classifies characters that terminate an implicit function
     *        argument.
     *
     * Includes ASCII, ANSI and Unicode mathematical-operators.
     *
     * @param c Character to test
     * @return true if c is an operator
     * @note roots are excluded from these ranges because they are handled as
     *       functions.
     */
    [[nodiscard]] inline static constexpr bool
    is_operator(const CharT c) noexcept {
      return (c == CharT('+') || c == CharT('-') || c == CharT('*') || // ASCII
              c == CharT('/') || c == CharT(',') ||                    // ASCII
              c == 0xB7 || c == 0xD7 || c == 0xF7 ||                   // ANSI
              c >= 0x2200 && c <= 0x2219 || // Unicode before roots
              c >= 0x221E && c <= 0x22FF    // Unicode after roots
      );
    }

    /**
     * @brief Classifies left parenthesis characters.
     *
     * Includes ASCII, Unicode subscript, Unicode small and Unicode fullwidth.
     *
     * @param c Character to test
     * @return true if c is a left parenthesis
     */
    [[nodiscard]] inline static constexpr bool
    is_left_parenthesis(const CharT c) noexcept {
      return (c == CharT('(') ||
              c == 0x208D || // U+208D ₍ SUBSCRIPT LEFT PARENTHESIS
              c == 0xFE59 || // U+FE59 ﹙ SMALL LEFT PARENTHESIS
              c == 0xFF08    // U+FF08 （ FULLWIDTH LEFT PARENTHESIS
      );
    }

    /**
     * @brief Classifies right parenthesis characters.
     *
     * Includes ASCII, Unicode subscript, Unicode small and Unicode fullwidth.
     *
     * @param c Character to test
     * @return true if c is a right parenthesis
     */
    [[nodiscard]] inline static constexpr bool
    is_right_parenthesis(const CharT c) noexcept {
      return (c == CharT(')') ||
              c == 0x208E || // U+208E ₎ SUBSCRIPT RIGHT PARENTHESIS
              c == 0xFE5A || // U+FE5A ﹚ SMALL RIGHT PARENTHESIS
              c == 0xFF09    // U+FF09 ） FULLWIDTH RIGHT PARENTHESIS
      );
    }

    /**
     * @brief Classifies any parenthesis character (left or right).
     *
     * @param c Character to test
     * @return true if c is a parenthesis
     */
    [[nodiscard]] inline static constexpr bool
    is_parenthesis(const CharT c) noexcept {
      return is_left_parenthesis(c) || is_right_parenthesis(c);
    }

    /**
     * @brief Classifies printable ASCII characters.
     *
     * ASCII range 0x20-0x7E (' ' through '~') that can pass through unchanged.
     *
     * @param c Character to test
     * @return true if c is in the printable ASCII range
     */
    [[nodiscard]] inline static constexpr bool
    is_printable_ascii(const CharT c) noexcept {
      return c >= 0x20 && c <= 0x7E;
    }

    /**
     * @brief Inserts an opening parenthesis for an implicit argument.
     *
     * This function is called when a function symbol like "sqrt" is detected
     * and the next character is not a left parenthesis. It increments the
     * implicit argument depth counter and appends '(' to the output buffer.
     *
     * @param input Pointer to the current input character
     * @param end Pointer to the end of the input string
     * @param output Reference to the current output buffer position
     * @param depth Reference to the implicit argument depth counter
     */
    [[nodiscard]] inline static constexpr bool
    start_implicit_argument(const CharT *const input, const CharT *const end,
                            char *&output, uint_fast16_t &depth) noexcept {
      if (input + 1 == end) {
        return false;
      }
      if (!is_left_parenthesis(input[1])) {
        ++depth;
        *output++ = '(';
      }
      return true;
    }

    /**
     * @brief Detects the boundary of an implicit argument.
     *
     * Used by the implicit argument state machine to determine when to close
     * the automatically inserted opening parenthesis after function names like
     * "sqrt", etc.
     *
     * An implicit argument ends when encountering:
     *   - A separator such as space, tab, newline, etc
     *   - An operator
     *   - A parenthesis
     *
     * @param c Character to test
     * @return true if c marks the end of an implicit argument
     */
    [[nodiscard]] inline static constexpr bool
    is_implicit_argument_end(const CharT c) noexcept {
      return is_separator(c) || is_operator(c) || is_parenthesis(c);
    }

    /**
     * @brief Classifies unary sign characters that follow an implicit function.
     *
     * Recognizes ASCII and Unicode minus/plus signs.
     *
     * @param c Character to test
     * @param previous Character immediately preceding c
     * @return true if c is a unary sign immediately following an implicit
     *         function symbol
     */
    [[nodiscard]] inline static constexpr bool
    is_unary_sign_after_implicit_function(const CharT c,
                                          const CharT previous) noexcept {
      return (c == CharT('+') || // U+002B + PLUS SIGN
              c == CharT('-') || // U+002D - HYPHEN-MINUS
              c == 0x2212 ||     // U+2212 − MINUS SIGN

              c == 0x208A || // U+208A ₊ SUBSCRIPT PLUS SIGN
              c == 0x208B || // U+208B ₋ SUBSCRIPT MINUS SIGN

              c == 0x207A || // U+207A ⁺ SUPERSCRIPT PLUS SIGN
              c == 0x207B || // U+207B ⁻ SUPERSCRIPT MINUS SIGN

              c == 0xFE62 || // U+FE62 ﹢ SMALL PLUS SIGN
              c == 0xFE63 || // U+FE63 ﹣ SMALL HYPHEN-MINUS

              c == 0xFF0B || // U+FF0B ＋ FULLWIDTH PLUS SIGN
              c == 0xFF0D    // U+FF0D － FULLWIDTH HYPHEN-MINUS
              ) &&
             (previous == 0x221A || // √ SQUARE ROOT
              previous == 0x23B7 || // ⎷ RADICAL SYMBOL BOTTOM
              previous == 0x221B || // ∛ CUBE ROOT
              previous == 0x221C || // ∜ FOURTH ROOT
              previous == 0x33D1 || // ㏑ SQUARE LN
              previous == 0x33D2    // ㏒ SQUARE LOG
#ifdef CALC_USED_EQUALS_OPERATORS
              || previous == 0x00B1 // ± PLUS-MINUS SIGN
              || previous == 0x2213 // ∓ MINUS-OR-PLUS SIGN
#endif
             );
    }

    /**
     * @brief Classifies decimal exponent sign characters.
     *
     * Recognizes ASCII, Unicode, subscript, superscript, small
     * and fullwidth plus/minus signs.
     *
     * @param c Character to test
     * @param previous Character immediately preceding c
     * @return true if c is a sign immediately following ⏨
     */
    [[nodiscard]] inline static constexpr bool
    is_decimal_exponent_sign(const CharT c, const CharT previous) noexcept {
      return (previous == 0x23E8 && // U+23E8 ⏨ DECIMAL EXPONENT SYMBOL
              (c == CharT('+') ||   // U+002B + PLUS SIGN
               c == CharT('-') ||   // U+002D - HYPHEN-MINUS
               c == 0x2212 ||       // U+2212 − MINUS SIGN

               c == 0x208A || // U+208A ₊ SUBSCRIPT PLUS SIGN
               c == 0x208B || // U+208B ₋ SUBSCRIPT MINUS SIGN

               c == 0x207A || // U+207A ⁺ SUPERSCRIPT PLUS SIGN
               c == 0x207B || // U+207B ⁻ SUPERSCRIPT MINUS SIGN

               c == 0xFE62 || // U+FE62 ﹢ SMALL PLUS SIGN
               c == 0xFE63 || // U+FE63 ﹣ SMALL HYPHEN-MINUS

               c == 0xFF0B || // U+FF0B ＋ FULLWIDTH PLUS SIGN
               c == 0xFF0D    // U+FF0D － FULLWIDTH HYPHEN-MINUS
               ));
    }
#if 0
    /**
     * @brief Classifies decimal exponent digit characters.
     *
     * Recognizes:
     *   - ASCII digits 0-9
     *   - Unicode subscript digits ₀-₉
     *   - Unicode superscript digits ⁰⁴⁵⁶⁷⁸⁹
     *   - Unicode fullwidth digits ０-９
     *
     * @param c Character to test
     * @return true if c is a valid decimal exponent digit following ⏨
     */
    [[nodiscard]] inline static constexpr bool
    is_decimal_exponent_digit(const CharT c) noexcept {
      return (
          c >= CharT('0') && c <= CharT('9') ||
          c >= 0x2080 && c <= 0x2089 || // U+2080-U+2089 ₀-₉ SUBSCRIPT DIGITS
          c == 0x2070 ||                // U+2070 ⁰ SUPERSCRIPT ZERO
          c >= 0x2074 && c <= 0x2079 || // U+2074-U+2079 ⁴-⁹ SUPERSCRIPT DIGITS
          c == 0x00B2 ||                // U+00B2 ² SUPERSCRIPT TWO
          c == 0x00B3 ||                // U+00B3 ³ SUPERSCRIPT THREE
          c == 0x00B9 ||                // U+00B9 ¹ SUPERSCRIPT ONE
          c >= 0xFF10 && c <= 0xFF19    // U+FF10-U+FF19 ０-９ FULLWIDTH DIGITS
      );
    }
#endif

    /**
     * @brief Checks whether the current character immediately follows
     *        a decimal exponent marker or its sign.
     *
     * @param input Current input position
     * @param begin Beginning of input
     * @return true if the current position continues a decimal exponent prefix
     */
    [[nodiscard]] inline static constexpr bool
    is_decimal_exponent_continuation(const CharT *const input,
                                     const CharT *const begin) noexcept {
      return input[-1] == CharT(0x23E8) || // U+23E8 ⏨ DECIMAL EXPONENT SYMBOL
             (input - begin >= 2 &&
              is_decimal_exponent_sign(input[-1], input[-2]));
    }

    /**
     * @brief Normalization functor - the main processing engine.
     *
     * Processes the entire input string character-by-character, performing
     * Unicode-to-ASCII conversions and state machine management. This is
     * the primary normalization algorithm.
     *
     * **Algorithm Overview:**
     * 1. Try fast path for printable ASCII as fast as possible.
     * 2. Initialize superscript and implicit-argument state.
     * 3. Iterate through each UTF-16 code unit:
     *    a. Close active state when the current character marks a boundary.
     *    b. Pass through printable ASCII unchanged.
     *    c. Normalize separators, fullwidth forms, and subscripts.
     *    d. Dispatch special symbols through the switch statement.
     *    e. Stop at the first unsupported code unit.
     * 4. Finalize by closing any still-active superscript/argument state.
     *
     * **State Machines:**
     * - **Initial-superscript mode (bool only_superscript):**
     *   Keeps a leading superscript-only input unwrapped. It is cleared as soon
     *   as a non-superscript code unit is encountered.
     *
     * - **Superscript mode (bool in_superscript):**
     *   Activated by superscripts after ordinary input, generating a "^(...)"
     *   wrapper. Closed when a non-superscript character is encountered.
     *
     * - **Superscript parenthesis tracking (bool close_superscript):**
     *   Tracks ownership of an automatically generated opening parenthesis.
     *   Explicit superscript parentheses do not require an additional wrapper.
     *
     * - **Implicit argument mode (Length in_implicit_argument):**
     *   Counts implicit argument parentheses opened by sqrt, etc or
     *   parenthesized the decimal-exponent ⏨ after a ')'. It is decremented
     *   when a separator, operator, or parenthesis ends the implicit argument.
     *
     * **Performance Characteristics:**
     * - Single-pass algorithm: O(n) where n = input length
     * - No backtracking or lookahead (except ±1 check for implicit argument and
     *   superscript)
     * - Direct character-by-character output: minimal memory operations
     * - Uses switch statement (compiler jump table vs if-chain)
     * - memcpy for multi-character replacements
     *
     * @param buffer Output buffer for normalized ASCII string
     * @return Length of normalized output in bytes
     *
     * @note This function signature matches
     *       std::string::resize_and_overwrite's operation callback
     * @note The 'normalized' is set to the final count after input processing
     * @note Returns the count chars in the output buffer
     *
     * @note Processing stops at the first unsupported UTF-16 code unit and
     *       returns the successfully normalized prefix.
     */
    [[nodiscard]] inline auto operator()(char *__restrict buffer, size_t) {
      auto input = _begin;

      auto output = buffer;

      // Fast path for printable ASCII.
      for (; input != _end; ++input) [[likely]] {
        const auto &c = *input;
        if (is_printable_ascii(c)) [[likely]] {
          *output++ = static_cast<char>(c);
        } else {
          break;
        }
      }
      //
      if (input != _end) [[unlikely]] {
        // Tracks whether the input starts with a superscript sequence
        bool only_superscript = false;

        // Tracks whether we are processing a superscript sequence
        bool in_superscript = false;

        // Tracks superscript automatic parenthesis
        bool close_superscript = false;

        // Tracks nesting depth of implicit argument parentheses
        Length in_implicit_argument = 0;

        // Main processing loop - processes all input characters
        for (; input != _end; ++input) [[likely]] {
          const auto &c = input[0];

          // Exit leading superscript-only mode when encountering ordinary input
          if (only_superscript && !is_superscript(c)) [[unlikely]] {
            [[unlikely]] goto fast_processing_end;
          }

          // Exit superscript mode when encountering a non-superscript
          // character.
          if (in_superscript && !is_superscript(c)) [[unlikely]] {
            in_superscript = false;

            if (close_superscript) {
              *output++ = ')';
              close_superscript = false;
            }
          }

          // Exit implicit argument mode when encountering argument boundary
          if (in_implicit_argument && is_implicit_argument_end(c) &&
              !is_decimal_exponent_sign(c, input[-1]) &&
              !is_unary_sign_after_implicit_function(c, input[-1]))
              [[unlikely]] {
            do {
              --in_implicit_argument;
              *output++ = ')';
            } while (in_implicit_argument);
          }

          // Pass through printable ASCII characters unchanged
          if (is_printable_ascii(c)) [[likely]] {
            *output++ = static_cast<char>(c);
            continue;
          }

          // Replace all separators with ASCII space
          if (is_separator(c)) {
            *output++ = ' ';
            continue;
          }

          // Map subscript digits (₀-₉) directly to ASCII digits (0-9)
          if (c >= 0x2080 && c <= 0x2089) {
            *output++ = static_cast<char>(c - 0x2050);
            continue;
          }

          // Map subscript letters (ₐₑₒₓₔₕₖₗₘₙₚₛₜ) via lookup table
          if (c >= 0x2090 && c <= 0x209C &&
              c != 0x2094 /* ₔ LATIN SUBSCRIPT SMALL LETTER SCHWA */) {
            *output++ = "aeox?hklmnpst"[c - 0x2090];
            continue;
          }

          // Map fullwidth forms directly to ASCII
          if (c >= 0xFF01 && c <= 0xFF5E) {
            *output++ = static_cast<char>(c - 0xFEE0);
            continue;
          }

          // Dispatch to appropriate handler for other characters
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
          case 0xF7:   // ÷ ANSI DIVISION SIGN
          case 0x2044: // ⁄ FRACTION SLASH
          case 0x2215: // ∕ DIVISION SLASH
          case 0x29F8: // ⧸ BIG SOLIDUS
            *output++ = '/';
            break;

          // Multiplication sign variants
          case 0xB7:   // · ANSI MIDDLE DOT
          case 0xD7:   // × ANSI MULTIPLICATION SIGN
          case 0x204E: // ⁎ LOW ASTERISK
          case 0x2217: // ∗ ASTERISK OPERATOR
          case 0x22C5: // ⋅ DOT OPERATOR
          case 0xFE61: // ﹡ SMALL ASTERISK
            *output++ = '*';
            break;
#ifdef CALC_USED_EQUALS_OPERATORS
          // TODO
          // https://www.fileformat.info/info/unicode/category/Sm/list.htm
          // Relational Operators
          case 0x2260: // ≠
            output = append(output, "!=");
            break;

          case 0x2261: // ≡
            output = append(output, "==");
            break;

          case 0x2264: // ≤
            output = append(output, "<=");
            break;

          case 0x2265: // ≥
            output = append(output, ">=");
            break;

          case 0x2243: // ≃
          case 0x2245: // ≅
          case 0x2248: // ≈
            output = append(output, "~=");
            break;
#endif
          // Fractions - ANSI
          case 0xBC: // ¼
            output = append(output, "1/4");
            break;

          case 0xBD: // ½
            output = append(output, "1/2");
            break;

          case 0xBE: // ¾
            output = append(output, "3/4");
            break;

          // Fractions - Unicode
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

          case 0xB5:   // µ ANSI MICRO SIGN
          case 0x03BC: // μ GREEK SMALL LETTER MU
            output = append(output, "mu");
            break;

          case 0x03C3: // σ GREEK SMALL LETTER SIGMA
            output = append(output, "sigma");
            break;

          case 0x03D0: // ϐ GREEK BETA SYMBOL
          case 0x03B2: // β GREEK SMALL LETTER BETA
          case 0xA7B5: // β LATIN SMALL LETTER BETA
            output = append(output, "beta");
            break;

          case 0x221E: // ∞ INFINITY
            output = append(output, "inf");
            break;

          case 0x212F: // ℯ SCRIPT SMALL E
            *output++ = 'e';
            break;

          // Physical constants
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

#define START_IMPLICIT_ARGUMENT                                                \
  if (!start_implicit_argument(input, _end, output, in_implicit_argument)) {   \
    [[unlikely]] goto fast_processing_end;                                     \
  }

          // Special case
          case 0x23E8: // ⏨ DECIMAL EXPONENT SYMBOL
            if (_begin == input) {
              [[unlikely]] goto fast_processing_end;
            }
            output = append(output, "*10^");
            START_IMPLICIT_ARGUMENT
            break;
#ifdef CALC_USED_EQUALS_OPERATORS
            // TODO: do we need function to handle this?
          case 0xB1:   // ± PLUS-MINUS SIGN
          case 0x2213: // ∓ MINUS-OR-PLUS SIGN
            output = append(output, "PM");
            START_IMPLICIT_ARGUMENT
            break;
#endif
          // Unicode mathematical function symbols
          case 0x221A: // √ SQUARE ROOT
          case 0x23B7: // ⎷ RADICAL SYMBOL BOTTOM
            output = append(output, "sqrt");
            START_IMPLICIT_ARGUMENT
            break;

          case 0x221B: // ∛ CUBE ROOT
            output = append(output, "cbrt");
            START_IMPLICIT_ARGUMENT
            break;

          case 0x221C: // ∜ FOURTH ROOT
            output = append(output, "qdrt");
            START_IMPLICIT_ARGUMENT
            break;

          // CJK compatibility functions - Square symbol notations
          case 0x33D1: // ㏑ SQUARE LN
            output = append(output, "ln");
            START_IMPLICIT_ARGUMENT
            break;

          case 0x33D2: // ㏒ SQUARE LOG
            output = append(output, "log");
            START_IMPLICIT_ARGUMENT
            break;

#undef START_IMPLICIT_ARGUMENT

          // Astronomical symbols - Planets and celestial bodies
          case 0x2609: // ☉ SUN
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

          case 0x2641: // ♁ EARTH
            output = append(output, "_earth");
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
          case 0x26E2: // ⛢ ASTRONOMICAL SYMBOL FOR URANUS
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

            // Superscript - ANSI and Unicode
#define START_SUPERSCRIPT                                                      \
  if (!in_superscript) {                                                       \
    in_superscript = true;                                                     \
    close_superscript = false;                                                 \
    if (_begin == input) [[unlikely]] {                                        \
      only_superscript = true;                                                 \
    } else if (!is_decimal_exponent_continuation(input, _begin)) {             \
      *output++ = '^';                                                         \
      if (c != 0x207D /* ⁽ SUPERSCRIPT LEFT PARENTHESIS */) {                  \
        *output++ = '(';                                                       \
        close_superscript = true;                                              \
      }                                                                        \
    }                                                                          \
  }

          // Superscript ANSI digits
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
#ifdef CALC_TEST_EQUATION_SOLVER
          // Superscript Unicode letter
          case 0x2071: // ⁱ SUPERSCRIPT LATIN SMALL LETTER I
            START_SUPERSCRIPT
            *output++ = 'i';
            break;
#endif
          // case 0x2072: // not assigened.
          // case 0x2073: // not assigened.

          // Superscript Unicode digits
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

          // Superscript Unicode arithmetic operators
          case 0x207A: // ⁺ SUPERSCRIPT PLUS SIGN
            START_SUPERSCRIPT
            *output++ = '+';
            break;

          case 0x207B: // ⁻ SUPERSCRIPT MINUS SIGN
            START_SUPERSCRIPT
            *output++ = '-';
            break;
#ifdef CALC_USED_EQUALS_OPERATORS
          case 0x207C: // ⁼ SUPERSCRIPT EQUALS SIGN
            START_SUPERSCRIPT
            *output++ = '=';
            break;
#endif
          // Superscript Unicode parentheses. Should be in pairs
          case 0x207D: // ⁽ SUPERSCRIPT LEFT PARENTHESIS
            START_SUPERSCRIPT
            *output++ = '(';
            break;

          case 0x207E: // ⁾ SUPERSCRIPT RIGHT PARENTHESIS
            START_SUPERSCRIPT
            *output++ = ')';
            break;
#ifdef CALC_TEST_EQUATION_SOLVER
          // Superscript Unicode letter
          case 0x207F: // ⁿ SUPERSCRIPT LATIN SMALL LETTER N
            START_SUPERSCRIPT
            *output++ = 'n';
            break;
#endif

#undef START_SUPERSCRIPT

          // Unknown character - abort processing
          default:
            [[unlikely]] goto fast_processing_end;
          }
        }

        // Close any unclosed parentheses from active state machines

        if (close_superscript) [[unlikely]] {
          *output++ = ')';
        }
        while (in_implicit_argument) [[unlikely]] {
          --in_implicit_argument;
          *output++ = ')';
        }
      }

    fast_processing_end:
      _normalized = static_cast<Length>(input - _begin);
      return static_cast<size_t>(output - buffer);
    }
  };
};

} // namespace GUI

#endif

#endif
