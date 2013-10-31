#pragma once

// CNECEditor dialog

class CNECEditor : public CDialog
{
    DECLARE_DYNAMIC(CNECEditor)

public:
    CNECEditor(CWnd* pParent = NULL);   // standard constructor
    virtual ~CNECEditor();

    void UpdateText(void);

// Dialog Data
    enum { IDD = IDD_EDITOR_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
};
