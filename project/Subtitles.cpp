// Subtitles.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "Subtitles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CSubtitles

CSubtitles::CSubtitles():	
					bLoaded(FALSE),
					timerId(0),
					pMovie(NULL),
					pOptions(NULL),
					bFullscreen(FALSE),
					psbCurrent(NULL),
					pSubtitles(NULL),
					hbmBackground(NULL),
					hFont(NULL),
					iFontOutlinePix(2),

					iSubtitlesMode(SUBTITLES_VMR),
					iSubtitlesRows(2),
					iSubtitlesRowsDistance(0), //[pixel]
					valSubtitlesDistanceFromBottom(5), //[percent]
					iSubtitlesMultiRowMode(SUBTITLESMR_TOP_BOTTOM),
					bSubtitlesBkgrndShort(TRUE),	//TRUE-LITTLE FASTER
					bSubtitlesRgnPrecise(TRUE),	//ONLY REGION MODE

					pszFontName("Verdana"),
					iFontSize(16),
					iFontWeight(FW_NORMAL),
					iFontOutline(FO_BOLD),
					bFontAutosize(TRUE),
					bFontBkgrndTransparent(TRUE),
					colFont( 0x000000 ),
					colFontBkgrnd( 0x101010 ),	//TRANSPARENT
					colFontOutline( 0xFFFFFF )
{
}

CSubtitles::~CSubtitles()
{
}


BEGIN_MESSAGE_MAP(CSubtitles, CWnd)
	//{{AFX_MSG_MAP(CSubtitles)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSubtitles message handlers

BOOL CSubtitles::Create( stConfig *pOpt, CSubtitlesFs *pSubtitlesFs, CWnd* pParentWnd, CMpegMovie *pMovie)
{

	pOptions = pOpt;

	UpdateOptions();

	// Create subtitles window, used to show subtitles in windowed mode.
	char *pszClassName = (char*)AfxRegisterWndClass( 
				0, 0, NULL,
				LoadIcon( AfxGetInstanceHandle(), 
					MAKEINTRESOURCE(IDR_MAINFRAME) ) );	

	RECT rc = {0,0,0,0};
	BOOL ret = CWnd::CreateEx( 0, pszClassName,"Subtitles",
		WS_POPUP,rc, pParentWnd, 0 );

	// Initialize window used to show subtitles in fullscreen mode.
	this->pSubtitlesFs = pSubtitlesFs;
	pSubtitlesFs->SetBitmapBackground( &hbmBackground );
	pSubtitlesFs->SetMode( &iSubtitlesMode );

	// Create timer.
	timerId = SetTimer( TIMER_SUBTITLES, 500, NULL );

	// All calls to window state are through pThis, because
	//  pThis is pointer to current subtitles window
	//  ( in windowed and fs windows are diffrent ).
	pThis = this;

	if ( pMovie ) SetMovie( pMovie );
	
	return ret;
}

void CSubtitles::UpdateOptions()
{
	iSubtitlesMode = pOptions->iSubtitlesMode;
	iSubtitlesRows = pOptions->iSubtitlesRows;
	iSubtitlesRowsDistance = pOptions->iSubtitlesRowsDistance;
	iSubtitlesMultiRowMode = pOptions->iSubtitlesMultiRowMode;
	iSubtitlesHideAfter = pOptions->iSubtitlesHideAfter;
	bSubtitlesBkgrndShort = pOptions->bSubtitlesBkgrndShort;
	bSubtitlesRgnPrecise =  pOptions->bSubtitlesRgnPrecise;
	valSubtitlesDistanceFromBottom = pOptions->valSubtitlesDistanceFromBottom;
	pszFontName = pOptions->szSbtFontName;
	iFontSize = pOptions->iSbtFontSize;
	iFontWeight = pOptions->iSbtFontWeight;
	iFontOutline = pOptions->iSbtFontOutline;
	bFontAutosize = pOptions->bSbtFontAutosize;
	bFontBkgrndTransparent = pOptions->bSbtFontBkgrndTransparent;
	colFont = pOptions->colSbtFont;
	colFontBkgrnd = pOptions->colSbtFontBkgrnd;
	colFontOutline = pOptions->colSbtFontOutline;

	bSubtitlesOffsetTime = pOptions->bSubtitlesOffsetTime;
	valSubtitlesOffsetTime = pOptions->valSubtitlesOffsetTime;

	bEnabled = pOptions->bShowSubtitles;
}

