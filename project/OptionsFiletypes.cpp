// OptionsFiletypes.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsFiletypes.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsFiletypes dialog


COptionsFiletypes::COptionsFiletypes(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent )   // standard constructor
	: CDialog(COptionsFiletypes::IDD, pParent),
	hIcon(NULL)
{
	//{{AFX_DATA_INIT(COptionsFiletypes)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	bRegisterOnStart = pOptions->bRegisterOnStart;
	iRegisteredExtsIcon = pOptions->iRegisteredExtsIcon;
}


void COptionsFiletypes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsFiletypes)
	DDX_Control(pDX, IDC_TEXTT_REGISTERONSTART, m_TTRegisterOnStart);
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	DDX_Control(pDX, IDC_TEXTT_TITLEASSOCIATED, m_TTTitleAssociated);
	DDX_Control(pDX, IDC_TEXTT_TITLEICON, m_TTTitleIcon);
	DDX_Control(pDX, IDC_SLIDER_ICON, m_SIcon);
	DDX_Control(pDX, IDC_STATIC_ICON, m_STIcon);
	DDX_Control(pDX, IDC_LIST_EXTENSIONS, m_LExtensions);
	DDX_Control(pDX, IDC_CHECK_REGISTERONSTART, m_CKRegisterOnStart);
	DDX_Control(pDX, IDC_BUTTON_NONE, m_BSelectNone);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_BDefault);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_BApply);
	DDX_Control(pDX, IDC_BUTTON_ALL, m_BSelectAll);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsFiletypes, CDialog)
	//{{AFX_MSG_MAP(COptionsFiletypes)
	ON_BN_CLICKED(IDC_BUTTON_ALL, OnButtonAll)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_BN_CLICKED(IDC_BUTTON_NONE, OnButtonNone)
	ON_BN_CLICKED(IDC_CHECK_REGISTERONSTART, OnCheckRegisteronstart)
	ON_BN_CLICKED(IDC_TEXTT_REGISTERONSTART, OnTexttRegisteronstart)
	ON_LBN_SELCHANGE(IDC_LIST_EXTENSIONS, OnSelchangeListExtensions)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsFiletypes message handlers

BOOL COptionsFiletypes::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RegisterFiletypes( pOptions, pOptions->szRegisteredExts );

	
	InitDialog();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsFiletypes::InitDialog()
{
	char *ext[] = { 
		"asf", "avi", "dat", "divx", "m1v", "mov", "mpa", 
			"mpe", "mpeg", "mpg", "wmv", "imp" };
	char *psz = new char [ sizeof(pOptions->szRegisteredExts)+1 ];
	strcpy( psz, pOptions->szRegisteredExts );
	char *p; int count = 0; 
	for ( p=psz; *p; p++ ) if (*p=='|') { *p=0; count++; }
	if ( *psz ) count++;

	for ( int i=0; i<sizeof(ext)/4; i++ )
	{
		m_LExtensions.AddString( ext[i] );

		p=psz;
		for (int j=0; j<count && *p && strcmp(ext[i],p); p+=strlen(p)+1, j++ ) ;
		if ( j<count ) m_LExtensions.SetSel( i, TRUE );
	}

	delete[] psz;

	m_CKRegisterOnStart.SetCheck( bRegisterOnStart );
	m_SIcon.SetTicFreq(1); 
	m_SIcon.SetPageSize(1);
	m_SIcon.SetRange( 1, 5 );
	m_SIcon.SetPos( iRegisteredExtsIcon );

	InitIcon();
	InitLanguage();
}

void COptionsFiletypes::InitIcon()
{
	if ( hIcon ) {
		DestroyIcon( hIcon );
		hIcon = NULL;
	}

	// This will be work correctly only if id of ICONS will be sort
	//  ascendig ( IDI_ICON_MOVIE01 must have lowest id, and
	//  IDI_ICON_MOVIE05 must have highest id ), because linker 
	//  add icons exe depend on icon id ( lowest add first ).
	LPCTSTR lpszName = NULL;
	switch ( iRegisteredExtsIcon ) {
		case 1: lpszName = MAKEINTRESOURCE(IDI_ICON_MOVIE01); break;
		case 2: lpszName = MAKEINTRESOURCE(IDI_ICON_MOVIE02); break;
		case 3: lpszName = MAKEINTRESOURCE(IDI_ICON_MOVIE03); break;
		case 4: lpszName = MAKEINTRESOURCE(IDI_ICON_MOVIE04); break;
		case 5: lpszName = MAKEINTRESOURCE(IDI_ICON_MOVIE05); break;
	}

	if ( !lpszName ) return;

	hIcon = (HICON)LoadImage( AfxGetApp()->m_hInstance,
		lpszName, IMAGE_ICON, 48, 48, LR_SHARED );

	m_STIcon.SetIcon( hIcon );
}

