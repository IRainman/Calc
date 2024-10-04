#pragma once
/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "resource.h"

class CCalcApp : public CWinAppEx
{
	public:
		virtual BOOL InitInstance() override;
		DECLARE_MESSAGE_MAP()
		afx_msg void OnHelp();
};
