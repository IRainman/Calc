// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2016 Solomin Alexey Leonovich, a.rainman on gmail point com
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
#ifndef _CALCULATOR_WIDE_ADAPTER_CPP
#define _CALCULATOR_WIDE_ADAPTER_CPP

#include "CalculatorWideAdapter.h"
#include <locale>
#include <codecvt>

using namespace std;

static wstring m_message_string;
static wstring_convert<codecvt_utf8_utf16<wchar_t>> m_converter;
//---------------------------------------------------------------------------
inline wstring to_wstring(const string& p_text)
{
	return m_converter.from_bytes(p_text);
}
//---------------------------------------------------------------------------
inline string to_string(const wstring& p_text)
{
	return m_converter.to_bytes(p_text);
}
//---------------------------------------------------------------------------
extern void CalculatorInit();
//---------------------------------------------------------------------------
extern const string& Calculate(string p_input, string& p_output);

wstring& Calculate(const wstring& p_input, wstring& p_output)
{
	string l_output;
	
	m_message_string = to_wstring(Calculate(to_string(p_input), l_output));
	
	p_output = to_wstring(l_output);
	
	return m_message_string;
}
//---------------------------------------------------------------------------
#endif // _CALCULATOR_WIDE_ADAPTER_CPP