void CSubtitles::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if ( iSubtitlesMode==SUBTITLES_OVERLAY || 
				iSubtitlesMode==SUBTITLES_RGN )
	{
		if ( !hbmBackground ) return;

		SIZE size; GetBitmapDimensionEx( hbmBackground, &size );
		HDC hdcSource = CreateCompatibleDC( dc.m_hDC );
		::SelectObject( hdcSource, hbmBackground );
		::BitBlt( dc.m_hDC, 0,0, size.cx, size.cy,
			hdcSource, 0,0, SRCCOPY );
		::DeleteDC( hdcSource );

	}	
	// Do not call CWnd::OnPaint() for painting messages
}


void CSubtitles::SetMovie( CMpegMovie *pMovie )
{
	this->pMovie = pMovie;

	

	if ( pMovie )
	{
		pMovie->SetAppImage( NULL );

		if ( pMovie->IsWin9xCompatible() && iSubtitlesMode==SUBTITLES_VMR )
			iSubtitlesMode = SUBTITLES_OVERLAY;
	}
}


////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
BOOL CSubtitles::ReadNumber(DWORD *num, BOOL bReturn )
{
	char c, *pszSet = psz;
	while ( *psz>='0' && *psz<='9' ) psz++;

	if (*psz==*pszSet && !bReturn) return FALSE;
	c = *psz; *psz = 0; *num = atoi(pszSet); *psz = c;

	return TRUE;
}

char *CSubtitles::EndLine(char *sz)
{
	--sz; while ( *sz==' ' || *sz==0x0a || *sz==0x0d ) sz--; 
	++sz; *sz = 0;
	return sz;
}

//0x0d, 0x0a - new line
//0x7c - new line of text
// '{' '}' ':' 0x09-tab

BOOL CSubtitles::ReadSubtitles(char *pszFilename)
{	
	FILE *hFile = fopen(pszFilename,"rb"); 
	if ( !hFile ) return FALSE;

	bLoaded = FALSE;

	DeleteSubtitles();

	DWORD dwTextSize;
	
	
	fseek(hFile, 0, SEEK_END); 
	dwTextSize = ftell(hFile); fseek(hFile, 0, SEEK_SET);
	
	pszText = new char[ dwTextSize + 16]; psz = pszText;
	memset(pszText, 0, dwTextSize + 16);
	fread(pszText, 1, dwTextSize, hFile);
	fclose(hFile);

	
	dwSubtitlesNum = 0;
	while ( *psz )
	{
		DWORD num1, num2, num3;
		char *pszTemp = psz;
		if (  ( (*(psz+2)==':' || *(psz+1)==':') && ReadNumber(&num1) && *psz++==':' && ReadNumber(&num2) && *psz++==':' 
				&& ReadNumber(&num3) ) && 
			  (*psz==':' || *psz==',' || *psz=='=' || *psz==' ' || *psz==0x09)
			) ++dwSubtitlesNum;
		else { 
			psz = pszTemp;
			if ( *psz++=='{' && ReadNumber(&num1) && *psz++=='}' && *psz++=='{'
					&& ReadNumber(&num2,TRUE) && *psz++=='}' ) ++dwSubtitlesNum;
			else psz = pszTemp+1;
		}
	}
	psz = pszText;

	if (!dwSubtitlesNum)
		return FALSE;

	pSubtitles = new stSubtitle[dwSubtitlesNum];
	memset(pSubtitles, 0, dwSubtitlesNum*sizeof(stSubtitle) );
	stSubtitle *pst = pSubtitles;

	while ( *psz )
	{
		DWORD num1, num2, num3;
		char *pszTemp = psz;
		if (  ( (*(psz+2)==':' || *(psz+1)==':') && ReadNumber(&num1) && *psz++==':' && ReadNumber(&num2) && *psz++==':' 
				&& ReadNumber(&num3) ) && 
			  (*psz==':' || *psz==',' || *psz=='=' || *psz==' ' || *psz==0x09)
			)
		{
			psz++;
			
			if (*psz>='1' && *psz<='9' && *(psz+1)=='=') {
				//format 00:00:00,1=  \n00:00:00,2=	\n00:00:00,1=
				pst->dwType = 4 + (*psz) - '1'; psz += 2; }
			else pst->dwType = 1; //format 00:00:00

			pst->dwFrameIn = 0; pst->dwFrameOut = 0;
			pst->fTimeOut = 0.0f;
			pst->fTimeIn = (float)(num1*3600 + num2*60 + num3);
			pst->psz = psz;
			if (pst != pSubtitles) EndLine(pszTemp);
			pst++;								
		}
		else { 
			psz = pszTemp;
			if ( *psz++=='{' && ReadNumber(&num1) && *psz++=='}' && *psz++=='{'
					&& ReadNumber(&num2,TRUE) && *psz=='}' )
			{
				psz++;
				pst->dwType = 0; //format {}{}
				pst->fTimeIn = 0.0f; pst->fTimeOut = 0.0f;
				pst->dwFrameIn = num1; pst->dwFrameOut = num2;
				pst->psz = psz;
				if (pst != pSubtitles) EndLine(pszTemp);
				pst++;
			}
			else psz = pszTemp+1;
		}
	}

	bLoaded = TRUE;

	if ( pMovie )
		ConvertSubtitles( pMovie->GetFPS() );
	else
		ConvertSubtitles( 25.0 );

	// Fill fconstTimeIn and fconstTimeOut with true time in
	//  and true time out, before calling OffsetSubtitles().
	// From now fcostTimeIn and fconstTimeOut don't be modified.
	pst = pSubtitles;
	for (DWORD dw=0; dw<dwSubtitlesNum-1; dw++)
	{
		pst->fconstTimeIn = pst->fTimeIn;
		pst->fconstTimeOut = pst->fTimeOut;
		++pst;
	}

	OffsetSubtitles();

	return TRUE;
}

