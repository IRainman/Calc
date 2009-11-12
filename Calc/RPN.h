#ifndef _RPN_H
#define _RPN_H
//---------------------------------------------------------------------------
using namespace std;
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>
#include <string>
#include <stack>
#include "Flags.h"
#include "MyTypes.h"
//---------------------------------------------------------------------------
enum
{
	priority_function		= 16,
	priority_power			= 8,
	priority_multiply		= 4,
	priority_addition		= 2,
	priority_bracket		= 1,
	priority_default		= 0,
	priority_error			= -64
};
//---------------------------------------------------------------------------
extern int_8 GetPriority(char p_sym);
extern void CalculateOnLineExpression(stack<char>& c_operations, stack<long double>& c_operands);
extern void CalculateLineExpression(string p_input_str, string& p_output_str);
#ifdef _USE_RPN
extern void CalculateRPN(string& p_to_process_str);
#endif //_USE_RPN
//---------------------------------------------------------------------------
#endif