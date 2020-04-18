
// CalcDlg.h : файл заголовка
//

#pragma once

#include <string>

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
		HICON m_hIcon;//-V122_NOPTR
		
		// Созданные функции схемы сообщений
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnEnChangeEditInput();
	private:
		static constexpr std::wstring::size_type c_max_edit_input_size = 4096;
		std::wstring m_input;
		std::wstring m_result;
};
