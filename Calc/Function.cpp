
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
#include <list>
//---------------------------------------------------------------------------
void PreparingForFunction(string& p_input_str);
//---------------------------------------------------------------------------
#define c_max_argument_of_function 3
list <const string> c_function1;
list <const string> c_function2;
list <const string> c_function3;
//---------------------------------------------------------------------------
inline double CalculateParametrs(const uint_8 p_number_of_param, const uint p_count, double p_params[])
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
				/*case 9:
					return xxx(p_params[0]);
				case 10:
					return xxx(p_params[0]);*/
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
		case 3:// FOR TESTS ONLY, its not real function
			switch(p_count)
			{
				case 0:
					return p_params[0] + p_params[1] + p_params[2];
				case 1:
					return p_params[0] + p_params[1] + p_params[2];
#ifdef _DEBUG
				default:
					AddError(253);
					return 0;
#endif
			}
			break;
#ifdef _DEBUG
		default:
			AddError(253);
			return 0;
#endif
	}
	// TODO delete this block after add full function support
		m_NoError = false;
		return 0;
}
//---------------------------------------------------------------------------
void GetParametrs(string& l_buf, const string::size_type l_start, const uint_8 p_number_of_param, double l_params[], string::size_type l_comma_count, const string::size_type l_correct_end)
{
	if(l_comma_count >= p_number_of_param)
	{
		AddError(10, l_start, p_number_of_param, l_comma_count);
		return;
	}
	string::size_type l_count = l_buf.find_first_not_of("0123456789.e", 0);
	if(l_count != string::npos)
	{
		AddMessage(4);
		string l_outp;
		PreparingForFunction(l_buf);
		CalculateLineExpression(l_buf, l_outp); 
		l_buf = "";
		AddMessage(5);
		l_count = l_outp.find_first_not_of("0123456789.e", 0);
		if(l_count != string::npos || l_outp.empty())
		{
			AddError(18, l_start, l_comma_count, l_correct_end);
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
		|| GetPriority(p_input_str.c_str()[l_start - 1]) == priority_error)))
		return;

	string::size_type l_end, l_br_start;
	char l_char_buf[320];
	double l_params[c_max_argument_of_function];
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
			else if(l_current_comma != string::npos && l_current_comma < l_end && !l_nesting_level)
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
			else if(l_current_comma == string::npos || l_current_comma > l_end && !l_nesting_level)
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
			else
			{
				l_correct_end += l_end + 1;
				l_temp += l_buf.substr(0, l_end + 1);
				l_buf = l_buf.substr(l_end + 1);
				l_nesting_level--;
			}
		}
		if(l_end == string::npos)// WTF ?
		{
			AddError(6, l_start); // todo убрать если не будет появлятся
			m_NoError = true; // !!!!! 
		//	return; ?????? 
		}
		if(l_comma_count != p_number_of_param - 1)
		{
			AddError(10, l_start, p_number_of_param, l_comma_count);
			return;
		}
		double result = CalculateParametrs(p_number_of_param, p_count, l_params);
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
		m_Correct_count -= l_c + (p_in.size() + l_end + 1 - l_start);
		p_input_str.erase(l_start, p_in.size() + l_end + 1);
		p_input_str.insert(l_start, l_temp);

		l_start = p_input_str.find(p_in);
		if(l_start == string::npos)
			return;
	}
	while(true);
}
//---------------------------------------------------------------------------
void PreparingForFunction(string& p_input_str)
{
	if(c_function1.empty())
	{
		c_function1.push_back("sin(");
		c_function1.push_back("cos(");
		c_function1.push_back("exp(");
		c_function1.push_back("tan(");
		c_function1.push_back("acos(");
		c_function1.push_back("asin(");
		c_function1.push_back("atan(");
		c_function1.push_back("log(");
		c_function1.push_back("log10(");
		/*c_function1.push_back("ceil(");
		c_function1.push_back("fabs(");
		c_function1.push_back("floor(");
		c_function1.push_back("ldexp(");
		c_function1.push_back("modf(");*/
	}
	if(c_function2.empty())
	{
		c_function2.push_back("pow(");
		/*c_function2.push_back("atan2(");
		c_function2.push_back("modf(");
		c_function2.push_back("fmod(");
		c_function2.push_back("frexp(");*/
	}
	if(c_function3.empty()) // FOR TESTS ONLY, its not real function
	{
		c_function3.push_back("plus(");
		c_function3.push_back("spin(");
	}
	if(m_NoError)
	{
		AddMessage(7);
		uint j = 0;
		for(list <const string>::iterator i = c_function1.begin(); i != c_function1.end(); i++, j++)
			CalculateFunction(p_input_str, i->c_str(), j, 1);

		j = 0;
		for(list <const string>::iterator i = c_function2.begin(); i != c_function2.end(); i++, j++)
			CalculateFunction(p_input_str, i->c_str(), j, 2);

		j = 0;
		for(list <const string>::iterator i = c_function3.begin(); i != c_function3.end(); i++, j++)
			CalculateFunction(p_input_str, i->c_str(), j, 3);
		AddMessage(p_input_str.c_str());
	}
	if(m_NoError)
	{
		string::size_type l_count = p_input_str.find_first_not_of("0123456789+-*/^.e()", 0);
		if(l_count != string::npos)
		{
			AddError(2, l_count);
		}
	}
}
//---------------------------------------------------------------------------
#endif // _FUNCTION_CPP