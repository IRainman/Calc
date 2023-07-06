#pragma once
/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "resource.h"

class CCalcApp : public CWinAppEx
{
	public:
		explicit CCalcApp() noexcept;
		CCalcApp(const CCalcApp&) = delete;
		CCalcApp(CCalcApp&&) = delete;

		
	public:
		virtual BOOL InitInstance();
		
		DECLARE_MESSAGE_MAP()
};

extern CCalcApp theApp;