void CSubtitles::ConvertSubtitles(double fps)
{
	if ( !pSubtitles || !bLoaded ) return;

	DWORD dw = 0;
	stSubtitle *pst = pSubtitles;
	double rate = 1.0/fps;

	for (dw=0; dw<dwSubtitlesNum; dw++)
	{
		if ( pst->dwType > 4 && pst->dwType-(pst-1)->dwType==1 )
		{
			stSubtitle *pstFirst = pst-(pst->dwType-4);
			pst->dwFrameIn = pstFirst->dwFrameIn;
			pst->dwFrameOut = pstFirst->dwFrameOut;
			pst->fTimeIn = pstFirst->fTimeIn;
			pst->fTimeOut = pstFirst->fTimeOut;

			char *sz = new char [ strlen(pst->psz) +1 ]; strcpy(sz,pst->psz);
			DWORD dwStrlen = strlen(pstFirst->psz); *(pstFirst->psz+dwStrlen) = '|';
			strcpy(pstFirst->psz+dwStrlen+1,sz); 
			delete sz;

			pst->psz = pstFirst->psz;
		}
		pst++;
	}

	pst = pSubtitles;
	for (dw=0; dw<dwSubtitlesNum-1; dw++)
	{
		if ( pst->dwType == 0) {
			pst->fTimeIn = (float)((double)pst->dwFrameIn*rate);
			pst->fTimeOut = (float)((double)pst->dwFrameOut*rate);
		}
		++pst;
	}

	pst = pSubtitles;
	for (dw=0; dw<dwSubtitlesNum-1; dw++)
	{
		if ( pst->fTimeOut <= pst->fTimeIn || pst->fTimeOut > (pst+1)->fTimeIn)
		{
			if ( (pst+1)->fTimeIn-pst->fTimeIn > 4.0 )
				pst->fTimeOut = pst->fTimeIn+4.0f;
			else				
				pst->fTimeOut = (pst+1)->fTimeIn;
		}
		++pst;
	}
	if ( pst->dwType == 0) {
		pst->fTimeIn = (float)((double)pst->dwFrameIn*rate);
		pst->fTimeOut = (float)((double)pst->dwFrameOut*rate);
	}
	if ( pst->fTimeOut < pst->fTimeIn )
			pst->fTimeOut = pst->fTimeIn + 4.0f;
	psbCurrent = NULL;
}

void CSubtitles::CheckSubtitles(double time)
{
	BOOL bDefTimeout = TRUE;
	DWORD dwTimeout = iSubtitlesHideAfter;

	if ( !pSubtitles || !bLoaded ) return;

	if ( time < pSubtitles->fTimeIn ) { 
		if ( !psbCurrent ) return;

		ShowBackground( FALSE );
		psbCurrent = NULL;
		return; 
	}

	static stSubtitle *pstLastActive;

	pstLastActive = psbCurrent;

	psbCurrent = pSubtitles;
	stSubtitle *pstLast = pSubtitles + dwSubtitlesNum - 1;		
	while( time >= (psbCurrent+1)->fTimeIn && psbCurrent < pstLast ) psbCurrent++;
	
	if ( bDefTimeout ) {
		if ( time > psbCurrent->fTimeIn+dwTimeout) psbCurrent = NULL;
	} 
	else {
		if ( time > psbCurrent->fTimeOut ) psbCurrent = NULL;
	}

	if ( pstLastActive != psbCurrent )
	{
		if ( psbCurrent )
		{
			CreateBackground();
			ShowBackground( TRUE );
		}
		else
		{
			ShowBackground( FALSE );
		}
	}
	
}

