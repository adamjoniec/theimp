// FullscreenWnd.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "FullscreenWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFullscreenWnd

CFullscreenWnd::CFullscreenWnd()
{
	hwndParent = NULL;
	bUpdateBackground = TRUE;
	bKeepInitMouseMsg = FALSE;
}

CFullscreenWnd::~CFullscreenWnd()
{
}


BEGIN_MESSAGE_MAP(CFullscreenWnd, CWnd)
	//{{AFX_MSG_MAP(CFullscreenWnd)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFullscreenWnd message handlers


BOOL CFullscreenWnd::Create( LPCTSTR lpszWindowName, CWnd* pParentWnd) 
{
	char* szClassName = (char*)AfxRegisterWndClass( 
				0,//CS_DBLCLKS,
				NULL, //LoadCursor(NULL, IDC_ARROW),
				NULL, //(HBRUSH)GetStockObject(BLACK_BRUSH),
				LoadIcon( AfxGetInstanceHandle(), 
					MAKEINTRESOURCE(IDR_MAINFRAME) ) );

	RECT rc = {0,0,0,0};

	BOOL ret = CWnd::CreateEx(0, szClassName, lpszWindowName, 
		WS_POPUP|WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX, rc, NULL, 0, NULL);

	::ShowWindow(m_hWnd, SW_SHOW);

	return ret;
}

BOOL CFullscreenWnd::ShowWindow( int nCmdShow )
{
	BOOL ret = FALSE;

	switch ( nCmdShow ) {
		case SW_SHOW:
			SetWindowPos( NULL, 0,0, GetSystemMetrics( SM_CXSCREEN), 
								GetSystemMetrics( SM_CYSCREEN), SWP_NOZORDER );
			bKeepInitMouseMsg = TRUE;
			break;

		case SW_HIDE:
			SetWindowPos( NULL, 0,0, 0, 0, SWP_NOZORDER );
			break;

		default:
			ret = ::ShowWindow( m_hWnd, nCmdShow );
			break;
	}


	return ret;
}

CWnd* CFullscreenWnd::SetParent( CWnd* pWndNewParent )
{
	if ( pWndNewParent ) hwndParent = pWndNewParent->m_hWnd;

	return NULL;
}

void CFullscreenWnd::UpdateBackground(BOOL bUpdate) 
{
	bUpdateBackground = bUpdate;

	InvalidateRect(NULL);
	UpdateWindow();
}

LRESULT CFullscreenWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if ( hwndParent ) switch ( message ) 
	{
		case WM_ERASEBKGND:
			::PostMessage( hwndParent, WM_PAINT, wParam, lParam );
			break;

		case WM_MOUSEMOVE:
			if ( bKeepInitMouseMsg ) {
				bKeepInitMouseMsg = FALSE;
				break;
			}
			::PostMessage( hwndParent, message, -1, lParam );
			LoadCursor(NULL, IDC_ARROW);
			break;

		case WM_COMMAND:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		//case WM_DISPLAYCHANGE:
			bKeepInitMouseMsg = FALSE;
			::PostMessage( hwndParent, message, wParam, lParam );
			break;
	}
	
	return CWnd::DefWindowProc(message, wParam, lParam);
}

void CFullscreenWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if ( bUpdateBackground ) {
		RECT rc; GetWindowRect( &rc );

		dc.BitBlt(rc.left, rc.top, rc.right, rc.bottom, NULL, 0,0, BLACKNESS );
	}
	
}


void CFullscreenWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch (nID) {

		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			if ( (*pmovieState)&VCD_PLAYING )
				return;

			break;
	}
	
	CWnd::OnSysCommand(nID, lParam);
}


void CFullscreenWnd::PutInfoMovieState( DWORD *pMovieState )
{
	pmovieState = pMovieState;
}
