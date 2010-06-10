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

#ifdef _MSC_VER
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
wstring m_ErrorString;
bool m_NoError;
wstring::size_type m_Correct_count;
//---------------------------------------------------------------------------
const static wchar_t* MessageString[] =
{
// Сообщения
	L"\tПодготовка:",
	L"\tРазбор строки:",
//	L"\tВычисление:",
#ifdef _USE_RPN
	L"\tRPN:",
#endif
	L"Обнаружено вложенное выражение:",
	L"Конец вложенного выражения.",
	L"\tЗамена констант:",
	L"\tВычисление функций:",
// Предупреждения
	L"число слишком большое, вычисление может быть выполнено с ошибками",
//	L"число имеет слишком высокую точность %d максимальная поддерживаемая точность %d, вычисление может быть выполнено с ошибками",
// Ошибки
	L"Недопустимый символ",
	L"Количество открывающих %d и закрывающих %d скобок не совпадает",
	L"Недопустимый символ после замены переменных и проверки на функции, проверьте правильность их написания",
#ifdef _USE_RPN
	L"Недопустимый символ во время преобразования в обратную польскую запись",
#endif
	L"Последовательная запись нескольких операций не подерживается",
	L"На ноль делить нельзя",
//	L"У функции нехватает закрывающей скобки",
//	L"У функции отсутсвуют аргументы",
	L"Недостаточно операндов для получения результата",
	L"Выражение не может начинатся со знака операции",
	L"У функции неверное число аргументов необходимо %d, обнаружено %d",
	L"Пустые скобки",
	L"Открывающая скобка идёт сразу после закрывающей",
	L"Запись числа после закрывающей скобки недопустима",
	L"Выражение в скобках заканчивается знаком операции",
	L"Выражение в скобках начинается со знака операции",
	L"У функции отсутствует %d аргумент, позиция внутри скобок функции %d",
	L"Запись функции после закрывающей скобки недопустима",
	L"Запись нескольких точек в одном числе недопустима, следующая точка в позиции %d",
	L"Последним символом в выражении не может быть знак операции",
	L"Запись нескольких символов \"e\" в одном числе недопустима, следующий символ \"e\" в позиции %d",
	L"Перед символом степени \"e\" не обнаружено число",
	L"Запись \"--\" после степенного символа \"e\" недопустима",
	L"После символа \"e\" не обнаружен показатель степени",
//	L"Некорректное использование символа \"e\" сразу после показателя степени в позиции %d следует ещё один символ \"e\"",
	L"Запись \"+-\" после степенного символа \"e\" недопостима",
	L"Последним символом в выражении не может быть символ \"e\"",
	L"Закрывающая скобка находится раньше открывающей",
	L"Запись числа перед открывающей скобкой недопустима",
#ifdef _DEBUG
	L"DEBUG: Internal Processing Error: \"void CalculateFunction(...)\"",
	L"DEBUG: Internal Processing Error: \"bool CalculateOnLineExpression()\"",
	L"DEBUG: number of arguments passed to the function \"replace\" is not defined in the function of \"replace\", it is impossible to calculate the expression",
#endif
};
//---------------------------------------------------------------------------
static_assert(_countof(MessageString) == ERROR_LAST + 1, "MessageString and MessageEnum sizes do not match ;) Check them out!");
//---------------------------------------------------------------------------
#define MESSAGE(code) wstring(MessageString[code]) // TODO optimize this, refactoring witout string!
//---------------------------------------------------------------------------
inline void AddMessage(const wstring& p_message)
{
	m_ErrorString += p_message + L"\r\n";
}
//---------------------------------------------------------------------------
inline void AddMessage(uint_8 p_message)
{
#ifdef _DEBUG
	wchar_t l_temp_buf[1024];
	swprintf_s(l_temp_buf, L"AddMessage(%d... ", p_message);
	m_ErrorString += l_temp_buf;
	if(p_message < MESSAGE_FIRST || p_message > MESSAGE_LAST)
		m_ErrorString += L" DEBUG: Unknown Message!";
#endif
	m_ErrorString += MESSAGE(p_message) + L"\r\n";
}
//---------------------------------------------------------------------------
inline void AddWarning(uint_8 p_message)
{
#ifdef _DEBUG
	wchar_t l_temp_buf[1024];
	swprintf_s(l_temp_buf, L"AddWarning(%d... ", p_message);
	m_ErrorString += l_temp_buf;
	if(p_message < WARNING_FIRST || p_message > WARNING_LAST)
		m_ErrorString += L" DEBUG: Unknown Warning!";
#endif
	m_ErrorString += L"Внимание: " + MESSAGE(p_message) + L"\r\n";
}
//---------------------------------------------------------------------------
void AddError(uint_8 p_message, wstring::size_type p_count/* = -1*/, wstring::size_type p_1/* = 0*/, wstring::size_type p_2/* = 0*/)
{
	m_NoError = false;
	wchar_t l_pre_message_buf[1024];
	if(p_count == -1)
	{
		swprintf_s(l_pre_message_buf, L"Ошибка: %s\r\n", MESSAGE(p_message).c_str());
	}
	else
	{
		swprintf_s(l_pre_message_buf, L"В позиции %d ошибка: %s\r\n", p_count + m_Correct_count, MESSAGE(p_message).c_str());
	}
	wchar_t l_out_buf[1024];
	swprintf_s(l_out_buf, l_pre_message_buf, p_1, p_2);
#ifdef _DEBUG
	wchar_t l_temp_buf[1024];
	swprintf_s(l_temp_buf, L"AddError(%d... %s", p_message, l_out_buf);
	m_ErrorString += l_temp_buf;
	if(p_message < ERROR_FIRST || p_message > ERROR_LAST)
		m_ErrorString += L" DEBUG: Unknown Error!";
#else
	m_ErrorString += l_out_buf;
#endif
}
//---------------------------------------------------------------------------
#endif // _MESSAGE_CPP