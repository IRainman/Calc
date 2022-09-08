// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2022 Solomina Elle Leonovna, a.rainman on gmail point com
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

#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Flags.h"
#include "Calculator.h"
#include "Function.h"
#include "Message.h"
#include "Linear.h"
#include "Constant.h"

//---------------------------------------------------------------------------

#if 0

#include <boost/parameter.hpp>

// I use a prefix, for example, "api"

BOOST_PARAMETER_NAME(a)
BOOST_PARAMETER_NAME(b)
BOOST_PARAMETER_NAME(c)
BOOST_PARAMETER_FUNCTION(
    (void),
    api_do_something,
    tag,
    (required
     (a, (bool))
     (b, (int))
     (c, (std::string_view))
     (d, (std::string))
    )
)
{
	do_something(a, b, c);
}

// and we can call the function like this:

api_do_something(_a = true, _b = 42, _c = "str");

// or like this

api_do_something(_b = 42, _c = "str", _a = true);

// or like this

api_do_something(42, "str", true);

...

void do_something(bool a, int b, std::string_view c)
{
	// A body of a do_something function.
}





/*
template<typename... Args> inline void pass(Args&&...) {}
template<typename... Args> inline void expand(Args&&... args) {
    pass(do_something(args)...);
}


const bool a = true;
const int b = 12;
const std::string_view c = "qwerty";

expand(a, b, c);
*/

#endif

