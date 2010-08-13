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
	MESSAGE_FIRST = 0,// Сообщения
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
	
	WARNING_FIRST,// Предупреждения
	OUT_OF_RANGE = WARNING_FIRST,
//	LOW_ACCURACY,
	WARNING_LAST = OUT_OF_RANGE,
	
	ERROR_FIRST,// Ошибки
	INVALID_CHARACTER = ERROR_FIRST,
	NON_BRACKET_BALANCE,
	INVALID_CHARACTER_AFTER_VAR_AND_FUNC,
#ifdef _USE_RPN
	INVALID_CHARACTER_ON_RPN,
#endif
	CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS,
	DIVIDE_BY_ZERO,
//	FUNCTION_LACKS_CLOSING_BRACKET,
//	FUNCTION_NOT_HAVING_ARGUMENTS,
	NOT_ENOUGHT_OPERANDS,
	EXPRESSION_CAN_NOT_START_FROM_OPERATION,
	FUNCTION_INVALID_NUMBER_OF_ARGUMENTS,
	EMPTY_BRACKETS,
	OPENING_BRACKET_AFTER_CLOSING,
	NUMBERS_AFTER_CLOSING_BRACKET,
	EXPRESSION_IN_BRACKETS_MARK_OPERATION_ENDING,
	EXPRESSION_IN_BRACKETS_START_WITH_OPERATION,
	LOST_FUNCTION_ARGUMENTS,
	FUNCTION_AFTER_CLOSING_BRACKET,
	MULTI_POINTS_IN_NUMBER_IS_UNACCEPTABLE,
	LAST_SYMBOL_IN_EXPRESSION_IS_OPERATION,
	MULTI_E_SYMBOL_IN_NUMBER,
	BEFORE_E_NOT_FOUND_NUMBER,
	RECORD_DOUBLE_MINUS_AFTER_E,
	AFTER_E_NOT_FOUND_EXPONENT,
//	AFTER_E_FOUND_ANOTHER_E,
	RECORD_PLUS_MINUS_AFTER_E,
	LAST_SYMBOL_IN_EXPRESSION_IS_E,
	CLOSING_BRACKET_BEFORE_OPENING,
	NUMBERS_BEFORE_OPENING_BRACKET,
#ifdef _DEBUG
	INTERNAL_PROCESSING_ERROR_CalculateFunction,
	INTERNAL_PROCESSING_ERROR_CalculateOnLineExpression,
	INTERNAL_PROCESSING_ERROR_replace,
	ERROR_LAST = INTERNAL_PROCESSING_ERROR_replace,
#else
	ERROR_LAST = NUMBERS_BEFORE_OPENING_BRACKET,
#endif
};
//---------------------------------------------------------------------------
#endif