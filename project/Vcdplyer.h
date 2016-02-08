//------------------------------------------------------------------------------
// File: vcdplyer.h
//
// Desc: DirectShow sample code
//       - Class header file for VMRPlayer sample
//
// Copyright (c) 1994 - 2001, Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

/* -------------------------------------------------------------------------
** CMpegMovie - an Mpeg movie playback class.
** -------------------------------------------------------------------------
*/

#if !defined( FILE_VCDPLYER_H )
#define FILE_VCDPLYER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "alphablt.h"

enum EMpegMovieMode { MOVIE_NOTOPENED = 0x00,
                      MOVIE_OPENED = 0x01,
                      MOVIE_PLAYING = 0x02,
                      MOVIE_STOPPED = 0x03,
                      MOVIE_PAUSED = 0x04 };

BOOL VerifyVMR(void);

struct IMpegAudioDecoder;
struct IMpegVideoDecoder;
struct IQualProp;


class CMpegMovie :
    public CUnknown,
    public IVMRSurfaceAllocator,
    public IVMRImagePresenter,
    public IVMRSurfaceAllocatorNotify

{
private:
    // Our state variable - records whether we are opened, playing etc.
    EMpegMovieMode   m_Mode;
    HWND             m_hwndApp;
    GUID             m_TimeFormat;
    int             m_iDuration;		// movie duration

	 int					m_iBrightness, m_iContrast, m_iSaturation;

	 BOOL					m_bMute;
	 BOOL					m_bWin9xCompatible;

	 BOOL					m_bPanScan;
	 double				m_valPanScan;
	 RECT					m_rcMovieWindow, m_rcMovieDest;


    LPDIRECTDRAWSURFACE7    m_pddsPrimary;
    LPDIRECTDRAWSURFACE7    m_pddsPriText;
    LPDIRECTDRAWSURFACE7    m_pddsRenderT;

    IFilterGraph            *m_Fg;
    IGraphBuilder           *m_Gb;
    IMediaControl           *m_Mc;
    IMediaSeeking           *m_Ms;
    IMediaEventEx           *m_Me;
	 IBasicAudio				 *m_Ba;


    IVMRSurfaceAllocator*       m_Sa;
    IVMRImagePresenter*         m_Ip;
    IVMRWindowlessControl*      m_Wc;
    IVMRSurfaceAllocatorNotify* m_San;

	 LPDIRECTDRAWCOLORCONTROL				m_pDdcc;



	 // If we don't want use new VMR capabilites, we can use old
	 //  interfaces to manipulate the movie ( win9x compatible ).
	 IVideoWindow				 *m_Vw;
	 IBasicVideo				 *m_Bv;


    HRESULT CreateDefaultAllocatorPresenter();
    HRESULT AddVideoMixingRendererToFG();
    HRESULT OnSetDDrawDevice(LPDIRECTDRAW7 pDD, HMONITOR hMon);


public:
     CMpegMovie(HWND hwndApplication);
    ~CMpegMovie();

	 DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID, void**);

    // IVMRSurfaceAllocator
    STDMETHODIMP AllocateSurface(DWORD_PTR dwUserID,
                                VMRALLOCATIONINFO* lpAllocInfo,
                                 DWORD* lpdwActualBackBuffers,
                                 LPDIRECTDRAWSURFACE7* lplpSurface);
    STDMETHODIMP FreeSurface(DWORD_PTR dwUserID);
    STDMETHODIMP PrepareSurface(DWORD_PTR dwUserID,
                                LPDIRECTDRAWSURFACE7 lplpSurface,
                                DWORD dwSurfaceFlags);
    STDMETHODIMP AdviseNotify(IVMRSurfaceAllocatorNotify* lpIVMRSurfAllocNotify);

    // IVMRSurfaceAllocatorNotify
    STDMETHODIMP AdviseSurfaceAllocator(DWORD_PTR dwUserID,
                                        IVMRSurfaceAllocator* lpIVRMSurfaceAllocator);
    STDMETHODIMP SetDDrawDevice(LPDIRECTDRAW7 lpDDrawDevice,HMONITOR hMonitor);
    STDMETHODIMP ChangeDDrawDevice(LPDIRECTDRAW7 lpDDrawDevice,HMONITOR hMonitor);
    STDMETHODIMP RestoreDDrawSurfaces();
    STDMETHODIMP NotifyEvent(LONG EventCode, LONG_PTR lp1, LONG_PTR lp2);
    STDMETHODIMP SetBorderColor(COLORREF clr);

    // IVMRImagePresenter
    STDMETHODIMP StartPresenting(DWORD_PTR dwUserID);
    STDMETHODIMP StopPresenting(DWORD_PTR dwUserID);
    STDMETHODIMP PresentImage(DWORD_PTR dwUserID,
                              VMRPRESENTATIONINFO* lpPresInfo);


    HRESULT         OpenMovie(TCHAR *lpFileName, BOOL bWin9xCompatible);
    DWORD           CloseMovie();
    BOOL            PlayMovie();
    BOOL            PauseMovie();
    BOOL            StopMovie();
    OAFilterState   GetStateMovie();
    long            GetMovieEventCode();
    BOOL            PutMoviePosition(LONG x, LONG y, LONG cx, LONG cy);
    BOOL            GetMoviePosition(LONG *x, LONG *y, LONG *cx, LONG *cy);
    BOOL            GetNativeMovieSize(LONG *cx, LONG *cy);
    BOOL            CanMovieFrameStep();
    BOOL            FrameStepMovie();
    REFTIME         GetDuration();
    REFTIME         GetCurrentPosition();
    BOOL            SeekToPosition(REFTIME rt,BOOL bFlushData,BOOL bSeekToKeyFrame);
    EMpegMovieMode  StatusMovie();
    BOOL            IsTimeFormatSupported(GUID Format);
    BOOL            IsTimeSupported();
    BOOL            SetTimeFormat(GUID Format);
    GUID            GetTimeFormat();
    void            SetFocus();
    BOOL            ConfigDialog(HWND hwnd);
    BOOL            RepaintVideo(HWND hwnd, HDC hdc);
    BOOL            SetAppImage(VMRALPHABITMAP* lpBmpInfo);
    BOOL            UpdateAppImage(VMRALPHABITMAP* lpBmpInfo);
    void            DisplayModeChanged();
    HRESULT         GetCurrentImage(LPBYTE* lplpDib);

    void            SetBorderClr(COLORREF clr);
	 BOOL				  SetVolume( double dVolume );
	 BOOL				  SetMute( BOOL bMute, double dVolume=100.0 );
	 void				  SetMovieWindow( HWND hwndApplication );

	 void				  ShowMovie( BOOL bShow );
	 BOOL				  SetNotifyWindow( HWND hwnd, long lMsg );

	 void					EnablePanScan( BOOL bEnable );
	 void					SetPanScanValue( double val );
	 double				GetPanScanValue();

	 BOOL					SetColorControl( int valBrightness, 
								int valContrast, int valSaturation );

	 DWORD				GetColorKey();
	 DWORD				SetColorKey( DWORD col );

	 BOOL				  IsWin9xCompatible();
	 double				GetFPS();

	 IGraphBuilder*	GetGraphBuilder() { 
		 return m_Gb; 
	 }


};

#endif // !defined( FILE_VCDPLYER_H )