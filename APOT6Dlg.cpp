// GA_APOTDlg.cpp : implementation file
//

#include "stdafx.h"
#include "APOT6.h"
#include "APOT6Dlg.h"
#include "Geometry.h"
#include "AntennaConfiguration.h"
#include <assert.h>
#include <limits.h>
#include "EADlg.h"
#include "LuaCommands.h"
#include "StatusMessages.h"
#include "SplashDialog.h"
#include "Algorithms.h"
#include "SADlg.h"
#include "SA.h"
#include "HCDlg.h"
#include "HC.h"
#include "PopulationBasedAlgos.h"
#include "EvolutionaryStrategy.h"
#include "SaveToLua.h"
#include <time.h>
#include "APOT_svn_details.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

#define END_OF_RUN	10000
#define ABORTED_RUN	10001

int run_no = 0;
/*abaj*/
int last_algo_executed = 0;

#define LIST_RESULTS_MAX 100

CSplashDialog* CSplashDialog::c_pSplashDlg;
static CAPOT6Dlg* dialogInstance = NULL;
UINT RunGA(LPVOID pParam);
UINT RunSA(LPVOID pParam);
UINT RunHC(LPVOID pParam);
UINT RunES(LPVOID pParam);

int selected_algorithm = 0;
// CAPOT6Dlg dialog

CAPOT6Dlg::CAPOT6Dlg(CWnd* pParent /*=NULL*/)
    : CDialog(CAPOT6Dlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    dialogInstance = this;
}

void CAPOT6Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_ANTENNA, treeControl);
}

BEGIN_MESSAGE_MAP(CAPOT6Dlg, CDialog)
    //{{AFX_MSG_MAP(CAPOT6Dlg)
    ON_WM_SIZING()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_PLATFORM, OnBnClickedPlatform)
    ON_BN_CLICKED(IDC_BUTTON_ANTENNA, OnBnClickedButtonAntenna)
    ON_EN_CHANGE(IDC_EDIT_MIN_THETA, OnEnChangeEditMinTheta)
    ON_BN_CLICKED(IDC_BUTTON_ANTENNA_REMOVE, OnBnClickedButtonAntennaRemove)
    ON_EN_CHANGE(IDC_EDIT_STEP_THETA, OnEnChangeEditStepTheta)
    ON_EN_CHANGE(IDC_EDIT_MIN_PHI, OnEnChangeEditMinPhi)
    ON_EN_CHANGE(IDC_EDIT_STEP_PHI, OnEnChangeEditStepPhi)
    ON_EN_CHANGE(IDC_EDIT_MIN_FREQ, OnEnChangeEditMinFreq)
    ON_EN_CHANGE(IDC_EDIT_STEP_FREQ, OnEnChangeEditStepFreq)
    ON_EN_CHANGE(IDC_EDIT_INC_PHI, OnEnChangeEditIncPhi)
    ON_EN_CHANGE(IDC_EDIT_INC_THETA, OnEnChangeEditIncTheta)
    ON_EN_CHANGE(IDC_EDIT_INC_FREQ, OnEnChangeEditIncFreq)
    ON_BN_CLICKED(IDC_BUTTON_NEC, OnBnClickedButtonNec)
//	ON_MESSAGE(WM_USER_UPDATE_NEC_BUTTON, OnUpdateNECButton)
//	ON_MESSAGE(WM_USER_UPDATE_RUN_BUTTON, OnUpdateRunButton)
    ON_MESSAGE(WM_USER_UPDATE_GA, OnUpdateGA)
    ON_EN_CHANGE(IDC_EDIT_USERNAME, OnEnChangeEditUsername)
    ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnEnChangeEditPassword)
    ON_BN_CLICKED(IDC_BUTTON_RUN, OnBnClickedButtonRun)
    ON_LBN_DBLCLK(IDC_LIST_RESULTS, OnDblclkListResults)
    ON_BN_CLICKED(IDC_CHECK_LOCAL, OnCheckLocal)
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
    //ON_BN_CLICKED(IDC_BUTTON_EDIT, &CAPOT6Dlg::OnBnClickedButtonEdit)
    ON_BN_CLICKED(IDC_BUTTON_LUA, &CAPOT6Dlg::OnBnClickedButtonLua)
    ON_BN_CLICKED(IDC_BUTTON_X, &CAPOT6Dlg::OnBnClickedButtonX)
    ON_BN_CLICKED(IDC_BUTTON_Y, &CAPOT6Dlg::OnBnClickedButtonY)
    ON_BN_CLICKED(IDC_BUTTON_Z, &CAPOT6Dlg::OnBnClickedButtonZ)
    ON_BN_CLICKED(IDC_BUTTON_X_NEG, &CAPOT6Dlg::OnBnClickedButtonXNeg)
    ON_BN_CLICKED(IDC_BUTTON_Y_NEG, &CAPOT6Dlg::OnBnClickedButtonYNeg)
    ON_BN_CLICKED(IDC_BUTTON_Z_NEG, &CAPOT6Dlg::OnBnClickedButtonZNeg)
    ON_BN_CLICKED(IDC_CHECK_ORTHO, &CAPOT6Dlg::OnBnClickedCheckOrtho)
    ON_BN_CLICKED(IDC_BUTTON_EDIT_NEC, &CAPOT6Dlg::OnBnClickedButtonEditNec)
    ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CAPOT6Dlg::OnBnClickedButtonAbout)
    ON_BN_CLICKED(IDC_BUTTON_HELP, &CAPOT6Dlg::OnBnClickedButtonHelp)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_ANTENNA, &CAPOT6Dlg::OnNMDblclkTreeAntenna)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ANTENNA, &CAPOT6Dlg::OnTvnSelchangedTreeAntenna)
    ON_LBN_DBLCLK(IDC_LIST_ALGORITHMS, &CAPOT6Dlg::OnDblclkListAlgorithms)
    ON_LBN_SELCHANGE(IDC_LIST_ALGORITHMS, &CAPOT6Dlg::OnSelchangeListAlgorithms)
    ON_MESSAGE(WM_USER_UPDATE_SA, &CAPOT6Dlg::OnUpdateSA)
    ON_MESSAGE(WM_USER_UPDATE_HC, &CAPOT6Dlg::OnUpdateHC)
    ON_BN_CLICKED(IDC_BUTTON_SAVE_TO_LUA, &CAPOT6Dlg::OnBnClickedButtonSaveToLua)
    ON_MESSAGE(WM_USER_UPDATE_ES, &CAPOT6Dlg::OnUpdateES)
//	ON_LBN_SETFOCUS(IDC_LIST_RESULTS, &CAPOT6Dlg::OnSetfocusListResults)
ON_MESSAGE(WM_COMPLETE, &CAPOT6Dlg::OnComplete)
END_MESSAGE_MAP()


// CAPOT6Dlg message handlers

BOOL CAPOT6Dlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CSplashDialog::ShowSplashScreen(NULL);
    //system("SubWCRev ./ ./svntemplate ./APOT_svn_details.h");
	GetDlgItem(IDC_EDIT_BEST_FITNESS)->EnableWindow(false);
	GetDlgItem(IDC_EDIT_BEST_FITNESS)->SetWindowText("NA");
    char buffer[100];
    sprintf(buffer, "APOT6 v0.1 Build %s", Revision);
    SetWindowText(buffer);

// Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    CRect rect;

    int err;

//	CBitmap bmp;
//	err = bmp.LoadBitmap(IDB_BITMAP_TREE);
    static CImageList imglist;
    err = imglist.Create(16, 16, ILC_COLOR4, 0, 1);
//	err = imglist.Add(&bmp, RGB(255, 255, 255));
    err = imglist.Add(AfxGetApp()->LoadIcon(IDI_ICON_TREE));
    treeControl.SetImageList(&imglist, TVSIL_NORMAL);

    GetDlgItem(IDC_OPENGL_VIEW)->GetWindowRect(rect);
    ScreenToClient(rect);
    openGLControl.Create(rect,this);

    GetDlgItem(IDC_THETA_CONTROL)->GetWindowRect(rect);
    ScreenToClient(rect);
    thetaControl.Create(rect,this);

    GetDlgItem(IDC_PHI_CONTROL)->GetWindowRect(rect);
    ScreenToClient(rect);
    phiControl.Create(rect,this);

    // Set all of the EAInfo default values from eaDialog.

    eaDialog.ApplyValues();
    saDialog.ApplyValues();
    hcDialog.ApplyValues();
    esDialog.ApplyValues();

    ((CButton*) GetDlgItem(IDC_CHECK_LOCAL))->SetCheck(RunLocal ? BST_CHECKED : BST_UNCHECKED);

    kColors[0].set(1.0f, 1.0f, 0.0f);
    kColors[1].set(1.0f, 0.0f, 0.0f);
    kColors[2].set(1.0f, 0.0f, 1.0f);
    kColors[3].set(0.0f, 0.0f, 1.0f);
    kColors[4].set(1.0f, 1.0f, 0.0f);


    necDialog.Create(IDD_NEC_DIALOG, this);

    ((CButton*) GetDlgItem(IDC_BUTTON_ABOUT))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_ABOUT));
    ((CButton*) GetDlgItem(IDC_BUTTON_HELP))->SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_HELP));

    Sleep(4000);
    CSplashDialog::HideSplashScreen();
    CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_ALGORITHMS);
    list->AddString("Evolutionary Algorithm");
    list->AddString("Simulated Annealing");
    list->AddString("Hill Climber");
    list->AddString("Exhaustive Search");
    list->AddString("Evolutionary Strategy");

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void 
CAPOT6Dlg::OnSizing(UINT nSide,
					LPRECT lpRect)
{
    GetWindowRect(&m_prevRect);
    CDialog::OnSizing(nSide, lpRect);
}

