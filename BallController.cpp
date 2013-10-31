// this file uses Doxygen comment blocks for automatic extraction of source code documentation.

/*!\file
 Implementation of the CBallController class.
 \version 0.2
 \date 15/10/99
 \author Alessandro Falappa
*/

//-----------------------------------------------------------------------------
// BallController.cpp: implementation of the CBallController class.

#include "stdafx.h"
//#include "TestGL.h"
#include "BallController.h"
#include <GL\gl.h>
#include <GL\glu.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------

CBallController::CBallController(const real& rad)
{
    initVars();
    radius=clamped(rad,0.1,1);
}


CBallController::CBallController(const real& rad,const unitquaternion& initialOrient)
{
    initVars();
    radius=clamped(rad,0.1,1);
    currentQuat=initialOrient;
}

CBallController& CBallController::operator=(const CBallController& other)
{
    if(this==&other) return *this;
    initVars();
    currentQuat=other.currentQuat;
    previousQuat=other.previousQuat;
    radius=other.radius;
    winWidth=other.winWidth;
    winHeight=other.winHeight;
    otherAxesNum=other.otherAxesNum;
    otherAxes=new vector3d[otherAxesNum];
    for(int c=0; c<otherAxesNum; c++) otherAxes[c]=other.otherAxes[c];
    BallColor=other.BallColor;
    return *this;
}

void CBallController::MouseDown(const CPoint& location)
{
    xprev=(2*location.x-winWidth)/winWidth;
    yprev=(winHeight-2*location.y)/winHeight;
    previousQuat=currentQuat;
    mouseButtonDown=true;
    bDrawBallArea=bProjectionMethod2;// draw circle only if method 2 active
}


void CBallController::MouseUp(const CPoint& location)
{
    mouseButtonDown=false;
    xprev=yprev=0.0;
    bDrawBallArea=false;
    // save current rotation axes for bodyAxes constraint at next rotation
    bodyorientation=currentQuat.getRotMatrix();
    bodyAxes[0]=vector3d(bodyorientation(0,0),bodyorientation(1,0),bodyorientation(2,0));
    bodyAxes[1]=vector3d(bodyorientation(0,1),bodyorientation(1,1),bodyorientation(2,1));
    bodyAxes[2]=vector3d(bodyorientation(0,2),bodyorientation(1,2),bodyorientation(2,2));
}


void CBallController::MouseMove(const CPoint& location)
{
    real xcurr=(2*location.x-winWidth)/winWidth;
    real ycurr=(winHeight-2*location.y)/winHeight;
    vector3d vfrom(xprev,yprev,0);
    vector3d vto(xcurr,ycurr,0);
    if(mouseButtonDown)
    {
// find the two points on sphere according to the projection method
        ProjectOnSphere(vfrom);
        ProjectOnSphere(vto);
// modify the vectors according to the active constraint
        if(whichConstraints != NO_AXES)
        {
            vector3d* axisSet=GetUsedAxisSet();
            vfrom=ConstrainToAxis(vfrom,axisSet[currentAxisIndex]);
            vto=ConstrainToAxis(vto,axisSet[currentAxisIndex]);
        };
// get the corresponding unitquaternion
        unitquaternion lastQuat=RotationFromMove(vfrom,vto);
        currentQuat*=lastQuat;
        xprev=xcurr;
        yprev=ycurr;
    }
    else if(whichConstraints != NO_AXES)
    {
        ProjectOnSphere(vto);
        currentAxisIndex=NearestConstraintAxis(vto);
    };
}

void CBallController::IssueGLrotation()
{
#ifdef REAL_IS_DOUBLE
    glMultMatrixd(currentQuat.getRotMatrix());
#else
    glMultMatrixf(currentQuat.getRotMatrix());
#endif
}

void CBallController::TransformVector(vector3d& v)
{
    tmatrix m(currentQuat.getRotMatrix());
    m.invert();
    float* mv = (float*) m;
    vector3d r;
    r.x() = mv[0] * v.x() + mv[4] * v.y() + mv[8] * v.z();
    r.y() = mv[1] * v.x() + mv[5] * v.y() + mv[9] * v.z();
    r.z() = mv[2] * v.x() + mv[6] * v.y() + mv[10] * v.z();
    v = r;
}

