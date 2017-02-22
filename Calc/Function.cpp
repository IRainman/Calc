/*
 * Copyright 2009-2017 Solomin Alexey Leonovich, a.rainman on gmail point com
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
#ifndef _FUNCTION_CPP
#define _FUNCTION_CPP
//---------------------------------------------------------------------------
#include "Flags.h"
#include "Linear.h"
#include "Message.h"
#include "Function.h"
#include <stdlib.h>
//---------------------------------------------------------------------------
// TODO
//extern const calc_variable c_l_constant[];
//
//inline calc_variable rad(calc_variable x)
//{
//	return x * c_l_constant[0] / 180;
//}
// TODO http://en.cppreference.com/w/cpp/numeric/math
// TODO http://en.cppreference.com/w/cpp/experimental/special_math
//---------------------------------------------------------------------------
const string c_function1[] =
{
	//"rad(",// One radian is equivalent to 180/PI degrees.
	"sin(",// sinus
	"cos(",// cosinus
	"exp(",// exponent function, on x=1 return value is e
	"tan(",// tangens
	"acos(",// arccosinus
	"asin(",// arcsinus
	"atan(",// arctangens
	"log(",// natural logarithm
	"log10(",// base-10 logarithm
	"sqrt("// square root
	//"ceil(",
	//"fabs(",
	//"floor(",
	//"ldexp(",
	//"modf(",
};
typedef calc_variable(* fptr1)(calc_variable);
const fptr1 f_function1[] =
{
	//rad,
	sin,
	cos,
	exp,
	tan,
	acos,
	asin,
	atan,
	log,
	log10,
	sqrt,
	//ceil,
	//fabs,
	//floor,
	//ldexp,
	//modf,
};
const string c_function2[] =
{
	"pow("// power
	//"atan2(",
	//"modf(",
	//"fmod(",
	//"frexp(",
	
	//"max(",
	//"min(",
};
typedef calc_variable(* fptr2)(calc_variable, calc_variable);
const fptr2 f_function2[] =
{
	pow,// power
	//atan2,
	//modf,
	//fmod,
	//frexp,
	
	//fmax,
	//fmin,
};
#ifdef _DEBUG_FUNCTION // TODO: delete this block after add full function support
const string c_function3[] =
{
	"plus3(",// this function performs addition of three arguments
	"spin3("// this function performs addition of three arguments
};
const string c_function5[] =
{
	"plus5("// this function performs addition of five arguments
};
#define c_max_argument_of_function 5
#else
#define c_max_argument_of_function 2
#endif
//---------------------------------------------------------------------------
inline calc_variable CalculateParametrs(const size_t p_number_of_param, const size_t p_function_number, calc_variable p_params[])
{
	switch (p_number_of_param)
	{
		case 1:
			return f_function1[p_function_number](p_params[0]);
		case 2:
			return f_function2[p_function_number](p_params[0], p_params[1]);
#ifdef _DEBUG_FUNCTION // TODO: delete this block after add full function support
		case 3:
			return p_params[0] + p_params[1] + p_params[2];
		case 5:
			return p_params[0] + p_params[1] + p_params[2] + p_params[3] + p_params[4];
#endif
#ifdef _DEBUG
		default:
			AddError(INTERNAL_PROCESSING_ERROR_CalculateFunction);
			return 0;
#endif
	}
	
	// TODO: delete this block after add full function support
	m_no_error = false;
	return 0;
	// ~TODO
}
//---------------------------------------------------------------------------
inline string::size_type CheckParametr(const string& p_param_str)
{
	const auto l_count = p_param_str.find_first_not_of("0123456789.e", (p_param_str.c_str()[0] != '-') ? 0 : 1);
	if (l_count == string::npos)
	{
		return string::npos;
	}
	if (p_param_str.compare(l_count, 2, "e+", 2) == 0
	        || p_param_str.compare(l_count, 2, "e-", 2) == 0)
	{
		return string::npos;
	}
	return l_count;
}
//---------------------------------------------------------------------------
void ProcessParametr(string& p_param_str, const string::size_type p_start, const size_t p_number_of_param, calc_variable p_params[], const string::size_type p_comma_count, const string::size_type p_correct_end)
{
	if (p_comma_count >= p_number_of_param)
	{
		AddError(FUNCTION_INVALID_NUMBER_OF_ARGUMENTS, p_start, p_number_of_param, p_comma_count + 1);
		return;
	}
	auto l_count = CheckParametr(p_param_str);
	if (l_count != string::npos)
	{
		AddMessage(FOUND_SUBEXPRESSIONS);
		string l_outp;
		ProcessFunctions(p_param_str, p_start);
		CalculateLineExpression(p_param_str, l_outp, p_start);
		p_param_str.clear();
		AddMessage(END_OF_THE_SUBEXPRESSION);
		if (l_outp.empty())
		{
			AddError(LOST_FUNCTION_ARGUMENTS, p_start, p_comma_count + 1, p_correct_end);
			return;
		}
		l_count = CheckParametr(l_outp);
		if (l_count != string::npos)
		{
			AddError(LOST_FUNCTION_ARGUMENTS, p_start, p_comma_count + 1, p_correct_end);
			return;
		}
		p_params[p_comma_count] = calc_input_function(l_outp.c_str(), nullptr);
		return;
	}
	p_params[p_comma_count] = calc_input_function(p_param_str.c_str(), nullptr);
	p_param_str.clear();
}
//---------------------------------------------------------------------------
void CalculateFunction(string& p_io_str, const string& p_func_name, const size_t p_count, const size_t p_number_of_param)
{
	// find func
	auto l_start = p_io_str.find(p_func_name);
	if (l_start == string::npos)
	{
		return;
	}
	else if (l_start)
	{
		const auto l_current_priority = GetPriority(p_io_str.c_str()[l_start - 1]);
		if (l_current_priority < priority_bracket ||
		        l_current_priority == priority_error)
		{
			return;
		}
	}
	// ~find func
	do
	{
		string l_params_str;
		string l_buf = p_io_str.substr(l_start + p_func_name.size());
		calc_variable l_params[c_max_argument_of_function];
		string::size_type l_comma_count = 0;
		size_t l_nesting_level = 0;
		string::size_type l_end;
		
		for (string::size_type l_correct_end = 0;;)
		{
			const auto l_br_start = l_buf.find('(');
			l_end = l_buf.find(')');
			const auto l_current_comma = l_buf.find(',');
			if (l_br_start != string::npos && l_br_start < l_end && l_current_comma > l_br_start)
			{
				l_correct_end += l_br_start + 1;
				l_params_str += l_buf.substr(0, l_br_start + 1);
				l_buf = l_buf.substr(l_br_start + 1);
				l_nesting_level++;
			}
			else if (!l_nesting_level)
			{
				if (l_current_comma != string::npos && l_current_comma < l_end)
				{
					l_params_str += l_buf.substr(0, l_current_comma);
					l_correct_end += l_current_comma + 1;
					l_buf = l_buf.substr(l_current_comma + 1);
					if (l_params_str.empty())
					{
						AddError(LOST_FUNCTION_ARGUMENTS, l_start, l_comma_count + 1, l_correct_end);
						return;
					}
					ProcessParametr(l_params_str, l_start, p_number_of_param, l_params, l_comma_count, l_correct_end);
					l_comma_count++;
				}
				else if (l_current_comma == string::npos || l_current_comma > l_end)
				{
					l_params_str += l_buf.substr(0, l_end);
					if (l_params_str.empty())
					{
						AddError(LOST_FUNCTION_ARGUMENTS, l_start, l_comma_count + 1, l_correct_end);
						return;
					}
					l_end += l_correct_end;
					ProcessParametr(l_params_str, l_start, p_number_of_param, l_params, l_comma_count, l_correct_end);
					break;
				}
			}
			else
			{
				l_correct_end += l_end + 1;
				l_params_str += l_buf.substr(0, l_end + 1);
				l_buf = l_buf.substr(l_end + 1);
				l_nesting_level--;
			}
		}
		if (l_comma_count != p_number_of_param - 1)
		{
			AddError(FUNCTION_INVALID_NUMBER_OF_ARGUMENTS, l_start, p_number_of_param, l_comma_count + 1);
			return;
		}
		const auto l_result = CalculateParametrs(p_number_of_param, p_count, l_params);
		l_params_str.resize(CALC_BUFFER_SIZE);
		l_params_str.resize(snprintf(&l_params_str[0], CALC_BUFFER_SIZE - 1, CALC_INTERNAL_ACCURACY_FORMAT, l_result));
		
		{
			const string::size_type l_erased = p_func_name.size() + l_end + 1;
			p_io_str.erase(l_start, l_erased);
			m_correct_count -= l_erased;
			p_io_str.insert(l_start, l_params_str);
			m_correct_count += l_params_str.size();
		}
		// find func
		l_start = p_io_str.find(p_func_name);
		if (l_start == string::npos)
		{
			return;
		}
		else if (l_start)
		{
			const auto l_current_priority = GetPriority(p_io_str.c_str()[l_start - 1]);
			if (l_current_priority < priority_bracket ||
			        l_current_priority == priority_error)
			{
				return;
			}
		}
		// ~find func
	}
	while (true);
}
//---------------------------------------------------------------------------
void ProcessFunctions(string& p_io_str, const string::size_type p_mes_pos_shift /*= 0*/)
{
	AddMessage(CALCULATE_FUNCTIONS);
	
	for (size_t i = 0; i < _countof(c_function1); i++)
		CalculateFunction(p_io_str, c_function1[i], i, 1);
		
	for (size_t i = 0; i < _countof(c_function2); i++)
		CalculateFunction(p_io_str, c_function2[i], i, 2);
		
#ifdef _DEBUG_FUNCTION // TODO: delete this block after add full function support
	for (size_t i = 0; i < _countof(c_function3); i++)
		CalculateFunction(p_io_str, c_function3[i], i, 3);
		
	for (size_t i = 0; i < _countof(c_function5); i++)
		CalculateFunction(p_io_str, c_function5[i], i, 5);
#endif
	AddMessage(p_io_str.c_str());
	
	for (string::size_type l_count = 0; l_count < p_io_str.size() - 1; l_count++)
	{
		if (p_io_str.c_str()[l_count + 1] == '('
		        && GetPriority(p_io_str.c_str()[l_count]) == priority_default)
		{
			AddError(NUMBERS_BEFORE_OPENING_BRACKET, l_count - 1 + p_mes_pos_shift);
			return;
		}
	}
	const auto l_count = p_io_str.find_first_not_of("0123456789+-*/^.e()", 0);
	if (l_count != string::npos)
	{
		AddError(INVALID_CHARACTER_AFTER_CONSTANT_AND_FUNCTION_PROCESS, l_count + p_mes_pos_shift);
		return;
	}
}
//---------------------------------------------------------------------------
#endif // _FUNCTION_CPP