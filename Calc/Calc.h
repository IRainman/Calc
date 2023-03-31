
#pragma once

#include "resource.h"

class CCalcApp : public CWinAppEx
{
	public:
		CCalcApp() noexcept;
		
	public:
		virtual BOOL InitInstance();
		
		DECLARE_MESSAGE_MAP()
};

extern CCalcApp theApp;