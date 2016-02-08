// OptionsSubtitlesGen.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsSubtitlesGen.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitlesGen dialog


COptionsSubtitlesGen::COptionsSubtitlesGen(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent /*=NULL*/)
	: CDialog(COptionsSubtitlesGen::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsSubtitlesGen)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	iSubtitlesRows = pOptions->iSubtitlesRows;
	iSubtitlesRowsDistance = pOptions->iSubtitlesRowsDistance;
	iSubtitlesMultiRowMode = pOptions->iSubtitlesMultiRowMode;
	iSubtitlesHideAfter = pOptions->iSubtitlesHideAfter;
	valSubtitlesDistanceFromBottom = 100-pOptions->valSubtitlesDistanceFromBottom;
	bSbtFontAutosize = pOptions->bSbtFontAutosize;
}


void COptionsSubtitlesGen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsSubtitlesGen)
	DDX_Control(pDX, IDD_TEXTT_TEXTROWS, m_TTTextRows);
	DDX_Control(pDX, IDD_TEXTT_TIMETOHIDE, m_TTTimeToHide);
	DDX_Control(pDX, IDD_TEXTT2_TIMETOHIDE, m_TT2TimeToHide);
	DDX_Control(pDX, IDD_TEXTT2_ROWDISTANCE, m_TT2RowDistance);
	DDX_Control(pDX, IDD_TEXTT_TITLETIMEPROP, m_TTTitleTimeProp);
	DDX_Control(pDX, IDD_TEXTT_ROWDISTANCE, m_TTRowDistance);
	DDX_Control(pDX, IDD_TEXTT_MROWDIR, m_TTMRowDir);
	DDX_Control(pDX, IDC_TEXTT_TITLEPOSITION, m_TTTitlePosition);
	DDX_Control(pDX, IDC_TEXTT_TITLEADDITIONAL, m_TTTitleAdditional);
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	DDX_Control(pDX, IDC_SPIN_TIMETOHIDE, m_SPHideAfter);
	DDX_Control(pDX, IDC_SPIN_TEXTROWS, m_SPTextRows);
	DDX_Control(pDX, IDC_SPIN_DISTANCE, m_SPRowDistance);
	DDX_Control(pDX, IDC_EDIT_TIMETOHIDE, m_EHideAfter);
	DDX_Control(pDX, IDC_EDIT_DISTANCE, m_ERowDistance);
	DDX_Control(pDX, IDC_COMBO_MROWDIR, m_CMRowDir);
	DDX_Control(pDX, IDC_CHECK_AUTOSIZE, m_CKAutosize);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_BDefault);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_BApply);
	DDX_Control(pDX, IDC_EDIT_TEXTROWS, m_ETextRows);
	DDX_Control(pDX, IDC_SCROLLBAR_POSITION, m_SBPosition);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsSubtitlesGen, CDialog)
	//{{AFX_MSG_MAP(COptionsSubtitlesGen)
	ON_WM_VSCROLL()
	ON_EN_CHANGE(IDC_EDIT_TEXTROWS, OnChangeEditTextrows)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_EN_CHANGE(IDC_EDIT_DISTANCE, OnChangeEditDistance)
	ON_EN_CHANGE(IDC_EDIT_TIMETOHIDE, OnChangeEditTimetohide)
	ON_BN_CLICKED(IDC_CHECK_AUTOSIZE, OnCheckAutosize)
	ON_CBN_SELCHANGE(IDC_COMBO_MROWDIR, OnSelchangeComboMrowdir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitlesGen message handlers

BOOL COptionsSubtitlesGen::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_SBPosition.SetScrollRange( 0, 100 );

	m_ETextRows.SetLimitText( 1 ); m_SPTextRows.SetRange( 1, 5 );
	m_ERowDistance.SetLimitText( 2 ); m_SPRowDistance.SetRange( 0, 99 );
	m_EHideAfter.SetLimitText( 2 ); m_SPHideAfter.SetRange( 1, 30 );

	InitLanguage();
	InitDialog();

	m_BApply.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsSubtitlesGen::InitDialog()
{
	m_CKAutosize.SetCheck( bSbtFontAutosize );
	m_SBPosition.SetScrollPos( (int)valSubtitlesDistanceFromBottom );

	switch ( iSubtitlesMultiRowMode )
	{

		case SUBTITLESMR_TOP_BOTTOM: m_CMRowDir.SetCurSel(0); break;
		case SUBTITLESMR_BOTTOM_TOP: m_CMRowDir.SetCurSel(1); break;
	}

	
	m_SPTextRows.SetPos( iSubtitlesRows );
	m_SPRowDistance.SetPos( iSubtitlesRowsDistance );
	m_SPHideAfter.SetPos( iSubtitlesHideAfter );

	SetDepedencies();
}


void COptionsSubtitlesGen::SetDepedencies()
{
}

void COptionsSubtitlesGen::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ( pScrollBar!=&m_SBPosition ) return;

	int nCurPos = pScrollBar->GetScrollPos();
	int nMax, nMin; pScrollBar->GetScrollRange(&nMin, &nMax);
	switch (nSBCode)
	{
	case SB_TOP: //    Scroll to far left.
		nCurPos = nMin;
		break;
	case SB_LINEUP: //    Scroll left.
		nCurPos--;
		break;
	case SB_LINEDOWN: //    Scroll right.
		nCurPos++;
		break;
	case SB_PAGEUP: //    Scroll one page left.
		nCurPos -= nMax/20;
		break;
	case SB_PAGEDOWN: //    Scroll one page right.
		nCurPos += nMax/20;
		break;
	case SB_BOTTOM: //    Scroll to far right.
		nCurPos = nMax;
		break;
	case SB_THUMBPOSITION: //    Scroll to absolute position. The current position is specified by the nPos parameter.
	case SB_THUMBTRACK: //    Drag scroll box to specified position. The current position is specified by the nPos parameter.
		nCurPos = nPos;
	}
	if (nCurPos < nMin) nCurPos = nMin;
	if (nCurPos > nMax) nCurPos = nMax;
	pScrollBar->SetScrollPos(nCurPos);

	
	valSubtitlesDistanceFromBottom = nCurPos;

	pOptions->valSubtitlesDistanceFromBottom = 100-valSubtitlesDistanceFromBottom;
	pControlWnd->PostMessage( WM_COMMAND,
		IDM_CALLBACK_SUBTITLESPOSCHANGED, 0 );

	m_BApply.EnableWindow( TRUE );
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


void COptionsSubtitlesGen::OnButtonDefault() 
{
	iSubtitlesRows = defConfig.iSubtitlesRows;
	iSubtitlesRowsDistance = defConfig.iSubtitlesRowsDistance;
	iSubtitlesMultiRowMode = defConfig.iSubtitlesMultiRowMode;
	iSubtitlesHideAfter = defConfig.iSubtitlesHideAfter;
	valSubtitlesDistanceFromBottom = 100.0 - defConfig.valSubtitlesDistanceFromBottom;
	bSbtFontAutosize = defConfig.bSbtFontAutosize;

	InitDialog();

	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitlesGen::OnChangeEditTextrows() 
{
	if ( !m_SPTextRows ) return;
	int min=1, max=5;

	int ival = m_SPTextRows.GetPos();

	if ( ival<min ) ival = min;
	if ( ival>max ) ival = min;

	m_SPTextRows.SetPos( ival );

	iSubtitlesRows = ival;

	m_BApply.EnableWindow( TRUE );

}

void COptionsSubtitlesGen::OnChangeEditDistance() 
{
	if ( !m_SPRowDistance ) return;
	int min=0, max=99;

	int ival = m_SPRowDistance.GetPos();

	if ( ival<min ) ival = min;
	if ( ival>max ) ival = min;

	m_SPRowDistance.SetPos( ival );

	iSubtitlesRowsDistance = ival;

	m_SPHideAfter.SetPos( iSubtitlesHideAfter );
	m_BApply.EnableWindow( TRUE );
	
}

void COptionsSubtitlesGen::OnChangeEditTimetohide() 
{
	if ( !m_SPHideAfter ) return;
	int min=1, max=30;

	int ival = m_SPHideAfter.GetPos();

	if ( ival<min ) ival = min;
	if ( ival>max ) ival = min;

	m_SPHideAfter.SetPos( ival );

	iSubtitlesHideAfter = ival;

	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitlesGen::OnCheckAutosize() 
{
	bSbtFontAutosize = !bSbtFontAutosize;
	
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitlesGen::OnSelchangeComboMrowdir() 
{
	int cur = m_CMRowDir.GetCurSel();

	switch ( cur )
	{

		case 0: iSubtitlesMultiRowMode = SUBTITLESMR_TOP_BOTTOM; break;
		case 1: iSubtitlesMultiRowMode = SUBTITLESMR_BOTTOM_TOP; break;
	}
	
	m_BApply.EnableWindow( TRUE );
}

void COptionsSubtitlesGen::OnButtonApply() 
{

	m_BApply.EnableWindow( FALSE );

	pOptions->iSubtitlesRows = iSubtitlesRows;
	pOptions->iSubtitlesRowsDistance = iSubtitlesRowsDistance;
	pOptions->iSubtitlesMultiRowMode = iSubtitlesMultiRowMode;
	pOptions->iSubtitlesHideAfter = iSubtitlesHideAfter;
	pOptions->valSubtitlesDistanceFromBottom = 100-valSubtitlesDistanceFromBottom;
	pOptions->bSbtFontAutosize = bSbtFontAutosize;

	pControlWnd->PostMessage( WM_COMMAND, IDM_CALLBACK_FONTCHANGED, 0 );

}

void COptionsSubtitlesGen::InitLanguage()
{
	TRANSLW( m_BApply, IDC_BUTTON_APPLY, TRANSL_OPTIONS );
	TRANSLW( m_BDefault, IDC_BUTTON_DEFAULT, TRANSL_OPTIONS );
	
	TRANSLW( m_TTTitle, IDD_PP_SUBTITLES_GENERAL, TRANSL_OPTIONS );

	int sect = TRANSL_PP_SUBTITLES_GENERAL;

	TRANSLW( m_TTTitleTimeProp, IDD_TEXTT_TITLETIMEPROP, sect );
	TRANSLW( m_TTTitleAdditional, IDC_TEXTT_TITLEADDITIONAL, sect );
	TRANSLW( m_TTTimeToHide, IDD_TEXTT_TIMETOHIDE, sect );
	TRANSLW( m_TT2TimeToHide, IDD_TEXTT2_TIMETOHIDE, sect );
	TRANSLW( m_TTTextRows, IDD_TEXTT_TEXTROWS, sect );
	TRANSLW( m_TTRowDistance, IDD_TEXTT_ROWDISTANCE, sect );
	TRANSLW( m_TTMRowDir, IDD_TEXTT_MROWDIR, sect );
	TRANSLW( m_CKAutosize, IDC_CHECK_AUTOSIZE, sect );
	TRANSLW( m_TTTitlePosition, IDC_TEXTT_TITLEPOSITION, sect );

	m_CMRowDir.ResetContent();
	m_CMRowDir.AddString( transl(0, sect) );
	m_CMRowDir.AddString( transl(1, sect) );

}
