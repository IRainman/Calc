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
#ifndef _CONSTANT_CPP
#define _CONSTANT_CPP
//---------------------------------------------------------------------------
#include "Flags.h"
#include "Constant.h"
#include "Message.h"
#include "Linear.h"
//---------------------------------------------------------------------------
const static char* c_str_constant[] =
{
	"pi",// Pi, Archimedes' constant or Ludolph's number
	"c",// Speed of light in vacuum (m·s-1)
	"G",// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
	"J",// Constants of Gauss fild
	"phi",// Golden ratio
	"h",// Planck constant (J·s)
	"atm",// Standard atmosphere (Pa), this constant valid on Earths only ;)
	"L",// Avogadro's number (mol−1)
	"R",// Gas constant (J·K−1·mol−1)
};
//---------------------------------------------------------------------------
const static calc_variable c_cv_constant[] =
{
	3.14159265358979323846264338327950288L,// Pi, Archimedes' constant or Ludolph's number
	299792458.0L,// Speed of light in vacuum (m·s-1)
	6.67428676767676767676767676767676767e-11L,// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
	3.058198247456354132564564787888767L,// Constants of Gauss fild
	1.61803398874989484820458683436563812L,// Golden ratio
	6.62606896333333333333333333333333333e-34L,// Planck constant (J·s)
	101325.0L,// Standard atmosphere (Pa), this constant valid on Earths only ;)
	6.02214151010101010101010101010101010e23L,// Avogadro's number (mol−1)
	8.31447215151515151515151515151515151L,// Gas constant (J·K−1·mol−1)
};
//---------------------------------------------------------------------------
static_assert(_countof(c_str_constant) == _countof(c_cv_constant) && _countof(c_cv_constant) == constant_counts, "c_str_constant and c_l_constant sizes do not match ;) Check them out!");
//---------------------------------------------------------------------------
const calc_variable GetConstant(Constants p_const_ind)
{
	return c_cv_constant[p_const_ind];
}
//---------------------------------------------------------------------------
inline void ReplaceConstant(string& p_io_str, const string& p_const_name, const size_t p_number_of_constant)
{
	string::size_type l_pos;
	
	auto ConstantNotFound = [](const string & p_io_str, const string & p_const_name, string::size_type & l_pos) -> bool
	{
		l_pos = p_io_str.find(p_const_name);
		return (l_pos == string::npos \
		|| \
		(l_pos \
		&& (GetPriority(p_io_str.c_str()[l_pos - 1]) < priority_bracket \
		|| GetPriority(p_io_str.c_str()[l_pos - 1]) == priority_error) \
		&& p_io_str.c_str()[l_pos - 1] != ',' \
		) \
		|| \
		((p_const_name.length() + l_pos) < p_io_str.length() \
		&& (GetPriority(p_io_str.c_str()[l_pos + p_const_name.length()]) < priority_bracket \
		|| GetPriority(p_io_str.c_str()[l_pos + p_const_name.length()]) == priority_error) \
		&& p_io_str.c_str()[l_pos + p_const_name.length()] != ',' \
		) \
		       );
	};
	
	if (!ConstantNotFound(p_io_str, p_const_name, l_pos))
	{
		string l_const;
		l_const.resize(CALC_BUFFER_SIZE);
		l_const.resize(snprintf(&l_const[0], CALC_BUFFER_SIZE - 1, CALC_INTERNAL_ACCURACY_FORMAT, c_cv_constant[p_number_of_constant]));
		do
		{
			p_io_str.erase(l_pos, p_const_name.length());
			m_correct_count -= p_const_name.length();
			p_io_str.insert(l_pos, l_const);
			m_correct_count += l_const.length();
			
			if (ConstantNotFound(p_io_str, p_const_name, l_pos))
				break;
		}
		while (true);
	}
}
//---------------------------------------------------------------------------
void ReplaceConstants(string& p_io_str)
{
	AddMessage(REPLACEMENTS_OF_THE_CONSTANTS);
	
	for (size_t i = 0; i < _countof(c_str_constant); i++)
		ReplaceConstant(p_io_str, c_str_constant[i], i);
		
	AddMessage(p_io_str);
}
//---------------------------------------------------------------------------
#endif // _CONSTANT_CPP