void 
CAPOT6Dlg::OnSize(UINT nType, 
				int cx,
				int cy)
{
    CDialog::OnSize(nType, cx, cy);

    CRect dlgRect;
    GetWindowRect(&dlgRect);
    int height = dlgRect.Height();
    int width = dlgRect.Width();

    CRect ctrlRect;
    CWnd* wnd;

    wnd = GetDlgItem(IDC_TREE_ANTENNA);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 0, 0, ctrlRect.Width(),height-530, SWP_NOMOVE | SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_BUTTON_ANTENNA);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 10, height-460, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_BUTTON_ANTENNA_REMOVE);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 130, height-460, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_STATIC_THETA);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 10, height-430, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_THETA_CONTROL);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        thetaControl.SetWindowPos(NULL, 120, height-405, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_T1);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-405, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_MIN_THETA);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-410, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_T2);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-375, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_STEP_THETA);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-380, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_T3);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-345, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_INC_THETA);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-350, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_STATIC_PHI);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 10, height-310, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_PHI_CONTROL);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        phiControl.SetWindowPos(NULL, 120, height-285, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_P1);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-290, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_MIN_PHI);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-295, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_P2);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-260, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_STEP_PHI);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-265, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_P3);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-230, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_INC_PHI);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-235, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_STATIC_FREQ);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 10, height-190, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_F1);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-165, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_MIN_FREQ);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-170, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_F2);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-135, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_STEP_FREQ);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-140, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_F3);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 65-ctrlRect.Width(), height-105, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_INC_FREQ);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 68, height-110, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_BUTTON_EDIT_NEC);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 10, height-70, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_BUTTON_NEC);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 90, height-70, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_PROGRESS);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 208, height-79, width-400, ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_STATUS);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, 210, height-53, width-400, ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_BUTTON_EDIT);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-20, 67, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    /*
    wnd = GetDlgItem(IDC_STATIC_GA);
    if (wnd != NULL) {
    	wnd->GetWindowRect(&ctrlRect);
    	wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-20, 15, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_POPSIZE);
    if (wnd != NULL) {
    	wnd->GetWindowRect(&ctrlRect);
    	wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-90, 40, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_POPSIZE);
    if (wnd != NULL) {
    	wnd->GetWindowRect(&ctrlRect);
    	wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-25, 35, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_GENERATIONS);
    if (wnd != NULL) {
    	wnd->GetWindowRect(&ctrlRect);
    	wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-90, 70, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_GENERATIONS);
    if (wnd != NULL) {
    	wnd->GetWindowRect(&ctrlRect);
    	wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-25, 65, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_MUTATION);
    if (wnd != NULL) {
    	wnd->GetWindowRect(&ctrlRect);
    	wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-90, 100, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_MUTATION);
    if (wnd != NULL) {
    	wnd->GetWindowRect(&ctrlRect);
    	wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-25, 95, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    */
    wnd = GetDlgItem(IDC_CHECK_LOCAL);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-95, 173, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

	wnd = GetDlgItem(IDC_STATIC_BEST_FITNESS);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-123, 153, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

	wnd = GetDlgItem(IDC_EDIT_BEST_FITNESS);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-35, 150, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }


    wnd = GetDlgItem(IDC_STATIC_ALGORITHMS);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-20, 65, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_LIST_ALGORITHMS);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-30, 80, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
        wnd->RedrawWindow();
    }

    wnd = GetDlgItem(IDC_STATIC_RESULTS);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-20, 190, ctrlRect.Width(), height-320, SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_LIST_RESULTS);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-25, 210, ctrlRect.Width(), height-350, SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_STATIC_USERNAME);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-130, height-120, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_USERNAME);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-20, height-125, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_STATIC_PASSWORD);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-130, height-95, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_EDIT_PASSWORD);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-20, height-100, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_BUTTON_RUN);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-20, height-70, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_BUTTON_SAVE_TO_LUA);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-100, height-70, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_BUTTON_X);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-150, 11, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_BUTTON_X_NEG);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-150, 37, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_BUTTON_Y);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-120, 11, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_BUTTON_Y_NEG);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-120, 37, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_BUTTON_Z);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-90, 11, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }
    wnd = GetDlgItem(IDC_BUTTON_Z_NEG);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-90, 37, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_CHECK_ORTHO);
    if (wnd != NULL)
    {
        wnd->GetWindowRect(&ctrlRect);
        wnd->SetWindowPos(NULL, dlgRect.Width()-ctrlRect.Width()-25, 15, ctrlRect.Width(), ctrlRect.Height(), SWP_NOZORDER);
    }

    wnd = GetDlgItem(IDC_OPENGL_VIEW);
    if (wnd != NULL)
    {
        openGLControl.SetWindowPos(NULL, 0, 0, width-400, height-94, SWP_NOMOVE | SWP_NOZORDER);
    }

    Invalidate(TRUE);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void 
CAPOT6Dlg::OnPaint()
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
        thetaControl.UpdateWindow();
        phiControl.UpdateWindow();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAPOT6Dlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void 
CAPOT6Dlg::LoadPlatform(CString filename)
{
    if (platform != NULL)
    {
        for (unsigned int ii = 0; ii < antennas.size(); ii++)
        {
            antennas[ii]->erase_points();
        }
        delete platform;
    }
	mesh_resolution = 0.0f;
    platform = new antenna;
	platform->isPlatform = true;
    strcpy(platform->label, filename);
    platform->load();
    platform->lines2points();
	/* Pythagoras Theorem */
	default_mesh_resolution = mesh_resolution = sqrt( 2 * pow(mesh_resolution, 2) );
    //GetDlgItem(IDC_STATIC_PLATFORM)->SetWindowText(filename);
}

void 
CAPOT6Dlg::UpdateGUI()
{
    GetDlgItem(IDC_BUTTON_EDIT_NEC)->EnableWindow(platform != NULL);
    // Update the platform name.
    GetDlgItem(IDC_STATIC_PLATFORM)->SetWindowText(CString(platform->label));

    // Update the antenna dialog box.
//	if (antennas.size() != treeControl.GetCount()) {	// TBD

    VERIFY(treeControl.DeleteAllItems());
    for (unsigned i = 0; i < antennas.size(); i++)
    {

        antenna* ant = antennas[i];

        TVINSERTSTRUCT tvInsert;
        tvInsert.hParent = TVI_ROOT;
        tvInsert.hInsertAfter = TVI_LAST;
        tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        tvInsert.item.iImage = 0;
        tvInsert.item.iSelectedImage = 0;
        CString buffer(ant->label);
        buffer.AllocSysString();
        tvInsert.item.pszText = buffer.GetBuffer(0);
        HTREEITEM hItem = treeControl.InsertItem(&tvInsert);
        treeControl.SetItemData(hItem, (DWORD_PTR) ant);
        treeControl.SelectItem(hItem);
    }

    LRESULT Result;
    OnTvnSelchangedTreeAntenna(NULL, &Result);
    OnUpdateNECButton(0, 0);
//	}
    GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(RunLocal?0:1);
    GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(RunLocal?0:1);

    // Update the run button.
    if(selected_algorithm == EVOLUTIONARY_ALGORITHM || selected_algorithm == EXHAUSTIVE_SEARCH)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || GAparams_valid()) && antennas_valid());
    else if(selected_algorithm == SIMULATED_ANNEALING)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || SAparams_valid()) && antennas_valid());
    else if(selected_algorithm == HILL_CLIMBER)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || HCparams_valid()) && antennas_valid());
    else if(selected_algorithm == EVOLUTIONARY_STRATEGY)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || ESparams_valid()) && antennas_valid());


    ((CButton*) GetDlgItem(IDC_CHECK_LOCAL))->SetCheck(RunLocal ? BST_CHECKED : BST_UNCHECKED);

    CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_ALGORITHMS);
    list->SetCurSel(selected_algorithm);

    eaDialog.UpdateValues();
    saDialog.UpdateValues();
    hcDialog.UpdateValues();
    esDialog.UpdateValues();
}

void
CAPOT6Dlg::OnBnClickedPlatform()
{
    char strFilter[] = { "NEC Files (*.nec)|*.nec; *.lua||" };
    CFileDialog dlgFile(TRUE, "*.nec", NULL, 0, strFilter);

    if (dlgFile.DoModal() == IDOK)
    {
// I was going to add this as a kind of easter egg, but there are
// complications that I wasn't aware of.  It seems that I can't make
// GetDlgItem() calls when its run through Lua in the same thread.
        //char *filename = T2A(dlgFile.GetFileName().GetBuffer());
        CString f = dlgFile.GetFileName();
        CString ext = dlgFile.GetFileExt();
        const char *filename = (LPCTSTR) f;
        if (stricmp("lua", ext) == 0)
        {
            // Assumption here is that the lua file contains the test antennas as well, so clear out what we have.
            ClearScenario();
            initLua();
            loadLuaLibrary((char *) filename);
            closeLua();
            UpdateGUI();
        }
        else
        {
            //LoadPlatform(dlgFile.GetPathName());
            LoadPlatform(dlgFile.GetFileName());
            // Update the platform name.
            GetDlgItem(IDC_STATIC_PLATFORM)->SetWindowText(dlgFile.GetFileName());
            GetDlgItem(IDC_BUTTON_EDIT_NEC)->EnableWindow(platform != NULL);
        }
        openGLControl.UpdatePlatform();
    }
}

