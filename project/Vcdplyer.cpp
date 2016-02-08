//------------------------------------------------------------------------------
// File: vcdplyer.cpp
//
// Desc: DirectShow sample code
//       - A VMR-enabled player application
//
// Copyright (c) 1994 - 2001, Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#include "stdafx.h"
#include "vcdplyer.h"


#define MY_USER_ID 0x27924721


/******************************Public*Routine******************************\
* CMpegMovie
*
* Constructors and destructors
*
\**************************************************************************/

CMpegMovie::CMpegMovie(HWND hwndApplication)
		: CUnknown(NAME("Allocator Presenter"), NULL),
      m_hwndApp(hwndApplication),
      m_Mode(MOVIE_NOTOPENED),
      m_Fg(NULL),
      m_Gb(NULL),
      m_Mc(NULL),
      m_Ms(NULL),
      m_Me(NULL),
      m_Wc(NULL),
		m_Ba(NULL),
		m_Vw(NULL),
		m_Bv(NULL),
      m_TimeFormat(TIME_FORMAT_MEDIA_TIME),
		m_bMute(FALSE),

		m_bPanScan(FALSE),
		m_valPanScan(0),

      m_San(NULL),
      m_pddsRenderT(NULL),
      m_pddsPriText(NULL),
      m_pddsPrimary(NULL),
      m_iDuration(-1),
		m_pDdcc(NULL),

		m_iBrightness(0),
		m_iContrast(50),
		m_iSaturation(50)
{
	AddRef();
	SetRectEmpty( &m_rcMovieDest ); m_rcMovieWindow = m_rcMovieDest;
}

CMpegMovie::~CMpegMovie()
{
}


/******************************Public*Routine******************************\
* SetRenderingMode
*
\**************************************************************************/
static HRESULT SetRenderingMode( IBaseFilter* pBaseFilter, VMRMode mode )
{

    IVMRFilterConfig* pConfig;
    HRESULT hr = pBaseFilter->QueryInterface(IID_IVMRFilterConfig,
        (LPVOID *)&pConfig);
    if(SUCCEEDED(hr))
    {
        pConfig->SetRenderingMode(mode);
		  pConfig->SetRenderingPrefs(RenderPrefs_AllowOverlays);
		  
		  if ( hr!=S_OK)
		  {
			  int i=0;
			  switch( hr)
			  {
				case E_FAIL: i=1; break;
				case E_INVALIDARG: i=2; break;
				case E_OUTOFMEMORY: i=3; break;

			  }
		  }
        pConfig->Release();
    }
    return hr;
}


/******************************Public*Routine******************************\
* AddVideoMixingRendererToFG
*
\**************************************************************************/
HRESULT
CMpegMovie::AddVideoMixingRendererToFG()
{
	if ( m_Vw ) {

		return FALSE;

	}	else {


    IBaseFilter* pBF = NULL;
    HRESULT hRes = CoCreateInstance(CLSID_VideoMixingRenderer,
        NULL,
        CLSCTX_INPROC,
        IID_IBaseFilter,
        (LPVOID *)&pBF);


    if(SUCCEEDED(hRes))
    {
        hRes = m_Fg->AddFilter(pBF, L"Video Mixing Renderer");

        if(SUCCEEDED(hRes))
        {
				hRes = SetRenderingMode(pBF, VMRMode_Renderless);
				
				hRes = pBF->QueryInterface(IID_IVMRSurfaceAllocatorNotify,
							(LPVOID *)&m_San);

				hRes = CreateDefaultAllocatorPresenter();
				hRes = m_San->AdviseSurfaceAllocator(MY_USER_ID, this);

        }
    }

    if(pBF)
    {
        pBF->Release();
    }

    if(SUCCEEDED(hRes))
    {
        HRESULT hr = m_Wc->SetVideoClippingWindow(m_hwndApp);
        hr = m_Wc->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
    }
    else
    {
        if(m_Wc)
        {
            m_Wc->Release();
            m_Wc = NULL;
        }
    }

    return hRes;

	}
}

