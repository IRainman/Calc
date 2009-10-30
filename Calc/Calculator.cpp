
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
#ifdef _USE_Function
//list <string> c_function1; // TODO
list <string> c_function2;
#define c_max_argument_of_function 2
#endif //_USE_Function
//---------------------------------------------------------------------------
#ifdef _USE_Function
inline void Recursive(string& l_buf, string::size_type c, double l_params[], uint_8 l_count)
{
	AddMessage(4);
	string l_inp = l_buf.substr(0, c);
	string l_outp;
	ValidateAndPrepareInputString(l_inp);
	CalculateLineExpression(l_inp, l_outp); 
	sscanf_s(l_outp.c_str(), "%lf", &l_params[l_count]);
	AddMessage(5);
}
//---------------------------------------------------------------------------
void CalculateFunction(string& p_input_str, string p_in, uint p_count, uint_8 p_number_of_param)
{
	string::size_type l_start, l_end, l_br_start;
	string l_buf;
	string l_temp;
	char l_char_buf[320];
	double l_params[c_max_argument_of_function];
	while(true)
	{
		l_start = p_input_str.find(p_in);
		if(l_start == string::npos)
			return;

		l_buf = p_input_str.substr(l_start + p_in.size());
		l_temp = l_buf;
		//string::size_type l_correct_comma = 0; ???????????
		for(string::size_type l_nesting_level = 0, l_correct_end = 0;;)
		{
			l_br_start = l_temp.find("(");
			l_end = l_temp.find(")");
			//l_correct_comma += ??????
			if(l_br_start != string::npos)
			{
				l_correct_end += l_br_start;
				l_temp = l_temp.substr(l_br_start + 1);
				if(l_br_start < l_end)
				{
					l_nesting_level++;
				}
			}
			if(!l_nesting_level)
			{
				l_end += l_correct_end;
				break;
			}
			if(l_end != string::npos)
			{
				l_nesting_level--;
				l_correct_end += l_end;
				l_temp = l_temp.substr(l_end - l_br_start + 1);
			}
			else
			{
				AddError(6, l_start);
			}
		}
		if(l_end == string::npos) // !!! Uncoment this after refactoring function
		{
			AddError(6, l_start);
			return;
		}
		l_buf = l_buf.substr(0, l_end);
		if(l_buf.empty())
		{
			AddError(7, l_start);
			return;
		}
		string::size_type c;
		uint_8 l_count = 0;
		string::size_type l_scanf_count;
		while(true)
		{
			c = l_buf.find(","); // косяк !!!!!!!!!!!!!!!!!!1
			if(c != string::npos)
			{
				l_scanf_count = sscanf_s(l_buf.c_str(), "%lf", &l_params[l_count]);
				if(l_scanf_count != c)
				{
					Recursive(l_buf, c, l_params, l_count);
				}
				else
				{
					l_scanf_count = sscanf_s(l_buf.c_str(), "%lf", &l_params[l_count]);
					if(!l_scanf_count)
					{
						AddError(18, l_start, l_count + 1, c + 1);
					}
				}
				l_buf.erase(0, c + 1);
				l_count++;
			}
			else
				break;
		}
		l_scanf_count = sscanf_s(l_buf.c_str(), "%lf", &l_params[l_count]);
		if(l_scanf_count != l_buf.size())
		{
			Recursive(l_buf, c, l_params, l_count);
		}
		if(!l_buf.size() || !l_scanf_count)
		{
			AddError(18, l_start, l_count + 1, c + 1);
		}
		if(l_count != p_number_of_param - 1)
		{
			AddError(10, l_start, p_number_of_param, l_count + 1);
			return;
		}
		double result;
		//string::size_type l_c; todo
		switch(p_number_of_param)
		{
			case 2:
				switch(p_count)
				{
					case 0:
							result = pow(l_params[0], l_params[1]);
						break;
				}
				break;
#ifdef _DEBUG
			default:
				AddError(10, l_start, p_number_of_param, l_count + 1);
				return;
#endif
		}
		try
		{
			/*l_c =*/ sprintf_s(l_char_buf, "%lf", result);
		}
		catch(...)
		{
#ifdef _DEBUG
			AddError(253);
			return;
#endif
		}
		p_input_str.erase(l_start, l_start + l_end + 1 + p_in.size());
		l_temp = l_char_buf;
		//l_temp = l_temp.substr(0, l_c);
		p_input_str.insert(l_start, l_temp);
	}
}
//---------------------------------------------------------------------------
#endif // _USE_Function
void ValidateAndPrepareInputString(string& p_input_str)
{
	uint count_inp = 0, count_outp = 0;
	string::size_type c;

	while(true)
	{
		c = p_input_str.find(" ");
		if(c != string::npos)
			p_input_str.erase(c, 1);
		else
			break;
	}
	string::size_type l_count = 0;
	for(; l_count < p_input_str.size(); l_count++)
	{
		p_input_str[l_count] = tolower(p_input_str.c_str()[l_count]);
		if( (p_input_str.c_str()[l_count] >= '(' && p_input_str.c_str()[l_count] <= '9') ||
			(p_input_str.c_str()[l_count] >= 'a' && p_input_str.c_str()[l_count] <= 'z') ||
			 p_input_str.c_str()[l_count] == '^' )
		{
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
		}
		else
		{
			AddError(0, l_count);
		}
	}
	if(count_inp != count_outp)
	{
		AddError(1, -1, count_inp, count_outp);
	}
#ifdef _USE_Function
	if(m_NoError)
	{
		uint j = 0;
/*		for(list <string>::iterator i = c_function1.begin(); i != c_function1.end(); i++, j++) // TODO
			CalculateFunction(p_input_str, i->c_str(), j, 1);
*/
		j = 0;
		for(list <string>::iterator i = c_function2.begin(); i != c_function2.end(); i++, j++)
			CalculateFunction(p_input_str, i->c_str(), j, 2);
	}
#endif // _USE_Function
	if(m_NoError)
	{
		for(l_count = 0; l_count < p_input_str.size(); l_count++)
		{
			if((p_input_str.c_str()[l_count] >= 'a' && p_input_str.c_str()[l_count] <= 'z')
#ifdef _DEBUG
				|| p_input_str.c_str()[l_count] == ','
#endif
				)
			{
				AddError(2, l_count);
			}
		}
	}
}
//---------------------------------------------------------------------------
wstring& Calculate(wstring p_input, wstring& p_output)
{
#ifdef _USE_Function
	/*if(c_function1.empty())// TODO
	{
		c_function1.push_back("sin(");
		c_function1.push_back("cos(");
	}*/
	if(c_function2.empty())
	{
		c_function2.push_back("pow(");
	}
	m_Correct_count = 0;
#endif // _USE_Function
	m_ErrorString = L"";
	m_NoError = true;

	string l_input_str, l_output_str;
// TODO убрать сей костыль :)
	
	for(string::size_type l_count = 0; l_count < p_input.size(); l_count++)
	{
		p_input[l_count] = tolower(p_input.c_str()[l_count]);
		if( (p_input.c_str()[l_count] >= '(' && p_input.c_str()[l_count] <= '9') ||
			(p_input.c_str()[l_count] >= 'a' && p_input.c_str()[l_count] <= 'z') ||
			 p_input.c_str()[l_count] == '^' || p_input.c_str()[l_count] == ' '/* при переносе проверку на пробел убрать*/)
		{
			// void :)
		}
		else
		{
			AddError(0, l_count);
		}
	}
//  ----- end of костыль
	l_input_str.assign(p_input.begin(), p_input.end());

	AddMessage(0);
	ValidateAndPrepareInputString(l_input_str);
	if(m_NoError)
	{
		AddMessage(l_input_str);
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