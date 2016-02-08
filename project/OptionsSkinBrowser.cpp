// OptionsSkinBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "OptionsSkinBrowser.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSkinBrowser dialog


COptionsSkinBrowser::COptionsSkinBrowser(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent /*=NULL*/)
	: CDialog(COptionsSkinBrowser::IDD, pParent),
	pPreview(NULL),
	iCurSel(-1)
{
	//{{AFX_DATA_INIT(COptionsSkinBrowser)
	//}}AFX_DATA_INIT
	pOptions = pOpt;
	this->pControlWnd = pControlWnd;
}


void COptionsSkinBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsSkinBrowser)
	DDX_Control(pDX, IDC_SBLIST_SKINS, m_ListSkins);
	DDX_Control(pDX, IDC_TEXTT_AUTHOR, m_TTAuthor);
	DDX_Control(pDX, IDC_TEXTT_COMMENT, m_TTComment);
	DDX_Control(pDX, IDC_TEXTT_TITLESKINIFNO, m_TTTitleSkinInfo);
	DDX_Control(pDX, IDC_TEXT_VERSION, m_TVersion);
	DDX_Control(pDX, IDC_TEXT_PREVNOTAVAIL, m_TPrevNotAvail);
	DDX_Control(pDX, IDC_TEXT_NAME, m_TName);
	DDX_Control(pDX, IDC_TEXT_COMMENT, m_TComment);
	DDX_Control(pDX, IDC_TEXT_AUTHOR, m_TAuthor);
	DDX_Control(pDX, IDC_BUTTON_SELECTSKIN, m_BSelectSkin);
	DDX_Control(pDX, IDC_TEXTT_VERSION, m_TTVersion);
	DDX_Control(pDX, IDC_TEXTT_NAME, m_TTName);
	DDX_Control(pDX, IDC_TEXTT_TITLE, m_TTTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsSkinBrowser, CDialog)
	//{{AFX_MSG_MAP(COptionsSkinBrowser)
	ON_LBN_SELCHANGE(IDC_SBLIST_SKINS, OnSelchangeSblistSkins)
	ON_LBN_DBLCLK(IDC_SBLIST_SKINS, OnDblclkSblistSkins)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_SELECTSKIN, OnButtonSelectskin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSkinBrowser message handlers

