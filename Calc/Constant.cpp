// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2017 Solomina Elle Leonovna, a.rainman on gmail point com
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
#include "Flags.h"
#include "Constant.h"
#include "Message.h"
#include "Linear.h"
//---------------------------------------------------------------------------
constexpr static string_view c_str_constant[] =
{
	"pi",// Pi, Archimedes' constant or Ludolph's number
	"e",// Euler's number
	"c",// Speed of light in vacuum (m·s-1)
	"G",// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
	"J",// Constants of Gauss fild
	"phi",// Golden ratio
	"h",// Planck constant (J·s)
	"atm",// Standard atmosphere (Pa), this constant valid on Earths only ;)
	"L",// Avogadro's number (mol−1)
	"R",// Gas constant (J·K−1·mol−1)
	//"INF",// Infinity
};
//---------------------------------------------------------------------------
constexpr static calc_variable c_cv_constant[] =
{
	// TODO https://en.cppreference.com/w/cpp/numeric/constants
	numbers::pi_v<calc_variable>,// Pi, Archimedes' constant or Ludolph's number
	numbers::e_v<calc_variable>,// Euler's number
	299792458.0L,// Speed of light in vacuum (m·s-1)
	6.674286767676767676767676767676767676e-11L,// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
	3.058198247456354132564564787888767L,// Constants of Gauss fild
	numbers::phi_v<calc_variable>,// Golden ratio
	6.626068963333333333333333333333333333e-34L,// Planck constant (J·s)
	101325.0L,// Standard atmosphere (Pa), this constant valid on Earths only ;)
	6.022141510101010101010101010101010101e23L,// Avogadro's number (mol−1)
	8.314472151515151515151515151515151515L,// Gas constant (J·K−1·mol−1)
	//std::numeric_limits<calc_variable>::infinity(),// Infinity
};
//---------------------------------------------------------------------------
static_assert(_countof(c_str_constant) == _countof(c_cv_constant) && _countof(c_cv_constant) == static_cast<unsigned int>(Constants::constant_counts), "c_str_constant and c_l_constant sizes do not match or constant_counts don't equal to it ;) Check them out!");
//---------------------------------------------------------------------------
const calc_variable& GetConstant(const Constants p_const_ind)
{
	return c_cv_constant[static_cast<unsigned int>(p_const_ind)];
}
//---------------------------------------------------------------------------
inline void ReplaceConstant(string& p_io_str, const string_view& p_const_name, const size_t p_number_of_constant)
{
	string::size_type l_pos;
	
	auto ConstantNotFound = [](const string_view & p_io_str, const string_view & p_const_name, string::size_type & l_pos)
	{
		l_pos = p_io_str.find(p_const_name);
		return (l_pos == string_view::npos \
		        || \
		        (l_pos \
		         && GetPriority(p_io_str[l_pos - 1]) < Priority::bracket \
		         && p_io_str[l_pos - 1] != ',' \
		        ) \
		        || \
		        ((p_const_name.length() + l_pos) < p_io_str.length() \
		         && GetPriority(p_io_str[l_pos + p_const_name.length()]) < Priority::bracket \
		         && p_io_str[l_pos + p_const_name.length()] != ',' \
		        ) \
		       );
	};
	
	while (!ConstantNotFound(p_io_str, p_const_name, l_pos))
	{
		p_io_str.erase(l_pos, p_const_name.length());
		m_correct_count -= p_const_name.length();
		
		string l_const;
		print_value(l_const, c_cv_constant[p_number_of_constant]);
		
		p_io_str.insert(l_pos, l_const);
		m_correct_count += l_const.length();
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