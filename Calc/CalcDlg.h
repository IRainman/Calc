
// CalcDlg.h : файл заголовка
//

#pragma once

// диалоговое окно CCalcDlg
class CCalcDlg : public CDialog
{
// Создание
	public:
		CCalcDlg(CWnd* pParent = NULL); // стандартный конструктор
		
// Данные диалогового окна
		enum { IDD = IDD_CALC_DIALOG };
		
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
		
		
// Реализация
	protected:
		HICON m_hIcon;
		
		// Созданные функции схемы сообщений
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnEnChangeEditInput();
};
