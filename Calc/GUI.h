#pragma once
/*
 * Copyright 2009-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#include "targetver.h"
#include "resource.h"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
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
#define NOGDI
#define NOSOUND
#define NOCOMM
#define NOCRYPT
#define NOMINMAX  
#include <afxwinappex.h>

class CCalcApp : public CWinAppEx
{
	public:
		virtual BOOL InitInstance() override;
		afx_msg void OnHelp();
		DECLARE_MESSAGE_MAP()
};

class CCalcDlg : public CDialog
{
	public:
		explicit CCalcDlg(CWnd* pParent = nullptr) noexcept;
		CCalcDlg(const CCalcDlg&) = delete;
		CCalcDlg(CCalcDlg&&) = delete;
		enum { IDD = IDD_CALC_DIALOG };
		afx_msg void OnEnChangeEditInput();
		HICON m_hIcon; //-V122
		virtual BOOL OnInitDialog() override;
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
