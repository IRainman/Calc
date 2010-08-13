
// Calc.cpp : Определяет поведение классов для приложения.
//

#include "stdafx.h"
#include "Calc.h"
#include "CalcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCalcApp

BEGIN_MESSAGE_MAP(CCalcApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// создание CCalcApp

CCalcApp::CCalcApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CCalcApp

CCalcApp theApp;


// инициализация CCalcApp

BOOL CCalcApp::InitInstance()
{
	// InitCommonControlsEx() требуется для Windows XP, если манифест
	// приложения использует ComCtl32.dll версии 6 или более поздней версии для включения
	// стилей отображения. В противном случае будет возникать сбой при создании любого окна.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Выберите этот параметр для включения всех общих классов управления, которые необходимо использовать
	// в вашем приложении.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	
	CWinAppEx::InitInstance();
	
	AfxEnableControlContainer();
	
	// Стандартная инициализация
	// Если эти возможности не используются и необходимо уменьшить размер
	// конечного исполняемого файла, необходимо удалить из следующих
	// конкретных процедур инициализации, которые не требуются
	// Измените раздел реестра, в котором хранятся параметры
	// TODO: следует изменить эту строку на что-нибудь подходящее,
	// например на название организации
//[-]L  SetRegistryKey(_T("Локальные приложения, созданные с помощью мастера приложений"));

	CCalcDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	/*  if (nResponse == IDOK)
	    {
	        // TODO: Введите код для обработки закрытия диалогового окна
	        //  с помощью кнопки "ОК"
	    }
	    else if (nResponse == IDCANCEL)
	    {
	        // TODO: Введите код для обработки закрытия диалогового окна
	        //  с помощью кнопки "Отмена"
	    }
	*/
	// Поскольку диалоговое окно закрыто, возвратите значение FALSE, чтобы можно было выйти из
	//  приложения вместо запуска генератора сообщений приложения.
	return FALSE;
}
