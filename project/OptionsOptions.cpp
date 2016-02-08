// OptionsOptions.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsOptions.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsOptions dialog


COptionsOptions::COptionsOptions(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent )   // standard constructor
	: CDialog(COptionsOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsOptions)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	maxPlaylistFiles = pOptions->maxPlaylistFiles;
	bHidePLSOnLButtonUp = pOptions->bHidePLSOnLButtonUp;
	bMaximizeOverTaskMenu = pOptions->bMaximizeOverTaskMenu;
	bAlwaysOnTop = pOptions->bAlwaysOnTop;
	dwMilisecondsToHideMouse = pOptions->dwMilisecondsToHideMouse;
}


void COptionsOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsOptions)
	DDX_Control(pDX, IDC_TEXTT2_HIDEAFTER, m_TT2HideAfter);
	DDX_Control(pDX, IDC_TEXTT_TITLEPLAYLIST, m_TTTitlePlaylist);
	DDX_Control(pDX, IDC_TEXTT_TITLEGENERAL, m_TTTitleGeneral);
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	DDX_Control(pDX, IDC_TEXTT_MAXPLSFILES, m_TTMaxPLSFiles);
	DDX_Control(pDX, IDC_TEXTT_HIDEAFTER, m_TTHideAfter);
	DDX_Control(pDX, IDC_CHECK_ALWAYSONTOP, m_CKAlwaysOnTop);
	DDX_Control(pDX, IDC_CHECK_HIDEPLSMLEFT, m_CKHidePLSOnClick);
	DDX_Control(pDX, IDC_SPIN_MAXPLSFILES, m_SPMaxPLSFiles);
	DDX_Control(pDX, IDC_SPIN_HIDEAFTER, m_SPHideAfter);
	DDX_Control(pDX, IDC_EDIT_MAXPLSFILES, m_EMaxPLSFiles);
	DDX_Control(pDX, IDC_EDIT_HIDEAFTER, m_EHideAfter);
	DDX_Control(pDX, IDC_CHECK_MAXOVERTASK, m_CKMaxOverTask);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_BDefault);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_BApply);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsOptions, CDialog)
	//{{AFX_MSG_MAP(COptionsOptions)
	ON_EN_CHANGE(IDC_EDIT_HIDEAFTER, OnChangeEditHideafter)
	ON_EN_CHANGE(IDC_EDIT_MAXPLSFILES, OnChangeEditMaxplsfiles)
	ON_BN_CLICKED(IDC_CHECK_ALWAYSONTOP, OnCheckAlwaysontop)
	ON_BN_CLICKED(IDC_CHECK_HIDEPLSMLEFT, OnCheckHideplsmleft)
	ON_BN_CLICKED(IDC_CHECK_MAXOVERTASK, OnCheckMaxovertask)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsOptions message handlers

BOOL COptionsOptions::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_EHideAfter.SetLimitText( 5 ); m_SPHideAfter.SetRange( 100, 30000 );
	m_EMaxPLSFiles.SetLimitText( 3 ); m_SPMaxPLSFiles.SetRange( 0, 999 );

	InitDialog();
	InitLanguage();

	m_BApply.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsOptions::InitDialog()
{

	m_SPHideAfter.SetPos( dwMilisecondsToHideMouse );
	m_SPMaxPLSFiles.SetPos( maxPlaylistFiles );

	m_CKHidePLSOnClick.SetCheck( bHidePLSOnLButtonUp );
	m_CKMaxOverTask.SetCheck( bMaximizeOverTaskMenu );
	m_CKAlwaysOnTop.SetCheck( bAlwaysOnTop );

}

void COptionsOptions::OnChangeEditHideafter() 
{
	if ( !m_SPHideAfter ) return;
	int min=100, max=30000;

	int ival = m_SPHideAfter.GetPos();

	if ( ival<min ) ival = min;
	if ( ival>max ) ival = min;

	m_SPHideAfter.SetPos( ival );

	dwMilisecondsToHideMouse = ival;

	m_BApply.EnableWindow( TRUE );
}