/******************************Public*Routine******************************\
* OpenMovie
*
\**************************************************************************/
HRESULT
CMpegMovie::OpenMovie(
    TCHAR *lpFileName, BOOL bWin9xCompatible
    )
{
    USES_CONVERSION;
    IUnknown        *pUnk;
    HRESULT         hres;
    WCHAR           FileName[MAX_PATH];

    wcscpy(FileName, T2W(lpFileName));

    hres = CoInitialize(NULL);
    if(hres == S_FALSE)
        CoUninitialize();

	if ( bWin9xCompatible || !VerifyVMR() ) {

		//----------------------------------------------------
		// If video must be compatible with Win9x or 
		//  VMR don't exist on the system use old style movie.
		//----------------------------------------------------


		// Get the interface for DirectShow's GraphBuilder
		hres = CoCreateInstance( CLSID_FilterGraph, 
								NULL, CLSCTX_INPROC_SERVER, 
                         IID_IGraphBuilder, (void **)&m_Gb );

		if( SUCCEEDED(hres) )
		{
			// Have the graph builder construct its the appropriate graph automatically
			hres = m_Gb->RenderFile(FileName, NULL);
			if ( FAILED(hres) ) {
				m_Gb->Release(); m_Gb = NULL;
				return hres;
			}

			// QueryInterface for DirectShow interfaces
			hres = m_Gb->QueryInterface(IID_IMediaControl, (void **)&m_Mc);
			if ( FAILED(hres) ) {
				m_Mc->Release(); m_Mc = NULL;
				m_Gb->Release(); m_Gb = NULL;
				return hres;
			}

			hres = m_Gb->QueryInterface(IID_IMediaEventEx, (void **)&m_Me);
			if (m_Me) m_Me->SetNotifyWindow(NULL, 0, 0);

			hres = m_Gb->QueryInterface(IID_IMediaSeeking, (void **)&m_Ms);
			if ( FAILED(hres) ) {
				m_Ms->Release(); m_Ms = NULL;
				m_Me->Release(); m_Me = NULL;
				m_Mc->Release(); m_Mc = NULL;
				m_Gb->Release(); m_Gb = NULL;
				return hres;
			}

			// Query for video interfaces, 
			//  which may not be relevant for audio files.
			hres = m_Gb->QueryInterface(IID_IVideoWindow, (void **)&m_Vw);
			if ( FAILED(hres) ) {
				m_Vw = NULL;
			}
			hres = m_Gb->QueryInterface(IID_IBasicVideo, (void **)&m_Bv);
			if ( FAILED(hres) ) {
				m_Bv = NULL;
			}
			
		
			// Query for audio interfaces, which may not be relevant for video-only files
			hres = m_Gb->QueryInterface(IID_IBasicAudio, (void **)&m_Ba);
			if ( FAILED(hres) ) {
				m_Ba = NULL;
			}


			if ( m_Vw )
			{
				m_Vw->put_Visible(OAFALSE);
				m_Vw->put_AutoShow(OAFALSE);
            m_Vw->put_Owner((OAHWND)m_hwndApp);
				m_Vw->put_MessageDrain((OAHWND)m_hwndApp);
            m_Vw->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS 
					| WS_CLIPCHILDREN );

			}

			m_bWin9xCompatible = TRUE;			

		}

		return hres;


	} else {

		//------------------------------------------
		//We can play using new WinXp WMR technique.
		//------------------------------------------

    hres = CoCreateInstance(
        CLSID_FilterGraph,
        NULL,
		  CLSCTX_INPROC_SERVER,
        IID_IUnknown,
        (LPVOID *)&pUnk);

    if(SUCCEEDED(hres))
    {
        m_Mode = MOVIE_OPENED;
        hres = pUnk->QueryInterface(IID_IFilterGraph, (LPVOID *)&m_Fg);
        if(FAILED(hres))
        {
            pUnk->Release();
            return hres;
        }

        hres = AddVideoMixingRendererToFG();
        if(FAILED(hres))
        {
            m_Fg->Release(); m_Fg = NULL;
            return hres;
        }

        hres = pUnk->QueryInterface(IID_IGraphBuilder, (LPVOID *)&m_Gb);
        if(FAILED(hres))
        {
            pUnk->Release();
            m_Fg->Release(); m_Fg = NULL;
            m_Wc->Release(); m_Wc = NULL;
            return hres;
        }

        hres = m_Gb->RenderFile(FileName, NULL);
        if(FAILED(hres))
        {
            pUnk->Release();
            m_Fg->Release(); m_Fg = NULL;
            m_Wc->Release(); m_Wc = NULL;
            m_Gb->Release(); m_Gb = NULL;
            return hres;
        }

        hres = pUnk->QueryInterface(IID_IMediaControl, (LPVOID *)&m_Mc);
        if(FAILED(hres))
        {
            pUnk->Release();
            m_Fg->Release(); m_Fg = NULL;
            m_Wc->Release(); m_Wc = NULL;
            m_Gb->Release(); m_Gb = NULL;
            return hres;
        }

		  // Query audio control
		  hres = m_Gb->QueryInterface(IID_IBasicAudio, (LPVOID *)&m_Ba);
		  if (FAILED(hres))
		  {
			  m_Ba = NULL;
		  }

        //
        // Not being able to get the IMediaEvent interface doesn't
        // necessarly mean that we can't play the graph.
        //
        pUnk->QueryInterface(IID_IMediaEventEx, (LPVOID *)&m_Me);
			if (m_Me) m_Me->SetNotifyWindow(NULL, 0, 0);

        pUnk->QueryInterface(IID_IMediaSeeking, (LPVOID *)&m_Ms);
        pUnk->Release();

		  m_bWin9xCompatible = FALSE;

        return S_OK;
    }
    else
    {
        m_Fg = NULL;
    }

    return hres;

	}
}


