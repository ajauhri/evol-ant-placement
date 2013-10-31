#if !defined(AFX_ANGLECTRL_H__2C85DED3_11A7_4D36_8B90_F9DC34FD2CBB__INCLUDED_)
#define AFX_ANGLECTRL_H__2C85DED3_11A7_4D36_8B90_F9DC34FD2CBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AngleCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAngleCtrl view

class CAngleCtrl : public CWnd
{
protected:
// Attributes
public:

    DWORD	minAngle;
    DWORD	maxAngle;
// Operations
public:
    CAngleCtrl();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CAngleCtrl)
    virtual ~CAngleCtrl();

    void Create(CRect rect,CWnd* parent);

// Implementation
protected:
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
protected:
    //{{AFX_MSG(CAngleCtrl)
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANGLECTRL_H__2C85DED3_11A7_4D36_8B90_F9DC34FD2CBB__INCLUDED_)
