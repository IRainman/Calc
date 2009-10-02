
#ifdef _MSC_VER
	#include "stdafx.h"
/*#elif __INTEL_COMPILER
	// TODO */

// #endif // The comment is used in a silly Visual Studio :)
//---------------------------------------------------------------------------
#ifndef _CALCULATOR_CPP
#define _CALCULATOR_CPP
//---------------------------------------------------------------------------
#include "Calculator.h"
#include <string>
//TODO #define _USE_MATH_DEFINES
#include <math.h>
#include <stack>
#include <list>
#include "MyTypes.h"
//---------------------------------------------------------------------------
static list <string::size_type> c_coordinate;
//---------------------------------------------------------------------------
static wstring m_ErrorString;
//---------------------------------------------------------------------------
static const uint_8 priority_function =	16;
static const uint_8 priority_power =	8;
static const uint_8 priority_multiply =	4;
static const uint_8 priority_addition =	2;
static const uint_8 priority_bracket =	1;
static const uint_8 priority_default =	0;
static const uint_8 priority_error =	-1;
//---------------------------------------------------------------------------
void AddMessage(uint_8 p_message, const string& p_string_message)
{
	switch(p_message)
	{
		case 0:
			m_ErrorString += L"\tПодготовка:\r\n";
			break;
		case 1:
			m_ErrorString += L"\tСоздание RPN:\r\n";
		case 2:
			break;
#ifdef _DEBUG
		default:
			m_ErrorString += L" DEBUG: Unknown Message!";
			break;
#endif
	}
	wstring l_message;
	l_message.assign(p_string_message.begin(), p_string_message.end());
	static wchar_t l_wchar_buf[1024];
	swprintf_s(l_wchar_buf, L"%s", l_wchar_buf);
	m_ErrorString += l_wchar_buf;
}
//---------------------------------------------------------------------------
void AddWarning(uint_8 p_message)
{
	switch(p_message)
	{
		case 0:
			m_ErrorString += L"Внимание: аргументы операции x^y будут приведены к целочисленному типу. Для вещественных типов используйте функцию pow(x,y)";
			break;
#ifdef _DEBUG
		default:
			m_ErrorString += L"DEBUG: Unknown Warning!";
			break;
#endif
	}
	m_ErrorString += L"\r\n";
}
//---------------------------------------------------------------------------
void AddError(uint_8 p_message, string::size_type p_count = -1, string::size_type p_1 = 0, string::size_type p_2 = 0)
{
	static wstring l_error;
	static wchar_t l_1[6];
	_itow_s(p_1, l_1, 10);
	static wchar_t l_2[6];
	_itow_s(p_2, l_2, 10);
	switch(p_message)
	{
		case 0:
			l_error = L"Недопустимый символ";
			break;
		case 1:
			l_error = L"Количество открывающих " + (wstring)l_1 +
				L" и закрывающих " + (wstring)l_2 +
				L" скобок не совпадает";
			break;
		case 2:
			l_error = L"Недопустимый символ после проверки на функции, проверьте правильность их написания";
			break;
		case 3:
			l_error = L"Недопустимый символ во время преобразования в обратную польскую запись";
			break;
		case 4:
			l_error = L"Последовательная запись нескольких операций не подерживается";
			break;
		case 5:
			l_error = L"На ноль делить нельзя";
			break;
		case 6:
			l_error = L"У функции нехватает закрывающей скобки";
			break;
		case 7:
			l_error = L"У функции отсутсвуют аргументы";
			break;
		case 8:
			l_error = L"Недостаточно операндов для получения результата";
			break;
		case 9:
			l_error = L"Выражение не может начинатся со знака операции";
			break;
		case 10:
			l_error = L"У функции неверное число аргументов необходимо " + (wstring)l_1 + L", обнаружено " + (wstring)l_2;
			break;
#ifdef _DEBUG
		case 255:
			l_error = L"DEBUG: number of arguments passed to the function \"replace\" is not defined in the function of \"replace\", it is impossible to calculate the expression";
			break;
		default:
			l_error = L"DEBUG: Unknown Error!";
			break;
#endif
	}
	static wchar_t l_wchar_buf[1024];
	if(p_count == -1)
	{
		swprintf_s(l_wchar_buf, L"Ошибка: %s\r\n", l_error.c_str());
	}
	else
	{
		swprintf_s(l_wchar_buf, L"В позиции %d ошибка: %s\r\n", p_count, l_error.c_str());
	}
	m_ErrorString += l_wchar_buf;
}
//---------------------------------------------------------------------------
uint_8 GetPriority(char p_sym)
{
	switch(p_sym)
	{
		case 'P':
			return priority_function;

		case '^':
			return priority_power;

		case '*':
		case '/':
			return priority_multiply;

		case '+':
		case '-':
			return priority_addition;

		case '(':
		case ')':
			return priority_bracket;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '=':
		case '.':
		case ' ':
			return priority_default;

		default:
			return priority_error;
	}
}
//---------------------------------------------------------------------------
bool replace(string& p_input_str, string p_in, char p_out, uint_8 p_number_of_param)
{
	string::size_type l_start, l_end;
	string l_buf;
	static char l_char_buf[200];
	float l_a, l_b;
	while(true)
	{
		l_start = p_input_str.find(p_in);
		if(l_start == string::npos)
			return true;

		l_buf = p_input_str;
		l_buf.erase(0, l_start);

		l_end = l_buf.find(")");
		if(l_end == string::npos)
		{
			AddError(6, l_start);
			return false;
		}

		l_buf = l_buf.substr(p_in.size() + 1, l_end - p_in.size() - 1);
		if(l_buf.empty())
		{
			AddError(7, l_start);
			return false;
		}
		string::size_type c;
		uint_8 count = 0;
		while(true)
		{
			c = l_buf.find(",");
			if(c != string::npos)
			{
				l_buf.erase(c, 1);
				l_buf.insert(c, " ");
				count++;
			}
			else
				break;
		}
		if(count != p_number_of_param - 1)
		{
			AddError(10, l_start, p_number_of_param, count + 1);
			return false;
		}
		switch(p_number_of_param)
		{
			case 2:
				sscanf_s(l_buf.c_str(), "%f %f", &l_a, &l_b);
				sprintf_s(l_char_buf, " %f%c%f ", l_a, p_out, l_b);
				break;
#ifdef _DEBUG
			default:
				AddError(10, l_start, p_number_of_param, count + 1);
				return false;
#endif
		}
		p_input_str.erase(l_start, l_start + l_end + 1);
		p_input_str.insert(l_start, l_char_buf);
	}
}
//---------------------------------------------------------------------------
/* TODO
void replace(string& p_input_str, string p_in, string p_out)
{
	string::size_type l_start;
	while(true)
	{
		l_start = p_input_str.find(p_in);

		if(l_start == string::npos)
		{
			break;
		}
		else
		{
			p_input_str.erase(l_start, p_in.size() - l_start);
			p_input_str.insert(l_start, p_out);
		}
	}
}
*/
//---------------------------------------------------------------------------
bool ValidateAndPrepareInputString(string& p_input_str)
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
	for(string::size_type l_count = 0; l_count < p_input_str.size(); l_count++)
	{
		p_input_str[l_count] = tolower(p_input_str.c_str()[l_count]);
		if( (p_input_str.c_str()[l_count] >= '(' && p_input_str.c_str()[l_count] <= '9') ||
			(p_input_str.c_str()[l_count] >= 'a' && p_input_str.c_str()[l_count] <= 'z') ||
			 p_input_str.c_str()[l_count] == '^' )
		{
			// TODO переписать :) 
		}
		else
		{
			AddError(0, l_count);
			return false;
		}
	}

	bool l_function_ok = true;

	l_function_ok &= replace(p_input_str, "pow", 'P', 2);

	for(string::size_type l_count = 0; l_count < p_input_str.size(); l_count++)
	{
		if(p_input_str.c_str()[l_count] == '(')
		{
			count_inp++;
			continue;
		}
		if(p_input_str.c_str()[l_count] == ')')
		{
			count_outp++;
			continue;
		}
		if(p_input_str.c_str()[l_count] >= 'a' && p_input_str.c_str()[l_count] <= 'z')
		{
			AddError(2, l_count);
			return false;
		}
	}
	if(count_inp != count_outp)
	{
		AddError(1, -1, count_inp, count_outp);
		return false;
	}
	return l_function_ok;
}
//---------------------------------------------------------------------------
bool CalculateLineExpression(const string& p_input_str, string& p_output_str)
{
	uint_8 l_current_prioritet = priority_error, l_old_prioritet = priority_error;
	stack <char> c_operations;
	if(!c_coordinate.empty())
	{
		c_coordinate.erase(c_coordinate.begin());
	}
	string::size_type l_count = 0;
	if(GetPriority(p_input_str.c_str()[l_count]) > priority_bracket && p_input_str.c_str()[l_count] != '-')
	{
		AddError(9);
		return false;
	}
	for(; l_count < p_input_str.size(); l_count++)
	{
		l_current_prioritet = GetPriority(p_input_str.c_str()[l_count]);
		if(l_current_prioritet)
		{
			if(l_current_prioritet == priority_error)
			{
				AddError(3, l_count);
				return false;
			}
			if(l_current_prioritet == l_old_prioritet && p_input_str.c_str()[l_count] != '-')
			{
				AddError(4, l_count - 1);
				return false;
			}
			if(c_operations.empty())
			{
				/*
				a)	если стек пуст, то опеpация из входной стpоки пеpеписывается в стек;
				*/
				c_operations.push(p_input_str.c_str()[l_count]);
				p_output_str += " ";
			}
			else
			{
				if(l_current_prioritet != priority_bracket)
				{
					do
					{
						if(GetPriority(c_operations.top()) >= l_current_prioritet)
						{
							/*
							b)	опеpация выталкивает из стека все опеpации с большим или pавным пpиоpитетом в выходную стpоку;
							*/
							p_output_str += c_operations.top();
							c_coordinate.push_back(p_output_str.size() - 1);
							c_operations.pop();
							if(c_operations.empty())
							{
								c_operations.push(p_input_str.c_str()[l_count]);
								p_output_str += " ";
								break;
							}
						}
						else
						{
							c_operations.push(p_input_str.c_str()[l_count]);
							p_output_str += " ";
							break;
						}
					}
					while(!c_operations.empty());
				}
				else
				{
					if(p_input_str.c_str()[l_count] != ')')
					{
						/*
						c)	если очеpедной символ из исходной стpоки есть откpывающая скобка, то он пpоталкивается в стек;
						*/
						c_operations.push(p_input_str.c_str()[l_count]);
						p_output_str += " ";
					}
					else
					{
						while(!c_operations.empty())
						{
							/*
							d)	закpывающая кpуглая скобка выталкивает все опеpации из стека до ближайшей откpывающей скобки,
							сами скобки в выходную стpоку не пеpеписываются, а уничтожают дpуг дpуга. 
							*/
							if(GetPriority(c_operations.top()) == priority_bracket && c_operations.top() != ')')
							{
								c_operations.pop();
								break;
							}
							else
							{
								p_output_str += c_operations.top();
								c_coordinate.push_back(p_output_str.size() - 1);
								c_operations.pop();
								p_output_str += " ";
							}
						}
					}
				}
			}
		}
		else
		{
			p_output_str += p_input_str.c_str()[l_count];
		}
	}
	if(l_current_prioritet > priority_bracket)
	{
		AddError(8, l_count);
		return false;
	}
	while(!c_operations.empty())
	{
		p_output_str += c_operations.top();
		c_coordinate.push_back(p_output_str.size() + c_operations.size() - 2);
		c_operations.pop();
	}
	return true;
}
//---------------------------------------------------------------------------
bool TranslateToOutputString(string& p_to_process_str)
{
	string input = p_to_process_str;
	p_to_process_str = "";
	static char l_char_buf[200];
	float a, b, c;
	uint_8 count;
	string::size_type l_count = 0;

	count = sscanf_s(input.c_str(), "%f %f %f", &a, &b, &c);
	switch(count)
	{
		case 255:
			AddError(8);
			return false;
		case 1:
			if(!c_coordinate.empty())
			{
				sprintf_s(l_char_buf, "%c%f", input.c_str()[c_coordinate.front()], a);
				c_coordinate.pop_front();
			}
			else
			{
				sprintf_s(l_char_buf, "%f", a);
			}
			p_to_process_str = l_char_buf;
			return true;
		case 3:
			a=b;
			b=c;
			break;
	}
	if(!c_coordinate.empty())
	{
		input.erase(l_count, c_coordinate.front());
		c_coordinate.pop_front();
	}
	switch(input.c_str()[l_count])
	{
		case 'P':
			sprintf_s(l_char_buf, " %f ", pow(a,b));
			p_to_process_str += l_char_buf;
			break;
		case '^':
			if((int)a != a || (int)b != b)
			{
				AddWarning(0);
			}
			sprintf_s(l_char_buf, " %d ", (int)a^(int)b);
			p_to_process_str += l_char_buf;
			break;
		case '*':
			sprintf_s(l_char_buf, " %f ", a*b);
			p_to_process_str += l_char_buf;
			break;
		case '/':
			if(!b)
			{
				AddError(5);
				return false;
			}
			sprintf_s(l_char_buf, " %f ", a/b);
			p_to_process_str += l_char_buf;
			break;
		case '+':
			sprintf_s(l_char_buf, " %f ", a+b);
			p_to_process_str += l_char_buf;
			break;
		case '-':
			sprintf_s(l_char_buf, " %f ", a-b);
			p_to_process_str += l_char_buf;
			break;
		case ' ':
		case '.':
			break;
#ifdef _DEBUG
		default:
			p_to_process_str += "DEBUG: Internal Processing Error!";
			break;
#endif
	}
	return true;
}
//---------------------------------------------------------------------------
wstring& Calculate(wstring p_input, wstring& p_output)
{
	m_ErrorString = L"";
	bool l_No_Error = true;

	static string l_input_str, l_output_str;
	l_input_str.assign(p_input.begin(), p_input.end());
	//l_output_str = "";

	if(l_No_Error)
	{
		l_No_Error &= ValidateAndPrepareInputString(l_input_str);
		if(l_No_Error)
		{
			AddMessage(0, l_input_str);
			l_No_Error &= CalculateLineExpression(l_input_str, l_output_str);
			if(l_No_Error)
			{
				AddMessage(1, l_output_str);
				l_No_Error &= TranslateToOutputString(l_output_str);
				if(l_No_Error)
				{
					//AddMessage();
					string::size_type c;
					while(l_No_Error)
					{
						c = l_output_str.find(" ");
						if(c != string::npos)
							l_output_str.erase(c, 1);
						else
							break;
					}
				}
			}
		}
	}
	if(l_No_Error)
	{
		p_output.assign(l_output_str.begin(), l_output_str.end());
	}
	return m_ErrorString;
}
//---------------------------------------------------------------------------
#endif // _CALCULATOR_CPP