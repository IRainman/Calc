#ifndef _MESSAGE_H
#define _MESSAGE_H
//---------------------------------------------------------------------------
using namespace std;
#include <string>
#include "MyTypes.h"
#include "Flags.h"
//---------------------------------------------------------------------------
extern wstring m_ErrorString;
extern bool m_NoError;
extern string::size_type m_Correct_count;
#ifdef _DEBUG
extern void AddMessage(const string& p_string_message);// TODO убрать
#endif
//extern void AddMessage(const wstring& p_message);
extern void AddMessage(uint_8 p_message);
extern void AddWarning(uint_8 p_message);
extern void AddError(uint_8 p_message, string::size_type p_count = -1, string::size_type p_1 = 0, string::size_type p_2 = 0);
//---------------------------------------------------------------------------
enum MessageEnum
{
	MESSAGE_FIRST,
	PREPARING,
	PARSING_A_STRING,
//	CALCULATION,
#ifdef _USE_RPN
	RPN,
#endif
	MESSAGE_LAST,

	WARNING_FIRST = MESSAGE_LAST,
	WARNING_LAST,

	ERROR_FIRST = WARNING_LAST,
	ERROR_LAST
};
//---------------------------------------------------------------------------
#endif