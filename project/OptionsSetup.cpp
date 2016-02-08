// OptionsSetup.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsSetup.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSetup dialog


COptionsSetup::COptionsSetup(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent )   // standard constructor
	: CDialog(COptionsSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsSetup)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;

	pszCurrLangFile = pOptions->szLanguageFileName;
}


void COptionsSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsSetup)
	DDX_Control(pDX, IDC_TEXTT_TITLECURRLANG, m_TTTitleCurrLang);
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	DDX_Control(pDX, IDC_TEXTT_TRANSLATION, m_TTTranslAuthor);
	DDX_Control(pDX, IDC_BUTTON_DEFAULT, m_BDefault);
	DDX_Control(pDX, IDC_BUTTON_APPLY, m_BApply);
	DDX_Control(pDX, IDC_COMBO_CURRLANG, m_CCurrLang);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsSetup, CDialog)
	//{{AFX_MSG_MAP(COptionsSetup)
	ON_CBN_SELCHANGE(IDC_COMBO_CURRLANG, OnSelchangeComboCurrlang)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DEFAULT, OnButtonDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSetup message handlers

#define MAX_LANGUAGES 16
int countLangs = 0;
CString	pstrLangFile[ MAX_LANGUAGES+1 ],
			pstrLangName[ MAX_LANGUAGES+1 ],
			pstrLangAuthor[ MAX_LANGUAGES+1 ];

char		szLangDef[] = "English (default)";

BOOL COptionsSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	char path[_MAX_PATH], pathtemp[_MAX_PATH];
	_makepath( pathtemp, NULL, pOptions->szAppPath, pOptions->szLanguageDirectory, NULL );
	_makepath( path, NULL, pathtemp, "*.*", NULL );


   CFileFind finder;
   BOOL bIsNextFile = finder.FindFile(path);
   while (bIsNextFile && countLangs<MAX_LANGUAGES )
   {
      bIsNextFile = finder.FindNextFile();
		if ( !strcmp(finder.GetFileName(), ".") ||
				!strcmp(finder.GetFileName(), "..") 
				 ) continue;
		if ( !finder.IsDirectory() ) {

			char filename[_MAX_PATH ]; sprintf( filename, finder.GetFilePath() );
			CString name, author;
			if ( IsLang( (char*)filename, name, author) && !name.IsEmpty() )
			{
				if ( m_CCurrLang.FindStringExact( -1, name )==CB_ERR )
				{
				
					pstrLangFile[ countLangs ] = finder.GetFileName();
					pstrLangName[ countLangs ] = name;
					pstrLangAuthor[ countLangs++ ] = author;
				
					m_CCurrLang.AddString( name );
				}

			}

		}
   }

	CString name, author;
	_makepath( pathtemp, NULL, pOptions->szAppPath, pOptions->szLanguageDirectory, NULL );
	_makepath( path, NULL, pathtemp, pszCurrLangFile, NULL );
	if ( !IsLang( path, name, author)  )
	{
		strcpy( pszCurrLangFile, "" );

	}

	pstrLangFile[ countLangs ] = "";
	pstrLangName[ countLangs ] = szLangDef;
	pstrLangAuthor[ countLangs++ ] = "English translation: Adam Joniec ( adamjoniec@o2.pl )";
	m_CCurrLang.AddString( szLangDef );

	InitDialog();
	InitLanguage();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int currSel=0;
void COptionsSetup::InitDialog()
{
	for ( int i=0; i<countLangs; i++ )
		if ( !pstrLangFile[i].CompareNoCase( pszCurrLangFile ) ) break;

	currSel = m_CCurrLang.FindStringExact( -1, pstrLangName[i] );

	m_TTTranslAuthor.SetWindowText( pstrLangAuthor[i] );
	m_CCurrLang.SetCurSel( currSel );
}


void COptionsSetup::OnSelchangeComboCurrlang() 
{
	if ( m_CCurrLang.GetCurSel() == currSel ) return;

	currSel = m_CCurrLang.GetCurSel();

	CString strcurr;
	m_CCurrLang.GetLBText( currSel, strcurr );
	for ( int i=0; i<countLangs; i++ )
		if ( !pstrLangName[i].CompareNoCase( strcurr ) ) break;
	if ( !(i<countLangs) ) return;

	strcpy( pszCurrLangFile, pstrLangFile[i] );
	InitDialog();
	
	m_BApply.EnableWindow( TRUE );

}

void COptionsSetup::OnButtonApply() 
{
	currSel = m_CCurrLang.GetCurSel();

	CString strcurr;
	m_CCurrLang.GetLBText( currSel, strcurr );
	for ( int i=0; i<countLangs; i++ )
		if ( !pstrLangName[i].CompareNoCase( strcurr ) ) break;

	if ( !(i<countLangs) ) return;//

	char path[_MAX_PATH], pathtemp[_MAX_PATH];
	_makepath( pathtemp, NULL, pOptions->szAppPath, pOptions->szLanguageDirectory, NULL );
	_makepath( path, NULL, pathtemp, pstrLangFile[i], NULL );

	// If pstrLangFile[i]=="" it means that user selet default 
	//  language (english), and it is defined in exe, sow we
	//  don't must load it from additional file.
	if ( pstrLangFile[i].IsEmpty() )
		strcpy( path, "" );

	if ( ChangeLang( path ) )
	{
		strcpy( pOptions->szLanguageFileName, pstrLangFile[i] );

		/* Language has changed, so we must inform other windows about it.

		  		 <-	this ( this property page send message to parent=COptions )
		  		|
		  		 ->		COptions ( options wnd send message to main wnd )
		  					|
		  	 CMainWnd <-	( main wnd send message to movie prop wnd and playlist wnd )
			 |
			 |-> CMovieProperties
			 |
			  -> CPlaylist

		*/

		// First change language of this window.
		InitLanguage();

		// Than send message to parent ( COptions ).
		GetParent()->PostMessage( WM_COMMAND, 
			IDM_CALLBACK_LANGUAGECHANGED, 0 );
	}

	m_BApply.EnableWindow( FALSE );
}

void COptionsSetup::OnButtonDefault() 
{

	int idef = m_CCurrLang.FindStringExact( -1, szLangDef );
	if ( m_CCurrLang.GetCurSel() == idef ) return;

	m_CCurrLang.SetCurSel( idef );

	m_BApply.EnableWindow( TRUE );
}


void COptionsSetup::InitLanguage()
{
	m_BApply.SetWindowText( transl(IDC_BUTTON_APPLY, TRANSL_OPTIONS) );
	m_BDefault.SetWindowText( transl(IDC_BUTTON_DEFAULT, TRANSL_OPTIONS) );

	m_TTTitle.SetWindowText( transl(IDD_PP_SETUP, TRANSL_OPTIONS) );
	m_TTTitleCurrLang.SetWindowText( transl(IDC_TEXTT_TITLECURRLANG,TRANSL_PP_SETUP) );
}
