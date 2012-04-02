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

#ifndef _RPN_H
#define _RPN_H
//---------------------------------------------------------------------------
using namespace std;
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>
#include <string>
#include <stack>
#include "Flags.h"
#include "MyTypes.h"
//---------------------------------------------------------------------------
enum
{
	priority_function       = 16,
	priority_power          = 8,
	priority_multiply       = 4, //-V112
	priority_addition       = 2,
	priority_bracket        = 1,
	priority_default        = 0,
	priority_error          = -1
};
//---------------------------------------------------------------------------
extern int GetPriority(wchar_t p_sym);
extern void CalculateOnLineExpression(stack<wchar_t>& c_operations, stack<long double>& c_operands);
extern void CalculateLineExpression(wstring p_input_str, wstring& p_output_str);
#ifdef _USE_RPN
extern void CalculateRPN(wstring& p_to_process_str);
#endif //_USE_RPN
//---------------------------------------------------------------------------
#endif