/******************************Public*Routine******************************\
* CloseMovie
*
\**************************************************************************/
DWORD//
CMpegMovie::CloseMovie(
    )
{
	m_Mode = MOVIE_NOTOPENED;

	if ( m_bWin9xCompatible ) {

		ShowMovie( FALSE );


		if(m_Mc)
		{
			if(m_Me)
			{
				m_Me->SetNotifyWindow(NULL, 0, 0);
            m_Me->Release();
            m_Me = NULL;
			}

			if(m_Ms)
			{
            m_Ms->Release();
            m_Ms = NULL;
			}


			if (m_Bv)
			{
				m_Bv->Release();
				m_Bv = NULL;
			}

			if (m_Vw)
			{
				m_Vw->put_Owner(NULL);
				m_Vw->put_MessageDrain(NULL);
				m_Vw->Release();
				m_Vw = NULL;
			}

			if (m_Ba)
			{
				m_Ba->Release();
				m_Ba = NULL;
			}


			m_Mc->Release();
			m_Mc = NULL;

			if(m_Gb)
			{
				m_Gb->Release();
            m_Gb = NULL;
			}

		}

		QzUninitialize();
		return 0L;

	}	else {

		if ( m_pDdcc ) m_pDdcc->Release();

		if(m_Mc)
		{
			if(m_Me)
			{
				m_Me->SetNotifyWindow(NULL, 0, 0);
            m_Me->Release();
            m_Me = NULL;
			}

        if(m_Ms)
        {
            m_Ms->Release();
            m_Ms = NULL;
        }

        if(m_Wc)
        {
            m_Wc->Release();
            m_Wc = NULL;
        }

 			if (m_Ba)
			{
				m_Ba->Release();
				m_Ba = NULL;
			}


        m_Mc->Release();
        m_Mc = NULL;

        if(m_Gb)
        {
            m_Gb->Release();
            m_Gb = NULL;
        }

        if(m_Fg)
        {
            m_Fg->Release();
            m_Fg = NULL;
        }
    }

    QzUninitialize();
    return 0L;

	}
}


/******************************Public*Routine******************************\
* CMpegMovie::GetNativeMovieSize
*
\**************************************************************************/
BOOL
CMpegMovie::GetNativeMovieSize(
    LONG *pcx,
    LONG *pcy
    )
{
	BOOL    bRet = FALSE;

	if ( m_Vw ) {


		bRet = ( m_Bv->GetVideoSize( pcx, pcy ) == S_OK );

	}	else {


    
    if(m_Wc)
    {
        bRet = (m_Wc->GetNativeVideoSize(pcx, pcy, NULL, NULL) == S_OK);
    }


	}

   return bRet;
}


/******************************Public*Routine******************************\
* GetMoviePosition
*
\**************************************************************************/
BOOL
CMpegMovie::GetMoviePosition(
    LONG *px,
    LONG *py,
    LONG *pcx,
    LONG *pcy
    )
{
    BOOL    bRet = FALSE;

	if ( m_Vw ) {

		bRet = ( m_Vw->GetWindowPosition( px, py, pcx, pcy) == S_OK );


	}	else {


		if(m_Wc)
		{
			RECT src={0}, dest={0};
			bRet = ( m_Wc->GetVideoPosition(&src, &dest) == S_OK );
			*px = dest.left;
			*py = dest.right;
			*pcx = dest.right - dest.left;
			*pcy = dest.bottom - dest.top;
		}

	}
	return bRet;
}

