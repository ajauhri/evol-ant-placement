#include "stdAfx.h"
#include "Texture.h"
#include <assert.h>

// ===========================================================================
//	CLASS Texture
// ===========================================================================
/*****************************************************************************
	Texture::Texture

	Constructors
*****************************************************************************/
Texture::Texture(void) :
    texName(0)
    ,target_type(GL_TEXTURE_1D)
    ,hBmp(NULL)
    ,imageWidth(0)
    ,imageHeight(0)
{
}

/*****************************************************************************
	Texture::~Texture

	Destructor.
*****************************************************************************/
Texture::~Texture(void)
{
    if (hBmp != NULL) ::DeleteObject(hBmp);
}

/*****************************************************************************
	Texture::Load


*****************************************************************************/
BOOL Texture::Load(UINT resourceID, BOOL repeat)
{
    // Load the bitmap.
    // To load an image from the resources use:

//   hBmp = (HBITMAP) ::LoadImage(AfxGetResourceHandle(),
    hBmp = (HBITMAP) ::LoadImage(GetModuleHandle(NULL),
                                 MAKEINTRESOURCE(resourceID), IMAGE_BITMAP, 0, 0,
                                 LR_CREATEDIBSECTION);

    assert(hBmp != NULL);
//	wglGetLastError();
    SetLastError(0);
    if (hBmp == NULL) return FALSE;
    // where IDB_BITMAP1 is the ID of the bitmap resource.
    return InternalLoad(repeat);
}

/*****************************************************************************
	Texture::InternalLoad


*****************************************************************************/
BOOL Texture::InternalLoad(BOOL repeat)
{
    assert(hBmp != NULL);
    ::LockResource(hBmp);

    // Get color map info.
    BITMAP BM;
    ::GetObject(hBmp, sizeof(BM), &BM);
    imageWidth	= BM.bmWidth;
    imageHeight = BM.bmHeight;

    // Reverse the bits from BGR to RGB because some cards don't support GL_BGR_EXT
    if (BM.bmBitsPixel == 24)
    {
        unsigned char* bits = (unsigned char*) BM.bmBits;

        unsigned char blue;
        long numlines = BM.bmHeight;
        long scanlen = BM.bmWidthBytes;
        int line = 0;
        int pos = 0;
        while (line < numlines)
        {
            do
            {
                blue = bits[pos];
                bits[pos] = bits[pos+2];
                bits[pos+2] = blue;
                pos += 3;
            }
            while (pos < scanlen);
            scanlen += BM.bmWidthBytes;
            line++;
        }
    }

    // Generate name for and bind texture.
    if (!texName) ::glGenTextures(1, &texName);
    ::glBindTexture(target_type, texName);

    // Set up all the the texture mapping params.
    if (BM.bmBitsPixel == 24)
    {
        ::glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        ::glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        ::glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        ::glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        ::glTexParameteri(target_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        ::glTexParameteri(target_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // HACK - turned off LINEAR_MIPMAP for eye pupil
//		::glTexParameteri(target_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
        ::glTexParameteri(target_type, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP);
        if (target_type == GL_TEXTURE_2D)
            ::glTexParameteri(target_type, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP);

        // Build the texture map
        // Could use glTexImage2D instead but then we would have to scale
        // the image to make it a power of 2.
        if (target_type == GL_TEXTURE_2D)
            ::gluBuild2DMipmaps(target_type, 3, BM.bmWidth, BM.bmHeight,
                                GL_RGB, GL_UNSIGNED_BYTE, BM.bmBits );
        else if (target_type == GL_TEXTURE_1D)
            ::gluBuild1DMipmaps(target_type, 3, BM.bmWidth,
                                GL_RGB, GL_UNSIGNED_BYTE, BM.bmBits );
        else
            assert(false);

    }
    else
    {
        // Set up all the the texture mapping params.
        ::glTexParameteri(target_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        ::glTexParameteri(target_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (target_type == GL_TEXTURE_2D)
            ::gluBuild2DMipmaps(target_type, GL_LUMINANCE, BM.bmWidth, BM.bmHeight,
                                GL_LUMINANCE, GL_UNSIGNED_BYTE, BM.bmBits);
        else if (target_type == GL_TEXTURE_1D)
            ::gluBuild1DMipmaps(target_type, GL_LUMINANCE, BM.bmWidth,
                                GL_LUMINANCE, GL_UNSIGNED_BYTE, BM.bmBits);
        else
            assert(false);
    }
    // If your texture REPLACES the fragment colors,
    // you won't get any lighting effects. If your texture
    // MODULATES or BLENDS the texture and fragment colors, you
    // will get lighting effects.
    ::glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    ::glBindTexture(target_type, 0);

    GLenum error = ::glGetError();
    assert(GL_NO_ERROR == error);

    return TRUE;
}

/*****************************************************************************
	Texture::LoadBytes


*****************************************************************************/
CGLRGBTRIPLE* Texture::LoadBytes(const BITMAP& BM)
{
    CGLRGBTRIPLE* map = new CGLRGBTRIPLE[BM.bmWidth * BM.bmHeight];
    switch (BM.bmBitsPixel)
    {
    case 32:
    {
        RGBQUAD*		pSrc = (RGBQUAD*) BM.bmBits;
        CGLRGBTRIPLE*	pDest = map;
        for(int jj = 0; jj < BM.bmHeight; jj++)
        {
            for(int ii = 0; ii < BM.bmWidth; ii++)
            {
                pDest->red		= pSrc->rgbRed;
                pDest->green	= pSrc->rgbGreen;
                pDest->blue		= pSrc->rgbBlue;
                pDest++;
                pSrc++;
            }
        }
    }
    break;

    case 24:
    {
        RGBTRIPLE*		pSrc = (RGBTRIPLE*) BM.bmBits;
        CGLRGBTRIPLE*	pDest = map;
        // calculate padding
        int widthDiff = BM.bmWidthBytes - BM.bmWidth * sizeof(RGBTRIPLE);
        for(int jj = 0; jj < BM.bmHeight; jj++)
        {
            for(int ii = 0; ii <  BM.bmWidth; ii++)
            {
                pDest->red	= pSrc->rgbtRed;
                pDest->green	= pSrc->rgbtGreen;
                pDest->blue	= pSrc->rgbtBlue;
                pDest++;
                pSrc++;
            }
            pSrc = (RGBTRIPLE*)((BYTE*) pSrc + widthDiff) ;
        }
    }
    break ;

    case 16:
    {
        WORD*			pSrc = (WORD*) BM.bmBits;	//WORD = 16 bit UINT
        CGLRGBTRIPLE*	pDest = map;
        int widthDiff = BM.bmWidthBytes - BM.bmWidth * sizeof(WORD);
        for(int jj = 0 ; jj < BM.bmHeight; jj++)
        {
            for(int ii = 0; ii < BM.bmWidth; ii++)
            {
                pDest->red		= (BYTE)((*pSrc & 0x7C00) >> 10);
                pDest->green	= (BYTE)((*pSrc & 0x03E0) >> 5);
                pDest->blue		= (BYTE)(*pSrc & 0x001F);
                pDest++;
                pSrc++;
            }
            pSrc = (WORD*)((BYTE*) pSrc + widthDiff) ;
        }
    }
    break;
    default:
        assert(FALSE);
        return NULL;
    }
    return map;
}
