#pragma once
#include "afxwin.h"


// CNECDialog dialog

class CNECDialog : public CDialog
{
    DECLARE_DYNAMIC(CNECDialog)

public:
    CNECDialog(CWnd* pParent = NULL);   // standard constructor
    virtual ~CNECDialog();

    void UpdateParams();

// Dialog Data
    enum { IDD = IDD_NEC_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedCheckGs();
    afx_msg void OnBnClickedCheckGn();
    afx_msg void OnBnClickedCheckGd();
    afx_msg void OnBnClickedCheckNe();
    afx_msg void OnBnClickedCheckLe();
private:
    CEdit gs_fscale;
public:
    CComboBox gn_iperf;
    CEdit gn_nradl;
    CEdit gn_epsr;
    CEdit gn_sig;
    CComboBox gd_iclif;
    CEdit gd_epsr2;
    CEdit gd_sig2;
    CEdit gd_clt;
    CEdit gd_cht;
    CComboBox ne_near;
    CEdit ne_nrx;
    CEdit ne_nry;
    CEdit ne_nrz;
    CEdit ne_xnr;
    CEdit ne_ynr;
    CEdit ne_znr;
    CComboBox le_rset;
    CEdit le_npts;
    CEdit le_x1;
    CEdit le_y1;
    CEdit le_z1;
    CEdit le_x2;
    CEdit le_y2;
    CEdit le_z2;
protected:
    virtual void OnOK();
};
