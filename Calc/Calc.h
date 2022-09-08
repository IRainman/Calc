
#pragma once

#ifndef __AFXWIN_H__
#error "The stdafx.h isn't set as the PCH file"
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