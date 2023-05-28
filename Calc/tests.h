
#pragma once

#define CALC_TESTS_ENABLED

#ifdef CALC_TESTS_ENABLED

#include <string>
#include <vector>
#include "Calculator.h"
// const string& Calculate(string p_input, string& p_output)

void calc_tests()
{
	std::vector<std::pair<std::string, std::string>> tests;
	tests.reserve(100);

	tests.emplace_back("2 * 2", "4");
	tests.emplace_back("2 + 2 * 2", "6");
	tests.emplace_back("(2 + 2) * 2", "8");
	//tests.emplace_back("a", "x");
	tests.emplace_back("2 + )", "");
	tests.emplace_back("sin(rad(0))", "0");
	tests.emplace_back("pow(sin(pi/2) / .001 + 24, 2)", "1048576");
	tests.emplace_back("- 4 ^ 2", "16");
	tests.emplace_back("0 - 4 ^ 2", "-16");
	tests.emplace_back("pi * sqrt(163)", "40.10916999113252");
	tests.emplace_back("640320^3 + 744", "262537412640768744");
	tests.emplace_back("e^(pi * sqrt(163))", "262537412640768744");
	tests.emplace_back("-5 % 3", "-2");
	tests.emplace_back("5 % -3", "2");
	tests.emplace_back("-5 % -3", "-2");
	//tests.emplace_back("4 % 1.5", "1");
	//tests.emplace_back("3.5 % 2", "1.5");
	tests.emplace_back("3 ^ 3 ^ 3", "7625597484987");
	tests.emplace_back("7 % 3 % 2", "1");
	tests.emplace_back("7 % 2 ^ 2", "3");
	tests.emplace_back("6 / 3 % 2", "0");
	tests.emplace_back("6 % 4 / 2", "1");

	std::string result;
	std::string output;
	
	try
	{
		for (auto t : tests)
		{
			Calculate(t.first, result);
			if (result != t.second)
			{
				output += "" + t.first + " = " + t.second + " != " + result;
			}
		}
	}
	catch (const std::exception& e)
	{
		output += std::string(e.what());
	}
	output;
}

#endif // CALC_TESTS_ENABLED
