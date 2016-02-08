// OptionsSubtitles.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsSubtitles.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitles dialog


COptionsSubtitles::COptionsSubtitles(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent /*=NULL*/)
	: CDialog(COptionsSubtitles::IDD, pParent),
	hFont(NULL)
{
	//{{AFX_DATA_INIT(COptionsSubtitles)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	iSubtitlesMode = pOptions->iSubtitlesMode;
	bSubtitlesBkgrndShort =  pOptions->bSubtitlesBkgrndShort;
	bSubtitlesRgnPrecise = pOptions->bSubtitlesRgnPrecise;
	
	iSbtFontSize = pOptions->iSbtFontSize;
	iSbtFontWeight = pOptions->iSbtFontWeight;
	iSbtFontOutline = pOptions->iSbtFontOutline;
	bSbtFontBkgrndTransparent = pOptions->bSbtFontBkgrndTransparent;
	colSbtFont = pOptions->colSbtFont;
	colSbtFontBkgrnd = pOptions->colSbtFontBkgrnd;
	colSbtFontOutline = pOptions->colSbtFontOutline;

	strcpy( szSbtFontName, pOptions->szSbtFontName );

	strcpy( szExampleText, "Example text" );

}


void COptionsSubtitles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsSubtitles)
	DDX_Control(pDX, IDC_TEXT_OUTLINECOL, m_TOutlinecol);
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	DDX_Control(pDX, IDC_CHECK_TRANSPARENTBKGRND, m_CKTranspBkgrnd);
	DDX_Control(pDX, IDC_COMBO_MODESEL, m_CModesel);
	DDX_Control(pDX, IDC_COMBO_OUTLINESEL, m_COutlinesel);
	DDX_Control(pDX, IDC_TEXT_BKGRNDCOL, m_TBkgrndcol);
	DDX_Control(pDX, IDC_TEXTT_OUTLINESEL, m_TTOutlinesel);
	DDX_Control(pDX, IDC_TEXTT_MODESEL, m_TTModesel);
	DDX_Control(pDX, IDC_TEXT_FONTCOL, m_TFontcol);
	DDX_Control(pDX, IDC_CHECK_SHORTBKGRND, m_CKShortbkgrnd);
	DDX_Control(pDX, IDC_CHECK_RGNPRECISE, m_CKRgnprecise);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_BDefault);
	DDX_Control(pDX, IDC_BUTTON_CHANGEFONT, m_BChangeFont);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_BApply);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsSubtitles, CDialog)
	//{{AFX_MSG_MAP(COptionsSubtitles)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHECK_RGNPRECISE, OnCheckRgnprecise)
	ON_BN_CLICKED(IDC_CHECK_SHORTBKGRND, OnCheckShortbkgrnd)
	ON_BN_CLICKED(IDC_CHECK_TRANSPARENTBKGRND, OnCheckTransparentbkgrnd)
	ON_CBN_SELCHANGE(IDC_COMBO_MODESEL, OnSelchangeComboModesel)
	ON_CBN_SELCHANGE(IDC_COMBO_OUTLINESEL, OnSelchangeComboOutlinesel)
	ON_BN_CLICKED(IDC_BUTTON_CHANGEFONT, OnButtonChangefont)
	ON_BN_CLICKED(IDC_TEXT_BKGRNDCOL, OnTextBkgrndcol)
	ON_BN_CLICKED(IDC_TEXT_FONTCOL, OnTextFontcol)
	ON_BN_CLICKED(IDC_TEXT_OUTLINECOL, OnTextOutlinecol)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitles message handlers
