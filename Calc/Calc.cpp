// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "stdafx.h"
#include "Calc.h"
#include "CalcDlg.h"

BEGIN_MESSAGE_MAP(CCalcApp, CWinAppEx)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

CCalcApp::CCalcApp() noexcept
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
