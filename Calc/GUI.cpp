// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
 * Copyright 2009-2024 Solomina Elle Leonovna, a.rainman on gmail point com
 */
 
// GUI.cpp : Defines the class behaviors for the application.
//

#include "pch.h"

#include "flags.h"
#include "lexer.h"
#include "parser.h"
#include "issue_manager.h"
#include "formatter.h"
#ifdef CALC_TESTS_ENABLED
#include "tests.h"
#endif

// ================= GUI ===============

#ifdef _WIN32

#include "framework.h"
#include "GUI.h"

BEGIN_MESSAGE_MAP(CCalcApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()



#include "resource.h"

// CCalcApp construction
CCalcApp::CCalcApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	IssueManager::speedup();
}


// The one and only CCalcApp object
CCalcApp theApp;


// CCalcApp initialization
BOOL CCalcApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_STANDARD_CLASSES | ICC_NATIVEFNTCTL_CLASS | ICC_PAGESCROLLER_CLASS | ICC_COOL_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
#endif

	CCalcDlg dlg;
	m_pMainWnd = &dlg;
#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
	INT_PTR nResponse = 
#endif
		dlg.DoModal();
#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
	if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel [Pressed Esc or Close button]
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	delete pShellManager;

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}








CCalcDlg::CCalcDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CALC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
void CCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
#endif

BEGIN_MESSAGE_MAP(CCalcDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CCalcDlg::OnBnClickedButtonCalc)
END_MESSAGE_MAP()


// CCalcDlg message handlers
BOOL CCalcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	static_assert((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	static_assert(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
#ifdef CALC_TESTS_ENABLED
	SetDlgItemText(IDC_EDIT_MESSAGE, calc_tests().data());
#endif

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCalcDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CCalcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCalcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCalcDlg::OnBnClickedButtonCalc()
{
	if(CString text; GetDlgItemTextA(IDC_EDIT_INPUT, text) >= 1)
	{
		const std::string_view input(text.GetString(), static_cast<unsigned int>(text.GetLength()));

		Lexer l{ input };
		Parser p{ l };

		const auto result = p.parse();

		if (std::isnan(result))
		{
			SetDlgItemTextA(IDC_EDIT_RESULT, "");
			SetDlgItemTextA(IDC_EDIT_MESSAGE, Formatter::create_summary().data());
			IssueManager::clear();
		}
		else
		{
			SetDlgItemTextA(IDC_EDIT_RESULT, Formatter::format(result).data());
			SetDlgItemTextA(IDC_EDIT_MESSAGE, "");
		}
	}
	else
	{
		SetDlgItemTextA(IDC_EDIT_RESULT, "");
		SetDlgItemTextA(IDC_EDIT_MESSAGE, "");
	}
	GetDlgItem(IDC_EDIT_INPUT)->SetFocus();
}


// CAboutDlg dialog used for App About
CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
#endif

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDlgItemTextA(IDC_COMPILED_DATE, _T(__TIMESTAMP__));
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_CANCEL, &CAboutDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

void CAboutDlg::OnBnClickedCancel() noexcept
{
	EndDialog(FALSE);
}


#else

// add other platforms here.

#endif // _WIN32