/******************************Public*Routine******************************\
* PutMoviePosition
*
\**************************************************************************/
BOOL
CMpegMovie::PutMoviePosition(
    LONG x,
    LONG y,
    LONG cx,
    LONG cy
    )
{
	BOOL    bRet = FALSE;

	if ( m_Vw || m_Wc )
	{
		RECT rcDest, rcSource;

		LONG moviex, moviey;
		GetNativeMovieSize( &moviex, &moviey );
		SetRect( &rcSource, 0,0,moviex,moviey);

					// Calculate valid aspect ratio of movie.
		double	ww=cx, wh=cy,	//window width,height
					dw=cx, dh=cy, dOffX=0, dOffY=0, dMinOffX, dMinOffY,	//dest rect 
					sw=moviex, sh=moviey, sOffX=0, sOffY=0,	//source rect
					ars = sw/sh, arw = ww/wh,
					val = m_valPanScan;

		if ( ars < arw ) { dw = wh*ars; dOffX = (ww-dw)/2; } 
		if ( ars > arw ) { dh = ww/ars; dOffY = (wh-dh)/2; }

		SetRect(&rcDest,(long)dOffX+x,(long)dOffY+y,
			(long)(dw+dOffX)+x, (long)(dh+dOffY)+y );

		if ( m_bPanScan ) {

			if ( val < 0 ) {

				if ( val == -169.0 )	//16:9
				{
					double ar = 16.0/9.0, h=(sw/ar), 
						screenh = GetSystemMetrics(SM_CYSCREEN ); 
					if ( h> screenh ) h=screenh;
					if (h>sh) {
						val = ((h/sh)-1)*100.0;
						if (val>100.0) val = 100.0;
					}
				}

			}

			if ( val>0.0 && val <= 100.0 )
			{

				// Calculate PanScan
				dMinOffX = -dw*val/100.0/2; dMinOffY = -dh*val/100.0/2; 
				dw += dw*val/100.0; dh += dh*val/100.0;
				dOffX = (ww-dw)/2; dOffY = (wh-dh)/2;

				if ( dOffX<0 ) {
					double d = val;
					if ( dMinOffX<dOffX ) d = (dOffX/dMinOffX*val);

					sOffX = (sw*d/100.0/2.0); sw -= (sw*d/100.0);
					
					dw += (dOffX*2); dOffX = 0;
				}
				if ( dOffY<0 ) {
					double d = val;
					if ( dMinOffY<dOffY ) d = (dOffY/dMinOffY*val);

					sOffY = (sh*d/100.0/2.0); sh -= (sh*d/100.0);

					dh += (dOffY*2); dOffY = 0;
				}

				SetRect( &rcSource, (int)sOffX, (int)sOffY, 
					(int)(sw+sOffX), (int)(sh+sOffY) );

				SetRect(&rcDest,(long)dOffX+x,(long)dOffY+y,
					(long)(dw+dOffX)+x, (long)(dh+dOffY)+y );

				SetRect( &m_rcMovieWindow, x, y, x+cx, y+cy );
				m_rcMovieDest = rcDest;

			} else if ( val==0  ){ //OK!!!
				// When m_valPanScan==0 should be automatic PanScan
				//  (movie is zoomed to whole dest rectangle with
				//	  correct aspect ratio).

				double mx=(double)moviex, my=(double)moviey;

				double xxx = 0, armovie = mx/my, 
					ardest = (double)cx/(double)cy,
					dx=0, dy=0;

				if ( armovie>ardest ) {
					xxx = armovie/ardest;
					dx = ( mx- (mx/xxx) )/2.0;
					
				}
				if ( ardest>armovie ) {
					xxx = ardest/armovie;						
					dy = ( my-(my/xxx))/2.0;
				}

				SetRect( &rcSource, (int)dx, (int)dy, 
					(int)(mx-dx), (int)(my-dy) );

				SetRect(&rcDest, x, y, x + cx, y + cy);
			}
		}


		if ( m_Vw ) {
			

			bRet = (m_Vw->SetWindowPosition(x, y, cx, cy) == S_OK );

			m_Bv->SetDestinationPosition( rcDest.left-x, rcDest.top-y,
				rcDest.right-rcDest.left, rcDest.bottom-rcDest.top );

			m_Bv->SetSourcePosition( rcSource.left, rcSource.top,
				rcSource.right-rcSource.left, rcSource.bottom-rcSource.top );

			m_Vw->put_BorderColor(0);
			m_Vw->put_Visible(OATRUE);

		} else if ( m_Wc ) {

			m_Wc->SetBorderColor(0);
			m_Wc->SetAspectRatioMode(VMR_ARMODE_NONE);
			bRet = (m_Wc->SetVideoPosition(&rcSource, &rcDest) == S_OK);

			SetRect( &m_rcMovieWindow, x, y, x+cx, y+cy );
			m_rcMovieDest = rcDest;
		}

		SetRect( &m_rcMovieWindow, x, y, x+cx, y+cy );
		m_rcMovieDest = rcDest;

	}

	return bRet;
}


/******************************Public*Routine******************************\
* PlayMovie
*
\**************************************************************************/
BOOL
CMpegMovie::PlayMovie(
    )
{

	REFTIME rt, rtAbs, rtDur;
	HRESULT hr = S_OK;

	rt = GetCurrentPosition();
   rtDur = GetDuration();

   //
   // If we are near the end of the movie seek to the start, otherwise
   // stay where we are.
   //
   rtAbs = rt - rtDur;
   if(rtAbs < (REFTIME)0)
   {
		rtAbs = -rtAbs;
   }

   if(rtAbs < (REFTIME)1)
   {
		SeekToPosition((REFTIME)0,FALSE, FALSE);
	}

	//
	// Change mode after setting m_Mode but before starting the graph
	//
	m_Mode = MOVIE_PLAYING;
	hr = m_Mc->Run();

	return (hr==S_OK);
}


