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
#include "Linear.h"
#include "Message.h"
//---------------------------------------------------------------------------
#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
//---------------------------------------------------------------------------
inline void CalculateOnLineExpression(stack<char>& p_operations, stack<calc_variable>& p_operands, const string::size_type p_count, const string::size_type p_mes_pos_shift)
{
	// TODO: refactoring this function
// 2st!!! get operand's
	if (!p_operands.empty())
	{
		const auto b = p_operands.top();
		p_operands.pop();
		if (!p_operands.empty())
		{
			const auto a = p_operands.top();
			p_operands.pop();
			switch (p_operations.top()) // first get operations
			{
					//case '%':
					//  c_operands.push((static_cast<int>a)%(static_cast<int>b));
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
		else
		{
			switch (p_operations.top())
			{
				case '-':
					p_operands.push(-b);
					break;
				case '+':
					p_operands.push(b);
					break;
				default:
#ifdef _DEBUG
					AddError(INTERNAL_PROCESSING_ERROR_CalculateOnLineExpression_p_operands_is_not_addition);
#endif
					AddError(UNKNOWN_ERROR, p_count + p_mes_pos_shift);
					break;
			}
		}
	}
	p_operations.pop();
}
//---------------------------------------------------------------------------
void CalculateLineExpression(string p_input_str, string& p_output_str, string::size_type p_mes_pos_shift /*= 0*/)
{
	AddMessage(CALCULATION);
	Priority l_current_prioritet = priority_error;
	stack <char> l_operations;
	stack <calc_variable> l_operands;
	string::size_type l_count;
	for (l_count = 0; !p_input_str.empty(); l_count++)
	{
		l_current_prioritet = GetPriority(p_input_str.c_str()[0]);
		if (l_current_prioritet != priority_default)
		{
			if (l_operations.empty())
			{
				/*
				a)  если стек пуст, то опеpация из входной стpоки пеpеписывается в стек;
				*/
				l_operations.push(p_input_str.c_str()[0]);
				p_input_str.erase(0, 1);
				p_mes_pos_shift += 1;
			}
			else
			{
				if (l_current_prioritet != priority_bracket)
				{
					do
					{
						/*
						b)  опеpация выталкивает из стека все опеpации с большим или pавным пpиоpитетом в выходную стpоку;
						*/
						if (GetPriority(l_operations.top()) >= l_current_prioritet)
						{
							CalculateOnLineExpression(l_operations, l_operands, l_count, p_mes_pos_shift);
							if (l_operations.empty())
							{
								l_operations.push(p_input_str.c_str()[0]);
								p_input_str.erase(0, 1);
								p_mes_pos_shift += 1;
								break;
							}
						}
						else
						{
							l_operations.push(p_input_str.c_str()[0]);
							p_input_str.erase(0, 1);
							p_mes_pos_shift += 1;
							break;
						}
					}
					while (!l_operations.empty());
				}
				else
				{
					if (p_input_str.c_str()[0] == '(')
					{
						/*
						c)  если очеpедной символ из исходной стpоки есть откpывающая скобка, то он пpоталкивается в стек;
						*/
						l_operations.push(p_input_str.c_str()[0]);
						p_input_str.erase(0, 1);
						p_mes_pos_shift += 1;
					}
					else
					{
						/*
						d)  закpывающая кpуглая скобка выталкивает все опеpации из стека до ближайшей откpывающей скобки,
						сами скобки в выходную стpоку не пеpеписываются, а уничтожают дpуг дpуга.
						*/
						do
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
						while (!l_operations.empty());
					}
				}
			}
		}
		else
		{
			char* l_current_operand_ptr = &p_input_str[0];
			char** l_current_operand_ptr_ptr = &l_current_operand_ptr;
			const auto l_current_operand = calc_input_function(p_input_str.c_str(), l_current_operand_ptr_ptr);
			const auto l_diff = *l_current_operand_ptr_ptr - p_input_str.c_str();
			if (l_diff)
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
				l_current_prioritet = priority_default;
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
	if (l_current_prioritet > priority_bracket)
	{
		AddError(NOT_ENOUGHT_OPERANDS, l_count + p_mes_pos_shift);
		return;
	}
	while (!l_operations.empty())
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
	if (!l_operands.empty())
	{
		p_output_str.resize(CALC_BUFFER_SIZE);
		p_output_str.resize(snprintf(&p_output_str[0], CALC_BUFFER_SIZE - 1, CALC_INTERNAL_ACCURACY_FORMAT, l_operands.top()));
	}
	AddMessage(p_output_str);
}
//---------------------------------------------------------------------------
#endif // _LINEAR_CPP