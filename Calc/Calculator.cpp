/*
 * Copyright 2009-2016 Solomin Alexey Leonovich, a.rainman on gmail point com
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

//#ifdef _MSC_VER
	#include "stdafx.h"
/*#elif __INTEL_COMPILER
	// TODO */

// #endif // The comment is used in a silly Visual Studio :)
//---------------------------------------------------------------------------
#ifndef _CALCULATOR_CPP
#define _CALCULATOR_CPP
//---------------------------------------------------------------------------
#include "Flags.h"
#include "Calculator.h"
#include "Function.h"
#include "Message.h"
#include "Linear.h"
//---------------------------------------------------------------------------
const static char* c_str_constant[] = {
	"pi",// Pi, Archimedes' constant or Ludolph's number
	"c",// Speed of light in vacuum (m·s-1)
	"G",// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
	"J",// Constants of Gauss fild
	"phi",// Golden ratio
	"h",// Planck constant (J·s)
	"atm",// Standard atmosphere (Pa), this constant valid on Earths only ;)
	"L",// Avogadro's number (mol−1)
	"R",// Gas constant (J·K−1·mol−1)
};
//---------------------------------------------------------------------------
const static calc_variable c_l_constant[] = {
	3.14159265358979323846264338327950288L,// Pi, Archimedes' constant or Ludolph's number
	299792458.0L,// Speed of light in vacuum (m·s-1)
	6.67428676767676767676767676767676767e-11L,// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
	3.058198247456354132564564787888767L,// Constants of Gauss fild
	1.61803398874989484820458683436563812L,// Golden ratio
	6.62606896333333333333333333333333333e-34L,// Planck constant (J·s)
	101325.0L,// Standard atmosphere (Pa), this constant valid on Earths only ;)
	6.02214151010101010101010101010101010e23L,// Avogadro's number (mol−1)
	8.31447215151515151515151515151515151L,// Gas constant (J·K−1·mol−1)
};
//---------------------------------------------------------------------------
static_assert(_countof(c_str_constant) == _countof(c_l_constant), "c_str_constant and c_l_constant sizes do not match ;) Check them out!");
//---------------------------------------------------------------------------
inline void ReplaceConstant(string& p_io_str, const string& p_const_name, const size_t p_number_of_constant)
{
	auto l_pos = p_io_str.find(p_const_name);

	auto ConstantNotFound = [](const string& p_io_str, const string& p_const_name, string::size_type l_pos) -> bool
	{
		return (l_pos == string::npos \
			|| \
			(l_pos \
			&& (GetPriority(p_io_str.c_str()[l_pos - 1]) < priority_bracket \
			|| GetPriority(p_io_str.c_str()[l_pos - 1]) == priority_error) \
			&& p_io_str.c_str()[l_pos - 1] != ',' \
			) \
			|| \
			((p_const_name.length() + l_pos) < p_io_str.length() \
			&& (GetPriority(p_io_str.c_str()[l_pos + p_const_name.length()]) < priority_bracket \
			|| GetPriority(p_io_str.c_str()[l_pos + p_const_name.length()]) == priority_error) \
			&& p_io_str.c_str()[l_pos + p_const_name.length()] != ',' \
			) \
			);
	};

	if(!ConstantNotFound(p_io_str, p_const_name, l_pos))
	{
		string l_const;
		l_const.resize(CALC_BUFFER_SIZE);
		l_const.resize(snprintf(&l_const[0], CALC_BUFFER_SIZE - 1, CALC_INTERNAL_ACCURACY_FORMAT, c_l_constant[p_number_of_constant]));
		do
		{
			p_io_str.erase(l_pos, p_const_name.length());
			m_correct_count -= p_const_name.length();
			p_io_str.insert(l_pos, l_const);
			m_correct_count += l_const.length();

			l_pos = p_io_str.find(p_const_name);
			if(ConstantNotFound(p_io_str, p_const_name, l_pos))
				break;
		}
		while(true);
	}
}
//---------------------------------------------------------------------------
void ReplaceConstants(string& p_io_str)
{
	AddMessage(REPLACEMENTS_OF_THE_CONSTANTS);

	for (size_t i = 0; i < _countof(c_str_constant); i++)
		ReplaceConstant(p_io_str, c_str_constant[i], i);

	AddMessage(p_io_str);
}
//---------------------------------------------------------------------------
void ValidateInputString(string& p_io_str)
{
	AddMessage(PREPARING);
	AddMessage(p_io_str);

	string::size_type count_inp, count_outp;
	string::size_type l_count;
	string::size_type l_count_m1;
	string::size_type l_count_p1;
	const auto* l_start = p_io_str.c_str();

	if (p_io_str.empty())
	{
		AddError(EXPRESSION_NOT_FOUND);
		return;
	}
	l_count = p_io_str.find_first_not_of("0123456789+-*/^.e(),picGJphiatmLRpowsincosexptanacosasinatanloglog10sqrt");
	if (l_count != string::npos)
	{
		AddError(INVALID_CHARACTER, l_count);
		return;
	}
	if (GetPriority(l_start[0]) > priority_bracket && l_start[0] != '-')
	{
		AddError(EXPRESSION_CAN_NOT_START_FROM_OPERATION, 0);
		return;
	}
	if (l_start[0] == 'e' && GetPriority(l_start[1]) != priority_error)
	{
		AddError(BEFORE_E_NOT_FOUND_NUMBER, 0);
		return;
	}
	for (l_count = 0, count_inp = 0, count_outp = 0; l_count < p_io_str.size(); l_count++)
	{
		if (count_outp > count_inp)
		{
			AddError(CLOSING_BRACKET_BEFORE_OPENING, l_count - 1);
			return;
		}
		if (l_start[l_count] == '(')
		{
			if (l_start[l_count - 1] == ')')
			{
				AddError(OPENING_BRACKET_AFTER_CLOSING, l_count - 1);
				return;
			}
			l_count_p1 = l_count + 1;
			if (GetPriority(l_start[l_count_p1]) > priority_bracket &&
				l_start[l_count_p1] != '-')
			{
				AddError(EXPRESSION_IN_BRACKETS_START_WITH_OPERATION, l_count_p1);
				return;
			}
			count_inp++;
			continue;
		}
		if (l_start[l_count] == ')')
		{
			l_count_m1 = l_count - 1;
			if (l_start[l_count_m1] == '(')
			{
				AddError(EMPTY_BRACKETS, l_count_m1);
				return;
			}
			if (GetPriority(l_start[l_count_m1]) > priority_bracket)
			{
				AddError(EXPRESSION_IN_BRACKETS_MARK_OPERATION_ENDING, l_count_m1);
				return;
			}
			l_count_p1 = l_count + 1;
			if (GetPriority(l_start[l_count_p1]) == priority_default)
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
		l_count_p1 = l_count + 1;
		if (l_start[l_count] == '.')
		{
			const auto l_nextOperator = p_io_str.find_first_of("-+/*^()", l_count_p1);
			const auto l_nextPt = p_io_str.find('.', l_count_p1);
			if (l_nextPt != string::npos && (l_nextOperator == string::npos || l_nextPt < l_nextOperator))
			{
				AddError(MULTI_POINTS_IN_NUMBER_IS_UNACCEPTABLE, l_count, l_nextPt);
				return;
			}
			continue;
		}
		if (l_start[l_count] == 'e')
		{
			const auto l_nextOperator = p_io_str.find_first_of("-+/*^()", l_count_p1);
			const auto l_nextE = p_io_str.find('e', l_count_p1);
			if (l_nextE != string::npos && (l_nextOperator == string::npos || l_nextE < l_nextOperator))
			{
				AddError(MULTI_E_SYMBOL_IN_NUMBER, l_count, l_nextE);
				return;
			}
			l_count_m1 = l_count - 1;
			if (GetPriority(l_start[l_count_m1]) != priority_error
				&& GetPriority(l_start[l_count_p1]) != priority_error)
			{
				if (GetPriority(l_start[l_count_m1]) > priority_default)
				{
					AddError(BEFORE_E_NOT_FOUND_NUMBER, l_count_m1);
					return;
				}
				if (GetPriority(l_start[l_count_p1]) != priority_default
					&& GetPriority(l_start[l_count_p1]) != priority_addition)
				{
					AddError(AFTER_E_NOT_FOUND_EXPONENT, l_count_p1);
					return;
				}
			}
			continue;
		}
		if ((l_start[l_count] == '+' || l_start[l_count] == '-'
			|| l_start[l_count] == '/' || l_start[l_count] == '*'
			|| l_start[l_count] == '^'
			//			|| l_start[l_count] == '%'
			)

			&& (l_start[l_count_p1] == '+' || l_start[l_count_p1] == '*'
				|| l_start[l_count_p1] == '/' || l_start[l_count_p1] == '^'
				//			|| l_start[l_count_p1] == '%'
				))
		{
			AddError(CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS, l_count_p1);
			return;
		}
	}
	if (l_start[l_count - 1] == 'e')
	{
		AddError(LAST_SYMBOL_IN_EXPRESSION_IS_E, l_count);
		return;
	}
	if (count_inp != count_outp)
	{
		AddError(NON_BRACKET_BALANCE, -1, count_inp, count_outp);
		return;
	}
	const auto p_input_str_length_m1 = p_io_str.length() - 1;
	if (p_io_str[p_input_str_length_m1] == '-' || p_io_str[p_input_str_length_m1] == '+'
		|| p_io_str[p_input_str_length_m1] == '*' || p_io_str[p_input_str_length_m1] == '/'
		|| p_io_str[p_input_str_length_m1] == '^'
		//		|| p_input_str[p_input_str_length_m1] == '%'
		)
	{
		AddError(LAST_SYMBOL_IN_EXPRESSION_IS_OPERATION, p_input_str_length_m1);
		return;
	}
	l_count = p_io_str.find("e--");
	if (l_count != string::npos)
	{
		AddError(AFTER_E_NOT_FOUND_EXPONENT, l_count + 1);
		return;
	}
	l_count = p_io_str.find("e+-");
	if (l_count != string::npos)
	{
		AddError(RECORD_PLUS_MINUS_AFTER_E, l_count + 1);
		return;
	}
	// This hack is needed to address the algorithmic errors below :(
	l_count = p_io_str.find("---");
	if (l_count != string::npos)
	{
		AddError(CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS, l_count + 1);
		return;
	}
	// end hack!!!

	// This hack is needed to address the algorithmic errors below :(
	l_count = 0;
	do
	{
		l_count = p_io_str.find(")--(", l_count);
		if (l_count == string::npos)
			break;

		l_count_p1 = l_count + 1;
		p_io_str.erase(l_count_p1, 2);
		p_io_str.insert(l_count_p1, 1, '+');
		m_correct_count--;
	}
	while(true);
	l_count = 0;
	do
	{
		l_count = p_io_str.find(")+-(", l_count);
		if (l_count == string::npos)
			break;

		l_count_p1 = l_count + 1;
		p_io_str.erase(l_count_p1, 2);
		p_io_str.insert(l_count_p1, 1, '-');
		m_correct_count--;
	}
	while(true);
	// end hack!!!
}
//---------------------------------------------------------------------------
const string& Calculate(string p_input, string& p_output)
{
	m_correct_count = 0;
#ifdef 0
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
#ifdef 0
			m_corrected_spaces.push_back(l_count);
#endif
		}
		while (true);
	}
	
	// Allow all cases, don't activate this code!
	//for(string::size_type l_count = 0; l_count < p_input.size(); l_count++)
	//	p_input[l_count] = tolower(p_input.c_str()[l_count]);

	ValidateInputString(p_input);
	if (m_no_error)
	{
		ReplaceConstants(p_input);
		ProcessFunctions(p_input);
		if (m_no_error)
		{
			CalculateLineExpression(p_input, p_output);
		}
	}
	if(m_no_error)
	{
		// TODO: Add variable accuracy
		//const auto l_tmp = (round(calc_input_function(p_output.c_str(), nullptr) * calc_output_accuracy_mult)) / calc_output_accuracy_mult;
		const auto l_tmp = calc_input_function(p_output.c_str(), nullptr);
		p_output.resize(CALC_BUFFER_SIZE);
		p_output.resize(snprintf(&p_output[0], CALC_BUFFER_SIZE - 1, CALC_OUTPUT_ACCURACY_FORMAT, l_tmp));
	}
	else
	{
		p_output.clear();
	}

	return m_message_string;
}
//---------------------------------------------------------------------------
#endif // _CALCULATOR_CPP