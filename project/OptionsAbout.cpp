// OptionsAbout.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsAbout.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsAbout dialog

static int frame=0;
BOOL bDrawingScroller = FALSE;

COptionsAbout::COptionsAbout(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent, 
						cGImage *pimgAbout,cGImage  *pimgAboutScroller,
						cGImage *pimgAboutWWW, cGImage *pimgAboutRegisterIt, cGImage *pimgAboutMail )   // standard constructor
	: CDialog(COptionsAbout::IDD, pParent),
	iLast(0)
{
	//{{AFX_DATA_INIT(COptionsAbout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	this->pimgAbout = pimgAbout;
	this->pimgAboutScroller = pimgAboutScroller;
	this->pimgAboutWWW = pimgAboutWWW;
	this->pimgAboutMail = pimgAboutMail;
	this->pimgAboutRegisterIt = pimgAboutRegisterIt;
}


void COptionsAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsAbout)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsAbout, CDialog)
	//{{AFX_MSG_MAP(COptionsAbout)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsAbout message handlers

void CALLBACK OnMTimer( UINT uID, UINT uMsg,     
						DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	frame++;
	if ( !bDrawingScroller ) 
		((COptionsAbout*)dwUser)->DrawScroller(NULL);
}

BOOL SendMail( HWND hWnd, char *pszTo, char *pszSubject )
{
	HINSTANCE hinstMapi32 = ::LoadLibraryA("MAPI32.DLL");
	if ( !hinstMapi32 ) return FALSE;

	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)lpfnSendMail = GetProcAddress(hinstMapi32, "MAPISendMail");
	if (lpfnSendMail == NULL)
	{
		FreeLibrary( hinstMapi32 );
		return FALSE;
	}

	MapiRecipDesc mapird = { 0, MAPI_TO, pszTo, 0, 0,0 };

	MapiMessage message;
	memset(&message, 0, sizeof(message));
	message.lpszSubject = pszSubject;
	message.nRecipCount = 1;
	message.lpRecips = &mapird;

	// Function lpfnSendMail is really MAPISendMail, but we can link
	//  MAPISendMail in MFC, so we must load it from MAPI32.DLL.
	int nError = lpfnSendMail(0, (ULONG)hWnd,//(ULONG)this->GetSafeHwnd(),
		&message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	FreeLibrary( hinstMapi32 );

	SetForegroundWindow( hWnd );

	return nError==SUCCESS_SUCCESS;
}

BOOL COptionsAbout::OnInitDialog() 
{
	CDialog::OnInitDialog();

	::SetClassLong( m_hWnd, GCL_HCURSOR, 0 );	

	if (!frame) frame = -270;


	mmTimer = timeSetEvent( 16, 8, OnMTimer, (DWORD)this, TIME_PERIODIC );
	//#define TIMER_ABOUT 3
	//SetTimer( TIMER_ABOUT, 12, NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void COptionsAbout::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawAbout(dc.m_hDC);
	
	// Do not call CDialog::OnPaint() for painting messages
}

void COptionsAbout::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	frame ++;

	DrawScroller(NULL);
	
	CDialog::OnTimer(nIDEvent);
}

void COptionsAbout::OnDestroy() 
{
	CDialog::OnDestroy();

	timeKillEvent( mmTimer );
	//KillTimer( TIMER_ABOUT );
}

void COptionsAbout::DrawAbout( HDC hdc )
{
	HDC hDC = hdc; if (!hdc ) hDC = ::GetDC( m_hWnd );

	HDC hdcSrc = CreateCompatibleDC( hDC );
	::SelectObject( hdcSrc, pimgAbout->hBitmap );

	RECT rc; GetWindowRect( &rc );
	int x = (RECTW(rc)-pimgAbout->w)/2, 
		 y = (RECTH(rc)-pimgAbout->h)/2;
	TransparentBlt( hDC, x,y, pimgAbout->w, pimgAbout->h, 
		hdcSrc, 0,0,pimgAbout->w, pimgAbout->h, RGB(255,255,255) );

	DeleteDC( hdcSrc );

	DrawScroller( hDC );

	if ( !hdc ) ::ReleaseDC( m_hWnd, hDC );

}

