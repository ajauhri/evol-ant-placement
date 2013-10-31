#ifndef _TEXTURE_
#define _TEXTURE_

#include <GL\gl.h>
#include <GL\glu.h>


typedef struct _CGLRGBTRIPLE
{
    BYTE red;
    BYTE green;
    BYTE blue;
} CGLRGBTRIPLE;

//=====================================================
// CLASS Texture
//
//
//
//=====================================================
class Texture
{

public:
    Texture(void);
    ~Texture(void);

    BOOL				Load(UINT resourceID, BOOL repeat = FALSE);

    inline void			SetTarget(GLuint target)
    {
        target_type = target;
    };
    inline GLuint		GetTarget(void) const
    {
        return target_type;
    };

    inline BOOL			IsEmpty(void) const
    {
        return (hBmp == NULL);
    };
    inline  void		Bind(void) const
    {
        ::glBindTexture(target_type, texName);
    };
    inline UINT			GetTexName(void) const
    {
        return texName;
    };

    inline UINT			GetImageWidth(void) const
    {
        return imageWidth;
    };
    inline UINT			GetImageHeight(void) const
    {
        return imageHeight;
    };

protected:

private:

    BOOL				InternalLoad(BOOL repeat);
    CGLRGBTRIPLE*		LoadBytes(const BITMAP& BM);

    UINT				texName; // Name of bound texture.
    GLenum				target_type;

    HBITMAP				hBmp;

    UINT				imageWidth;
    UINT				imageHeight;
};


#endif	// _TEXTURE_