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
#include <string>
#include "Flags.h"
#include "MyTypes.h"
//---------------------------------------------------------------------------
enum class Priority
{
    function    = 16,
    power       = 8,
    multiply    = 4, //-V112
    addition    = 2,
    bracket     = 1,
    number      = 0,
    error       = -1
};
//---------------------------------------------------------------------------
constexpr inline Priority GetPriority(const char p_sym)
{
	switch (p_sym)
	{
		case '^':
			//      case '%':
			return Priority::power;
			
		case '*':
		case '/':
			return Priority::multiply;
			
		case '+':
		case '-':
			return Priority::addition;
			
		case '(':
		case ')':
			return Priority::bracket;
			
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
			return Priority::number;
			
		default:
			return Priority::error;
	}
}
//---------------------------------------------------------------------------
extern void CalculateLineExpression(string p_input_str, string& p_output_str, string::size_type p_mes_pos_shift = 0);
//---------------------------------------------------------------------------
#endif // _LINEAR_H