#pragma once

//#define CALC_MFC_USING_EXTENDENT_FUNCTIONS // disabled by default, because we have a simple application
//#define CALC_SUPPORT_WINDOWS_XP // deprecated by Windows SDK
//#define CALC_SUPPORT_WINDOWS_7_8_81 // deprecated by Windows SDK since version 18 of the MSVC compiler.

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // Ensure that we using min/max from std algorithms.

#include "targetver.h"

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#define _AFX_NO_COM_SUPPORT
#define _AFX_SINGLE_THREADED
#define _AFX_NO_OLE_SUPPORT
#define _AFX_NO_DB_SUPPORT
#define _AFX_NO_DAO_SUPPORT
#define _AFX_NO_AFXCMN_SUPPORT
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS

#define _ATL_CSTRING_NO_CRT
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _CONVERSION_DONT_USE_THREAD_LOCALE
#define _CSTRING_DISABLE_NARROW_WIDE_CONVERSION // ensure that we are not using conversion to wchar_t

#define _ATL_ALL_WARNINGS
#define _ATL_ALL_USER_WARNINGS
#define _ATL_SINGLE_THREADED
#define _ATL_NO_COM_SUPPORT
#define _ATL_NO_OLE_SUPPORT
#define _ATL_NO_DB_SUPPORT
#define _ATL_NO_DAO_SUPPORT
#define _ATL_NO_PERF_SUPPORT
#define _ATL_NO_DEFAULT_LIBS


#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
// To mimic older RichEdit behavior, set _RICHEDIT_VER to appropriate value
//		Version 1.0 	0x0100	
//		Version 2.0 	0x0200	
//		Version 2.1 	0x0210	
#define _RICHEDIT_VER 0x0210
#endif

#include <afxwin.h>         // MFC core and standard components

#ifdef CALC_MFC_USING_EXTENDENT_FUNCTIONS
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#endif
