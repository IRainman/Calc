/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#pragma once
#ifndef CALC_VER2
//---------------------------------------------------------------------------
using namespace std;
#include "Flags.h"
#include "MyTypes.h"
//---------------------------------------------------------------------------
enum class Priority
{
    function    = 16,
    power       = 8,
    multiply    = 4, //-V112
    addition    = 2,
    bracket     = 1,
    number      = 0,
    error       = -1
};
//---------------------------------------------------------------------------
constexpr inline Priority GetPriority(const char p_sym)
{
	switch (p_sym)
	{
		case '^':
			return Priority::power;
			
		case '*':
		case '/':
		case '%':
			return Priority::multiply;
			
		case '+':
		case '-':
			return Priority::addition;
			
		case '(':
		case ')':
			return Priority::bracket;
			
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '.':
			return Priority::number;
			
		default:
			return Priority::error;
	}
}
//---------------------------------------------------------------------------
extern [[nodiscard]] calc_variable CalculateLineExpression(const string_view p_input_str, string_view::size_type p_mes_pos_shift = 0);
//---------------------------------------------------------------------------
#endif