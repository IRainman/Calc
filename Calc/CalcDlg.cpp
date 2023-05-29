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
#include "Calculator.h"
#include "tests.h"

class CAboutDlg : public CDialog
{
	public:
		CAboutDlg();
		
		enum { IDD = IDD_ABOUTBOX };
		
	protected:
		virtual BOOL OnInitDialog();
		virtual void DoDataExchange(CDataExchange* pDX);
		
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

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

inline void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(ID_SITE, &CAboutDlg::OnBnClickedSite)
END_MESSAGE_MAP()

CCalcDlg::CCalcDlg(CWnd* pParent /*=NULL*/)
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
	ON_EN_CHANGE(IDC_EDIT_INPUT, &CCalcDlg::OnEnChangeEditInput)
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
	m_input.resize(c_max_edit_input_size);
	const auto l_count = GetDlgItemText(IDC_EDIT_INPUT, m_input.data(), c_max_edit_input_size);
	if (l_count > 0)
	{
		m_input.resize(l_count); //-V106
		
		if (IsDlgButtonChecked(IDC_CHECK_AUTO_CALCULATE) == BST_UNCHECKED &&
		        m_input.size() > 1 && m_input[m_input.size() - 1] == '=')
		{
			m_input.erase(m_input.size() - 1);
			goto calculate_function_call;
		}
		if ((IsDlgButtonChecked(IDC_CHECK_AUTO_CALCULATE) == BST_CHECKED))
		{
calculate_function_call:
			auto& l_message = Calculate(m_input, m_result);
			SetDlgItemText(IDC_EDIT_MESSAGE, l_message.data());
			SetDlgItemText(IDC_EDIT_RESULT, m_result.data());
		}
	}
	else
	{
		SetDlgItemText(IDC_EDIT_MESSAGE, "");
		SetDlgItemText(IDC_EDIT_RESULT, "");
	}
}

void CAboutDlg::OnBnClickedSite() noexcept
{
	::ShellExecute(nullptr, nullptr, "http://studio2000.xyz/txt/calc.shtml", nullptr, nullptr, SW_SHOWNORMAL); //-V2001
}