void CBallController::ProjectOnSphere(vector3d& v) const
{
    real rsqr=radius*radius;
    real dsqr=v.x()*v.x()+v.y()*v.y();
    if(bProjectionMethod2)
    {
        // if inside sphere project to sphere else on plane
        if(dsqr>rsqr)
        {
            register real scale=(radius-.05)/sqrt(dsqr);
            v.x()*=scale;
            v.y()*=scale;
            v.z()=0;
        }
        else
        {
            v.z()=sqrt(rsqr-dsqr);
        }
    }
    else
    {
        // if relatively "inside" sphere project to sphere else on hyperbolic sheet
        if(dsqr<(rsqr*0.5))	v.z()=sqrt(rsqr-dsqr);
        else v.z()=rsqr/(2*sqrt(dsqr));
    };
}

unitquaternion CBallController::RotationFromMove(const vector3d& vfrom,const vector3d& vto)
{
    if(bProjectionMethod2)
    {
        quaternion q;
        q.x()=vfrom.z()*vto.y()-vfrom.y()*vto.z();
        q.y()=vfrom.x()*vto.z()-vfrom.z()*vto.x();
        q.z()=vfrom.y()*vto.x()-vfrom.x()*vto.y();
        q.w()=vfrom*vto;
        return unitquaternion(q);
    }
    else
    {
// calculate axis of rotation and correct it to avoid "near zero length" rot axis
        vector3d rotaxis=(vto^vfrom);
        rotaxis.EpsilonCorrect(X_AXIS);
// find the amount of rotation
        vector3d d(vfrom-vto);
        real t=d.length()/(2.0*radius);
        clamp(t,-1.0,1.0);
        real phi=2.0*asin(t);
        return unitquaternion(phi,rotaxis);
    }
}

void CBallController::Key(UINT nChar)
{
    switch(nChar)
    {
    case VK_UP:
    case VK_NUMPAD8:
        currentQuat*=unitquaternion(DegToRad(angleKeyIncrement),X_AXIS);
        break;
    case VK_DOWN:
    case VK_NUMPAD2:
        currentQuat*=unitquaternion(DegToRad(-angleKeyIncrement),X_AXIS);
        break;
    case VK_RIGHT:
    case VK_NUMPAD6:
        currentQuat*=unitquaternion(DegToRad(angleKeyIncrement),Y_AXIS);
        break;
    case VK_LEFT:
    case VK_NUMPAD4:
        currentQuat*=unitquaternion(DegToRad(-angleKeyIncrement),Y_AXIS);
        break;
    case VK_PRIOR:
    case VK_NUMPAD9:
        currentQuat*=unitquaternion(DegToRad(angleKeyIncrement),Z_AXIS);
        break;
    case VK_HOME:
    case VK_NUMPAD7:
        currentQuat*=unitquaternion(DegToRad(-angleKeyIncrement),Z_AXIS);
        break;
    case VK_DELETE:
    case VK_NUMPAD5:
        currentQuat=unitquaternion(0,X_AXIS);
        break;
    case VK_ESCAPE:
        currentQuat=previousQuat;
        MouseUp(CPoint(-1,-1));
        break;
    case VK_TAB:
        if(mouseButtonDown && whichConstraints!=NO_AXES) currentAxisIndex=(currentAxisIndex+1)%3;
        break;
    };
}

void CBallController::DrawBallLimit()
{
    // "spherical zone" of controller
#ifdef REAL_IS_DOUBLE
    glColor3dv(BallColor);
#else
    glColor3fv(BallColor);
#endif
    glCallList(GLdisplayList);
}

void CBallController::InitDisplayLists()
{
    GLdisplayList=glGenLists(1);
    if(GLdisplayList!=0)
    {
        GLUquadricObj* qobj=gluNewQuadric();
        gluQuadricDrawStyle(qobj,GLU_LINE);
        gluQuadricNormals(qobj,GLU_NONE);
        glNewList(GLdisplayList,GL_COMPILE);
        gluDisk(qobj,radius,radius,24,1);
        glEndList();
        gluDeleteQuadric(qobj);
    }
}

void CBallController::initVars()
{
    winWidth=winHeight=0;
    previousQuat=currentQuat=unitquaternion(0,X_AXIS);
    mouseButtonDown=bDrawBallArea=bProjectionMethod2=bDrawConstraints=false;
    xprev=yprev=0.0;
    center=CPoint(0,0),
    radius=0.6;
    GLdisplayList=currentAxisIndex=otherAxesNum=0;
    BallColor=vector3d(0.0,0.5,1.0);
    otherAxes=NULL;
    whichConstraints=NO_AXES;
    cameraAxes[0]=bodyAxes[0]=X_AXIS;
    cameraAxes[1]=bodyAxes[1]=Y_AXIS;
    cameraAxes[2]=bodyAxes[2]=Z_AXIS;
    bodyorientation.loadIdentity();
    angleKeyIncrement=5;
}