void 
CAPOT6Dlg::ClearScenario(void)
{
    if (platform != NULL)
    {
        delete platform;
        platform = NULL;
    }
    for (unsigned int ii = 0; ii < antennas.size(); ii++) delete antennas[ii];
    antennas.clear();
}

void 
CAPOT6Dlg::AddAntenna(CString filename)
{
    antenna* ant = new antenna;
    strcpy(ant->label, filename);
    ant->load();
    ant->color = kColors[antennas.size() % NUM_COLORS];
    antennas.push_back(ant);
}

void 
CAPOT6Dlg::OnBnClickedButtonAntenna()
{
    char strFilter[] = { "NEC Files (*.nec)|*.nec||" };
    CFileDialog dlgFile(TRUE, "*.nec", NULL, 0, strFilter);

    if (dlgFile.DoModal() == IDOK)
    {
        AddAntenna(dlgFile.GetFileName());
        antenna *ant = antennas.back();

        TVINSERTSTRUCT tvInsert;
        tvInsert.hParent = TVI_ROOT;
        tvInsert.hInsertAfter = TVI_LAST;
        tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
        tvInsert.item.iImage = 0;
        tvInsert.item.iSelectedImage = 0;
        CString buffer(dlgFile.GetFileName());
        buffer.AllocSysString();
        tvInsert.item.pszText = buffer.GetBuffer(0);
        HTREEITEM hItem = treeControl.InsertItem(&tvInsert);
        treeControl.SetItemData(hItem, (DWORD_PTR) ant);
        treeControl.SelectItem(hItem);

        //		list->AddString(dlgFile.GetFileName());
//		int idx = list->GetCount()-1;
//		list->SetItemData(idx, (DWORD_PTR) ant);
        openGLControl.display_antenna = ant;
//		list->SetCurSel(idx);
        treeControl.SetFocus();
//		OnTvnSelchangedTreeAntenna();
        OnUpdateNECButton(0, 0);

        //UpdateGUI();
        //CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_ANTENNA);
        //list->SetFocus();
        //OnTvnSelchangedTreeAntenna();
    }
}


void 
CAPOT6Dlg::OnBnClickedButtonSaveToLua()
{
    SaveToLua stl;
    CFileDialog dlgFile(FALSE, "*.lua*", NULL, 0, "LUA Files (*.lua)|*.lua||");
    if (dlgFile.DoModal() == IDOK)
    {
        stl.addPlatform(platform->label);
        for (int ii = 0; ii < antennas.size(); ii++)
        {
            stl.addAntenna(antennas[ii]->label);
            for (int jj=0; jj<antennas[ii]->points->size(); jj++)
                stl.addPoint(antennas[ii]->points->at(jj)->x, antennas[ii]->points->at(jj)->y, antennas[ii]->points->at(jj)->z);
        }
        switch(selected_algorithm)
        {
			case EVOLUTIONARY_ALGORITHM:
			{
				stl.setAlgorithm(EVOLUTIONARY_ALGORITHM);
				stl.setGAInfo(EAInfo);
				break;
			}
			case SIMULATED_ANNEALING:
			{
				stl.setAlgorithm(SIMULATED_ANNEALING);
				stl.setSAInfo(SAInfo);
				break;
			}
			case HILL_CLIMBER:
			{
				stl.setAlgorithm(HILL_CLIMBER);
				stl.setHCInfo(HCInfo);
				break;
			}
	        case EXHAUSTIVE_SEARCH:
		    {
			    EAInfo.algorType = EXHAUST;
				stl.setAlgorithm(EXHAUSTIVE_SEARCH);
				//stl.setGAInfo(EAInfo);
				break;
			}

			case EVOLUTIONARY_STRATEGY:
			{
				stl.setAlgorithm(EVOLUTIONARY_STRATEGY);
				stl.setESInfo(ESInfo);
				break;
			}
        }
        stl.setLocalRun(RunLocal);
        stl.writeToLua(dlgFile.GetFileName());
    }
}

void 
CAPOT6Dlg::AddPointToLastAntenna(point *pt)
{
    antennas.back()->points->push_back(new point(*pt));
}

void 
CAPOT6Dlg::OnBnClickedButtonAntennaRemove()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        for (unsigned int jj = 0; jj < antennas.size(); jj++)
        {
            if (antennas[jj] == ant)
            {
                antennas.erase(antennas.begin() + jj);
                break;
            }
        }
        if (openGLControl.display_antenna == ant) openGLControl.display_antenna = NULL;
        delete ant;
        HTREEITEM newSelection = treeControl.GetNextSiblingItem(hItem);
        if (newSelection == NULL) treeControl.GetPrevSiblingItem(hItem);
        treeControl.DeleteItem(hItem);
        if (newSelection != NULL) treeControl.SelectItem(newSelection);
        treeControl.SetFocus();
        LRESULT Result;
        OnTvnSelchangedTreeAntenna(NULL, &Result);
        OnUpdateNECButton(0, 0);
    }
}

void 
CAPOT6Dlg::OnEnChangeEditMinTheta()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_MIN_THETA))->GetWindowText(buffer);
        ant->params.min_theta = atoi(buffer);
        thetaControl.minAngle = ant->params.min_theta;
        thetaControl.maxAngle = ant->params.min_theta + ant->params.step_theta * ant->params.incr_theta;
        thetaControl.Invalidate(TRUE);
        thetaControl.UpdateWindow();
    }
}

void 
CAPOT6Dlg::OnEnChangeEditStepTheta()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_STEP_THETA))->GetWindowText(buffer);
        ant->params.step_theta = atoi(buffer);
        thetaControl.maxAngle = ant->params.min_theta + ant->params.step_theta * ant->params.incr_theta;
        thetaControl.Invalidate(TRUE);
        thetaControl.UpdateWindow();
    }
}

void 
CAPOT6Dlg::OnEnChangeEditMinPhi()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_MIN_PHI))->GetWindowText(buffer);
        ant->params.min_phi = atoi(buffer);
        phiControl.minAngle = ant->params.min_phi;
        phiControl.maxAngle = ant->params.min_phi + ant->params.step_phi * ant->params.incr_phi;
        phiControl.Invalidate(TRUE);
        phiControl.UpdateWindow();
    }
}

void 
CAPOT6Dlg::OnEnChangeEditStepPhi()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_STEP_PHI))->GetWindowText(buffer);
        ant->params.step_phi = atoi(buffer);
        phiControl.maxAngle = ant->params.min_phi + ant->params.step_phi * ant->params.incr_phi;
        phiControl.Invalidate(TRUE);
        phiControl.UpdateWindow();
    }
}

void
CAPOT6Dlg::OnEnChangeEditMinFreq()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_MIN_FREQ))->GetWindowText(buffer);
        ant->params.min_freq = atof(buffer);
    }
}

void 
CAPOT6Dlg::OnEnChangeEditStepFreq()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_STEP_FREQ))->GetWindowText(buffer);
        ant->params.step_freq = atoi(buffer);
    }
}

void 
CAPOT6Dlg::OnEnChangeEditIncTheta()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_INC_THETA))->GetWindowText(buffer);
        ant->params.incr_theta = atoi(buffer);
        thetaControl.maxAngle = ant->params.min_theta + ant->params.step_theta * ant->params.incr_theta;
        thetaControl.Invalidate(TRUE);
        thetaControl.UpdateWindow();
    }
}

void 
CAPOT6Dlg::OnEnChangeEditIncPhi()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_INC_PHI))->GetWindowText(buffer);
        ant->params.incr_phi = atoi(buffer);
        phiControl.maxAngle = ant->params.min_phi + ant->params.step_phi * ant->params.incr_phi;
        phiControl.Invalidate(TRUE);
        phiControl.UpdateWindow();
    }
}


void
CAPOT6Dlg::OnEnChangeEditIncFreq()
{
    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);
        CString buffer;
        ((CEdit*) GetDlgItem(IDC_EDIT_INC_FREQ))->GetWindowText(buffer);
        ant->params.incr_freq = atoi(buffer);
    }
}

void 
CAPOT6Dlg::OnBnClickedButtonNec()
{
    if (platform != NULL)
    {
        editDialog.UpdateText();
        editDialog.ShowWindow(SW_SHOW);
        editDialog.BringWindowToTop();
    }
}

LRESULT 
CAPOT6Dlg::OnUpdateNECButton(WPARAM wParam, 
							LPARAM lParam)
{
    GetDlgItem(IDC_BUTTON_NEC)->EnableWindow(antennas_valid());
    OnUpdateRunButton(0, 0);
    return 0;
}

