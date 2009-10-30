#ifndef _MESSAGE_H
#define _MESSAGE_H
//---------------------------------------------------------------------------
using namespace std;
#include <string>
#include "MyTypes.h"
//---------------------------------------------------------------------------
extern wstring m_ErrorString;
extern bool m_NoError;
extern string::size_type m_Correct_count;
//---------------------------------------------------------------------------
extern void AddMessage(const string& p_string_message);
extern void AddMessage(uint_8 p_message);
extern void AddWarning(uint_8 p_message);
extern void AddError(uint_8 p_message, string::size_type p_count = -1, string::size_type p_1 = 0, string::size_type p_2 = 0);
//---------------------------------------------------------------------------
#endif