void CSubtitles::OffsetSubtitles( )
{
	if ( !pSubtitles || !bLoaded ) return;

	BOOL bStoreEnabled = bEnabled;
	EnableSubtitles( FALSE );

	// If valSubtitlesOffsetTime is greater than zero subtitles
	//  will be speed up, if less - delayed.
	stSubtitle *pst = pSubtitles;
	float foffset = valSubtitlesOffsetTime;
	if ( !bSubtitlesOffsetTime ) foffset = 0;
	for (DWORD dw=0; dw<dwSubtitlesNum; dw++)
	{
		pst->fTimeIn = pst->fconstTimeIn - foffset;
		pst->fTimeOut = pst->fconstTimeOut - foffset;
		++pst;
	}

	EnableSubtitles( bStoreEnabled );

}

BOOL CSubtitles::WriteOffsetedSubtitles( const char *pszFileName )
{
	FILE *f = fopen( pszFileName, "wt" );

	if ( !f ) return FALSE;
	
	stSubtitle *pst = pSubtitles;
	for (DWORD dw=0; dw<dwSubtitlesNum; dw++, pst++)
	{
		if ( pst->fTimeIn<0 ) continue;

		fprintf( f, "%.2d:%.2d:%.2d:%s\n", 
			((int)pst->fTimeIn/3600),
			((int)pst->fTimeIn%3600)/60,
			((int)pst->fTimeIn%3600)%60,
			pst->psz );
	}

	fclose(f);

	return TRUE;
}


void CSubtitles::DeleteSubtitles()
{
	if ( !pSubtitles ) return;

	ShowBackground( FALSE );

	delete []pszText; pszText = NULL;
	delete []pSubtitles; pSubtitles = NULL; psbCurrent = NULL;
}

void CSubtitles::OnTimer(UINT nIDEvent) 
{
	if ( !pMovie || !bEnabled || !bLoaded )
		return;

	CheckSubtitles( pMovie->GetCurrentPosition() );

	
	CWnd::OnTimer(nIDEvent);
}

DWORD colTransparent = 0x100010;
void CSubtitles::Reposition(int x, int y, int cx, int cy, BOOL bForceRedraw)
{
	if ( !pMovie ) return;

	HDC hDC = ::GetDC( m_hWnd );

	SetRect( &rcMoviePos, x,y, x+cx, y+cy );

	int sizeFont = iFontSize;
	if ( bFontAutosize ) 
	{
		double screencx = GetSystemMetrics( SM_CXSCREEN ), destcx = cx;
		sizeFont = (int)((double)iFontSize*( destcx/screencx ));
		if ( sizeFont<4 ) sizeFont = 4;
	}
	LOGFONT logfont= {

		-MulDiv(sizeFont, GetDeviceCaps(hDC, LOGPIXELSY), 72), //lfHeight
		0, 0, 0,							//lfWidth, lfEscapement, lfOrientation
		iFontWeight,						//lfWeight(def)
		FALSE, FALSE, FALSE,			//lfItalic, lfUnderline, lfStrikeOut
		DEFAULT_CHARSET,				//lfCharSet 
		OUT_DEFAULT_PRECIS,			//lfOutPrecision 
		CLIP_DEFAULT_PRECIS,			//lfClipPrecision 
		ANTIALIASED_QUALITY,//DEFAULT_QUALITY,				//lfQuality 
		DEFAULT_PITCH|FF_DONTCARE	//lfPitchAndFamily 

	}; strcpy( logfont.lfFaceName, pszFontName );

	if ( hFont ) DeleteObject( hFont );
	hFont = CreateFontIndirect( &logfont );
	SelectObject( hDC, hFont );

	SIZE size; GetTextExtentPoint32( hDC, "A", 1, &size );

	::ReleaseDC( m_hWnd, hDC );


	if ( iSubtitlesMode==SUBTITLES_OVERLAY || iSubtitlesMode==SUBTITLES_RGN )
	{

		int iHeight = (size.cy+iSubtitlesRowsDistance)*iSubtitlesRows + 
								iFontOutlinePix*2 - iSubtitlesRowsDistance;
		if ( iHeight>cy ) iHeight = cy;
		int iUpheave = (int)(valSubtitlesDistanceFromBottom*(double)(cy-iHeight)/100.0);


		RECT rc; pThis->GetWindowRect(&rc);
		if ( (RECTW(rc)!=cx || RECTH(rc)!=iHeight)
				|| bForceRedraw )
			CreateBackground( cx, iHeight );
		
		pThis->GetParent()->GetWindowRect( &rc );

	
		pThis->SetWindowPos( NULL, 
			rc.left+x, rc.top+y+cy-iHeight -iUpheave, cx, iHeight, 
			SWP_NOZORDER|SWP_NOSENDCHANGING|SWP_NOACTIVATE|SWP_NOCOPYBITS );

		pThis->InvalidateRect(NULL); pThis->UpdateWindow();

		
	}
	else
	{

	}
}

