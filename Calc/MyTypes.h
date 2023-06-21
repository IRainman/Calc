#pragma once
/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#ifndef CALC_VER2
//---------------------------------------------------------------------------
// TODO 128 bit is hardware supported on x86 and ARM. Need fix from compiler.
typedef long double calc_variable;
//---------------------------------------------------------------------------
constexpr auto CALC_BUFFER_SIZE = 32;
//---------------------------------------------------------------------------
template<typename T>
inline constexpr T& print_value(T& p_str, const calc_variable p_val)
{
	p_str.resize(CALC_BUFFER_SIZE);
	p_str.resize(std::to_chars(p_str.data(), p_str.data() + p_str.size(), p_val, std::chars_format::general, std::numeric_limits<calc_variable>::digits10 - 1).ptr - p_str.data());
	return p_str;
}
//---------------------------------------------------------------------------
#endif