void COptionsOptions::OnChangeEditMaxplsfiles() 
{
	if ( !m_SPMaxPLSFiles ) return;
	int min=0, max=999;

	int ival = m_SPMaxPLSFiles.GetPos();

	if ( ival<min ) ival = min;
	if ( ival>max ) ival = min;

	m_SPMaxPLSFiles.SetPos( ival );

	maxPlaylistFiles = ival;

	m_BApply.EnableWindow( TRUE );
}

void COptionsOptions::OnCheckAlwaysontop() 
{
	bAlwaysOnTop = !bAlwaysOnTop;

	m_BApply.EnableWindow( TRUE );
}

void COptionsOptions::OnCheckHideplsmleft() 
{
	bHidePLSOnLButtonUp = !bHidePLSOnLButtonUp;

	m_BApply.EnableWindow( TRUE );
}

void COptionsOptions::OnCheckMaxovertask() 
{
	bMaximizeOverTaskMenu = !bMaximizeOverTaskMenu;

	m_BApply.EnableWindow( TRUE );
}

void COptionsOptions::OnButtonApply() 
{

	if ( bAlwaysOnTop!=pOptions->bAlwaysOnTop )
		pControlWnd->PostMessage( WM_COMMAND, IDM_CALLBACK_ALWAYSONTOPCHANGED, NULL );

	pOptions->maxPlaylistFiles = maxPlaylistFiles;
	pOptions->bHidePLSOnLButtonUp = bHidePLSOnLButtonUp;
	pOptions->bMaximizeOverTaskMenu = bMaximizeOverTaskMenu;
	pOptions->bAlwaysOnTop = bAlwaysOnTop;
	pOptions->dwMilisecondsToHideMouse = dwMilisecondsToHideMouse;

	m_BApply.EnableWindow( FALSE );
}

void COptionsOptions::OnButtonDefault() 
{
	maxPlaylistFiles = defConfig.maxPlaylistFiles;
	bHidePLSOnLButtonUp = defConfig.bHidePLSOnLButtonUp;
	bMaximizeOverTaskMenu = defConfig.bMaximizeOverTaskMenu;
	bAlwaysOnTop = defConfig.bAlwaysOnTop;
	dwMilisecondsToHideMouse = defConfig.dwMilisecondsToHideMouse;

	InitDialog();
	m_BApply.EnableWindow( TRUE );

}

void COptionsOptions::InitLanguage()
{
	TRANSLW( m_BApply, IDC_BUTTON_APPLY, TRANSL_OPTIONS );
	TRANSLW( m_BDefault, IDC_BUTTON_DEFAULT, TRANSL_OPTIONS );
	
	TRANSLW( m_TTTitle, IDD_PP_OPTIONS, TRANSL_OPTIONS );

	int sect = TRANSL_PP_OPTIONS;
	TRANSLW( m_TTTitleGeneral, IDC_TEXTT_TITLEGENERAL, sect );
	//TRANSLW( m_TTTitlePlaylist, IDC_TEXTT_TITLEPLAYLIST, sect);
	TRANSLW( m_TTTitlePlaylist, IDD_DIALOG_PLAYLIST, TRANSL_QMENU_PLS);
	TRANSLW( m_CKAlwaysOnTop, IDC_CHECK_ALWAYSONTOP, sect );
	TRANSLW( m_CKMaxOverTask, IDC_CHECK_MAXOVERTASK, sect );
	TRANSLW( m_TTHideAfter, IDC_TEXTT_HIDEAFTER, sect );
	TRANSLW( m_TT2HideAfter, IDC_TEXTT2_HIDEAFTER, sect );
	TRANSLW( m_TTMaxPLSFiles, IDC_TEXTT_MAXPLSFILES, sect );
	TRANSLW( m_CKHidePLSOnClick, IDC_CHECK_HIDEPLSMLEFT, sect );
}
