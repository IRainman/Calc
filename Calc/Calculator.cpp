#include "stdafx.h"
#include "Calculator.h"
//TODO #define _USE_MATH_DEFINES
#include <math.h>
#include <stack>
#include "MyTypes.h"

static string m_buf;
static string::size_type start, end;
static wstring m_ErrorString;

static const uint_8 c_err_end =	1;
static const uint_8 c_err_nf =	2;
static const uint_8 c_err_emp =	4;
static const uint_8 c_err_ok =	0;

inline bool ValidateInputString(const string& p_input_str)
{
	uint count_inp = 0, count_outp = 0;
	for(string::size_type l_count = 0; l_count < p_input_str.size(); l_count++)
	{
		if(p_input_str.c_str()[l_count] == '(')
		{
			count_inp++;
			continue;
		}
		if(p_input_str.c_str()[l_count] == ')')
		{
			count_outp++;
			continue;
		}
	}
	if(count_inp != count_outp)
	{
		char l_char_buf[100];
		TCHAR l_wchar_buf[100];

		_itoa_s(count_inp, l_char_buf, 100, 10);
		wsprintf(l_wchar_buf, L"%hs", l_char_buf);

		m_ErrorString += (wstring)L"Ошибка: Во входной строке количество открывающих (" +
			l_wchar_buf + (wstring)L")";

		_itoa_s(count_outp, l_char_buf, 100, 10);
		wsprintf(l_wchar_buf, L"%hs", l_char_buf);

		m_ErrorString += (wstring)L" не совпадает с количеством закрывающих скобок (" +
			l_wchar_buf + (wstring)L")";
		return false;
	}
	return true;
}

void AddFunctionError(const string& p_function, const string& p_end, bool p_empty = false)
{
	char l_char_buf[100];
	TCHAR l_wchar_buf[100];
	_itoa_s(start, l_char_buf, 100, 10);
	wstring l_function = L"", l_end = L"";
	l_function.assign(p_function.begin(), p_function.end());
	l_end.assign(p_end.begin(), p_end.end());
	wsprintf(l_wchar_buf, L"%hs", l_char_buf);

	m_ErrorString += (wstring)L"Ошибка: У функции ";

	if(p_empty)
	{
		m_ErrorString += l_function + l_end +
			(wstring)L", начало в " + l_wchar_buf +
			(wstring)L" отсутсвуют аргументы";
	}
	else
	{
		m_ErrorString += l_function +
			(wstring)L", начало в " + l_wchar_buf +
			(wstring)L" нехватает " +  l_end;
	}
	m_ErrorString += (wstring)L"!\r\n";
}

inline uint_8 find(const string& inp, const string& st, const string& en) {
	start = inp.find(st);
	if(start == string::npos)
		return c_err_nf;

	end = inp.find(en);
	if(end == string::npos)
		return c_err_end;

	m_buf = inp.substr(start + st.size(), end - (start + st.size()));
	if(m_buf.empty())
		return c_err_emp;

	return c_err_ok;
}

bool ProcessingFunction(string& p_input, string& p_output,
							const string& p_start, const string& p_end)
{
	while(true)
	{
		switch(find(p_input, p_start, p_end))
		{
			case c_err_nf:
				p_output = p_input;
				return true;
			case c_err_end:
				AddFunctionError(p_start, p_end);
				p_output = "";
				return false;
			case c_err_emp:
				AddFunctionError(p_start, p_end, true);
				p_output = "";
				return false;
			case c_err_ok:
				p_input.erase(start, end + p_end.size() - start);
				p_input.insert(start, m_buf);
				p_output = p_input;
		}
	}
}

void Analize(wstring p_input, wstring& p_output, wstring& p_ErrorString)
{
	m_ErrorString = L"";
	bool l_No_Error = true;
	
	string l_input_str, l_output_str;
	l_input_str.assign(p_input.begin(), p_input.end());
	l_output_str = "";

//TODO processing constant
//	l_No_Error &= ProcessingFunction(l_input_str, l_output_str, "sin(", ")");
//	l_No_Error &= ProcessingFunction(l_input_str, l_output_str, "cos(", ")");

	if(l_No_Error)
	{
		l_No_Error &= ValidateInputString(l_input_str);
//TODO		l_No_Error &= …(l_input_str);
	}

	p_output = L"";
	p_output.assign(l_output_str.begin(), l_output_str.end());

	if(l_No_Error)
	{
		p_ErrorString = L"Преобразование успешно выполнено";
	}
	else
	{
		p_ErrorString = L"";
		p_ErrorString.assign(m_ErrorString.begin(), m_ErrorString.end());
	}
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