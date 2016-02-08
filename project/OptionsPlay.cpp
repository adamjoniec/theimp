// OptionsPlay.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsPlay.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsPlay dialog


COptionsPlay::COptionsPlay(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent )   // standard constructor
	: CDialog(COptionsPlay::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsPlay)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	bFastSeek = pOptions->bFastSeek;
	bFastRewind = pOptions->bFastRewind;
	bAutoRewind = pOptions->bAutoRewind;
	bLoop = pOptions->bLoop;
	bAutoPlay = pOptions->bAutoPlay;
	dRewindSecs = pOptions->dRewindSecs;
}


void COptionsPlay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsPlay)
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	DDX_Control(pDX, IDC_TEXTT2_REWINDBY, m_TT2RewindBy);
	DDX_Control(pDX, IDC_TEXTT_TITLESEEKING, m_TTTitleSeeking);
	DDX_Control(pDX, IDC_TEXTT_TITLEGENERAL, m_TTTitleGeneral);
	DDX_Control(pDX, IDC_TEXTT_REWINDBY, m_TTRewindBy);
	DDX_Control(pDX, IDC_TEXTT_FASTSEEK, m_TTFastSeek);
	DDX_Control(pDX, IDC_SPIN_REWINDBY, m_SPRewind);
	DDX_Control(pDX, IDC_EDIT_REWINDBY, m_ERewind);
	DDX_Control(pDX, IDC_CHECK_LOOP, m_CKLoop);
	DDX_Control(pDX, IDC_CHECK_FASTSEEK, m_CKFastseek);
	DDX_Control(pDX, IDC_CHECK_AUTOREWIND, m_CKAutorewind);
	DDX_Control(pDX, IDC_CHECK_AUTOPLAY, m_CKAutoplay);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_BDefault);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_BApply);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsPlay, CDialog)
	//{{AFX_MSG_MAP(COptionsPlay)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_BN_CLICKED(IDC_CHECK_AUTOPLAY, OnCheckAutoplay)
	ON_BN_CLICKED(IDC_CHECK_AUTOREWIND, OnCheckAutorewind)
	ON_BN_CLICKED(IDC_CHECK_FASTSEEK, OnCheckFastseek)
	ON_BN_CLICKED(IDC_CHECK_LOOP, OnCheckLoop)
	ON_EN_CHANGE(IDC_EDIT_REWINDBY, OnChangeEditRewindby)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsPlay message handlers

BOOL COptionsPlay::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ERewind.SetLimitText( 3 ); m_SPRewind.SetRange( 1, 180 );

	InitDialog();
	InitLanguage();

	m_BApply.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsPlay::InitDialog()
{
	m_CKFastseek.SetCheck( bFastSeek );
	m_CKLoop.SetCheck( bLoop );
	m_CKFastseek.SetCheck( bFastSeek );
	m_CKAutoplay.SetCheck( bAutoPlay );
	m_CKAutorewind.SetCheck( bAutoRewind );

	m_SPRewind.SetPos( (int)dRewindSecs );

	SetDepedencies();

}

void COptionsPlay::SetDepedencies()
{

	if ( bLoop )
		m_CKAutorewind.EnableWindow( FALSE );
	else
		m_CKAutorewind.EnableWindow( TRUE );
}

void COptionsPlay::OnButtonApply() 
{
	pOptions->bFastSeek = bFastSeek;
	pOptions->bFastRewind = bFastRewind;
	pOptions->bAutoRewind = bAutoRewind;
	pOptions->bLoop = bLoop;
	pOptions->bAutoPlay = bAutoPlay;
	pOptions->dRewindSecs = dRewindSecs;

	m_BApply.EnableWindow( FALSE );
}

void COptionsPlay::OnButtonDefault() 
{
	bFastSeek = defConfig.bFastSeek;
	bFastRewind = defConfig.bFastRewind;
	bAutoRewind = defConfig.bAutoRewind;
	bLoop = defConfig.bLoop;
	bAutoPlay = defConfig.bAutoPlay;
	dRewindSecs = defConfig.dRewindSecs;

	InitDialog();
	m_BApply.EnableWindow( TRUE );
}

void COptionsPlay::OnCheckAutoplay() 
{
	bAutoPlay = !bAutoPlay;

	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsPlay::OnCheckAutorewind() 
{
	bAutoRewind	= !bAutoRewind;

	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsPlay::OnCheckFastseek() 
{
	bFastSeek = !bFastSeek;

	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsPlay::OnCheckLoop() 
{
	bLoop = !bLoop;
	
	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsPlay::OnChangeEditRewindby() 
{
	if ( !m_SPRewind ) return;
	int min=1, max=180;

	int ival = m_SPRewind.GetPos();

	if ( ival<min ) ival = min;
	if ( ival>max ) ival = min;

	m_SPRewind.SetPos( ival );

	dRewindSecs = ival;

	m_SPRewind.SetPos( ival );
	m_BApply.EnableWindow( TRUE );
	
}

void COptionsPlay::InitLanguage()
{
	TRANSLW( m_BApply, IDC_BUTTON_APPLY, TRANSL_OPTIONS );
	TRANSLW( m_BDefault, IDC_BUTTON_DEFAULT, TRANSL_OPTIONS );
	
	TRANSLW( m_TTTitle, IDD_PP_PLAY, TRANSL_OPTIONS );

	int sect = TRANSL_PP_PLAY;
	TRANSLW( m_TTTitleSeeking, IDC_TEXTT_TITLESEEKING, sect );
	TRANSLW( m_TTRewindBy, IDC_TEXTT_REWINDBY, sect );
	TRANSLW( m_TT2RewindBy, IDC_TEXTT2_REWINDBY, sect );
	TRANSLW( m_CKFastseek, IDC_CHECK_FASTSEEK, sect );
	TRANSLW( m_TTFastSeek, IDC_TEXTT_FASTSEEK, sect );
	TRANSLW( m_TTTitleGeneral, IDC_TEXTT_TITLEGENERAL, sect );
	TRANSLW( m_CKLoop, IDC_CHECK_LOOP, sect );
	TRANSLW( m_CKAutorewind, IDC_CHECK_AUTOREWIND, sect );
	TRANSLW( m_CKAutoplay, IDC_CHECK_AUTOPLAY, sect );
}
