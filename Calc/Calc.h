/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

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