// APOT6.h : main header file for the APOT6 application
//

#if !defined(AFX_APOT6_H__3B792D96_5074_4100_AB62_73791198F58F__INCLUDED_)
#define AFX_APOT6_H__3B792D96_5074_4100_AB62_73791198F58F__INCLUDED_

// Turns off the warnings about sprintf()s.
#pragma warning(disable:4996)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define NET_ENVIRON	1

/////////////////////////////////////////////////////////////////////////////
// CAPOT6App:
// See APOT6.cpp for the implementation of this class
//

#define WM_USER_UPDATE_NEC_BUTTON	WM_USER+1
#define WM_USER_UPDATE_RUN_BUTTON	WM_USER+2
#define WM_USER_UPDATE_GA			WM_USER+3
#define WM_USER_ANTENNA_POPUP		WM_USER+4
#define WM_USER_UPDATE_PLATFORM		WM_USER+5
#define WM_USER_UPDATE_SA			WM_USER+6
#define WM_USER_UPDATE_HC			WM_USER+7
#define WM_USER_UPDATE_ES			WM_USER+8
#define WM_COMPLETE					WM_USER+9

class CAPOT6App : public CWinApp
{
public:
    CAPOT6App();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAPOT6App)
public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

// Implementation

    //{{AFX_MSG(CAPOT6App)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    virtual BOOL ProcessMessageFilter(int code, LPMSG lpMsg);
};


/////////////////////////////////////////////////////////////////////////////
extern HWND    ghDlg;          // Handle to main dialog box.
extern HACCEL  ghAccelTable;   // Handle to accelerator table.
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APOT6_H__3B792D96_5074_4100_AB62_73791198F58F__INCLUDED_)
