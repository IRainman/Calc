
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
#include "Function.h"
#include "Message.h"
#include "RPN.h"
#include <list>
//---------------------------------------------------------------------------
list <const string> c_constant;
//---------------------------------------------------------------------------
inline void ReplaceConstant(string& p_input_str, const string p_in, const uint_8 p_number_of_constant)
{
	string::size_type c = p_input_str.find(p_in);
	if(c == string::npos
		||
		(c
		&& (GetPriority(p_input_str.c_str()[c - 1]) < priority_bracket
		|| GetPriority(p_input_str.c_str()[c - 1]) == priority_error)
		)
		||
		((p_input_str.length() - p_in.length()) > 0
		&& (GetPriority(p_input_str.c_str()[c + p_in.length()]) < priority_bracket
		|| GetPriority(p_input_str.c_str()[c + p_in.length()]) == priority_error)
		)
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
		m_Correct_count -= l_const.length() + p_in.length();

		c = p_input_str.find(p_in);
		if(c == string::npos
			||
			(c
			&& (GetPriority(p_input_str.c_str()[c - 1]) < priority_bracket
			|| GetPriority(p_input_str.c_str()[c - 1]) == priority_error)
			)
			||
			((p_input_str.length() - p_in.length()) > 0
			&& (GetPriority(p_input_str.c_str()[c + p_in.length()]) < priority_bracket
			|| GetPriority(p_input_str.c_str()[c + p_in.length()]) == priority_error)
			)
			)
			return;
	}
	while(true);
}
//---------------------------------------------------------------------------
void ValidateInputStringAndConstantReplace(string& p_input_str)
{
	AddMessage(0);
	string::size_type count_inp = 0, count_outp = 0;
	string::size_type l_count = 0;
	if(GetPriority(p_input_str.c_str()[0]) > priority_bracket && p_input_str.c_str()[0] != '-')
	{
		AddError(9);
	}
	if(p_input_str.c_str()[0] == 'e')
	{
		AddError(23, 0);
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
		else if(p_input_str.c_str()[l_count] == '.')
		{
			string::size_type l_nextOperator = p_input_str.find_first_of("-+/*^()", l_count + 1);
			string::size_type l_nextPt = p_input_str.find(".", l_count + 1);
			if(l_nextPt != string::npos && (l_nextOperator == string::npos || l_nextPt < l_nextOperator))
			{
				AddError(20, l_count, l_nextPt);
			}
		}
		else if(p_input_str.c_str()[l_count] == 'e')
		{
			string::size_type l_nextOperator = p_input_str.find_first_of("-+/*^()", l_count + 1);
			string::size_type l_nextE = p_input_str.find("e", l_count + 1);
			if(l_nextE != string::npos && (l_nextOperator == string::npos || l_nextE < l_nextOperator))
			{
				AddError(22, l_count, l_nextE);
			}
			if(GetPriority(p_input_str.c_str()[l_count - 1]) > priority_default)
			{
				AddError(23, l_count - 1);
			}
			if(GetPriority(p_input_str.c_str()[l_count + 1]) != priority_default
				&& GetPriority(p_input_str.c_str()[l_count + 1]) != priority_addition)
			{
				AddError(25, l_count + 1);
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
	l_count = p_input_str.find("e--");
	if(l_count != string::npos)
	{
		AddError(24, l_count + 1);
	}
	if(m_NoError)
	{
		uint j = 0;
		AddMessage(6);
		for(list <const string>::iterator i = c_constant.begin(); i != c_constant.end(); i++, j++)
			ReplaceConstant(p_input_str, i->c_str(), j);
		AddMessage(p_input_str.c_str());
	}
}
//---------------------------------------------------------------------------
wstring& Calculate(wstring p_input, wstring& p_output)
{
	if(c_constant.empty())
	{
		c_constant.push_back("pi");
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

	l_count = p_input.find_first_not_of(L"0123456789+-*/^.e(),powsincosexptanacosasinatanloglog10", 0);
	if(l_count != string::npos)
	{
		AddError(0, l_count);
	}

	l_input_str.assign(p_input.begin(), p_input.end());

	ValidateInputStringAndConstantReplace(l_input_str);
	if(m_NoError)
	{
		PreparingForFunction(l_input_str);
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
	}
	return m_ErrorString;
}
//---------------------------------------------------------------------------
#endif // _CALCULATOR_CPP