BOOL COptionsSkinBrowser::OnInitDialog() 
{
	CDialog::OnInitDialog();

	InitLanguage();
	
	char dir[_MAX_PATH], dirtemp[_MAX_PATH];
	_makepath( dir, NULL, pOptions->szAppPath, pOptions->szSkinDirectory, NULL );
	sprintf(dirtemp,"%s*.*", dir);

   CFileFind finder;
   BOOL bIsNextFile = finder.FindFile(dirtemp);
   while (bIsNextFile)
   {
      bIsNextFile = finder.FindNextFile();
		if ( !strcmp(finder.GetFileName(), ".") ||
				!strcmp(finder.GetFileName(), "..") ) continue;
		if ( finder.IsDirectory() ) {
			char sz[_MAX_PATH+2]; sprintf(sz,"/%s", finder.GetFileName() );
			m_ListSkins.AddString( sz );
		}
		else
		{
			m_ListSkins.AddString( finder.GetFileName() );
		}
   }

	char szSkinFileName[_MAX_PATH];
	if ( pOptions->bSkinInZIP )
		sprintf( szSkinFileName, pOptions->szSkinFileName );
	else
		sprintf( szSkinFileName, "/%s", pOptions->szSkinFileName );

	int count=m_ListSkins.GetCount();
	for (int i=0; i<count; i++)
	{
		CString str;
		m_ListSkins.GetText(i, str);
		if ( !str.Compare(szSkinFileName) )
		{
			m_ListSkins.SetCurSel( i ); 
			OnSelchangeSblistSkins();
			m_BSelectSkin.EnableWindow( FALSE );
			break;
		}
	}

	EnableWindow(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsSkinBrowser::OnSelchangeSblistSkins() 
{
	char szPreview[32] = "preview.png",
		  szSkinDefinition[32] = "player.xml",
		  szSkinDefinitionId[32] = "player";

	BOOL bIsSkinFile = FALSE;

	if ( iCurSel == m_ListSkins.GetCurSel() )
		return;

	if (pPreview) {
		delete pPreview;
		pPreview = NULL;
	}

	iCurSel = m_ListSkins.GetCurSel();
	CString str; m_ListSkins.GetText(iCurSel, str);


	cArchive zip; 
	CXMLParser XMLFile;
	
	char dir[_MAX_PATH], dirtemp[_MAX_PATH];
	_makepath( dir, NULL, pOptions->szAppPath, pOptions->szSkinDirectory, NULL );
	if ( str[0]=='/' )
	{
		// Skin in directory.
		sprintf( dirtemp, "%s%s\\%s", dir, 
			str.Right( str.GetLength()-1 ), szSkinDefinition );

		XMLFile.OpenXMLFile( dirtemp );

		sprintf( dirtemp, "%s%s\\%s", dir, 
			str.Right( str.GetLength()-1 ), szPreview );
	}
	else
	{
		// Skin in ZIP.
		sprintf( dirtemp, "%s%s", dir, str );
		zip.OpenZIP( dirtemp );

		XMLFile.OpenXMLFile( szSkinDefinition, &zip );
	}

	m_TVersion.SetWindowText( "" );
	m_TName.SetWindowText( "" );
	m_TAuthor.SetWindowText( "" );
	m_TComment.SetWindowText( "Invalid skin !!!" );

	if ( XMLFile.IsOpened() )
	{
		int iLevel;
		CPascalStr *pstrId;
		if ( !XMLFile.NextElement( &pstrId, &iLevel ) )
		{
			if ( !pstrId->cmp( szSkinDefinitionId ) )
			{
				m_TVersion.SetWindowText( "1.0" );
				m_TName.SetWindowText( "noname" );
				m_TAuthor.SetWindowText( "anonim" );
				m_TComment.SetWindowText( "" );
				bIsSkinFile  = TRUE;

				CPascalStr *pstrName, *pstrNameVal;
				while ( XMLFile.NextAttrib( &pstrName, &pstrNameVal ) )
				{
					CStatic *pText = NULL;
					if ( !pstrName->cmp("ver") )
					{
						pText = &m_TVersion;
					}
					else if ( !pstrName->cmp("name") )
					{
						pText = &m_TName;
					}
					else if ( !pstrName->cmp("author") )
					{
						pText = &m_TAuthor;
					}
					else if ( !pstrName->cmp("comment") )
					{
						pText = &m_TComment;
					}
					else if ( !pstrName->cmp("authorEmail") )
					{
					}
					else if ( !pstrName->cmp("authorWWW") )
					{
					}

					if ( pText )
					{
						pText->SetWindowText( pstrNameVal->psz() );
					}
				}
			}
		}
	}

	if ( bIsSkinFile )
	{
		if ( zip.IsOpenedArchive() )
		{
			pPreview = new cGImage();
			pPreview->LoadImage( szPreview, &zip );
			pPreview->ConvertToBitmap( m_hWnd );
		}
		else
		{
			pPreview = new cGImage();
			pPreview->LoadImage( dirtemp );
			pPreview->ConvertToBitmap( m_hWnd );

		}

		if ( pPreview->hBitmap ) {

			m_TPrevNotAvail.ShowWindow( SW_HIDE );
			DrawPreview();
		}
		else
		{
			m_TPrevNotAvail.ShowWindow( SW_SHOW );
			RECT rcDest = { 151,32,151+201,32+151 };
			InvalidateRect( &rcDest ); UpdateWindow();
		}

		m_BSelectSkin.EnableWindow( TRUE );	

	} else {
		m_TPrevNotAvail.ShowWindow( SW_SHOW );
		RECT rcDest = { 151,32,151+201,32+151 };
		InvalidateRect( &rcDest ); UpdateWindow();
		m_BSelectSkin.EnableWindow( FALSE );	
	}

}


void COptionsSkinBrowser::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	DrawPreview( dc.m_hDC );

}

void COptionsSkinBrowser::DrawPreview( HDC hDC )
{
	HDC hDCDest = hDC;
	if ( pPreview && pPreview->hBitmap )
	{
		int x=151, y=32, cx=200, cy=150,
			 bmcx = (pPreview->w > cx)?cx:pPreview->w, 
			 bmcy = (pPreview->h > cy)?cy:pPreview->h,
			 bmx = x+((cx-bmcx)/2),
			 bmy = y+((cy-bmcy)/2);

		RECT rcDest = { x,y,x+cx,y+cy },
			rcDestBm = { bmx,bmy, bmx+bmcx, bmy+bmcy };

		HDC hbmDC = CreateCompatibleDC( hDC );
		::SelectObject( hbmDC , pPreview->hBitmap );

		if ( !hDCDest ) hDCDest = ::GetDC( m_hWnd );

		::BitBlt( hDCDest, rcDestBm.left, rcDestBm.top,
			RECTW(rcDestBm), RECTH(rcDestBm), hbmDC , 0,0, SRCCOPY );
		rcDest.right++; rcDest.bottom++;
		HRGN hrgnDest = ::CreateRectRgnIndirect( &rcDest ),
				hrgnDestBm = ::CreateRectRgnIndirect( &rcDestBm ),
				hrgn = ::CreateRectRgnIndirect( &rcDest );
		CombineRgn( hrgn, hrgnDest, hrgnDestBm, RGN_DIFF );
		FillRgn( hDCDest, hrgn, (HBRUSH)GetStockObject(BLACK_BRUSH) );

		DeleteObject( hrgn );
		DeleteObject( hrgnDest );
		DeleteObject( hrgnDestBm );
		if ( !hDC ) ::ReleaseDC( m_hWnd, hDCDest );
		DeleteDC( hbmDC );
	}
}

void COptionsSkinBrowser::OnButtonSelectskin() 
{
	if ( iCurSel<0 || !m_BSelectSkin.IsWindowEnabled() ) return;

	CString str;

	m_ListSkins.GetText( iCurSel, str );

	if ( str[0]=='/' )
	{
		// Skin in directory.
		sprintf( pOptions->szSkinFileName, str.Right( str.GetLength()-1) );

		pOptions->bSkinInZIP = FALSE;

	}
	else
	{
		// Skin in ZIP.
		sprintf( pOptions->szSkinFileName, str );
		pOptions->bSkinInZIP = TRUE;
	}

	pControlWnd->PostMessage( WM_COMMAND, IDM_CALLBACK_SKINCHANGED, (LPARAM)m_hWnd );
	
	m_BSelectSkin.EnableWindow( FALSE );	
	m_TComment.SetWindowText( " Changing skin... " );
	
	EnableWindow(FALSE);
}

void COptionsSkinBrowser::OnDblclkSblistSkins() 
{
	OnButtonSelectskin();
}

BOOL COptionsSkinBrowser::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD wNotifyCode = HIWORD(wParam); // notification code 
	WORD wID = LOWORD(wParam);


	if ( !wNotifyCode )
	{ // Notify from menu.

		switch ( wID )
		{
			case IDM_CALLBACK_SKINCHANGED:
				//m_TComment.SetWindowText( " Skin changed OK. " );
				m_TComment.SetWindowText( (char*)lParam );
				EnableWindow(TRUE);
				lParam = 0;
				return TRUE;
		}
	}

	
	return CDialog::OnCommand(wParam, lParam);
}

void COptionsSkinBrowser::InitLanguage()
{
	
	TRANSLW( m_TTTitle, IDD_PP_SKINBROWSER, TRANSL_OPTIONS );

	int sect = TRANSL_PP_SKINBROWSER;

	TRANSLW( m_TTTitleSkinInfo, IDC_TEXTT_TITLESKINIFNO, sect );
	TRANSLW( m_TPrevNotAvail, IDC_TEXT_PREVNOTAVAIL, sect );
	TRANSLW( m_TTVersion, IDC_TEXTT_VERSION, sect );
	TRANSLW( m_TTName, IDC_TEXTT_NAME, sect );
	TRANSLW( m_TTAuthor, IDC_TEXTT_AUTHOR, sect );
	TRANSLW( m_TTComment, IDC_TEXTT_COMMENT, sect );

}
