
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
inline int_8 GetPriority(char p_sym)
{
	switch(p_sym)
	{
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
		case 'e':
			return priority_default;

		default:
			return priority_error;
	}
}
//---------------------------------------------------------------------------
inline void CalculateOnLineExpression(stack<char>& c_operations, stack<long double>& c_operands)
{
	// TODO: refactoring this function
	bool set_a = false, set_b = false;// don't use flags
	long double a, b;
// 2st!!! get operand's
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
			switch(c_operations.top())// first get operations
			{
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

				default:
					AddError(4); // TODO delete this!
#ifdef _DEBUG
					AddError(254);
#endif
					break;
			}
		}
	}
	c_operations.pop();
}
//---------------------------------------------------------------------------
void CalculateLineExpression(string p_input_str, string& p_output_str, bool p_scientific /*= true*/)
{
	int_8 l_current_prioritet = priority_error;
	stack <char> c_operations;
	stack <long double> c_operands;
	bool l_negative_number;
	string::size_type l_count = 0;
	for(; p_input_str.size(); l_count++)
	{
		l_negative_number = l_current_prioritet > 0 && p_input_str.c_str()[0] == '-' && p_input_str.c_str()[1] != '(';
		l_current_prioritet = GetPriority(p_input_str.c_str()[0]);
		if(l_current_prioritet && !l_negative_number)
		{
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
							CalculateOnLineExpression(c_operations, c_operands);
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
								CalculateOnLineExpression(c_operations, c_operands);
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
			string::size_type l_countE = 0;
			for(; l_count_of_num < p_input_str.size(); l_count_of_num++, l_count++)
			{
				if(GetPriority(p_input_str.c_str()[l_count_of_num]) == priority_default
					|| (p_input_str.c_str()[l_count_of_num - 1] == 'e'
					&& (p_input_str.c_str()[l_count_of_num] == '+' || p_input_str.c_str()[l_count_of_num] == '-')))
				{
					if(p_input_str.c_str()[l_count_of_num - 1] == 'e')
						l_countE++;

					l_operand_string += p_input_str.c_str()[l_count_of_num];
				}
				else
				{
					break;
				}
			}
			if(l_countE > 1)
			{
				AddError(26, l_count, l_countE);
			}
			if(!l_negative_number || (l_negative_number && l_count_of_num > 1))
			{
				c_operands.push(atof(l_operand_string.c_str()));
			//	if(c_operands.top() >= std::numeric_limits<long double>::max() || c_operands.top() <= std::numeric_limits<long double>::min()) // TODO add normal terms ;)
				if(c_operands.top() > 999999999999999 || c_operands.top() < -999999999999999) // TODO add normal terms ;)
				{
					AddWarning(0);
				}
				l_current_prioritet = priority_default;
			}
			p_input_str.erase(0, l_count_of_num);
		}
	}
	if(l_current_prioritet > priority_bracket && !l_negative_number)
	{
		AddError(8, l_count);
	}
	AddMessage(PARSING_A_STRING);
	while(!c_operations.empty())
	{
		CalculateOnLineExpression(c_operations, c_operands);
	}
	if(!c_operands.empty())
	{
		char l_char_buf[400];
		sprintf_s(l_char_buf, p_scientific ? "%.14lge" : "%.14g", c_operands.top()); // TODO: Add variable accuracy
		p_output_str = l_char_buf;
	}
}
//---------------------------------------------------------------------------
#ifdef _ALLOW_INPUT_RPN_STRING
void CalculateRPN(string& p_to_process_str)
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
		sprintf_s(l_char_buf, " %.14lge ", c_operands.top());
		p_to_process_str = l_char_buf;
	}
}
#endif //_ALLOW_INPUT_RPN_STRING
//---------------------------------------------------------------------------
#endif // _RPN_CPP