
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
static wstring m_ErrorString;
static stack <char> c_operations;
static stack <double> c_operands;
static double a, b;
static string::size_type p_count;
//---------------------------------------------------------------------------
enum {
	priority_function	= 16,
	priority_power		= 8,
	priority_multiply	= 4,
	priority_addition	= 2,
	priority_bracket	= 1,
	priority_default	= 0,
	priority_error		= 255
};
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
			m_ErrorString += L"\tПодготовка:\r\n";
			break;
		case 1:
			m_ErrorString += L"\tРазбор строки:\r\n";
			break;
		case 2:
			m_ErrorString += L"\tВычисление:\r\n";
			break;
		/*case 3:
			m_ErrorString += L"\tRPN:\r\n";
			break;*/
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
/*		case 2:
			l_error = L"Недопустимый символ после проверки на функции, проверьте правильность их написания";
			break;
		case 3:*/
			l_error = L"Недопустимый символ во время преобразования в обратную польскую запись";
			break;
		case 4:
			l_error = L"Последовательная запись нескольких операций не подерживается";
			break;
		case 5:
			l_error = L"На ноль делить нельзя";
			break;
/*		case 6:
			l_error = L"У функции нехватает закрывающей скобки";
			break;
		case 7:
			l_error = L"У функции отсутсвуют аргументы";
			break;*/
		case 8:
			l_error = L"Недостаточно операндов для получения результата";
			break;
		case 9:
			l_error = L"Выражение не может начинатся со знака операции";
			break;
/*		case 10:
			l_error = L"У функции неверное число аргументов необходимо " + (wstring)l_1 + L", обнаружено " + (wstring)l_2;
			break;
		case 11:
			l_error = L"Вложенные функции не поддерживаются, позиция внутри функции " + (wstring)l_1;
			break;*/
		case 12:
			l_error = L"Выражение не может начинаться и заканчиваться скобкой";
			break;
#ifdef _DEBUG
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
			return priority_default;

		default:
			return priority_error;
	}
}
//---------------------------------------------------------------------------
/*
bool replace(string& p_input_str, string p_in, char p_out, uint_8 p_number_of_param)
{
	string::size_type l_start, l_end, l_br_start;
	string l_buf;
	static char l_char_buf[200];
	double l_a, l_b;
	while(true)
	{
		l_start = p_input_str.find(p_in);
		if(l_start == string::npos)
			return true;

		l_buf = p_input_str;
		l_buf.erase(0, l_start);

		l_end = l_buf.find(")");
		l_br_start = l_buf.find("(");
		if(l_br_start > l_end)
		{
			AddError(11, l_start, l_br_start);
			return false;
		}
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
				sscanf_s(l_buf.c_str(), "%lf %lf", &l_a, &l_b);
				sprintf_s(l_char_buf, " %lf%c%lf ", l_a, p_out, l_b);
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
*/
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
//---------------------------------------------------------------------------
bool ValidateAndPrepareInputString(string& p_input_str)
{
	uint count_inp = 0, count_outp = 0;
	string::size_type c;
	bool l_function_ok = true;

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
				count_inp++;
				continue;
			}
			if(p_input_str.c_str()[l_count] == ')')
			{
				count_outp++;
				continue;
			}
		}
		else
		{
			AddError(0, l_count);
			l_function_ok = false;
		}
	}
	if(count_inp != count_outp)
	{
		AddError(1, -1, count_inp, count_outp);
		l_function_ok = false;
	}
/*
	l_function_ok &= replace(p_input_str, "pow", 'P', 2);

	for(l_count = 0; l_count < p_input_str.size(); l_count++)
	{
		if(p_input_str.c_str()[l_count] >= 'a' && p_input_str.c_str()[l_count] <= 'z')
		{
			AddError(2, l_count);
			l_function_ok = false;
		}
	}*/
	/* TODO проверка ну пустые скобки 
	if(GetPriority(p_input_str.c_str()[--l_count]) == priority_bracket && GetPriority(p_input_str.c_str()[0]) == priority_bracket && count_inp < 2)
	{
		AddError(12);
		l_function_ok = false;
	}*/

	return l_function_ok;
}
//---------------------------------------------------------------------------
bool CalculateOnLineExpression()
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
						return false;
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
					AddError(254);
					break;