/******************************Public*Routine******************************\
* PauseMovie
*
\**************************************************************************/
BOOL
CMpegMovie::PauseMovie(
    )
{

	m_Mode = MOVIE_PAUSED;

	HRESULT hr = m_Mc->Pause();

	return TRUE;
}


/******************************Public*Routine******************************\
* GetStateMovie
*
\**************************************************************************/

OAFilterState
CMpegMovie::GetStateMovie(
    )
{

    OAFilterState State;

    HRESULT hr = m_Mc->GetState(INFINITE,&State);
    return State;

}


/******************************Public*Routine******************************\
* StopMovie
*
\**************************************************************************/
BOOL
CMpegMovie::StopMovie(
    )
{

    m_Mode = MOVIE_STOPPED;
    HRESULT hr = m_Mc->Stop();
    return TRUE;

}


/******************************Public*Routine******************************\
* StatusMovie
*
\**************************************************************************/
EMpegMovieMode
CMpegMovie::StatusMovie(
    )
{
   if(m_Mc)
	{
		FILTER_STATE    fs;
      HRESULT         hr;

		hr = m_Mc->GetState(100, (OAFilterState *)&fs);

		// Don't know what the state is so just stay at old state.
		if(hr == VFW_S_STATE_INTERMEDIATE)
		{
			return m_Mode;
		}

		switch(fs)
		{
			case State_Stopped:
				m_Mode = MOVIE_STOPPED;
				break;

			case State_Paused:
				m_Mode = MOVIE_PAUSED;
				break;

			case State_Running:
				m_Mode = MOVIE_PLAYING;
				break;
		}
	}

	return m_Mode;
}


/******************************Public*Routine******************************\
* CanMovieFrameStep
*
\**************************************************************************/
BOOL
CMpegMovie::CanMovieFrameStep()
{
	if ( m_Vw ) {

		return E_FAIL;

	}	else {

		IVideoFrameStep* lpFS;
		HRESULT hr;

		hr = m_Fg->QueryInterface(__uuidof(IVideoFrameStep), (LPVOID *)&lpFS);
		if(SUCCEEDED(hr))
		{
			hr = lpFS->CanStep(0L, NULL);
			lpFS->Release();
		}

		return SUCCEEDED(hr);
	}
}


/******************************Public*Routine******************************\
* FrameStepMovie
*
\**************************************************************************/
BOOL
CMpegMovie::FrameStepMovie()
{
	if ( m_Vw ) {

		return E_FAIL;

	}	else {


    IVideoFrameStep* lpFS;
    HRESULT hr;

    hr = m_Fg->QueryInterface(__uuidof(IVideoFrameStep), (LPVOID *)&lpFS);
    if(SUCCEEDED(hr))
    {
        hr = lpFS->Step(1, NULL);
        lpFS->Release();
    }

    return SUCCEEDED(hr);
	}
}

/******************************Public*Routine******************************\
* SetNotifyWindow
*
\**************************************************************************/
BOOL
CMpegMovie::SetNotifyWindow( HWND hwnd, long lMsg )
{
	HRESULT hr;
	if ( m_Me )
	{
		hr = m_Me->SetNotifyWindow((OAHWND)hwnd, lMsg, 0);

		return (hr==S_OK);
	}

	return FALSE;
}


/******************************Public*Routine******************************\
* GetMovieEventCode
*
\**************************************************************************/
long
CMpegMovie::GetMovieEventCode()
{
	HRESULT hr;
	long    lEventCode;
   LONG_PTR    lParam1, lParam2;

   if(m_Me != NULL)
   {
		hr = m_Me->GetEvent(&lEventCode, &lParam1, &lParam2, 0);
      if(SUCCEEDED(hr))
      {
			hr = m_Me->FreeEventParams(lEventCode, lParam1, lParam2);
         return lEventCode;
      }
	}

	return 0L;
}


/******************************Public*Routine******************************\
* GetDuration
*
* Returns the duration of the current movie
*
\**************************************************************************/
REFTIME
CMpegMovie::GetDuration()
{

	HRESULT hr;
	LONGLONG Duration;

	// Should we seek using IMediaSelection
	if(m_TimeFormat != TIME_FORMAT_MEDIA_TIME)
	{
		hr = m_Ms->GetDuration(&Duration);
		if(SUCCEEDED(hr))
		{
			return double(Duration);
		}
   }
	else if(m_Ms != NULL)
	{
		hr = m_Ms->GetDuration(&Duration);
		if(SUCCEEDED(hr))
		{
			return double(Duration) / UNITS;
		}
	}

	return 0;
}


