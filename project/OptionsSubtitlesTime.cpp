// OptionsSubtitlesTime.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsSubtitlesTime.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitlesTime dialog


COptionsSubtitlesTime::COptionsSubtitlesTime(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent )   // standard constructor
	: CDialog(COptionsSubtitlesTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsSubtitlesTime)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	bSubtitlesLoaded = pOptions->bSubtitlesLoaded;
	bSubtitlesOffsetTime = pOptions->bSubtitlesOffsetTime;
	valSubtitlesOffsetTime = pOptions->valSubtitlesOffsetTime;

}


void COptionsSubtitlesTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsSubtitlesTime)
	DDX_Control(pDX, IDC_TEXTT_TITLEOFFSET, m_TTTitleOffset);
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	DDX_Control(pDX, IDC_RADIO_SPEEDUP, m_RSpeedUp);
	DDX_Control(pDX, IDC_RADIO_DELAY, m_RDelay);
	DDX_Control(pDX, IDC_TEXTT2_TIME, m_TT2Time);
	DDX_Control(pDX, IDC_TEXTT_TIME, m_TTTime);
	DDX_Control(pDX, IDC_DATETIME_TIME, m_DTTime);
	DDX_Control(pDX, IDC_CHECK_OFFSET, m_CKOffset);
	DDX_Control(pDX, IDC_BUTTON_SAVEAS, m_BSaveAs);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_BDefault);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_BApply);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsSubtitlesTime, CDialog)
	//{{AFX_MSG_MAP(COptionsSubtitlesTime)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_BN_CLICKED(IDC_BUTTON_SAVEAS, OnButtonSaveas)
	ON_BN_CLICKED(IDC_CHECK_OFFSET, OnCheckOffset)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIME_TIME, OnDatetimechangeDatetimeTime)
	ON_BN_CLICKED(IDC_RADIO_DELAY, OnRadioDelay)
	ON_BN_CLICKED(IDC_RADIO_SPEEDUP, OnRadioSpeedup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitlesTime message handlers

void COptionsSubtitlesTime::OnButtonApply() 
{

	pOptions->bSubtitlesLoaded = bSubtitlesLoaded;
	pOptions->bSubtitlesOffsetTime = bSubtitlesOffsetTime;
	pOptions->valSubtitlesOffsetTime = valSubtitlesOffsetTime;

	pControlWnd->PostMessage( WM_COMMAND,
		IDM_CALLBACK_OFFSETSUBTITLESCHANGED, 0 );

	if ( bSubtitlesLoaded && bSubtitlesOffsetTime 
		&& valSubtitlesOffsetTime != 0 )
		m_BSaveAs.EnableWindow( TRUE );

	m_BApply.EnableWindow( FALSE );
}

void COptionsSubtitlesTime::OnButtonDefault() 
{
	bSubtitlesLoaded = defConfig.bSubtitlesLoaded;
	bSubtitlesOffsetTime = defConfig.bSubtitlesOffsetTime;
	valSubtitlesOffsetTime = defConfig.valSubtitlesOffsetTime;

	InitDialog();
	m_BApply.EnableWindow();
}

void COptionsSubtitlesTime::OnButtonSaveas() 
{
	if ( bSubtitlesLoaded  ) 
	{
		pControlWnd->PostMessage( WM_COMMAND,
			IDM_CALLBACK_SAVESUBTITLES, 0 );
		
	}
}

void COptionsSubtitlesTime::OnCheckOffset() 
{
	bSubtitlesOffsetTime	= !bSubtitlesOffsetTime;

	SetDepedencies();
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitlesTime::OnDatetimechangeDatetimeTime(NMHDR* pNMHDR, LRESULT* pResult) 
{

	CTime time(2000,1,1,0,0,0);
	m_DTTime.GetTime( time );

	int secs = time.GetSecond() + time.GetMinute()*60 + time.GetHour()*3600;
	if ( m_RDelay.GetCheck() ) secs = -secs;

	valSubtitlesOffsetTime = (float)secs;

	if ( valSubtitlesOffsetTime!=0 && 
		  ( m_RSpeedUp.GetCheck() || m_RDelay.GetCheck() ) )
		m_BApply.EnableWindow( TRUE );
	
	*pResult = 0;
}

void COptionsSubtitlesTime::OnRadioDelay() 
{
	m_RSpeedUp.SetCheck( !m_RDelay.GetCheck() );

	valSubtitlesOffsetTime = (float)abs( (int)valSubtitlesOffsetTime );
	if ( m_RDelay.GetCheck() )
		valSubtitlesOffsetTime = -valSubtitlesOffsetTime;

	if ( valSubtitlesOffsetTime!=0 && 
		  ( m_RSpeedUp.GetCheck() || m_RDelay.GetCheck() ) )
		m_BApply.EnableWindow( TRUE );


	
}

void COptionsSubtitlesTime::OnRadioSpeedup() 
{
	m_RDelay.SetCheck( !m_RSpeedUp.GetCheck() );	

	valSubtitlesOffsetTime = (float)abs( (int)valSubtitlesOffsetTime );
	if ( m_RDelay.GetCheck() )
		valSubtitlesOffsetTime = -valSubtitlesOffsetTime;

	if ( valSubtitlesOffsetTime!=0 && 
		  ( m_RSpeedUp.GetCheck() || m_RDelay.GetCheck() ) )
		m_BApply.EnableWindow( TRUE );
}

BOOL COptionsSubtitlesTime::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitLanguage();
	InitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsSubtitlesTime::InitDialog()
{
	int secs = abs((int)valSubtitlesOffsetTime),
		h = (secs / 3600),
		m = (secs % 3600) / 60,
		s = (secs % 3600) % 60;
	CTime time(2000,1,1,h,m,s);
	m_DTTime.SetTime( &time );

	if ( valSubtitlesOffsetTime >= 0 )
	{
		m_RSpeedUp.SetCheck( TRUE );
		m_RDelay.SetCheck( FALSE );
	}
	else
	{
		m_RSpeedUp.SetCheck( FALSE );
		m_RDelay.SetCheck( TRUE );
	} 

	m_CKOffset.SetCheck( bSubtitlesOffsetTime );

	SetDepedencies();
}

void COptionsSubtitlesTime::SetDepedencies()
{
	if ( bSubtitlesOffsetTime )
	{
		m_DTTime.EnableWindow( TRUE );

		m_RDelay.EnableWindow( TRUE );
		m_RSpeedUp.EnableWindow( TRUE );

		m_TTTime.EnableWindow( TRUE );
		m_TT2Time.EnableWindow( TRUE );

		if ( bSubtitlesLoaded && bSubtitlesOffsetTime 
			&& valSubtitlesOffsetTime != 0 )
			m_BSaveAs.EnableWindow( TRUE );
		else
			m_BSaveAs.EnableWindow( FALSE );


	}
	else
	{
		m_DTTime.EnableWindow( FALSE );

		m_RDelay.EnableWindow( FALSE );
		m_RSpeedUp.EnableWindow( FALSE );

		m_TTTime.EnableWindow( FALSE );
		m_TT2Time.EnableWindow( FALSE );

		m_BSaveAs.EnableWindow( FALSE );
	}

}

BOOL COptionsSubtitlesTime::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch ( wParam )
	{
		case IDM_CALLBACK_TRIEDLOADSUBTITLES:
			bSubtitlesLoaded = pOptions->bSubtitlesLoaded;
			InitDialog();
			break;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

void COptionsSubtitlesTime::InitLanguage()
{
	TRANSLW( m_BApply, IDC_BUTTON_APPLY, TRANSL_OPTIONS );
	TRANSLW( m_BDefault, IDC_BUTTON_DEFAULT, TRANSL_OPTIONS );
	
	TRANSLW( m_TTTitle, IDD_PP_SUBTITLES_TIME, TRANSL_OPTIONS );

	int sect = TRANSL_PP_SUBTITLES_TIME;
	TRANSLW( m_CKOffset, IDC_CHECK_OFFSET, sect );
	TRANSLW( m_RSpeedUp, IDC_RADIO_SPEEDUP, sect );
	TRANSLW( m_RDelay, IDC_RADIO_DELAY, sect );
	TRANSLW( m_TTTime, IDC_TEXTT_TIME, sect );
	TRANSLW( m_TT2Time, IDC_TEXTT2_TIME, sect );
	TRANSLW( m_BSaveAs, IDC_BUTTON_SAVEAS, sect );
}
