#pragma once
/*
 * Copyright 2009-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */

#ifdef _WIN32

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
		explicit CCalcDlg(CWnd* pParent = nullptr);
		CCalcDlg(const CCalcDlg&) = delete;
		CCalcDlg(CCalcDlg&&) = delete;
		enum { IDD = IDD_CALC_DIALOG };
		afx_msg void OnEnChangeEditInput();
		HICON m_hIcon; //-V122
		virtual BOOL OnInitDialog() override;
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg HCURSOR OnQueryDragIcon() noexcept;
		DECLARE_MESSAGE_MAP()
};

class CAboutDlg : public CDialog
{
	public:
		explicit CAboutDlg();
		CAboutDlg(const CAboutDlg&) = delete;
		CAboutDlg(CAboutDlg&&) = delete;
		enum { IDD = IDD_ABOUTBOX };
		virtual BOOL OnInitDialog() override;
		afx_msg void OnBnClickedCancel() noexcept;
		DECLARE_MESSAGE_MAP()
};

#else

// add other platforms here.

#endif // _WIN32
