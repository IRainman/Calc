/*
 * Copyright 2026-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef EQUASION_NORMALIZATON_TESTS_HPP
#define EQUASION_NORMALIZATON_TESTS_HPP

#if defined(CALC_SUPPORT_UNICODE_INPUT) && defined(CALC_TESTS_ENABLED)

namespace GUI {

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
 * @see Normalizer, gui_tests()
 */
constinit static auto normalizer_tests = std::to_array< std::pair< std::string_view, std::pair< bool, std::string_view > > >({
    // Complex expressions
    { "½∜(7π÷2)", { false, "1/2qdrt(7pi/2)" }},
    { "½∜(7π÷3)R☉×c * (⅒+⅟₉)⏨₉  +ℯ⁶", { false ,"1/2qdrt(7pi/3)R_sun*c * (1/10+1/9)*10^(9)  +e^(6)" }},
    { "⅜∛176 + R☉(7π÷3⏨⁵)c * (⅟₁₇₉ + ⅞)⏨₇ + R⛢(7π÷3⏨⁵)c", { false, "3/8cbrt(176) + R_sun(7pi/3*10^(5))c * (1/179 + 7/8)*10^(7) + R_uranus(7pi/3*10^(5))c" }},
    { "﹣₉﹒₀₉⏨₉", { false, "-9.09*10^(9)" }},
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
        "  abc\tdef\nxyz ",
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

    // Mathematical and physical constants and Greek letters. Last symbol Ƶ is intentionally unsupported after "ℯ ℇ "
    { "Ɛ π ℇ τ ℎ ℏ φ ϕ ⅇ Γ α ζ μ µ σ ϐ ∞ ℯ Γ γ α ζ μ µ σ ε β ϐ ∞ Γ α ζ μ µ σ ε ϐ ∞ ℯ ℇ Ƶ 12354567896541587463", { true, "E pi E (2*pi) h hbar phi phi e_atomic gamma alpha riemann_zeta mu mu sigma beta inf e gamma gamma alpha riemann_zeta mu mu sigma epsilon beta beta inf gamma alpha riemann_zeta mu mu sigma epsilon beta inf e E " }},

    // Mathematical functions
    {"√x", {false, "sqrt(x)"}},
    {"∛x", {false, "cbrt(x)"}},
    {"∜x", {false, "qdrt(x)"}},
    {"√(x)", {false, "sqrt(x)"}},
    {"∛(x)", {false, "cbrt(x)"}},
    {"∜(x)", {false, "qdrt(x)"}},
    {"√x+1", {false, "sqrt(x)+1"}},
    {"∛x−1", {false, "cbrt(x)-1"}},
    {"∜x×2", {false, "qdrt(x)*2"}},
    {"√x y", {false, "sqrt(x) y"}},
    {"√x,y", {false, "sqrt(x),y"}},
    {"⎷x", {false, "sqrt(x)"}},
    {"√√x", {false, "sqrt(sqrt(x))"}},
    {"√∛x", {false, "sqrt(cbrt(x))"}},
    {"∛∜x", {false, "cbrt(qdrt(x))"}},
    {"㏑x", {false, "ln(x)"}},
    {"㏒x", {false, "log(x)"}},
    {"㏑x+㏒y", {false, "ln(x)+log(y)"}},
    {"√x²", {false, "sqrt(x^(2))"}},

    // Decimal exponent notation
    {"⏨", {true, ""}},
    {"⏨+3", {true, ""}},
    {"⏨³⁴", {true, ""}},
    {"1⏨3", {false, "1*10^(3)"}},
    {"1⏨+3", {false, "1*10^(+3)"}},
    {"1⏨−3", {false, "1*10^(-3)"}},
    {"1⏨₉", {false, "1*10^(9)"}},
    {"1⏨³", {false, "1*10^(3)"}},
    {"2⏨³", {false, "2*10^(3)"}},
    {"(2)⏨3", {false, "(2)*10^(3)"}},
    {"(2)⏨(3)", {false, "(2)*10^(3)"}},
    {"(2)⏨−3", {false, "(2)*10^(-3)"}},
    {"(2)⏨+3", {false, "(2)*10^(+3)"}},
    {"(2)⏨₋₃", {false, "(2)*10^(-3)"}},
    {"2⏨−3", {false, "2*10^(-3)"}},
    {"2⏨³⁴", {false, "2*10^(34)"}},
    {"2⏨3⁴", {false, "2*10^(3^(4))"}},
    {"(2)⏨³⁴", {false, "(2)*10^(34)"}},
    {"2⏨−³", {false, "2*10^(-3)"}},
    {"x²³+1", {false, "x^(23)+1"}},
    {"x²³y⁴⁵", {false, "x^(23)y^(45)"}},
    {"²x³", {true, "2"}},
    { "2⏨⁻³",    { false, "2*10^(-3)" }},
    { "2⏨₋³",    { false, "2*10^(-3)" }},
    { "2⏨﹣³",   { false, "2*10^(-3)" }},
    { "2⏨＋³",   { false, "2*10^(+3)" }},

    // Astronomical symbols
    {"♁", {false, "_earth"}},
    {"☉", {false, "_sun"}},
    {"☽☾", {false, "_moon_moon"}},
    {"☿", {false, "_mercury"}},
    {"♀", {false, "_venus"}},
    {"♂", {false, "_mars"}},
    {"♃", {false, "_jupiter"}},
    {"♄", {false, "_saturn"}},
    {"♅⛢", {false, "_uranus_uranus"}},
    {"♆", {false, "_neptune"}},
    {"♇", {false, "_pluto"}},
    {"㍳", {false, "*au"}},
    {"㍶", {false, "*pc"}},
    {"R☉×c", {false, "R_sun*c"}},

    // Subscript digits and letters
    {"₀₁₂₃₄₅₆₇₈₉", {false, "0123456789"}},
    {"ₐₑₒₓₕₖₗₘₙₚₛₜ", {false, "aeoxhklmnpst"}},
    {"x₂₃₄₅₆₇₈₉", {false, "x23456789"}},
    {"₊₋₍₎", {false, "+-()"}},

    // Superscripts: individual characters
    {"x⁰", {false, "x^(0)"}},
    {"x¹", {false, "x^(1)"}},
    {"x²", {false, "x^(2)"}},
    {"x³", {false, "x^(3)"}},
    {"x⁴", {false, "x^(4)"}},
    {"x⁵", {false, "x^(5)"}},
    {"x⁶", {false, "x^(6)"}},
    {"x⁷", {false, "x^(7)"}},
    {"x⁸", {false, "x^(8)"}},
    {"x⁹", {false, "x^(9)"}},

#ifdef CALC_TEST_EQUATION_SOLVER
    {"xⁱ", {false, "x^(i)"}},
    {"xⁿ", {false, "x^(n)"}},
#endif

    {"x⁺", {false, "x^(+)"}},
    {"x⁻", {false, "x^(-)"}},

#ifdef CALC_USED_EQUALS_OPERATORS
    {"x⁼", {false, "x^(=)"}},
#endif

    // Superscript grouping.
    {"x²³", {false, "x^(23)"}},
    {"x¹²³⁴⁵⁶⁷⁸⁹", {false, "x^(123456789)"}},

#ifdef CALC_TEST_EQUATION_SOLVER
    {"xⁱⁿ", {false, "x^(in)"}},
#endif

#ifdef CALC_USED_EQUALS_OPERATORS
    {"x²⁺⁻⁼", {false, "x^(2+-=)"}},
#endif

    {"x²+1", {false, "x^(2)+1"}},
    {"x²)", {false, "x^(2))"}},
    {"x² y", {false, "x^(2) y"}},
    {"x²y³", {false, "x^(2)y^(3)"}},
    {"x²³y⁴⁵", {false, "x^(23)y^(45)"}},

    // Explicit superscript parentheses.
    {"x⁽⁾", {false, "x^()"}},
    {"x⁽²⁾", {false, "x^(2)"}},
    {"x²⁽⁾", {false, "x^(2())"}},

#ifdef CALC_TEST_EQUATION_SOLVER
    {"x⁽ⁿ⁾", {false, "x^(n)"}},
    {"x²⁽ⁿ⁾", {false, "x^(2(n))"}},
#endif

    // Leading superscript sequences.
    {"²", {false, "2"}},
    {"²³", {false, "23"}},
    {"¹²³⁴⁵⁶⁷⁸⁹⁰", {false, "1234567890"}},
    {"²x³", {true, "2"}},
    {"² ³", {true, "2"}},
    {"¹²³⁴⁵⁶⁷⁸⁹⁰ ¹²³⁴⁵⁶⁷⁸⁹⁰", {true, "1234567890"}},

    // Superscript-only input followed by an unsupported superscript
    {"²⁲", {true, "2"}},

    // Unary signs at the start of implicit function arguments
    {"√−1", {false, "sqrt(-1)"}},
    {"√-1", {false, "sqrt(-1)"}},
    {"√+1", {false, "sqrt(+1)"}},
    {"√−1+2", {false, "sqrt(-1)+2"}},
    {"√−1-2", {false, "sqrt(-1)-2"}},
    {"∛−8", {false, "cbrt(-8)"}},
    {"√−(1)", {false, "sqrt(-)(1)"}}, // it's ok

    // Decimal exponent signs
    {"1⏨-1", {false, "1*10^(-1)"}},
    {"1⏨+1", {false, "1*10^(+1)"}},
    {"1⏨−1", {false, "1*10^(-1)"}},

    // Implicit argument at end-of-input
    {"√", {true, "sqrt"}},
    {"1⏨", {true, "1*10^"}},
    {"∛", {true, "cbrt"}},
    {"∜", {true, "qdrt"}},
    {"㏑", {true, "ln"}},
    {"㏒", {true, "log"}},

    // Nested implicit arguments and boundaries
    {"√√x+1", {false, "sqrt(sqrt(x))+1"}},
    {"√√x y", {false, "sqrt(sqrt(x)) y"}},
    {"√∛x−1", {false, "sqrt(cbrt(x))-1"}},
    {"√√x,y", {false, "sqrt(sqrt(x)),y"}},

    // Combinations of normalization features
    {"½π²", {false, "1/2pi^(2)"}},
    {"(½π)²", {false, "(1/2pi)^(2)"}},
    {"α+β−γ×μ÷σ", {false, "alpha+beta-gamma*mu/sigma"}},
    {"√(1−½)²", {false, "sqrt(1-1/2)^(2)"}},
    {"⅓×π+⅔×π", {false, "1/3*pi+2/3*pi"}},
    {"⅓π+⅔π", {false, "1/3pi+2/3pi"}},
    {"R☉=c×ℏ", {false, "R_sun=c*hbar"}},
    {"㏑(1)+㏒(10)", {false, "ln(1)+log(10)"}},
    {"１２３．４５６", {false, "123.456"}},
    {"﹣９﹒０９", {false, "-9.09"}},

    // Unsupported characters: normalization must stop at the first unsupported UTF-16 code unit
    {"A😀B", {true, "A"}},
    {"abc©def", {true, "abc"}},
    {"abcₔdef", {true, "abc"}}, // U+2094 is explicitly excluded from the subscript-letter lookup
    {"abc⁲def", {true, "abc"}},
    {"|—J│Оa╓Чn", {true, "|"}},
#ifndef CALC_USED_EQUALS_OPERATORS
    {"±", {true, ""}},
    {"x⁼", {true, "x"}},
    {"xⁱ", {true, "x"}},
    {"xⁿ", {true, "x"}},
#endif
    {"x²😀", {true, "x^(2)"}},
    {"ₔ", {true, ""}}, // U+2094 is explicitly excluded from the subscript-letter lookup
    {"√😀", {true, "sqrt("}}, // Should be fast exit without correctnes of state machines

    // TODO: user constants and variables support:
    // Also I ask ChatGPT https://chatgpt.com/c/6a89e39f-5610-83eb-8182-d914dc425042
    // Which syntax we should use?
    {"x=2π;", {false, "x=2pi;"}},
#ifdef CALC_USED_EQUALS_OPERATORS
    // Intervals needed:
    {"[0…π]", {false, "[0???pi]"}},             // How exactly?
    {"[0…π, step]", {false, "[0???pi, step]"}}, // How exactly?
#else
    {"[0…π]", {true, "[0"}},
    {"[0…π, step]", {true, "[0"}},
#endif
#ifdef CALC_USED_EQUALS_OPERATORS
    // blocks?
    {"{x= 150 ⁄ 12 + 1 }", {false, "{x= 150 / 12 + 1 }"}},
    // TODO: equation support, for input test https://xrjunque.nom.es/AllInOne:
    {"±", {false, "?"}}, // what exactly? I think we should use function, because it is more flexible and can be extended to other symbols
    {"{x=((−b ± √(b²−4⋅a⋅c))⁄(2a))π}", {false, "{x=((-b ? sqrt(b^(2)-4*a*c))/(2a))pi}"}},
#endif
});
// clang-format on

} // namespace GUI
#endif

#endif
