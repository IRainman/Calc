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
//#include <numbers>
#include "Linear.h"
#include "Message.h"
//---------------------------------------------------------------------------
inline void CalculateOnLineExpression(stack<char>& p_operations, stack<calc_variable>& p_operands, const string::size_type p_pos)
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
					AddError(UNKNOWN_ERROR, p_pos);
					break;
			}
		}
	}
	p_operations.pop();
}
inline void FinalizeOnLineExpression(stack<char>& p_operations, stack<calc_variable>& p_operands, const string_view::size_type p_pos)
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
			AddError(UNKNOWN_ERROR, p_pos);
			break;
	}
}
//---------------------------------------------------------------------------
[[nodiscard]] calc_variable CalculateLineExpression(const string_view& p_input_str, string_view::size_type p_mes_pos_shift /*= 0*/)
{
#ifdef ENABLE_INPUT_VALIDATION
	AddMessage(CALCULATION);
#endif
	auto l_current_priority = Priority::error;
	stack <char> l_operations;
	stack <calc_variable> l_operands;
	const auto l_begin = p_input_str.data();
	const auto l_end = l_begin + p_input_str.size();
	auto l_current = l_begin;
	
	auto isNegativeNumber = [&]()
	{
		return *l_current == '-' && l_current + 1 < l_end && GetPriority(*(l_current + 1)) == Priority::number;
	};

	auto currentPosition = [&]()
	{
		return l_current - l_begin + p_mes_pos_shift;
	};
	
	if (GetPriority(*l_current) > Priority::bracket && !isNegativeNumber())
	{
		AddError(EXPRESSION_CAN_NOT_START_FROM_OPERATION, 0);
		return NAN;
	}
	while (l_current != l_end)
	{
		const auto l_previous_priority = l_current_priority;
		const bool l_negative_number = l_previous_priority != Priority::number && isNegativeNumber();
		l_current_priority = l_negative_number ? Priority::number : GetPriority(*l_current);

		if (l_previous_priority > Priority::bracket && !l_negative_number && l_current_priority > Priority::bracket)
		{
			AddError(CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS, currentPosition());
			return NAN;
		}
		
		if (l_current_priority != Priority::number)
		{
			if (l_operations.empty())
			{
				/*
				a) if the stack is empty, an operation from the input string is pushed onto the stack.
				*/
				l_operations.push(*l_current);
				++l_current;
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
							CalculateOnLineExpression(l_operations, l_operands, currentPosition());
							if (l_operations.empty())
							{
								l_operations.push(*l_current);
								++l_current;
								break;
							}
						}
						else
						{
							l_operations.push(*l_current);
							++l_current;
							break;
						}
					}
				}
				else
				{
					if (*l_current == '(')
					{
						/*
						c) if the next character from the source string is an opening bracket, then it is pushed onto the stack.
						*/
						l_operations.push(*l_current);
						++l_current;
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
								CalculateOnLineExpression(l_operations, l_operands, currentPosition());
							}
							else
							{
								l_operations.pop();
								
								++l_current;
								break;
							}
						}
					}
				}
			}
		}
		//else if (l_current_priority == Priority::error)
		//{
		//	auto isEconst = [&]()
		//	{
		//		return l_previous_priority != Priority::number && *l_current == 'e' && l_current + 1 < l_end && GetPriority(*(l_current + 1)) != Priority::number;
		//	};
		//	if (isEconst())
		//	{
		//		l_operands.push(numbers::e_v<calc_variable>);
		//	}
		//	l_current += 1;
		//}
		else
		{
			calc_variable l_current_operand;
			const auto l_res = from_chars(l_current, l_end, l_current_operand);
			const auto l_diff = l_res.ptr - l_current;
			if (l_res.ec == errc())
			{
				l_operands.push(l_current_operand);
				l_current += l_diff;
				
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
				AddError(INTERNAL_PROCESSING_ERROR_CalculateLineExpression_l_current_operand, currentPosition());
#endif
				AddError(UNKNOWN_ERROR, currentPosition());
				return NAN;
			}
		}
	}
	if (l_current_priority > Priority::bracket)
	{
		AddError(NOT_ENOUGHT_OPERANDS, currentPosition());
		return NAN;
	}
	while (!l_operations.empty() && l_operands.size() > 1)
	{
		CalculateOnLineExpression(l_operations, l_operands, currentPosition());
	}
	if (l_operands.size() > 1)
	{
#ifdef _DEBUG
		AddError(INTERNAL_PROCESSING_ERROR_CalculateLineExpression_l_operands, currentPosition());
#endif
		AddError(UNKNOWN_ERROR, currentPosition());
		return NAN;
	}
	if (!l_operations.empty() && !l_operands.empty())
	{
		FinalizeOnLineExpression(l_operations, l_operands, currentPosition());
	}
	if (!l_operands.empty())
	{
#ifdef ENABLE_INPUT_VALIDATION
		AddMessage(print_value(string(), l_operands.top()));
#endif
		return l_operands.top();
	}
	AddError(UNKNOWN_ERROR, currentPosition());
	return NAN;
}
//---------------------------------------------------------------------------
#endif // _LINEAR_CPP