LRESULT 
CAPOT6Dlg::OnUpdateRunButton(WPARAM wParam, 
							LPARAM lParam)
{
    if (selected_algorithm == EVOLUTIONARY_ALGORITHM || selected_algorithm == EXHAUSTIVE_SEARCH)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || GAparams_valid()) && antennas_valid());
    else if (selected_algorithm == SIMULATED_ANNEALING)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || SAparams_valid()) && antennas_valid());
    else if (selected_algorithm == HILL_CLIMBER)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || HCparams_valid()) && antennas_valid());
	 else if (selected_algorithm == EVOLUTIONARY_STRATEGY)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || ESparams_valid()) && antennas_valid());
    return 0;
}

void 
CAPOT6Dlg::OnEnChangeEditPopsize()
{
    CString buffer;
    ((CEdit*) GetDlgItem(IDC_EDIT_POPSIZE))->GetWindowText(buffer);
    EAInfo.popSize = atoi(buffer);
    OnUpdateRunButton(0, 0);
}

void 
CAPOT6Dlg::OnEnChangeEditGenerations()
{
    CString buffer;
    ((CEdit*) GetDlgItem(IDC_EDIT_GENERATIONS))->GetWindowText(buffer);
    EAInfo.generations = atoi(buffer);
    OnUpdateRunButton(0, 0);
}

void
CAPOT6Dlg::OnEnChangeEditMutation()
{
    CString buffer;
    ((CEdit*) GetDlgItem(IDC_EDIT_MUTATION))->GetWindowText(buffer);
    EAInfo.mutation = (float) atof(buffer);
    OnUpdateRunButton(0, 0);
}

void 
CAPOT6Dlg::OnEnChangeEditUsername()
{
    ((CEdit*) GetDlgItem(IDC_EDIT_USERNAME))->GetWindowText(EAInfo.username);
    OnUpdateRunButton(0, 0);
}

void 
CAPOT6Dlg::OnEnChangeEditPassword()
{
    ((CEdit*) GetDlgItem(IDC_EDIT_PASSWORD))->GetWindowText(EAInfo.password);
    OnUpdateRunButton(0, 0);
}


void 
CAPOT6Dlg::OnBnClickedButtonRun()
{
    CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_RESULTS);
    for (int ii = list->GetCount() - 1; ii >= 0; ii--)
        list->DeleteString(ii);
    last_algo_executed = selected_algorithm;

    /*tbll Why delete required even with clear()?*/
    for (unsigned ii = 0; ii < outputs.size(); ii++) delete outputs[ii];
    outputs.clear();

    switch(selected_algorithm)
    {

    case EVOLUTIONARY_ALGORITHM:
    {
		
		StartEA();
        break;
    }
    case SIMULATED_ANNEALING:
    {
        StartSA();
        break;
    }
    case HILL_CLIMBER:
    {
        StartHC();
        break;
    }

    case EXHAUSTIVE_SEARCH:
    {
		EAInfo.algorType = EXHAUST;
		StartEA();
        break;
	}
    case EVOLUTIONARY_STRATEGY:
    {
        StartES();
        break;
    }
    }
}


void
CAPOT6Dlg::StopSearch()
{
    if (SearchThreadRunning)
    {
        SearchAbortThread = TRUE;
    }
}


void 
CAPOT6Dlg::StartEA()
{
    CProgressCtrl* progressBar = (CProgressCtrl*) GetDlgItem(IDC_PROGRESS);

    if (EAInfo.algorType == STDGA)
        progressBar->SetRange(0, EAInfo.generations * EAInfo.popSize);
    else if (EAInfo.algorType == ALPS)
        progressBar->SetRange(0, EAInfo.generations * EAInfo.layerSize);

    progressBar->SetStep(1);
    progressBar->SetPos(0);

    if (SearchThreadRunning)
    {
        // Stop the run.
        statusf(STOP_RUN);
        SearchAbortThread   = TRUE;
        return;
    }
    statusf(INIT_RUN);

    SearchThreadRunning = TRUE;
    SearchAbortThread   = FALSE;

    openGLControl.display_individual = NULL;
    openGLControl.Invalidate(TRUE);

#if NET_ENVIRON
    for (unsigned int ii = 0; ii < outputs.size(); ii++) delete outputs[ii];
#else
    for (ii = 0; ii < outputs(); ii++) delete outputs[ii];
#endif
    outputs.clear();

    UpdateButtonsForRun(FALSE);
    EAInfo.hWindow = this;
	
	CWinThread* pThread = AfxBeginThread(RunGA, &EAInfo);
}


void 
CAPOT6Dlg::StartSA()
{
    CProgressCtrl* progressBar = (CProgressCtrl*) GetDlgItem(IDC_PROGRESS);
    progressBar->SetRange(0, SAInfo.iterations);
    progressBar->SetStep(1);
    progressBar->SetPos(0);
    if (SearchThreadRunning)
    {
        // Stop the run.
        SearchAbortThread   = TRUE;
        return;
    }
    statusf(INIT_RUN);

    outputs.clear();

    SearchThreadRunning = TRUE;
    SearchAbortThread   = FALSE;

    openGLControl.display_individual = NULL;
    openGLControl.Invalidate(TRUE);

    UpdateButtonsForRun(FALSE);
    SAInfo.hWindow = this;
    CWinThread* pThread = AfxBeginThread(RunSA, &SAInfo);
}




void
CAPOT6Dlg::StartHC()
{
    CProgressCtrl* progressBar = (CProgressCtrl*) GetDlgItem(IDC_PROGRESS);
    progressBar->SetRange(0, HCInfo.iterations);
    progressBar->SetStep(1);
    progressBar->SetPos(0);
    if (SearchThreadRunning)
    {
        // Stop the run.
        SearchAbortThread   = TRUE;
        return;
    }
    statusf(INIT_RUN);

    outputs.clear();

    SearchThreadRunning = TRUE;
    SearchAbortThread   = FALSE;

    openGLControl.display_individual = NULL;
    openGLControl.Invalidate(TRUE);

    UpdateButtonsForRun(FALSE);
    HCInfo.hWindow = this;
    CWinThread* pThread = AfxBeginThread(RunHC, &HCInfo);

}

void 
CAPOT6Dlg::StartES()
{
    CProgressCtrl* progressBar = (CProgressCtrl*) GetDlgItem(IDC_PROGRESS);
    progressBar->SetRange(0, ESInfo.generations * ESInfo.mu);
    progressBar->SetStep(1);
    progressBar->SetPos(0);
    if (SearchThreadRunning)
    {
        // Stop the run.
        SearchAbortThread   = TRUE;
        return;
    }
    statusf(INIT_RUN);

    outputs.clear();

    SearchThreadRunning = TRUE;
    SearchAbortThread   = FALSE;

    openGLControl.display_individual = NULL;
    openGLControl.Invalidate(TRUE);

    UpdateButtonsForRun(FALSE);
    ESInfo.hWindow = this;
    CWinThread* pThread = AfxBeginThread(RunES, &ESInfo);
}


void 
CAPOT6Dlg::UpdateButtonsForRun(bool readyForRun)
{
    if (readyForRun)
        ((CProgressCtrl*) GetDlgItem(IDC_PROGRESS))->SetPos(0);

    GetDlgItem(IDC_BUTTON_RUN)->SetWindowText(readyForRun ? "Run" : "Stop");

    GetDlgItem(IDC_PLATFORM)->EnableWindow(readyForRun);
    GetDlgItem(IDC_BUTTON_ANTENNA)->EnableWindow(readyForRun);
    GetDlgItem(IDC_BUTTON_ANTENNA_REMOVE)->EnableWindow(readyForRun);
    GetDlgItem(IDC_TREE_ANTENNA)->EnableWindow(readyForRun);

    GetDlgItem(IDC_EDIT_MIN_THETA)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_MIN_PHI)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_MIN_FREQ)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_STEP_THETA)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_STEP_PHI)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_STEP_FREQ)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_INC_THETA)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_INC_PHI)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_INC_FREQ)->EnableWindow(readyForRun);
    GetDlgItem(IDC_BUTTON_NEC)->EnableWindow(readyForRun);

    GetDlgItem(IDC_CHECK_LOCAL)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(readyForRun);
    GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(readyForRun);
    GetDlgItem(IDC_LIST_ALGORITHMS)->EnableWindow(readyForRun);
}


void 
CAPOT6Dlg::OnDblclkListResults()
{
    CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_RESULTS);
    switch (last_algo_executed)
    {

    case EVOLUTIONARY_ALGORITHM:
    {
        //openGLControl.display_individual = (individual *)gBestOf[list->GetCurSel()];
        openGLControl.display_individual = outputs[list->GetCurSel()];
        openGLControl.CopyAntennaPointer(antennasEA);
        break;
    }

    case SIMULATED_ANNEALING:
    {
        openGLControl.display_individual = outputs[list->GetCurSel()];
        openGLControl.CopyAntennaPointer(antennasSA);
        break;
    }

    case HILL_CLIMBER:
    {
        openGLControl.display_individual = outputs[list->GetCurSel()];
        openGLControl.CopyAntennaPointer(antennasHC);
        break;
    }

    case EXHAUSTIVE_SEARCH:
    {
        openGLControl.display_individual = outputs[list->GetCurSel()];
        openGLControl.CopyAntennaPointer(antennasEA);
        break;
    }

    case EVOLUTIONARY_STRATEGY:
    {
        openGLControl.display_individual = outputs[list->GetCurSel()];
        openGLControl.CopyAntennaPointer(antennasES);
        break;
    }

    openGLControl.Invalidate(TRUE);
    }
}