void CSubtitles::UpdateVerticalPos()
{
	if ( !RECTH(rcMoviePos) || !RECTW(rcMoviePos) )
		return;

	RECT rc; pThis->GetWindowRect( &rc );
	int iUpheave = (int)(valSubtitlesDistanceFromBottom*
		(double)(RECTH(rcMoviePos)-RECTH(rc))/100.0);

	pThis->SetWindowPos( NULL, 
		rcMoviePos.left, 
		rcMoviePos.top+RECTH(rcMoviePos)-RECTH(rc)-iUpheave, 
		0, 0, 
		SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS|
		SWP_NOSIZE );

}

void CSubtitles::SetFullscreen( BOOL bFs )
{
	if ( iSubtitlesMode==SUBTITLES_VMR ) {
		bFullscreen = bFs; 
		return;
	}

	if ( bFs && !bFullscreen )
	{
		// Show fullscreen subtitles window.
		RECT rc; GetWindowRect( &rc );

		ShowWindow( SW_HIDE );
		DWORD dwFlags = SWP_NOZORDER|SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOACTIVATE;
		if ( psbCurrent ) dwFlags |= SWP_SHOWWINDOW;
		pSubtitlesFs->SetWindowPos( NULL, 
			rc.left, rc.top, RECTW(rc), RECTH(rc), dwFlags );

		pThis = pSubtitlesFs;

		
	}
	else if ( !bFs && bFullscreen )
	{
		// Show windowed subtitles window.

		RECT rc; pSubtitlesFs->GetWindowRect( &rc );

		pSubtitlesFs->ShowWindow( SW_HIDE );
		DWORD dwFlags = SWP_NOZORDER|SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOACTIVATE;
		if ( psbCurrent ) dwFlags |= SWP_SHOWWINDOW;
		SetWindowPos( NULL, 
			rc.left, rc.top, RECTW(rc), RECTH(rc), dwFlags );

		pThis = this;
	}

	bFullscreen = bFs;
}

void CSubtitles::CreateBackground(int cx, int cy)
{

	switch ( iSubtitlesMode )
	{
		case SUBTITLES_VMR: CreateBkgrndUsingVMR( cx, cy ); break;
		case SUBTITLES_OVERLAY: CreateBkgrndUsingOverlay( cx, cy ); break;
		case SUBTITLES_RGN: CreateBkgrndUsingRgn( cx, cy ); break;
	}

}

void CSubtitles::ShowBackground( BOOL bShow )
{
	if ( iSubtitlesMode==SUBTITLES_OVERLAY || iSubtitlesMode==SUBTITLES_RGN )
	{
		if ( bShow && psbCurrent )
		{
			pThis->SetWindowPos( NULL, 0,0,0,0, 
				SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOSENDCHANGING|
				SWP_NOCOPYBITS|SWP_NOACTIVATE|SWP_SHOWWINDOW );
			pThis->InvalidateRect(NULL); pThis->UpdateWindow();

		}
		else
		{
			pThis->ShowWindow( SW_HIDE );
		}

	}
	else
	{

	}
}

void CSubtitles::EnableSubtitles( BOOL bEnable )
{
	if ( bEnable && ! bEnabled ) {
		// Enable subtitles.

		// Subtitles will be shown not from now,
		//  but from next subtitle text.
		psbCurrent = NULL;

	}
	else if ( !bEnable && bEnabled ) {
		// Disable subtitles.
		
		psbCurrent = NULL;
		ShowBackground( FALSE );
	}

	bEnabled = bEnable;
}


LRESULT CSubtitles::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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


