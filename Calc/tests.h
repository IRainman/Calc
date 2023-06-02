/*
 * Copyright 2023 Solomina Elle Leonovna, a.rainman on gmail point com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#ifdef _DEBUG
	#define CALC_TESTS_ENABLED
#endif

#ifdef CALC_TESTS_ENABLED

#include "Calculator.h"

auto calc_tests()
{
	const std::vector<std::pair<std::string, std::string>> tests = { //-V826
		{"2 + )", "nan"},
		{"2 + (", "nan"},
		{"e(", "nan"},
		{"1(", "nan"},
		{"sin(0 ", "nan"},

		
		{"1.4e-3", "0.0014"},

		{"2 + 2", "4"},
		{"2 * 2", "4"},
		{"2 + 2 * 2", "6"},
		{"(2 + 2) * 2", "8"},

		{"1 / 0", "inf"},

		{"sin( rad(0) )", "0"},

		{"log(sh(42) + ch(42))", "42"},

		{"pow( sin( pi / 2 ) / .001 + 24, 2 )", "1048576"},
		{"0^0", "1"},
		{"- 4 ^ 2", "16"},
		{"0 - 4 ^ 2", "-16"},
		{"3 ^ 3 ^ 3", "7625597484987"},
		{"(3 ^ 3) ^ 3", "19683"},

		{"-5 % 3", "-2"},
		{"5 % -3", "2"},
		{"-5 % -3", "-2"},
		{"4 % 1.5", "1"},
		{"3.5 % 2", "1.5"},
		{"7 % 3 % 2", "1"},
		{"7 % 2 ^ 2", "3"},
		{"6 / 3 % 2", "0"},
		{"6 % 4 / 2", "1"},

		{"min(1)", "1"},
		{"min(1, 2, 3)", "1"},
		{"max(1, 2, 3)", "3"},
		
		{"pi * sqrt(163)", "40.10916999113252"},
		//{"640320 ^ 3 + 744", "262537412640768744"},
		//{"e ^ (pi * sqrt(163))", "262537412640768744"},
	};

	std::string result;
	std::string output;
	
	for (auto& t : tests)
	{
		const std::string message = Calculate(t.first, result);
		if (result != t.second)
		{
			output += "Test failed: " + t.first + " = " + t.second + " != " + result + ". " + message + "\r\n";
		}
	}
	if (output.empty())
	{
		output += "Tests passed!";
	}
	return output;
}

#endif // CALC_TESTS_ENABLED