/******************************Public*Routine******************************\
* GetCurrentPosition
*
* Returns the duration of the current movie
*
\**************************************************************************/
REFTIME
CMpegMovie::GetCurrentPosition()
{
	REFTIME rt = (REFTIME)0;
   HRESULT hr;
   LONGLONG Position;

	// Should we return a media position
	if(m_TimeFormat != TIME_FORMAT_MEDIA_TIME)
	{
		hr = m_Ms->GetPositions(&Position, NULL);
		if(SUCCEEDED(hr))
		{
			return double(Position);
		}
	}
	else if(m_Ms != NULL)
	{
		hr = m_Ms->GetPositions(&Position, NULL);
		if(SUCCEEDED(hr))
		{
			return double(Position) / UNITS;
	   }
	}

	return rt;
}


/*****************************Private*Routine******************************\
* SeekToPosition
*
\**************************************************************************/
BOOL
CMpegMovie::SeekToPosition(
    REFTIME rt,
    BOOL bFlushData,
	 BOOL bSeekToKeyFrame
    )
{
	HRESULT hr;
   LONGLONG llTime = LONGLONG(m_TimeFormat == TIME_FORMAT_MEDIA_TIME ? rt * double(UNITS) : rt );

   if(m_Ms != NULL)
   {
		FILTER_STATE fs;
		hr = m_Mc->GetState(100, (OAFilterState *)&fs);

		int dwFlags = AM_SEEKING_AbsolutePositioning;
		if ( bSeekToKeyFrame) dwFlags |= AM_SEEKING_SeekToKeyFrame;

		hr = m_Ms->SetPositions(&llTime, dwFlags, NULL, 0);

		// This gets new data through to the renderers
		if(fs == State_Stopped && bFlushData)
		{
			hr = m_Mc->Pause();
			hr = m_Mc->GetState(INFINITE, (OAFilterState *)&fs);
			hr = m_Mc->Stop();
		}

		if(SUCCEEDED(hr))
		{
			return TRUE;
		}
	}

	return FALSE;
}

/*****************************Public*Routine******************************\
* IsTimeFormatSupported
*
\**************************************************************************/
BOOL
CMpegMovie::IsTimeFormatSupported(GUID Format)
{

	return m_Ms != NULL && m_Ms->IsFormatSupported(&Format) == S_OK;

}


/*****************************Public*Routine******************************\
* IsTimeSupported
*
\**************************************************************************/
BOOL
CMpegMovie::IsTimeSupported()
{

	return m_Ms != NULL && m_Ms->IsFormatSupported(&TIME_FORMAT_MEDIA_TIME) == S_OK;

}


/*****************************Public*Routine******************************\
* GetTimeFormat
*
\**************************************************************************/
GUID
CMpegMovie::GetTimeFormat()
{
	return m_TimeFormat;
}

/*****************************Public*Routine******************************\
* SetTimeFormat
*
\**************************************************************************/
BOOL
CMpegMovie::SetTimeFormat(GUID Format)
{

	HRESULT hr = m_Ms->SetTimeFormat(&Format);
	if(SUCCEEDED(hr))
	{
		m_TimeFormat = Format;
	}

	return SUCCEEDED(hr);
}

/******************************Public*Routine******************************\
* SetFocus
*
\**************************************************************************/
void
CMpegMovie::SetFocus()
{
	if ( m_Vw ) {



	}	else {


    if(m_Fg)
    {
        // Tell the resource manager that we are being made active.  This
        // will then cause the sound to switch to us.  This is especially
        // important when playing audio only files as there is no other
        // playback window.
        IResourceManager* pResourceManager;

        HRESULT hr = m_Fg->QueryInterface(IID_IResourceManager, (void**)&pResourceManager);

        if(SUCCEEDED(hr))
        {
            IUnknown* pUnknown;

            hr = m_Fg->QueryInterface(IID_IUnknown, (void**)&pUnknown);

            if(SUCCEEDED(hr))
            {
                hr = pResourceManager->SetFocus(pUnknown);
                pUnknown->Release();
            }

            pResourceManager->Release();
        }
    }
	}
}