HBRUSH hBrush=NULL;
HBRUSH COptionsSubtitles::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if ( pWnd==&m_TFontcol || pWnd==&m_TOutlinecol || pWnd==&m_TBkgrndcol )
	{
		if ( hBrush) DeleteObject( hBrush );
		DWORD col = colSbtFont;
		if (pWnd==&m_TOutlinecol) col = colSbtFontOutline;
		else if (pWnd==&m_TBkgrndcol) col = colSbtFontBkgrnd;

		hBrush = CreateSolidBrush( col );
		hbr = hBrush;
		int r=255-GetRValue(col), g=255-GetGValue(col), b = 255-GetBValue(col);
		pDC->SetTextColor( RGB(r,g,b) );
		pDC->SetBkMode( TRANSPARENT );
	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL COptionsSubtitles::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetClassLong( m_TFontcol.m_hWnd, GCL_HCURSOR, 
		(DWORD)AfxGetApp()->LoadStandardCursor(IDC_HAND) );
	SetClassLong( m_TOutlinecol.m_hWnd, GCL_HCURSOR, 
		(DWORD)AfxGetApp()->LoadStandardCursor(IDC_HAND) );
	SetClassLong( m_TBkgrndcol.m_hWnd, GCL_HCURSOR, 
		(DWORD)AfxGetApp()->LoadStandardCursor(IDC_HAND) );

	SetClassLong( m_hWnd, GCL_HCURSOR, 
		(DWORD)AfxGetApp()->LoadStandardCursor(IDC_ARROW) );

	InitLanguage();
	InitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsSubtitles::InitDialog()
{
	int sel = 0;
	switch ( iSubtitlesMode )
	{
		case SUBTITLES_OVERLAY: sel = 0; break;
		case SUBTITLES_RGN: sel = 1; break;
	}
	m_CModesel.SetCurSel( sel );

	switch ( iSbtFontOutline )
	{
		case FO_NONE: sel = 0; break;
		case FO_THIN: sel = 1; break;
		case FO_BOLD: sel = 2; break;
	}
	m_COutlinesel.SetCurSel( sel );

	m_CKTranspBkgrnd.SetCheck( bSbtFontBkgrndTransparent );
	m_CKShortbkgrnd.SetCheck( bSubtitlesBkgrndShort );
	m_CKRgnprecise.SetCheck( bSubtitlesRgnPrecise );

	SetDepedencies();
}

void COptionsSubtitles::DrawExampleText( HDC hdc, BOOL bChanged )
{
	HDC hDC = hdc;
	if ( !hDC ) hDC = ::GetDC( m_hWnd );

	if ( bChanged && hFont )  {
		DeleteObject( hFont ); hFont = NULL;
	}

	RECT rc = { 15, 15, 360, 92 };

	HBRUSH hbrBkgrnd;
	if ( bSbtFontBkgrndTransparent )
		hbrBkgrnd = CreateSolidBrush( 0x100010 );
	else
		hbrBkgrnd = CreateSolidBrush( colSbtFontBkgrnd );

	FillRect( hDC, &rc, hbrBkgrnd );


	HRGN hrgnClip = CreateRectRgnIndirect( &rc );
	SelectClipRgn( hDC, hrgnClip ); ::DeleteObject( hrgnClip );


	LOGFONT logfont= {

		-MulDiv(iSbtFontSize, GetDeviceCaps(hDC, LOGPIXELSY), 72), //lfHeight
		0, 0, 0,							//lfWidth, lfEscapement, lfOrientation
		iSbtFontWeight,						//lfWeight(def)
		FALSE, FALSE, FALSE,			//lfItalic, lfUnderline, lfStrikeOut
		DEFAULT_CHARSET,				//lfCharSet 
		OUT_DEFAULT_PRECIS,			//lfOutPrecision 
		CLIP_DEFAULT_PRECIS,			//lfClipPrecision 
		ANTIALIASED_QUALITY,//DEFAULT_QUALITY,				//lfQuality 
		DEFAULT_PITCH|FF_DONTCARE	//lfPitchAndFamily 

	}; strcpy( logfont.lfFaceName, szSbtFontName );
	memcpy( &LogFont, &logfont, sizeof(logfont) );

	if ( hFont ) DeleteObject( hFont );
	hFont = CreateFontIndirect( &logfont );
	SelectObject( hDC, hFont );
	SetBkMode( hDC, TRANSPARENT );

	SIZE sizeText;
	GetTextExtentPoint32( hDC, szExampleText, 
		strlen(szExampleText), &sizeText );

	char *psz = szExampleText;
	int x = rc.left+((RECTW(rc)-sizeText.cx)/2),
		 y = rc.top+((RECTH(rc)-sizeText.cy)/2),
		 len = strlen(szExampleText);

	SetTextColor( hDC, colSbtFontOutline );
	switch ( iSbtFontOutline )
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
	SetTextColor( hDC, colSbtFont );
	TextOut( hDC, x,y,psz,len);



	SelectClipRgn( hDC, NULL );
	::DeleteObject( hbrBkgrnd );

	if ( !hdc ) ::ReleaseDC( m_hWnd, hDC );
}

void COptionsSubtitles::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawExampleText( dc.m_hDC );
	
	// Do not call CDialog::OnPaint() for painting messages
}

