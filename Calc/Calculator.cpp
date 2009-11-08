
#ifdef _MSC_VER
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
#include "Message.h"
#include "RPN.h"
#include <list>
//---------------------------------------------------------------------------
void ValidateAndPrepareInputString(string& p_input_str);
//---------------------------------------------------------------------------
#define c_max_argument_of_function 3
list <const string> c_function1;
list <const string> c_function2;
list <const string> c_function3;
//---------------------------------------------------------------------------
list <const string> c_constant;
//---------------------------------------------------------------------------
inline int Recursive(string& l_buf, const string::size_type c, double l_params[], const string::size_type l_count)
{
	AddMessage(4);
	string l_inp = l_buf.substr(0, c);
	string l_outp;
	ValidateAndPrepareInputString(l_inp);
	CalculateLineExpression(l_inp, l_outp); 
	l_buf.erase(0, c + 1);
	AddMessage(5);
	return sscanf_s(l_outp.c_str(), "%lf,", &l_params[l_count]);
}
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
		case 3:
			switch(p_count)
			{
				case 0:
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

	{// TODO delete this block after full function support
		m_NoError = false;
		return 0;
	}
}
//---------------------------------------------------------------------------
void CalculateFunction(string& p_input_str, const string p_in, const uint p_count, const uint_8 p_number_of_param)
{
	string::size_type l_start;
	l_start = p_input_str.find(p_in);
	if(l_start == string::npos)
		return;

	string::size_type l_end, l_br_start;
	string l_buf;
	char l_char_buf[320];
	double l_params[c_max_argument_of_function];
	do
	{
		l_buf = p_input_str.substr(l_start + p_in.size());
		string::size_type l_comma[c_max_argument_of_function], l_current_comma, l_comma_count = 0;
		uint l_nesting_level = 0;
		for(string::size_type l_correct_end = 0;;)
		{
			l_br_start = l_buf.find("(");
			l_end = l_buf.find(")");
			l_current_comma = l_buf.find(",");
			if(l_br_start != string::npos && l_br_start < l_end && l_current_comma > l_br_start)
			{
				l_correct_end += l_br_start + 1;
				l_buf = l_buf.substr(l_br_start + 1);
				l_nesting_level++;
			}
			else if(l_current_comma != string::npos && l_current_comma < l_end && !l_nesting_level)
			{
				l_comma[l_comma_count] = l_current_comma;
				l_correct_end += l_current_comma + 1;
				l_buf = l_buf.substr(l_current_comma + 1);
				l_comma_count++;
			}
			else if(l_current_comma == string::npos || l_current_comma > l_end)
			{
				if(!l_nesting_level)
				{
					l_end += l_correct_end;
					break;
				}
			}
			if(l_end != string::npos && l_nesting_level)
			{
				l_correct_end += l_end + 1;
				l_buf = l_buf.substr(l_end + 1);
				l_nesting_level--;
			}
			/*if(l_end != string::npos)
			{
				l_nesting_level--;
				l_correct_position += l_end + 1;
			}
			else
			{
				AddError(6, l_start);
			}*/
		}
		if(l_end == string::npos)
		{
			AddError(6, l_start);
			return;
		}
		l_buf = p_input_str.substr(l_start + p_in.size(), l_end); // не трогать строку!!! если здесь не сходится, ошибка в алгоритме выше :)
		if(l_buf.empty())
		{
			AddError(7, l_start);
			return;
		}
		l_comma[l_comma_count++] = l_buf.size();
		l_buf += ",";
		if(l_comma_count != p_number_of_param)
		{
			AddError(10, l_start, p_number_of_param, l_comma_count);
			return;
		}
		l_current_comma = 0;
		for(int l_scanf_count; l_current_comma < l_comma_count; l_current_comma++)
		{
				l_scanf_count = sscanf_s(l_buf.c_str(), "%lf,", &l_params[l_current_comma]);
				if(l_buf.c_str()[0] == ',')
				{
					AddError(18, l_start, l_current_comma + 1, l_comma[l_current_comma]);
					return;
				}
				if(!l_scanf_count)
				{
					l_scanf_count = Recursive(l_buf, l_comma[l_current_comma], l_params, l_current_comma);
				}
				else
				{
					l_buf.erase(0, l_comma[l_current_comma] + 1);
				}
				if(!l_scanf_count)
				{
					AddError(18, l_start, l_current_comma + 1, l_comma[l_current_comma]);
					return;
				}
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
		string l_temp = l_char_buf;
		l_temp = l_temp.substr(0, l_c);
		m_Correct_count += (p_in.size() + l_end + 1 - l_start) - l_c;
		p_input_str.erase(l_start, p_in.size() + l_end + 1);
		p_input_str.insert(l_start, l_temp);

		l_start = p_input_str.find(p_in);
		if(l_start == string::npos)
			return;
	}
	while(true);
}
//---------------------------------------------------------------------------
inline void ReplaceConstant(string& p_input_str, const string p_in, const uint_8 p_number_of_constant)
{
	string::size_type c = p_input_str.find(p_in);
	if(c == string::npos
		//|| (GetPriority(p_input_str.c_str()[c - 1]) < priority_bracket || GetPriority(p_input_str.c_str()[c - 1]) == priority_error)
		//|| (GetPriority(p_input_str.c_str()[c + 1]) < priority_bracket || GetPriority(p_input_str.c_str()[c + 1]) == priority_error)
		)
		return;

	char l_char_buf[100];
	double l_constant;
	string l_const;
	switch(p_number_of_constant)
	{
		case 0:
			l_constant = M_PI;
			break;
#ifdef _DEBUG
		default:
			AddError(252);
			return;
#endif
	}
	sprintf_s(l_char_buf, "%.25lf", l_constant);
	l_const = l_char_buf;
	do
	{
		p_input_str.erase(c, p_in.length());
		p_input_str.insert(c, l_const);
		m_Correct_count += l_const.length() - p_in.length();

		c = p_input_str.find(p_in);
		if(c == string::npos
			//|| (GetPriority(p_input_str.c_str()[c - 1]) < priority_bracket || GetPriority(p_input_str.c_str()[c - 1]) == priority_error)
			//|| (GetPriority(p_input_str.c_str()[c + 1]) < priority_bracket || GetPriority(p_input_str.c_str()[c + 1]) == priority_error)
			)
			return;
	}
	while(true);
}
//---------------------------------------------------------------------------
void ValidateAndPrepareInputString(string& p_input_str)
{
	AddMessage(0);
	string::size_type count_inp = 0, count_outp = 0;
	string::size_type l_count = 0;
	if(GetPriority(p_input_str.c_str()[0]) > priority_bracket && p_input_str.c_str()[0] != '-')
	{
		AddError(9);
	}
	for(; l_count < p_input_str.size(); l_count++)
	{
		p_input_str[l_count] = tolower(p_input_str.c_str()[l_count]);
		if(p_input_str.c_str()[l_count] == '(')
		{
			if(p_input_str.c_str()[l_count - 1] == ')')
			{
				AddError(14, l_count - 1);
			}
			else if(GetPriority(p_input_str.c_str()[l_count + 1]) > priority_bracket &&
				p_input_str.c_str()[l_count + 1] != '-')
			{
				AddError(17, l_count + 1);
			}
			count_inp++;
		}
		else if(p_input_str.c_str()[l_count] == ')')
		{
			if(p_input_str.c_str()[l_count - 1] == '(')
			{
				AddError(13, l_count - 1);
			}
			else if(GetPriority(p_input_str.c_str()[l_count - 1]) > priority_bracket)
			{
				AddError(16, l_count - 1);
			}
			else if(GetPriority(p_input_str.c_str()[l_count + 1]) == priority_default)
			{
				AddError(15, l_count + 1);
			}
			else if(p_input_str.c_str()[l_count + 1] >= 'a' && p_input_str.c_str()[l_count + 1] <= 'z')
			{
				AddError(19, l_count + 1);
			}
			count_outp++;
		}
		else if(p_input_str.c_str()[l_count] == '.' && p_input_str.c_str()[l_count + 1] == '.')
		{
			AddError(20, l_count + 1);
		}
		else if(p_input_str.c_str()[l_count] == 'e')
		{
			if(p_input_str.c_str()[l_count + 1] == 'e')
			{
				AddError(22, l_count + 1);
			}
			if(GetPriority(p_input_str.c_str()[l_count - 1]) > priority_default)
			{
				AddError(23, l_count - 1);
			}
		}
		else if((p_input_str.c_str()[l_count] == '+' || p_input_str.c_str()[l_count] == '-'
			|| p_input_str.c_str()[l_count] == '/' || p_input_str.c_str()[l_count] == '*'
			|| p_input_str.c_str()[l_count] == '^')

			&& (p_input_str.c_str()[l_count + 1] == '+' || p_input_str.c_str()[l_count + 1] == '*'
			|| p_input_str.c_str()[l_count + 1] == '/' || p_input_str.c_str()[l_count + 1] == '^'))
		{
			AddError(4, l_count + 1);
		}
	}
	if(count_inp != count_outp)
	{
		AddError(1, -1, count_inp, count_outp);
	}
	if(p_input_str[p_input_str.length() - 1] == '-' || p_input_str[p_input_str.length() - 1] == '+'
		|| p_input_str[p_input_str.length() - 1] == '*' || p_input_str[p_input_str.length() - 1] == '/'
		|| p_input_str[p_input_str.length() - 1] == '^')
	{
		AddError(21, p_input_str.length());
	}
	if(m_NoError)
	{
		AddMessage(6);
		uint j = 0;
		for(list <const string>::iterator i = c_constant.begin(); i != c_constant.end(); i++, j++)
			ReplaceConstant(p_input_str, i->c_str(), j);
		AddMessage(p_input_str.c_str());

		AddMessage(7);
		j = 0;
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
		l_count = p_input_str.find_first_not_of("0123456789+-*/^.e()", 0);
		if(l_count != string::npos)
		{
			AddError(2, l_count);
		}
	}
}
//---------------------------------------------------------------------------
wstring& Calculate(wstring p_input, wstring& p_output)
{
	if(c_constant.empty())
	{
		c_constant.push_back("pi");
	}
	if(c_function1.empty())
	{
		c_function1.push_back("sin(");
		c_function1.push_back("cos(");
		c_function1.push_back("exp(");
	}
	if(c_function2.empty())
	{
		c_function2.push_back("pow(");
	}
	if(c_function3.empty()) // FOR TESTS ONLY!!!
	{
		c_function3.push_back("plus(");
		c_function3.push_back("spin(");
	}
	m_Correct_count = 0;
	m_ErrorString = L"";
	m_NoError = true;

	string l_input_str, l_output_str;
/* Not allowed spaces on this expression
	while(true)
	{
		wstring::size_type c = p_input.find(L" ");
		if(c != wstring::npos)
			p_input.erase(c, 1);
		else
			break;
	}
*/
	string::size_type l_count = 0;
	for(string::size_type l_count = 0; l_count < p_input.size(); l_count++)
	{
		p_input[l_count] = tolower(p_input.c_str()[l_count]);
	}

	l_count = p_input.find_first_not_of(L"0123456789+-*/^.e(),powsincosplusexp", 0);
	if(l_count != string::npos)
	{
		AddError(0, l_count);
	}

	l_input_str.assign(p_input.begin(), p_input.end());

	ValidateAndPrepareInputString(l_input_str);
	if(m_NoError)
	{
		CalculateLineExpression(l_input_str, l_output_str);
		#ifdef _USE_RPN
		if(m_NoError)
		{
			AddMessage(1, l_output_str);
			CalculateRPN(l_output_str);
		}
		#endif //_USE_RPN
		if(m_NoError)
		{
			p_output.assign(l_output_str.begin(), l_output_str.end());
		}
		else
		{
			p_output = L"";
		}
	}
	return m_ErrorString;
}
//---------------------------------------------------------------------------
#endif // _CALCULATOR_CPP