// APOT6.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "APOT6.h"
#include "APOT6Dlg.h"

// XXX - Note if you start this lua CLI thread with the interpreter.  It will cause issues
// with the open file dialog box.  It seems like it doesn't like the blocking IO
// in the other thread.  -SEC
//#define LUA_SHELL 1
#ifdef LUA_SHELL
#include "LuaCommands.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HWND    ghDlg = 0;          // Handle to main dialog box.
HACCEL  ghAccelTable = 0;   // Handle to accelerator table.

/////////////////////////////////////////////////////////////////////////////
// CAPOT6App

BEGIN_MESSAGE_MAP(CAPOT6App, CWinApp)
    //{{AFX_MSG_MAP(CAPOT6App)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAPOT6App construction

CAPOT6App::CAPOT6App()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAPOT6App object

CAPOT6App theApp;


/////////////////////////////////////////////////////////////////////////////
// CAPOT6App initialization

BOOL CAPOT6App::InitInstance()
{
    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

#ifdef _AFXDLL
    Enable3dControls();			// Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    ghAccelTable = LoadAccelerators(AfxGetInstanceHandle(),
                                    MAKEINTRESOURCE(IDR_ACCELERATOR1));

    CAPOT6Dlg dlg;
    m_pMainWnd = &dlg;



#ifdef LUA_SHELL
    CWinThread* pThread = AfxBeginThread(RunCLI, &dlg, THREAD_PRIORITY_LOWEST);
#endif
    dlg.DoModal();
#if 0
    fputs("Bye!\n", stdout);
    fflush(stdout);
#endif
    //exit(0);
    //dlg.ShowWindow(SW_SHOW);

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.

    /* Check the termination message */
#if 0
    DWORD errorCode = ::GetLastError();
    CString errorMessage = "";
    LPTSTR s;
    if(::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        errorCode,
                        0,
                        (LPTSTR)&s,
                        0,
                        NULL) != 0)
    {
        LPTSTR p = _tcsrchr(s, _T('\r'));
        errorMessage = p;
        LocalFree(s);
    }
#endif
    return FALSE;
}

BOOL CAPOT6App::ProcessMessageFilter(int code, LPMSG lpMsg)
{
    if (code < 0) CWinApp::ProcessMessageFilter(code, lpMsg);

    if (ghDlg && ghAccelTable)
    {
        if (::TranslateAccelerator(ghDlg, ghAccelTable, lpMsg)) return(TRUE);
    }

    return CWinApp::ProcessMessageFilter(code, lpMsg);
}