//---------------------------------------------------------------------------
#ifdef ENABLE_INPUT_VALIDATION
void ValidateInputString(const string& p_input_str)
{
	// TODO shrink the checking
	AddMessage(PREPARING);
	
	if (p_input_str.empty())
	{
		AddError(EXPRESSION_NOT_FOUND);
		return;
	}
	
	AddMessage(p_input_str);
	
	string::size_type count_inp, count_outp;
	string::size_type l_count;
	string::size_type l_count_m1;
	string::size_type l_count_p1;
	const auto* l_start = p_input_str.data();
	
	if (l_start[0] != '-' && GetPriority(l_start[0]) > Priority::priority_bracket)
	{
		AddError(EXPRESSION_CAN_NOT_START_FROM_OPERATION, 0);
		return;
	}
	if (l_start[0] == 'e' && GetPriority(l_start[1]) != Priority::priority_error)
	{
		AddError(BEFORE_E_NOT_FOUND_NUMBER, 0);
		return;
	}
	
	for (l_count = 0, count_inp = 0, count_outp = 0; l_count < p_input_str.size(); ++l_count)
	{
		l_count_p1 = l_count + 1;
		l_count_m1 = l_count - 1;
		if (count_outp > count_inp)
		{
			AddError(CLOSING_BRACKET_BEFORE_OPENING, l_count_m1);
			return;
		}
		switch (l_start[l_count])
		{
			case '(':
			{
				if (l_start[l_count_m1] == ')')
				{
					AddError(OPENING_BRACKET_AFTER_CLOSING, l_count_m1);
					return;
				}
				if (l_start[l_count_p1] != '-' &&
				        GetPriority(l_start[l_count_p1]) > Priority::priority_bracket)
				{
					AddError(EXPRESSION_IN_BRACKETS_START_WITH_OPERATION, l_count_p1);
					return;
				}
				count_inp++;
				continue;
			}
			case ')':
			{
				if (l_start[l_count_m1] == '(')
				{
					AddError(EMPTY_BRACKETS, l_count_m1);
					return;
				}
				if (GetPriority(l_start[l_count_m1]) > Priority::priority_bracket)
				{
					AddError(EXPRESSION_IN_BRACKETS_MARK_OPERATION_ENDING, l_count_m1);
					return;
				}
				if (GetPriority(l_start[l_count_p1]) == Priority::priority_default)
				{
					AddError(NUMBERS_AFTER_CLOSING_BRACKET, l_count_p1);
					return;
				}
				if (l_start[l_count_p1] >= 'a' && l_start[l_count_p1] <= 'z')
				{
					AddError(FUNCTION_AFTER_CLOSING_BRACKET, l_count_p1);
					return;
				}
				count_outp++;
				continue;
			}
			case '.':
			{
				const auto l_nextOperator = p_input_str.find_first_of("-+/*^(),", l_count_p1);
				const auto l_nextPt = p_input_str.find('.', l_count_p1);
				if (l_nextPt != string::npos && (l_nextOperator == string::npos || l_nextPt < l_nextOperator))
				{
					AddError(MULTI_POINTS_IN_NUMBER_IS_UNACCEPTABLE, l_count, l_nextPt);
					return;
				}
				continue;
			}
			case 'e':
			{
				const auto l_nextOperator = p_input_str.find_first_of("-+/*^(),", l_count_p1);
				const auto l_nextE = p_input_str.find('e', l_count_p1);
				if (l_nextE != string::npos && (l_nextOperator == string::npos || l_nextE < l_nextOperator))
				{
					AddError(MULTI_E_SYMBOL_IN_NUMBER, l_count, l_nextE);
					return;
				}
				const auto l_count_m1_priority = GetPriority(l_start[l_count_m1]);
				const auto l_count_p1_priority = GetPriority(l_start[l_count_p1]);
				if (l_count_m1_priority != Priority::priority_error
				        && l_count_p1_priority != Priority::priority_error)
				{
					if (l_count_m1_priority > Priority::priority_default)
					{
						AddError(BEFORE_E_NOT_FOUND_NUMBER, l_count_m1);
						return;
					}
					if (l_count_p1_priority != Priority::priority_default
					        && l_count_p1_priority != Priority::priority_addition)
					{
						AddError(AFTER_E_NOT_FOUND_EXPONENT, l_count_p1);
						return;
					}
				}
				continue;
			}
			case '+':
			case '-':
			case '/':
			case '*':
			case '^':
				//case '%':
			{
				switch (l_start[l_count_p1])
				{
					case '+':
					case '*':
					case '/':
					case '^':
						//case '%':
					{
						AddError(CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS, l_count_p1);
						return;
					}
				}
			}
		}
		
	}
	
	++l_count_m1;
	if (l_start[l_count_m1] == 'e')
	{
		AddError(LAST_SYMBOL_IN_EXPRESSION_IS_E, l_count_m1);
		return;
	}
	if (count_inp != count_outp)
	{
		AddError(NON_BRACKET_BALANCE, -1, count_inp, count_outp);
		return;
	}
	switch (l_start[l_count_m1])
	{
		case '-':
		case '+':
		case '*':
		case '/':
		case '^':
			//case '%':
		{
			AddError(LAST_SYMBOL_IN_EXPRESSION_IS_OPERATION, l_count_m1);
			return;
		}
	}
	
	
	l_count = p_input_str.find("e--");
	if (l_count != string::npos)
	{
		++l_count;
		AddError(RECORD_DOUBLE_MINUS_AFTER_E, l_count);
		return;
	}
	l_count = p_input_str.find("e+-");
	if (l_count != string::npos)
	{
		++l_count;
		AddError(RECORD_PLUS_MINUS_AFTER_E, l_count);
		return;
	}
	l_count = p_input_str.find("---");
	if (l_count != string::npos)
	{
		++l_count;
		AddError(CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS, l_count);
		return;
	}
}
#endif
//---------------------------------------------------------------------------
const string& Calculate(string p_input, string& p_output)
{
	m_correct_count = 0;
#ifdef EXTENDENT_REPORT_OF_POSITION_IN_LOG
	m_corrected_spaces.clear();
#endif
	m_message_string.clear();
	m_no_error = true;
	
	// Replace all spaces on this expression
	{
		string::size_type l_count = 0;
		do
		{
			l_count = p_input.find(' ', l_count);
			if (l_count == string::npos)
				break;
				
			p_input.erase(l_count, 1);
#ifdef EXTENDENT_REPORT_OF_POSITION_IN_LOG
			m_corrected_spaces.push_back(l_count);
#endif
		}
		while (true);
	}
	
#ifdef ENABLE_INPUT_VALIDATION
	ValidateInputString(p_input);
	if (m_no_error)
#endif
	{
		ReplaceConstants(p_input);
		ProcessFunctions(p_input);
		if (m_no_error)
		{
			print_value(p_output, CalculateLineExpression(p_input));
		}
	}
	if (!m_no_error)
	{
		p_output.clear();
	}
#ifdef ENABLE_INPUT_VALIDATION
	else
	{
		AddMessage(CALCULATION);
		AddMessage(p_output);
	}
#endif
	return m_message_string;
}
//---------------------------------------------------------------------------