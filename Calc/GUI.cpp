// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "pch.h"

#include "lexer.h"
#include "parser.h"
#include "issue_manager.h"
#include "formatter.h"
#include "tests.h"


// ================= GUI ===============
#include "GUI.h"

#ifdef _WIN32

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

	m_hIcon = theCalcApp.LoadIcon(IDR_MAINFRAME);

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

#ifdef CALC_TESTS_ENABLED
	SetDlgItemText(IDC_EDIT_MESSAGE, calc_tests().data());
#endif

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

HCURSOR CCalcDlg::OnQueryDragIcon() noexcept
{
	return m_hIcon;
}

void CCalcDlg::OnEnChangeEditInput()
{
	CString str;
	GetDlgItemText(IDC_EDIT_INPUT, str);
	const std::string_view input(str, static_cast<unsigned int>(str.GetLength()));
	Lexer l{ input };
	Parser p{ l };
	SetDlgItemText(IDC_EDIT_RESULT, Formatter::format_output_value(p.parse()).data());
	SetDlgItemText(IDC_EDIT_MESSAGE, Formatter::create_summary().data());
	IssueManager::clear();
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
	ON_BN_CLICKED(ID_SITE, &ThisClass::OnBnClickedSite)
END_MESSAGE_MAP()

void CAboutDlg::OnBnClickedSite() noexcept
{
	::ShellExecute(nullptr, nullptr, "http://studio2000.xyz/txt/calc.shtml", nullptr, nullptr, SW_SHOWNORMAL); //-V2001
}

#else

// add other platforms here.

#endif // _WIN32