// Playlist.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "Playlist.h"

#include "language.h"
#include "accelerators.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AddFileToList( CListBox *pList, char *psz )
{
	if ( !pList || !psz ) return;

	char sz[_MAX_PATH], name[_MAX_FNAME], ext[_MAX_EXT]; 
	_splitpath(psz,NULL,NULL,name,ext);
	sprintf(sz,"%i. %s%s",pList->GetCount()+1,name,ext);
	pList->AddString( sz );
}

/////////////////////////////////////////////////////////////////////////////
// CPlaylist dialog


CPlaylist::CPlaylist(CWnd* pParent /*=NULL*/)
	: CDialog(CPlaylist::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlaylist)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPlaylist::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlaylist)
		// NOTE: the ClassWizard will add DDX and DDV calls here
		DDX_Control(pDX, IDC_LIST_PLAYLIST, m_List);
	//}}AFX_DATA_MAP
}


void CPlaylist::OnOK() {}
void CPlaylist::OnCancel() {}

BEGIN_MESSAGE_MAP(CPlaylist, CDialog)
	//{{AFX_MSG_MAP(CPlaylist)
	ON_WM_CTLCOLOR()
	ON_LBN_DBLCLK(IDC_LIST_PLAYLIST, OnDblclkListPlaylist)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_QMPLS_CLOSE, OnQmplsClose)
	ON_COMMAND(ID_QMPLS_CLEARPLS, OnQmplsClearpls)
	ON_COMMAND(ID_QMPLS_ADDFILES, OnQmplsAddfiles)
	ON_COMMAND(ID_QMPLS_REMOVEFILE, OnQmplsRemovefile)
	ON_LBN_SELCHANGE(IDC_LIST_PLAYLIST, OnSelchangeListPlaylist)
	ON_COMMAND(KEY_PLSDELETE, OnPlsDeleteselfile)
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_COMMAND(ID_QMPLS_OPEN, OnQmplsOpen)
	ON_COMMAND(ID_QMPLS_SAVE, OnQmplsSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlaylist message handlers


void CPlaylist::Create( int nSize, BOOL bReadList, stConfig *pConfig/*NULL*/ )
{
	size = nSize; count = 0; curr = 0;
	pRecentFileList = 
		new CRecentFileList( 0, "Playlist", "File%d", nSize, 0 );
	if (bReadList) ReadList();

	this->pConfig = pConfig;

}

void CPlaylist::CreateWnd( CWnd* pParentWnd )
{
	CDialog::Create(IDD_DIALOG_PLAYLIST, pParentWnd);	

	if ( pRecentFileList ) 
	{

		for (int i=0; i<count; i++)
		{
			LPCTSTR str = (*pRecentFileList)[count-i-1];
			AddFileToList( &m_List, (char*)str );
		}

		m_List.SetCurSel( curr-1 );

	}
	m_List.SetParent( this );
	m_List.SetOwner( this );
	m_List.SetCaretIndex( count-1 );

	// Textured background
	//HBITMAP hBitmap = (HBITMAP)LoadImage( AfxGetApp()->m_hInstance,
	//	MAKEINTRESOURCE(IDB_BITMAP_PATTERN), IMAGE_BITMAP, 0, 0, 0 );
	//hBrush = CreatePatternBrush( hBitmap );
	//DeleteObject( hBitmap );
	hBrush = CreateSolidBrush( 0x504040 );

	pMenu = new CMenu();
	pMenu->LoadMenu( IDR_MENU_PLS );


	hwndParent = pParentWnd->m_hWnd;

	InitLanguage();
}

CPlaylist::~CPlaylist() 
{
	if(pRecentFileList) 
	{
		pRecentFileList->WriteList();
		delete pRecentFileList;
	}
}

void CPlaylist::ReadList() 
{
	pRecentFileList->ReadList(); 
	for( count=0; count<size && !(*pRecentFileList)[count].IsEmpty(); count++) ;
	curr = count; if (curr) curr++;
}

void CPlaylist::WriteList() 
{
	pRecentFileList->WriteList(); 
}

void CPlaylist::ClearList() 
{
	for (; count; count--) pRecentFileList->Remove( count-1 ); 
	curr = count = 0;
}


// Add file or playlist(*.im, *.m3u) to playlist. If you want add several files, and play it 
//  when call to Next(), set bSetCurr to false;
void CPlaylist::Add(LPCTSTR lpszPathName, BOOL bSetCurr) 
{
	char ext[_MAX_EXT];
	_splitpath( lpszPathName, NULL, NULL, NULL, ext );
	if ( !_stricmp(ext, ".imp") || !stricmp(ext,".m3u") ) {

		AddPlaylist( lpszPathName, bSetCurr );
		return;
	}

	pRecentFileList->Add( lpszPathName );
	int prevCount =  count;
	for( count=0; count<size && !(*pRecentFileList)[count].IsEmpty(); count++) ;
	if ( bSetCurr ) curr = count;
	else if (prevCount==count && curr>0) curr--;

	if ( !m_hWnd ) return; 

	m_List.ResetContent();
	for (int i=0; i<count; i++)
	{
		LPCTSTR str = (*pRecentFileList)[count-i-1];
		AddFileToList( &m_List, (char*)str );
	}
	m_List.SetCurSel( curr-1 );
}

void CPlaylist::AddPlaylist(LPCTSTR lpszPathName, BOOL bSetCurr)
{
	static recursive = 0;

	// If calling playlist from playlist there is possibility of
	//  invinite loop, so allow only three recursive calls
	//  playlist from playlist, if more just exit.
	if ( recursive>3 ) return;

   char drive[_MAX_DRIVE], dir[_MAX_DIR], ext[_MAX_EXT];
	_splitpath( lpszPathName, drive, dir, NULL, ext );

	FILE *f = fopen( lpszPathName, "rt" );
	if ( !f ) return;

	recursive++;

	char path[_MAX_PATH];
	while ( fgets(path, _MAX_PATH, f) )
	{
		// Get string length, if len==0 read next line.
		int len = strlen(path); if ( !len ) continue;

		// Replace newline character added at end of string with NULL.
		char *psz = path+strlen(path)-1;
		while ( *psz==0x0D || *psz==0x0A ) psz--; *++psz=0;

		char szFilePath[_MAX_PATH];
		if ( !stricmp(ext, ".m3u") && path[0]=='#' )
		{
			// In .m3u playlist extended info starts with char '#',
			//  but we don't want read ext info, so just skip it.
			continue;
		}
		if ( path[0]=='/' )
		{	// Path is option.

			if ( !pConfig ) continue;

			// @@@ THIS SHOULD BE PROCESSING IN MAIN PROGRAM NOT HERE
			//			BECAUSE IT CAN'T SEND ANY MESSAGE TO INFORM 
			//			OTHER WINDOWS THAT OPTIONS CHANGED =>ABOUT TO CHANGE
			if ( !strcmp(path+1, "NoLoop") )
				pConfig->bLoop = FALSE;
			continue;
		}
		else if ( len>2 && path[1]==':' ) 
		{	// Path is absolute ( ex. C:\Films\New\Heat.avi ).
			strcpy( szFilePath, path );
		}
		else {
			// Path is relative ( ex. New\Heat.avi ), so
			//  need convert it to absolute ( get dir and path from
			//  playlist path ex. 
			//   playlist		- C:\Films\Playlist.imp
			//	  path			- New\Heat.avi
			//	  output path	- C:\Films\New\Heat.avi )

			// If path starts with '\' we must create output path
			//  relative only to playlist directory.
			if ( path[0]=='\\' )
				_makepath( szFilePath, drive, NULL, path, NULL );
			else
				_makepath( szFilePath, drive, dir, path, NULL );

		}
		Add( szFilePath, bSetCurr ); 
		bSetCurr = FALSE;
	}


	recursive--;
}


void CPlaylist::Remove(int num)
{

	if ( count && num>=0 && num<count )
	{
		int sel = m_List.GetCurSel();

		pRecentFileList->Remove(count-num-1);
		count--; 
		
		m_List.ResetContent();
		for (int i=0; i<count; i++)
		{
			LPCTSTR str = (*pRecentFileList)[count-i-1];
			AddFileToList( &m_List, (char*)str );
		}
		m_List.SetCurSel( sel-1 );

	}

}

void CPlaylist::GoToLast()
{
	if ( curr<count ) {
		for( count=0; count<size && !(*pRecentFileList)[count].IsEmpty(); count++) ;
		curr = count;
	}
}

int CPlaylist::CountFiles() 
{ 
	return count; 
}

BOOL CPlaylist::IsNext() 
{
	if ( curr<count ) return TRUE;
	else return FALSE;
}

BOOL CPlaylist::IsPrev() 
{
	if ( curr>1 ) return TRUE;
	else return FALSE;
}

BOOL CPlaylist::IsCurr()
{
	if ( curr>count || !curr ) return FALSE;
	else return TRUE;
}

LPCTSTR CPlaylist::Curr() 
{
	if ( m_hWnd ) m_List.SetCurSel( curr-1 );

	if ( curr>count || !curr ) return NULL;
	return (*pRecentFileList)[count-curr]; 
}

LPCTSTR CPlaylist::Prev() 
{
	if ( IsPrev() ) {
		curr--; return Curr(); }
	return NULL;
}

LPCTSTR CPlaylist::Next() 
{
	if ( IsNext() ) {
		curr++; return Curr(); }
	return NULL;
}

CFont *pFont = NULL;
HBRUSH CPlaylist::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	if ( nCtlColor==CTLCOLOR_LISTBOX )
	{
		hbr = hBrush;
		if ( !pFont )
		{
			pFont = new CFont();
			LOGFONT logfont= {
				-MulDiv(8, GetDeviceCaps(pDC->m_hDC, LOGPIXELSY), 72), //lfHeight
				0, 0, 0,							//lfWidth, lfEscapement, lfOrientation
				FW_BOLD,						//lfWeight(def)
				FALSE, FALSE, FALSE,			//lfItalic, lfUnderline, lfStrikeOut
				DEFAULT_CHARSET,				//lfCharSet 
				OUT_DEFAULT_PRECIS,			//lfOutPrecision 
				CLIP_DEFAULT_PRECIS,			//lfClipPrecision 
				ANTIALIASED_QUALITY,//DEFAULT_QUALITY,				//lfQuality 
				DEFAULT_PITCH|FF_DONTCARE	//lfPitchAndFamily 
			}; strcpy( logfont.lfFaceName, "Tahoma" );
			pFont->CreateFontIndirect( &logfont );
		}
		pDC->SelectObject( pFont );
		pDC->SetTextColor( RGB(0xc0,0xc0,0xc0) );
		pDC->SetBkMode( TRANSPARENT );

	}
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

LRESULT CPlaylist::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if ( hwndParent ) switch ( message ) 
	{
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
			::PostMessage( hwndParent, message, -1, lParam );
			break;

		case WM_DROPFILES:
		case WM_COMMAND:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
			::PostMessage( hwndParent, message, wParam, lParam );
			break;


	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CPlaylist::OnDblclkListPlaylist() 
{
	// TODO: Add your control notification handler code here
	int nCurSel = m_List.GetCurSel();
	curr = nCurSel+1;


	::PostMessage( hwndParent, WM_COMMAND, IDM_CALLBACK_OPENFROMPLS, NULL );
	
}

void CPlaylist::OnClose() 
{
	::PostMessage( hwndParent, WM_COMMAND, IDM_CALLBACK_CLOSEDPLS, NULL );
}

void CPlaylist::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	m_List.SetCurSel( curr-1 );
	m_List.SetCaretIndex( m_List.GetCount()-1 );
}


BOOL CPlaylist::ShowWindow( int nCmdShow )
{
	if ( nCmdShow==SW_SHOW && !IsWindowVisible() )
	{
		RECT rc; GetWindowRect( &rc );
		POINT pt; GetCursorPos( &pt );

		int x = pt.x - ( RECTW(rc)/2 ),
			 y = pt.y - ( RECTH(rc) ) - 8,
			 cx = RECTW(rc), cy = RECTH(rc),
			 screencx = GetSystemMetrics(SM_CXSCREEN);
		if ( x<0 ) x = 0;  if ( x+cx>screencx ) x = screencx-cx;
		if ( y<0 ) y = 0;
		return SetWindowPos( &wndTop, x,y,0,0, SWP_NOSIZE|SWP_SHOWWINDOW );
	}
	else
	{
		GetParent()->SetActiveWindow();
	}

	BOOL ret = CDialog::ShowWindow( nCmdShow );

	return ret;

}



void CPlaylist::ShowQuickMenu()
{
	BOOL	bSelected = (m_List.GetCurSel()==LB_ERR)?FALSE:TRUE,
		bEmpty = m_List.GetCount()?FALSE:TRUE;

	CMenu *pQuickMenu = pMenu->GetSubMenu(0);

	DWORD dwFlags[] = {

		//T_QMPLS_OPEN
		MF_BYCOMMAND|MF_ENABLED,
		//T_QMPLS_SAVE
		MF_BYCOMMAND|bEmpty?MF_GRAYED:MF_ENABLED,
		//T_QMPLS_CLEARPLS
		MF_BYCOMMAND|bEmpty?MF_GRAYED:MF_ENABLED,
		//T_QMPLS_ADDFILES
		MF_BYCOMMAND|MF_ENABLED,
		//T_QMPLS_REMOVEFILE
		MF_BYCOMMAND|bSelected?MF_ENABLED:MF_GRAYED,
		//T_QMPLS_CLOSE
		MF_BYCOMMAND|MF_ENABLED

	};

	DWORD nPosition[] = {
		ID_QMPLS_OPEN, ID_QMPLS_SAVE,
		ID_QMPLS_CLEARPLS, ID_QMPLS_ADDFILES,
		ID_QMPLS_REMOVEFILE, ID_QMPLS_CLOSE
	};

	long shortcut[][2] = {
		KEY_PLSOPEN, 0,
		KEY_PLSSAVE, 0,
		KEY_PLSCLEAR, 0,
		KEY_PLSADDFILES, 0,
		KEY_PLSDELETE, 0,
		KEY_PLSCLOSE, 0
	};
/*			

		}
		else
			pszkey = GetKeyName( shortcut[i][0] );

		strncpy( sz,pszstart, psz-pszstart ); sz[psz-pszstart] = 0;

		if ( pszkey )
			sprintf( sz+strlen(sz), "\t%s", pszkey );
			*/
	int count = sizeof(dwFlags)/sizeof(DWORD);
	for (int i=0; i<count; i++)
	{
		DWORD f = dwFlags[i];
		char sz[256], 
			*psz = transl( nPosition[i], TRANSL_QMENU_PLS ),
			*pszstart = psz,
			*pszkey;

		while ( *psz && *psz!='#' ) *psz++;
		if ( f&MF_CHECKED && *psz=='#' ) {
			psz++; pszstart = psz; while ( *psz ) *psz++; 
			pszkey = GetKeyName( shortcut[i][1] );
		}
		else
			pszkey = GetKeyName( shortcut[i][0] );

		strncpy( sz,pszstart, psz-pszstart ); sz[psz-pszstart] = 0;

		if ( pszkey ) sprintf( sz+strlen(sz), "\t%s", pszkey );

		pQuickMenu->ModifyMenu( nPosition[i], f, nPosition[i],  sz );
	}

	POINT pt; GetCursorPos( &pt );
	pQuickMenu->TrackPopupMenu( TPM_LEFTALIGN|TPM_RIGHTBUTTON,
		pt.x, pt.y, this, NULL );

	bQuickMenu = TRUE;
}

void CPlaylist::OnRButtonUp(UINT nFlags, CPoint point) 
{

	ShowQuickMenu();
	
	CDialog::OnRButtonUp(nFlags, point);
}

void CPlaylist::OnQmplsClose() 
{
	OnClose();	
}

void CPlaylist::OnQmplsClearpls() 
{
	m_List.ResetContent();
	ClearList();
}

void CPlaylist::OnQmplsAddfiles() 
{
	AddFiles();	
}

void CPlaylist::OnQmplsRemovefile() 
{
	Remove( m_List.GetCurSel() );	
}

void CPlaylist::OnSelchangeListPlaylist() 
{
	
}

void CPlaylist::OnPlsDeleteselfile() 
{
	Remove( m_List.GetCurSel() );
}

BOOL CPlaylist::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}

