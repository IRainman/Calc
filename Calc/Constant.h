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

#pragma once
using namespace std;
#include "MyTypes.h"
//---------------------------------------------------------------------------
enum class Constants
{
    pi = 0,// Pi, Archimedes' constant or Ludolph's number
    e,// Euler's number
    c,// Speed of light in vacuum (m·s-1)
    G,// Newtonian constant of gravitation (m3·kg−1·s−2), this constant valid on Earths only ;)
    J,// Constants of Gauss fild
    phi,// Golden ratio
    h,// Planck constant (J·s)
    atm,// Standard atmosphere (Pa), this constant valid on Earths only ;)
    L,// Avogadro's number (mol−1)
    R,// Gas constant (J·K−1·mol−1)
    //inf_constant,// Infinity
    constant_counts,
};
//---------------------------------------------------------------------------
const calc_variable& GetConstant(const Constants p_const_ind);
//---------------------------------------------------------------------------
void ReplaceConstants(string& p_io_str);
//---------------------------------------------------------------------------