/*=============================================
Genetic Algorithm Controller
=============================================*/
UINT 
RunGA(LPVOID pParam)
{
    EAParams*	pEA = (EAParams*) pParam;
	time_t start, end;
	float best_configuration_found = (std::numeric_limits<float>::max) ();
	CString best_dlg_string;
    // make a copy of the antenna list
    for (unsigned ii = 0; ii < antennasEA.size(); ii++)
    {
        delete antennasEA[ii];
    }
    antennasEA.clear();
#if NET_ENVIRON
    for (unsigned ii = 0; ii < antennas.size(); ii++)
    {
#else
    for (ii = 0; ii < antennas.size(); ii++)
    {
#endif
        antennasEA.push_back(new antenna(*(antennas[ii])));
    }

    assert(antennas.size() == antennasEA.size());
    // Set up the random number generator.
    if (EAInfo.autoSeed)
    {
        EAInfo.seed = (unsigned) time(NULL);
    }
    srand(EAInfo.seed);
    LOCAL_NEC	simulator;
    simulator.Initialize();
    // analysis test antennas to get ideal radiation patterns
    char buffer[100];
#if NET_ENVIRON
    for (unsigned ii = 0; ii < antennasEA.size(); ii++)
    {
#else
    for (ii = 0; ii < antennasEA.size(); ii++)
    {
#endif
        // SEC - Clarify this piece of code: Are we evaluating the antennas on an infinite ground plane?
		antennasEA[ii]->calculate_centroid();
		antennasEA[ii]->calculate_dist_from_centroid();
        antennasEA[ii]->analyze();
		sprintf(buffer,"ANT%02d.nec", ii);
        print_nec_deck(buffer);
        simulator.Run(ii);
        simulator.Retrieve(0);
        sprintf(buffer,"ANT%02d.nou", ii);
        read_nec_results(buffer, antennasEA[ii]->results, *(antennasEA[ii]));
    }
    if (RunLocal)
        Simulator = new LOCAL_NEC;
    else
        Simulator = new GLOBAL_NEC;
    Simulator->Initialize();

    population *deme;

    if (EAInfo.algorType == STDGA)
        deme = new sim_population;
    else if (EAInfo.algorType == EXHAUST)
    {
        deme = new exhaustive_search();
        pEA->generations = 1;
    }
    else if (EAInfo.algorType == ALPS)
    {
        deme = new alps_population;
        pEA->popSize = pEA->layerSize;
    }


    deme->initialize(pEA->popSize);
	
	start = time(NULL);

	/* GA main generation loop */
    for (unsigned int jj = 0; jj < pEA->generations; jj++)
    {
		statusf(EVAL_GEN, jj);

        deme->evaluate(jj);
        if (SearchAbortThread) break;

        deme->sort_by_fitness();
    
		antenna_configuration* best = deme->get_best();
		best = best->clone(true);
		outputs.push_back(best);
		

		if (best->fitness < best_configuration_found)
		{
			best_configuration_found = best->fitness;
			best_dlg_string.Format("%5.2f", best_configuration_found);
			pEA->hWindow->GetDlgItem(IDC_EDIT_BEST_FITNESS)->SetWindowText(best_dlg_string);
		}

        /*tbll*/
        //best->build_antenna(antennasEA);
        //outputs.push_back(best);


        if (SearchAbortThread) break;
        pEA->hWindow->SendMessage(WM_USER_UPDATE_GA, jj, 0);
        pEA->hWindow->GetDlgItem(IDC_PROGRESS)->SetWindowText(buffer);


        if (jj+1 < pEA->generations) deme->select();

		end = time(NULL);
		pEA->hWindow->SendMessage(WM_COMPLETE, jj, end - start);
    }
    delete Simulator;
    delete deme;
    statusf(EVAL_DONE);

    if (!SearchAbortThread)
        pEA->hWindow->SendMessage(WM_USER_UPDATE_GA, END_OF_RUN, 0);
    else
        pEA->hWindow->SendMessage(WM_USER_UPDATE_GA, ABORTED_RUN, 0);

	//pEA->hWindow->SendMessage(WM_COMPLETE, 0, 0);
    SearchThreadRunning = FALSE;
    return 0;
}


/*=============================================
Genetic Algorithm Updater
=============================================*/
LRESULT 
CAPOT6Dlg::OnUpdateGA(WPARAM wParam,
					LPARAM lParam)
{
    if (wParam == END_OF_RUN || wParam == ABORTED_RUN)
    {
        UpdateButtonsForRun(true);
        SearchThreadRunning = FALSE;
    }
    else
    {
        CString buffer;
        int w = outputs.size() - 1;
        buffer.Format("G = %04d   F = %5.2f", wParam, outputs[w]->fitness);
        CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_RESULTS);
        list->AddString(buffer);
        int idx = list->GetCount() - 1;
        list->SetItemData(idx, (DWORD_PTR) outputs[w]);
        buffer.Format("best%04d.nec", wParam);
        /*tbll*/
        outputs[w]->write(buffer, 0);
        if (list->GetCount() > LIST_RESULTS_MAX)
        {
            list->DeleteString(0);
            delete outputs[0];
            outputs.erase(outputs.begin());
        }

        CProgressCtrl* progressBar = (CProgressCtrl*) GetDlgItem(IDC_PROGRESS);
        progressBar->SetPos((wParam+1) * EAInfo.popSize);
    }
    return 0;
}


/*=============================================
Simulated Annealing Controller
=============================================*/
UINT 
RunSA(LPVOID pParam)
{
    SAParams* pSA = (SAParams*) pParam;
	time_t start, end;
	float best_configuration_found = (std::numeric_limits<float>::max) ();
	CString best_dlg_string;
    int num_iterations_with_constant_fitness = 0 , swap_counter = 0, print_output, j, ii;
    float delta_fitness, prob, previous_parent_fitness, mut_prob = SAInfo.mutationProbability, temperature = SAInfo.initialTemp, convergedIterations = 0;
	antenna_configuration *SAOutput, *swap_pointer;

    for(ii=0; ii<antennasSA.size(); ii++)
    {
        delete antennasSA[ii];
    }

    convergedIterations = (int)(SAInfo.iterations * SAInfo.convergenceFactor);
    antennasSA.clear();

#if NET_ENVIRON
    for (ii = 0; ii < antennas.size(); ii++)
    {
#else
    for (ii = 0; ii < antennas.size(); ii++)
    {
#endif
        antennasSA.push_back(new antenna(*(antennas[ii])));
    }

	if (SAInfo.autoSeed)
        SAInfo.seed = (unsigned) time(NULL);

    srand(SAInfo.seed);

    LOCAL_NEC simulator;
    simulator.Initialize();

    char buffer[100];
#if NET_ENVIRON
    for (ii = 0; ii < antennasSA.size(); ii++)
    {
#else
    for (ii = 0; ii < antennasSA.size(); ii++)
    {
#endif

		antennasSA[ii]->calculate_centroid();
		antennasSA[ii]->calculate_dist_from_centroid();
        antennasSA[ii]->analyze();
        sprintf(buffer,"ANT%02d.nec", ii);
        print_nec_deck(buffer);
        simulator.Run(ii);
        simulator.Retrieve(0);
        sprintf(buffer,"ANT%02d.nou", ii);
        read_nec_results(buffer, antennasSA[ii]->results, *(antennasSA[ii]));
    }

    if (RunLocal)
        Simulator = new LOCAL_NEC;
    else
        Simulator = new GLOBAL_NEC;

    Simulator->Initialize();
    antenna_configuration* parent = new antenna_configuration;
    antenna_configuration* child = new antenna_configuration;
	

    sprintf(buffer, "C:\\cygwin\\bin\\mkdir ./Iterations");
    execute(buffer);

    /*initializes values to be fed into the .nec files */
	start = time(NULL);
    parent->initialize(antennasSA);

    /* creates the nec decks */
    parent->setup(0);

    /*create .nou files */
    statusf(START_SIM);
    parent->simulate(Simulator,0);
    parent->evaluate(0,antennasSA, SAInfo.expWeight);

	/* save the initialization configuration of SA */
	end = time(NULL);
	SAOutput = (antenna_configuration*) parent->clone(true);
	outputs.push_back(SAOutput);
	pSA->hWindow->SendMessage(WM_USER_UPDATE_SA, swap_counter, 0);
	pSA->hWindow->SendMessage(WM_COMPLETE, 0, end - start);


	if (parent->fitness < best_configuration_found)
	{
		best_configuration_found = parent->fitness;
		best_dlg_string.Format("%5.2f", best_configuration_found);
		pSA->hWindow->GetDlgItem(IDC_EDIT_BEST_FITNESS)->SetWindowTextA(best_dlg_string);
	}


	/* main SA loop */
    for (ii = 1; ii <= pSA->iterations; ii++)
    {
        child = parent->clone(false);
		print_output = !(ii % SAInfo.outputFreq);
        previous_parent_fitness = parent->fitness;
		
		if (SAInfo.mutationType == SIMPLE_MUTATION)
			child = child->simple_mutation(mut_prob, antennasSA);
		else if (SAInfo.mutationType == GLOBAL_NEIGHBOURHOOD_MUTATION)
			child = child->global_neighbourhood_mutation(mut_prob, antennasSA);
		else if (SAInfo.mutationType == LOCAL_NEIGHBOURHOOD_MUTATION)
			child = child->local_neighbourhood_mutation(mut_prob, antennasSA);
		
		for (j = 0; j < child->ants.size(); j++)
		{
			if (!parent->check_overlap(child->ants[j]->points->at(0)) )
				break;
		}

		if (j != child->ants.size())
		{	
			child->setup(ii);

			statusf(START_SIM);
			child->simulate(Simulator, ii);

			statusf(EVAL_SA_ITER,ii);
			child->evaluate(ii, antennasSA, SAInfo.expWeight);


			if ( child->fitness < parent->fitness)
			{
				//TestSA(ii, parent->fitness, child->fitness, parent->fitness - child->fitness, "IMPROVE", mut_prob);
				swap_pointer = parent;
				parent = child;
				child = swap_pointer;
				swap_counter++;

			
				/* for collecting results */ 
				end = time(NULL);
				SAOutput = (antenna_configuration*) parent->clone(true);
		        outputs.push_back(SAOutput);
				pSA->hWindow->SendMessage(WM_COMPLETE, ii, end - start);
			    pSA->hWindow->SendMessage(WM_USER_UPDATE_SA, swap_counter, ii);
			
				if (parent->fitness < best_configuration_found)
				{
					best_configuration_found = parent->fitness;
					best_dlg_string.Format("%5.2f", best_configuration_found);
					pSA->hWindow->GetDlgItem(IDC_EDIT_BEST_FITNESS)->SetWindowText(best_dlg_string);
				}
			 }

			else if (child->fitness > parent->fitness )
			{
				delta_fitness = ((child->fitness - parent->fitness) / 100);
				prob = exp((-1.0 * delta_fitness)/temperature);

				if(SA::flip(prob))
				{
					//TestSA(ii, parent->fitness, child->fitness, child->fitness - parent->fitness, "*", mut_prob);

					swap_pointer = parent;
					parent = child;
					child = swap_pointer;
					swap_counter++;
					
					/* for collecting results */ 
					end = time(NULL);
				
					SAOutput = (antenna_configuration*) parent->clone(true);
					outputs.push_back(SAOutput);
					pSA->hWindow->SendMessage(WM_USER_UPDATE_SA, swap_counter, ii);
				
					/* writes the configuration to a file */
					pSA->hWindow->SendMessage(WM_COMPLETE, ii, end - start);
                
				}
				else;
					//TestSA(ii, parent->fitness, child->fitness, child->fitness - parent->fitness, "KP", mut_prob);
			}

			else;
				//TestSA(ii, parent->fitness, child->fitness, child->fitness - parent->fitness, "=", mut_prob);
		}
		else;
			//TestSA(ii, parent->fitness, child->fitness, child->fitness - parent->fitness, "NM", mut_prob);
#if 0
        if (print_output)
        {
			
            antenna_configuration* SAoutput = (antenna_configuration*) parent->clone(true);
            /*tbll*/
            SAoutput->build_antenna(antennasSA);
            outputs.push_back(SAoutput);
            pSA->hWindow->SendMessage(WM_USER_UPDATE_SA, swap_counter, ii);
        }
#endif

        if(!(ii % SAInfo.iterationsPerTempChng))
        {
            temperature = temperature * SAInfo.tempFactor;
        }

        if (parent->fitness == previous_parent_fitness)
            num_iterations_with_constant_fitness++;
        else
            num_iterations_with_constant_fitness = 0;

        if (num_iterations_with_constant_fitness > convergedIterations)
        {
			if (mut_prob < 1.0f)
				mut_prob = 1.1 * mut_prob;
			num_iterations_with_constant_fitness = 0;
        }

		delete child;
    }

    delete Simulator;
    /*tbll*/
    delete parent;
    statusf(EVAL_DONE);


    if (!SearchAbortThread)
        pSA->hWindow->SendMessage(WM_USER_UPDATE_SA, END_OF_RUN, 0);
    else
        pSA->hWindow->SendMessage(WM_USER_UPDATE_SA, ABORTED_RUN, 0);
    SearchThreadRunning = FALSE;

	pSA->hWindow->SendMessage(WM_COMPLETE, 0, 0);
    return 0;
}


void 
TestSA(int iteration,
				float parent_fitness,
				float child_fitness,
				float delta,
				char *message, float mutation)
{
	char buffer[100];
	char *filename = "sa_test_2.txt";
    FILE* fp = fopen(filename, "a");
    if (fp != NULL)
    {
			fprintf(fp, "%d\t %f\t %f\t %f\t %s\t %f\n",
				iteration, parent_fitness, child_fitness, delta, message, mutation);
	}
        fclose(fp);
        sprintf(buffer, "C:\\cygwin\\bin\\dos2unix %s", filename);
        execute(buffer);
}


/*=============================================
Simulated Annealing Updater
=============================================*/
afx_msg 
LRESULT 
CAPOT6Dlg::OnUpdateSA(WPARAM wParam,	
					LPARAM lParam)
{
    if (wParam == END_OF_RUN || wParam == ABORTED_RUN)
    {
        UpdateButtonsForRun(true);
        SearchThreadRunning = FALSE;
    }
    else
    {
        CString buffer;
        //buffer.Format("F=%5.2f",outputs.back()->fitness);
        //buffer.Format("Swap=%04d, Iter=%06d, F = %5.2f", wParam, lParam, outputs.back()->fitness);
        buffer.Format("I = %06d, F = %5.2f", lParam, outputs.back()->fitness);
        CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_RESULTS);
        list->AddString(buffer);
        int idx = list->GetCount()-1;
        list->SetItemData(idx, (DWORD_PTR) outputs.back());
        buffer.Format("best%04d.nec", wParam);
        /*tbll*/
        outputs.back()->write(buffer, 0);

        CProgressCtrl* progressBar = (CProgressCtrl*) GetDlgItem(IDC_PROGRESS);
        progressBar->SetPos(lParam);
	}
    return 0;
}



/*=============================================
Hill Climber Controller
=============================================*/
UINT 
RunHC(LPVOID pParam)
{
    HCParams* pHC = (HCParams*) pParam;
	time_t start, end;
	float best_configuration_found = (std::numeric_limits<float>::max) ();
	CString best_dlg_string;
    int num_iterations_with_constant_fitness = 0 , swap_counter = 0, print_output, j;
    float delta_fitness, prob, previous_parent_fitness, mut_prob = HCInfo.mutationProbability, convergedIterations = 0;
	antenna_configuration *HCOutput, *swap_pointer;

    for(unsigned ii=0; ii<antennasHC.size(); ii++)
    {
        delete antennasHC[ii];
    }

    convergedIterations = (int)(HCInfo.iterations*HCInfo.convergenceFactor);
    antennasHC.clear();

#if NET_ENVIRON
    for (unsigned int ii = 0; ii < antennas.size(); ii++)
    {
#else
    for (ii = 0; ii < antennas.size(); ii++)
    {
#endif
        antennasHC.push_back(new antenna(*(antennas[ii])));
    }

    if (HCInfo.autoSeed)
        HCInfo.seed = (unsigned) time(NULL);
    srand(HCInfo.seed);

    LOCAL_NEC simulator;
    simulator.Initialize();

    char buffer[100];
    for (unsigned int ii = 0; ii < antennasHC.size(); ii++)
    {
		antennasHC[ii]->calculate_centroid();
		antennasHC[ii]->calculate_dist_from_centroid();
        antennasHC[ii]->analyze();
        sprintf(buffer,"ANT%02d.nec", ii);
        print_nec_deck(buffer);
        simulator.Run(ii);
        simulator.Retrieve(0);
        sprintf(buffer,"ANT%02d.nou", ii);
        read_nec_results(buffer, antennasHC[ii]->results, *(antennasHC[ii]));
    }

    if (RunLocal)
        Simulator = new LOCAL_NEC;
    else
        Simulator = new GLOBAL_NEC;

    Simulator->Initialize();
    antenna_configuration* parent = new antenna_configuration;
    antenna_configuration* child = new antenna_configuration;


    sprintf(buffer, "C:\\cygwin\\bin\\mkdir ./Iterations");
    execute(buffer);

    /*initializes values to be fed into the .nec files */
	start = time(NULL);
    parent->initialize(antennasHC);

    /* creates the nec decks */
    parent->setup(0);

    /*create .nou files */
    statusf(START_SIM);
    parent->simulate(Simulator,0);
    //statusf(EVAL_SA_ITER,0);
    parent->evaluate(0,antennasHC, HCInfo.expWeight);

	/*save the initilization confifuration of HC */
	end = time(NULL);
	HCOutput = (antenna_configuration*) parent->clone(true);
	outputs.push_back(HCOutput);
	pHC->hWindow->SendMessage(WM_USER_UPDATE_HC, swap_counter, 0);
	pHC->hWindow->SendMessage(WM_COMPLETE, 0, end - start);

	
	if (parent->fitness < best_configuration_found)
	{
		best_configuration_found = parent->fitness;
		best_dlg_string.Format("%5.2f", best_configuration_found);
		pHC->hWindow->GetDlgItem(IDC_EDIT_BEST_FITNESS)->SetWindowTextA(best_dlg_string);
	}

	/* main HC loop */
    for (unsigned int ii = 1; ii <= pHC->iterations; ii++)
    {
		child = parent->clone(false);
        print_output = !(ii % HCInfo.outputFreq);
        previous_parent_fitness = parent->fitness;

		if (HCInfo.mutationType == SIMPLE_MUTATION)
			child = child->simple_mutation(mut_prob, antennasHC);
		else if (HCInfo.mutationType == GLOBAL_NEIGHBOURHOOD_MUTATION)
			child = child->global_neighbourhood_mutation(mut_prob, antennasHC);
		else if (HCInfo.mutationType == LOCAL_NEIGHBOURHOOD_MUTATION)
			child = child->local_neighbourhood_mutation(mut_prob, antennasHC);

		for (j = 0; j < child->ants.size(); j++)
		{
			if ( !parent->check_overlap(child->ants[j]->points->at(0)) )
				break;
		}

		if (j != child->ants.size()) 
		{
		
			child->setup(ii);

			statusf(START_SIM);
			child->simulate(Simulator,ii);

			 statusf(EVAL_HC_ITER,ii);
	        child->evaluate(ii,antennasHC, HCInfo.expWeight);

#if 0
			if (print_output)
			{
				antenna_configuration* HCoutput = (antenna_configuration*) parent->clone(true);
				 /*tbll*/
				//HCoutput->build_antenna(antennasHC);
				outputs.push_back(HCoutput);
				pHC->hWindow->SendMessage(WM_USER_UPDATE_HC, swap_counter, ii);
			}
#endif
			if ( child->fitness < parent->fitness)
			{
				swap_pointer = parent;
				parent = child;
				child = swap_pointer;
				swap_counter++;

				end = time(NULL);
				HCOutput = (antenna_configuration*) parent->clone(true);
				outputs.push_back(HCOutput);
				pHC->hWindow->SendMessage(WM_COMPLETE, ii, end - start);
				pHC->hWindow->SendMessage(WM_USER_UPDATE_HC, swap_counter, ii);

				if (parent->fitness < best_configuration_found)
				{
					best_configuration_found = parent->fitness;
					best_dlg_string.Format("%5.2f", best_configuration_found);
					pHC->hWindow->GetDlgItem(IDC_EDIT_BEST_FITNESS)->SetWindowTextA(best_dlg_string);
				}
			}

		}

        if (parent->fitness == previous_parent_fitness)
            num_iterations_with_constant_fitness++;
        else
            num_iterations_with_constant_fitness = 0;

        if (num_iterations_with_constant_fitness > convergedIterations)
            mut_prob = 1.5 * mut_prob;
    }

    delete Simulator;
    statusf(EVAL_DONE);

    if (!SearchAbortThread)
        pHC->hWindow->SendMessage(WM_USER_UPDATE_HC, END_OF_RUN, 0);
    else
        pHC->hWindow->SendMessage(WM_USER_UPDATE_HC, ABORTED_RUN, 0);
	pHC->hWindow->SendMessage(WM_COMPLETE, 0, 0);
    SearchThreadRunning = FALSE;
    return 0;
}


/*=============================================
Hill Climber Updater
=============================================*/
afx_msg 
LRESULT
CAPOT6Dlg::OnUpdateHC(WPARAM wParam, 
					LPARAM lParam)
{
    if (wParam == END_OF_RUN || wParam == ABORTED_RUN)
    {
        UpdateButtonsForRun(true);
        SearchThreadRunning = FALSE;
    }
    else
    {
        CString buffer;
        buffer.Format("I = %06d, F = %5.2f", lParam, outputs.back()->fitness);
        //buffer.Format("Swap=%04d, Iter=%06d, F = %5.2f", wParam, lParam, outputs.back()->fitness);
        CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_RESULTS);
        list->AddString(buffer);
        int idx = list->GetCount()-1;
        list->SetItemData(idx, (DWORD_PTR) outputs.back());
        buffer.Format("best%04d.nec", wParam);
        /*tbll*/
        outputs.back()->write(buffer, 0);

        CProgressCtrl* progressBar = (CProgressCtrl*) GetDlgItem(IDC_PROGRESS);
        progressBar->SetPos(lParam);
    }

    return 0;
}


/*=============================================
Evolutionary Strategy Controller
=============================================*/
UINT 
RunES(LPVOID pParam)
{
    ESParams* pES = (ESParams*) pParam;
	time_t start, end;
	float best_configuration_found = (std::numeric_limits<float>::max) ();
	CString best_dlg_string;
    // make a copy of the antenna list
    for (unsigned ii = 0; ii < antennasES.size(); ii++)
	    delete antennasES[ii];
    antennasES.clear();

    for (unsigned ii = 0; ii < antennas.size(); ii++)
        antennasES.push_back(new antenna(*(antennas[ii])));
    
    assert(antennas.size() == antennasES.size());
    
	// Set up the random number generator.
    if (ESInfo.autoSeed)
        ESInfo.seed = (unsigned) time(NULL);
    
	srand(ESInfo.seed);

    LOCAL_NEC	simulator;
    simulator.Initialize();
    // analysis test antennas to get ideal radiation patterns
    char buffer[100];
    for (unsigned ii = 0; ii < antennasES.size(); ii++)
    {
		antennasES[ii]->calculate_centroid();
		antennasES[ii]->calculate_dist_from_centroid();
        antennasES[ii]->analyze();
        sprintf(buffer,"ANT%02d.nec", ii);
        print_nec_deck(buffer);
        // do local analysis and wait for results
        simulator.Run(ii);
        simulator.Retrieve(0);
        sprintf(buffer,"ANT%02d.nou", ii);
        read_nec_results(buffer, antennasES[ii]->results, *(antennasES[ii]));
    }
    if (RunLocal)
        Simulator = new LOCAL_NEC;
    else
        Simulator = new GLOBAL_NEC;
    Simulator->Initialize();

    evolutionary_strategy *deme = new evolutionary_strategy();

    deme->initialize(pES->mu);
	start = time(NULL);

	/* main ES loop */
    for (unsigned int j = 0; j < pES->generations; j++)
    {
        statusf(EVAL_GEN, j);

		deme->mutate(pES->mutationType);

        deme->evaluate(j);
        if (SearchAbortThread) break;

        deme->sort_by_fitness();
        antenna_configuration* best = deme->get_best();
		best = best->clone(true);
		outputs.push_back(best);
        deme->survivor_selection();

		if (best->fitness < best_configuration_found)
		{
			best_configuration_found = best->fitness;
			best_dlg_string.Format("%5.2f", best_configuration_found);
			pES->hWindow->GetDlgItem(IDC_EDIT_BEST_FITNESS)->SetWindowText(best_dlg_string);
		}

        if (SearchAbortThread) break;
        pES->hWindow->SendMessage(WM_USER_UPDATE_ES, j, 0);
        pES->hWindow->GetDlgItem(IDC_PROGRESS)->SetWindowText(buffer);
		end = time(NULL);
		pES->hWindow->SendMessage(WM_COMPLETE, j, end - start);

    }
    delete Simulator;
    delete deme;
    statusf(EVAL_DONE);

    if (!SearchAbortThread)
        pES->hWindow->SendMessage(WM_USER_UPDATE_ES, END_OF_RUN, 0);
    else
        pES->hWindow->SendMessage(WM_USER_UPDATE_ES, ABORTED_RUN, 0);

	pES->hWindow->SendMessage(WM_COMPLETE, 0, 0);
    SearchThreadRunning = FALSE;
    return 0;
}

/*=============================================
Evolutionary Strategy Updater
=============================================*/
afx_msg 
LRESULT 
CAPOT6Dlg::OnUpdateES(WPARAM wParam, 
					LPARAM lParam)
{
    if (wParam == END_OF_RUN || wParam == ABORTED_RUN)
    {
        UpdateButtonsForRun(true);
        SearchThreadRunning = FALSE;
    }
    else
    {
        CString buffer;
        int w = outputs.size() - 1;
        buffer.Format("G = %04d   F = %5.2f", wParam, outputs[w]->fitness);
        CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_RESULTS);
        list->AddString(buffer);
        int idx = list->GetCount() - 1;
        list->SetItemData(idx, (DWORD_PTR) outputs[w]);
        buffer.Format("best%04d.nec", wParam);
        /*tbll*/
        outputs[w]->write(buffer, 0);
        if (list->GetCount() > LIST_RESULTS_MAX)
        {
            list->DeleteString(0);
            delete outputs[0];
            outputs.erase(outputs.begin());
        }

        CProgressCtrl* progressBar = (CProgressCtrl*) GetDlgItem(IDC_PROGRESS);
        progressBar->SetPos((wParam+1) * ESInfo.mu);
    }
    return 0;
}

void
CAPOT6Dlg::SetLocalRun(bool b)
{
    RunLocal = b;
}

void
CAPOT6Dlg::OnCheckLocal()
{

    RunLocal = (((CButton*) GetDlgItem(IDC_CHECK_LOCAL))->GetCheck() == BST_CHECKED);
    //AJ
#if 0
    if (selected_algorithm == EVOLUTIONARY_ALGORITHM)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || GAparams_valid()) && antennas_valid());
    else if (selected_algorithm == SIMULATED_ANNEALING)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || SAparams_valid()) && antennas_valid());
    else if (selected_algorithm == HILL_CLIMBER)
        GetDlgItem(IDC_BUTTON_RUN)->EnableWindow((RunLocal || HCparams_valid()) && antennas_valid());

    ((CButton*) GetDlgItem(IDC_CHECK_LOCAL))->SetCheck(RunLocal ? BST_CHECKED : BST_UNCHECKED);
