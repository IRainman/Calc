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

#ifndef MYTYPES_H
#define MYTYPES_H

//---------------------------------------------------------------------------
#include <charconv>
//---------------------------------------------------------------------------
typedef long double calc_variable;
#define calc_input_function strtold
//---------------------------------------------------------------------------
constexpr auto CALC_BUFFER_SIZE = 32;
//---------------------------------------------------------------------------
template<typename T>
inline constexpr void print_value(T& p_str, const calc_variable& p_val)
{
    p_str.resize(CALC_BUFFER_SIZE);
    p_str.resize(to_chars(p_str.data(), p_str.data() + p_str.size(), p_val).ptr - p_str.data());
}
//---------------------------------------------------------------------------
#endif// MYTYPES_H