void CPlaylist::OnNcPaint() 
{
	HDC hDC = ::GetWindowDC( m_hWnd );

	int framex = GetSystemMetrics(SM_CXFRAME)/2,
		 framey = GetSystemMetrics(SM_CYFRAME)/2;

	RECT rc; GetWindowRect( &rc );

	SetRect( &rc, framex, framey, 
		RECTW(rc)-framex, 
		GetSystemMetrics(SM_CYSMCAPTION)+framey );

	FrameRect( hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH) );
	rc.bottom--;
	DrawCaption( m_hWnd, hDC, &rc, DC_SMALLCAP|DC_GRADIENT|DC_TEXT|DC_ACTIVE );

	GetWindowRect( &rc );
	RECT rcDest = { 0,0, RECTW(rc), RECTH(rc) };

	FrameRect( hDC, &rcDest, (HBRUSH)GetStockObject(GRAY_BRUSH) );
	InflateRect( &rcDest, -1, -1 );
	FrameRect( hDC, &rcDest, (HBRUSH)GetStockObject(BLACK_BRUSH) );


	::ReleaseDC( m_hWnd, hDC );
}

void CPlaylist::AddFiles()
{
	char szFileName[MAX_PATH*16];

	char szFilter[] = {
		"All supported files|*.avi;*.mpg;*.mpeg;*.mpe;*.mpa;*.m1v;"
			"*.asf;*.wmv;*.mov;*.divx;*.dat;*.imp;*.m3u|"

		"Movie files (*.avi;*.mpg;*.mpeg;*.mpe;*.mpa;*.m1v;"
			"*.asf;*.wmv;*.mov;*.divx;*.dat)|"
			"*.avi;*.mpg;*.mpeg;*.mpe;*.mpa;*.m1v;"
			"*.asf;*.wmv;*.mov;*.divx;*.dat|"

		"Playlist files (*.imp;*.m3u)|"
			"*.imp;*.m3u)|"

		"All Files (*.*)|*.*||"
	}; for(char *p = szFilter; *p; p++) if(*p == '|') *p = '\0' ;

	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetParent()->m_hWnd;
	ofn.Flags = OFN_FILEMUSTEXIST |OFN_ALLOWMULTISELECT|OFN_EXPLORER |
			OFN_SHAREAWARE | OFN_PATHMUSTEXIST;

	ofn.lpstrInitialDir = pConfig->szLastMoviePath;
	ofn.lpstrFilter = szFilter;      

	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ZeroMemory(szFileName, sizeof(szFileName));

	if( GetOpenFileName(&ofn) )
	{
		char *pszDir = szFileName, 
			  *pszFileName = (char*)pszDir + ofn.nFileOffset;

		int storedCurr = curr;
		strcpy( pConfig->szLastMoviePath, pszDir );
 		GoToLast();


		BOOL bAdded = FALSE;
		if ( *(pszFileName-1) ) {
			// Single file selected.
			//OpenMovie(szFileName, pConfig->bAutoPlay );
			Add( szFileName, TRUE );
			bAdded = TRUE;

		} else {
			// Multiple files selected.

			while( *pszFileName ) {
				char sz[_MAX_PATH];
				sprintf(sz,"%s\\%s", pszDir, pszFileName);
				pszFileName += strlen(pszFileName)+1;

				Add( sz, !bAdded );
				bAdded = TRUE;
			}
		}
		
		curr = storedCurr;
		m_List.SetCurSel( curr-1 );
	}

	if ( IsWindowVisible() ) SetActiveWindow();

}
void CPlaylist::OpenPlaylist()
{
	char szFileName[MAX_PATH*16];

	char szFilter[] = {
		"Playlist files (*.imp;*.m3u)|"
			"*.imp;*.m3u)|"

		"All Files (*.*)|*.*||"
	}; for(char *p = szFilter; *p; p++) if(*p == '|') *p = '\0' ;

	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwndParent;
	ofn.Flags = OFN_FILEMUSTEXIST |OFN_ALLOWMULTISELECT|OFN_EXPLORER |
			OFN_SHAREAWARE | OFN_PATHMUSTEXIST;

	ofn.lpstrInitialDir = pConfig->szLastMoviePath;
	ofn.lpstrFilter = szFilter;      

	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ZeroMemory(szFileName, sizeof(szFileName));

	if( GetOpenFileName(&ofn) )
	{
		char *pszDir = szFileName, 
			  *pszFileName = (char*)pszDir + ofn.nFileOffset;
		
		strcpy( pConfig->szLastMoviePath, pszDir );

		ClearList();

		BOOL bAdded = FALSE;
		if ( *(pszFileName-1) ) {
			// Single file selected.
			//OpenMovie(szFileName, pConfig->bAutoPlay );
			Add( szFileName, TRUE );
			bAdded = TRUE;

		} else {
			// Multiple files selected.

			while( *pszFileName ) {
				char sz[_MAX_PATH];
				sprintf(sz,"%s\\%s", pszDir, pszFileName);
				pszFileName += strlen(pszFileName)+1;

				Add( sz, !bAdded );
				bAdded = TRUE;
			}
		}
		if ( bAdded ) {
			::PostMessage( hwndParent, WM_COMMAND, IDM_CALLBACK_OPENFROMPLS, NULL );
		}
	}

	if ( IsWindowVisible() ) SetActiveWindow();

}
void CPlaylist::SavePlaylist()
{
	char szFileName[MAX_PATH];

	char szFilter[] = {
		"Playlist files (*.imp)|"
			"*.imp|"

		"All Files (*.*)|*.*||"
	}; for(char *p = szFilter; *p; p++) if(*p == '|') *p = '\0' ;
	char szDefExt[] = "imp";

	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwndParent;
	ofn.Flags = OFN_EXPLORER;

	ofn.lpstrInitialDir = pConfig->szLastMoviePath;
	ofn.lpstrFilter = szFilter;      

	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ZeroMemory( szFileName, sizeof(szFileName) );
	ofn.lpstrDefExt = szDefExt;


	if( GetSaveFileName(&ofn) )
	{
		FILE *f = fopen( szFileName, "wt" );

		if ( !f ) return;
		
		for ( int i=1; i<=count; i++ )
		{

			LPCTSTR pszMoviePath = (*pRecentFileList)[count-i];
			char *pszmovie = (char*)pszMoviePath,
					*pszpls = szFileName;
			if ( *pszmovie==*pszpls )
			{
				int skipped = 0;
				while ( *pszmovie && *pszpls &&
							*pszmovie++==*pszpls++ ) skipped++;
				pszmovie--; pszpls--;

				while ( *pszpls++ ) 
					if ( *pszpls=='\\' ) {
						pszmovie = (char*)pszMoviePath+2; break; }
			}
			fprintf( f, "%s\n", pszmovie );

		}


		fclose(f);

	}

	if ( IsWindowVisible() ) SetActiveWindow();
}