#endif

    //GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(RunLocal);
    //GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(RunLocal);
    UpdateGUI();
}



void
CAPOT6Dlg::OnClose()
{
    SearchAbortThread = TRUE;
    while (SearchThreadRunning) ::Sleep(1000);

    CDialog::OnClose();
}

/*
void CAPOT6Dlg::OnBnClickedButtonEdit()
{
	eaDialog.ReadValues();
    eaDialog.UpdateValues();
	eaDialog.ShowWindow(SW_SHOW);
	eaDialog.BringWindowToTop();
}
*/
void 
CAPOT6Dlg::SetStatus(char *msg)
{
    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(msg);
}


// Print a message to the status bar using a printf format string and args.
int 
statusf(char *format, ...)
{
    static va_list argp;
    static char output[255];
    int result;
    va_start(argp, format);
    result = vsprintf(output, format, argp);
    va_end(argp);
    dialogInstance->SetStatus(output);
    return result;
}

void 
CAPOT6Dlg::OnBnClickedButtonLua()
{
    CWinThread* pThread = AfxBeginThread(RunCLI, this, THREAD_PRIORITY_LOWEST);
}

void 
CAPOT6Dlg::OnBnClickedButtonX()
{
    openGLControl.SetViewAxis(quaternion(1, Y_AXIS));
}