/******************************Public*Routine******************************\
* RepaintVideo
*
\**************************************************************************/
BOOL
CMpegMovie::RepaintVideo(
    HWND hwnd,
    HDC hdc
    )
{

	BOOL bRet = FALSE;
	
	long lnativeWidth=0, lnativeHeight=0;
	GetNativeMovieSize( &lnativeWidth, &lnativeHeight );
	if ( !lnativeWidth || !lnativeHeight )
	{
		// No picture, sound only.
		HDC hDC = GetDC( hwnd );

		RECT *prc = &m_rcMovieWindow;
		BitBlt( hDC,  prc->left, prc->top, RECTW((*prc)), RECTH((*prc)),
			NULL, 0, 0, BLACKNESS );

		ReleaseDC( hwnd, hDC );

		return TRUE;
	}

	if ( m_Vw ) {

		// Windowed movie playback don't need repaint.

		return TRUE;

	}	else if( m_Wc ) {


		bRet = (m_Wc->RepaintVideo(hwnd, hdc) == S_OK);

		HDC hDC = GetDC( hwnd );
		int mx = m_rcMovieWindow.left, my = m_rcMovieWindow.top,
			 mw = m_rcMovieWindow.right - mx, mh = m_rcMovieWindow.bottom - my,
			 dx = m_rcMovieDest.left, dy = m_rcMovieDest.top,
			 dw = m_rcMovieDest.right - dx, dh = m_rcMovieDest.bottom - dy;

		int x=0,y=0,w=0,h=0;

		x = mx; y= my; w = mw; h = dy-my;
		BitBlt(hDC, x,y,w,h, NULL, 0,0, BLACKNESS);

		x = mx; y= dy+dh; w = mw; h = my+mh-y;
		BitBlt(hDC, x,y,w,h, NULL, 0,0, BLACKNESS);


		x=mx; y=my; w=dx-mx; h = mh;
		BitBlt(hDC, x,y,w,h, NULL, 0,0, BLACKNESS);

		x=dx+dw; y=my; w=mx+mw-x; h = mh;
		BitBlt(hDC, x,y,w,h, NULL, 0,0, BLACKNESS);

		ReleaseDC( hwnd, hDC );

	}

	return bRet;
		
}


/******************************Public*Routine******************************\
* SetAppImage
* DON'T WORK IN RENDERLESS MODE
\**************************************************************************/
BOOL
CMpegMovie::SetAppImage(
    VMRALPHABITMAP* lpBmpInfo
    )
{
	if ( m_Vw ) {

		// Windowed movie playback can't have app image.
		return FALSE;

	}	else {


		IVMRMixerBitmap* pBmp;
		HRESULT hres = m_Wc->QueryInterface(IID_IVMRMixerBitmap, (LPVOID *)&pBmp);
		if(SUCCEEDED(hres))
		{
			hres = pBmp->SetAlphaBitmap(lpBmpInfo);
			pBmp->Release();
		}
		else
		{
			char sz[256]; 
			AMGetErrorText(hres, sz, 256);
		}

		return hres;
	}
}


/******************************Public*Routine******************************\
* UpdateAppImage
* DON'T WORK IN RENDERLESS MODE
\**************************************************************************/
BOOL//
CMpegMovie::UpdateAppImage(VMRALPHABITMAP* lpBmpInfo)
{
	if ( m_Vw ) {

		// Windowed movie playback can't have app image.
		return FALSE;

	}	else {


		IVMRMixerBitmap* pBmp;

		if (!m_Wc)
			return FALSE;

		HRESULT hres = m_Wc->QueryInterface(IID_IVMRMixerBitmap, (LPVOID *)&pBmp);
		if(SUCCEEDED(hres))
		{
			hres = pBmp->UpdateAlphaBitmapParameters(lpBmpInfo);
			pBmp->Release();
		}

		return hres;

	}
}

void 
CMpegMovie::DisplayModeChanged() 
{
	if ( m_bWin9xCompatible )
	{
		// Don't need DISPLAY_MODE_CHANGED.
	}
	else
	{
		if ( m_Wc ) m_Wc->DisplayModeChanged();
	}
}

HRESULT 
CMpegMovie::GetCurrentImage(LPBYTE* lplpDib)
{
	if ( m_bWin9xCompatible )
	{	
		// Need lot of work to do.
	}
	else
	{
		if ( m_Wc ) return m_Wc->GetCurrentImage(lplpDib);
	}

	return NULL;
}


/******************************Public*Routine******************************\
* SetVolume
*
\**************************************************************************/
BOOL
CMpegMovie::SetVolume( double dVolume )
{
	if ( !m_Ba || m_bMute ) return FALSE;

	dVolume = (100.0-dVolume)*(100.0-dVolume)*0.5;
	if ( dVolume > 4500.0 ) dVolume = (dVolume -4500.0) * 10 + dVolume;

	long lVolume = (long)-dVolume;

	if ( lVolume > 0 ) lVolume = 0;
	if ( lVolume < -10000 ) lVolume = -10000;

	// 0 is full volume, -10000 is silence
	m_Ba->put_Volume( lVolume );


	return TRUE;
}

/******************************Public*Routine******************************\
* SetMute
*
\**************************************************************************/
BOOL
CMpegMovie::SetMute( BOOL bMute, double dVolume )
{
	if ( !m_Ba ) return FALSE;

	if ( bMute && !m_bMute ) {
		m_Ba->put_Volume( -10000 );
		m_bMute = TRUE;

	} else 
	if ( !bMute && m_bMute ) {

		m_bMute = FALSE;
		SetVolume( dVolume );

	}

	return TRUE;
}