void CPlaylist::OnQmplsOpen() 
{
	OpenPlaylist();	
}

void CPlaylist::OnQmplsSave() 
{
	SavePlaylist();
}

BOOL CPlaylist::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD wNotifyCode = HIWORD(wParam); // notification code 
	WORD wID = LOWORD(wParam);

	if ( wNotifyCode ) switch ( wID )
	{	// Notify from accelerator.
		
		case KEY_PLSOPEN: OnQmplsOpen(); return TRUE;
		case KEY_PLSSAVE: OnQmplsSave(); return TRUE;
		case KEY_PLSCLEAR: OnQmplsClearpls(); return TRUE;
		case KEY_PLSADDFILES: OnQmplsAddfiles(); return TRUE;
		case KEY_PLSDELETE: OnQmplsRemovefile(); return TRUE;
		case KEY_PLSPLAY: OnDblclkListPlaylist(); return TRUE;


		case KEY_PLSCLOSE: OnQmplsClose(); return TRUE;

		default: if (wID>=ID_ACCEL) return TRUE;

	}
	else switch ( wID )
	{
		case IDM_CALLBACK_LANGUAGECHANGED:
			InitLanguage();
			return TRUE;		// Do not process this message further.
	}

	
	return CDialog::OnCommand(wParam, lParam);
}

void CPlaylist::InitLanguage()
{
	SetWindowText( transl(IDD_DIALOG_PLAYLIST, TRANSL_QMENU_PLS) );
}