void 
CAPOT6Dlg::OnBnClickedButtonXNeg()
{
    openGLControl.SetViewAxis(quaternion(-1, Y_AXIS));
}

void 
CAPOT6Dlg::OnBnClickedButtonY()
{
    openGLControl.SetViewAxis(quaternion(-1,X_AXIS));
}

void 
CAPOT6Dlg::OnBnClickedButtonYNeg()
{
    openGLControl.SetViewAxis(quaternion(1,X_AXIS));
}

void 
CAPOT6Dlg::OnBnClickedButtonZ()
{
    openGLControl.SetViewAxis(quaternion(cos(0.5f*G_PI),Y_AXIS));
}

void 
CAPOT6Dlg::OnBnClickedButtonZNeg()
{
    openGLControl.SetViewAxis(unitquaternion(0, X_AXIS));
}

void 
CAPOT6Dlg::OnBnClickedCheckOrtho()
{
    openGLControl.TogglePerspective();
    openGLControl.SetFocus();
}

void 
CAPOT6Dlg::OnBnClickedButtonEditNec()
{
    if (platform != NULL)
    {
        necDialog.UpdateParams();
        necDialog.ShowWindow(SW_SHOW);
        necDialog.BringWindowToTop();
    }
}

void 
CAPOT6Dlg::OnBnClickedButtonAbout()
{
    CSplashDialog::ShowSplashScreen(NULL);
}

