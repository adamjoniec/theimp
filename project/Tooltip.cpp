// Tooltip.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "Tooltip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTooltip

#define TIMER_CLOSE 0x10

CTooltip::CTooltip(CWnd *pWndParent, char *szTip, BOOL bEnabled)
{
	char* szClassName = (char*)AfxRegisterWndClass( 
				0, NULL, NULL, NULL );

	RECT rc = {0,0,0,0};
	CWnd::CreateEx(0, szClassName, "ToolTip", 
		WS_POPUP, rc, pWndParent, 0, NULL);

	hWndParent = pWndParent->m_hWnd;

	strncpy(szToolTip, szTip, 31 ); szToolTip[32] = 0;
	bDrawEnabled = bEnabled; bPainted = FALSE;

	Font.CreatePointFont(85,"Arial");
	CDC *pDC = GetDC();
	pDC->SetMapMode(MM_TEXT);
	pDC->SelectObject( &Font );

	// Size is retuned in logical units, but in map mode MM_TEXT,
	//  one logical unit = one pixel.
	size = pDC->GetTextExtent( szToolTip, strlen(szToolTip) );
	size.cx += 20; size.cy = 17;
	ReleaseDC( pDC );

	if ( GetLastError() ) {
		::SendMessage(hWndParent, WM_PARENTNOTIFY, WM_CLOSE, (long)m_hWnd );
		return;
	}

	POINT pt; GetCursorPos( &pt );

	int sx = GetSystemMetrics( SM_CXSCREEN );

	if ( pt.x+size.cx > sx ) pt.x -= size.cx-(sx-pt.x);
	if ( pt.y-size.cy-5<0 ) pt.y += size.cy+size.cy+15;
	SetWindowPos(NULL, pt.x, pt.y-size.cy-5, size.cx, size.cy, 
		SWP_NOZORDER|SWP_NOSENDCHANGING|SWP_NOACTIVATE|SWP_SHOWWINDOW);

	SetTimer( TIMER_CLOSE, 1500, NULL );
}

CTooltip::~CTooltip()
{
	CWnd::DestroyWindow();
}


BEGIN_MESSAGE_MAP(CTooltip, CWnd)
	//{{AFX_MSG_MAP(CTooltip)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTooltip message handlers


void CTooltip::OnPaint() 
{
	if (bPainted) {
		::SendMessage(hWndParent, WM_PARENTNOTIFY, WM_CLOSE, (long)m_hWnd );
		return;
	}

	CPaintDC dc(this); // device context for painting

	CPen pen; pen.CreatePen( PS_SOLID,0,RGB(0,0,0) );	
	LOGBRUSH lb = {BS_NULL,0,0};
	CBrush brush; brush.CreateBrushIndirect( &lb );
	dc.SelectObject( &brush );

	int w=size.cx, h=size.cy;
	
	if ( bDrawEnabled ) {
		int r=0xFF, g=0xF8, b=0xD7;
		POINT pt[2] = { {1,1}, {w-1, 1} };
		for ( int i=1; i<h-1; i++) {

			pen.DeleteObject(); 
			pen.CreatePen(PS_SOLID,0,RGB(r,g,b)); 
			dc.SelectObject( &pen );

			pt->y = i; (pt+1)->y = i;
			dc.Polyline( pt,2);

			b++; if( !(b%5) ) { g++; b++; }
		}
	} else {
		int r=0xC0, g=0xC0, b=0xC0;
		POINT pt[2] = { {1,1}, {w-1, 1} };
		for ( int i=1; i<h-1; i++) {

			pen.DeleteObject(); 
			pen.CreatePen(PS_SOLID,0,RGB(r,g,b)); 
			dc.SelectObject( &pen );

			pt->y = i; (pt+1)->y = i;
			dc.Polyline( pt,2);

			r+=3; g=b=r;
		}
	}

	pen.DeleteObject(); pen.CreatePen( PS_SOLID,0,RGB(0,0,0) );
	dc.SelectObject( &pen );

	dc.Rectangle(0,0,size.cx, size.cy);

	POINT ptNW[4] = { {1,1}, {2,1}, {1,2}, {1,1} }, 
			ptNE[4] = { {w-2,1}, {w-3,1}, {w-2,2}, {w-2,1} },
			ptSW[4] = { {1,h-2}, {2,h-2}, {1,h-3}, {1,h-2} },
			ptSE[4] = { {w-2,h-2}, {w-3,h-2}, {w-2,h-3}, {w-2,h-2} };
	dc.Polyline( ptNW, 4 ); dc.Polyline( ptNE, 4 );
	dc.Polyline( ptSW, 4 ); dc.Polyline( ptSE, 4 );

	dc.SelectObject( &Font );
	dc.SetBkMode(TRANSPARENT);
	dc.TextOut( 10,1, szToolTip );

	bPainted = TRUE;
}

void CTooltip::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent==TIMER_CLOSE ) {

		KillTimer( TIMER_CLOSE );
		::SendMessage(hWndParent, WM_PARENTNOTIFY, WM_CLOSE, (long)m_hWnd );
		return;
	}
	
	CWnd::OnTimer(nIDEvent);
}

void CTooltip::OnClose() 
{
	KillTimer( TIMER_CLOSE );
	::SendMessage(hWndParent, WM_PARENTNOTIFY, WM_CLOSE, (long)m_hWnd );
	
	PostQuitMessage(0);
}

LRESULT CTooltip::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch ( message ) {
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_MOUSEWHEEL:
		case WM_MOUSEMOVE:
		case WM_COMMAND:			
			KillTimer( TIMER_CLOSE );
			::SendMessage(hWndParent, WM_PARENTNOTIFY, WM_CLOSE, (long)m_hWnd );
			return TRUE;

	}
		
	return CWnd::DefWindowProc(message, wParam, lParam);
}
