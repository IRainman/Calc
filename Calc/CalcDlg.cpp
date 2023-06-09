// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "stdafx.h"
#include "Calc.h"
#include "CalcDlg.h"
#include "lexer.h"
#include "parser.h"
#include "formatter.h"
#include "tests.h"

class CAboutDlg : public CDialog
{
	public:
		CAboutDlg() noexcept;
		
		enum { IDD = IDD_ABOUTBOX };
		
	protected:
		virtual BOOL OnInitDialog() override;
		virtual void DoDataExchange(CDataExchange* pDX) override;
		
	protected:
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnBnClickedSite() noexcept;
};

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDlgItemText(IDC_COMPILED_DATE, _T(__TIMESTAMP__));
	return TRUE;
}

CAboutDlg::CAboutDlg() noexcept : CDialog(CAboutDlg::IDD)
{
}

inline void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(ID_SITE, OnBnClickedSite)
END_MESSAGE_MAP()

CCalcDlg::CCalcDlg(CWnd* pParent /*=nullptr*/) noexcept
	: CDialog(CCalcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

inline void CCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCalcDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT_INPUT, OnEnChangeEditInput)
END_MESSAGE_MAP()


BOOL CCalcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	
	CheckDlgButton(IDC_CHECK_AUTO_CALCULATE, TRUE);
	
#ifdef CALC_TESTS_ENABLED
	SetDlgItemText(IDC_EDIT_MESSAGE, calc_tests().data());
#endif
	
	return TRUE;
}

inline void CCalcDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

inline HCURSOR CCalcDlg::OnQueryDragIcon() noexcept
{
	return m_hIcon;
}

void CCalcDlg::OnEnChangeEditInput()
{
	constexpr std::string::size_type c_max_edit_input_size = 4096;
	std::string l_input;
	l_input.resize(c_max_edit_input_size);
	const auto l_count = GetDlgItemText(IDC_EDIT_INPUT, l_input.data(), c_max_edit_input_size);
	if (l_count > 0)
	{
		l_input.resize(l_count); //-V106
		
		if (IsDlgButtonChecked(IDC_CHECK_AUTO_CALCULATE) == BST_UNCHECKED &&
		        l_input.size() > 1 && l_input[l_input.size() - 1] == '=')
		{
			l_input.erase(l_input.size() - 1);
			goto calculate_function_call;
		}
		if ((IsDlgButtonChecked(IDC_CHECK_AUTO_CALCULATE) == BST_CHECKED))
		{
		calculate_function_call:

			std::string l_result;
			Lexer l{ l_input };
			Parser p{ l };
			format_output_value(l_result, p.parse());
			SetDlgItemText(IDC_EDIT_RESULT, l_result.data());
			SetDlgItemText(IDC_EDIT_MESSAGE, Formatter::create_summary().data());
			IssueManager::get_instance().clear();
		}
	}
	else
	{
		SetDlgItemText(IDC_EDIT_RESULT, "");
		SetDlgItemText(IDC_EDIT_MESSAGE, "");
	}
}

void CAboutDlg::OnBnClickedSite() noexcept
{
	::ShellExecute(nullptr, nullptr, "http://studio2000.xyz/txt/calc.shtml", nullptr, nullptr, SW_SHOWNORMAL); //-V2001
}
