// Options.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "Options.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptions dialog
/*
	Setup //[Language]
		|
		- File types
	Options
		|
		- Play
		| 
		- Startup
	Subtitles
		|
		- Time correction

	Skin browser
	About
*/
struct stTreeItem {
	BOOL bRoot;
	int idPropertyPage, idText;
	char szText[32];
	HTREEITEM hTreeItem;
};

#define TREE_ITEMS 10

stTreeItem TreeDef[TREE_ITEMS] = {
	{ TRUE,  IDD_PP_SETUP, -1, "Setup" },
	{ FALSE, IDD_PP_FILETYPES, -1, "File types" },
	{ TRUE,  IDD_PP_OPTIONS, -1, "Options" },
	{ FALSE, IDD_PP_PLAY, -1, "Play" },
	{ FALSE, IDD_PP_STARTUP, -1, "Startup" },
	{ TRUE,  IDD_PP_SUBTITLES, -1, "Subtitles" },
	{ FALSE, IDD_PP_SUBTITLES_GENERAL, -1, "Additional" },
	{ FALSE, IDD_PP_TIMECORRECTION, -1, "Time correction" },
	{ TRUE,  IDD_PP_SKINBROWSER, -1, "Skin browser" },
	{ TRUE,  IDD_PP_ABOUT, -1, "About" }
};


COptions::COptions(stConfig *pOpt, CWnd *pControlWnd, int idStartPropertyPage, CWnd* pParent /*=NULL*/)
	:CDialog(COptions::IDD, pParent),
	pPropertyPage( NULL ),
	idPropertyPage(-1)
{
	//{{AFX_DATA_INIT(COptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	pOptions = pOpt;
	this->pControlWnd = pControlWnd;
	this->idStartPropertyPage = idStartPropertyPage;

	// Load images that will be used in About (load here because
	//  we can here load images only once (on start) and use 
	//  everytime when user select About ).
	HMODULE hModule = AfxGetApp()->m_hInstance;

	HRSRC hRsrc = FindResource( hModule, MAKEINTRESOURCE(IDR_ABOUT), "RAWDATA" );
	HGLOBAL hResData = LoadResource( hModule, hRsrc );
	DWORD size = SizeofResource( hModule, hRsrc );
	char *png = (char*)LockResource( hResData );

	pimgAbout = new cGImage();
	pimgAbout->LoadImage( png, size );
	pimgAbout->ConvertToBitmap( m_hWnd );

	hRsrc = FindResource( hModule, MAKEINTRESOURCE(IDR_ABOUTSCROLLER), "RAWDATA" );
	hResData = LoadResource( hModule, hRsrc );
	size = SizeofResource( hModule, hRsrc );
	png = (char*)LockResource( hResData );
	pimgAboutScroller = new cGImage();
	pimgAboutScroller->LoadImage( png, size );
	pimgAboutScroller->ConvertToBitmap( m_hWnd );

	hRsrc = FindResource( hModule, MAKEINTRESOURCE(IDR_ABOUTWWW), "RAWDATA" );
	hResData = LoadResource( hModule, hRsrc );
	size = SizeofResource( hModule, hRsrc );
	png = (char*)LockResource( hResData );
	pimgAboutWWW = new cGImage();
	pimgAboutWWW->LoadImage( png, size );
	pimgAboutWWW->ConvertToBitmap( m_hWnd );

	hRsrc = FindResource( hModule, MAKEINTRESOURCE(IDR_ABOUTREGISTERIT), "RAWDATA" );
	hResData = LoadResource( hModule, hRsrc );
	size = SizeofResource( hModule, hRsrc );
	png = (char*)LockResource( hResData );
	pimgAboutRegisterIt = new cGImage();
	pimgAboutRegisterIt->LoadImage( png, size );
	pimgAboutRegisterIt->ConvertToBitmap( m_hWnd );

	hRsrc = FindResource( hModule, MAKEINTRESOURCE(IDR_ABOUTMAIL), "RAWDATA" );
	hResData = LoadResource( hModule, hRsrc );
	size = SizeofResource( hModule, hRsrc );
	png = (char*)LockResource( hResData );
	pimgAboutMail = new cGImage();
	pimgAboutMail->LoadImage( png, size );
	pimgAboutMail->ConvertToBitmap( m_hWnd );
}


void COptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptions)
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_BClose);
	DDX_Control(pDX, IDC_TREE_PAGE, m_TreePage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptions, CDialog)
	//{{AFX_MSG_MAP(COptions)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PAGE, OnSelchangedTreePage)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptions message handlers


