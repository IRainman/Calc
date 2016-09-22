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
//---------------------------------------------------------------------------
#define MESSAGE_BUFFER_SIZE 1024
//---------------------------------------------------------------------------
const static char* c_message_string[] =
{
// Сообщения
	"\tПодготовка:",
	"\tРазбор строки:",
//	"\tВычисление:",
#ifdef _USE_RPN
	"\tRPN:",
#endif
	"Обнаружено вложенное выражение:",
	"Конец вложенного выражения.",
	"\tЗамена констант:",
	"\tВычисление функций:",
// Предупреждения
	"число слишком большое, вычисление может быть выполнено с ошибками",
//	"число имеет слишком высокую точность %d максимальная поддерживаемая точность %d, вычисление может быть выполнено с ошибками",
// Ошибки
	"Выражение не найдено",
	"Недопустимый символ",
	"Количество открывающих %d и закрывающих %d скобок не совпадает",
	"Недопустимый символ после замены переменных и проверки на функции, проверьте правильность их написания",
#ifdef _USE_RPN
	"Недопустимый символ во время преобразования в обратную польскую запись",
#endif
	"Последовательная запись нескольких операций не подерживается",
	"На ноль делить нельзя",
//	"У функции нехватает закрывающей скобки",
//	"У функции отсутсвуют аргументы",
	"Недостаточно операндов для получения результата",
	"Выражение не может начинатся со знака операции",
	"У функции неверное число аргументов необходимо %d, обнаружено %d",
	"Пустые скобки",
	"Открывающая скобка идёт сразу после закрывающей",
	"Запись числа после закрывающей скобки недопустима",
	"Выражение в скобках заканчивается знаком операции",
	"Выражение в скобках начинается со знака операции",
	"У функции отсутствует %d аргумент, позиция внутри скобок функции %d",
	"Запись функции после закрывающей скобки недопустима",
	"Запись нескольких точек в одном числе недопустима, следующая точка в позиции %d",
	"Последним символом в выражении не может быть знак операции",
	"Запись нескольких символов \"e\" в одном числе недопустима, следующий символ \"e\" в позиции %d",
	"Перед символом степени \"e\" не обнаружено число",
	"Запись \"--\" после степенного символа \"e\" недопустима",
	"После символа \"e\" не обнаружен показатель степени",
//	"Некорректное использование символа \"e\" сразу после показателя степени в позиции %d следует ещё один символ \"e\"",
	"Запись \"+-\" после степенного символа \"e\" недопостима",
	"Последним символом в выражении не может быть символ \"e\"",
	"Закрывающая скобка находится раньше открывающей",
	"Запись числа перед открывающей скобкой недопустима",
#ifdef _DEBUG
	"DEBUG: Internal Processing Error: \"void CalculateFunction(...)\"",
	"DEBUG: Internal Processing Error: \"bool CalculateOnLineExpression()\"",
	"DEBUG: number of arguments passed to the function \"replace\" is not defined in the function of \"replace\", it is impossible to calculate the expression",
#endif
};
//---------------------------------------------------------------------------
static_assert(_countof(c_message_string) == ERROR_LAST + 1, "MessageString and MessageEnum sizes do not match ;) Check them out!");
//---------------------------------------------------------------------------
#define MESSAGE(code) (c_message_string[code])
//---------------------------------------------------------------------------
inline void AddMessage(const string& p_message)
{
	m_message_string += p_message + "\r\n";
}
//---------------------------------------------------------------------------
inline void AddMessage(const MessageEnum p_message)
{
#ifdef _DEBUG
	char l_temp_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_temp_buf, MESSAGE_BUFFER_SIZE - 1, "AddMessage(%u... ", p_message);
	m_message_string += l_temp_buf;
	if (p_message > MESSAGE_LAST)
		m_message_string += " DEBUG: Unknown Message!";
#endif
	m_message_string += MESSAGE(p_message);
	m_message_string += "\r\n";
}
//---------------------------------------------------------------------------
inline void AddWarning(const MessageEnum p_message)
{
#ifdef _DEBUG
	char l_temp_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_temp_buf, MESSAGE_BUFFER_SIZE - 1, "AddWarning(%u... ", p_message);
	m_message_string += l_temp_buf;
	if (p_message < WARNING_FIRST || p_message > WARNING_LAST)
		m_message_string += " DEBUG: Unknown Warning!";
#endif
	m_message_string += "Внимание: ";
	m_message_string += MESSAGE(p_message);
	m_message_string += "\r\n";
}
//---------------------------------------------------------------------------
void AddError(const MessageEnum p_message, string::size_type p_count/* = -1*/, string::size_type p_1/* = 0*/, string::size_type p_2/* = 0*/)
{
	m_no_error = false;
	char l_pre_message_buf[MESSAGE_BUFFER_SIZE];
	if (p_count == -1)
	{
		snprintf(l_pre_message_buf, MESSAGE_BUFFER_SIZE - 1, "Ошибка: %s\r\n", MESSAGE(p_message)); //-V111
	}
	else
	{
		snprintf(l_pre_message_buf, MESSAGE_BUFFER_SIZE - 1, "В позиции %zd ошибка: %s\r\n", p_count + m_correct_count, MESSAGE(p_message)); //-V111
	}
	char l_out_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_out_buf, MESSAGE_BUFFER_SIZE - 1, l_pre_message_buf, p_1, p_2); //-V111
#ifdef _DEBUG
	char l_temp_buf[MESSAGE_BUFFER_SIZE];
	snprintf(l_temp_buf, MESSAGE_BUFFER_SIZE - 1, "AddError(%u... %s", p_message, l_out_buf); //-V111
	m_message_string += l_temp_buf;
	if (p_message < ERROR_FIRST || p_message > ERROR_LAST)
		m_message_string += " DEBUG: Unknown Error!";
#else
	m_message_string += l_out_buf;
#endif
}
//---------------------------------------------------------------------------
#endif // _MESSAGE_CPP