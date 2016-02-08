// OptionsStartup.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsStartup.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsStartup dialog


COptionsStartup::COptionsStartup(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent )   // standard constructor
	: CDialog(COptionsStartup::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsStartup)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	modeIdStartWhenMovie = pOptions->modeIdStartWhenMovie;
	modeIdStartWhenNoMovie = pOptions->modeIdStartWhenNoMovie;

}


void COptionsStartup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsStartup)
	DDX_Control(pDX, IDC_TEXTT2_OPENWITHMOVIE, m_TT2OpenWithMovie);
	DDX_Control(pDX, IDC_TEXTT2_OPENONMOVIE, m_TT2OpenNoMovie);
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	DDX_Control(pDX, IDC_TEXTT_OPENWITHMOVIE, m_TTOpenWithMovie);
	DDX_Control(pDX, IDC_TEXTT_OPENONMOVIE, m_TTOpenNoMovie);
	DDX_Control(pDX, IDC_COMBO_OPENWITHMOVIE, m_COpenWithMovie);
	DDX_Control(pDX, IDC_COMBO_OPENNOMOVIE, m_COpenNoMovie);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_BDefault);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_BApply);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsStartup, CDialog)
	//{{AFX_MSG_MAP(COptionsStartup)
	ON_CBN_SELCHANGE(IDC_COMBO_OPENNOMOVIE, OnSelchangeComboOpennomovie)
	ON_CBN_SELCHANGE(IDC_COMBO_OPENWITHMOVIE, OnSelchangeComboOpenwithmovie)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsStartup message handlers

void COptionsStartup::OnSelchangeComboOpennomovie() 
{
	int cur = m_COpenNoMovie.GetCurSel();
	switch ( cur )
	{
		case 0: modeIdStartWhenNoMovie = MODE_NORMAL; break;
		case 1: modeIdStartWhenNoMovie = MODE_COMPACT; break;
		case 2: modeIdStartWhenNoMovie = MODE_FULLSCREEN; break;
		case 3: modeIdStartWhenNoMovie = MODE_NONE; break;
	}

	m_BApply.EnableWindow( TRUE );	
}

void COptionsStartup::OnSelchangeComboOpenwithmovie() 
{
	int cur = m_COpenWithMovie.GetCurSel();
	switch ( cur )
	{
		case 0: modeIdStartWhenMovie = MODE_NORMAL; break;
		case 1: modeIdStartWhenMovie = MODE_COMPACT; break;
		case 2: modeIdStartWhenMovie = MODE_FULLSCREEN; break;
		case 3: modeIdStartWhenMovie = MODE_NONE; break;
	}

	m_BApply.EnableWindow( TRUE );
}

BOOL COptionsStartup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	InitLanguage();
	InitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsStartup::InitDialog()
{

	int cur;
	switch ( modeIdStartWhenNoMovie )
	{
		case MODE_NORMAL: cur = 0; break;
		case MODE_COMPACT: cur = 1; break;
		case MODE_FULLSCREEN: cur = 2; break;
		case MODE_NONE: cur = 3; break;
	}

	m_COpenNoMovie.SetCurSel( cur );
	
	switch ( modeIdStartWhenMovie )
	{
		case MODE_NORMAL: cur = 0; break;
		case MODE_COMPACT: cur = 1; break;
		case MODE_FULLSCREEN: cur = 2; break;
		case MODE_NONE: cur = 3; break;
	}

	m_COpenWithMovie.SetCurSel( cur );

}

void COptionsStartup::OnButtonApply() 
{
	pOptions->modeIdStartWhenMovie = modeIdStartWhenMovie;
	pOptions->modeIdStartWhenNoMovie = modeIdStartWhenNoMovie;

	m_BApply.EnableWindow( FALSE );
	
}

void COptionsStartup::OnButtonDefault() 
{
	modeIdStartWhenMovie = defConfig.modeIdStartWhenMovie;
	modeIdStartWhenNoMovie = defConfig.modeIdStartWhenNoMovie;

	InitDialog();
	m_BApply.EnableWindow( TRUE );
}

void COptionsStartup::InitLanguage()
{
	TRANSLW( m_BApply, IDC_BUTTON_APPLY, TRANSL_OPTIONS );
	TRANSLW( m_BDefault, IDC_BUTTON_DEFAULT, TRANSL_OPTIONS );
	
	TRANSLW( m_TTTitle, IDD_PP_STARTUP, TRANSL_OPTIONS );

	int sect = TRANSL_PP_STARTUP;
	TRANSLW( m_TTOpenNoMovie, IDC_TEXTT_OPENONMOVIE, sect );
	TRANSLW( m_TTOpenWithMovie, IDC_TEXTT_OPENONMOVIE, sect );
	TRANSLW( m_TT2OpenNoMovie, IDC_TEXTT2_OPENONMOVIE, sect );
	TRANSLW( m_TT2OpenWithMovie, IDC_TEXTT2_OPENWITHMOVIE, sect );

	m_COpenNoMovie.ResetContent();
	m_COpenNoMovie.AddString( transl( 0, sect) );
	m_COpenNoMovie.AddString( transl( 1, sect) );
	m_COpenNoMovie.AddString( transl( 2, sect) );
	m_COpenNoMovie.AddString( transl( 3, sect) );

	m_COpenWithMovie.ResetContent();
	m_COpenWithMovie.AddString( transl( 0, sect) );
	m_COpenWithMovie.AddString( transl( 1, sect) );
	m_COpenWithMovie.AddString( transl( 2, sect) );
	m_COpenWithMovie.AddString( transl( 3, sect) );
}
