#include "stdafx.h"
#include "Calculator.h"
//#define _USE_MATH_DEFINES // TODO 
#include <math.h>
#include "MyTypes.h"

//static char m_buf[c_string_size];
static string m_buf;
static string::size_type start, end;
static uint count;
static string m_ErrorString;
static wstring c_ErrorString;

static const uint_8 c_err_end =	1;
static const uint_8 c_err_nf =	2;
static const uint_8 c_err_emp =	4;
static const uint_8 c_err_ok =	0;

void AddFunctionError(const string& p_function, const string& p_end, bool p_empty = false)
{
	char l_buf[100];
	_itoa_s(start, l_buf, 100, 10);
	m_ErrorString += (string)"Ошибка: У функции ";
	if(p_empty)
	{
		m_ErrorString += p_function + p_end +
			(string)" отсутсвуют аргументы";
	}
	else
	{
		m_ErrorString += p_function +
			(string)", начало в " + (string)l_buf +
			(string)" нехватает " +  p_end;
	}
	m_ErrorString += (string)"!\r\n";
}

uint_8 find(const string& inp, const string& st, const string& en) {
	start = inp.find(st);
	if(start == string::npos)
		return c_err_nf;

	end = inp.find(en);
	if(end == string::npos)
		return c_err_end;

	if(start == end)
		return c_err_emp;

	m_buf = inp.substr(start + st.size(), end - (start + st.size()));//+ (start + st.size()));// - en.size());
	/*
	memset(m_buf, 0, sizeof(m_buf));
	memcpy(m_buf, inp.c_str() + start + st.size(), (end - (start + st.size())) * sizeof(char));
	*/
	return c_err_ok;
}

inline uint ReplaceConstant(string& p_input, string& p_output,
							const string& p_start, const string& p_end)
{
	count = 0;
	while(true)
	{
		switch(find(p_input, p_start, p_end))
		{
			case c_err_nf: // не найдена
				return count;
			case c_err_end: // не хватает закр скобки
				AddFunctionError(p_start, p_end);
				return 0;
			case c_err_emp: // нет аргументов
				AddFunctionError(p_start, p_end, true);
				return 0;
			case c_err_ok:
				//p_input.erase(start, start + p_start.size());
				count++;
				p_input.erase(start, start + (p_start.size()-1) + (m_buf.size()) + (p_end.size()) - count/2);
				
				p_input.insert(start, m_buf);//, end - start);
				p_output = p_input;
		}
	}
}

bool Analize(wstring p_input, wstring& p_output, wstring& p_ErrorString)
{
	c_ErrorString.empty();
	
	string l_input_str, l_output_str;
	l_input_str.assign(p_input.begin(), p_input.end());
	l_output_str.empty();

	ReplaceConstant(l_input_str, l_output_str, "sin(", "_1_2)");

	p_output.empty();
	p_output.assign(l_output_str.begin(), l_output_str.end());

	p_ErrorString.empty();
	p_ErrorString.assign(c_ErrorString.begin(), c_ErrorString.end());

	return true;
}
/*
int GetPriorityOnLineExpression(char p_sym)
{
	switch(p_sym)
	{
		case '^':
			return 4;

		case '*':
		case '/':
			return 3;

		case '-':
		case '+':
			return 2;

		case '(':
			return 1;

		default:
			return 0;
	}
}
*/
/*
int GetPriority()
{

}*/