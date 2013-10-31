// GA_APOTDlg.h : header file
//

#pragma once
#include "OpenGLView.h"
#include "AngleCtrl.h"
#include "NECDialog.h"
#include "NECEditor.h"
#include "HelpDialog.h"
#include "afxcmn.h"
#include "EADlg.h"
#include "SADlg.h"
#include "HCDlg.h"
#include "ESDlg.h"

// CAPOT6Dlg dialog
class CAPOT6Dlg : public CDialog
{
// Construction
public:
    CAPOT6Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
    enum { IDD = IDD_APOT6_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    HICON		m_hIcon;
    COpenGLView openGLControl;
    CAngleCtrl	thetaControl;
    CAngleCtrl	phiControl;
    CRect		m_prevRect;
    EADlg       eaDialog;
    SADlg		saDialog;
    HCDlg		hcDialog;
    ESDlg		esDialog;
    CHelpDialog	helpDialog;
    CNECDialog	necDialog;
    CNECEditor	editDialog;
//	vector<antenna*> antennas;
    virtual BOOL OnInitDialog();

protected:
    // Generated message map functions
    //{{AFX_MSG(CAPOT6Dlg)
    afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedPlatform();
    afx_msg void OnBnClickedButtonAntenna();
    afx_msg void OnEnChangeEditMinTheta();
    afx_msg void OnBnClickedButtonAntennaRemove();
    afx_msg void OnEnChangeEditStepTheta();
    afx_msg void OnEnChangeEditMinPhi();
    afx_msg void OnEnChangeEditStepPhi();
    afx_msg void OnEnChangeEditMinFreq();
    afx_msg void OnEnChangeEditStepFreq();
    afx_msg void OnEnChangeEditIncPhi();
    afx_msg void OnEnChangeEditIncTheta();
    afx_msg void OnEnChangeEditIncFreq();
    afx_msg void OnBnClickedButtonNec();
    afx_msg LRESULT OnUpdateNECButton(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateRunButton(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateGA(WPARAM wParam, LPARAM lParam);
    afx_msg void OnEnChangeEditPopsize();
    afx_msg void OnEnChangeEditGenerations();
    afx_msg void OnEnChangeEditMutation();
    afx_msg void OnEnChangeEditUsername();
    afx_msg void OnEnChangeEditPassword();
    afx_msg void OnBnClickedButtonRun();
    afx_msg void OnDblclkListResults();
    afx_msg void OnCheckLocal();
    afx_msg void OnClose();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    //afx_msg void OnBnClickedButtonEdit();
    void UpdateButtonsForRun(bool readyForRun);
    void SetStatus(char* msg);
    void LoadPlatform(CString filename);
    void ClearScenario(void);
    void AddAntenna(CString filename);
    void AddPointToLastAntenna(point *pt);
    void SetLocalRun(bool b);
    void StartEA();
    void StopSearch();
    void UpdateGUI();
    void SetAlgorithm(unsigned int algo_type);
    afx_msg void OnBnClickedButtonLua();
    afx_msg void OnBnClickedButtonX();
    afx_msg void OnBnClickedButtonY();
    afx_msg void OnBnClickedButtonZ();
    afx_msg void OnBnClickedButtonXNeg();
    afx_msg void OnBnClickedButtonYNeg();
    afx_msg void OnBnClickedButtonZNeg();
    afx_msg void OnBnClickedCheckOrtho();
    afx_msg void OnBnClickedButtonEditNec();
    afx_msg void OnBnClickedButtonAbout();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedButtonHelp();
    CTreeCtrl treeControl;
    afx_msg void OnNMDblclkTreeAntenna(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelchangedTreeAntenna(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnDblclkListAlgorithms();
    afx_msg void OnSelchangeListAlgorithms();
    void StartSA();
    void StopSA();
    void StartHC();
    void StopHC();
    void StartES();
    void StopES();
protected:
//	afx_msg LRESULT OnUserUpdateSa(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateSA(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateHC(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnUpdateES(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnBnClickedButtonSaveToLua();

//	afx_msg void OnSetfocusListResults();
protected:
	afx_msg LRESULT OnComplete(WPARAM wParam, LPARAM lParam);
};

void TestSA(int iteration, float parent_fitness, float child_fitness, float delta, char *message, float mutation);
extern int selected_algorithm;