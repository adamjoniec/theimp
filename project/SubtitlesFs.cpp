// SubtitlesFs.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "SubtitlesFs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSubtitlesFs

CSubtitlesFs::CSubtitlesFs():
					pSubtitlesMode(NULL),
					phbmBackground(NULL)
{
}

CSubtitlesFs::~CSubtitlesFs()
{
}


BEGIN_MESSAGE_MAP(CSubtitlesFs, CWnd)
	//{{AFX_MSG_MAP(CSubtitlesFs)
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSubtitlesFs message handlers

BOOL CSubtitlesFs::Create(CWnd* pParentWnd) 
{

	char *pszClassName = (char*)AfxRegisterWndClass( 
				0, 0, NULL, 
				LoadIcon( AfxGetInstanceHandle(), 
					MAKEINTRESOURCE(IDR_MAINFRAME) ) );	

	RECT rc = {0,0,0,0};
	BOOL ret = CWnd::CreateEx( 0, pszClassName,"Subtitles Fs",
		WS_POPUP,rc, pParentWnd, 0 );

	return ret;
}

void CSubtitlesFs::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if ( !pSubtitlesMode || !phbmBackground )
		return;
	
	if ( *pSubtitlesMode==SUBTITLES_OVERLAY || *pSubtitlesMode==SUBTITLES_RGN )
	{
		if ( !*phbmBackground ) return;

		SIZE size; GetBitmapDimensionEx( *phbmBackground, &size );
		HDC hdcSource = CreateCompatibleDC( dc.m_hDC );
		::SelectObject( hdcSource, *phbmBackground );
		::BitBlt( dc.m_hDC, 0,0, size.cx, size.cy,
			hdcSource, 0,0, SRCCOPY );
		::DeleteDC( hdcSource );

	}	
	
	// Do not call CWnd::OnPaint() for painting messages
}

LRESULT CSubtitlesFs::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND hWndParent = GetParent()->m_hWnd;

	switch ( message )
	{
		case WM_MOUSEWHEEL:
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:

			::PostMessage( hWndParent, message, wParam, lParam );
			break;

		case WM_MOUSEMOVE:
			int x,y; 
			RECT rc, rcParent; 
			::GetWindowRect( hWndParent, &rcParent ); GetWindowRect( &rc );
			
			x = (rc.left-rcParent.left) + LOWORD(lParam); 
			y = (rc.top-rcParent.top) + HIWORD(lParam);

			lParam = x + (y<<16);
			::PostMessage( hWndParent, message, wParam, lParam );

			break;

	}
	
	return CWnd::DefWindowProc(message, wParam, lParam);
}

void CSubtitlesFs::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	
	if ( nState!=WA_INACTIVE )
	{
		pWndOther->SetWindowPos( &wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE );
	}
	
}
