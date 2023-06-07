// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "stdafx.h"
//---------------------------------------------------------------------------
#include "Message.h"
//---------------------------------------------------------------------------
string m_message_string;
bool m_no_error;
string::size_type m_correct_count;
#ifdef EXTENDENT_REPORT_OF_POSITION_IN_LOG
vector<string::size_type> m_corrected_spaces;
#endif
//---------------------------------------------------------------------------
constexpr size_t MESSAGE_BUFFER_SIZE = 1024;
//---------------------------------------------------------------------------
constexpr static const string_view c_message_string[] =
{
// Messages
	"An nested expression was found:",
	"End of nested expression.",
#ifdef ENABLE_INPUT_VALIDATION
	"\tPreparation:",
	"\tCalculation:",
#endif
	"\tReplacing constants:",
	"\tCalculating functions:",
// Warnings
	"the number is out of the acceptable range, the calculation may be performed with errors",
	"the number has too many digits %d the maximum supported %d, the calculation can be performed with an error",
// Errors
	"undefined error",
#ifdef ENABLE_INPUT_VALIDATION
	"expression not found",
	"invalid character after replacing constants and processing functions, check the correctness of their spelling",
	"the number of opening %d and closing %d brackets does not match",
#endif
	"sequential recording of multiple operations isn't supported",
	"not enough operands to get the result",
	"an expression cannot start with an operation sign",
#ifdef ENABLE_INPUT_VALIDATION
	"empty brackets",
	"the opening bracket comes right after the closing one",
	"writing a number after the closing parenthesis is not allowed",
	"the expression in parentheses ends with an operation sign",
	"the expression in parentheses begins with the operation sign",
	"writing a function after the closing parenthesis is not allowed",
	"writing multiple points in one number is not allowed, the next point is at position %d",
	"the last character in the expression cant'be the operation sign",
	"writing multiple characters \"e\" in one number is not allowed, the next character \"e\" in position %d",
	"no number found before the degree symbol \"e\"",
	"the entry \"--\" after the power symbol \"e\" is not allowed",
	"no exponent was found after the \"e\" symbol",
	"the entry \"+-\" after the power symbol \"e\" is not allowed",
	"the last character in the expression cannot be the \"e\" symbol"",
	"the closing bracket is located before the opening one",
	"writing a number before the opening parenthesis is not allowed",
#endif
	"writing a number before a function is not allowed",
	"writing a number after a function is not allowed",
	"the function has an incorrect number of arguments:% d is needed, % d is detected",
	"the function has no % d argument, the position inside the brackets of the function % d",
	"the function is missing a closing parenthesis",
#ifdef _DEBUG
	"DEBUG: Internal Processing Error: \"void CalculateFunction(...)\"",
	"DEBUG: Internal Processing Error: \"void CalculateOnLineExpression(...): p_operands is unknown\"",
	"DEBUG: Internal Processing Error: \"void CalculateOnLineExpression(...): p_operands is not addition\"",
	"DEBUG: Internal Processing Error: \"void CalculateLineExpression(...): l_current_operand is invalid\"",
	"DEBUG: Internal Processing Error: \"void CalculateLineExpression(...): l_operands.size() > 1 after processing\"",
	"DEBUG: number of arguments passed to the function \"replace\" is not defined in the function of \"replace\", it is impossible to calculate the expression",
#endif
};
//---------------------------------------------------------------------------
static_assert(_countof(c_message_string) == static_cast<unsigned int>(MessageEnum::ERROR_LAST) + 1, "MessageString and MessageEnum sizes do not match ;) Check them out!");
//---------------------------------------------------------------------------
inline constexpr auto& MESSAGE(const MessageEnum code)
{
	return c_message_string[static_cast<unsigned int>(code)];
}
inline constexpr const auto* CMESSAGE(const MessageEnum code)
{
	return MESSAGE(code).data();
}
//---------------------------------------------------------------------------
inline void AddMessage(const string_view p_message)
{
	m_message_string += p_message;
	m_message_string += "\r\n";
}
//---------------------------------------------------------------------------
inline void AddMessage(const MessageEnum p_message)
{
#ifdef ENABLE_LOG_DEBUG
	char l_temp_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_temp_buf, MESSAGE_BUFFER_SIZE - 1, "AddMessage(%u... ", p_message);
	m_message_string += l_temp_buf;
	if (p_message > MESSAGE_LAST)
		m_message_string += " DEBUG: Unknown Message!";
#endif
	AddMessage(MESSAGE(p_message));
}
//---------------------------------------------------------------------------
inline void AddWarning(const MessageEnum p_message)
{
#ifdef ENABLE_LOG_DEBUG
	char l_temp_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_temp_buf, MESSAGE_BUFFER_SIZE - 1, "AddWarning(%u... ", p_message);
	m_message_string += l_temp_buf;
	if (p_message < WARNING_FIRST || p_message > WARNING_LAST)
		m_message_string += " DEBUG: Unknown Warning!";
#endif
	m_message_string += "Attention: ";
	AddMessage(MESSAGE(p_message));
}
//---------------------------------------------------------------------------
void AddWarning(const MessageEnum p_message, string::size_type p_1, string::size_type p_2)
{
#ifdef ENABLE_LOG_DEBUG
	char l_temp_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_temp_buf, MESSAGE_BUFFER_SIZE - 1, "AddWarning(%u... ", p_message);
	m_message_string += l_temp_buf;
	if (p_message < WARNING_FIRST || p_message > WARNING_LAST)
		m_message_string += " DEBUG: Unknown Warning!";
#endif
	char l_pre_message_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_pre_message_buf, MESSAGE_BUFFER_SIZE - 1, "Attention: %s", CMESSAGE(p_message)); //-V111
	char l_out_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_out_buf, MESSAGE_BUFFER_SIZE - 1, l_pre_message_buf, p_1, p_2); //-V111
	AddMessage(l_out_buf);
}
//---------------------------------------------------------------------------
void AddError(const MessageEnum p_message, string::size_type p_count/* = string::npos*/, string::size_type p_1/* = 0*/, string::size_type p_2/* = 0*/)
{
	m_no_error = false;
	char l_pre_message_buf[MESSAGE_BUFFER_SIZE];
	if (p_count == string::npos)
	{
		snprintf(l_pre_message_buf, MESSAGE_BUFFER_SIZE - 1, "Error: %s", CMESSAGE(p_message)); //-V111
	}
	else
	{
		const auto l_i = p_count - m_correct_count;
#ifdef EXTENDENT_REPORT_OF_POSITION_IN_LOG
		m_corrected_spaces.push_back(l_i); ???
#endif
		snprintf(l_pre_message_buf, MESSAGE_BUFFER_SIZE - 1, "Error at position %zd: %s", l_i, CMESSAGE(p_message)); //-V111
	}
	char l_out_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_out_buf, MESSAGE_BUFFER_SIZE - 1, l_pre_message_buf, p_1, p_2); //-V111
	AddMessage(l_out_buf);
#ifdef ENABLE_LOG_DEBUG
	char l_temp_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_temp_buf, MESSAGE_BUFFER_SIZE - 1, "AddError(%u... %s", p_message, l_out_buf); //-V111
	m_message_string += l_temp_buf;
	if (p_message < ERROR_FIRST || p_message > ERROR_LAST)
		m_message_string += " DEBUG: Unknown Error!";
#endif
}
//---------------------------------------------------------------------------