void COptionsAbout::DrawScroller( HDC hdc )
{
	bDrawingScroller = TRUE;

	HDC hDC = hdc; if (!hdc ) hDC = ::GetDC( m_hWnd );

	HDC hdcSrc = CreateCompatibleDC( hDC );
	::SelectObject( hdcSrc, pimgAboutScroller->hBitmap );

	RECT rc; GetWindowRect( &rc );
	int x = (RECTW(rc)-pimgAbout->w)/2 + 40, 
		 y = (RECTH(rc)-pimgAbout->h)/2 + 215,
		 w = 270, h = 23;

	HRGN hrgnClip = CreateRectRgn( x,y, x+w, y+h );
	SelectClipRgn( hDC, hrgnClip ); ::DeleteObject( hrgnClip );
	
	::BitBlt( hDC, x,y, w, h, hdcSrc, frame,0, SRCCOPY );
	if ( frame > pimgAboutScroller->w) frame=-270;

	DeleteDC( hdcSrc );


	if ( !hdc ) ::ReleaseDC( m_hWnd, hDC );

	bDrawingScroller = FALSE;
}

void COptionsAbout::OnLButtonUp(UINT nFlags, CPoint point) 
{
	switch ( iLast )
	{
		case 1:
			ShellExecute(m_hWnd,
				"open", "http://www.theimp.prv.pl/register.html",
				NULL,NULL,SW_SHOWDEFAULT);
			break;
		case 2: 
			SendMail( m_hWnd, "adamjoniec@o2.pl", "Note to 'The Imp' author" );
			break;
		case 3:
			ShellExecute(m_hWnd,
				"open", "http://www.theimp.prv.pl/",
				NULL,NULL,SW_SHOWDEFAULT);
			break;
	}
	
		
	CDialog::OnLButtonUp(nFlags, point);
}

void COptionsAbout::OnMouseMove(UINT nFlags, CPoint point) 
{

	RECT rcRegisterIt = { 29, 181, 29+306, 181+34 },
		rcMail = { 51,130, 51+180, 130+30 },
		rcWWW = { 50,151, 50+180, 151+30 };


	RECT rc; GetWindowRect( &rc );
	int x = (RECTW(rc)-pimgAbout->w)/2, 
		 y = (RECTH(rc)-pimgAbout->h)/2;

	OffsetRect( &rcRegisterIt, x, y );
	OffsetRect( &rcMail, x, y );
	OffsetRect( &rcWWW, x, y );

	HDC hDC = ::GetDC( m_hWnd );

	HDC hDCSrc = CreateCompatibleDC( hDC );


	RECT *prc = NULL;
	cGImage *pimg = NULL;

	if ( PtInRect( &rcRegisterIt, point) )
	{
		if ( iLast!=1 ) {
			prc = &rcRegisterIt;
			pimg = pimgAboutRegisterIt;
			iLast = 1;
		}
	}
	else if ( PtInRect( &rcMail, point) )
	{
		if ( iLast!=2 ) {
			prc = &rcMail;
			pimg = pimgAboutMail;
			iLast = 2;
		}
	}
	else if ( PtInRect( &rcWWW, point) )
	{
		if ( iLast!=3 ) {
			prc = &rcWWW;
			pimg = pimgAboutWWW;
			iLast = 3;
		}
	}
	else if ( iLast )
	{
		DrawAbout( hDC );
		SetCursor( AfxGetApp()->LoadStandardCursor( MAKEINTRESOURCE(IDC_ARROW) ) );

		iLast = 0;
	}

	if ( prc && pimg && iLast) 
	{
		DrawAbout( NULL );

		SelectObject( hDCSrc, pimg->hBitmap );
		::BitBlt( hDC, prc->left, prc->top,
			RECTW((*prc)), RECTH((*prc)),
			hDCSrc, 0,0, SRCCOPY );
		SetCursor( AfxGetApp()->LoadStandardCursor( 
			MAKEINTRESOURCE(IDC_HAND) ) );

	}

	::DeleteDC( hDCSrc );
	::ReleaseDC( m_hWnd, hDC );
	
	CDialog::OnMouseMove(nFlags, point);
}

/*
The Illegal Movie Player      Register It!!! Register It!!! Register It!!! If you want full version just register it ( ONLY 3$ )   you'll get new version with new languages support, with new skins and more, more...   Registering is safe and easy ( really Safe and easy )   just click ' register it!!! '   what are you waiting for ???  just click ' register it!!! ' to register   i'm poor - give me some cash - just click ' register it!!! ' and register this program 
*/
