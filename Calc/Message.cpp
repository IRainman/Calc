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
const wstring MessageString[] =
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
	L"число имеет слишком высокую точность %d максимальная поддерживаемая точность %d, вычисление может быть выполнено с ошибками",
// Ошибки
};
//---------------------------------------------------------------------------
static_assert(_countof(MessageString) != ERROR_LAST, "MessageString and MessageEnum sizes do not match ;) Check them out!");
//---------------------------------------------------------------------------
#define MESSAGE(code) MessageString[code]
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
/* TODO inline */void AddError(uint_8 p_message, wstring::size_type p_count/* = -1*/, wstring::size_type p_1/* = 0*/, wstring::size_type p_2/* = 0*/)
{
	m_NoError = false;
	wstring l_error;
	wchar_t l_1[6];
#ifdef _WIN64
	_ui64tow_s(p_1, l_1, 6, 10);
#else
	_itow_s(p_1, l_1, 6, 10);
#endif
	wchar_t l_2[6];
#ifdef _WIN64
	_ui64tow_s(p_2, l_2, 6, 10);
#else
	_itow_s(p_2, l_2, 6, 10);
#endif
	switch(p_message)
	{
		case 0:
			l_error = L"Недопустимый символ";
			break;
		case 1:
			l_error = L"Количество открывающих " + (wstring)l_1 +
				L" и закрывающих " + (wstring)l_2 +
				L" скобок не совпадает";
			break;
		case 2:
			l_error = L"Недопустимый символ после замены переменных и проверки на функции, проверьте правильность их написания";
			break;
#ifdef _USE_RPN
		case 3:
			l_error = L"Недопустимый символ во время преобразования в обратную польскую запись";
			break;
#endif // _USE_RPN
		case 4:
			l_error = L"Последовательная запись нескольких операций не подерживается";
			break;
		case 5:
			l_error = L"На ноль делить нельзя";
			break;
		case 6:
			l_error = L"У функции нехватает закрывающей скобки";
			break;
		case 7:
			l_error = L"У функции отсутсвуют аргументы";
			break;
		case 8:
			l_error = L"Недостаточно операндов для получения результата";
			break;
		case 9:
			l_error = L"Выражение не может начинатся со знака операции";
			break;
		case 10:
			l_error = L"У функции неверное число аргументов необходимо " + (wstring)l_1 + L", обнаружено " + (wstring)l_2;
			break;
		case 12:
			l_error = L"Выражение не может начинаться и заканчиваться скобкой";
			break;
		case 13:
			l_error = L"Пустые скобки";
			break;
		case 14:
			l_error = L"Открывающая скобка идёт сразу после закрывающей";
			break;
		case 15:
			l_error = L"Запись числа после закрывающей скобки недопустима";
			break;
		case 16:
			l_error = L"Выражение в скобках заканчивается знаком операции";
			break;
		case 17:
			l_error = L"Выражение в скобках начинается со знака операции";
			break;
		case 18:
			l_error = L"У функции отсутсвует " + (wstring)l_1 + L" аргумент, позиция внутри скобок функции " + (wstring)l_2;
			break;
		case 19:
			l_error = L"Запись функции после закрывающей скобки недопустима";
			break;
		case 20:
			l_error = L"Запись нескольких точек в одном числе недопустима, следующая точка в позиции " + (wstring)l_1;
			break;
		case 21:
			l_error = L"Последним символом в выражении не может быть знак операции";
			break;
		case 22:
			l_error = L"Запись нескольких символов \"e\" в одном числе недопустима, следующий символ \"e\" в позиции " + (wstring)l_1;
			break;
		case 23:
			l_error = L"Перед символом степени \"e\" не обнаружено число";
			break;
		case 24:
			l_error = L"Запись \"--\" после степенного символа \"e\" недопостима";
			break;
		case 25:
			l_error = L"После символа \"e\" не обнаружн показатель степени";
			break;
		case 26:
			l_error = L"Некорректное использование символа \"e\" сразу после показателя степени в позиции " + (wstring)l_1 + L" следует ещё один символ \"e\"";
			break;
		case 27:
			l_error = L"Запись \"+-\" после степенного символа \"e\" недопостима";
			break;
		case 28:
			l_error = L"Последним символом в выражении не может быть символ \"e\"";
			break;
		case 29:
			l_error = L"Закрывающая скобка находится раньше открывающей";
			break;
		case 30:
			l_error = L"Запись числа перед открывающей скобкой недопустима";
			break;
#ifdef _DEBUG
		case 252:
			l_error = L"DEBUG: Internal Processing Error: \"void ReplaceConstant(...)\"";
			break;
		case 253:
			l_error = L"DEBUG: Internal Processing Error: \"void CalculateFunction(...)\"";
			break;
		case 254:
			l_error = L"DEBUG: Internal Processing Error: \"bool CalculateOnLineExpression()\"";
			break;
		case 255:
			l_error = L"DEBUG: number of arguments passed to the function \"replace\" is not defined in the function of \"replace\", it is impossible to calculate the expression";
			break;
		default:
			l_error = L"DEBUG: Unknown Error!";
			break;
#endif
	}
	wchar_t l_wchar_buf[1024];
	if(p_count == -1)
	{
		swprintf_s(l_wchar_buf, L"Ошибка: %s\r\n", l_error.c_str());
	}
	else
	{
		swprintf_s(l_wchar_buf, L"В позиции %d ошибка: %s\r\n", p_count + m_Correct_count, l_error.c_str());
	}
#ifdef _DEBUG
	wchar_t l_temp_buf[1024];
	swprintf_s(l_temp_buf, L"AddError(%d... %s", p_message, l_wchar_buf);
	m_ErrorString += l_temp_buf;
#else
	m_ErrorString += l_wchar_buf;
#endif
}
//---------------------------------------------------------------------------
#endif // _MESSAGE_CPP