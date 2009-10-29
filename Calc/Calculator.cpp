
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
//#define _USE_RPN //TODO: Calculate on RPN string
#define _USE_Function //TODO: Add function support
//#define _USE_MATH_DEFINES //TODO: Add constant support
#include <math.h>
#include <stack>
#include <list>
#include "MyTypes.h"
//---------------------------------------------------------------------------
static wstring m_ErrorString;
static bool m_NoError;
static stack <char> c_operations;
static stack <double> c_operands;
static double a, b;
//---------------------------------------------------------------------------
enum
{
	priority_function		= 16,
	priority_power			= 8,
	priority_multiply		= 4,
	priority_addition		= 2,
	priority_bracket		= 1,
	priority_default		= 0,
	priority_error			= -64
};
#ifdef _USE_Function
//list <string> c_function1; // TODO
list <string> c_function2;
#define c_max_argument_of_function 2
#endif //_USE_Function
//---------------------------------------------------------------------------
void AddMessage(const string& p_string_message)
{
	wstring l_message;
	l_message.assign(p_string_message.begin(), p_string_message.end());
	m_ErrorString += l_message + L"\r\n";
}
//---------------------------------------------------------------------------
void AddMessage(uint_8 p_message)
{
	switch(p_message)
	{
		case 0:
#ifdef _USE_Function
			m_ErrorString += L"\tПодготовка:\r\n";
#else
			m_ErrorString += L"\tПодготовка и вычисление функций:\r\n";
#endif // _USE_Function
			break;
		case 1:
			m_ErrorString += L"\tРазбор строки:\r\n";
			break;
		case 2:
			m_ErrorString += L"\tВычисление:\r\n";
			break;
#ifdef _USE_RPN
		case 3:
			m_ErrorString += L"\tRPN:\r\n";
			break;
#endif // _USE_RPN
#ifdef _DEBUG
		default:
			m_ErrorString += L" DEBUG: Unknown Message!";
			break;
#endif
	}
}
//---------------------------------------------------------------------------
void AddWarning(uint_8 p_message)
{
#ifdef _DEBUG
	wchar_t l_temp_buf[1024];
	swprintf_s(l_temp_buf, L"AddWarning(%d... ", p_message);
	m_ErrorString += l_temp_buf;
#endif
	m_ErrorString += L"Внимание: ";
	switch(p_message)
	{
		case 0:
			m_ErrorString += L"число слишком большое, вычисление может быть выполнено с ошибками";
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
	m_NoError = false;
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
#ifdef _USE_RPN
		case 3:
			l_error = L"Недопустимый символ во время преобразования в обратную польскую запись";
			break;
#endif // _USE_RPN
		case 4:
			l_error = L"Последовательная запись нескольких операций не подерживается";
			break;
		case 5:
			l_error = L"На ноль делить нельзя";
			break;
#ifdef _USE_Function
		case 6:
			l_error = L"У функции нехватает закрывающей скобки";
			break;
		case 7:
			l_error = L"У функции отсутсвуют аргументы";
			break;
#endif // _USE_Function
		case 8:
			l_error = L"Недостаточно операндов для получения результата";
			break;
		case 9:
			l_error = L"Выражение не может начинатся со знака операции";
			break;
#ifdef _USE_Function
		case 10:
			l_error = L"У функции неверное число аргументов необходимо " + (wstring)l_1 + L", обнаружено " + (wstring)l_2;
			break;
		case 11:
			l_error = L"Выражения как параметры функции не поддерживаются, позиция внутри функции " + (wstring)l_1;
			break;
#endif // _USE_Function
		case 12:
			l_error = L"Выражение не может начинаться и заканчиваться скобкой";
			break;
		case 13:
			l_error = L"Пустые скобки";
			break;
		case 14:
			l_error = L"Открывающая скобка идёт сразу после закрывающей";
			break;
		case 15:
			l_error = L"Запись числа после закрывающей скобки недопустима";
			break;
		case 16:
			l_error = L"Выражение в скобках заканчивается знаком операции";
			break;
		case 17:
			l_error = L"Выражение в скобках начинается со знака операции";
			break;
#ifdef _USE_Function
		case 18:
			l_error = L"У функции отсутсвует " + (wstring)l_1 + L" аргумент, позиция внутри скобок функции " + (wstring)l_2;
			break;
#endif // _USE_Function
#ifdef _DEBUG
		case 253:
			l_error = L"DEBUG: Internal Processing Error: \"void CalculateFunction(...)\"";
			break;
		case 254:
			l_error = L"DEBUG: Internal Processing Error: \"bool CalculateOnLineExpression()\"";
			break;
		case 255:
			l_error = L"DEBUG: number of arguments passed to the function \"replace\" is not defined in the function of \"replace\", it is impossible to calculate the expression";
			break;
		default:
			l_error = L"DEBUG: Unknown Error!";
			break;
#endif
	}
	wchar_t l_wchar_buf[1024];
	if(p_count == -1)
	{
		swprintf_s(l_wchar_buf, L"Ошибка: %s\r\n", l_error.c_str());
	}
	else
	{
		swprintf_s(l_wchar_buf, L"В позиции %d ошибка: %s\r\n", p_count, l_error.c_str());
	}
#ifdef _DEBUG
	wchar_t l_temp_buf[1024];
	swprintf_s(l_temp_buf, L"AddError(%d... %s", p_message, l_wchar_buf);
	m_ErrorString += l_temp_buf;
#else
	m_ErrorString += l_wchar_buf;
#endif
}
//---------------------------------------------------------------------------
int_8 GetPriority(char p_sym)
{
	switch(p_sym)
	{
/*		case 'P':
			return priority_function;
*/
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
		case '.':
		case ' ':
//		case 'E': TODO add power
			return priority_default;

		default:
			return priority_error;
	}
}
//---------------------------------------------------------------------------
#ifdef _USE_Function
/*void CalculateFunction(string& p_input_str)
{
	string::size_type c;
	for(uint i = 0; i < c_function2count; i++)
	{
		c = p_input_str.find(c_function2[i]);
		if(c != string::npos)
		{
			//add(c);
		}
	}
}*/
//---------------------------------------------------------------------------
void CalculateFunction(string& p_input_str, string p_in, uint p_count, uint_8 p_number_of_param)
{
	string::size_type l_start, l_end, l_br_start;
	string l_buf;
	char l_char_buf[320];
	double l_params[c_max_argument_of_function];
	while(true)
	{
		l_start = p_input_str.find(p_in);
		if(l_start == string::npos)
			return;

		l_buf = p_input_str;
		l_buf.erase(0, l_start + p_in.size());

		l_end = l_buf.find(")");
		if(l_end == string::npos)
		{
			AddError(6, l_start);
			return;
		}
		l_br_start = l_buf.find("(");
		if(l_br_start != string::npos && l_br_start < l_end)
		{
			AddError(11, l_start, l_br_start);
			return;
		}
		l_buf = l_buf.substr(0, l_end);
		if(l_buf.empty())
		{
			AddError(7, l_start);
			return;
		}
		string::size_type c;
		string::size_type old_c = 0;
		uint_8 l_count = 0;
		while(true)
		{
			c = l_buf.find(",");
			if(c != string::npos)
			{
				sscanf_s(l_buf.c_str(), "%lf", &l_params[l_count]);
				if(c - old_c == 0 || (l_buf[c-1] == '-' && c - old_c == 1))
				{
					AddError(18, l_start, l_count + 1, c + 1);
				}
				l_buf.erase(0, c + 1);
				l_count++;
			}
			else
			{
				if(!l_buf.size())
				{
					AddError(18, l_start, l_count + 1, c + 1);
				}
				break;
			}
			old_c = c;
		}
		sscanf_s(l_buf.c_str(), "%lf", &l_params[l_count]);
		if(l_count != p_number_of_param - 1)
		{
			AddError(10, l_start, p_number_of_param, l_count + 1);
			return;
		}
		double result;
		switch(p_number_of_param)
		{
			case 2:
				switch(p_count)
				{
					case 0:
							result = pow(l_params[0], l_params[1]);
						break;
				}
				try
				{
					sprintf_s(l_char_buf, "%.20lf", result);
				}
				catch(...)
				{
#ifdef _DEBUG
				AddError(253);
				return;
#endif
				}
				break;
#ifdef _DEBUG
			default:
				AddError(10, l_start, p_number_of_param, l_count + 1);
				return;
#endif
		}
		p_input_str.erase(l_start, l_start + l_end + 1 + p_in.size());
		p_input_str.insert(l_start, l_char_buf);
	}
}
//---------------------------------------------------------------------------
/*
void replace(string& p_input_str, string p_in, string p_out, string::size_type p_count_end)
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
			p_input_str.erase(l_start + 1, p_count_end - l_start);
			p_input_str.insert(l_start + 1, p_out);
		}
	}
}
*/
#endif // _USE_Function
//---------------------------------------------------------------------------
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
		/*p_input_str[l_count] = tolower(p_input_str.c_str()[l_count]);
		if( (p_input_str.c_str()[l_count] >= '(' && p_input_str.c_str()[l_count] <= '9') ||
			(p_input_str.c_str()[l_count] >= 'a' && p_input_str.c_str()[l_count] <= 'z') ||
			 p_input_str.c_str()[l_count] == '^' )
		{*/
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
				count_outp++;
			}
		/*}
		else
		{
			AddError(0, l_count);
		}*/
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
void CalculateOnLineExpression()
{
	bool set_a = false, set_b = false;
	if(!c_operands.empty())
	{
		set_b = true;
		b = c_operands.top();
		c_operands.pop();
	}
	if(!c_operands.empty())
	{
		set_a = true;
		a = c_operands.top();
		c_operands.pop();
	}
	if(set_b)
	{
		if(set_a)
		{
			switch(c_operations.top())
			{
				#ifdef _USE_Function
				case 'P':
					c_operands.push(pow(a,b));
					break;
				#endif // _USE_Function
				case '^':
					c_operands.push(pow(a,b));
					break;
				case '*':
					c_operands.push(a*b);
					break;
				case '/':
					if(!b)
					{
						AddError(5);
					}
					else
					{
						c_operands.push(a/b);
					}
					break;
				case '+':
					c_operands.push(a+b);
					break;
				case '-':
					c_operands.push(a-b);
					break;
#ifdef _DEBUG
				default:
					AddError(254);
					break;
#endif
			}
		}
		else
		{
			switch(c_operations.top())
			{
				case '-':
					c_operands.push(-b);
					break;
#ifdef _DEBUG
				default:
					AddError(254);
					break;
#endif
			}
		}
	}
	c_operations.pop();
}
//---------------------------------------------------------------------------
void CalculateLineExpression(string p_input_str, string& p_output_str)
{
	int_8 l_current_prioritet = priority_error, l_old_prioritet = priority_error;
	while(!c_operands.empty())
	{
		c_operands.pop();
	}
	if(GetPriority(p_input_str.c_str()[0]) > priority_bracket && p_input_str.c_str()[0] != '-')
	{
		AddError(9);
	}
	bool l_negative_number;
	string::size_type l_count = 0;
	for(; p_input_str.size(); l_count++)
	{
		l_current_prioritet = GetPriority(p_input_str.c_str()[0]);
		l_negative_number = l_old_prioritet > 0 && p_input_str.c_str()[0] == '-';
		if(l_current_prioritet && !l_negative_number)
		{
			if(l_current_prioritet > l_old_prioritet && l_old_prioritet > priority_bracket && p_input_str.c_str()[0] != '-')
			{
				AddError(4, l_count - 1);
			}
			if(c_operations.empty())
			{
				/*
				a)	если стек пуст, то опеpация из входной стpоки пеpеписывается в стек;
				*/
				c_operations.push(p_input_str.c_str()[0]);
				p_input_str.erase(0,1);
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
							CalculateOnLineExpression();
							if(c_operations.empty())
							{
								c_operations.push(p_input_str.c_str()[0]);
								p_input_str.erase(0,1);
								break;
							}
						}
						else
						{
							c_operations.push(p_input_str.c_str()[0]);
							p_input_str.erase(0,1);
							break;
						}
					}
					while(!c_operations.empty());
				}
				else
				{
					if(p_input_str.c_str()[0] == '(')
					{
						/*
						c)	если очеpедной символ из исходной стpоки есть откpывающая скобка, то он пpоталкивается в стек;
						*/
						c_operations.push(p_input_str.c_str()[0]);
						p_input_str.erase(0,1);
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
								p_input_str.erase(0,1);
								break;
							}
							else
							{
								CalculateOnLineExpression();
							}
						}
					}
				}
			}
		}
		else
		{
			string l_operand_string = "";
			string::size_type l_count_of_num = 0;
			if(l_negative_number)
			{
				l_operand_string = p_input_str.c_str()[0];
				l_count_of_num++;
				l_count++;
			}
			for(; l_count_of_num < p_input_str.size() && GetPriority(p_input_str.c_str()[l_count_of_num]) == priority_default; l_count_of_num++, l_count++)
			{
				l_operand_string += p_input_str.c_str()[l_count_of_num];
			}
			if(!l_negative_number || (l_negative_number && l_count_of_num > 1))
			{
				c_operands.push(atof(l_operand_string.c_str()));
				if(c_operands.top() > 999999999999999 || c_operands.top() < -999999999999999) // TODO add normal terms ;)
				{
					AddWarning(0);
				}
				l_current_prioritet = priority_default;
			}
			else
			{
				AddError(4, l_count);
			}
			p_input_str.erase(0, l_count_of_num);
		}
		l_old_prioritet = l_current_prioritet;
	}
	if(l_current_prioritet > priority_bracket && !l_negative_number)
	{
		AddError(8, l_count);
	}
	AddMessage(1);
	while(!c_operations.empty())
	{
		CalculateOnLineExpression();
	}
	if(!c_operands.empty())
	{
		char l_char_buf[320];
		sprintf_s(l_char_buf, "%.16lf", c_operands.top()); // TODO: Add variable precision
		p_output_str = l_char_buf;
	}
}
//---------------------------------------------------------------------------
#ifdef _USE_RPN
void CalculateRPN(string& p_to_process_str)
{
	/*
	Автоматизация вычисления выражений в обратной польской нотации основана на использовании стека. Алгоритм вычисления для стековой машины элементарен:
	Обработка входного символа:
	...
	 3)Если входной набор символов обработан не полностью, перейти к шагу 1.
	После полной обработки входного набора символов результат вычисления выражения лежит на вершине стека.
	*/
	stack <double> c_operands;
	string input = p_to_process_str;
	p_to_process_str = "";
	double a, b;
	int_8 l_current_priority;
	
	for(string::size_type l_count = 0; l_count < input.size(); l_count++)
	{
		l_current_priority = GetPriority(input.c_str()[l_count]);
		if(l_current_priority == priority_default)
		{
			/*
			1)Если на вход подан операнд, он помещается на вершину стека.
			*/
			string l_operand_string = "";
			for(; l_count < input.size() && GetPriority(input.c_str()[l_count]) == priority_default; l_count++)
			{
				l_operand_string += input.c_str()[l_count];
				if(input.c_str()[l_count] == ' ' && l_count)
				{
					l_count++;
					break;
				}
			}
			c_operands.push(atof(l_operand_string.c_str()));
			input.erase(0, l_count);
			l_count = -1;
		}
		else
		{
			if(l_current_priority != priority_error)
			{
				/*
				2)Если на вход подан знак операции,
				то соответствующая операция выполняется над требуемым количеством значений,
				извлечённых из стека, взятых в порядке добавления.
				Результат выполненной операции кладётся на вершину стека.
				*/
				bool set_a = false, set_b = false;
				if(!c_operands.empty())
				{
					set_b = true;
					b = c_operands.top();
					c_operands.pop();
				}
				if(!c_operands.empty())
				{
					set_a = true;
					a = c_operands.top();
					c_operands.pop();
				}
				if(set_b)
				{
					if(set_a)
					{
						switch(input.c_str()[l_count])
						{
							case 'P':
								c_operands.push(pow(a,b));
								break;
							case '^':
								if((int)a != a || (int)b != b)
								{
									AddWarning(0);
								}
								c_operands.push((int)a^(int)b);
								break;
							case '*':
								c_operands.push(a*b);
								break;
							case '/':
								if(!b)
								{
									AddError(5);
								}
								c_operands.push(a/b);
								break;
							case '+':
								c_operands.push(a+b);
								break;
							case '-':
								c_operands.push(a-b);
								break;
#ifdef _DEBUG
							default:
								p_to_process_str += "DEBUG: Internal Processing Error!" + input.c_str()[l_count];
								break;
#endif
						}
						input.erase(l_count, l_count + 1);
						l_count = -1;
					}
					else
					{
						switch(input.c_str()[l_count])
						{
							case '-':
								c_operands.push(-b);
								break;

							default:
								AddError(8);
								/*char l_op = input.c_str()[l_count++];
								string temp = "";
								for(; l_count < input.size(); l_count++)
								{
									temp += input.c_str()[l_count];
									if(GetPriority(input.c_str()[l_count]) > priority_default)
									{
										break;
									}
								}
								
								double temp_operand = atof(temp.c_str());
								a = b;
								b = temp_operand;*/
							/* 
							switch(input.c_str()[l_count])
							{
								case 'P':
									c_operations.push(pow(a,b));
									break;
								case '^':
									if((int)a != a || (int)b != b)
									{
										AddWarning(0);
									}
									c_operations.push((int)a^(int)b);
									break;
								case '*':
									c_operations.push(a*b);
									break;
								case '/':
									if(!b)
									{
										AddError(5);
										return false;
									}
									c_operations.push(a/b);
									break;
								case '+':
									c_operations.push(a+b);
									break;
								case '-':
									c_operations.push(a-b);
									break;
#ifdef _DEBUG
								default:
									p_to_process_str += "DEBUG: Internal Processing Error!" + input.c_str()[l_count];
									break;
#endif
							}
							*/ 
						}
						l_count = -1;
					}
				}
			}
		}
	}
	if(!c_operands.empty())
	{
		char l_char_buf[100];
		sprintf_s(l_char_buf, " %lf ", c_operands.top());
		p_to_process_str = l_char_buf;
	}
}
#endif //_USE_RPN
//---------------------------------------------------------------------------
wstring& Calculate(wstring p_input, wstring& p_output)
{
	/*if(c_function1.empty())// TODO
	{
		c_function1.push_back("sin(");
		c_function1.push_back("cos(");
	}*/
	if(c_function2.empty())
	{
		c_function2.push_back("pow(");
	}
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