
#pragma once

#define CALC_TESTS_ENABLED

#ifdef CALC_TESTS_ENABLED

#include <string>
#include <vector>
#include "Calculator.h"
// const string& Calculate(string p_input, string& p_output)

auto calc_tests()
{
	std::vector<std::pair<std::string, std::string>> tests = {
		{"2 * 2", "4"},
		{"2 * 2", "4"},
		{"2 + 2 * 2", "6"},
		{"(2 + 2) * 2", "8"},
		{"2 + )", ""},
		{"1 / 0", "inf"},
		{"sin( rad(0) )", "0"},
		{"pow( sin( pi / 2 ) / .001 + 24, 2 )", "1048576"},
		{"- 4 ^ 2", "16"},
		{"0 - 4 ^ 2", "-16"},
		{"pi * sqrt(163)", "40.10916999113252"},
		{"640320 ^ 3 + 744", "262537412640768744"},
		{"e ^ (pi * sqrt(163))", "262537412640768744"},
		{"-5 % 3", "-2"},
		{"5 % -3", "2"},
		{"-5 % -3", "-2"},
		{"4 % 1.5", "1"},
		{"3.5 % 2", "1.5"},
		{"3 ^ 3 ^ 3", "7625597484987"},
		{"7 % 3 % 2", "1"},
		{"7 % 2 ^ 2", "3"},
		{"6 / 3 % 2", "0"},
		{"6 % 4 / 2", "1"},
	};

	std::string result, output;
	
	try
	{
		for (auto t : tests)
		{
			Calculate(t.first, result);
			if (result != t.second)
			{
				output += "Test failed: " + t.first + " = " + t.second + " != " + result + "                    ";
			}
		}
	}
	catch (const std::exception& e)
	{
		output += "Test failed: " + std::string(e.what()) + "                    ";
	}
	return output;
}

#endif // CALC_TESTS_ENABLED