void COptionsSubtitles::OnCheckRgnprecise() 
{
	bSubtitlesRgnPrecise = !bSubtitlesRgnPrecise;
	DrawExampleText(NULL,TRUE);
	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitles::OnCheckShortbkgrnd() 
{
	bSubtitlesBkgrndShort = !bSubtitlesBkgrndShort;
	DrawExampleText(NULL,TRUE);	
	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitles::OnCheckTransparentbkgrnd() 
{
	bSbtFontBkgrndTransparent = !bSbtFontBkgrndTransparent;
	DrawExampleText(NULL,TRUE);
	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitles::OnSelchangeComboModesel() 
{
	int sel = m_CModesel.GetCurSel();
	switch ( sel )
	{
		case 0: iSubtitlesMode = SUBTITLES_OVERLAY; break;
		case 1: iSubtitlesMode = SUBTITLES_RGN; break;
	}
	DrawExampleText(NULL,TRUE);

	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitles::OnSelchangeComboOutlinesel() 
{
	int sel = m_COutlinesel.GetCurSel();
	switch ( sel )
	{
		case 0: iSbtFontOutline = FO_NONE; break;
		case 1: iSbtFontOutline = FO_THIN; break;
		case 2: iSbtFontOutline = FO_BOLD; break;
	}
	DrawExampleText(NULL,TRUE);
	
	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitles::SetDepedencies()
{
	if ( bSbtFontBkgrndTransparent )
		m_CKShortbkgrnd.EnableWindow( FALSE );
	else
		m_CKShortbkgrnd.EnableWindow( TRUE );

	if ( iSubtitlesMode == SUBTITLES_RGN )
		m_CKRgnprecise.EnableWindow( TRUE );
	else
		m_CKRgnprecise.EnableWindow( FALSE );
}

void COptionsSubtitles::OnButtonChangefont() 
{
	CFontDialog dlgFont( &LogFont, CF_SCREENFONTS );

	if ( dlgFont.DoModal()!=IDOK ) return;

	iSbtFontSize = dlgFont.GetSize()/10;
	iSbtFontWeight = dlgFont.GetWeight();

	strcpy( szSbtFontName, dlgFont.GetFaceName() );
	
	DrawExampleText(NULL,TRUE);
	
	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitles::OnTextBkgrndcol() 
{
	CColorDialog dlgColor( colSbtFontBkgrnd, CC_RGBINIT|CC_ANYCOLOR );

	if ( dlgColor.DoModal()!=IDOK ) return;

	colSbtFontBkgrnd  = dlgColor.GetColor();
	m_TBkgrndcol.InvalidateRect(NULL); m_TBkgrndcol.UpdateWindow();

	DrawExampleText(NULL,TRUE);
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitles::OnTextFontcol() 
{
	CColorDialog dlgColor( colSbtFont, CC_RGBINIT|CC_ANYCOLOR );

	if ( dlgColor.DoModal()!=IDOK ) return;

	colSbtFont = dlgColor.GetColor();
	m_TFontcol.InvalidateRect(NULL); m_TFontcol.UpdateWindow();

	DrawExampleText(NULL,TRUE);
	m_BApply.EnableWindow( TRUE );
	
}

void COptionsSubtitles::OnTextOutlinecol() 
{
	CColorDialog dlgColor( colSbtFontOutline, CC_RGBINIT|CC_ANYCOLOR );

	if ( dlgColor.DoModal()!=IDOK ) return;

	colSbtFontOutline = dlgColor.GetColor();
	m_TOutlinecol.InvalidateRect(NULL); m_TOutlinecol.UpdateWindow();

	DrawExampleText(NULL,TRUE);
	m_BApply.EnableWindow( TRUE );	
}

void COptionsSubtitles::OnButtonApply() 
{

	m_BApply.EnableWindow( FALSE );

	pOptions->iSubtitlesMode = iSubtitlesMode;
	pOptions->bSubtitlesBkgrndShort = bSubtitlesBkgrndShort;
	pOptions->bSubtitlesRgnPrecise = bSubtitlesRgnPrecise;
	
	pOptions->iSbtFontSize = iSbtFontSize;
	pOptions->iSbtFontWeight = iSbtFontWeight;
	pOptions->iSbtFontOutline = iSbtFontOutline;
	pOptions->bSbtFontBkgrndTransparent = bSbtFontBkgrndTransparent;
	pOptions->colSbtFont = colSbtFont;
	pOptions->colSbtFontBkgrnd = colSbtFontBkgrnd;
	pOptions->colSbtFontOutline = colSbtFontOutline;

	strcpy( pOptions->szSbtFontName, szSbtFontName );
	
	pControlWnd->PostMessage( WM_COMMAND, IDM_CALLBACK_FONTCHANGED, 0 );
}

void COptionsSubtitles::OnButtonDefault() 
{
	iSubtitlesMode = defConfig.iSubtitlesMode;
	bSubtitlesBkgrndShort = defConfig.bSubtitlesBkgrndShort;
	bSubtitlesRgnPrecise = defConfig.bSubtitlesRgnPrecise;

	strcpy( szSbtFontName, defConfig.szSbtFontName );
	iSbtFontSize = defConfig.iSbtFontSize;
	iSbtFontWeight = defConfig.iSbtFontWeight;
	iSbtFontOutline = defConfig.iSbtFontOutline;
	bSbtFontBkgrndTransparent = defConfig.bSbtFontBkgrndTransparent;
	colSbtFont =  defConfig.colSbtFont;
	colSbtFontBkgrnd = defConfig.colSbtFontBkgrnd;
	colSbtFontOutline = defConfig.colSbtFontOutline;

	InitDialog();
	m_BApply.EnableWindow( TRUE );

	InvalidateRect(NULL); UpdateWindow();
}

void COptionsSubtitles::InitLanguage()
{
	TRANSLW( m_BApply, IDC_BUTTON_APPLY, TRANSL_OPTIONS );
	TRANSLW( m_BDefault, IDC_BUTTON_DEFAULT, TRANSL_OPTIONS );
	
	TRANSLW( m_TTTitle, IDD_PP_SUBTITLES, TRANSL_OPTIONS );

	int sect = TRANSL_PP_SUBTITLES;
	TRANSLW( m_BChangeFont, IDC_BUTTON_CHANGEFONT, sect );
	TRANSLW( m_TFontcol, IDC_TEXT_FONTCOL, sect );
	TRANSLW( m_TOutlinecol, IDC_TEXT_OUTLINECOL, sect );
	TRANSLW( m_TBkgrndcol, IDC_TEXT_BKGRNDCOL, sect );
	TRANSLW( m_TTModesel, IDC_TEXTT_MODESEL, sect );
	TRANSLW( m_TTOutlinesel, IDC_TEXTT_OUTLINESEL, sect );
	TRANSLW( m_CKTranspBkgrnd, IDC_CHECK_TRANSPARENTBKGRND, sect );
	TRANSLW( m_CKShortbkgrnd, IDC_CHECK_SHORTBKGRND, sect );
	TRANSLW( m_CKRgnprecise, IDC_CHECK_RGNPRECISE, sect );

	m_CModesel.ResetContent();
	m_CModesel.AddString( transl(0, sect) );
	m_CModesel.AddString( transl(1, sect) );

	m_COutlinesel.ResetContent();
	m_COutlinesel.AddString( transl(2, sect) );
	m_COutlinesel.AddString( transl(3, sect) );
	m_COutlinesel.AddString( transl(4, sect) );

	ZeroMemory( szExampleText, sizeof(szExampleText) );
	strncpy( szExampleText, transl(5, sect), 63 );

}
