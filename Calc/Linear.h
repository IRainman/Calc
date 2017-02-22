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

#ifndef _LINEAR_H
#define _LINEAR_H
//---------------------------------------------------------------------------
using namespace std;
#include <cmath>
#include <limits>
#include <stack>
#include "Flags.h"
#include "MyTypes.h"
//---------------------------------------------------------------------------
enum Priority
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
extern Priority GetPriority(char p_sym);
extern void CalculateLineExpression(string p_input_str, string& p_output_str, string::size_type p_mes_pos_shift = 0);
//---------------------------------------------------------------------------
#endif // _LINEAR_H