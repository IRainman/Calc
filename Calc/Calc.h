
#pragma once

#ifndef __AFXWIN_H__
#error "�������� stdafx.h �� ��������� ����� ����� � PCH"
#endif

#include "resource.h"

class CCalcApp : public CWinAppEx
{
	public:
		CCalcApp();
		
	public:
		virtual BOOL InitInstance();
		
		DECLARE_MESSAGE_MAP()
};

extern CCalcApp theApp;