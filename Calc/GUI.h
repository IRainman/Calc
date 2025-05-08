/*
 * Copyright 2009-2025 Solomina Elle, a.rainman on gmail point com
 */

#ifdef _WIN32

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

// CCalcApp:
// See GUI.cpp for the implementation of this class
//
class CCalcApp : public CWinApp
{
public:
	CCalcApp();

// Overrides
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

// CCalcDlg dialog
class CCalcDlg : public CDialog
{
// Construction
public:
	CCalcDlg(CWnd* pParent = nullptr);	// standard constructor
	CCalcDlg(const CCalcDlg&) = delete;
	CCalcDlg(CCalcDlg&&) = delete;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CALC_DIALOG };
#endif

#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
#endif
	DECLARE_MESSAGE_MAP()

#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
// Implementation
protected:
	HICON m_hIcon;
#endif

public:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
#endif
	afx_msg void OnBnClickedButtonCalc();
private:
	CStringA Input;
};

class CAboutDlg : public CDialog
{
public:
	explicit CAboutDlg();
	CAboutDlg(const CAboutDlg&) = delete;
	CAboutDlg(CAboutDlg&&) = delete;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif
	virtual BOOL OnInitDialog() override;
	afx_msg void OnBnClickedCancel() noexcept;

#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
#endif

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

#else

// add other platforms here.

#endif // _WIN32
