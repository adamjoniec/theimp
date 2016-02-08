// SplashScreen.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "SplashScreen.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashScreen

CSplashScreen::CSplashScreen()
{
}

CSplashScreen::~CSplashScreen()
{
}


BEGIN_MESSAGE_MAP(CSplashScreen, CWnd)
	//{{AFX_MSG_MAP(CSplashScreen)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSplashScreen message handlers

BOOL CSplashScreen::Create() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	HMODULE hModule = AfxGetApp()->m_hInstance;

	HRSRC hRsrc = FindResource( hModule, MAKEINTRESOURCE(IDR_SETUP), "RAWDATA" );
	HGLOBAL hResData = LoadResource( hModule, hRsrc );
	DWORD size = SizeofResource( hModule, hRsrc );
	char *png = (char*)LockResource( hResData );

	pimg = new cGImage();
	pimg->LoadImage( png, size );
	pimg->ConvertToBitmap( ::GetDesktopWindow() );

	int sw = GetSystemMetrics( SM_CXSCREEN ),
		sh = GetSystemMetrics( SM_CYSCREEN ),
		x = (sw-pimg->w)/2, y = (sh-pimg->h)/2;

	BOOL ret = CWnd::CreateEx( 0, AfxRegisterWndClass( 0 ),
		"Splash screen", WS_POPUP|WS_VISIBLE, x,y, pimg->w, pimg->h, NULL, 0 );

	return ret;
}

BOOL CSplashScreen::OnEraseBkgnd(CDC* pDC) 
{
	HDC hdcSrc = CreateCompatibleDC( pDC->m_hDC );
	::SelectObject( hdcSrc, pimg->hBitmap );
	
	::BitBlt( pDC->m_hDC, 0,0, pimg->w, pimg->h,
		hdcSrc, 0,0, SRCCOPY );

	::DeleteObject( hdcSrc );
	
	return CWnd::OnEraseBkgnd(pDC);
}
