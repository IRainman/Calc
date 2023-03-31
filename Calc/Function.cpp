// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2022 Solomina Elle Leonovna, a.rainman on gmail point com
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
#include "Linear.h"
#include "Message.h"
#include "Function.h"
#include "Constant.h"
//---------------------------------------------------------------------------
inline calc_variable rad(const calc_variable p_grad)
{
	return p_grad * GetConstant(Constants::pi) / 180;
}
// TODO https://en.cppreference.com/w/cpp/numeric/math
// TODO https://en.cppreference.com/w/cpp/numeric/special_math
//---------------------------------------------------------------------------
constexpr static string_view c_function1[] =
{
	"rad(",// One radian is equivalent to 180/PI degrees.
	"sin(",// sinus
	"cos(",// cosinus
	"exp(",// exponent function, on x=1 return value is e
	"tan(",// tangens
	"arccos(",// arccosinus
	"arcsin(",// arcsinus
	"arctan(",// arctangens
	"log(",// natural logarithm
	"log10(",// base-10 logarithm
	"sqrt(",// square root
	//"ceil(",
	//"fabs(",
	//"floor(",
	//"ldexp(",
	//"modf(",
	"sh("
	"ch("
	"tanh("
	
	"abs(",
	
	"log2(",
	"log1p(",
	
	"cbrt(",
	
	"arsinh(",
	"arcosh(",
	"artanh(",
	
	"tgamma(",
	"lgamma(",
	
	"trunc(",
	"round(",
};
typedef calc_variable(* fptr1)(calc_variable);
constexpr static fptr1 f_function1[] =
{
	rad,
	sin,
	cos,
	exp,
	tan,
	acos,
	asin,
	atan,
	log,
	log10,
	sqrt,
	//ceil,
	//fabs,
	//floor,
	//ldexp,
	//modf,
	sinh, // https://en.cppreference.com/w/cpp/numeric/math/sinh
	cosh, // https://en.cppreference.com/w/cpp/numeric/math/cosh
	tanh, // https://en.cppreference.com/w/cpp/numeric/math/tanh
	
	fabs,// https://en.cppreference.com/w/cpp/numeric/math/fabs
	
	log2,// https://en.cppreference.com/w/cpp/numeric/math/log2
	log1p,// https://en.cppreference.com/w/cpp/numeric/math/log1p
	
	cbrt,// https://en.cppreference.com/w/cpp/numeric/math/cbrt
	
	asinh, // https://en.cppreference.com/w/cpp/numeric/math/asinh
	acosh, // https://en.cppreference.com/w/cpp/numeric/math/acosh
	atanh, // https://en.cppreference.com/w/cpp/numeric/math/atanh
	
	tgamma, // https://en.cppreference.com/w/cpp/numeric/math/tgamma
	lgamma, // https://en.cppreference.com/w/cpp/numeric/math/lgamma
	
	trunc, // https://en.cppreference.com/w/cpp/numeric/math/trunc
	round, // https://en.cppreference.com/w/cpp/numeric/math/round
};
constexpr static string_view c_function2[] =
{
	"pow(",// power
	//"atan2(",
	//"modf(",
	//"fmod(",
	//"frexp(",
	"max(",
	"min(",
	"hypot2(",
//	"gcd(",
//	"lcm(",
};
typedef calc_variable(* fptr2)(calc_variable, calc_variable);
constexpr static fptr2 f_function2[] =
{
	pow,// power
	//atan2,
	//modf,
	//fmod,
	//frexp,
	fmax,// https://en.cppreference.com/w/cpp/numeric/math/fmax
	fmin,// https://en.cppreference.com/w/cpp/numeric/math/fmin
	hypot, // https://en.cppreference.com/w/cpp/numeric/math/hypot
//	gcd, // https://en.cppreference.com/w/cpp/numeric/gcd
//	lcm, // https://en.cppreference.com/w/cpp/numeric/lcm
};
constexpr static string_view c_function3[] =
{
	"hypot3(",
};
typedef calc_variable(* fptr3)(calc_variable, calc_variable, calc_variable);
constexpr static fptr3 f_function3[] =
{
	hypot, // https://en.cppreference.com/w/cpp/numeric/math/hypot
};
constexpr size_t c_max_argument_of_function = 3;
//---------------------------------------------------------------------------
constexpr inline calc_variable CalculateParametrs(const size_t p_number_of_param, const size_t p_function_number, calc_variable p_params[])
{
	switch (p_number_of_param)
	{
		case 1:
			return f_function1[p_function_number](p_params[0]);
		case 2:
			return f_function2[p_function_number](p_params[0], p_params[1]);
		case 3:
			return f_function3[p_function_number](p_params[0], p_params[1], p_params[2]);
		default:
#ifdef _DEBUG
			AddError(INTERNAL_PROCESSING_ERROR_CalculateFunction);
			return 0;
#else
			__assume(false); // C++23 unreachable(); 
#endif
	}
}
//---------------------------------------------------------------------------
inline constexpr bool CheckParametrIsSubexpr(const string_view p_param_str)
{
	const auto l_count = p_param_str.find_first_not_of("0123456789.", (p_param_str[0] != '-') ? 0 : 1);
	if (l_count == string_view::npos
	        || p_param_str.compare(l_count, 2, "e+", 2) == 0
	        || p_param_str.compare(l_count, 2, "e-", 2) == 0
	        || p_param_str.compare(l_count, 1, "e",  1) == 0
	   )
	{
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
[[nodiscard]] bool ProcessParametr(string& p_param_str, const string::size_type p_start, const size_t p_number_of_param, calc_variable p_params[], const string::size_type p_comma_count, const string::size_type p_correct_end)
{
	if (p_comma_count >= p_number_of_param)
	{
		AddError(FUNCTION_INVALID_NUMBER_OF_ARGUMENTS, p_start, p_number_of_param, p_comma_count + 1);
		return false;
	}
	if (p_param_str.empty())
	{
		AddError(LOST_FUNCTION_ARGUMENTS, p_start, p_comma_count + 1, p_correct_end);
		return false;
	}
	
	calc_variable l_outp;
	
	if (CheckParametrIsSubexpr(p_param_str))
	{
		AddMessage(FOUND_SUBEXPRESSIONS);
		ProcessFunctions(p_param_str
#ifdef ENABLE_INPUT_VALIDATION
		                 , p_start
#endif
		                );
		l_outp = CalculateLineExpression(p_param_str, p_start);
		AddMessage(END_OF_THE_SUBEXPRESSION);
		if (isnan(l_outp))
		{
			AddError(LOST_FUNCTION_ARGUMENTS, p_start, p_comma_count + 1, p_correct_end);
			return false;
		}
	}
	else
	{
		const auto l_res = from_chars(p_param_str.data(), p_param_str.data() + p_param_str.size(), l_outp);
		const size_t l_diff = l_res.ptr - p_param_str.data();
		if (l_res.ec != errc() || l_diff != p_param_str.size())
		{
			AddError(LOST_FUNCTION_ARGUMENTS, p_start, p_comma_count + 1, p_correct_end);
			return false;
		}
	}
	p_params[p_comma_count] = l_outp;
	p_param_str.clear();
	return true;
}
//---------------------------------------------------------------------------
void CalculateFunction(string& p_io_str, const string_view p_func_name, const size_t p_count, const size_t p_number_of_param)
{
	do
	{
		// find function
		const auto l_start = p_io_str.find(p_func_name);
		if (l_start == string::npos)
		{
			return;
		}
		else if (l_start != 0)
		{
			if (GetPriority(p_io_str[l_start - 1]) < Priority::bracket)
			{
				AddError(NUMBER_BEFORE_FUNCTION, l_start);
				return;
			}
		}
		// ~find function
		
		string::size_type l_end;
		calc_variable l_params[c_max_argument_of_function]; // TODO use p_number_of_param as size
		
		// process function
		{
			string l_params_str;
			string l_buf = p_io_str.substr(l_start + p_func_name.size());
			string::size_type l_comma_count = 0;
			size_t l_nesting_level = 0;
			
			for (string::size_type l_correct_end = 0;;)
			{
				const auto l_br_start = l_buf.find('(');
				l_end = l_buf.find(')');
				if (l_end == string::npos)
				{
					AddError(FUNCTION_LOST_CLOSING_BRACKET, l_start);
					return;
				}
				if (l_end + 1 < l_buf.size() && GetPriority(l_buf[l_end + 1]) < Priority::bracket)
				{
					AddError(NUMBER_AFTER_FUNCTION, l_end + 1);
					return;
				}
				const auto l_current_comma = l_buf.find(',');
				if (l_br_start != string::npos && l_br_start < l_end && l_current_comma > l_br_start)
				{
					l_correct_end += l_br_start + 1;
					l_params_str += l_buf.substr(0, l_br_start + 1);
					l_buf = l_buf.substr(l_br_start + 1);
					l_nesting_level++;
				}
				else if (!l_nesting_level)
				{
					if (l_current_comma != string::npos && l_current_comma < l_end)
					{
						l_params_str += l_buf.substr(0, l_current_comma);
						l_correct_end += l_current_comma + 1;
						l_buf = l_buf.substr(l_current_comma + 1);
						if (!ProcessParametr(l_params_str, l_start, p_number_of_param, l_params, l_comma_count, l_correct_end))
						{
							return;
						}
						l_comma_count++;
					}
					else if (l_current_comma == string::npos || l_current_comma > l_end)
					{
						l_params_str += l_buf.substr(0, l_end);
						l_end += l_correct_end;
						if (!ProcessParametr(l_params_str, l_start, p_number_of_param, l_params, l_comma_count, l_correct_end))
						{
							return;
						}
						break;
					}
				}
				else
				{
					l_correct_end += l_end + 1;
					l_params_str += l_buf.substr(0, l_end + 1);
					l_buf = l_buf.substr(l_end + 1);
					l_nesting_level--;
				}
			}
			if (l_comma_count != p_number_of_param - 1)
			{
				AddError(FUNCTION_INVALID_NUMBER_OF_ARGUMENTS, l_start, p_number_of_param, l_comma_count + 1);
				return;
			}
		}
		// ~process function
		
		// process result
		{
			string l_tmp;
			const auto l_outp = print_value(l_tmp, CalculateParametrs(p_number_of_param, p_count, l_params));
			const auto l_erased = p_func_name.size() + l_end + 1;
			p_io_str.erase(l_start, l_erased);
			p_io_str.insert(l_start, l_outp);
			m_correct_count += l_outp.size() - l_erased;
		}
		// ~process result
	}
	while (true);
}
//---------------------------------------------------------------------------
void ProcessFunctions(string& p_io_str
#ifdef ENABLE_INPUT_VALIDATION
                      , const string::size_type p_mes_pos_shift /*= 0*/
#endif
                     )
{
	AddMessage(CALCULATE_FUNCTIONS);
	
	for (size_t i = 0; i < _countof(c_function1); i++)
		CalculateFunction(p_io_str, c_function1[i], i, 1);
		
	for (size_t i = 0; i < _countof(c_function2); i++)
		CalculateFunction(p_io_str, c_function2[i], i, 2);
		
	for (size_t i = 0; i < _countof(c_function3); i++)
		CalculateFunction(p_io_str, c_function3[i], i, 3);
		
	AddMessage(p_io_str);
	
#ifdef ENABLE_INPUT_VALIDATION
	for (string::size_type l_count = 0; l_count < p_io_str.size() - 1; ++l_count)
	{
		if (p_io_str[l_count + 1] == '('
		        && GetPriority(p_io_str[l_count]) == Priority::priority_default)
		{
			AddError(NUMBERS_BEFORE_OPENING_BRACKET, l_count - 1 + p_mes_pos_shift);
			return;
		}
	}
	const auto l_count = p_io_str.find_first_not_of("0123456789+-*/^.e()", 0);
	if (l_count != string::npos)
	{
		AddError(INVALID_CHARACTER_AFTER_CONSTANT_AND_FUNCTION_PROCESS, l_count + p_mes_pos_shift);
		return;
	}
#endif
}
//---------------------------------------------------------------------------