#endif
			}
		}
	}
	c_operations.pop();
	return true;
}
//---------------------------------------------------------------------------
bool CalculateLineExpression(string p_input_str, string& p_output_str)
{
	uint_8 l_current_prioritet = priority_error, l_old_prioritet = priority_error;
	p_count = 0;
	string::size_type l_count = 0;
	bool is_ok = true;
	while(!c_operands.empty())
	{
		c_operands.pop();
	}
	if(GetPriority(p_input_str.c_str()[p_count]) > priority_bracket && p_input_str.c_str()[p_count] != '-')
	{
		AddError(9);
		is_ok = false;
	}
	bool l_negative_number;
	for(; p_count < p_input_str.size(); p_count++, l_count++)
	{
		l_current_prioritet = GetPriority(p_input_str.c_str()[p_count]);
		l_negative_number = l_old_prioritet && p_input_str.c_str()[p_count] == '-';
		if(l_current_prioritet && !l_negative_number)
		{
			if(l_current_prioritet == priority_error)
			{
				AddError(3, l_count);
				is_ok = false;
			}
			if(l_old_prioritet > priority_bracket && l_current_prioritet >= l_old_prioritet && p_input_str.c_str()[p_count] != '-')
			{
				AddError(4, l_count - 1);
				is_ok = false;
			}
			if(c_operations.empty())
			{
				/*
				a)	если стек пуст, то опеpация из входной стpоки пеpеписывается в стек;
				*/
				c_operations.push(p_input_str.c_str()[p_count]);
				p_input_str.erase(0,1);
				p_count = -1;
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
							is_ok &= CalculateOnLineExpression();
							if(c_operations.empty())
							{
								c_operations.push(p_input_str.c_str()[p_count]);
								p_input_str.erase(0,1);
								p_count = -1;
								break;
							}
						}
						else
						{
							c_operations.push(p_input_str.c_str()[p_count]);
							p_input_str.erase(0,1);
							p_count = -1;
							break;
						}
					}
					while(!c_operations.empty());
				}
				else
				{
					if(p_input_str.c_str()[p_count] != ')')
					{
						/*
						c)	если очеpедной символ из исходной стpоки есть откpывающая скобка, то он пpоталкивается в стек;
						*/
						c_operations.push(p_input_str.c_str()[p_count]);
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
								p_count = -1;
								break;
							}
							else
							{
								is_ok &= CalculateOnLineExpression();
							}
						}
					}
				}
			}
		}
		else
		{
			string l_operand_string = "";
			if(l_negative_number)
			{
				l_operand_string += p_input_str.c_str()[p_count];
				p_count++;
				l_count++;
			}
			for(; p_count < p_input_str.size() && GetPriority(p_input_str.c_str()[p_count]) == priority_default; p_count++, l_count++)
			{
				l_operand_string += p_input_str.c_str()[p_count];
			}
			if(!l_negative_number || (l_negative_number && p_count > 1))
			{
				c_operands.push(atof(l_operand_string.c_str()));
				l_current_prioritet = priority_default;
			}
			else
			{
				AddError(4, l_count);
				is_ok = false;
			}
			p_input_str.erase(0, p_count);
			p_count = -1;
		}
		l_old_prioritet = l_current_prioritet;
	}
	if(l_current_prioritet > priority_bracket && !l_negative_number)
	{
		AddError(8, l_count);
		is_ok = false;
	}
	AddMessage(1);
	while(!c_operations.empty())
	{
		is_ok &= CalculateOnLineExpression();
	}
	if(!c_operands.empty())
	{
		char l_char_buf[100];
		try
		{
			sprintf_s(l_char_buf, "%lf", c_operands.top());
			p_output_str = l_char_buf;
		}
		catch(...)
		{
			is_ok = false;
			m_ErrorString += L"Суслик, ты балбес!!! число слишком большое\r\n";
		}
	}
	return is_ok;
}
//---------------------------------------------------------------------------
bool TranslateToOutputString(string& p_to_process_str)
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
	uint_8 l_current_priority;
	
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
									return false;
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
								return false;
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
	return true;
}
//---------------------------------------------------------------------------
wstring& Calculate(wstring p_input, wstring& p_output)
{
	m_ErrorString = L"";
	bool l_No_Error = true;

	string l_input_str, l_output_str;
	l_input_str.assign(p_input.begin(), p_input.end());

	AddMessage(0);
	l_No_Error &= ValidateAndPrepareInputString(l_input_str);
	if(l_No_Error)
	{
		AddMessage(l_input_str);
		l_No_Error &= CalculateLineExpression(l_input_str, l_output_str);
		/*if(l_No_Error) //TODO: Calculate on RPN string
		{
			AddMessage(1, l_output_str);
			l_No_Error &= TranslateToOutputString(l_output_str);
			
		}*/
		if(l_No_Error)
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