void COptionsFiletypes::OnButtonAll() 
{
	int count = m_LExtensions.GetCount();
	for ( int i=0; i<count; i++ )
		m_LExtensions.SetSel( i, TRUE );

	m_CKRegisterOnStart.SetCheck( bRegisterOnStart );
	
	m_BApply.EnableWindow( TRUE );
}


void COptionsFiletypes::OnButtonNone() 
{
	int count = m_LExtensions.GetCount();
	for ( int i=0; i<count; i++ )
		m_LExtensions.SetSel( i, FALSE );
	
	m_BApply.EnableWindow( TRUE );
}

void COptionsFiletypes::OnCheckRegisteronstart() 
{
	bRegisterOnStart = !bRegisterOnStart;

	m_CKRegisterOnStart.SetCheck( bRegisterOnStart );

	m_BApply.EnableWindow( TRUE );
}

void COptionsFiletypes::OnTexttRegisteronstart() 
{ OnCheckRegisteronstart(); }

void COptionsFiletypes::OnSelchangeListExtensions() 
{
	m_BApply.EnableWindow( TRUE );	
}


void COptionsFiletypes::OnButtonDefault() 
{
	bRegisterOnStart = defConfig.bRegisterOnStart;
	OnButtonAll();

	m_BApply.EnableWindow( TRUE );
}

void COptionsFiletypes::OnButtonApply() 
{
	char *psz = new char [ sizeof(pOptions->szRegisteredExts)+1 ];

	pOptions->iRegisteredExtsIcon = iRegisteredExtsIcon;

	// Register selected extensions.
	int count = m_LExtensions.GetCount(); *psz = 0;
	for (int i=0; i<count; i++ )
	{
		BOOL sel = m_LExtensions.GetSel(i);
		if ( sel ) {
			CString str; m_LExtensions.GetText(i,str);
			sprintf( psz+strlen(psz), "%s|", str );
		}
	}
	if ( *psz ) psz[ strlen(psz)-1 ] = 0;

	strcpy( pOptions->szRegisteredExts, psz );
	RegisterFiletypes( pOptions, pOptions->szRegisteredExts );


	// Unregister unselected extensions.
	for ( i=0, *psz = 0; i<count; i++ )
	{
		BOOL sel = m_LExtensions.GetSel(i);
		if ( !sel ) {
			CString str; m_LExtensions.GetText(i,str);
			sprintf( psz+strlen(psz), "%s|", str );
		}
	}
	if ( *psz ) psz[ strlen(psz)-1 ] = 0;

	UnregisterFiletypes( pOptions, psz );

	delete[] psz;

	m_BApply.EnableWindow( FALSE );
}

void COptionsFiletypes::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ( pScrollBar != (CScrollBar*)&m_SIcon ) return;

	

	iRegisteredExtsIcon = m_SIcon.GetPos();

	InitIcon();
	m_BApply.EnableWindow( TRUE );
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL COptionsFiletypes::DestroyWindow() 
{
	if ( hIcon ) {
		DestroyIcon( hIcon );
		hIcon = NULL;
	}
	
	return CDialog::DestroyWindow();
}

void COptionsFiletypes::InitLanguage()
{
	m_BApply.SetWindowText( transl(IDC_BUTTON_APPLY, TRANSL_OPTIONS) );
	m_BDefault.SetWindowText( transl(IDC_BUTTON_DEFAULT, TRANSL_OPTIONS) );
	
	m_TTTitle.SetWindowText( transl(IDD_PP_FILETYPES, TRANSL_OPTIONS) );
	m_TTTitleAssociated.SetWindowText( transl(IDC_TEXTT_TITLEASSOCIATED, TRANSL_PP_FILETYPES) );
	m_TTTitleIcon.SetWindowText( transl(IDC_TEXTT_TITLEICON, TRANSL_PP_FILETYPES) );
	m_TTRegisterOnStart.SetWindowText( transl(IDC_TEXTT_REGISTERONSTART, TRANSL_PP_FILETYPES) );
	m_BSelectAll.SetWindowText( transl(IDC_BUTTON_ALL, TRANSL_PP_FILETYPES) );
	m_BSelectNone.SetWindowText( transl(IDC_BUTTON_NONE, TRANSL_PP_FILETYPES) );

}
