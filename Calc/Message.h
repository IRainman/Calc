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

#ifndef _MESSAGE_H
#define _MESSAGE_H
//---------------------------------------------------------------------------
using namespace std;
#include <string>
#include "MyTypes.h"
#include "Flags.h"
//---------------------------------------------------------------------------
extern wstring m_ErrorString;
extern bool m_NoError;
extern wstring::size_type m_Correct_count;
extern void AddMessage(const wstring& p_message);
extern void AddMessage(uint_8 p_message);
extern void AddWarning(uint_8 p_message);
extern void AddError(uint_8 p_message, wstring::size_type p_count = -1, wstring::size_type p_1 = 0, wstring::size_type p_2 = 0);
//---------------------------------------------------------------------------
enum MessageEnum
{
	MESSAGE_FIRST,
	PREPARING = MESSAGE_FIRST,
	PARSING_A_STRING,
//	CALCULATION,
#ifdef _USE_RPN
	RPN,
#endif
	FOUND_SUBEXPRESSIONS,
	END_OF_THE_SUBEXPRESSION,
	REPLACEMENTS_OF_THE_CONSTANTS,
	CALCULATION_FUNCTIONS,
	MESSAGE_LAST = CALCULATION_FUNCTIONS,

	WARNING_FIRST,
	OUT_OF_RANGE = WARNING_FIRST,
	LOW_ACCURACY, 
	WARNING_LAST = LOW_ACCURACY,

	ERROR_FIRST,
	ERROR_LAST
};
//---------------------------------------------------------------------------
#endif