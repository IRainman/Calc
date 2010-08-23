
// Calc.cpp : ќпредел€ет поведение классов дл€ приложени€.
//

#include "stdafx.h"
#include "Calc.h"
#include "CalcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCalcApp

BEGIN_MESSAGE_MAP(CCalcApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// создание CCalcApp

CCalcApp::CCalcApp()
{
	// TODO: добавьте код создани€,
	// –азмещает весь важный код инициализации в InitInstance
}

CCalcApp theApp;

// инициализаци€ CCalcApp

BOOL CCalcApp::InitInstance()
{
	// InitCommonControlsEx() требуетс€ дл€ Windows XP, если манифест
	// приложени€ использует ComCtl32.dll версии 6 или более поздней версии дл€ включени€
	// стилей отображени€. ¬ противном случае будет возникать сбой при создании любого окна.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	
	CWinAppEx::InitInstance();
	
	AfxEnableControlContainer();

	CCalcDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	return FALSE;
}
