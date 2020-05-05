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

#pragma once
//---------------------------------------------------------------------------
using namespace std;
#include "Flags.h"
#include "MyTypes.h"
//---------------------------------------------------------------------------
enum MessageEnum
{
	MESSAGE_FIRST = 0,// Messages
	FOUND_SUBEXPRESSIONS = MESSAGE_FIRST,
	END_OF_THE_SUBEXPRESSION,
#ifdef ENABLE_INPUT_VALIDATION
	PREPARING,
	CALCULATION,
#endif // ENABLE_INPUT_VALIDATION
	REPLACEMENTS_OF_THE_CONSTANTS,
	CALCULATE_FUNCTIONS,
	MESSAGE_LAST = CALCULATE_FUNCTIONS,
#ifdef ENABLE_WARNINGS_IN_LOG
	WARNING_FIRST,// Warnings
	OUT_OF_RANGE = WARNING_FIRST,
	MAX_DIGITS,
	WARNING_LAST = MAX_DIGITS,
#endif // ENABLE_WARNINGS_IN_LOG
	ERROR_FIRST,// Errors
	UNKNOWN_ERROR = ERROR_FIRST,
#ifdef ENABLE_INPUT_VALIDATION
	EXPRESSION_NOT_FOUND,
	//INVALID_CHARACTER,
	INVALID_CHARACTER_AFTER_CONSTANT_AND_FUNCTION_PROCESS,
	NON_BRACKET_BALANCE,
	//  FUNCTION_LACKS_CLOSING_BRACKET,
	//  FUNCTION_NOT_HAVING_ARGUMENTS,
#endif // ENABLE_INPUT_VALIDATION
	CONSECUTIVE_RECORD_NUMBER_OF_TRANSACTIONS,
	NOT_ENOUGHT_OPERANDS,
	EXPRESSION_CAN_NOT_START_FROM_OPERATION,
#ifdef ENABLE_INPUT_VALIDATION
	EMPTY_BRACKETS,
	OPENING_BRACKET_AFTER_CLOSING,
	NUMBERS_AFTER_CLOSING_BRACKET,
	EXPRESSION_IN_BRACKETS_MARK_OPERATION_ENDING,
	EXPRESSION_IN_BRACKETS_START_WITH_OPERATION,
	FUNCTION_AFTER_CLOSING_BRACKET,
	MULTI_POINTS_IN_NUMBER_IS_UNACCEPTABLE,
	LAST_SYMBOL_IN_EXPRESSION_IS_OPERATION,
	MULTI_E_SYMBOL_IN_NUMBER,
	BEFORE_E_NOT_FOUND_NUMBER,
	RECORD_DOUBLE_MINUS_AFTER_E,
	AFTER_E_NOT_FOUND_EXPONENT,
	RECORD_PLUS_MINUS_AFTER_E,
	LAST_SYMBOL_IN_EXPRESSION_IS_E,
	CLOSING_BRACKET_BEFORE_OPENING,
	NUMBERS_BEFORE_OPENING_BRACKET,
#endif // ENABLE_INPUT_VALIDATION
	NUMBER_BEFORE_FUNCTION,
	NUMBER_AFTER_FUNCTION,
	FUNCTION_INVALID_NUMBER_OF_ARGUMENTS,
	LOST_FUNCTION_ARGUMENTS,
	FUNCTION_LOST_CLOSING_BRACKET,
#ifndef _DEBUG
	ERROR_LAST = FUNCTION_LOST_CLOSING_BRACKET,
#else
	INTERNAL_PROCESSING_ERROR_CalculateFunction,
	INTERNAL_PROCESSING_ERROR_CalculateOnLineExpression_p_operands_is_unknown,
	INTERNAL_PROCESSING_ERROR_CalculateOnLineExpression_p_operands_is_not_addition,
	INTERNAL_PROCESSING_ERROR_CalculateLineExpression_l_current_operand,
	INTERNAL_PROCESSING_ERROR_CalculateLineExpression_l_operands,
	INTERNAL_PROCESSING_ERROR_replace,
	ERROR_LAST = INTERNAL_PROCESSING_ERROR_replace,
#endif
};
//---------------------------------------------------------------------------
extern string m_message_string;
extern bool m_no_error;
extern string::size_type m_correct_count;
#ifdef EXTENDENT_REPORT_OF_POSITION_IN_LOG
extern vector<string::size_type> m_corrected_spaces;
#endif
//---------------------------------------------------------------------------
extern void AddMessage(const string_view& p_message);
extern void AddMessage(const MessageEnum p_message);
#ifdef ENABLE_WARNINGS_IN_LOG
extern void AddWarning(const MessageEnum p_message);
extern void AddWarning(const MessageEnum p_message, string::size_type p_1, string::size_type p_2);
#endif // ENABLE_WARNINGS_IN_LOG
extern void AddError(const MessageEnum p_message, string::size_type p_count = string::npos, string::size_type p_1 = 0, string::size_type p_2 = 0);
//---------------------------------------------------------------------------