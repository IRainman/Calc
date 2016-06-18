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
#ifndef _FUNCTION_CPP
#define _FUNCTION_CPP
//---------------------------------------------------------------------------
#include "Flags.h"
#include "RPN.h"
#include "Message.h"
#include "Function.h"
#include <stdlib.h>
//---------------------------------------------------------------------------
void PreparingForFunction(string& p_input_str);
//---------------------------------------------------------------------------
const static char* c_function1[] =
{
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
	//"modf("
};
typedef calc_variable(* fptr1)(calc_variable);
const static fptr1 f_function1[] =
{
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
	//modf
};
const static char* c_function2[] =
{
	"pow("// power
	//"atan2(",
	//"modf(",
	//"fmod(",
	//"frexp(",
};
typedef calc_variable(* fptr2)(calc_variable, calc_variable);
const static fptr2 f_function2[] =
{
	pow,// power
	//atan2,
	//modf,
	//fmod,
	//frexp,
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
inline string::size_type CheckParametrs(string& p_buf)
{
	const string::size_type l_count = p_buf.find_first_not_of("0123456789.e", (p_buf.c_str()[0] != '-') ? 0 : 1);
	if (l_count &&
	        (p_buf.find("e+") == l_count - 1
	         || p_buf.find("e-") == l_count - 1))
	{
		return string::npos;
	}
	return l_count;
}
//---------------------------------------------------------------------------
void GetParametrs(string& p_buf, const string::size_type p_start, const size_t p_number_of_param, calc_variable p_params[], const string::size_type p_comma_count, const string::size_type p_correct_end)
{
	if (p_comma_count >= p_number_of_param)
	{
		AddError(FUNCTION_INVALID_NUMBER_OF_ARGUMENTS, p_start, p_number_of_param, p_comma_count + 1);
		return;
	}
	string::size_type l_count = CheckParametrs(p_buf);
	if (l_count != string::npos)
	{
		AddMessage(FOUND_SUBEXPRESSIONS);
		string l_outp;
		PreparingForFunction(p_buf);
		CalculateLineExpression(p_buf, l_outp);
		p_buf.clear();
		AddMessage(END_OF_THE_SUBEXPRESSION);
		l_count = CheckParametrs(l_outp);
		if (l_count != string::npos || l_outp.empty())
		{
			AddError(LOST_FUNCTION_ARGUMENTS, p_start, p_comma_count + 1, p_correct_end);
			return;
		}
		p_params[p_comma_count] = atof(l_outp.c_str());
		return;
	}
	p_params[p_comma_count] = atof(p_buf.c_str());
	p_buf.clear();
}
//---------------------------------------------------------------------------
void CalculateFunction(string& p_input_str, const string& p_in, const size_t p_count, const size_t p_number_of_param)
{
	string::size_type l_start;
	l_start = p_input_str.find(p_in);
	if (l_start == string::npos
	        || (l_start && (GetPriority(p_input_str.c_str()[l_start - 1]) < priority_bracket
	                        || GetPriority(p_input_str.c_str()[l_start - 1]) == priority_error))
	        || p_in.empty()
	   )
		return;
		
	string::size_type l_end, l_br_start;
	char l_char_buf[320];
	calc_variable l_params[c_max_argument_of_function];
	do
	{
		string l_temp;
		string l_buf = p_input_str.substr(l_start + p_in.size());
		string::size_type l_current_comma, l_comma_count = 0;
		size_t l_nesting_level = 0;
		for (string::size_type l_correct_end = 0;;)
		{
			l_br_start = l_buf.find('(');
			l_end = l_buf.find(')');
			l_current_comma = l_buf.find(',');
			if (l_br_start != string::npos && l_br_start < l_end && l_current_comma > l_br_start)
			{
				l_correct_end += l_br_start + 1;
				l_temp += l_buf.substr(0, l_br_start + 1);
				l_buf = l_buf.substr(l_br_start + 1);
				l_nesting_level++;
			}
			else if (!l_nesting_level)
			{
				if (l_current_comma != string::npos && l_current_comma < l_end)
				{
					l_temp += l_buf.substr(0, l_current_comma);
					l_correct_end += l_current_comma + 1;
					l_buf = l_buf.substr(l_current_comma + 1);
					if (l_temp.empty())
					{
						AddError(LOST_FUNCTION_ARGUMENTS, l_start, l_comma_count + 1, l_correct_end);
						return;
					}
					GetParametrs(l_temp, l_start, p_number_of_param, l_params, l_comma_count, l_correct_end);
					l_comma_count++;
				}
				else if (l_current_comma == string::npos || l_current_comma > l_end)
				{
					l_temp += l_buf.substr(0, l_end);
					if (l_temp.empty())
					{
						AddError(LOST_FUNCTION_ARGUMENTS, l_start, l_comma_count + 1, l_correct_end);
						return;
					}
					l_end += l_correct_end;
					GetParametrs(l_temp, l_start, p_number_of_param, l_params, l_comma_count, l_correct_end);
					break;
				}
			}
			else
			{
				l_correct_end += l_end + 1;
				l_temp += l_buf.substr(0, l_end + 1);
				l_buf = l_buf.substr(l_end + 1);
				l_nesting_level--;
			}
		}
		if (l_comma_count != p_number_of_param - 1)
		{
			AddError(FUNCTION_INVALID_NUMBER_OF_ARGUMENTS, l_start, p_number_of_param, l_comma_count + 1);
			return;
		}
		auto result = CalculateParametrs(p_number_of_param, p_count, l_params);
		string::size_type l_c;
		try
		{
			l_c = static_cast<string::size_type>(sprintf_s(l_char_buf, "%.25g", result)); //-V618
		}
		catch (...)
		{
#ifdef _DEBUG
			AddError(INTERNAL_PROCESSING_ERROR_CalculateFunction);
			return;
#endif
		}
		l_temp = l_char_buf;
		if (m_no_error)
			m_correct_count += l_c - (p_in.size() + l_end + 1);
			
		p_input_str.erase(l_start, p_in.size() + l_end + 1);
		p_input_str.insert(l_start, l_temp);
		
		l_start = p_input_str.find(p_in);
		if (l_start == string::npos
		        || (l_start && (GetPriority(p_input_str.c_str()[l_start - 1]) < priority_bracket
		                        || GetPriority(p_input_str.c_str()[l_start - 1]) == priority_error))
		        || p_in.empty()
		   )
			return;
	}
	while (true);
}
//---------------------------------------------------------------------------
void PreparingForFunction(string& p_input_str)
{
	if (!m_no_error)
		return;
		
	AddMessage(CALCULATION_FUNCTIONS);

	size_t i;
	i = 0;
	for (; i < _countof(c_function1); i++)
		CalculateFunction(p_input_str, c_function1[i], i, 1);

	i = 0;
	for (; i < _countof(c_function2); i++)
		CalculateFunction(p_input_str, c_function2[i], i, 2);
	
#ifdef _DEBUG_FUNCTION // TODO: delete this block after add full function support
	i = 0;
	for (; i < _countof(c_function3); i++)
		CalculateFunction(p_input_str, c_function3[i], i, 3);
		
	i = 0;
	for (; i < _countof(c_function5); i++)
		CalculateFunction(p_input_str, c_function5[i], i, 5);
#endif
	AddMessage(p_input_str.c_str());
	
	if (!m_no_error)
		return;
		
	string::size_type l_count = 0;
	for (; l_count < p_input_str.size() - 1; l_count++)
	{
		if (GetPriority(p_input_str.c_str()[l_count]) == priority_default
		        && p_input_str.c_str()[l_count + 1] == '(')
		{
			AddError(NUMBERS_BEFORE_OPENING_BRACKET, l_count - 1);
		}
	}
	l_count = p_input_str.find_first_not_of("0123456789+-*/^.e()", 0);
	if (l_count != string::npos)
	{
		AddError(INVALID_CHARACTER_AFTER_VAR_AND_FUNC, l_count);
	}
}
//---------------------------------------------------------------------------
#endif // _FUNCTION_CPP