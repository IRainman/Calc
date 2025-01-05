// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"

#include "flags.h"
#include "lexer.h"
#include "parser.h"
#include "issue_manager.h"
#include "formatter.h"
#ifdef CALC_TESTS_ENABLED
#include "tests.h"
#endif

// ================= GUI ===============

#ifdef _WIN32

#include "targetver.h"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _AFX_NO_OLE_SUPPORT
#define _AFX_NO_DB_SUPPORT
#define _AFX_NO_DAO_SUPPORT
#define _AFX_NO_AFXCMN_SUPPORT
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_ALL_WARNINGS
#define _ATL_SINGLE_THREADED
#define _ATL_NO_COM_SUPPORT
#define _AFX_MINREBUILD
#define _AFX_ALL_WARNINGS
#include <afxwinappex.h>
#include <afxwin.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <libloaderapi.h>
#include <WinUser.h>
#include <afx.h>
#include <afxstr.h>

#include "resource.h"
#include "GUI.h"

CCalcApp theCalcApp;

CCalcDlg dlg;

BOOL CCalcApp::InitInstance()
{
	CWinAppEx::InitInstance();

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

BEGIN_MESSAGE_MAP(CCalcApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &ThisClass::OnHelp)
END_MESSAGE_MAP()

void CCalcApp::OnHelp()
{
	// FIXME
}







CCalcDlg::CCalcDlg(CWnd* pParent /*=nullptr*/) //-V730
	: CDialog(CCalcDlg::IDD, pParent)
{
	IssueManager::speedup();
}

BOOL CCalcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	pSysMenu->AppendMenu(MF_SEPARATOR);
	pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);

	HICON hIcon = theCalcApp.LoadIcon(IDR_MAINFRAME);

	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CCalcDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_INPUT, &ThisClass::OnEnChangeEditInput)
END_MESSAGE_MAP()

void CCalcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CCalcDlg::OnEnChangeEditInput()
{
	if (GetDlgItemText(IDC_EDIT_INPUT, m_str) >= 1)
	{
		const std::string_view input(m_str, static_cast<unsigned int>(m_str.GetLength()));

#ifdef CALC_TESTS_ENABLED
		if (input.data()[0] == '\r')
		{
			SetDlgItemText(IDC_EDIT_MESSAGE, calc_tests().data());
		}
		else
#endif
		{
			Lexer l{ input };
			Parser p{ l };

			SetDlgItemText(IDC_EDIT_RESULT, Formatter::format(p.parse()).data());
			SetDlgItemText(IDC_EDIT_MESSAGE, Formatter::create_summary().data());

			IssueManager::clear();
		}
	}
	else
	{
		SetDlgItemText(IDC_EDIT_RESULT, "");
		SetDlgItemText(IDC_EDIT_MESSAGE, "");
	}
}







CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDlgItemText(IDC_COMPILED_DATE, _T(__TIMESTAMP__));
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_CANCEL, &CAboutDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

void CAboutDlg::OnBnClickedCancel() noexcept
{
	EndDialog(FALSE);
}

#else

// add other platforms here.

#endif // _WIN32
