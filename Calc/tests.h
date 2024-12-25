#pragma once
/*
 * Copyright 2023-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#ifdef CALC_TESTS_ENABLED

auto calc_tests()
{
	const std::vector<std::pair<std::string, std::string>> tests =   //-V826
	{
		// syntax errors
		{"2 + )", "nan"
#ifdef _WIN64
		"(snan)"
#endif
		},
		{"2 + (", "nan"},
		{"e(", "nan"},
		{"1(", "nan"
#ifdef _WIN64
		"(snan)"
#endif
		},
		{"sin(0 ", "nan"},
		{"sin(", "nan"},

		// value parsing
		{"1.4e-3", "0.0014"},

		// operation priority
		{"2 + 2", "4"},
		{"2 * 2", "4"},
		{"2 + 2 * 2", "6"},
		{"(2 + 2) * 2", "8"},

		// precision
		{"10000 / 540 * 3", "55.5555555555556"},
		{"1 / 3 * 3", "1"},
		{"640320 ^ 3 + 744",   "2.62537412640769e+17"},
		{"e^(pi * sqrt(163))", "2.62537412640768e+17"},
		
		// special value support
		{"1 / 0", "inf"},
		
		// trigonometric functions
		{"sin( rad(0) )", "0"},
		{"87 * tan(pi) - 7", "-7"},
		{"tan(-pi)", "0"},
		{"tan(inf)", "-nan(ind)"},
		
		{"log(sh(42) + ch(42))", "42"},

		{"ln(e)", "1"},
		
		// power and root functions
		{"pow( sin( pi / 2 ) / .001 + 24, 2 )", "1048576"},
		{"0^0", "1"},
		{"-4 ^ 2", "16"},
		{"- 4 ^ 2", "16"},
		{"0 - 4 ^ 2", "-16"},
		{"3 ^ 3 ^ 3", "7625597484987"},
		{"(3 ^ 3) ^ 3", "19683"},
		
		// modulo operator
		{"-5 % 3", "-2"},
		{"5 % -3", "2"},
		{"-5 % -3", "-2"},
		{"4 % 1.5", "1"},
		{"3.5 % 2", "1.5"},
		{"7 % 3 % 2", "1"},
		{"7 % 2 ^ 2", "3"},
		{"6 / 3 % 2", "0"},
		{"6 % 4 / 2", "1"},
		{"mod(3.5, 2)", "1.5"},
		
		// min max functions
		{"min(1)", "1"},
		{"min(1, 2, 3)", "1"},
		{"max(1, 2, 3)", "3"},

		{"fma(2, 2, 2)", "6"},

		{"minkowski_distance(1, 1, 2, 3)", "6"},
	};
	
	std::string output;
	for (const auto& t : tests)
	{
		Lexer l{ t.first };
		Parser p{ l };
		const auto result = Formatter::format_output_value(p.parse());
		if (result != t.second)
		{
			output += "Test failed:\r\n\t" + t.first + " = " + t.second + " != " + result + ". " + Formatter::create_summary() + "\r\n";
		}
		IssueManager::clear();
	}
	if (output.empty())
	{
		output += "Tests passed!";
	}
	return output;
}

#endif
