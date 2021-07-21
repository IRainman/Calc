// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2016 Solomina Elle Leonovna, a.rainman on gmail point com
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

#include "stdafx.h"
//---------------------------------------------------------------------------
#ifdef _WIN32
//---------------------------------------------------------------------------
#include "CalculatorWideAdapter.h"

using namespace std;

#ifdef NEW_CODECVT_CONVERTER


std::wstring utf8_decode(const std::string& str)
{
	TODO cleanup.
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

// TODO, how?

// https://en.cppreference.com/w/cpp/locale/codecvt

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
	template<class ...Args>
	deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
	~deletable_facet() {}
};



typedef deletable_facet<std::codecvt_byname<char16_t, char, std::mbstate_t>> gbfacet_t;


static std::u16string m_message_string;

// using standard (locale-independent) codecvt facet
std::wstring_convert<
	deletable_facet<std::codecvt<char16_t, char8_t, std::mbstate_t>>, char16_t> m_converter;

typedef u16string refactoring_wstring;

#else // NEW_CODECVT_CONVERTER

static wstring_convert<codecvt_utf8_utf16<wchar_t>> m_converter;

typedef wstring refactoring_wstring;

#endif // NEW_CODECVT_CONVERTER

static refactoring_wstring m_message_string;
//---------------------------------------------------------------------------
inline refactoring_wstring to_wstring(const string& p_text)
{
	return m_converter.from_bytes(p_text);
}
//---------------------------------------------------------------------------
inline string to_string(const refactoring_wstring& p_text)
{
	return m_converter.to_bytes(p_text);
}
//---------------------------------------------------------------------------
extern const string& Calculate(string p_input, string& p_output);

refactoring_wstring& Calculate(const refactoring_wstring& p_input, refactoring_wstring& p_output)
{
	string l_output;
	
	m_message_string = to_wstring(Calculate(to_string(p_input), l_output));
	
	p_output = to_wstring(l_output);
	
	return m_message_string;
}
//---------------------------------------------------------------------------
#endif //  _WIN32
//---------------------------------------------------------------------------