BOOL
CAPOT6Dlg::PreTranslateMessage(MSG* pMsg)
{
    if (CSplashDialog::PreTranslateAppMessage(pMsg)) return TRUE;

    return CDialog::PreTranslateMessage(pMsg);
}

void 
CAPOT6Dlg::OnBnClickedButtonHelp()
{
    helpDialog.Create(IDD_HELP_DIALOG, this);
    helpDialog.ShowWindow(SW_SHOW);
    helpDialog.BringWindowToTop();
}

void 
CAPOT6Dlg::OnNMDblclkTreeAntenna(NMHDR *pNMHDR,
								LRESULT *pResult)
{
    openGLControl.display_individual = NULL;
    openGLControl.Invalidate(TRUE);
    *pResult = 0;
}

void 
CAPOT6Dlg::OnTvnSelchangedTreeAntenna(NMHDR *pNMHDR,
										LRESULT *pResult)
{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    openGLControl.display_individual = NULL;

    HTREEITEM hItem = treeControl.GetSelectedItem();
    if (hItem != NULL)
    {
        HTREEITEM parent;
        while ((parent = treeControl.GetParentItem(hItem)) != NULL) hItem = parent;
        antenna* ant = (antenna*) treeControl.GetItemData(hItem);

        openGLControl.display_antenna = ant;
        CString buffer;
        buffer.Format("%d", ant->params.min_theta);
        ((CEdit*) GetDlgItem(IDC_EDIT_MIN_THETA))->SetWindowText(buffer);
        buffer.Format("%d", ant->params.step_theta);
        ((CEdit*) GetDlgItem(IDC_EDIT_STEP_THETA))->SetWindowText(buffer);
        buffer.Format("%d", ant->params.incr_theta);
        ((CEdit*) GetDlgItem(IDC_EDIT_INC_THETA))->SetWindowText(buffer);
        buffer.Format("%d", ant->params.min_phi);
        ((CEdit*) GetDlgItem(IDC_EDIT_MIN_PHI))->SetWindowText(buffer);
        buffer.Format("%d", ant->params.step_phi);
        ((CEdit*) GetDlgItem(IDC_EDIT_STEP_PHI))->SetWindowText(buffer);
        buffer.Format("%d", ant->params.incr_phi);
        ((CEdit*) GetDlgItem(IDC_EDIT_INC_PHI))->SetWindowText(buffer);
        buffer.Format("%f", ant->params.min_freq);
        ((CEdit*) GetDlgItem(IDC_EDIT_MIN_FREQ))->SetWindowText(buffer);
        buffer.Format("%d", ant->params.step_freq);
        ((CEdit*) GetDlgItem(IDC_EDIT_STEP_FREQ))->SetWindowText(buffer);
        buffer.Format("%d", ant->params.incr_freq);
        ((CEdit*) GetDlgItem(IDC_EDIT_INC_FREQ))->SetWindowText(buffer);

        GetDlgItem(IDC_BUTTON_ANTENNA_REMOVE)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_BUTTON_ANTENNA_REMOVE)->EnableWindow(FALSE);
        openGLControl.display_antenna = NULL;
    }

    openGLControl.Invalidate(TRUE);

    thetaControl.Invalidate(TRUE);
    phiControl.Invalidate(TRUE);
    thetaControl.UpdateWindow();
    phiControl.UpdateWindow();

    *pResult = 0;
}


void 
CAPOT6Dlg::OnDblclkListAlgorithms()
{
    // TODO: Add your control notification handler code here
    CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_ALGORITHMS);
    switch(list->GetCurSel())
    {
		 case EVOLUTIONARY_ALGORITHM:
		 {
			eaDialog.ReadValues();
			eaDialog.UpdateValues();
			eaDialog.ShowWindow(SW_SHOW);
			eaDialog.BringWindowToTop();
			break;
		}
		case SIMULATED_ANNEALING:
		{
			saDialog.ReadValues();
			saDialog.UpdateValues();
			saDialog.ShowWindow(SW_SHOW);
			saDialog.BringWindowToTop();
			break;
		}

		case HILL_CLIMBER:
		{
			hcDialog.ReadValues();
			hcDialog.UpdateValues();
			hcDialog.ShowWindow(SW_SHOW);
			hcDialog.BringWindowToTop();
			break;
		}

		case EVOLUTIONARY_STRATEGY:
		{
			esDialog.ReadValues();
			esDialog.UpdateValues();
			esDialog.ShowWindow(SW_SHOW);
			esDialog.BringWindowToTop();
			break;
		}
	}
}


void 
CAPOT6Dlg::OnSelchangeListAlgorithms()
{
    CListBox* list = (CListBox*) GetDlgItem(IDC_LIST_ALGORITHMS);
    selected_algorithm = list->GetCurSel();
}

void 
CAPOT6Dlg::SetAlgorithm(unsigned int algo_type)
{
    selected_algorithm = algo_type;
}


/* prints the antenna configuration for every entry in the vector<optputs> */
afx_msg 
LRESULT 
CAPOT6Dlg::OnComplete(WPARAM wParam,	
					LPARAM lParam)
{
	char buffer[100];
	char* filename = "sa_test_3_rp_run_1.txt";
    FILE* fp = fopen(filename, "a");
    if (fp != NULL)
    {
		for (int j = 0; j < outputs.back()->ants.size(); j++)
			fprintf(fp, "Antenna %s\n Position: x = %f\t y = %f\t z = %f\n", 
				outputs.back()->ants.at(j)->label,
				outputs.back()->ants.at(j)->points->at(0)->x,
				outputs.back()->ants.at(j)->points->at(0)->y,
				outputs.back()->ants.at(j)->points->at(0)->z);
			fprintf(fp, "Fitness = %f\t Generation = %d \t Time Elapsed: %ld\n\n", outputs.back()->fitness, wParam, lParam);
        }
        fclose(fp);
        sprintf(buffer, "C:\\cygwin\\bin\\dos2unix %s", filename);
        execute(buffer);
	return 0;
}