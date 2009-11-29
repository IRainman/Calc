
#ifdef _MSC_VER
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
const char* c_function1[] = {
	"sin(",// sinus
	"cos(",// cosinus
	"exp(",// exponent function, on x=1 return value is e
	"tan(",// tangens
	"acos(",// arccosinus
	"asin(",// arcsinus
	"atan(",// arctangens
	"log(",// natural logarithm
	"log10("// base-10 logarithm
	/*"ceil(",
	"fabs(",
	"floor(",
	"ldexp(",
	"modf("*/
};
const char* c_function2[] = {
	"pow("// power
	/*"atan2(",
	"modf(",
	"fmod(",
	"frexp(",*/
};
#ifdef _DEBUG_FUNCTION // TODO: delete this block after add full function support
const string c_function3[] = {
	"plus3(",// this function performs addition of three arguments
	"spin3("// this function performs addition of three arguments
};
const string c_function5[] = {
	"plus5("// this function performs addition of five arguments
};
#define c_max_argument_of_function 5
#else
#define c_max_argument_of_function 2
#endif
//---------------------------------------------------------------------------
inline long double CalculateParametrs(const uint_8 p_number_of_param, const uint p_count, long double p_params[])
{
	switch(p_number_of_param)
	{
		case 1:
			switch(p_count)
			{
				case 0:
					return sin(p_params[0]);
				case 1:
					return cos(p_params[0]);
				case 2:
					return exp(p_params[0]);
				case 3:
					return tan(p_params[0]);
				case 4:
					return acos(p_params[0]);
				case 5:
					return asin(p_params[0]);
				case 6:
					return atan(p_params[0]);
				case 7:
					return log(p_params[0]);
				case 8:
					return log10(p_params[0]);
#ifdef _DEBUG
				default:
					AddError(253);
					return 0;
#endif
			}
			break;
		case 2:
			switch(p_count)
			{
				case 0:
					return pow(p_params[0], p_params[1]);
#ifdef _DEBUG
				default:
					AddError(253);
					return 0;
#endif
			}
			break;
#ifdef _DEBUG_FUNCTION // TODO: delete this block after add full function support
		case 3:// FOR TESTS ONLY, its not real function
			switch(p_count)
			{
				case 0:
				case 1:
					return p_params[0] + p_params[1] + p_params[2];
#ifdef _DEBUG
				default:
					AddError(253);
					return 0;
#endif
			}
			break;
		case 5:// FOR TESTS ONLY, its not real function
			switch(p_count)
			{
				case 0:
				case 1:
					return p_params[0] + p_params[1] + p_params[2] + p_params[3] + p_params[4];
#ifdef _DEBUG
				default:
					AddError(253);
					return 0;
#endif
			}
			break;
#endif
#ifdef _DEBUG
		default:
			AddError(253);
			return 0;
#endif
	}
	// TODO: delete this block after add full function support
		m_NoError = false;
		return 0;
}
//---------------------------------------------------------------------------
void GetParametrs(string& l_buf, const string::size_type l_start, const uint_8 p_number_of_param, long double l_params[], string::size_type l_comma_count, const string::size_type l_correct_end)
{
	if(l_comma_count >= p_number_of_param)
	{
		AddError(10, l_start, p_number_of_param, l_comma_count + 1);
		return;
	}
	string::size_type l_count;
	if(l_buf.c_str()[0] != '-')
	{
		l_count = l_buf.find_first_not_of("0123456789.e", 0);
	}
	else
	{
		l_count = l_buf.find_first_not_of("0123456789.e", 1);
	}
	if(l_count != string::npos)
	{
		AddMessage(4);
		string l_outp;
		PreparingForFunction(l_buf);
		CalculateLineExpression(l_buf, l_outp); 
		l_buf = "";
		AddMessage(5);
		if(l_outp.c_str()[0] != '-')
		{
			l_count = l_outp.find_first_not_of("0123456789.e", 0);
		}
		else
		{
			l_count = l_outp.find_first_not_of("0123456789.e", 1);
		}
		if(l_count != string::npos || l_outp.empty())
		{
			AddError(18, l_start, l_comma_count + 1, l_correct_end);
			return;
		}
		l_params[l_comma_count] = atof(l_outp.c_str());
		return;
	}
	l_params[l_comma_count] = atof(l_buf.c_str());
	l_buf = "";
}
//---------------------------------------------------------------------------
void CalculateFunction(string& p_input_str, const string p_in, const uint p_count, const uint_8 p_number_of_param)
{
	string::size_type l_start;
	l_start = p_input_str.find(p_in);
	if(l_start == string::npos
		|| (l_start && (GetPriority(p_input_str.c_str()[l_start - 1]) < priority_bracket
		|| GetPriority(p_input_str.c_str()[l_start - 1]) == priority_error))
		|| p_in.empty()
		)
		return;

	string::size_type l_end, l_br_start;
	char l_char_buf[320];
	long double l_params[c_max_argument_of_function];
	do
	{
		string l_temp;
		string l_buf = p_input_str.substr(l_start + p_in.size());
		string::size_type l_current_comma, l_comma_count = 0;
		uint l_nesting_level = 0;
		for(string::size_type l_correct_end = 0;;)
		{
			l_br_start = l_buf.find("(");
			l_end = l_buf.find(")");
			l_current_comma = l_buf.find(",");
			if(l_br_start != string::npos && l_br_start < l_end && l_current_comma > l_br_start)
			{
				l_correct_end += l_br_start + 1;
				l_temp += l_buf.substr(0, l_br_start + 1);
				l_buf = l_buf.substr(l_br_start + 1);
				l_nesting_level++;
			}
			else if(!l_nesting_level)
			{
				if(l_current_comma != string::npos && l_current_comma < l_end)
				{
					l_temp += l_buf.substr(0, l_current_comma);
					l_correct_end += l_current_comma + 1;
					l_buf = l_buf.substr(l_current_comma + 1);
					if(l_temp.empty())
					{
						AddError(18, l_start, l_comma_count + 1, l_correct_end);
						return;
					}
					GetParametrs(l_temp, l_start, p_number_of_param, l_params, l_comma_count, l_correct_end);
					l_comma_count++;
				}
				else if(l_current_comma == string::npos || l_current_comma > l_end)
				{
					l_temp += l_buf.substr(0, l_end);
					if(l_temp.empty())
					{
						AddError(18, l_start, l_comma_count + 1, l_correct_end);
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
		if(l_comma_count != p_number_of_param - 1)
		{
			AddError(10, l_start, p_number_of_param, l_comma_count + 1);
			return;
		}
		long double result = CalculateParametrs(p_number_of_param, p_count, l_params);
		string::size_type l_c;
		try
		{
			l_c = sprintf_s(l_char_buf, "%.25lf", result);
		}
		catch(...)
		{
#ifdef _DEBUG
			AddError(253);
			return;
#endif
		}
		l_temp = l_char_buf;
		if(m_NoError)
			m_Correct_count += l_c - (p_in.size() + l_end + 1 - l_start);

		p_input_str.erase(l_start, p_in.size() + l_end + 1);
		p_input_str.insert(l_start, l_temp);

		l_start = p_input_str.find(p_in);
		if(l_start == string::npos
			|| (l_start && (GetPriority(p_input_str.c_str()[l_start - 1]) < priority_bracket
			|| GetPriority(p_input_str.c_str()[l_start - 1]) == priority_error))
			|| p_in.empty()
			)
			return;
	}
	while(true);
}
//---------------------------------------------------------------------------
void PreparingForFunction(string& p_input_str)
{
	if(m_NoError)
	{
		AddMessage(7);
		uint i = 0;
		for(; i < _countof(c_function1); i++)
			CalculateFunction(p_input_str, c_function1[i], i, 1);

		i = 0;
		for(; i < _countof(c_function2); i++)
			CalculateFunction(p_input_str, c_function2[i], i, 2);
#ifdef _DEBUG_FUNCTION // TODO: delete this block after add full function support
		i = 0;
		for(; i < _countof(c_function3); i++)
			CalculateFunction(p_input_str, c_function3[i], i, 3);

		i = 0;
		for(; i < _countof(c_function5); i++)
			CalculateFunction(p_input_str, c_function5[i], i, 5);
#endif
		AddMessage(p_input_str.c_str());
	}
	if(m_NoError)
	{
		string::size_type l_count = 0;
		for(; l_count < p_input_str.size() - 1; l_count++)
		{
			if(GetPriority(p_input_str.c_str()[l_count]) == priority_default
				&& p_input_str.c_str()[l_count + 1] == '(')
			{
				AddError(30, l_count - 1);
			}
		}
		l_count = p_input_str.find_first_not_of("0123456789+-*/^.e()", 0);
		if(l_count != string::npos)
		{
			AddError(2, l_count);
		}
	}
}
//---------------------------------------------------------------------------
#endif // _FUNCTION_CPP