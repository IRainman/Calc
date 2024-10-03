// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"
#include "Calc.h"
#include "CalcDlg.h"

BEGIN_MESSAGE_MAP(CCalcApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CCalcApp::OnHelp)
END_MESSAGE_MAP()

CCalcApp::CCalcApp() noexcept
{
}

void CCalcApp::OnHelp()
{
}

CCalcApp theApp;

BOOL CCalcApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	
	InitCtrls.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	
	CWinAppEx::InitInstance();
	
	CCalcDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}