void CSubtitles::CreateBkgrndUsingOverlay(int cx, int cy )
{
	if ( cx && cy )
	{
		if ( hbmBackground ) DeleteObject( hbmBackground );
		HDC hDC = ::GetDC( m_hWnd );
		hbmBackground = CreateCompatibleBitmap( hDC, cx, cy );
		SetBitmapDimensionEx( hbmBackground, cx, cy, NULL );
		::ReleaseDC( m_hWnd, hDC );
	}

	if ( !hbmBackground || !psbCurrent || !bEnabled ) return;


	SIZE size; GetBitmapDimensionEx( hbmBackground, &size );
	cx = size.cx; cy = size.cy;

	HDC hWindowDC = ::GetDC( m_hWnd );
	HDC hDC = CreateCompatibleDC( hWindowDC );
	SelectObject( hDC, hbmBackground );


	HBRUSH hBkgrndBrush;
	if ( bFontBkgrndTransparent )
	{
		hBkgrndBrush = CreateSolidBrush( colTransparent );
	}
	else
	{
		if ( colFontBkgrnd==colTransparent ) colFontBkgrnd++;

		hBkgrndBrush = CreateSolidBrush( colFontBkgrnd );
	}

	if ( !bSubtitlesBkgrndShort )
	{
		RECT rc = { 0,0, size.cx+1, size.cy+1 };
		FillRect( hDC, &rc, hBkgrndBrush );
	}

	SelectObject( hDC, hFont );
	SetBkMode( hDC, TRANSPARENT );


	HRGN	hRgn = NULL;
	SIZE sizeFont; GetTextExtentPoint32( hDC, "A", 1, &sizeFont );
	int x=0, y=0, len;


	char *psz = psbCurrent->psz, *pszRow[5];
	for ( int i=0; i<5; i++ ) pszRow[i] = new char[ strlen(psz)+1 ];
	for ( int iRows=0; iRows<iSubtitlesRows; iRows++ )
	{

		char *pszend = psz, *pszprevword = psz;
		if ( !*pszend ) break;
		do {
			pszend++;
			if ( !*pszend || *pszend==' ' || *pszend=='|') {

				int len = pszend-psz;
				SIZE sizeText; 
				GetTextExtentPoint32( hDC, psz, len, &sizeText );
				if ( sizeText.cx > cx ) {
					pszend = pszprevword-1;
					break;
				}
				pszprevword = pszend+1;

			}
		} while ( *pszend && *pszend!='|' );

		len = pszend-psz; if ( len<=0 ) break;

		strncpy( pszRow[iRows], psz, len ); pszRow[iRows][len] = 0;
		
		psz = pszend; if ( *psz=='|' || *psz==' ' ) psz++;
	}

	if ( !iRows )
	{
		hRgn = CreateRectRgn(0,0,0,0);
		pThis->SetWindowRgn( hRgn, TRUE );
		if ( hRgn ) DeleteObject( hRgn );

		for ( i=0; i<5; i++ ) delete[] pszRow[i];

		DeleteObject( hBkgrndBrush );
		DeleteDC( hDC );
		::ReleaseDC( m_hWnd, hWindowDC );

		return;
	}


	if ( iSubtitlesMultiRowMode==SUBTITLESMR_BOTTOM_TOP )
	{
		y = size.cy - iFontOutlinePix*2 -
				(iSubtitlesRowsDistance+sizeFont.cy*iRows);
		if (iRows==1) y += iSubtitlesRowsDistance;
	}
	for ( i=0; i<iRows; i++ )
	{
		psz = pszRow[i];
		len = strlen(psz); if ( !len ) break;

		SIZE sizeText; 
		GetTextExtentPoint32( hDC, psz, len, &sizeText );
		x = (size.cx - sizeText.cx)/2;

		if ( bSubtitlesBkgrndShort )
		{
			int pixBorder = iFontOutlinePix;
			switch (iFontOutline) {
				case FO_THIN: pixBorder += 1; break;
				case FO_BOLD: pixBorder += 2; break; }

			int x1 = x-pixBorder, y1= y,
				 x2 = x1+sizeText.cx+pixBorder*2,
				 y2 = y1+sizeText.cy+pixBorder;

			RECT rc = { x1, y1, x2, y2 };
			FillRect( hDC, &rc, hBkgrndBrush );
			if ( !hRgn ) hRgn = CreateRectRgnIndirect( &rc );
			else {
				HRGN hrgn1 = CreateRectRgnIndirect( &rc ),
					hrgn2 = CreateRectRgnIndirect( &rc ); 
				CombineRgn( hrgn2, hRgn, NULL, RGN_COPY);
				CombineRgn( hRgn, hrgn1, hrgn2, RGN_OR);

				DeleteObject( hrgn1 ); DeleteObject( hrgn2 );

			}


		}

		SetTextColor( hDC, colFontOutline );
		switch ( iFontOutline )
		{
			case FO_NONE:
				TextOut( hDC, x,y,psz,len);
				break;
			case FO_THIN:
				TextOut( hDC, x,y-1,psz,len); TextOut( hDC, x,y+1,psz,len);
				TextOut( hDC, x-1,y,psz,len); TextOut( hDC, x+1,y,psz,len);
				break;
			case FO_BOLD:
				TextOut( hDC, x-1,y-1,psz,len); TextOut( hDC, x+1,y+1,psz,len);
				TextOut( hDC, x-1,y+1,psz,len); TextOut( hDC, x+1,y-1,psz,len);
				TextOut( hDC, x-2,y-2,psz,len); TextOut( hDC, x+2,y+2,psz,len); 
				TextOut( hDC, x-2,y+2,psz,len); TextOut( hDC, x+2,y-2,psz,len);
				TextOut( hDC, x+2,y,psz,len); TextOut( hDC, x-2,y,psz,len);
				TextOut( hDC, x,y+2,psz,len); TextOut( hDC, x,y-2,psz,len);
				break;

		}

		SetTextColor( hDC, colFont );
		TextOut( hDC,  x, y, psz , len);

		y += ( sizeFont.cy+iSubtitlesRowsDistance+iFontOutlinePix );
	}


	pThis->SetWindowRgn( hRgn, TRUE );
	if ( hRgn ) DeleteObject( hRgn );
			
	for ( i=0; i<5; i++ ) delete[] pszRow[i];

	DeleteObject( hBkgrndBrush );
	DeleteDC( hDC );
	::ReleaseDC( m_hWnd, hWindowDC );

}