void CBallController::Reset(void)
{
    previousQuat=currentQuat=unitquaternion(0,X_AXIS);
    xprev=yprev=0.0;
    center=CPoint(0,0),
//	radius=0.6;
    GLdisplayList=currentAxisIndex=otherAxesNum=0;
    otherAxes=NULL;
    whichConstraints=NO_AXES;
    cameraAxes[0]=bodyAxes[0]=X_AXIS;
    cameraAxes[1]=bodyAxes[1]=Y_AXIS;
    cameraAxes[2]=bodyAxes[2]=Z_AXIS;
    bodyorientation.loadIdentity();
    angleKeyIncrement=5;
}

void CBallController::Rotate(const quaternion& rot)
{
    previousQuat=currentQuat=rot;
}

void CBallController::SetColor(COLORREF col)
{
    BallColor.x()=GetRValue(col)/255.0;
    BallColor.y()=GetGValue(col)/255.0;
    BallColor.z()=GetBValue(col)/255.0;
}

void CBallController::SetColorV(vector3d colvec)
{
    clamp(colvec,0,1);
    BallColor=colvec;
}

vector3d CBallController::ConstrainToAxis(const vector3d& loose,const vector3d& axis)
{
    vector3d onPlane;
    register real norm;
    onPlane = loose-axis*(axis*loose);
    norm = onPlane.length();
    if (norm > 0)
    {
        if (onPlane.z() < 0.0) onPlane = -onPlane;
        return ( onPlane/=sqrt(norm) );
    };
    if (axis.z() == 1) onPlane = X_AXIS;
    else
    {
        onPlane = vector3d(-axis.y(), axis.x(), 0);
        onPlane.normalize();
    }
    return (onPlane);
}

int CBallController::NearestConstraintAxis(const vector3d& loose)
{
    vector3d* axisSet=GetUsedAxisSet();
    vector3d onPlane;
    register float max, dot;
    register int i, nearest;
    max = -1;
    nearest = 0;
    if(whichConstraints == OTHER_AXES)
    {
        for (i=0; i<otherAxesNum; i++)
        {
            onPlane = ConstrainToAxis(loose, axisSet[i]);
            dot = onPlane*loose;
            if (dot>max) max = dot;
            nearest = i;
        }
    }
    else
    {
        for (i=0; i<3; i++)
        {
            onPlane = ConstrainToAxis(loose, axisSet[i]);
            dot = onPlane*loose;
            if (dot>max)
            {
                max = dot;
                nearest = i;
            };
        }
    };
    return (nearest);
}

vector3d* CBallController::GetUsedAxisSet()
{
    vector3d* axes=NULL;
    switch(whichConstraints)
    {
    case CAMERA_AXES:
        axes=cameraAxes;
        break;
    case BODY_AXES:
        axes=bodyAxes;
        break;
    case OTHER_AXES:
        axes=otherAxes;
        break;
    };
    return axes;
}

void CBallController::DrawConstraints()
{
    glColor3f(0,.75f,0);
    if(whichConstraints==CAMERA_AXES)
    {
        glCallList(GLdisplayList);
        glBegin(GL_LINES);
        glVertex3f(-radius,0,0);
        glVertex3f(radius,0,0);
        glVertex3f(0,-radius,0);
        glVertex3f(0,radius,0);
        glEnd();
    };
    if(whichConstraints==BODY_AXES)
    {
        glPushMatrix();
#ifdef REAL_IS_DOUBLE
        glMultMatrixd(bodyorientation);
#else
        glMultMatrixf(bodyorientation);
#endif
        glCallList(GLdisplayList);
        glRotated(90,1.0,0.0,0.0);
        glCallList(GLdisplayList);
        glRotated(90,0.0,1.0,0.0);
        glCallList(GLdisplayList);

        glPopMatrix();
    };
}

void CBallController::DrawBall()
{
// change the projection matrix to identity (no view transformation )
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
// reset the transformations
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
// prepare the circle display list the first time
    if(GLdisplayList == 0) InitDisplayLists();
// disable lighting and depth testing
    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
// draw the constraints or the ball limit if appropriate
    if(bDrawConstraints && whichConstraints!=NO_AXES) DrawConstraints();
    else if(bDrawBallArea) DrawBallLimit();
    glPopAttrib();
// restore the modelview and projection matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
