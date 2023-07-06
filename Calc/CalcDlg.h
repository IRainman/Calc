#pragma once
/*
 * Copyright 2009-2023 Solomina Elle Leonovna, a.rainman on gmail point com
 */

class CCalcDlg : public CDialog
{
	public:
		explicit CCalcDlg(CWnd* pParent = nullptr) noexcept;
		CCalcDlg(const CCalcDlg&) = delete;
		CCalcDlg(CCalcDlg&&) = delete;
		
		enum { IDD = IDD_CALC_DIALOG };
		
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
		
	protected:
		HICON m_hIcon;//-V122_NOPTR
		
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg HCURSOR OnQueryDragIcon() noexcept;
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnEnChangeEditInput();
};