void CSubtitles::CreateBkgrndUsingRgn(int cx, int cy )
{
	if ( !bFontBkgrndTransparent ) {
		CreateBkgrndUsingOverlay( cx, cy );
		return;
	}

	if ( cx && cy )
	{
		if ( hbmBackground ) DeleteObject( hbmBackground );
		HDC hDC = ::GetDC( m_hWnd );
		hbmBackground = CreateCompatibleBitmap( hDC, cx, cy );
		SetBitmapDimensionEx( hbmBackground, cx, cy, NULL );
		::ReleaseDC( m_hWnd, hDC );
	}

	if ( !hbmBackground || !psbCurrent || !bEnabled ) return;

	SIZE size; GetBitmapDimensionEx( hbmBackground, &size );
	cx = size.cx; cy = size.cy;

	HDC hWindowDC = ::GetDC( m_hWnd );
	HDC hDC = CreateCompatibleDC( hWindowDC );
	SelectObject( hDC, hbmBackground );


	HBRUSH hBkgrndBrush;
	if ( colFontBkgrnd==colTransparent ) colFontBkgrnd++;
	hBkgrndBrush = CreateSolidBrush( colFontOutline );

	SelectObject( hDC, hFont );
	SetBkMode( hDC, TRANSPARENT );


	int icountDraw = bSubtitlesRgnPrecise?1:2;
	for ( int iDraw=0; iDraw<icountDraw; iDraw++ )
	{

		if ( !iDraw ) BeginPath( hDC ); 

		SIZE sizeFont; GetTextExtentPoint32( hDC, "A", 1, &sizeFont );
		int x=0, y=0, len;
		char *psz = psbCurrent->psz, *pszRow[5];
		for ( int i=0; i<5; i++ ) pszRow[i] = new char[ strlen(psz)+1 ];
		for ( int iRows=0; iRows<iSubtitlesRows; iRows++ )
		{

			char *pszend = psz, *pszprevword = psz;
			if ( !*pszend ) break;
			do {
				pszend++;
				if ( !*pszend || *pszend==' ' || *pszend=='|') {

					int len = pszend-psz;
					SIZE sizeText; 
					GetTextExtentPoint32( hDC, psz, len, &sizeText );
					if ( sizeText.cx > cx ) {
						pszend = pszprevword-1;
						break;
					}
					pszprevword = pszend+1;

				}
			} while ( *pszend && *pszend!='|' );

			len = pszend-psz; if ( len<=0 ) break;

			strncpy( pszRow[iRows], psz, len ); pszRow[iRows][len] = 0;
		
			psz = pszend; if ( *psz=='|' || *psz==' ' ) psz++;
		}

		if ( !iRows )
		{
			HRGN hRgn = NULL; hRgn = CreateRectRgn(0,0,0,0);
			pThis->SetWindowRgn( hRgn, TRUE );
			if ( hRgn ) DeleteObject( hRgn );

			for ( i=0; i<5; i++ ) delete[] pszRow[i];

			DeleteObject( hBkgrndBrush );
			DeleteDC( hDC );
			::ReleaseDC( m_hWnd, hWindowDC );

			return;
		}


		if ( iSubtitlesMultiRowMode==SUBTITLESMR_BOTTOM_TOP )
		{
			y = size.cy - iFontOutlinePix*2 -
					(iSubtitlesRowsDistance+sizeFont.cy*iRows);
			if (iRows==1) y += iSubtitlesRowsDistance;
		}

		for ( i=0; i<iRows; i++ )
		{
			psz = pszRow[i];
			len = strlen(psz); if ( !len ) break;

			SIZE sizeText; GetTextExtentPoint32( hDC, psz, len, &sizeText );
			x = (size.cx - sizeText.cx)/2;


			if ( !iDraw ) 
			{
				int pixBorder = iFontOutlinePix;
				switch (iFontOutline) {
					case FO_THIN: pixBorder += 1; break;
					case FO_BOLD: pixBorder += 2; break; }

				int x1 = x-pixBorder, y1= y,
					 x2 = x1+sizeText.cx+pixBorder*2,
					y2 = y1+sizeText.cy+pixBorder;

				RECT rc = { x1, y1, x2, y2 };
				FillRect( hDC, &rc, hBkgrndBrush );
			}

			SetTextColor( hDC, colFont );
			TextOut( hDC,  x, y, psz , len);

			y += ( sizeFont.cy+iSubtitlesRowsDistance+iFontOutlinePix );
		}

		if ( !iDraw ) EndPath( hDC );
		for ( i=0; i<5; i++ ) delete[] pszRow[i];

	}

	HRGN	hRgn = PathToRegion( hDC );

	if ( bSubtitlesRgnPrecise )
	{
		HBRUSH hbrush = CreateSolidBrush( colFont );
		FillRgn( hDC, hRgn, hbrush );
		DeleteObject( hbrush );
	}


	if ( hRgn )
	{
		DWORD buffsize = GetRegionData( hRgn, 0, NULL );
		char *buff = new char [ buffsize ];
		RGNDATA *pRgnData = (RGNDATA*)buff;
		GetRegionData( hRgn, buffsize, pRgnData );

		int count = pRgnData->rdh.nCount;
		register RECT *prc = (RECT*)pRgnData->Buffer;
		register int pixBorder = 0;
		switch (iFontOutline) {
			case FO_THIN: pixBorder = 1; break;
			case FO_BOLD: pixBorder = 2; break; }
		for ( int i=0; i<count; i++ )
		{
			prc->left -= pixBorder; prc->top -= pixBorder;
			prc->right += pixBorder; prc->bottom += pixBorder;
			prc++;
			
		}
		DeleteObject( hRgn );
		hRgn = ExtCreateRegion( NULL, buffsize, pRgnData );

		delete[] buff;

		pThis->SetWindowRgn( hRgn, TRUE );
		DeleteObject( hRgn );
	}
			

	DeleteObject( hBkgrndBrush );
	DeleteDC( hDC );
	::ReleaseDC( m_hWnd, hWindowDC );

}

