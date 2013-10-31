// AngleCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "APOT6.h"
#include "AngleCtrl.h"
#include "Geometry.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CAngleCtrl, CWnd)
    //{{AFX_MSG_MAP(CAngleCtrl)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAngleCtrl

IMPLEMENT_DYNCREATE(CAngleCtrl, CWnd)

CAngleCtrl::CAngleCtrl()
{
    minAngle = 0;
    maxAngle = 0;
}

CAngleCtrl::~CAngleCtrl()
{
}

void CAngleCtrl::Create(CRect rect, CWnd *parent)
{
    CString className = AfxRegisterWndClass(
                            CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                            ::LoadCursor(NULL, IDC_ARROW),
                            (HBRUSH)GetStockObject(WHITE_BRUSH),
                            NULL);

    BOOL result = CreateEx(
                      0,
                      className,
                      "Angle",
                      WS_CHILD | WS_VISIBLE,
                      rect,
                      parent,
                      0);
}

/////////////////////////////////////////////////////////////////////////////
// CAngleCtrl diagnostics

#ifdef _DEBUG
void CAngleCtrl::AssertValid() const
{
    CWnd::AssertValid();
}

void CAngleCtrl::Dump(CDumpContext& dc) const
{
    CWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAngleCtrl message handlers

void CAngleCtrl::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    CRect	rect;
    GetClientRect(&rect);
    dc.SelectStockObject(WHITE_BRUSH);
    dc.Ellipse(rect);

    if (minAngle < maxAngle)
    {
        dc.SelectStockObject(BLACK_BRUSH);

        CPoint ptStart;
        float theta = minAngle * PI / 180.0f + PI;
        ptStart.x = rect.Width() * sin(theta);
        ptStart.y = rect.Height() * cos(theta);
        ptStart += rect.CenterPoint();

        CPoint ptStop;
        theta = maxAngle * PI / 180.0f + PI;
        ptStop.x = rect.Width() * sin(theta);
        ptStop.y = rect.Height() * cos(theta);
        ptStop += rect.CenterPoint();

        dc.Pie(rect, ptStart, ptStop);
    }
}

BOOL CAngleCtrl::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
//	return CWnd::OnEraseBkgnd(pDC);
}
