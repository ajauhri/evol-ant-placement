// this file uses Doxygen comment blocks for automatic extraction of source code documentation.

/*!\file
 Declaration of the CBallController class.
 This class implements a virtual trackball rotation controller suitable for
 use in 3D graphical applications.
 This implementation "takes inspiration" and merges features found in
 K.Shoemake's Arcball controller (see Graphic Gems vol.4) and J.Childs' GLcube rotation
 controller which is in turn based on M.J.Kilgard's GLUT code.
 The interaction method has been tailored on a Document-View-Controller paradigm
 such as that in Microsoft MFC framework but it's applicable to other
 contexts as well.
 \version 0.2
 \date 15/10/99
 \author Alessandro Falappa
*/

//-----------------------------------------------------------------------------
// BallController.h: interface for the CBallController class.

#if !defined(AFX_BALLCONTROLLER_H__196CBD82_3858_11D2_80B1_A7800FACFE7F__INCLUDED_)
#define AFX_BALLCONTROLLER_H__196CBD82_3858_11D2_80B1_A7800FACFE7F__INCLUDED_

#include "3dmath.h"	// Added by ClassView

//MSVC compiler specifics
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Auxiliary Type Definitions
/*!
Defines some constant values used to set the axes to which
constrain rotations about.
*/
enum AxisSet
{
    NO_AXES,
    CAMERA_AXES,
    BODY_AXES,
    OTHER_AXES
};

/*!
This is the controller object
*/
class CBallController
{
private:
    bool bDrawConstraints;
    tmatrix bodyorientation;
    int angleKeyIncrement;
    void DrawConstraints();
    vector3d* GetUsedAxisSet();
    vector3d BallColor;
    bool bProjectionMethod2;
    bool bDrawBallArea;
    int GLdisplayList;
    unitquaternion currentQuat;
    unitquaternion previousQuat;
    real radius;
    real winWidth;
    real winHeight;
    real xprev;
    real yprev;
    CPoint center;
    bool mouseButtonDown;
    AxisSet whichConstraints;
    int currentAxisIndex;
    vector3d cameraAxes[3];
    vector3d bodyAxes[3];
    vector3d* otherAxes;
    int otherAxesNum;

    void InitDisplayLists();
    void initVars(void);
    void ProjectOnSphere(vector3d& v) const;
    unitquaternion RotationFromMove(const vector3d& vfrom,const vector3d& vto);
    vector3d ConstrainToAxis(const vector3d& loose,const vector3d& axis);
    int NearestConstraintAxis(const vector3d& loose);
    void DrawBallLimit();
public:
    void Reset(void);
    void Rotate(const quaternion& rot);
    bool GetDrawConstraints();
    void SetDrawConstraints(bool flag=true);
    void DrawBall();
    int GetAngleKeyIncrement();
    void SetAngleKeyIncrement(int ang);
    void UseConstraints(AxisSet constraints);
    void ToggleMethod();
    void SetAlternateMethod(bool flag=true);
    vector3d GetColorV();
    COLORREF GetColor();
    void SetColor(COLORREF col);
    void SetColorV(vector3d colvec);
    CBallController();
    CBallController(const real& rad);
    CBallController(const real& rad,const unitquaternion& initialOrient);
    CBallController(const CBallController& other);
    virtual ~CBallController();
    CBallController& operator=(const CBallController& other);
    void Resize(const real& newRadius);
    void ClientAreaResize(const CRect& newSize);
    void MouseDown(const CPoint& location);
    void MouseUp(const CPoint& location);
    void MouseMove(const CPoint& location);
    void IssueGLrotation();
    void TransformVector(vector3d& v);
    void Key(UINT nChar);
};

//---------------------------------------------------------------------------
// inlines

inline CBallController::~CBallController()
{
    if(otherAxes) delete[] otherAxes;
}

inline CBallController::CBallController(const CBallController& other)
{
    *this=other;
}

inline void CBallController::Resize(const real& newRadius)
{
    radius=newRadius;
}

inline void CBallController::ClientAreaResize(const CRect& newSize)
{
    winWidth=real(newSize.Width());
    winHeight=real(newSize.Height());
//	center=CPoint( (newSize.right-newSize.left)/2 , (newSize.bottom-newSize.top)/2);
}

inline CBallController::CBallController()
{
    initVars();
}

inline COLORREF CBallController::GetColor()
{
    return RGB(int(BallColor.x()*255),int(BallColor.y()*255),int(BallColor.z()*255));
}

inline vector3d CBallController::GetColorV()
{
    return BallColor;
}

inline void CBallController::SetAlternateMethod(bool flag)
{
    bProjectionMethod2=flag;
}

inline void CBallController::ToggleMethod()
{
    if(bProjectionMethod2) bProjectionMethod2=false;
    else bProjectionMethod2=true;
}

inline void CBallController::UseConstraints(AxisSet constraints)
{
    whichConstraints=constraints;
}

inline int CBallController::GetAngleKeyIncrement()
{
    return angleKeyIncrement;
}

inline void CBallController::SetAngleKeyIncrement(int ang)
{
    angleKeyIncrement=abs(ang)%360;
}

inline bool CBallController::GetDrawConstraints()
{
    return bDrawConstraints;
}

inline void CBallController::SetDrawConstraints(bool flag)
{
    bDrawConstraints=flag;
}

#endif