BOOL COptions::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	HTREEITEM hTreeItemRoot = NULL;
	stTreeItem *ptd = TreeDef;
	for (int count=0; count<TREE_ITEMS; count++ )
	{
		if ( ptd->bRoot ) {

			if (hTreeItemRoot) 
				m_TreePage.Expand( hTreeItemRoot, TVE_EXPAND );

			ptd->hTreeItem = m_TreePage.InsertItem( ptd->szText );
			hTreeItemRoot = ptd->hTreeItem;

		}
		else {

			ptd->hTreeItem = 
				m_TreePage.InsertItem( ptd->szText, hTreeItemRoot );
		}

		ptd++;
	}
	
	InitLanguage();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptions::OnSelchangedTreePage(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	
	HTREEITEM hTreeItem = m_TreePage.GetSelectedItem();

	stTreeItem *ptd = TreeDef;
	for ( int idd=0; 
			idd<TREE_ITEMS && ptd->hTreeItem!=hTreeItem;
			idd++, ptd++ ) ;

	if (pPropertyPage) {

		pPropertyPage->DestroyWindow();
		delete pPropertyPage; 
		pPropertyPage = NULL;
		idPropertyPage = -1;	
	}

	if ( idd<TREE_ITEMS ) {

		switch ( ptd->idPropertyPage )
		{
			case IDD_PP_SKINBROWSER: 
				pPropertyPage = new COptionsSkinBrowser( pOptions , GetParent(), this );
				break;
			case IDD_PP_SUBTITLES: 
				pPropertyPage = new COptionsSubtitles( pOptions , GetParent(), this );
				break;
			case IDD_PP_SUBTITLES_GENERAL:
				pPropertyPage = new COptionsSubtitlesGen( pOptions , GetParent(), this );
				break;
			case IDD_PP_FILETYPES:
				pPropertyPage = new COptionsFiletypes( pOptions , GetParent(), this );
				break;
			case IDD_PP_OPTIONS:
				pPropertyPage = new COptionsOptions( pOptions , GetParent(), this );
				break;
			case IDD_PP_PLAY:
				pPropertyPage = new COptionsPlay( pOptions , GetParent(), this );
				break;
			case IDD_PP_SETUP:
				pPropertyPage = new COptionsSetup( pOptions , GetParent(), this );
				break;
			case IDD_PP_STARTUP:
				pPropertyPage = new COptionsStartup( pOptions , GetParent(), this );
				break;
			case IDD_PP_SUBTITLES_TIME:
				pPropertyPage = new COptionsSubtitlesTime( pOptions , GetParent(), this );
				break;
			case IDD_PP_ABOUT:
				pPropertyPage = new COptionsAbout( pOptions , GetParent(), this, 
					pimgAbout, pimgAboutScroller, 
					pimgAboutWWW, pimgAboutRegisterIt, pimgAboutMail );
				break;
		}
		if ( pPropertyPage )
		{
			pPropertyPage->Create(ptd->idPropertyPage, this );
			pPropertyPage->SetWindowPos( NULL, 130,0,0,0, SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW);
			idPropertyPage = ptd->idPropertyPage;	
		}

	}
	else idPropertyPage = -1;

	
	*pResult = 0;
}

void COptions::OnButtonClose() 
{

	OnClose();
	CDialog::OnOK();
}

void COptions::OnClose() 
{
	if (pPropertyPage) {

		pPropertyPage->DestroyWindow();
		delete pPropertyPage; 
		pPropertyPage = NULL;
		idPropertyPage = -1;	
	}
	
	CDialog::OnClose();
}

void COptions::ShowOptions( int idStartPropertyPage )
{
	this->idStartPropertyPage = idStartPropertyPage;


	stTreeItem *ptd = TreeDef;
	for ( int idd=0;
			idd<TREE_ITEMS && ptd->idPropertyPage!=idStartPropertyPage;
			idd++, ptd++ ) ;
	m_TreePage.SelectItem(NULL);
	if ( idStartPropertyPage>= 0 )
	{

		if ( idd<TREE_ITEMS ) {
			m_TreePage.SelectItem( ptd->hTreeItem );
		}
		else
		{
			// If idStartPropertyPage is invalid propert page id,
			//  select last property page.
			ptd--;
			m_TreePage.SelectItem( ptd->hTreeItem );
		}
	}
	else
	{
		// If idStartPropertyPage==-1 (default) select last property page.
		ptd--;
		m_TreePage.SelectItem( ptd->hTreeItem );
	}

	m_TreePage.SetFocus();

	ShowWindow( SW_SHOW );
}

BOOL COptions::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch ( wParam )
	{
		case IDM_CALLBACK_LANGUAGECHANGED:
			InitLanguage();
			pControlWnd->PostMessage( WM_COMMAND, 
				IDM_CALLBACK_LANGUAGECHANGED, 0 );
			break;

		default:
			if ( pPropertyPage) 
				pPropertyPage->PostMessage( WM_COMMAND, wParam, lParam );	
			break;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

void COptions::InitLanguage()
{

	char *psz = transl( IDD_DIALOG_OPTIONS, TRANSL_OPTIONS );
	CString str = " The Imp "; str += psz;
	SetWindowText( str );

	m_BClose.SetWindowText( transl(IDC_BUTTON_CLOSE,TRANSL_OPTIONS) );
	
	int idds[TREE_ITEMS] = {
		IDD_PP_SETUP, IDD_PP_FILETYPES,
		IDD_PP_OPTIONS, IDD_PP_PLAY, IDD_PP_STARTUP,
		IDD_PP_SUBTITLES, IDD_PP_SUBTITLES_GENERAL,
		IDD_PP_TIMECORRECTION, IDD_PP_SKINBROWSER,
		IDD_PP_ABOUT };

	stTreeItem *pt = TreeDef;
	for ( int i=0; i<TREE_ITEMS; i++ ) {

		psz = transl(idds[i], TRANSL_OPTIONS );
		m_TreePage.SetItemText( pt->hTreeItem, psz );
		pt++;
	}

}
