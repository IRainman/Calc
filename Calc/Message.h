/*
 * Copyright 2009-2016 Solomin Alexey Leonovich, a.rainman on gmail point com
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
#ifdef 0
#include <vector>
#endif
#include "MyTypes.h"
#include "Flags.h"
//---------------------------------------------------------------------------
enum MessageEnum
{
	MESSAGE_FIRST = 0,// Сообщения
	PREPARING = MESSAGE_FIRST,
	//  PARSING_A_STRING,
	CALCULATION,
	FOUND_SUBEXPRESSIONS,
	END_OF_THE_SUBEXPRESSION,
	REPLACEMENTS_OF_THE_CONSTANTS,
	CALCULATE_FUNCTIONS,
	MESSAGE_LAST = CALCULATE_FUNCTIONS,
#ifdef ENABLE_WARNINGS_IN_LOG
	WARNING_FIRST,// Предупреждения
	OUT_OF_RANGE = WARNING_FIRST,
	LOW_ACCURACY,
	WARNING_LAST = LOW_ACCURACY,
#endif // ENABLE_WARNINGS_IN_LOG
	ERROR_FIRST,// Ошибки
	EXPRESSION_NOT_FOUND = ERROR_FIRST,
	INVALID_CHARACTER,
	INVALID_CHARACTER_AFTER_CONSTANT_AND_FUNCTION_PROCESS,
	NON_BRACKET_BALANCE,
	CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS,
	//  FUNCTION_LACKS_CLOSING_BRACKET,
	//  FUNCTION_NOT_HAVING_ARGUMENTS,
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
	//  AFTER_E_FOUND_ANOTHER_E,
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
extern string m_message_string;
extern bool m_no_error;
extern string::size_type m_correct_count;
#ifdef 0
extern vector<string::size_type> m_corrected_spaces;
#endif
//---------------------------------------------------------------------------
extern void AddMessage(const string& p_message);
extern void AddMessage(const MessageEnum p_message);
#ifdef ENABLE_WARNINGS_IN_LOG
extern void AddWarning(const MessageEnum p_message);
extern void AddWarning(const MessageEnum p_message, string::size_type p_1, string::size_type p_2);
#endif // ENABLE_WARNINGS_IN_LOG
extern void AddError(const MessageEnum p_message, string::size_type p_count = string::npos, string::size_type p_1 = 0, string::size_type p_2 = 0);
//---------------------------------------------------------------------------
#endif