void CSubtitles::CreateBkgrndUsingVMR(int cx, int cy )
{
	//NIY
}

void CSubtitles::OnClose() 
{
	if ( timerId ) KillTimer( timerId );
	
	CWnd::OnClose();
}

void CSubtitles::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	
	if ( nState!=WA_INACTIVE )
	{
		pWndOther->SetWindowPos( &wndTop, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE );
	}
	
}
/*DWORD colFontBkgrnd_, colFontOutline_;
void Blur( HDC hdc, RECT *prc )
{
	int cx = prc->right, cy = prc->bottom;
	struct xy {
		int x,y;
	};
	xy tab[8] = { 
		{-1,-1}, {-1,0 }, {-1,1 },
		{0 , 1}, { 1, 1}, { 1, 0}, {1,-1}, {0,-1} };

	xy tab4[4] = { 
		{-1,-1}, {-1,1 },
		{ 1, 1}, {1,-1}, };

	xy tab1[1] = { 
		{-1,-1}, };
	xy tab2[2] = { 
		{-1,0}, {1,0} };

	for (int y=prc->top; y<cy; y++ )
	{
		for (int x=prc->left; x<cx; x++)
		{
			DWORD col = GetPixel( hdc, x,y );
			if ( col != colFontBkgrnd_ ) {
			float r= GetRValue(col), g=GetGValue(col), b=GetBValue(col);
			xy *pxy = tab4;
			for (int i=0; i<4; i++)
			{
				DWORD col_ = GetPixel( hdc, x+pxy->x,y+pxy->y );
				if ( col_!=colFontBkgrnd_ )
				{
				float r_= GetRValue(col_), g_=GetGValue(col_), b_=GetBValue(col_),
					m1=0.75, m2=1.0f-m1;


				col_ = RGB( r_*m1+r*m2, g_*m1+g*m2, b_*m1+b*m2 );
				SetPixel( hdc, x+pxy->x,y+pxy->y, col_ );
				}

				pxy++;
			}
			}

		}
	}
}*/

