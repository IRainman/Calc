/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
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
    //don't enable this inf_constant,// Infinity
    constant_counts,
};
//---------------------------------------------------------------------------
const calc_variable& GetConstant(const Constants p_const_ind) noexcept;
//---------------------------------------------------------------------------
void ReplaceConstants(string& p_io_str);
//---------------------------------------------------------------------------