#ifndef _CALCULATOR_H
#define _CALCULATOR_H
//---------------------------------------------------------------------------
using namespace std;
#include <string>
//---------------------------------------------------------------------------
wstring& Calculate(wstring p_input, wstring& p_output, bool p_scientific);
//---------------------------------------------------------------------------
static wstring getCompileDate()
{
	COleDateTime tCompileDate;
	tCompileDate.ParseDateTime(_T(__DATE__), LOCALE_NOUSEROVERRIDE, 1033);
	return tCompileDate.Format(_T("%Y-%m-%d")).GetString();
}
//---------------------------------------------------------------------------
#endif