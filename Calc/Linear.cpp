// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2017 Solomin Alexey Leonovich, a.rainman on gmail point com
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

//#ifdef _MSC_VER
#include "stdafx.h"
/*#elif __INTEL_COMPILER
    // TODO */

// #endif // The comment is used in a silly Visual Studio :)
//---------------------------------------------------------------------------
#ifndef _LINEAR_CPP
#define _LINEAR_CPP
//---------------------------------------------------------------------------
#include <charconv>
#include <stack>
#include "Linear.h"
#include "Message.h"
//---------------------------------------------------------------------------
inline void CalculateOnLineExpression(stack<char>& p_operations, stack<calc_variable>& p_operands, const string::size_type p_count, const string::size_type p_mes_pos_shift)
{
	if (!p_operands.empty())
	{
		const auto b = p_operands.top();
		p_operands.pop();
		if (!p_operands.empty())
		{
			const auto a = p_operands.top();
			p_operands.pop();
			switch (p_operations.top())
			{
				//case '%':
				//	c_operands.push((static_cast<int>a)%(static_cast<int>b));
				case '^':
					p_operands.push(pow(a, b));
					break;
				case '*':
					p_operands.push(a * b);
					break;
				case '/':
					p_operands.push(a / b);
					break;
				case '+':
					p_operands.push(a + b);
					break;
				case '-':
					p_operands.push(a - b);
					break;
				default:
#ifdef _DEBUG
					AddError(INTERNAL_PROCESSING_ERROR_CalculateOnLineExpression_p_operands_is_unknown);
#endif
					AddError(UNKNOWN_ERROR, p_count + p_mes_pos_shift);
					break;
			}
		}
	}
	p_operations.pop();
}
inline void FinalizeOnLineExpression(stack<char>& p_operations, stack<calc_variable>& p_operands, const string::size_type p_count, const string::size_type p_mes_pos_shift)
{
	const auto b = p_operands.top();
	p_operands.pop();
	switch (p_operations.top())
	{
		case '-':
			p_operands.push(-b);
			break;
		default:
#ifdef _DEBUG
			AddError(INTERNAL_PROCESSING_ERROR_CalculateOnLineExpression_p_operands_is_not_addition);
#endif
			AddError(UNKNOWN_ERROR, p_count + p_mes_pos_shift);
			break;
	}
}
//---------------------------------------------------------------------------
void CalculateLineExpression(string p_input_str, string& p_output_str, string::size_type p_mes_pos_shift /*= 0*/)
{
	AddMessage(CALCULATION);
	auto l_current_priority = Priority::error;
	stack <char> l_operations;
	stack <calc_variable> l_operands;
	string::size_type l_count = 0;

	auto isNegativeNumber = [] (const string& p_input_str)
	{
		return p_input_str[0] == '-' && GetPriority(p_input_str[1]) == Priority::number;
	};

	if (GetPriority(p_input_str[0]) > Priority::bracket && !isNegativeNumber(p_input_str))
	{
		AddError(EXPRESSION_CAN_NOT_START_FROM_OPERATION, p_mes_pos_shift);
		return;
	}
	for (; !p_input_str.empty(); l_count++)
	{
		const auto l_previous_priority = l_current_priority;
		const bool l_negative_number = l_previous_priority != Priority::number && isNegativeNumber(p_input_str);
		l_current_priority = l_negative_number ? Priority::number : GetPriority(p_input_str[0]);
		if (l_previous_priority > Priority::bracket && !l_negative_number && l_current_priority > Priority::bracket)
		{
			AddError(CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS, l_count + p_mes_pos_shift);
			return;
		}

		if (l_current_priority != Priority::number)
		{
			if (l_operations.empty())
			{
				/*
				a) if the stack is empty, an operation from the input string is pushed onto the stack.
				*/
				l_operations.push(p_input_str[0]);
				p_input_str.erase(0, 1);
				p_mes_pos_shift += 1;
			}
			else
			{
				if (l_current_priority != Priority::bracket)
				{
					while (!l_operations.empty())
					{
						/*
						b) the operation pushes all operations with a large or equal priority to the output line from the stack.
						*/
						if (GetPriority(l_operations.top()) >= l_current_priority)
						{
							CalculateOnLineExpression(l_operations, l_operands, l_count, p_mes_pos_shift);
							if (l_operations.empty())
							{
								l_operations.push(p_input_str[0]);
								p_input_str.erase(0, 1);
								p_mes_pos_shift += 1;
								break;
							}
						}
						else
						{
							l_operations.push(p_input_str[0]);
							p_input_str.erase(0, 1);
							p_mes_pos_shift += 1;
							break;
						}
					}
				}
				else
				{
					if (p_input_str[0] == '(')
					{
						/*
						c) if the next character from the source string is an opening bracket, then it is pushed onto the stack.
						*/
						l_operations.push(p_input_str[0]);
						p_input_str.erase(0, 1);
						p_mes_pos_shift += 1;
					}
					else
					{
						/*
						d) the closing round bracket pushes all operations from the stack to the nearest opening bracket,
						the brackets themselves do not fall into the output line, but destroy the other.
						*/
						while (!l_operations.empty())
						{
							if (l_operations.top() != '(')
							{
								CalculateOnLineExpression(l_operations, l_operands, l_count, p_mes_pos_shift);
							}
							else
							{
								l_operations.pop();
								
								p_input_str.erase(0, 1);
								p_mes_pos_shift += 1;
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			calc_variable l_current_operand;
			const auto l_res = from_chars(p_input_str.data(), p_input_str.data() + p_input_str.size(), l_current_operand);
			const auto l_diff = l_res.ptr - p_input_str.data();
			if (l_res.ec == errc())
			{
				p_input_str.erase(0, l_diff);
				p_mes_pos_shift += l_diff;
				
				l_operands.push(l_current_operand);
				
#ifdef ENABLE_WARNINGS_IN_LOG
				// TODO
				if (l_operands.top() >= std::numeric_limits<calc_variable>::max())
				{
					AddWarning(OUT_OF_RANGE);
				}
				if (l_operands.top() <= std::numeric_limits<calc_variable>::min())
				{
					AddWarning(OUT_OF_RANGE);
				}
				if (l_diff > std::numeric_limits<calc_variable>::max_digits10)
				{
					AddWarning(MAX_DIGITS, l_diff, std::numeric_limits<calc_variable>::max_digits10);
				}
#endif
			}
			else
			{
#ifdef _DEBUG
				AddError(INTERNAL_PROCESSING_ERROR_CalculateLineExpression_l_current_operand, l_count + p_mes_pos_shift);
#endif
				AddError(UNKNOWN_ERROR, l_count + p_mes_pos_shift);
				return;
			}
		}
	}
	if (l_current_priority > Priority::bracket)
	{
		AddError(NOT_ENOUGHT_OPERANDS, l_count + p_mes_pos_shift);
		return;
	}
	while (!l_operations.empty() && l_operands.size() > 1)
	{
		CalculateOnLineExpression(l_operations, l_operands, l_count, p_mes_pos_shift);
	}
	if (l_operands.size() > 1)
	{
#ifdef _DEBUG
		AddError(INTERNAL_PROCESSING_ERROR_CalculateLineExpression_l_operands, l_count + p_mes_pos_shift);
#endif
		AddError(UNKNOWN_ERROR, l_count + p_mes_pos_shift);
		return;
	}
	if (!l_operations.empty() && !l_operands.empty())
	{
		FinalizeOnLineExpression(l_operations, l_operands, l_count, p_mes_pos_shift);
	}
	if (!l_operands.empty())
	{
		print_value(p_output_str, l_operands.top());
	}
	AddMessage(p_output_str);
}
//---------------------------------------------------------------------------
#endif // _LINEAR_CPP