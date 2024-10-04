#pragma once
/*
 * Copyright 2009-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "targetver.h"
#include "resource.h"

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_CSTRING_NO_CRT
#define _ATL_ALL_WARNINGS
#define _ATL_SINGLE_THREADED
#define _ATL_NO_COM_SUPPORT
#define _AFX_MINREBUILD
#define _AFX_ALL_WARNINGS
#define _AFXDLL
#include <afxwinappex.h>

class CCalcApp : public CWinAppEx
{
	public:
		virtual BOOL InitInstance() override;
		DECLARE_MESSAGE_MAP()
		afx_msg void OnHelp();
};

class CCalcDlg : public CDialog
{
	public:
		explicit CCalcDlg(CWnd* pParent = nullptr) noexcept;
		CCalcDlg(const CCalcDlg&) = delete;
		CCalcDlg(CCalcDlg&&) = delete;
		enum { IDD = IDD_CALC_DIALOG };
		afx_msg void OnEnChangeEditInput();
		HICON m_hIcon;//-V122_NOPTR
		BOOL OnInitDialog() override;
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg HCURSOR OnQueryDragIcon();
		DECLARE_MESSAGE_MAP()
};

class CAboutDlg : public CDialog
{
	public:
		explicit CAboutDlg() noexcept;
		CAboutDlg(const CAboutDlg&) = delete;
		CAboutDlg(CAboutDlg&&) = delete;
		enum { IDD = IDD_ABOUTBOX };
		virtual BOOL OnInitDialog() override;
		afx_msg void OnBnClickedSite();
		DECLARE_MESSAGE_MAP()
};
