#if !defined( FILE_ALPHABLT_H)
#define FILE_ALPHABLT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CAlphaBlt
{
private:

    LPDIRECTDRAW7               m_pDD;
    LPDIRECT3D7                 m_pD3D;
    LPDIRECT3DDEVICE7           m_pD3DDevice;
    LPDIRECTDRAWSURFACE7        m_lpDDBackBuffer;

    LPDIRECTDRAWSURFACE7        m_lpDDMirror;
    LPDIRECTDRAWSURFACE7        m_lpDDM32;
    LPDIRECTDRAWSURFACE7        m_lpDDM16;
    DDSURFACEDESC2              m_ddsdM32;
    DDSURFACEDESC2              m_ddsdM16;

    bool                        m_fPowerOf2;
    bool                        m_fSquare;

    //
    // IsSurfaceBlendable
    //
    // Checks the DD surface description and the given
    // alpha value to determine if this surface is blendable.
    //
    bool
    IsSurfaceBlendable(
        DDSURFACEDESC2& ddsd,
        BYTE fAlpha
        );

    //
    // MirrorSourceSurface
    //
    // The mirror surface can be either 16 or 32 bit RGB depending
    // upon the format of the source surface.
    //
    // Of course it should have the "texture" flag
    // set and should be in VRAM.  If we can't create the
    // surface then the AlphaBlt should fail
    //
    HRESULT MirrorSourceSurface(
        LPDIRECTDRAWSURFACE7 lpDDS,
        DDSURFACEDESC2& ddsd
        );

public:

   
    CAlphaBlt(LPDIRECTDRAWSURFACE7 lpDDSDst, HRESULT* phr);
	 ~CAlphaBlt();

    HRESULT
    AlphaBlt(RECT* lpDst,
             LPDIRECTDRAWSURFACE7 lpDDSSrc,
             RECT* lpSrc,
             BYTE  bAlpha
             );
    bool TextureSquare();

    bool TexturePower2();
};

#endif // !defined( FILE_ALPHABLT_H)