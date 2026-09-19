/*
 * Copyright 2026-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef EQUASION_NORMALIZATON_TESTS_HPP
#define EQUASION_NORMALIZATON_TESTS_HPP

#ifdef CALC_SUPPORT_UNICODE_INPUT

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
 * @see gui_tests()
 */
static auto normalizer_tests = std::to_array< std::pair< std::string_view, std::pair< bool, std::string_view > > >({

    // Complex expressions
    { "½∜(7π÷2)", { false, "1/2qdrt(7pi/2)" }},
    { "½∜(7π÷3)R☉×c * (⅒+ ⅟₉)⏨₉  +ℯ⁶", { false ,"1/2qdrt(7pi/3)R_sun*c * (1/10+ 1/9)*10^(9)  +e^(6)" }},
    { "﹣₉﹒₀₉⏨₉", { false, "-9.09e9" }},
    { "12³×(231² - 1)³ + 744 - 0.00000000000075", { false, "12^(3)*(231^(2) - 1)^(3) + 744 - 0.00000000000075" }},
    
    {   // Separators
        "\t\n\v\f\r \xC2\xA0 \xE3\x80\x80 \xE2\x80\x80 \xE2\x80\x81 \xE2\x80\xAE \xE2\x80\xAF"
        // Fullwidth forms
        "ＡＢＣＸＹＺａｂｃｘｙｚ０１２３４５６７８９＋－＊／（）＝＜＞＠＃％＆"
        "！＂＃＄％＆＇（）＊＋，－．／０１２３４５６７８９：；＜＝＞？＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～"
        "！＂＃＄％＆＇（）＊＋，－．／"
        "０１２３４５６７８９：；＜＝＞？＠"
        "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ"
        "ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ"
        "［＼］＾＿｀｛｜｝～"
        // Operators, parentheses and punctuation
        "₊﹢₋−﹣⁎∗⋅﹡⁄∕⧸·×÷()₍₎﹙﹚﹐﹒+-*/"
        // Fractions
        "¼½¾⅐⅑⅒⅓⅔⅕⅖⅗⅘⅙⅚⅛⅜⅝⅞⅟↉3⅟₉" // Does we need additional logic here? This logic can be apply or not?

        // ASCII pass-through and separator boundaries needs to be converted to the ASCII space
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" 
        "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~" 
        "  abc\tdef\nxyz "
        ,
    { false,
        // Separators
        "                 "
        // Fullwidth forms
        "ABCXYZabcxyz0123456789+-*/()=<>@#%&"
        "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
        "!\"#$%&'()*+,-./"
        "0123456789:;<=>?@"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "[\\]^_`{|}~"
        // Operators, parentheses and punctuation
        "++---****///**/()()(),.+-*/"
        // Fractions
        "1/41/23/41/71/91/101/32/31/52/53/54/51/65/61/83/85/87/81/0/331/9" // Does we need additional logic here? This logic can be apply or not?

        // ASCII pass-through and separator boundaries needs to be converted to the ASCII space
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
        "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
        "  abc def xyz "
    }
    },

    // One maximal normalization stress test.
    //
    // Covers:
    //   - printable ASCII fast path
    //   - every supported separator U+0009..U+000D, U+0020, U+00A0,
    //     U+2000..U+202F and U+3000
    //   - every supported fullwidth form U+FF01..U+FF5E
    //   - every supported operator mapping
    //   - optional relational/equality mappings
    //   - every supported vulgar fraction
    //   - every supported Greek / mathematical / physical symbol
    //   - every supported astronomical symbol and unit
    //   - every supported subscript digit, letter, operator and parenthesis
    //   - every supported superscript digit, letter, operator and parenthesis
    //   - implicit and explicit function arguments
    //   - nested implicit functions
    //   - function boundaries on whitespace, operators and parentheses
    //   - mapped characters which are NOT function boundaries
    //   - decimal exponent state and exponent/superscript interaction
    //   - superscript grouping and termination
    //   - function + superscript simultaneous state handling
    //   - unsupported character termination after state cleanup
    //
    // NOTE:
    //     This intentionally ends with U+2200 (∀). It is classified as an
    //     operator by is_operator(), but has no normalization mapping, so
    //     normalization must stop there with failed() == true.
    //
    // NOTE:
    //     The standalone "²" test above remains necessary for
    //     only_superscript, because that state is entered only at input[0].
    {   // Maximal Normalizer stress test
        // Printable ASCII: exercises the initial fast path.
        " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
        
        // Every separator classified by is_separator().
        "\t\n\v\f\r \xC2\xA0"
        "\xE2\x80\x80\xE2\x80\x81\xE2\x80\x82\xE2\x80\x83"
        "\xE2\x80\x84\xE2\x80\x85\xE2\x80\x86\xE2\x80\x87"
        "\xE2\x80\x88\xE2\x80\x89\xE2\x80\x8A\xE2\x80\x8B"
        "\xE2\x80\x8C\xE2\x80\x8D\xE2\x80\x8E\xE2\x80\x8F"
        "\xE2\x80\x90\xE2\x80\x91\xE2\x80\x92\xE2\x80\x93"
        "\xE2\x80\x94\xE2\x80\x95\xE2\x80\x96\xE2\x80\x97"
        "\xE2\x80\x98\xE2\x80\x99\xE2\x80\x9A\xE2\x80\x9B"
        "\xE2\x80\x9C\xE2\x80\x9D\xE2\x80\x9E\xE2\x80\x9F"
        "\xE2\x80\xA0\xE2\x80\xA1\xE2\x80\xA2\xE2\x80\xA3"
        "\xE2\x80\xA4\xE2\x80\xA5\xE2\x80\xA6\xE2\x80\xA7"
        "\xE2\x80\xA8\xE2\x80\xA9\xE2\x80\xAA\xE2\x80\xAB"
        "\xE2\x80\xAC\xE2\x80\xAD\xE2\x80\xAE\xE2\x80\xAF"
        "\xE3\x80\x80"

        // Every fullwidth character U+FF01..U+FF5E.
        "！＂＃＄％＆＇（）＊＋，－．／０１２３４５６７８９：；"
        "＜＝＞？＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ"
        "［＼］＾＿｀ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ"
        "｛｜｝～"

        // Every direct operator / punctuation mapping.
        "₊﹢₋−﹣⁎∗⋅﹡⁄∕⧸·×÷()₍₎﹙﹚﹐﹒+-*/"

#ifdef CALC_USED_EQUALS_OPERATORS
        // Every mapping enabled by CALC_USED_EQUALS_OPERATORS.
        "≠ ≤ ≥ ≡ ≈ ≃ ≅ "
#endif

        // Every supported vulgar fraction.
        "¼½¾⅐⅑⅒⅓⅔⅕⅖⅗⅘⅙⅚⅛⅜⅝⅞⅟↉"

        // Every mathematical / Greek / physical mapping.
        "πτφϕⅇΓγαζμµσϐ∞ℯƐℇℎℏε"

        // Every astronomical mapping and astronomical unit.
        "⊕♁⊙☉☼☽☾☿♀♂♃♄♅♆♇㍳㍶"

        // Every supported subscript mapping.
        "₀₁₂₃₄₅₆₇₈₉ₐₑₒₓₕₖₗₘₙₚₛₜ₊₋₍₎"

        // Functions:
        //   explicit argument
        //   implicit argument
        //   operators terminating an implicit argument
        //   nested implicit functions
        //   nested roots
        //   explicit parentheses
        "√(a) ∛(b) ∜(c) ㏑(d) ㏒(e) "
        "√x+∛y,∜z*㏑q/㏒r "
        "√∛x+1 "
        "√√x "
        "√x²+1 "
        "√(x) "

        // Superscript grouping:
        // every supported superscript digit, letter, operator and parenthesis.
        "x⁰¹²³⁴⁵⁶⁷⁸⁹ⁱⁿ⁺⁻⁼⁽⁾ "
        "x²³¹ⁱⁿ⁺⁻⁼⁽⁾ "
        "²³¹⁰⁴⁵⁶⁷⁸⁹ⁱⁿ⁺⁻⁼⁽⁾ "
        "x²+1 x³) xⁱ xⁿ x⁺ x⁻ x⁼ "
        "x⁽⁾ x⁽ⁿ⁾ "

        // Decimal exponent notation:
        // normal exponent, signed exponent, subscript/superscript exponent,
        // implicit function after a closing ')', and explicit '(...)'.
        "1⏨3 1⏨−3 ⏨₉ 2⏨³ "
        "(2)⏨3 (2)⏨(3) (2)⏨−3 "

        // Function-end boundaries using ASCII separators.
        "√x\t∛y\n∜z\v㏑q\f㏒r\r "

        // Function-end boundaries using Unicode separators.
        "√x\xC2\xA0∛y"
        "\xE2\x80\x80∜z"
        "\xE2\x80\xA8㏑q"
        "\xE2\x80\xAF㏒r"
        "\xE3\x80\x80 "

        // Function-end boundaries using operators.
        "√x+∛y-∜z*㏑q/㏒r,"
        "√a·∛b×∜c÷㏑d∕㏒e "

        // Function-end boundaries using every supported parenthesis spelling.
        "√x(∛y)√z₍∜q₎√r﹙㏑s﹚ "

        // Characters that are mapped to operators/punctuation but are NOT
        // recognized by is_operator()/is_function_end() before mapping.
        // The following boundary then forces the implicit function to close.
        "√x₊+∛y₋-∜z﹢+㏑q﹣-㏒r"
        "⁎+√a⁄+∛b∕+∜c⧸+㏑d﹡+㏒e﹐+√f﹒+ "

        // More combined state transitions.
        "√(1−½)² "
        "√(⅓π+⅔π) "
        "㏑(1)+㏒(10) "
        "R☉×c "
        "α+β−γ×μ÷σ "

        // Final unsupported U+2200:
        //
        //   √  -> in_function = 1, output "sqrt("
        //   x
        //   ²  -> in_superscript = true, output "^(2)"
        //   ∀  -> closes superscript
        //          closes function because ∀ is_operator()
        //          aborts at the unsupported character
        //
        // Therefore the normalized prefix must be:
        //     "sqrt(x^(2))"
        "√x²∀",

        { true,

          // ASCII
          " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

          // 56 separators -> 56 ASCII spaces.
          "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
          "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
          "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
          "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"

          // Fullwidth.
          "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

          // Operators.
          "++---****///**/()()(),.+-*/"

#ifdef CALC_USED_EQUALS_OPERATORS
          "!= <= >= == ~= ~= ~= "
#endif

          // Fractions.
          "1/41/23/41/71/91/101/32/31/52/53/54/51/65/61/83/85/87/81/0/3"

          // Mathematical / Greek / physical.
          "pi(2*pi)phiphie_atomicgammagammaalphariemann_zetamumusigmabetainfeEEhhbarepsilon"

          // Astronomy.
          "_earth_earth_sun_sun_sun_moon_moon_mercury_venus_mars_jupiter_saturn_uranus_neptune_pluto*au*pc"

          // Subscripts.
          "0123456789aeoxhklmnpst+-()"

          // Functions.
          "sqrt(a) cbrt(b) qdrt(c) ln(d) log(e) "
          "sqrt(x)+cbrt(y),qdrt(z)*ln(q)/log(r) "
          "sqrt(cbrt(x)+1) "
          "sqrt(sqrt(x) "
          "sqrt(x^(2))+1) "
          "sqrt(x) "

          // Superscripts.
          "x^(0123456789in+-=()) "
          "x^(231in+-=()) "
          "^(2310456789in+-=()) "
          "x^(2)+1 x^(3)) x^(i) x^(n) x^(+) x^(-) x^(=) "
          "x^(()) x^((n)) "

          // Decimal exponent.
          "1e3 1e-3 e9 2e3 "
          "(2)*10^(3) (2)*10^(3) (2)*10^()-3 "

          // Boundaries.
          "sqrt(x) cbrt(y) qdrt(z) ln(q) log(r)  "
          "sqrt(x) cbrt(y) qdrt(z) ln(q) log(r)  "
          "sqrt(x)+cbrt(y)-qdrt(z)*ln(q)/log(r),"
          "sqrt(a)*cbrt(b)*qdrt(c)/ln(d)/log(e) "
          "sqrt(x)(cbrt(y))sqrt(z)(qdrt(q))sqrt(r)(ln(s)) "
          "sqrt(x+)+cbrt(y-)-qdrt(z+)+ln(q-)-log(r*)"
          "+sqrt(a/)+cbrt(b)/+qdrt(c/)+ln(d*)+log(e,)+sqrt(f.)+ "

          // Combinations.
          "sqrt(1-1/2)^(2) "
          "sqrt(1/3pi+2/3pi) "
          "ln(1)+log(10) "
          "R_sun*c "
          "alpha+beta-gamma*mu/sigma "

          // Unsupported U+2200 terminates here.
          "sqrt(x^(2))"
        }
    },

    // Mathematical and phisical constants and Greek letters. Last symbol Ƶ is intentionally unsupported after "ℯℇ".
    { "ƐπℇτℎℏφϕⅇΓαζμµσϐ∞ℯΓγαζμµσεβϐ∞Γαζμµσεϐ∞ℯℇƵ", { true, "EpiE(2*pi)hhbarphiphie_atomicgammaalphariemann_zetamumusigmabetainfegammagammaalphariemann_zetamumusigmaepsilonbetabetainfgammaalphariemann_zetamumusigmaepsilonbetainfeE" }},

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

    // Superscripts: individual characters
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
    
    // Superscript grouping/state machine
    { "x²³", { false, "x^(23)" }},
    { "x¹²³⁴⁵⁶⁷⁸⁹", { false, "x^(123456789)" }},
    { "xⁱⁿ", { false, "x^(in)" }},
    { "x²⁺⁻⁼", { false, "x^(2+-=)" }},
    { "x²+1", { false, "x^(2)+1" }},
    { "x²)", { false, "x^(2))" }},
    { "x² y", { false, "x^(2) y" }},
    // Only superscript mode
    { "²", { false, "2" }},
    { "ⁿ", { false, "n" }},
    { "¹²³⁴⁵⁶⁷⁸⁹⁰", { false, "1234567890" }},
    // Superscript parentheses
    { "⁽", { false, "(" }},
    { "⁾", { false, ")" }},
    { "⁽⁾", { false, "()" }},
    { "x⁽⁾", { false, "x^(())" }},
    { "x⁽ⁿ⁾", { false, "x^((n))" }},
    { "¹²³⁴⁵⁶⁷⁸⁹⁰ ¹²³⁴⁵⁶⁷⁸⁹⁰", { false, "1234567890 ^(1234567890)" }}, // only_superscript -> separator -> in_superscript
    
    // Combinations of normalization features
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

    // Unsupported characters: normalization must stop at the first unsupported UTF-16 code unit
    { "A😀B", { true, "A" }},
    { "abc©def", { true, "abc" }},
    { "abcₔdef", { true, "abc" }},
    { "abc⁲def", { true, "abc" }},
    { "|—J│Оa╓Чn", { true, "| J" }},
    { "±", { true, "" }},
    { "x²😀", { true, "x^(2)" }},
    { "ₔ", { true, "" }},  // U+2094 is explicitly excluded from the subscript-letter lookup
    { "√😀", { true, "sqrt(" }}, // Should be fast exit without correctnes of states machines
    
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

} // namespace GUI
#endif
#endif
