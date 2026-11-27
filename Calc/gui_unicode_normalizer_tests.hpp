/*
 * Copyright 2026-present Elle Solomina, a.rainman on gmail point com
 */

#ifndef EQUASION_NORMALIZATON_TESTS_HPP
#define EQUASION_NORMALIZATON_TESTS_HPP

#if defined(CALC_SUPPORT_UNICODE_INPUT) && defined(CALC_TESTS_ENABLED)

namespace GUI {

/**
 * @struct NormalizerTest
 * @brief Helper for GUI Unicode normalization tests.
 *
 * contains:
 *   - input: UTF-8 encoded source text.
 *   - output: expected normalized ASCII text.
 *   - failed: expected Normalizer::failed() result.
 *
 * @see std::string_view
 */
struct NormalizerTest {
  std::string_view input;
  std::string_view output;
  bool failed;
};

/**
 * Test cases for GUI Unicode normalization.
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
 * @see Normalizer, NormalizerTest, gui_tests(), std::to_array
 */
constinit static auto normalizer_tests = std::to_array<NormalizerTest>({

    {// ASCII and ANSI separators
     "\t\n\v\f\r \xC2\xA0", "       ", false},

    {// Unicode separators
     "\xE2\x80\x80\xE2\x80\x81\xE2\x80\x82\xE2\x80\x83\xE2\x80\x84"
     "\xE2\x80\x85\xE2\x80\x86\xE2\x80\x87\xE2\x80\x88\xE2\x80\x89"
     "\xE2\x80\x8A",
     "     "
     "     "
     " ",
     false},

    // clang-format off
    {// Fullwidth forms
     "ＡＢＣＸＹＺａｂｃｘｙｚ０１２３４５６７８９＋－＊／（）＝＜＞＠＃％＆"
     "！＂＃＄％＆＇（）＊＋，－．／０１２３４５６７８９：；＜＝＞？＠"
     "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ［＼］＾＿｀"
     "ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ｛｜｝～"
     "！＂＃＄％＆＇（）＊＋，－．／"
     "０１２３４５６７８９：；＜＝＞？＠"
     "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ"
     "ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ"
     "［＼］＾＿｀｛｜｝～",
     "ABCXYZabcxyz0123456789+-*/()=<>@#%&"
     "!\"#$%&'()*+,-./0123456789:;<=>?@"
     "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
     "abcdefghijklmnopqrstuvwxyz{|}~"
     "!\"#$%&'()*+,-./"
     "0123456789:;<=>?@"
     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
     "abcdefghijklmnopqrstuvwxyz"
     "[\\]^_`{|}~",
     false},
    // clang-format on

    {// Operators, parentheses and punctuation
     "₊﹢₋−﹣⁎∗⋅﹡⁄∕·×÷()₍₎﹙﹚﹐﹒+-*/", "++---****//**/()()(),.+-*/", false},
    {"2"
     "\xE2\x81\xA2" // U+2062
     "3",
     "2*3", false},
    {"2"
     "\xE2\x81\xA4" // U+2064
     "3",
     "2+3", false},
    {"√"
     "\xE2\x81\xA4"
     "1",
     "sqrt(+1)", false},
    {"1⏨"
     "\xE2\x81\xA4"
     "3",
     "1*10^(+3)", false},
    {"√x"
     "\xE2\x81\xA4"
     "2",
     "sqrt(x)+2", false},

    // Degree sign
    {"90°", "90*pi/180", false},
    {"45°+π", "45*pi/180+pi", false},
    {"√90°", "sqrt(90*pi/180)", false},

    {// Fractions
     "¼½¾⅐⅑⅒⅓⅔⅕⅖⅗⅘⅙⅚⅛⅜⅝⅞⅟↉3⅟₉",
     // Does we need additional logic here? This logic can be apply or not?
     "1/41/23/41/71/91/101/32/31/52/53/54/51/65/61/83/85/87/81/0/331/9", false},

    {// ASCII pass-through
     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
     "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
     // separator boundaries needs to be converted to theASCII space
     "  abc\tdef\nxyz ",
     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
     "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"
     "  abc def xyz ",
     false},

    {// Mathematical and physical constants and Greek letters.
     "Ɛ π ℇ τ ℎ ℏ φ ϕ ⅇ Γ α ζ μ µ "
     "σ ϐ ∞ ℯ Γ γ α ζ μ µ σ ε β "
     "ϐ ∞ Γ α ζ μ µ σ ε "
     // Last symbol Ƶ is intentionally unsupported after "ℯ ℇ "
     "ϐ ∞ ℯ ℇ Ƶ 12354567896541587463",

     "E pi E (2*pi) h hbar phi phi e_atomic gamma alpha riemann_zeta mu mu "
     "sigma beta inf e gamma gamma alpha riemann_zeta mu mu sigma epsilon beta "
     "beta inf gamma alpha riemann_zeta mu mu sigma epsilon beta inf e E ",
     true},

    // Mathematical functions
    {"√x", "sqrt(x)", false},
    {"∛x", "cbrt(x)", false},
    {"∜x", "qdrt(x)", false},
    {"√(x)", "sqrt(x)", false},
    {"∛(x)", "cbrt(x)", false},
    {"∜(x)", "qdrt(x)", false},
    {"√x+1", "sqrt(x)+1", false},
    {"∛x−1", "cbrt(x)-1", false},
    {"∜x×2", "qdrt(x)*2", false},
    {"√x y", "sqrt(x) y", false},
    {"√x,y", "sqrt(x),y", false},
    {"⎷x", "sqrt(x)", false},
    {"√√x", "sqrt(sqrt(x))", false},
    {"√∛x", "sqrt(cbrt(x))", false},
    {"∛∜x", "cbrt(qdrt(x))", false},
    {"㏑x", "ln(x)", false},
    {"㏒x", "log(x)", false},
    {"㏑x+㏒y", "ln(x)+log(y)", false},
    {"√x²", "sqrt(x^(2))", false},
    {"√√(x+1)", "sqrt(sqrt(x+1))", false},
    {"∛∜(x+1)", "cbrt(qdrt(x+1))", false},
    {"√√(x+1)+2", "sqrt(sqrt(x+1))+2", false},
    {"∛∜(x+1)-2", "cbrt(qdrt(x+1))-2", false},

    // Decimal exponent notation
    {"⏨", "", true},
    {"⏨+3", "", true},
    {"⏨³⁴", "", true},
    {"1⏨3", "1*10^(3)", false},
    {"1⏨+3", "1*10^(+3)", false},
    {"1⏨−3", "1*10^(-3)", false},
    {"1⏨₉", "1*10^(9)", false},
    {"1⏨³", "1*10^(3)", false},
    {"2⏨³", "2*10^(3)", false},
    {"(2)⏨3", "(2)*10^(3)", false},
    {"(2)⏨(3)", "(2)*10^(3)", false},
    {"(2)⏨−3", "(2)*10^(-3)", false},
    {"(2)⏨+3", "(2)*10^(+3)", false},
    {"(2)⏨₋₃", "(2)*10^(-3)", false},
    {"2⏨−3", "2*10^(-3)", false},
    {"2⏨³⁴", "2*10^(34)", false},
    {"2⏨3⁴", "2*10^(3^(4))", false},
    {"(2)⏨³⁴", "(2)*10^(34)", false},
    {"2⏨−³", "2*10^(-3)", false},
    {"x²³+1", "x^(23)+1", false},
    {"x²³y⁴⁵", "x^(23)y^(45)", false},
    {"²x³", "2", true},
    {"2⏨⁻³", "2*10^(-3)", false},
    {"2⏨₋³", "2*10^(-3)", false},
    {"2⏨﹣³", "2*10^(-3)", false},
    {"2⏨＋³", "2*10^(+3)", false},

    // Astronomical symbols
    {"♁", "_earth", false},
    {"☉", "_sun", false},
    {"☽☾", "_moon_moon", false},
    {"☿", "_mercury", false},
    {"♀", "_venus", false},
    {"♂", "_mars", false},
    {"♃", "_jupiter", false},
    {"♄", "_saturn", false},
    {"♅⛢", "_uranus_uranus", false},
    {"♆", "_neptune", false},
    {"♇", "_pluto", false},
    {"㍳", "*au", false},
    {"㍶", "*pc", false},
    {"R☉×c", "R_sun*c", false},

    // Subscript digits and letters
    {"₀₁₂₃₄₅₆₇₈₉", "0123456789", false},
    {"ₐₑₒₓₕₖₗₘₙₚₛₜ", "aeoxhklmnpst", false},
    {"x₂₃₄₅₆₇₈₉", "x23456789", false},
    {"₊₋₍₎", "+-()", false},

    // Superscripts: individual characters
    {"x⁰", "x^(0)", false},
    {"x¹", "x^(1)", false},
    {"x²", "x^(2)", false},
    {"x³", "x^(3)", false},
    {"x⁴", "x^(4)", false},
    {"x⁵", "x^(5)", false},
    {"x⁶", "x^(6)", false},
    {"x⁷", "x^(7)", false},
    {"x⁸", "x^(8)", false},
    {"x⁹", "x^(9)", false},

#ifdef CALC_TEST_EQUATION_SOLVER
    {"xⁱ", "x^(i)", false},
    {"xⁿ", "x^(n)", false},
#endif

    {"x⁺", "x^(+)", false},
    {"x⁻", "x^(-)", false},

#ifdef CALC_USED_EQUALS_OPERATORS
    {"x⁼", "x^(=)", false},
#else
    {"x⁼", "x", true},
#endif

    // Superscript grouping.
    {"x²³", "x^(23)", false},
    {"x¹²³⁴⁵⁶⁷⁸⁹", "x^(123456789)", false},

#ifdef CALC_TEST_EQUATION_SOLVER
    {"xⁱⁿ", "x^(in)", false},
#else
    {"xⁱ", "x", true},
    {"xⁿ", "x", true},
#endif

#ifdef CALC_USED_EQUALS_OPERATORS
    {"x²⁺⁻⁼", "x^(2+-=)", false},
#endif

    {"x²+1", "x^(2)+1", false},
    {"x²)", "x^(2))", false},
    {"x² y", "x^(2) y", false},
    {"x²y³", "x^(2)y^(3)", false},
    {"x²³y⁴⁵", "x^(23)y^(45)", false},

    // Explicit superscript parentheses.
    {"x⁽⁾", "x^()", false},
    {"x⁽²⁾", "x^(2)", false},
    {"x²⁽⁾", "x^(2())", false},
    {"x⁽²⁾+1", "x^(2)+1", false},

#ifdef CALC_TEST_EQUATION_SOLVER
    {"x⁽ⁿ⁾", "x^(n)", false},
    {"x²⁽ⁿ⁾", "x^(2(n))", false},
#endif

    // Leading superscript sequences.
    {"²", "2", false},
    {"²³", "23", false},
    {"¹²³⁴⁵⁶⁷⁸⁹⁰", "1234567890", false},
    {"²x³", "2", true},
    {"² ³", "2", true},
    {"¹²³⁴⁵⁶⁷⁸⁹⁰ ¹²³⁴⁵⁶⁷⁸⁹⁰", "1234567890", true},

    {"2∙3", "2*3", false},
    {"√x∙2", "sqrt(x)*2", false},

    // Unary signs at the start of implicit function arguments
    {"√−1", "sqrt(-1)", false},
    {"√-1", "sqrt(-1)", false},
    {"√+1", "sqrt(+1)", false},
    {"√−1+2", "sqrt(-1)+2", false},
    {"√−1-2", "sqrt(-1)-2", false},
    {"∛−8", "cbrt(-8)", false},
    {"√−(1)", "sqrt(-)(1)", false}, // it's ok
    {"√∞", "sqrt(inf)", false},
    {"√−∞", "sqrt(-inf)", false},
    {"∛∞", "cbrt(inf)", false},

    // Decimal exponent signs
    {"1⏨-1", "1*10^(-1)", false},
    {"1⏨+1", "1*10^(+1)", false},
    {"1⏨−1", "1*10^(-1)", false},

    // Implicit argument at end-of-input
    {"√", "sqrt", true},
    {"1⏨", "1*10^", true},
    {"∛", "cbrt", true},
    {"∜", "qdrt", true},
    {"㏑", "ln", true},
    {"㏒", "log", true},

    // Nested implicit arguments and boundaries
    {"√√x+1", "sqrt(sqrt(x))+1", false},
    {"√√x y", "sqrt(sqrt(x)) y", false},
    {"√∛x−1", "sqrt(cbrt(x))-1", false},
    {"√√x,y", "sqrt(sqrt(x)),y", false},

    // Alternative additive signs inside implicit arguments
    {"√x₊1", "sqrt(x)+1", false},
    {"√x﹢1", "sqrt(x)+1", false},
    {"√x＋1", "sqrt(x)+1", false},

    // Unary signs immediately following implicit functions
    {"√⁻1", "sqrt(-1)", false},
    {"√⁺1", "sqrt(+1)", false},

    // Combinations of normalization features and other complex expressions
    {"½π²", "1/2pi^(2)", false},
    {"(½π)²", "(1/2pi)^(2)", false},
    {"α+β−γ×μ÷σ", "alpha+beta-gamma*mu/sigma", false},
    {"√(1−½)²", "sqrt(1-1/2)^(2)", false},
    {"⅓×π+⅔×π", "1/3*pi+2/3*pi", false},
    {"⅓π+⅔π", "1/3pi+2/3pi", false},
    {"R☉=c×ℏ", "R_sun=c*hbar", false},
    {"㏑(1)+㏒(10)", "ln(1)+log(10)", false},
    {"１２３．４５６", "123.456", false},
    {"﹣９﹒０９", "-9.09", false},
    {"﹣₉﹒₀₉⏨₉", "-9.09*10^(9)", false},
    {"½∜(7π÷2)", "1/2qdrt(7pi/2)", false},
    {"½∜(7π÷3)R☉×c * (⅒+⅟₉)⏨₉  +ℯ⁶",
     "1/2qdrt(7pi/3)R_sun*c * (1/10+1/9)*10^(9)  +e^(6)", false},
    {"⅜∛176 + R☉(7π÷3⏨⁵)c * (⅟₁₇₉ + ⅞)⏨₇ + R⛢(7π÷3⏨⁵)c",
     "3/8cbrt(176) + R_sun(7pi/3*10^(5))c * (1/179 + 7/8)*10^(7) + "
     "R_uranus(7pi/3*10^(5))c"},
    {"12³×(231² - 1)³ + 744 - 0.00000000000075",
     "12^(3)*(231^(2) - 1)^(3) + 744 - 0.00000000000075", false},

    // Normalization must stop at the first unsupported UTF-16 code unit
    {"A😀B", "A", true},
    {"abc©def", "abc", true},
    // U+2094 is explicitly excluded from the subscript-letter lookup
    {"ₔ", "", true},
    {"abcₔdef", "abc", true},

    {"|—J│Оa╓Чn", "|", true},
#ifndef CALC_USED_EQUALS_OPERATORS
    {"±", "", true},
    {"xⁱ", "x", true},
    {"xⁿ", "x", true},
#endif
    {"x²😀", "x^(2)", true},
    // Should be fast exit without correctnes of state machines
    {"√😀", "sqrt(", true},

    // TODO: user constants and variables support:
    // Also I ask ChatGPT
    // https://chatgpt.com/c/6a89e39f-5610-83eb-8182-d914dc425042
    // Which syntax we should use?
    {"x=2π;", "x=2pi;", false},
#ifdef CALC_USED_EQUALS_OPERATORS
    // Intervals needed:
    {"[0…π]", "0???pi]", false},             // How exactly?
    {"[0…π, step]", "0???pi, step]", false}, // How exactly?
#else
    {"[0…π]", "[0", true},
    {"[0…π, step]", "[0", true},
#endif
#ifdef CALC_USED_EQUALS_OPERATORS
    {"±", "PM", true},
    {"±1", "PM(1)", false},
    {"∓1", "PM(1)", false},
#else
    {"±", "", true},
#endif

#ifndef CALC_TEST_EQUATION_SOLVER
    {"xⁱ", "x", true},
    {"xⁿ", "x", true},
#endif
#ifdef CALC_USED_EQUALS_OPERATORS
    // blocks?
    {"{x= 150 ⁄ 12 + 1 }", "x= 150 / 12 + 1 }", false},
    // TODO: equation support, for input test https://xrjunque.nom.es/AllInOne:
    // what exactly? I think we should use function, because it is more flexible
    // and can be extended to other symbols
    {"±", "?", false},
    {"{x=((−b ± √(b²−4⋅a⋅c))⁄(2a))π}", "{x=((-b ? sqrt(b^(2)-4*a*c))/(2a))pi}",
     false},
#endif
});

} // namespace GUI
#endif

#endif