/******************************Public*Routine******************************\
* SetMovieWindow
*
\**************************************************************************/
void
CMpegMovie::SetMovieWindow( HWND hwndApplication )
{
	if ( !hwndApplication ) return;

	if ( m_Vw ) {

		m_Vw->put_Owner((OAHWND)hwndApplication);
		m_Vw->put_MessageDrain((OAHWND)hwndApplication);
      m_Vw->put_WindowStyle( WS_CHILD  | WS_CLIPSIBLINGS 
					| WS_CLIPCHILDREN );

		m_hwndApp = hwndApplication;

	}	else {

		m_Wc->SetVideoClippingWindow( hwndApplication );
		m_hwndApp = hwndApplication;

	}
}

/******************************Public*Routine******************************\
* IsWin9xCompatible
*
\**************************************************************************/
BOOL
CMpegMovie::IsWin9xCompatible()
{
	return m_bWin9xCompatible;
}

/******************************Public*Routine******************************\
* ShowMovie
*
\**************************************************************************/
void
CMpegMovie::ShowMovie( BOOL bShow )
{
	if ( m_Vw )
	{
		bShow = bShow?OATRUE:OAFALSE;
		if ( m_Vw ) m_Vw->put_Visible( bShow );
	}
	else
	{
		// Can't hide.
	}

}

/******************************Public*Routine******************************\
* ShowMovie
*
\**************************************************************************/
void CMpegMovie::EnablePanScan( BOOL bEnable )
{
	m_bPanScan = bEnable;
}
void CMpegMovie::SetPanScanValue( double val )
{
	if ( (val>=0 && val<=100) || val==-169 ) m_valPanScan = val;
}
double CMpegMovie::GetPanScanValue()
{
	return m_valPanScan;
}


BOOL CMpegMovie::SetColorControl( int valBrightness, 
						int valContrast, int valSaturation )
{
	if ( m_pDdcc )
	{
		// IRE = International Radio Engineers scale

		DDCOLORCONTROL ddcc = { sizeof(DDCOLORCONTROL), 0 };

		if ( valBrightness>=0 && valBrightness<=100)
		{
			// Luminance intensity, in IRE units times 100. The valid range 
			//  is from 0 through 10,000. The default is 750, 
			//  which translates to 7.5 IRE.
			ddcc.dwFlags |= DDCOLOR_BRIGHTNESS;
			ddcc.lBrightness = valBrightness*100;
			
		}

		if ( valContrast>=0 && valContrast<=100 )
		{
			// Relative difference between higher and lower intensity 
			//  luminance values in IRE units times 100. The valid range 
			//  is from 0 through 20,000. The default value is 10,000 (100 IRE).
			ddcc.dwFlags |= DDCOLOR_CONTRAST;
			ddcc.lContrast = valContrast*200;
		}

		if ( valSaturation>=0 && valSaturation<=100 )
		{
			// Color intensity, in IRE units times 100. The valid range 
			//  is from 0 through 20,000. The default value is 10,000, 
			//  which translates to 100 IRE.
			ddcc.dwFlags |= DDCOLOR_SATURATION;
			ddcc.lSaturation = valSaturation*200;
		}

		
		m_pDdcc->SetColorControls( &ddcc );

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/*****************************Private*Routine******************************\
* SetBorderClr
*
\**************************************************************************/
void
CMpegMovie::SetBorderClr(COLORREF clr)
{
	if ( m_Vw ) {

		m_Vw->put_BorderColor(clr);

	}	else {

		m_Wc->SetBorderColor(clr);
	}
}

double CMpegMovie::GetFPS()
{
	double fps = 25.0;

	GUID guid; m_Ms->GetTimeFormat( &guid );

	LONGLONG duration_frames, duration_time;

	m_Ms->SetTimeFormat( &TIME_FORMAT_FRAME );
	m_Ms->GetDuration( &duration_frames );

	m_Ms->SetTimeFormat( &TIME_FORMAT_MEDIA_TIME );
	m_Ms->GetDuration( &duration_time );

	double dur_secs = (double)duration_time/10000000,
			dur_frames = (double)duration_frames;

	fps = dur_frames/dur_secs;

	m_Ms->SetTimeFormat( &guid );

	return fps;
}


/*****************************Private*Routine******************************\
* VerifyVMR
*
\**************************************************************************/
BOOL VerifyVMR(void)
{
    HRESULT hres;

    // Verify that the VMR exists on this system
    IBaseFilter* pBF = NULL;
    hres = CoCreateInstance(CLSID_VideoMixingRenderer,
        NULL,
        CLSCTX_INPROC,
        IID_IBaseFilter,
        (LPVOID *)&pBF);

	if(SUCCEEDED(hres))
	{
		pBF->Release();
		return TRUE;
	}
    else
		return FALSE;
}

DWORD CMpegMovie::GetColorKey()
{
	DWORD col = 0x000000;


	return col;
}

DWORD CMpegMovie::SetColorKey( DWORD col )
{

	return 0x100010;
}
