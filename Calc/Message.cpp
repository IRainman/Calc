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
#ifndef _MESSAGE_CPP
#define _MESSAGE_CPP
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
// Сообщения
	"\tВычисление:",
#ifdef ENABLE_INPUT_VALIDATION
	"\tПодготовка:",
#endif
	"Обнаружено вложенное выражение:",
	"Конец вложенного выражения.",
	"\tЗамена констант:",
	"\tВычисление функций:",
#ifdef ENABLE_WARNINGS_IN_LOG
// Предупреждения
	"число выходит за рамки допустимого диапазона, вычисление может быть выполнено с ошибками",
	"число имеет слишком большое количество разрядов %d максимальное поддерживаемое %d, вычисление может быть выполнено с погрешностью",
#endif // ENABLE_WARNINGS_IN_LOG
// Ошибки
	"неопределённая ошибка",
#ifdef ENABLE_INPUT_VALIDATION
	"выражение не найдено",
	//"недопустимый символ",
	"недопустимый символ после замены констант и обработки функций, проверьте правильность их написания",
	"количество открывающих %d и закрывающих %d скобок не совпадает",
	//  "у функции нехватает закрывающей скобки",
	//  "у функции отсутсвуют аргументы",
#endif // ENABLE_INPUT_VALIDATION
	"последовательная запись нескольких операций не подерживается",
	"недостаточно операндов для получения результата",
	"выражение не может начинатся со знака операции",
#ifdef ENABLE_INPUT_VALIDATION
	"пустые скобки",
	"открывающая скобка идёт сразу после закрывающей",
	"запись числа после закрывающей скобки недопустима",
	"выражение в скобках заканчивается знаком операции",
	"выражение в скобках начинается со знака операции",
	"запись функции после закрывающей скобки недопустима",
	"запись нескольких точек в одном числе недопустима, следующая точка в позиции %d",
	"последним символом в выражении не может быть знак операции",
	"запись нескольких символов \"e\" в одном числе недопустима, следующий символ \"e\" в позиции %d",
	"перед символом степени \"e\" не обнаружено число",
	"запись \"--\" после степенного символа \"e\" недопустима",
	"после символа \"e\" не обнаружен показатель степени",
	"запись \"+-\" после степенного символа \"e\" недопостима",
	"последним символом в выражении не может быть символ \"e\"",
	"закрывающая скобка находится раньше открывающей",
	"запись числа перед открывающей скобкой недопустима",
#endif // ENABLE_INPUT_VALIDATION
	"у функции неверное число аргументов необходимо %d, обнаружено %d",
	"у функции отсутствует %d аргумент, позиция внутри скобок функции %d",
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
static_assert(_countof(c_message_string) == ERROR_LAST + 1, "MessageString and MessageEnum sizes do not match ;) Check them out!");
//---------------------------------------------------------------------------
inline constexpr auto& MESSAGE(const MessageEnum code)
{
	return c_message_string[code];
}
inline constexpr const auto* CMESSAGE(const MessageEnum code)
{
	return MESSAGE(code).data();
}
//---------------------------------------------------------------------------
inline void AddMessage(const string_view& p_message)
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
#endif // ENABLE_LOG_DEBUG
	AddMessage(MESSAGE(p_message));
}
//---------------------------------------------------------------------------
#ifdef ENABLE_WARNINGS_IN_LOG
inline void AddWarning(const MessageEnum p_message)
{
#ifdef ENABLE_LOG_DEBUG
	char l_temp_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_temp_buf, MESSAGE_BUFFER_SIZE - 1, "AddWarning(%u... ", p_message);
	m_message_string += l_temp_buf;
	if (p_message < WARNING_FIRST || p_message > WARNING_LAST)
		m_message_string += " DEBUG: Unknown Warning!";
#endif // ENABLE_LOG_DEBUG
	m_message_string += "Внимание: ";
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
#endif // ENABLE_LOG_DEBUG
	char l_pre_message_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_pre_message_buf, MESSAGE_BUFFER_SIZE - 1, "Внимание: %s", CMESSAGE(p_message)); //-V111
	char l_out_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_out_buf, MESSAGE_BUFFER_SIZE - 1, l_pre_message_buf, p_1, p_2); //-V111
	AddMessage(l_out_buf);
}
#endif // ENABLE_WARNINGS_IN_LOG
//---------------------------------------------------------------------------
void AddError(const MessageEnum p_message, string::size_type p_count/* = string::npos*/, string::size_type p_1/* = 0*/, string::size_type p_2/* = 0*/)
{
	m_no_error = false;
	char l_pre_message_buf[MESSAGE_BUFFER_SIZE];
	if (p_count == string::npos)
	{
		snprintf(l_pre_message_buf, MESSAGE_BUFFER_SIZE - 1, "Ошибка: %s", CMESSAGE(p_message)); //-V111
	}
	else
	{
		const auto l_i = p_count - m_correct_count;
#ifdef EXTENDENT_REPORT_OF_POSITION_IN_LOG
		m_corrected_spaces.push_back(l_i);
#endif
		snprintf(l_pre_message_buf, MESSAGE_BUFFER_SIZE - 1, "В позиции %zd ошибка: %s", l_i, CMESSAGE(p_message)); //-V111
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
#endif // ENABLE_LOG_DEBUG
}
//---------------------------------------------------------------------------
#endif // _MESSAGE_CPP