#include "stdafx.h"
#include "Calculator.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "MyTypes.h"

wchar_t buf[c_string_size];
uint start, end;

uint find(wstring inp, wstring st, wstring en) {
	end = inp.find(en);
	if(end == wstring::npos)
		return 2; // Не найден конец
	start = inp.find(st) + st.size();
	if(start == wstring::npos)
		return 1; // Не найдено начало

	memset(buf, 0, sizeof(buf));
	memcpy(buf, inp.c_str() + start, (end - start) * sizeof(wchar_t) );
	start ++;
	start -= st.size();
	return 0; // Завершено без ошибок
}
/*
uint find(wstring inp, wstring st, wstring en) {
	end = inp.find(en);
	if(end == wstring::npos)
		return 2;
	start = inp.find(st) + st.size();
	if(start == wstring::npos)
		return 1;

	memset(buf, 0, sizeof(buf));
	memcpy(buf, inp.c_str() + start, (end - start) * sizeof(wchar_t) );
	return 0;
}
*/
inline void ReplaceConstant(wstring& p_input, wstring& p_output)
{
	p_output = L"";

	while(!find(p_input, L"sin(", L")"))
	{
		p_output += buf;
		p_input.erase(start, end);
	}

	//while(p_input.find(L"PI") != wstring::npos)
	{
		//p_input.replace(L"PI", wstring(M_PI)); //пусто
	}
	//while(p_input.Replace(L"E", M_E))
	{
		//пусто
	}
}

bool Analize(wstring p_input, wstring& p_output)
{
	ReplaceConstant(p_input, p_output);
//	p_output = p_input;
//	for()
	{
		//if(Analize())
	}
	return true;
}

inline int GetPriorityOnLineExpression(wchar_t a)
{
	switch(a)
	{
		case '^':
			return 3;

		case '*':
		case '/':
			return 2;

		case '-':
		case '+':
			return 1;
/*
		case '(':
			return 1;
*/
		default:
			return 0;
	}
}
/*
int GetPriority()
{

}*/