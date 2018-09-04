// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// CalcDlg.cpp : файл реализации
//

#include "stdafx.h"
#include "Calc.h"
#include "CalcDlg.h"
#include "CalculatorWideAdapter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialog
{
	public:
		CAboutDlg();
		
// Данные диалогового окна
		enum { IDD = IDD_ABOUTBOX };
		
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
		
// Реализация
	protected:
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnBnClickedSite();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//std::wstring llll;
	//getCompileDate(llll);
	//dlg
	//dlg.PrepareCtrl(IDC_COMPILED_DATE);
	//SetDlgItemText(IDC_COMPILED_DATE, llll.c_str());
	//::SetDlgItemText(AfxGetApp()->m_hThread, IDC_COMPILED_DATE, getCompileDate().c_str());
}

inline void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(ID_SITE, &CAboutDlg::OnBnClickedSite)
END_MESSAGE_MAP()

// диалоговое окно CCalcDlg

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


// обработчики сообщений CCalcDlg

BOOL CCalcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// Добавление пункта "О программе..." в системное меню.
	
	// IDM_ABOUTBOX должен быть в пределах системной команды.
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
	
	// Задает значок для этого диалогового окна. Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);         // Крупный значок
	SetIcon(m_hIcon, FALSE);        // Мелкий значок
	
	// TODO: добавьте дополнительную инициализацию
	CheckDlgButton(IDC_CHECK_AUTO_CALCULATE, TRUE);
	
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
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

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок. Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

inline void CCalcDlg::OnPaint()
{
	//if (IsIconic())
	//{
	//  CPaintDC dc(this); // контекст устройства для рисования
	//
	//  SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
	//
	//  // Выравнивание значка по центру клиентского прямоугольника
	//  int cxIcon = GetSystemMetrics(SM_CXICON);
	//  int cyIcon = GetSystemMetrics(SM_CYICON);
	//  CRect rect;
	//  GetClientRect(&rect);
	//  int x = (rect.Width() - cxIcon + 1) / 2;
	//  int y = (rect.Height() - cyIcon + 1) / 2;
	//
	//  // Нарисуйте значок
	//  dc.DrawIcon(x, y, m_hIcon);
	//}
	//else
	{
		CDialog::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
inline HCURSOR CCalcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCalcDlg::OnEnChangeEditInput()
{
	m_input.resize(c_max_edit_input_size);
	const auto l_count = GetDlgItemText(IDC_EDIT_INPUT, &m_input[0], c_max_edit_input_size);
	if (l_count > 0)
	{
		m_input.resize(static_cast<wstring::size_type>(l_count));
		
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
			SetDlgItemText(IDC_EDIT_MESSAGE, l_message.c_str());
			SetDlgItemText(IDC_EDIT_RESULT, m_result.c_str());
		}
	}
	else
	{
		SetDlgItemText(IDC_EDIT_MESSAGE, L"");
		SetDlgItemText(IDC_EDIT_RESULT, L"");
	}
}

void CAboutDlg::OnBnClickedSite()
{
	::ShellExecute(NULL, NULL, L"http://studia2000.sytes.net/txt/calc.shtml", NULL, NULL, SW_SHOWNORMAL);
}
