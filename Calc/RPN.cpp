/*
 * Copyright 2009-2010 Solomin Alexey Leonovich, a.rainman on gmail point com
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

#ifdef _MSC_VER
#include "stdafx.h"
/*#elif __INTEL_COMPILER
    // TODO */

// #endif // The comment is used in a silly Visual Studio :)
//---------------------------------------------------------------------------
#ifndef _RPN_CPP
#define _RPN_CPP
//---------------------------------------------------------------------------
#include "RPN.h"
#include "Message.h"
//---------------------------------------------------------------------------
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
//---------------------------------------------------------------------------
inline int_8 GetPriority(wchar_t p_sym)
{
	switch (p_sym)
	{
		case L'^':
//		case L'%':
			return priority_power;
			
		case L'*':
		case L'/':
			return priority_multiply;
			
		case L'+':
		case L'-':
			return priority_addition;
			
		case L'(':
		case L')':
			return priority_bracket;
			
		case L'0':
		case L'1':
		case L'2':
		case L'3':
		case L'4':
		case L'5':
		case L'6':
		case L'7':
		case L'8':
		case L'9':
		case L'.':
		case L'e':
			return priority_default;
			
		default:
			return priority_error;
	}
}
//---------------------------------------------------------------------------
inline void CalculateOnLineExpression(stack<wchar_t>& c_operations, stack<long double>& c_operands)
{
	// TODO: refactoring this function
// 2st!!! get operand's
	if (!c_operands.empty())
	{
		const long double b = c_operands.top();
		c_operands.pop();
		if (!c_operands.empty())
		{
			const long double a = c_operands.top();
			c_operands.pop();
			switch (c_operations.top()) // first get operations
			{
					//case L'%':
					//  c_operands.push((static_cast<int>a)%(static_cast<int>b));
				case L'^':
					c_operands.push(pow(a, b));
					break;
				case L'*':
					c_operands.push(a * b);
					break;
				case L'/':
					if (!b)
						AddError(DIVIDE_BY_ZERO);
					else
						c_operands.push(a / b);
					break;
				case L'+':
					c_operands.push(a + b);
					break;
				case L'-':
					c_operands.push(a - b);
					break;
#ifdef _DEBUG
				default:
					AddError(INTERNAL_PROCESSING_ERROR_CalculateOnLineExpression);
					break;
#endif
			}
		}
		else
		{
			switch (c_operations.top())
			{
				case L'-':
					c_operands.push(-b);
					break;
					
				default:
					AddError(CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS); // TODO delete this!
#ifdef _DEBUG
					AddError(INTERNAL_PROCESSING_ERROR_CalculateOnLineExpression);
#endif
					break;
			}
		}
	}
	c_operations.pop();
}
//---------------------------------------------------------------------------
void CalculateLineExpression(wstring p_input_str, wstring& p_output_str)
{
	int_8 l_current_prioritet = priority_error;
	stack <wchar_t> c_operations;
	stack <long double> c_operands;
	bool l_negative_number;
	wstring::size_type l_count = 0;
	for (; p_input_str.size(); l_count++)
	{
		l_negative_number = l_current_prioritet > 0 && p_input_str.c_str()[0] == L'-' && p_input_str.c_str()[1] != L'(';
		l_current_prioritet = GetPriority(p_input_str.c_str()[0]);
		if (l_current_prioritet && !l_negative_number)
		{
			if (c_operations.empty())
			{
				/*
				a)  если стек пуст, то опеpация из входной стpоки пеpеписывается в стек;
				*/
				c_operations.push(p_input_str.c_str()[0]);
				p_input_str.erase(0, 1);
			}
			else
			{
				if (l_current_prioritet != priority_bracket)
				{
					do
					{
						if (GetPriority(c_operations.top()) >= l_current_prioritet)
						{
							/*
							b)  опеpация выталкивает из стека все опеpации с большим или pавным пpиоpитетом в выходную стpоку;
							*/
							CalculateOnLineExpression(c_operations, c_operands);
							if (c_operations.empty())
							{
								c_operations.push(p_input_str.c_str()[0]);
								p_input_str.erase(0, 1);
								break;
							}
						}
						else
						{
							c_operations.push(p_input_str.c_str()[0]);
							p_input_str.erase(0, 1);
							break;
						}
					}
					while (!c_operations.empty());
				}
				else
				{
					if (p_input_str.c_str()[0] == L'(')
					{
						/*
						c)  если очеpедной символ из исходной стpоки есть откpывающая скобка, то он пpоталкивается в стек;
						*/
						c_operations.push(p_input_str.c_str()[0]);
						p_input_str.erase(0, 1);
					}
					else
					{
						while (!c_operations.empty())
						{
							/*
							d)  закpывающая кpуглая скобка выталкивает все опеpации из стека до ближайшей откpывающей скобки,
							сами скобки в выходную стpоку не пеpеписываются, а уничтожают дpуг дpуга.
							*/
							if (GetPriority(c_operations.top()) == priority_bracket && c_operations.top() != L')')
							{
								c_operations.pop();
								p_input_str.erase(0, 1);
								break;
							}
							else
							{
								CalculateOnLineExpression(c_operations, c_operands);
							}
						}
					}
				}
			}
		}
		else
		{
			wstring l_operand_string = L"";
			wstring::size_type l_count_of_num = 0;
			if (l_negative_number)
			{
				l_operand_string = p_input_str.c_str()[0];
				l_count_of_num++;
				l_count++;
			}
			wstring::size_type l_countE = 0;
			for (; l_count_of_num < p_input_str.size(); l_count_of_num++, l_count++)
			{
				if (GetPriority(p_input_str.c_str()[l_count_of_num]) == priority_default
				        || (p_input_str.c_str()[l_count_of_num - 1] == L'e'
				            && (p_input_str.c_str()[l_count_of_num] == L'+' || p_input_str.c_str()[l_count_of_num] == L'-')))
				{
					// Возможная избыточность! уже есть ошибка MULTI_E_SYMBOL_IN_NUMBER
					//if(p_input_str.c_str()[l_count_of_num - 1] == L'e')
					//  l_countE++;
					
					l_operand_string += p_input_str.c_str()[l_count_of_num];
				}
				else
				{
					break;
				}
			}
			// Возможная избыточность! уже есть ошибка MULTI_E_SYMBOL_IN_NUMBER
			//if(l_countE > 1)
			//{
			//  AddError(AFTER_E_FOUND_ANOTHER_E, l_count, l_countE);
			//}
			if (!l_negative_number || (l_negative_number && l_count_of_num > 1))
			{
				c_operands.push(_wtof(l_operand_string.c_str()));
				if (c_operands.top() >= std::numeric_limits<long double>::max() || c_operands.top() <= std::numeric_limits<long double>::min())
				{
					AddWarning(OUT_OF_RANGE);
				}
				l_current_prioritet = priority_default;
			}
			// TODO
			//if(l_count_of_num > std::numeric_limits<long double>::max_digits10)
			//{
			//  AddWarning(LOW_ACCURACY);
			//}
			p_input_str.erase(0, l_count_of_num);
		}
	}
	if (l_current_prioritet > priority_bracket && !l_negative_number)
	{
		AddError(NOT_ENOUGHT_OPERANDS, l_count);
	}
	AddMessage(PARSING_A_STRING);
	while (!c_operations.empty())
	{
		CalculateOnLineExpression(c_operations, c_operands);
	}
	if (!c_operands.empty())
	{
		wchar_t l_char_buf[100];
		swprintf_s(l_char_buf, L"%.15g", c_operands.top()); // TODO: Add variable accuracy
		p_output_str = l_char_buf;
	}
}
//---------------------------------------------------------------------------
#ifdef _ALLOW_INPUT_RPN_STRING
void CalculateRPN(wstring& p_to_process_str)
{
	/*
	Автоматизация вычисления выражений в обратной польской нотации основана на использовании стека. Алгоритм вычисления для стековой машины элементарен:
	Обработка входного символа:
	...
	 3)Если входной набор символов обработан не полностью, перейти к шагу 1.
	После полной обработки входного набора символов результат вычисления выражения лежит на вершине стека.
	*/
	stack <long double> c_operands;
	string input = p_to_process_str;
	p_to_process_str = "";
	long double a, b;
	int_8 l_current_priority;
	
	for (string::size_type l_count = 0; l_count < input.size(); l_count++)
	{
		l_current_priority = GetPriority(input.c_str()[l_count]);
		if (l_current_priority == priority_default)
		{
			/*
			1)Если на вход подан операнд, он помещается на вершину стека.
			*/
			string l_operand_string = "";
			for (; l_count < input.size() && GetPriority(input.c_str()[l_count]) == priority_default; l_count++)
			{
				l_operand_string += input.c_str()[l_count];
				if (input.c_str()[l_count] == ' ' && l_count)
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
			if (l_current_priority != priority_error)
			{
				/*
				2)Если на вход подан знак операции,
				то соответствующая операция выполняется над требуемым количеством значений,
				извлечённых из стека, взятых в порядке добавления.
				Результат выполненной операции кладётся на вершину стека.
				*/
				bool set_a = false, set_b = false;
				if (!c_operands.empty())
				{
					set_b = true;
					b = c_operands.top();
					c_operands.pop();
				}
				if (!c_operands.empty())
				{
					set_a = true;
					a = c_operands.top();
					c_operands.pop();
				}
				if (set_b)
				{
					if (set_a)
					{
						switch (input.c_str()[l_count])
						{
							case 'P':
							case '^':
								c_operands.push(pow(a, b));
								break;
							case '*':
								c_operands.push(a * b);
								break;
							case '/':
								if (!b)
								{
									AddError(DIVIDE_BY_ZERO);
								}
								c_operands.push(a / b);
								break;
							case '+':
								c_operands.push(a + b);
								break;
							case '-':
								c_operands.push(a - b);
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
						switch (input.c_str()[l_count])
						{
							case '-':
								c_operands.push(-b);
								break;
								
							default:
								AddError(NOT_ENOUGHT_OPERANDS);
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
								
								long double temp_operand = atof(temp.c_str());
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
								            AddWarning(LOW_ACCURACY);
								        }
								        c_operations.push((int)a^(int)b);
								        break;
								    case '*':
								        c_operations.push(a*b);
								        break;
								    case '/':
								        if(!b)
								        {
								            AddError(DIVIDE_BY_ZERO);
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
	if (!c_operands.empty())
	{
		char l_char_buf[100];
		sprintf_s(l_char_buf, " %.15lge ", c_operands.top());
		p_to_process_str = l_char_buf;
	}
}
#endif //_ALLOW_INPUT_RPN_STRING
//---------------------------------------------------------------------------
#endif // _RPN_CPP