// MainWnd.cpp : implementation file
//

#include "stdafx.h"

#include "XPPlayer.h"
#include "MainWnd.h"
#include "language.h"
#include "accelerators.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD dwTicks;
void start_time()
{
	dwTicks = GetTickCount();
}
void end_time(HWND hWnd)
{
	dwTicks = GetTickCount() - dwTicks;
	char sz[32]; sprintf(sz, "Run Time: %i ms",dwTicks);
	if (hWnd)
		MessageBox(hWnd, sz, "Speed test", MB_OK);
}





/////////////////////////////////////////////////////////////////////////////
// CMainWnd

CMainWnd::CMainWnd()
{
	memset(szError, 0, 256);

	modeId = MODE_NONE;
	prevModeId = MODE_NONE;

	pskinNormal = NULL;
	pskinCompact = NULL;
	pskinFullscreen = NULL;
	pSkin = NULL;
	pprevSkin = NULL;

	hWndMovie = NULL;

	pOptions = NULL;

	hdcBackground = NULL;
	pMovie = NULL;
	stateMovie = VCD_NO_CD;

	ZeroMemory(&ofn, sizeof(ofn));

	//window reaction delay
	timeWindowDelay = 20;
	timeWindowReaction = GetTickCount();

	bMultipleFiles = FALSE;
	bSubtitles = FALSE;

	sizeWnd.cx = 0; sizeWnd.cy = 0;
	

	//mouse and window var
	bMovingWindow = FALSE;
	bMaximized = FALSE;
	idButtonPress = -1; idButtonOver = -1;
	idScrollbarPress = -1; idScrollbarOver = -1;

	timeMouseMove = 0;

	//movie var.
	dMoviePosition = 0.0;


	bAllowHideMouse = FALSE;
	bMouseHidden = FALSE;

	pTooltip = NULL;

	bShowPlaylist = FALSE;

	pMenu = NULL;
	bQuickMenu = FALSE;

	pFiltersProp = NULL;

	sizeScreen.cx = GetSystemMetrics( SM_CXSCREEN );
	sizeScreen.cy = GetSystemMetrics( SM_CYSCREEN );
	sizeMaximized.cx = GetSystemMetrics( SM_CXMAXIMIZED );
	sizeMaximized.cy = GetSystemMetrics( SM_CYMAXIMIZED );


	bStoredFsAllowResize = FALSE;
	SetRectEmpty( &rcStoredFsPos );
	SetRectEmpty( &rcStoredWindowedPos );

	bFsConstPos = FALSE;

	pSubtitlesWnd = NULL;

}

CMainWnd::~CMainWnd()
{
	CWnd::DestroyWindow();
}

//return 0 on success
int CMainWnd::cfg_parse_element( char **buffer,
				CMainWnd::cstr *opt, int *opts, int *level )
{
	#define SKIP_ENDL_SPACES							\
		{while(*buff && *buff<=32 ) buff++;				\
		 if (!*buff) return 1; /*unexpected eof*/ }

	//XMLVERINFO looks: <? ... ?>
	#define SKIP_XMLVERINFO									\
		if (*buff=='?') {										\
			while (*++buff && *buff!='>') ;				\
			if (!*buff) return 1; /*unexpected eof*/	\
			*opts = 0; *level = 0; *buffer = ++buff;	\
			return 0; }				
	
	//COMMENT looks: <!-- ... -->
	#define SKIP_COMMENT													\
	if (*buff=='!') { buff++;											\
			if ( *buff++!='-' || *buff++!='-' ) return 3;		\
			do {																\
				if (*buff=='-') buff++;									\
			} while (*buff && *buff++!='-');							\
			if (!*buff || *buff!='>') return 3;						\
			*opts = 0; *level = 0; *buffer = ++buff;				\
			return 0; }														\


	#define PARSE_LABEL											\
		if(*buff && ((*buff>='a' && *buff<='z') ||				\
			(*buff>='A' && *buff<='Z') || *buff=='_' )) {		\
			while (*++buff && ((*buff>='a' && *buff<='z') ||	\
				  (*buff>='A' && *buff<='Z') ||					\
				  (*buff>='0' && *buff<='9') || *buff=='_' )) ;	\
			if (!*buff) return 1; /*unexpected eof*/ }			\
		else return 2; /*expected label*/
	
	#define PARSE_STRING									\
		if (*buff && *buff=='"') {							\
			while (*++buff && *buff<=126 && *buff!='"') ;	\
			if (!*buff) return 1; /*unexpected eof*/		\
			if (*buff>126)	return 3; /*inavlid char*/		\
			buff++;											\
		} else return 4; /*expected string*/

	char *ps, *buff = *buffer;
	*opts = 0; *level = 0;


	SKIP_ENDL_SPACES
	if (*buff++!='<') return 3; /*invalid char*/
	SKIP_ENDL_SPACES
	SKIP_XMLVERINFO
	SKIP_COMMENT
	if (*buff=='/') { 
		(*level)--; buff++;
		SKIP_ENDL_SPACES
	}
	ps = buff;
	PARSE_LABEL
	opt->set(ps, buff-ps); opt++; (*opts)++;
	if (*level == -1) {
		SKIP_ENDL_SPACES
		if (*buff=='>') {
			*buff++; *buffer = buff;
			return 0;
		}
		return 3; /*invalid char*/
	}

	while (*buff && *buff!='/' && *buff!='>')
	{
		SKIP_ENDL_SPACES
		ps = buff; PARSE_LABEL 
		opt->set(ps, buff-ps); opt++; (*opts)++;

		SKIP_ENDL_SPACES
		if (*buff++!='=') return 3; /*invalid char*/


		SKIP_ENDL_SPACES
		ps = buff; PARSE_STRING
		opt->set(ps+1, buff-ps-2); opt++; (*opts)++;

		SKIP_ENDL_SPACES
	}
	if (!*buff) return 1; /*unexpected eof*/
	if (*buff=='/') { buff++; (*level)--; SKIP_ENDL_SPACES}
	if (*buff++!='>') return 3; /*invalid char*/

	*buffer = buff; (*level)++;

	return 0;
}


BEGIN_MESSAGE_MAP(CMainWnd, CWnd)
	//{{AFX_MSG_MAP(CMainWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ACTIVATE()
	ON_WM_SYSCOMMAND()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_MOUSEWHEEL()
	ON_WM_PARENTNOTIFY()
	ON_WM_DROPFILES()
	ON_WM_MBUTTONUP()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CMainWnd::SetMenuBitmap(char *pszId, cGImage *pimg, int x, int y, 
		int xChecked, int yChecked, BOOL bExistChecked)
{
	DWORD nId[] = {
		IDM_PLAYPAUSE, IDM_STOP, IDM_REWINDBACK, IDM_REWINDFORWARD,
		IDM_PREVMOVIE, IDM_NEXTMOVIE, IDM_PLAYLIST, 
		IDM_FULLSCREEN, IDM_PANSCAN,
		IDM_SHOWSUBTITLES,
		IDM_OPENMOVIE, IDM_OPENSUBTITLES,
		IDM_OPTIONS, IDM_SKINBROWSER,
		IDM_EXIT
	};

	char *szId[] = {
		"play_pause", "stop", "rewind_back", "rewind_forward",
		"prev_movie", "next_movie", "playlist",
		"fullscreen", "panscan",
		"show_subtitles",
		"open_movie", "open_subtitles",
		"options", "skin_browser",
		"exit"
	};

	int count = sizeof(nId)/sizeof(DWORD);
	for (int i=0; i<count && _stricmp(pszId, szId[i]); i++) ;

	if (i==count || !pimg || !pimg->hBitmap ) return FALSE;
	
//	int	w=GetSystemMetrics(SM_CXMENUCHECK),
//			h=GetSystemMetrics(SM_CXMENUCHECK);
	int w=13, h=13;

	
	static CBitmap *pMenuBitmap = NULL;
	if ( !pMenuBitmap ) pMenuBitmap = new CBitmap[count*2];


	CDC *pWndDC = GetDC();
	CBitmap *pbm = pMenuBitmap+i, *pbmchecked = pMenuBitmap+i+count;;
	if ( HBITMAP hbm = *pbm ) return FALSE;
	pbm->CreateCompatibleBitmap( pWndDC, w,h );
	pbmchecked->CreateCompatibleBitmap( pWndDC, w,h );

	CDC src, dest;
	src.CreateCompatibleDC( pWndDC ); 
	::SelectObject( src.m_hDC, pimg->hBitmap );
	dest.CreateCompatibleDC( pWndDC );

	dest.SelectObject( pbm );
	dest.BitBlt(0,0,w,h,&src,x,y,SRCCOPY);
	dest.SelectObject( pbmchecked );
	if ( bExistChecked && xChecked>=0 && yChecked>=0 )
		dest.BitBlt(0,0,w,h,&src,xChecked,yChecked,SRCCOPY);
	else
		dest.BitBlt(0,0,w,h,&src,x,y,SRCCOPY);

	CMenu *pQuickMenu = pMenu->GetSubMenu(0);
	BOOL ret = pQuickMenu->SetMenuItemBitmaps( nId[i], MF_BYCOMMAND, 
		pbm, pbmchecked );

	
	ReleaseDC( pWndDC );

	return TRUE;
}



/*-------------------- LoadXMLSkin() ---------------------------
parsing xml skin config file and loading data
path - directory of skin (absolute)
				example: "c:/programy/player/skins/default"
		 or ZIP file ex. "c:/programy/player/skins/default.zip"

RET:  on ERROR return 0 - general error, or 0x10+line with error
	  on OK return 1

  skin config file: "player.xml";
--------------------------------------------------------------*/
int CMainWnd::LoadXMLSkin()
{
	int ret = 0;

	cFile File;
	cArchive Archive;

	DWORD dwFilesize = 0;
	char *pFileXML = NULL;

	char szConfigXML[] = "player.xml";

	char szSkinPath[ _MAX_PATH ], sz[_MAX_PATH];
	sprintf( sz, "%s%s", pOptions->szSkinDirectory, pOptions->szSkinFileName );
	_makepath( szSkinPath, NULL, pOptions->szAppPath, sz, NULL );

	if ( pOptions->bSkinInZIP ) {
		if ( !Archive.OpenZIP( szSkinPath ) ) {
			sprintf(szError, "Skin file definition 'player.xml' "
						"not found.");
			return ret;
		}

		dwFilesize = Archive.SizeFile( szConfigXML );
		pFileXML = new char [ dwFilesize + 1 ];
		Archive.ReadFile( "player.xml", (BYTE*)pFileXML, &dwFilesize );

	} else {
		char file[_MAX_PATH];
		_makepath( file, NULL, szSkinPath, szConfigXML, NULL );
  
		if ( !File.Open( file, "rb") ) {
			sprintf(szError, "Skin file definition 'player.xml' "
				"not found.");
			return ret;
		}

		dwFilesize = File.Size();
		pFileXML = new char[ dwFilesize + 1 ];
		File.Read( pFileXML, dwFilesize );
	}

	if (!pFileXML || !dwFilesize) {
		sprintf(szError, "Skin file definition 'player.xml' not found .");
		delete[] pFileXML;
		return ret;
	}

	char *b = pFileXML; b[ dwFilesize ]=0;

	// create skins
	CSkin *psNormal = new CSkin(),
			*psCompact = new CSkin(),
			*psFullscreen = new CSkin(),
			*ps = NULL;

	#define MAX_ELEMENTS 64
	cstr elements[ MAX_ELEMENTS ];
	cstr str_level[16], *plevel = str_level;
	int countElem = 0, level=0, l;
	BOOL loadedBackground = FALSE, 
		 loadedButtons = FALSE;

	int buttonId = -1, scrollbarId = -1;
	BOOL levelResize = FALSE, levelMenuBitmaps = FALSE;
	ps = NULL;

	while ( !cfg_parse_element(&b, elements, &countElem, &l) ) {

		if ( !countElem ) continue;
		if ( countElem>= MAX_ELEMENTS )
			countElem = MAX_ELEMENTS;

		cstr *pel = elements;

		if (l==1) 
			*plevel = *pel;

		if (l==-1) {
			if ( level==0 || (plevel-1)->cmp( pel ) )
				break; //error

			level--; plevel--;
			if ( level==1 && !pel->cmp("skin") ) {
				scrollbarId = -1;
				buttonId = -1;
				ps = NULL;
			}

			if ( level==2 && !pel->cmp("button") )
				buttonId = -1;
			//
			if ( level==2 && !pel->cmp("scrollbar") ) {
				ps->AddScrollbar( scrollbarId, SSTATE_CORRECT_SLIDER,
											0,0,0,0 ,0 );
				scrollbarId = -1;
			}

			if ( level==2 && !pel->cmp("resizeDefinition") && levelResize) 
				levelResize = FALSE;

			if ( level==2 && !pel->cmp("menuBitmaps") && levelMenuBitmaps) {
				DeleteObject( pImgMenu->hBitmap );
				delete pImgMenu;
				levelMenuBitmaps = FALSE;
			}

			continue;
		}

		level += l; plevel += l;


		cstr *type = NULL, *id = NULL, *file = NULL, *bitmap = NULL;
		int x = 0, y = 0, h = 0, w = 0, marginLeft = 0, marginRight = 0,
			 xLine[ MAX_RESIZE_RECTS ], xLinesCount = 0,
			 yLine[ MAX_RESIZE_RECTS ], yLinesCount = 0,
			 fontSize = 8, fontColor = 0xFFFFFF,
			 xChecked = -1, yChecked = -1,
			 shapeId = SHAPE_RECT;
		BOOL xset = FALSE, yset = FALSE, wset = FALSE, hset = FALSE,
				constpos = TRUE;

		type = pel; pel++; countElem--;

		while ( countElem>=2 ) {
			if ( !pel->cmp("id") ) {
				pel++;
				id = pel;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("constPos") ) {
				pel++;
				if ( !pel->cmp("FALSE") ) constpos = FALSE;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("file") ) {
				pel++;
				file = pel;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("bitmap") ) {
				pel++;
				bitmap = pel;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("x") ) {
				pel++;
				x = atoi( pel->psz() ); xset = TRUE;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("y") ) {
				pel++;
				y = atoi( pel->psz() ); yset = TRUE;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("xChecked") ) {
				pel++;
				xChecked = atoi( pel->psz() );
				pel++; countElem -= 2;
			} else if ( !pel->cmp("yChecked") ) {
				pel++;
				yChecked = atoi( pel->psz() );
				pel++; countElem -= 2;
			} else if ( !pel->cmp("h") ) {
				pel++;
				h = atoi( pel->psz() ); hset =TRUE;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("w") ) {
				pel++;
				w = atoi( pel->psz() ); wset = TRUE;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("marginLeft") ) {
				pel++;
				marginLeft = atoi( pel->psz() ); wset = TRUE;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("marginRight") ) {
				pel++;
				marginRight = atoi( pel->psz() ); wset = TRUE;
				pel++; countElem -= 2;
			} else if ( !pel->cmp("fontSize") ) {
				pel++;
				fontSize = atoi( pel->psz() );
				pel++; countElem -= 2;
			} else if ( !pel->cmp("fontColor") ) {
				pel++;
				char *string = pel->psz(), *stopstring;
				if ( *string=='#' ) {
					fontColor = strtoul( string+1, &stopstring, 16 ); 
					// Convert from GBR to RGB.
					fontColor = ((fontColor&0xFF)<<16) +
									(fontColor&0xFF00) + 
									((fontColor&0xFF0000)>>16);
				}
				pel++; countElem -= 2;
			} else if ( !_strnicmp(pel->psz(), "xLine", 5) ) {
				pel++;
				if (xLinesCount<MAX_RESIZE_RECTS )
					xLine[ xLinesCount++ ] = atoi( pel->psz() );
				pel++; countElem -= 2;
			} else if ( !_strnicmp(pel->psz(), "yLine", 5) ) {
				pel++;
				if (yLinesCount<MAX_RESIZE_RECTS )
					yLine[ yLinesCount++ ] = atoi( pel->psz() );
				pel++; countElem -= 2;
			} else if ( !pel->cmp("shape") ) {
				pel++;
				if ( !pel->cmp("singleAlpha") ) shapeId = SHAPE_SINGLEALPHA;
				else if ( !pel->cmp("multiAlpha") ) shapeId = SHAPE_MULTIALPHA;
				pel++; countElem -= 2;
			} else {
				pel += 2; countElem -= 2;
			}
		}// while (countElem>=2)



		if (level==1 && type->cmp("player") )
			break;

		if ( level==3 && !type->cmp("resizeDefinition") )
			levelResize = TRUE;

		if ( level==2 && !type->cmp("menuBitmaps") && file )
		{
			pImgMenu = new cGImage();
			if (pOptions->bSkinInZIP) {
				pImgMenu->LoadImage(file->psz(), &Archive);
				pImgMenu->ConvertToBitmap(m_hWnd);
			}
			else {
				char szPath[_MAX_PATH];
				_makepath( szPath, NULL, szSkinPath, file->psz(), NULL );

				pImgMenu->LoadImage(szPath);
				pImgMenu->ConvertToBitmap(m_hWnd);

			}
			levelMenuBitmaps = TRUE;

			continue;
		}


		//@@@ from now id must exist
		if (level==2 && !id ) continue;

		if (level==2) {

			if ( !type->cmp("skin") ) {

				if ( !id->cmp("normal") )
					ps = psNormal;
				else if ( !id->cmp("compact") )
					ps = psCompact;
				else if ( !id->cmp("fullscreen") )
				{
					ps = psFullscreen;
					bFsConstPos = constpos;
				}

				continue;
			}
			
		}

		if ( level==2 && levelMenuBitmaps && !type->cmp("item") && xset && yset ) {

			if ( xChecked>=0 && yChecked >=0 )
				SetMenuBitmap( id->psz(), pImgMenu, x, y, xChecked, yChecked, TRUE );
			else 
				SetMenuBitmap( id->psz(), pImgMenu, x, y );
		}


		//@@@ from now ps must exist
		if ( !ps ) continue;

		if ( level==2) {

			if ( !type->cmp("bitmap") && file ) {

					int imageId;

					imageId = ps->GetImageId( id->psz() );
					if ( imageId == -1 ) break;

					if ( pOptions->bSkinInZIP )
						ps->AddImage( imageId, file->psz(), m_hWnd,
									NULL, &Archive, 
									(IMAGE_BACKGROUND==imageId) );
					else
						ps->AddImage( imageId, file->psz(), m_hWnd,
									szSkinPath, NULL, 
									(IMAGE_BACKGROUND==imageId) );

					if (!loadedBackground)
						loadedBackground = (IMAGE_BACKGROUND==imageId);

					continue;
			}

			if ( !type->cmp("textArea") && xset && yset && hset && wset ) {

				ps->AddStatus( id->psz(), x,y,w,h, fontSize,fontColor );

				continue;

			}

			if( wset && hset && !type->cmp("size") && !id->cmp("minimal") ) {

				ps->SetMinimalSize( w, h );
			}

			if ( !type->cmp("area") && !id->cmp("movie.playback") &&
				xset && yset && hset && wset ) {

				ps->SetPlayArea( x,y,w,h );

				continue;

			}


		}

		if ( level==3 ) {

			if ( !type->cmp("button") && xset && yset && wset && hset ) {

				buttonId = ps->GetButtonId( id->psz() );
				if ( buttonId == -1 ) continue;	//@@@ break

				if ( !ps->AddButton( buttonId, BSTATE_DEFPOSITION,
												x, y, w, h, 0, shapeId) ) 
				{
						buttonId = -1;
						continue; 
				}

				loadedButtons = TRUE;

				continue;
			}


			if ( !type->cmp("state") && bitmap && xset && yset && buttonId>=0 )
			{

				int buttonState = ps->GetButtonState( id->psz() );
				if ( buttonState==-1 ) continue;	//@@@ break

				int imageId = ps->GetImageId( bitmap->psz() );
				if ( imageId==-1 ) continue;

				ps->AddButton(buttonId, buttonState, x, y, 
											w, h, imageId, shapeId );

				continue;
			}


			if ( !type->cmp("scrollbar") && xset && yset && wset && hset ) {

				scrollbarId = ps->GetScrollbarId( id->psz() );
				if ( scrollbarId == -1 ) continue;	//@@@ break

				if ( !ps->AddScrollbar( scrollbarId, SSTATE_DEFPOSITION,
												x, y, w, h, 0, marginLeft, marginRight) ) 
					scrollbarId = -1;

				continue;
			}


			if ( !type->cmp("state") && bitmap && xset && yset && scrollbarId>=0 )
			{

				int scrollbarState = ps->GetScrollbarState( id->psz() );
				if ( scrollbarState==-1 ) continue;	//@@@ break

				int imageId = ps->GetImageId( bitmap->psz() );
				if ( imageId==-1 ) continue;

				ps->AddScrollbar(scrollbarId, scrollbarState , x, y, 
											w, h, imageId );

				continue;
			}

			if ( !type->cmp("resize") && levelResize) 
			{
				RECT *rc;
				int countLines, *pline;
				if ( !id->cmp("top") ) { 
					if ( !hset || !xLinesCount ) continue;
					rc = ps->rcResizeTop; 
					y = 0; w = 0; pline = xLine; countLines = xLinesCount;
					ps->countRectsTop = countLines;

				} else if ( !id->cmp("bottom") ) {
					if ( !hset || !xLinesCount ) continue;
					rc = ps->rcResizeBottom; 
					y = 0; w = 0; pline = xLine; countLines = xLinesCount;
					ps->countRectsBottom = countLines;

				} else if ( !id->cmp("left") ) {
					if ( !wset || !yLinesCount ) continue;
					rc = ps->rcResizeLeft; 
					x = 0; h = 0; pline = yLine; countLines = yLinesCount;
					ps->countRectsLeft = countLines;

				} else if ( !id->cmp("right") ) {
					if ( !wset || !yLinesCount ) continue;
					rc = ps->rcResizeRight;
					x = 0; h = 0; pline = yLine; countLines = yLinesCount;
					ps->countRectsRight = countLines;
				} else continue;

				for(int i=0; i<countLines-1; i++)
					for (int j=i+1; j<countLines; j++)
						if ( pline[i]>pline[j] ) {

							int temp = pline[i]; 
							pline[i] = pline[j]; pline[j] = temp;

						}

				for (i=0; i<countLines; i++)
					if (xLinesCount) SetRect( rc+i, xLine[i], y, xLine[i], h );
					else SetRect(rc+i, x, yLine[i], w, yLine[i]); 
					


			}

		}

	}//END while ( !cfg_parse_element(&b, elements, &countElem, &l) )

	while (*b && *b<32 ) b++;	//skip spaces at end file
	if ( *b ) {
		int errLine = 0;
		while (b>pFileXML) if (*b-- == 0x0a ) errLine++;

		sprintf(szError, "Syntax error in skin definition file "
				"'player.xml' at line %i .", errLine+1);

		delete[] pFileXML;
		delete psNormal;
		delete psCompact;
		delete psFullscreen;

		return 0x10+errLine+1;
	}

	if ( !loadedBackground || !loadedButtons ) {
		sprintf(szError, "Can't find primary skin definition structures "
				"in file 'player.xml' \n(not defined bitmap background "
				"or buttons) .");

		delete[] pFileXML;
		delete psNormal;
		delete psCompact;
		delete psFullscreen;

		return ret;
	}

	delete[] pFileXML;


	//initialize all skins
	psNormal->Initialize();
	psCompact->Initialize();
	psFullscreen->Initialize();

	psFullscreen->SetPlayArea(0,0,0,0);

	if ( pskinNormal ) 
	{
		pskinNormal->ShareBackgroundWith( psNormal );
	} 
	else
	{
		hdcBackground = psNormal->CreateBackground(m_hWnd);
		if ( !hdcBackground ) {
			delete psNormal;
			delete psCompact;
			delete psFullscreen;

			return ret;	//should exit from app
		}
	}
	psNormal->ShareBackgroundWith( psCompact );
	psNormal->ShareBackgroundWith( psFullscreen );

	SAFE_DELETE( pskinNormal ); pskinNormal = psNormal;
	SAFE_DELETE( pskinCompact ); pskinCompact = psCompact;
	SAFE_DELETE( pskinFullscreen ); pskinFullscreen = psFullscreen;


	if ( modeId==MODE_NONE ) {
		// Skin is loading at startup.

		int newModeId  = pOptions->modeId;
		switch ( pOptions->modeId ) {
			case MODE_NORMAL: ps = psNormal; break;
			case MODE_COMPACT: 
				ps = psCompact; 
				if ( !ps->IsDefined() ) { 
					ps = psNormal; newModeId = MODE_NORMAL; }
				break;
			case MODE_FULLSCREEN: ps = psFullscreen; break;

			default:
				ps = psNormal; newModeId = MODE_NORMAL;
				break;
		}

		if ( !sizeWnd.cx || !sizeWnd.cy ) ps->GetSkinSize( &sizeWnd.cx, &sizeWnd.cy );

		pSkin = ps;
		prevModeId = pOptions->prevModeId;
		SetMode( newModeId );

		return TRUE;
	}


	// If we are here skin is changing ( selected new skin 
	//												  from skin browser ).

	switch ( modeId ) {
		case MODE_NORMAL: 
			ps = psNormal;
			SetRectEmpty( &rcStoredFsPos );
			if ( bMaximized && !ps->AllowResize() ) {
				SetWindowPos( NULL, rcMaximized.left, rcMaximized.top, 0,0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOOWNERZORDER
					|SWP_NOCOPYBITS);
				bMaximized = FALSE; 
			}
			break;

		case MODE_COMPACT: 
			ps = psCompact;
			if ( !ps->IsDefined() ) { ps = psNormal; modeId = MODE_NORMAL; }

			SetRectEmpty( &rcStoredFsPos );
			if ( bMaximized && !ps->AllowResize() ) {
				SetWindowPos( NULL, rcMaximized.left, rcMaximized.top, 0,0,
					SWP_NOZORDER|SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOOWNERZORDER
					|SWP_NOCOPYBITS);
				bMaximized = FALSE; 
			}
			break;

		case MODE_FULLSCREEN: 
			ps = psFullscreen;
			ps->SetPlayArea( 0,0, sizeScreen.cx, sizeScreen.cy );

			if ( ps->IsDefined() ) 
			{
				if ( bFsConstPos ) {

					if ( ps->AllowResize() ) {

						long w,h; ps->GetSkinSize( &w, &h);
						w = sizeScreen.cx;
						//ps->Resize(m_hWnd, &w, &h, TRUE );
						ps->Resize(m_hWnd, &w, &h, TRUE );

					} else {

						ps->Resize(m_hWnd, &sizeWnd.cx, 
							&sizeWnd.cy, TRUE );
					}

					ps->SetPlayArea( 0,0, sizeScreen.cx, sizeScreen.cy );

					long w,h; ps->GetSkinSize( &w, &h);
					SetWindowPos( NULL, 0, sizeScreen.cy-h, 0,0, 
						SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|
						SWP_NOCOPYBITS|SWP_NOOWNERZORDER);

					bStoredFsAllowResize = ps->AllowResize();

					ps->SetAllowResize( FALSE );
					ps->SetAllowMove( FALSE );
				
				} else {

					// Center fullscreen skin window.
					long w,h; ps->GetSkinSize( &w, &h);
					sizeWnd.cx = w; sizeWnd.cy = h;
					ps->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );

					ps->SetPlayArea( 0,0, sizeScreen.cx, sizeScreen.cy );

					SetWindowPos( NULL, (sizeScreen.cx-w)/2,
						(sizeScreen.cy-h)/2, 0,0, 
						SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|
						SWP_NOCOPYBITS|SWP_NOOWNERZORDER);

					bStoredFsAllowResize = ps->AllowResize();

				}
				ShowWindow( SW_SHOW );
					
			}
			else
			{
				ShowWindow( SW_HIDE );
			}

			switch ( prevModeId ) {
				case MODE_NORMAL: 
					if ( bStoredMaximized  && !psNormal->AllowResize() ) {
						long w,h; psNormal->GetSkinSize( &w, &h);
						SetRect( &rcStoredWindowedPos, 
							(sizeScreen.cx-w)/2, (sizeScreen.cy-h)/2, w, h );
						bStoredMaximized = FALSE; 
					}
					break;

				case MODE_COMPACT:
					CSkin *pstemp; pstemp = psCompact;
					if	( !psCompact->IsDefined() ) { 
						pstemp = psNormal;
						prevModeId = MODE_NORMAL; 
					}

					if ( bStoredMaximized   && !pstemp->AllowResize() ) {
						long w,h; pstemp->GetSkinSize( &w, &h);
						SetRect( &rcStoredWindowedPos, 
							(sizeScreen.cx-w)/2, (sizeScreen.cy-h)/2, w, h );
						bStoredMaximized = FALSE; 
					}
				break;
			}


			break;
	}


	pSkin = ps;
	
	if ( modeId != MODE_FULLSCREEN )
	{
		UpdateButtons( FALSE ); UpdateStatus(); 
		pSkin->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );
		RepositionMovie();
	}
	else
	{
		UpdateButtons( TRUE ); UpdateStatus(); 
	}
	SetCursor( LoadCursor(NULL, IDC_ARROW) );

	InvalidateRect(NULL); UpdateWindow();

	return TRUE;
}

BOOL CMainWnd::Create( char *szWindowName, CWnd* pParentWnd, stConfig *pOpt, CSubtitles *pSubt)
{
	BOOL ret = FALSE;
	
	pOptions = pOpt;
		
	char* szClassName = (char*)AfxRegisterWndClass( 
				0, //CS_DBLCLKS,
				0, NULL,
				LoadIcon( AfxGetInstanceHandle(), 
					MAKEINTRESOURCE(IDR_MAINFRAME) ) );

	RECT rc = { 0, 0, 0, 0 };
	ret = CWnd::CreateEx( 0, szClassName, szWindowName, WS_POPUP, 
					rc, pParentWnd, 0, NULL);

	pFullscreenWnd = (CFullscreenWnd *)pParentWnd;
	pFullscreenWnd->SetParent( this );
	pFullscreenWnd->PutInfoMovieState( &stateMovie );

	if ( pOptions->bAlwaysOnTop ) 
	{
		SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE );
		pFullscreenWnd->SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE );
	}


	CSubtitlesFs *pSubtitlesFs = new CSubtitlesFs();
	pSubtitlesFs->Create( pFullscreenWnd );

	pSubtitles = pSubt;
	pSubtitles = new CSubtitles();
	pSubtitles->Create( pOptions, pSubtitlesFs, this );

	pDlgOptions = new COptions( pOptions, this, IDD_PP_ABOUT, this );
	pDlgOptions->Create( IDD_DIALOG_OPTIONS, this );

	if (!ret) {
		sprintf(szError, "Can't create main window. ",
						 "Restart windows and try again .");
		return ret;
	}

		// Create and load menu.
	pMenu = new CMenu();
	pMenu->LoadMenu( IDR_MENU );


	if ( LoadXMLSkin()!=1 )
	{
		// Propably skin file is valid so 'unpack' from resource
		//  default skin, and change to it.
		HMODULE hModule = AfxGetApp()->m_hInstance;

		HRSRC hRsrc = FindResource( hModule, MAKEINTRESOURCE(IDR_DEFAULTSKIN), "RAWDATA" );
		HGLOBAL hResData = LoadResource( hModule, hRsrc );
		DWORD size = SizeofResource( hModule, hRsrc );
		char *zip = (char*)LockResource( hResData );

		char szSkinDefault[_MAX_PATH], szTemp[ _MAX_PATH ];
		_makepath( szTemp, "", pOptions->szAppPath, pOptions->szSkinDirectory, "" );
		CreateDirectory( szTemp, NULL );
		_makepath( szSkinDefault, "", szTemp, "default.zip", "" );

		FILE *f = fopen( szSkinDefault, "wb" );
		if ( f )
		{
			fwrite( zip, size, 1, f  ); fclose(f);

			sprintf( pOptions->szSkinFileName, "default.zip" );
			pOptions->bSkinInZIP = TRUE;

			if ( LoadXMLSkin()!=1 )
			{
				sprintf( szError+strlen(szError), " \n Tried switch to default skin,"
					"but failed - YOU SHOULD RESTART WINDOWS OR REINSTALL 'The Imp'");
			
				return FALSE;
			}
		}
		else return FALSE;
	}

	timerId = SetTimer( TIMER_GLOBAL, 40, NULL );

	// This window accept drag-and-drop to load movie.
	DragAcceptFiles(TRUE);


	CWnd::ShowWindow(SW_SHOW);

	return TRUE;
}

void CMainWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting


	if ( pMovie ) {

		if ( hWndMovie == m_hWnd )
			pMovie->RepaintVideo(m_hWnd, dc.m_hDC);

		else {
			HDC hdcMovie = ::GetDC( hWndMovie );
			pMovie->RepaintVideo(hWndMovie, hdcMovie);
			::ReleaseDC( hWndMovie, hdcMovie );

		}
	}
	pSkin->DrawSkin( dc.m_hDC, (stateMovie&VCD_LOADED)?FALSE:TRUE );

}


BOOL CMainWnd::Maximize( BOOL maximize )
{
	if (maximize==bMaximized) return TRUE;

	if ( !pSkin->IsDefined() || !pSkin->AllowResize() ) return FALSE;


	
	if ( maximize && !bMaximized )
	{
		// Maximize.

		GetWindowRect( &rcMaximized );

		sizeWnd = sizeMaximized;
		if ( pOptions->bMaximizeOverTaskMenu ) sizeWnd = sizeScreen;

		int hidePixels = (sizeMaximized.cx-sizeScreen.cx)/2;
		if ( pOptions->bMaximizeOverTaskMenu ) {
			sizeWnd.cx += hidePixels*2;
			sizeWnd.cy += hidePixels*2;
		}

		SetWindowPos( NULL, -hidePixels, -hidePixels, 0,0,
			SWP_NOZORDER|SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOOWNERZORDER
			|SWP_NOCOPYBITS);
		pSkin->Resize( m_hWnd, &sizeWnd.cx, &sizeWnd.cy );
		RepositionMovie();
		InvalidateRect(NULL); UpdateWindow();
	}
	else if ( !maximize && bMaximized )
	{
		// Restore from maximize.
		sizeWnd.cx = rcMaximized.right - rcMaximized.left;
		sizeWnd.cy = rcMaximized.bottom - rcMaximized.top;

		SetWindowPos( NULL, rcMaximized.left, rcMaximized.top, 0,0,
			SWP_NOZORDER|SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOOWNERZORDER
			|SWP_NOCOPYBITS);
		pSkin->Resize( m_hWnd, &sizeWnd.cx, &sizeWnd.cy );
		RepositionMovie();
		InvalidateRect(NULL); UpdateWindow();
	}


	bMaximized = maximize;

	return TRUE;
}

BOOL CMainWnd::SetMode( int newModeId )
{
	if ( !pskinNormal || !pskinCompact || !pskinFullscreen )
		return FALSE;

	if ( newModeId == modeId )
		return FALSE;



	// If we are going to change window, it is possible that mouse
	//  is over button (button have state BSTATE_MOUSE_OVER ),
	//  so we must change button state to MOUSE_NORMAL (in the other
	//  button can stays in state MOUSE_OVER when we change to that window again ).
	if ( idButtonOver>=0 ) {
		if ( pSkin->SetButtonState(idButtonOver, BSTATE_MOUSE_NORMAL) )
			pSkin->DrawButton(m_hWnd, idButtonOver );
	}

	switch ( newModeId ) {

		case MODE_GO_FULLSCREEN:
			if ( modeId == MODE_FULLSCREEN ) return TRUE;

			newModeId = MODE_FULLSCREEN;
			break;

		case MODE_GO_WINDOWED:
			if ( modeId==MODE_NORMAL || modeId==MODE_COMPACT )
				return TRUE;

			newModeId = prevModeId;
			break;

		case MODE_FULLSCREEN_SWITCH:
			if ( modeId==MODE_FULLSCREEN ) newModeId = prevModeId;
			else newModeId = MODE_FULLSCREEN;

			break;

	}

	// If current mode is fullscreen, it mean we are going windowed,
	//  so restore windows state.
	if ( modeId&MODE_FULLSCREEN ) {

		ShowWindow( SW_HIDE );
		if ( pMovie && pMovie->IsWin9xCompatible() )
			pMovie->ShowMovie( FALSE );
		// Hide fullscreen window.
		pFullscreenWnd->ShowWindow(SW_HIDE);

		// Store current fs skin window pos.
		GetWindowRect( &rcStoredFsPos );

		// Restore skin window properties and show skin window.
		pSkin->SetAllowResize( bStoredFsAllowResize );
		//ShowWindow( SW_SHOW );

		if ( pMovie ) pMovie->SetMovieWindow( m_hWnd );

		switch ( prevModeId ) {
			case MODE_NORMAL: pSkin = pskinNormal; break;
			case MODE_COMPACT: pSkin = pskinCompact; break;
		}
		if ( !pSkin->IsDefined() ) pSkin = pskinNormal;

		prevModeId = MODE_FULLSCREEN;

		AllowHideMouse( FALSE );

		
		bMaximized = bStoredMaximized;
	}

	switch ( newModeId ) {

		case MODE_NORMAL:
			// Normal mode should be always defined.
			if ( !pskinNormal->IsDefined() ) break;

			if ( pSkin ) {

				RECT rc, rcNew; 
				
				rcNew = pSkin->GetEqualPlayAreas( pskinNormal );

				GetWindowRect( &rc );
				if ( pMovie && pMovie->IsWin9xCompatible() )
					pMovie->ShowMovie( FALSE );
				else if (pMovie) pMovie->PutMoviePosition(0,0,0,0);

				if ( bMaximized )
				{
					// Window is maximized so don't change window size at all.
					sizeWnd = sizeMaximized;
					if ( pOptions->bMaximizeOverTaskMenu ) sizeWnd = sizeScreen;

					int hidePixels = (sizeMaximized.cx-sizeScreen.cx)/2;
					if ( pOptions->bMaximizeOverTaskMenu ) {
						sizeWnd.cx += hidePixels*2;
						sizeWnd.cy += hidePixels*2;
					}

					SetWindowPos( NULL, -hidePixels, -hidePixels, 
						0,0, SWP_NOZORDER|SWP_SHOWWINDOW|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOSIZE);
				}
				else
				{
					if ( prevModeId==MODE_FULLSCREEN ) 
					{
						// We are going from fullscreen to windowed.

						SIZE size;
						RECT *prc; prc = &rcStoredWindowedPos;
						if ( !prc->right || !prc->bottom ) {
							// This is first time when going windowed
							//  ( run app in MODE_FULLSCREEN )
							
							if (pMovie) {

								long cx, cy;
								RECT pa = pskinNormal->GetPlayArea();
								pskinNormal->GetSkinSize( &cx, &cy );

								pMovie->GetNativeMovieSize( &size.cx, &size.cy );
								size.cx += cx-(pa.right-pa.left); 
								size.cy += cy-(pa.bottom-pa.top);
							}
							else {
								pskinNormal->GetSkinSize( &size.cx, &size.cy );
							}

							SetWindowPos( NULL, 
								(sizeScreen.cx-size.cx)/2, 
								(sizeScreen.cy-size.cy)/2,
								0,0, SWP_NOZORDER|SWP_SHOWWINDOW|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOSIZE);

							sizeWnd = size; 
						}
						else
						{
							RECT *prc; prc = &rcStoredWindowedPos;

							SetWindowPos( NULL, 
								prc->left+rcNew.left, prc->top+rcNew.top,
								0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOOWNERZORDER);

							sizeWnd.cx = rcNew.right-rcNew.left; 
							sizeWnd.cy = rcNew.bottom-rcNew.top;
						}


					}
					else
					{
						// We are going from windowed to windowed.
						if ( !rc.right || !rc.bottom ) {
							// This is first time when going windowed
							//  ( run app )
							
							SIZE size;
							if (pMovie) {

								long cx, cy;
								RECT pa = pskinNormal->GetPlayArea();
								pskinNormal->GetSkinSize( &cx, &cy );

								pMovie->GetNativeMovieSize( &size.cx, &size.cy );
								size.cx += cx-(pa.right-pa.left); 
								size.cy += cy-(pa.bottom-pa.top);
							}
							else {
								pskinNormal->GetSkinSize( &size.cx, &size.cy );
							}

							SetWindowPos( NULL, 
								(sizeScreen.cx-size.cx)/2, 
								(sizeScreen.cy-size.cy)/2,
								0,0, SWP_NOZORDER|SWP_SHOWWINDOW|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOSIZE);

							sizeWnd = size; 
						}
						else
						{

							SetWindowPos( NULL,	
								rc.left+rcNew.left, rc.top+rcNew.top,
								0, 0, SWP_NOSIZE|SWP_NOZORDER|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOOWNERZORDER);
				
								sizeWnd.cx = rcNew.right-rcNew.left; 
								sizeWnd.cy = rcNew.bottom-rcNew.top;
						}
					}
				}

			}

			pSkin = pskinNormal;
			modeId = MODE_NORMAL;
			prevModeId = MODE_NORMAL;

			UpdateButtons( FALSE ); UpdateStatus();
			pSkin->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );
			pSubtitles->SetFullscreen( FALSE );
			hWndMovie = m_hWnd; RepositionMovie();
			pSkin->DrawSkin( m_hWnd, (stateMovie&VCD_LOADED)?FALSE:TRUE );

			SetCursor( LoadCursor(NULL, IDC_ARROW) );

			break;

		case MODE_COMPACT:
			if ( !pskinCompact->IsDefined() ) break;

			if ( pSkin ) {

				RECT rc, rcNew; 
				
				rcNew = pSkin->GetEqualPlayAreas( pskinCompact );

				GetWindowRect( &rc );
				if ( pMovie && pMovie->IsWin9xCompatible() )
					pMovie->ShowMovie( FALSE );
				else if (pMovie) pMovie->PutMoviePosition(0,0,0,0);

				if ( bMaximized )
				{
					// Window is maximized so don't change window size at all.
					sizeWnd = sizeMaximized;
					if ( pOptions->bMaximizeOverTaskMenu ) sizeWnd = sizeScreen;

					int hidePixels = (sizeMaximized.cx-sizeScreen.cx)/2;
					if ( pOptions->bMaximizeOverTaskMenu ) {
						sizeWnd.cx += hidePixels*2;
						sizeWnd.cy += hidePixels*2;
					}

					SetWindowPos( NULL, -hidePixels, -hidePixels, 
						0,0, SWP_NOZORDER|SWP_SHOWWINDOW|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOSIZE);
				}
				else
				{
					if ( prevModeId==MODE_FULLSCREEN ) 
					{
						// We are going from fullscreen to windowed.

						SIZE size;
						RECT *prc; prc = &rcStoredWindowedPos;
						if ( !prc->right || !prc->bottom ) {
							// This is first time when going windowed
							//  ( run app in MODE_FULLSCREEN )
							
							if (pMovie) {

								long cx, cy;
								RECT pa = pskinCompact->GetPlayArea();
								pskinCompact->GetSkinSize( &cx, &cy );

								pMovie->GetNativeMovieSize( &size.cx, &size.cy );
								size.cx += cx-(pa.right-pa.left); 
								size.cy += cy-(pa.bottom-pa.top);
							}
							else {
								pskinCompact->GetSkinSize( &size.cx, &size.cy );
							}

							SetWindowPos( NULL, 
								(sizeScreen.cx-size.cx)/2, 
								(sizeScreen.cy-size.cy)/2,
								0,0, SWP_NOZORDER|SWP_SHOWWINDOW|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOSIZE);

							sizeWnd = size; 
						}
						else
						{
							RECT *prc; prc = &rcStoredWindowedPos;

							SetWindowPos( NULL, 
								prc->left+rcNew.left, prc->top+rcNew.top,
								0, 0, SWP_NOZORDER|SWP_NOSIZE|SWP_SHOWWINDOW|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOOWNERZORDER);

							sizeWnd.cx = rcNew.right-rcNew.left; 
							sizeWnd.cy = rcNew.bottom-rcNew.top;
						}


					}
					else
					{
						// We are going from windowed to windowed.
						if ( !rc.right || !rc.bottom ) {
							// This is first time when going windowed
							//  ( run app )
							
							SIZE size;
							if (pMovie) {

								long cx, cy;
								RECT pa = pskinCompact->GetPlayArea();
								pskinCompact->GetSkinSize( &cx, &cy );

								pMovie->GetNativeMovieSize( &size.cx, &size.cy );
								size.cx += cx-(pa.right-pa.left); 
								size.cy += cy-(pa.bottom-pa.top);
							}
							else {
								pskinCompact->GetSkinSize( &size.cx, &size.cy );
							}

							SetWindowPos( NULL, 
								(sizeScreen.cx-size.cx)/2, 
								(sizeScreen.cy-size.cy)/2,
								0,0, SWP_NOZORDER|SWP_SHOWWINDOW|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOSIZE);

							sizeWnd = size; 
						}
						else
						{

							SetWindowPos( NULL,	
								rc.left+rcNew.left, rc.top+rcNew.top,
								0, 0, SWP_NOSIZE|SWP_NOZORDER|
								SWP_NOSENDCHANGING|SWP_NOCOPYBITS|SWP_NOOWNERZORDER);
				
								sizeWnd.cx = rcNew.right-rcNew.left; 
								sizeWnd.cy = rcNew.bottom-rcNew.top;
						}
					}
				}

			}

			pSkin = pskinCompact;
			modeId = MODE_COMPACT;
			prevModeId = MODE_COMPACT;

			UpdateButtons( FALSE ); UpdateStatus();
			pSkin->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );
			pSubtitles->SetFullscreen( FALSE );
			hWndMovie = m_hWnd; RepositionMovie();
			pSkin->DrawSkin( m_hWnd, (stateMovie&VCD_LOADED)?FALSE:TRUE );

			SetCursor( LoadCursor(NULL, IDC_ARROW) );

			break;

		case MODE_FULLSCREEN:

			pFullscreenWnd->ShowWindow(SW_SHOW);

			pSkin = pskinFullscreen;
			modeId = MODE_FULLSCREEN;

			bStoredMaximized = bMaximized; bMaximized = FALSE;
			bStoredFsAllowResize = pSkin->AllowResize();
			GetWindowRect( &rcStoredWindowedPos );


			pSkin->SetPlayArea( 0,0, sizeScreen.cx, sizeScreen.cy );

			hWndMovie = pFullscreenWnd->m_hWnd; 

			ShowWindow( SW_HIDE );
			if ( pSkin->IsDefined() ) 
			{
				UpdateButtons( FALSE ); UpdateStatus();

				if ( bFsConstPos ) {

					if ( pSkin->AllowResize() ) {

						long w,h; pSkin->GetSkinSize( &w, &h);
						w = sizeScreen.cx;
						pSkin->Resize(m_hWnd, &w, &h, TRUE );

					} else {

						pSkin->Resize(m_hWnd, &sizeWnd.cx, 
							&sizeWnd.cy, TRUE );
					}

					// Resize change play area, so set it to fullscreen again.
					pSkin->SetPlayArea( 0,0, sizeScreen.cx, sizeScreen.cy );

					// Set skin window pos at bottom-left corner.
					long w,h; pSkin->GetSkinSize( &w, &h);
					SetWindowPos( NULL, 0, sizeScreen.cy-h, 0,0, 
						SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|
						SWP_NOCOPYBITS|SWP_NOOWNERZORDER);

					pSkin->SetAllowResize( FALSE );
					pSkin->SetAllowMove( FALSE );
				
				} else {

					if ( rcStoredFsPos.right && rcStoredFsPos.bottom )
					{
						sizeWnd.cx = rcStoredFsPos.right - rcStoredFsPos.left;
						sizeWnd.cy = rcStoredFsPos.bottom - rcStoredFsPos.top;

						pSkin->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );

						SetWindowPos( NULL, rcStoredFsPos.left,
							rcStoredFsPos.top, 0,0, 
							SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|
							SWP_NOCOPYBITS|SWP_NOOWNERZORDER);
					}

					else
					{
						// Center fullscreen skin window.
						long w,h; pSkin->GetSkinSize( &w, &h);
						sizeWnd.cx = w; sizeWnd.cy = h;
						pSkin->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );

						SetWindowPos( NULL, (sizeScreen.cx-w)/2,
							(sizeScreen.cy-h)/2, 0,0, 
							SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|
							SWP_NOCOPYBITS|SWP_NOOWNERZORDER);

					}
					
				}

				pSkin->DrawSkin( m_hWnd, FALSE );
				SetCursor( LoadCursor(NULL, IDC_ARROW) );

			}

			if ( pMovie ) pMovie->SetMovieWindow( hWndMovie );
			pSubtitles->SetFullscreen( TRUE );
			RepositionMovie();

			AllowHideMouse( TRUE );
			RECT rc; GetWindowRect( &rc ); POINT pt; GetCursorPos( &pt );
			if ( !PtInRect(&rc, pt) ) ShowMouse( FALSE );

			break;

		default: return FALSE;
	}

	return TRUE;
}


void CMainWnd::OnMouseMove(UINT nFlags, CPoint point) 
{

	// Fullscreen window set nFlags to '-1' and send message to this
	//  window when move occurs over fullscreen window.
	if ( (int)nFlags==-1 )
	{
		static POINT ptLast = {-1, -1 };

		if ( point.x!=ptLast.x || point.y!=ptLast.y )
			ShowMouse( TRUE );

		ptLast = point;

		// Show control window when move over.
		RECT rc; GetWindowRect( &rc );
		if ( PtInRect(&rc, point) ) {

			ShowMouse( TRUE );
			if (pSkin->IsDefined() ) ShowWindow( SW_SHOW );
		}

		// If we are here mouse is over fullscreen window, but if we
		//  are here first time idButtonOver may be valid button,
		//  so we must set idButtonOver to -1 ( tooltip uses idButtonOver ).
		if ( idButtonOver>=0 ) {

			BOOL ret = pSkin->SetButtonState(idButtonOver, BSTATE_MOUSE_NORMAL );
			if (ret) pSkin->DrawButton( m_hWnd, idButtonOver );
			idButtonOver = -1;
		}

		timeMouseMove = GetTickCount();
		return;
	}

	RECT rc;
	pressedLeft = (nFlags&MK_LBUTTON)?TRUE:FALSE;
	pressedMiddle = (nFlags&MK_MBUTTON)?TRUE:FALSE;
	pressedRight = (nFlags&MK_RBUTTON)?TRUE:FALSE;
	ptClient = point;
	

	timeMouseMove = GetTickCount();
	if ( pressedLeft && bMovingWindow && !bMaximized &&
		  (timeMouseMove-timeWindowReaction >= timeWindowDelay) ) 
	//if ( pressedLeft && bMovingWindow && !bMaximized )
	{

		int x = ptScreen.x, y = ptScreen.y;
		GetCursorPos( &ptScreen );
		
		if ( x!=ptScreen.x || y!=ptScreen.y )
		{
				
			// Call OnWindowMove() before setting new position (bMoved = FALSE).
			OnWindowMove( FALSE );

			//set new window position 
			GetWindowRect( &rc );
			x = rc.left+(ptScreen.x-x); y = rc.top+(ptScreen.y-y);
			SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);

			// Call OnWindowMove() after setting new position (bMoved = TRUE).
			OnWindowMove( TRUE );

		}

		timeWindowReaction = timeMouseMove;

	}

	if (idButtonPress == -1 && idScrollbarPress == -1)
	{

		int id = pSkin->GetButtonIdFromPoint(ptClient);
			
		if (id==idButtonOver && idButtonOver!= -1) //MOUSE_OVER
			OnButton( idButtonOver, MOUSE_OVER );

		else if (idButtonOver==-1 && id!=-1) //MOUSE_OVER_BEGIN
			OnButton( id, MOUSE_OVER_BEGIN );

		else if (idButtonOver!=-1 && id==-1) //MOUSE_OVER_END
			OnButton( idButtonOver, MOUSE_OVER_END );

		else if (idButtonOver!=-1 && id!=-1 && idButtonOver!=id) { 

			OnButton( id, MOUSE_OVER_BEGIN );

			OnButton( idButtonOver, MOUSE_OVER_END );
		}

		if ( idButtonOver == -1 ) {

			if (id != -1 ) {

				//mouse move from client area over button
				pSkin->SetButtonState(id, BSTATE_MOUSE_OVER);
				pSkin->DrawButton( m_hWnd, id );
				idButtonOver = id;

			} else {

				//mouse move over client area
				SetCursor( LoadCursor(NULL, IDC_ARROW) );
			}


		} else {

			if (id==-1) {
				//mouse moved from button over client area
		
				pSkin->SetButtonState(idButtonOver, BSTATE_MOUSE_NORMAL);
				pSkin->DrawButton( m_hWnd, idButtonOver );
				idButtonOver = id;

			} else if (id!=idButtonOver) {
				//mouse moved directly from one button over other button

				pSkin->SetButtonState(idButtonOver, BSTATE_MOUSE_NORMAL);
				pSkin->DrawButton( m_hWnd, idButtonOver );

				pSkin->SetButtonState( id, BSTATE_MOUSE_OVER );
				pSkin->DrawButton( m_hWnd, id );


				idButtonOver = id;

			}

		}


	} else {	// if ( idButtonPress != -1 )

		OnButton( idButtonPress, MOUSE_LBUTTON_MOVE);

	}

	if ( idScrollbarPress >= 0 ) {

		pSkin->OnScrollbarMove( idScrollbarPress, ptClient );
		pSkin->DrawScrollbar( m_hWnd, idScrollbarPress );

		OnScrollbar( idScrollbarPress, MOUSE_LBUTTON_MOVE );

	}

	
	CWnd::OnMouseMove(nFlags, point);
}


void CMainWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{

	pressedLeft = (nFlags&MK_LBUTTON)?TRUE:FALSE;
	pressedMiddle = (nFlags&MK_MBUTTON)?TRUE:FALSE;
	pressedRight = (nFlags&MK_RBUTTON)?TRUE:FALSE;
	ptClient = point;

	idButtonPress = pSkin->GetButtonIdFromPoint(ptClient);
	if ( idButtonPress == -1 )
		idScrollbarPress = pSkin->GetScrollbarIdFromPoint(ptClient);

	if ( idButtonPress != -1 ) {

		pSkin->SetButtonState( idButtonPress, BSTATE_MOUSE_PRESS );
		pSkin->DrawButton( m_hWnd, idButtonPress );

		SetCapture();

		OnButton( idButtonPress, MOUSE_LBUTTON_DOWN );		

	} else if ( idScrollbarPress != -1 ) {

		if ( pSkin->IsScrollbarEnabled( idScrollbarPress ) ) {

			pSkin->SetScrollbarState( idScrollbarPress, 
				SSTATE_DRAW_SLIDER|SSTATE_MOUSE_PRESS);
			pSkin->OnScrollbarMove( idScrollbarPress, ptClient );
			pSkin->DrawScrollbar( m_hWnd, idScrollbarPress );

			SetCapture();

			OnScrollbar( idScrollbarPress, MOUSE_LBUTTON_DOWN );


		} else {

			idScrollbarPress = -1;

		}

	} else if ( !bMovingWindow && pSkin->AllowMove()) {

		GetCursorPos( &ptScreen );
		SetCapture();

		bMovingWindow = TRUE;

	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CMainWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{

	pressedLeft = (nFlags&MK_LBUTTON)?TRUE:FALSE;
	pressedMiddle = (nFlags&MK_MBUTTON)?TRUE:FALSE;
	pressedRight = (nFlags&MK_RBUTTON)?TRUE:FALSE;
	ptClient = point;


	if ( bShowPlaylist && pOptions->bHidePLSOnLButtonUp && idButtonPress==-1 )
	{
		ShowPlaylist(FALSE);	
	}

	if ( idButtonPress >= 0 ) {
	
		int id = pSkin->GetButtonIdFromPoint(ptClient);

		if ( id==idButtonOver ) {

			int stateId = BSTATE_MOUSE_OVER;
			if (id==idButtonPress) stateId |= BSTATE_SWITCH;

			BOOL redraw = pSkin->SetButtonState( idButtonPress, stateId );

			if ( redraw ) pSkin->DrawButton( m_hWnd, idButtonPress );

		} else {

			int stateId = BSTATE_MOUSE_NORMAL;

			BOOL redraw = pSkin->SetButtonState( idButtonPress, stateId );

			if ( redraw ) pSkin->DrawButton( m_hWnd, idButtonPress );

		}

		ReleaseCapture();
		


		if ( id==idButtonPress ) {
			// release over button
			idButtonPress = -1;
			OnButton( id, MOUSE_LBUTTON_UP );
			
		}

		else {
			// release not over button
			id = idButtonPress;
			idButtonPress = -1;
			OnButton( id, MOUSE_LBUTTON_UP_NOT_OVER_BUTTON );

			// Propably we must update button here, because 
			//  button wasn't really pressed.
			UpdateButtons();
		}

				

	} else if ( idScrollbarPress >= 0 ) {

		BOOL redraw = pSkin->SetScrollbarState( idScrollbarPress, 
			SSTATE_DRAW_SLIDER|SSTATE_MOUSE_NORMAL );
		if ( redraw) pSkin->DrawScrollbar( m_hWnd, idScrollbarPress );
		ReleaseCapture();

		OnScrollbar( idScrollbarPress, MOUSE_LBUTTON_UP );

		idScrollbarPress = -1;

	} else if ( bMovingWindow ) {

		GetCursorPos( &ptScreen );
		ReleaseCapture();

		bMovingWindow = FALSE;

	}

	
	CWnd::OnLButtonUp(nFlags, point);
}

void CMainWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	LockMouseHide();

	BOOL	bLoaded = (stateMovie&VCD_LOADED)?TRUE:FALSE,
			bPlaying = stateMovie&VCD_PLAYING,
			bPaused = stateMovie&VCD_PAUSED,
			bStopped = stateMovie&VCD_STOPPED,
			bFullscreen = (modeId==MODE_FULLSCREEN),
			bPanScan = pOptions->bPanScanEnabled,
			bSubtitles = pOptions->bShowSubtitles,
			bPlaylist = bShowPlaylist;

	DWORD valPanScan = (DWORD)pOptions->valPanScan;


	CMenu *pQuickMenu = pMenu->GetSubMenu(0);

	DWORD dwFlags[] = {

		//T_QM_PLAYPAUSE
		MF_BYCOMMAND|(bLoaded|bPaused|bStopped)?MF_ENABLED:MF_GRAYED,
		//T_QM_STOP
		MF_BYCOMMAND|bLoaded?MF_ENABLED:MF_GRAYED,
		//T_QM_REWINDBACK
		MF_BYCOMMAND|bLoaded?MF_ENABLED:MF_GRAYED,
		//T_QM_REWINDFORWARD
		MF_BYCOMMAND|bLoaded?MF_ENABLED:MF_GRAYED,
		//T_QM_PREVMOVIE
		MF_BYCOMMAND|(pPlaylist->IsPrev()?MF_ENABLED:MF_GRAYED),
		//T_QM_NEXTMOVIE
		MF_BYCOMMAND|(pPlaylist->IsNext()?MF_ENABLED:MF_GRAYED),
		//T_QM_PLAYLIST
		MF_BYCOMMAND|MF_ENABLED|bShowPlaylist?MF_CHECKED:MF_UNCHECKED,
		//T_QM_FULLSCREEN,
		MF_BYCOMMAND|MF_ENABLED|bFullscreen?MF_CHECKED:MF_UNCHECKED,
		//T_QM_PANSCAN
		MF_BYCOMMAND|(bPanScan?MF_CHECKED:MF_UNCHECKED)|(bLoaded?MF_ENABLED:MF_GRAYED),
		//T_QM_PANSCANMODE,
		MF_BYPOSITION|((bPanScan&bLoaded)?MF_ENABLED:MF_GRAYED),
		//T_QM_PS_AUTO,
		MF_BYCOMMAND|((valPanScan==0)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_PS_WIDE
		MF_BYCOMMAND|((valPanScan==-169)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_PS_ZOOM10,
		MF_BYCOMMAND|((valPanScan==10)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_PS_ZOOM20
		MF_BYCOMMAND|((valPanScan==20)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_PS_ZOOM25
		MF_BYCOMMAND|((valPanScan==25)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_PS_ZOOM30
		MF_BYCOMMAND|((valPanScan==30)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_PS_ZOOM35
		MF_BYCOMMAND|((valPanScan==35)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_PS_ZOOM40,
		MF_BYCOMMAND|((valPanScan==40)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_PS_ZOOM50,
		MF_BYCOMMAND|((valPanScan==50)?MF_CHECKED:MF_UNCHECKED),
		//T_QM_SHOWSUBTITLES
		MF_BYCOMMAND|(bSubtitles?MF_CHECKED:MF_UNCHECKED)|(bLoaded?MF_ENABLED:MF_GRAYED),
		//T_QM_OPENMOVIE
		MF_BYCOMMAND|MF_ENABLED,
		//T_QM_OPENSUBTITLES
		MF_BYCOMMAND|MF_ENABLED,
		//T_QM_OPTIONS
		MF_BYCOMMAND|MF_ENABLED,
		//T_QM_FILTERSPROP
		MF_BYCOMMAND|(bLoaded?MF_ENABLED:MF_GRAYED),
		//T_QM_SKINBROWSER
		MF_BYCOMMAND|MF_ENABLED,
		//T_QM_EXIT
		MF_BYCOMMAND|MF_ENABLED

	};

	DWORD nPosition[] = {
		IDM_PLAYPAUSE, IDM_STOP, IDM_REWINDBACK, IDM_REWINDFORWARD,
		IDM_PREVMOVIE, IDM_NEXTMOVIE,
		IDM_PLAYLIST, IDM_FULLSCREEN,
		IDM_PANSCAN, 11,//IDM_PANSCANMODE,
		IDM_PS_AUTO, IDM_PS_WIDE,
		IDM_PS_ZOOM10, IDM_PS_ZOOM20, IDM_PS_ZOOM25,
		IDM_PS_ZOOM30, IDM_PS_ZOOM35, IDM_PS_ZOOM40, IDM_PS_ZOOM50,
		IDM_SHOWSUBTITLES,
		IDM_OPENMOVIE, IDM_OPENSUBTITLES,
		IDM_OPTIONS, IDM_FILTERSPROP, IDM_SKINBROWSER,
		IDM_EXIT
	};


	long shortcut[][2] = {
		KEY_PLAYPAUSE, 0,
		KEY_STOP, 0,
		KEY_REWINDBACK, 0,
		KEY_REWINDFORWARD, 0,
		KEY_PREVMOVIE, 0,
		KEY_NEXTMOVIE, 0,
		KEY_PLAYLIST, 0,
		KEY_GOFULLSCREEN, KEY_GOWINDOWED,
		KEY_PANSCAN, 0,
		0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0, 0,0,
		KEY_SUBTITLES, KEY_SUBTITLES,
		KEY_OPENMOVIE, 0, 
		KEY_OPENSUBTITLES, 0,
		KEY_OPTIONS,  0,
		KEY_MOVIEPROP,  0,
		0, 0,
		KEY_EXIT, 0,
	};

	int count = sizeof(dwFlags)/sizeof(DWORD);
	for (int i=0; i<count; i++)
	{
		DWORD f = dwFlags[i];
		char sz[256], 
			*psz = transl( nPosition[i], TRANSL_QMENU ),
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

		if ( pszkey )
			sprintf( sz+strlen(sz), "\t%s", pszkey );

		pQuickMenu->ModifyMenu( nPosition[i], f, nPosition[i],  sz );
	}

	POINT pt; GetCursorPos( &pt );
	pQuickMenu->TrackPopupMenu( TPM_LEFTALIGN|TPM_RIGHTBUTTON,
		pt.x, pt.y, this, NULL );

	bQuickMenu = TRUE;
	
	CWnd::OnRButtonUp(nFlags, point);
}


void CMainWnd::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	if ( idButtonPress != -1 ) {

		if ( pSkin->SetButtonState( idButtonPress, BSTATE_MOUSE_NORMAL ) )
			pSkin->DrawButton( m_hWnd, idButtonPress );

		ReleaseCapture();
			
		idButtonPress = -1;
	}

	if ( idButtonOver != -1 &&  !pTooltip ) {

		if ( pSkin->SetButtonState( idButtonOver, BSTATE_MOUSE_NORMAL ) )
			pSkin->DrawButton( m_hWnd, idButtonOver );
			
		idButtonOver = -1;
	}

	if ( idScrollbarPress != -1 ) {

		pSkin->SetScrollbarState( idScrollbarPress, 
			SSTATE_DRAW_SLIDER|SSTATE_MOUSE_NORMAL );
		pSkin->DrawScrollbar( m_hWnd, idScrollbarPress );

		ReleaseCapture();

		idScrollbarPress = -1;
	}

	if ( nState==WA_INACTIVE )
	{
		if ( modeId==MODE_FULLSCREEN && !pOptions->bAlwaysOnTop )
			SetWindowPos( &wndNoTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE );
	}
	else
   {
      // we are being activated - tell the Filter graph (for Sound follows focus)
      if(pMovie)
      {
          pMovie->SetFocus();
      }

		if ( modeId & MODE_FULLSCREEN )
		{
			//pFullscreenWnd->SetWindowPos(NULL, 0,0,0,0, 
			//	SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
			//::ShowWindow( pFullscreenWnd->m_hWnd, SW_SHOW );
		}
		
		//if ( modeId==MODE_FULLSCREEN )
		//	SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE );
  }

	CWnd::OnActivate( nState, pWndOther, bMinimized );
}


//-------------------------------------------------------------------
//									OnButton() 
//
// Called on any action with mause and button, add here call
//  to your buttons functions on mouse events.
//-------------------------------------------------------------------
void CMainWnd::OnButton( int id, int mouseEventId )
{
	if ( !pSkin->IsButtonEnabled(id) )
		return;

	switch ( mouseEventId ) {

		case MOUSE_OVER:
			switch ( id ) {
				case BW_RESIZE: OnWindowResize( MOUSE_OVER ); break;
			}
			break;

		case MOUSE_OVER_BEGIN:
			switch ( id ) {
				case BW_RESIZE: OnWindowResize( MOUSE_OVER_BEGIN ); break;
			}
			break;

		case MOUSE_OVER_END:
			switch ( id ) {
				case BW_RESIZE: OnWindowResize( MOUSE_OVER_END ); break;
			}
			break;

		case MOUSE_LBUTTON_MOVE:
			switch ( id ) {
				case BW_RESIZE: OnWindowResize( MOUSE_LBUTTON_MOVE ); break;
			}
			break;

		case MOUSE_LBUTTON_DOWN:
			switch ( id ) {
				case BW_RESIZE: OnWindowResize( MOUSE_LBUTTON_DOWN ); break;
			}
			break;

		case MOUSE_LBUTTON_UP:

			switch ( id ) {
				case BW_MINIMIZE: OnWindowMinimize(); break;
				case BW_MAXIMIZE: OnWindowMaximize(); break;
				case BW_CLOSE: OnWindowClose(); break;
				case BW_RESIZE: OnWindowResize( MOUSE_LBUTTON_UP ); break;
				case BP_OPEN: OnPlayerOpen(); break;
				case BP_OPEN_SUBTITLES: OnPlayerOpenSubtitles(); break;
				case BP_PLAY: OnPlayerPlay(); break;
				case BP_PAUSE: OnPlayerPause(); break;
				case BP_PLAY_PAUSE: OnPlayerPlayPause(); break;
				case BP_STOP: OnPlayerStop(); break;
				case BP_REWIND_BACK: OnPlayerRewindBack(); break;
				case BP_REWIND_FORWARD: OnPlayerRewindForward(); break;
				case BP_FULLSCREEN_WINDOWED: OnPlayerFullscreenWindowed(); break;
				case BP_PREV_MOVIE: OnPlayerPrev(); break;
				case BP_NEXT_MOVIE: OnPlayerNext(); break;
				case BP_VOLUME_ON_OFF: OnPlayerVolumeOnOff(); break;

				case BP_PLAYLIST_SHOW_HIDE: OnPlayerPlaylistSwitch(); break;
				case BP_PANSCAN_ON_OFF: OnPlayerPanScanSwitch(); break;
				case BP_SUBTITLES_ON_OFF: OnPlayerSubtitlesSwitch(); break;
				case BP_MODE_NORMAL_COMPACT: OnPlayerModeNormalCompact(); break; 

				case BP_OPTIONS: OnPlayerOptions(); break;

			}
			break;

		//call button function even if release not over button
		case MOUSE_LBUTTON_UP_NOT_OVER_BUTTON:
			switch ( id ) {
				case BW_RESIZE: OnWindowResize( MOUSE_LBUTTON_UP ); break;
			}
			break;

	}

}

//-------------------------------------------------------------------
//									OnScrollbar() 
//
// Called on any action with mause and scrollbar, add here call
//  to your scrollbars functions on mouse events.
//-------------------------------------------------------------------
void CMainWnd::OnScrollbar( int id, int mouseEventId )
{
	if ( !pSkin->IsScrollbarEnabled(id) )
		return;

	switch ( mouseEventId ) {

		case MOUSE_LBUTTON_MOVE:
			switch (id) {
				case SP_POSITION: OnPlayerPosition( MOUSE_LBUTTON_MOVE ); break;
				case SP_VOLUME: OnPlayerVolume(); break;
			}
			break;

		case MOUSE_LBUTTON_DOWN:
			switch (id) {
				case SP_POSITION: OnPlayerPosition( MOUSE_LBUTTON_DOWN ); break;
				case SP_VOLUME: OnPlayerVolume(); break;
			}
			break;

		case MOUSE_LBUTTON_UP:
			switch (id) {
				case SP_POSITION: OnPlayerPosition( MOUSE_LBUTTON_UP ); break;
			}
			break;
	}
}


// This function is called twice when window is moved.
// First when window is about to move (bMoved = FALSE),
//  second when window was mowed (bMoved = TRUE).
void CMainWnd::OnWindowMove( BOOL bMoved )
{
	if( pMovie && !bMoved && !bMaximized )
   {
		if(pMovie->GetStateMovie() != State_Running)
      {
          RepositionMovie();
      }
		else
		{
		
			RECT *prc = &pSkin->rcPlayArea;
			long xPos = prc->left, yPos = prc->top, 
				 cx = prc->right - xPos, cy = prc->bottom - yPos;

          // Reposition movie but don't invalidate the rect, since
          // the next video frame will handle the redraw.
          pMovie->PutMoviePosition(xPos, yPos, cx, cy);
      }
		
	}
	else
	{
		RECT rc = pSkin->GetPlayArea( );
		//ClientToScreen( &rc );
		
		if ( pSubtitles ) pSubtitles->Reposition(rc.left, rc.top,
									RECTW(rc), RECTH(rc) );

	}

}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//
//	My defined buttons and scrollbars functions, call to it from
//	 OnButton() and OnScrollbar().
//
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

void CMainWnd::OnWindowMinimize()
{
	pFullscreenWnd->ShowWindow(SW_MINIMIZE);
}

void CMainWnd::OnWindowMaximize()
{
	Maximize( !bMaximized );
	UpdateButtons();
}

void CMainWnd::OnWindowResize(int mouseEventId )
{
	static POINT ptPos = {0,0};
	static DWORD Width, Height;

	POINT pt;

	if ( bMaximized ) return;

	switch ( mouseEventId ) {

		case MOUSE_OVER_BEGIN:
			SetCursor( LoadCursor( NULL, IDC_SIZENWSE ) );
			break;
		case MOUSE_OVER_END:
			SetCursor( LoadCursor( NULL, IDC_ARROW ) );
			break;
		case MOUSE_LBUTTON_DOWN:
			pt = ptScreen;
			GetCursorPos( &ptPos );
			Width = sizeWnd.cx; Height = sizeWnd.cy;
			break;

		case MOUSE_LBUTTON_MOVE:
			
			int time; time = GetTickCount();
			if ( time-timeWindowReaction < timeWindowDelay ) break;
			timeWindowReaction = time;

			GetCursorPos( &pt );

			long w, h; w = sizeWnd.cx; h = sizeWnd.cy;
			sizeWnd.cx = Width + pt.x-ptPos.x;
			sizeWnd.cy = Height + pt.y-ptPos.y;

			pSkin->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy);
			pSkin->DrawSkin( m_hWnd, (stateMovie&VCD_LOADED)?FALSE:TRUE );
			RepositionMovie();

			break;
	}

}

void CMainWnd::OnWindowClose()
{
	if ( pMovie ) CloseMovie();
	KillTimer( timerId );

	pOptions->modeId = modeId;
	pOptions->prevModeId = prevModeId;

	PostQuitMessage(0);
}

void CMainWnd::OnPlayerPosition( int  mouseEventId )
{
	if ( pMovie )
	{
		static BOOL bPlayed = FALSE;

		switch ( mouseEventId ) {

			case MOUSE_LBUTTON_DOWN:
				bPlayed = stateMovie&VCD_PLAYING;
				pSkin->GetScrollbarValue( SP_POSITION, &dMoviePosition);
				if ( bPlayed ) OnPlayerPause();

			case MOUSE_LBUTTON_MOVE:
				double duration , curr; 
				duration = pMovie->GetDuration();
				if ( pSkin->GetScrollbarValue( SP_POSITION, &curr ) )
				{
					dMoviePosition = curr;
					pMovie->SeekToPosition( duration*curr/100.0, TRUE, 
													TRUE );
					UpdateStatus( FALSE );
				}
				
				break;

			case MOUSE_LBUTTON_UP:
				if ( bPlayed ) OnPlayerPlay();
				bPlayed = FALSE;
				break;
		}

	}

}

void CMainWnd::OnPlayerVolume()
{
	 pSkin->GetScrollbarValue( SP_VOLUME, &pOptions->dVolume );
	 if ( pMovie ) {
		 pMovie->SetVolume( pOptions->dVolume );
	 }
}

void CMainWnd::SetVolume( double vol, int wheelDelta )
{
	if (wheelDelta>0) vol = pOptions->dVolume+10.0;
	if (wheelDelta<0) vol = pOptions->dVolume-10.0;

	if (vol<0) vol = 0.0;
	if (vol>100) vol = 100.0;

	pOptions->dVolume = vol;
	if ( pSkin->SetScrollbarValue( SP_VOLUME, pOptions->dVolume ) )
		pSkin->DrawScrollbar( m_hWnd, SP_VOLUME );

	if ( pMovie ) {
		pMovie->SetVolume( pOptions->dVolume );
	}

}

void CMainWnd::OnPlayerVolumeOnOff()
{
	

	if ( pOptions->bMuteVolume ) {

		if ( pMovie ) pMovie->SetMute( FALSE, pOptions->dVolume );
		pOptions->bMuteVolume = FALSE;
		
	} else {

		if ( pMovie ) pMovie->SetMute( TRUE, pOptions->dVolume );
		pOptions->bMuteVolume = TRUE;

	}

	UpdateButtons();

}

void CMainWnd::OnPlayerOpen()
{
	static BOOL fFirstTime = TRUE;
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
	};

	// Convert the resource string into to something suitable for
	// GetOpenFileName ie.  replace '#' characters with '\0' characters.
	for(char *p = szFilter; *p; p++)
		if(*p == '|') *p = '\0' ;

	if(fFirstTime)
	{
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hWndMovie;//pFullscreenWnd->m_hWnd;
		ofn.Flags = OFN_FILEMUSTEXIST |OFN_ALLOWMULTISELECT|OFN_EXPLORER |
			OFN_SHAREAWARE | OFN_PATHMUSTEXIST;
	}
	ofn.lpstrInitialDir = pOptions->szLastMoviePath;
	ofn.lpstrFilter = szFilter;      

	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ZeroMemory(szFileName, sizeof(szFileName));

	LockMouseHide();

	if( GetOpenFileName(&ofn) )
	{
		fFirstTime = FALSE;

		if ( modeId&MODE_FULLSCREEN ) {
			//pFullscreenWnd->InvalidateRect(NULL,FALSE);
			pFullscreenWnd->UpdateWindow();
		} else {
			//InvalidateRect(NULL,FALSE);
			UpdateWindow();
		}

		char *pszDir = szFileName, 
			  *pszFileName = (char*)pszDir + ofn.nFileOffset;

		strcpy( pOptions->szLastMoviePath, pszDir );
 		pPlaylist->GoToLast();


		BOOL bAdded = FALSE;
		if ( *(pszFileName-1) ) {
			// Single file selected.
			//OpenMovie(szFileName, pOptions->bAutoPlay );
			pPlaylist->Add( szFileName, TRUE );
			bAdded = TRUE;

		} else {
			// Multiple files selected.

			while( *pszFileName ) {
				char sz[_MAX_PATH];
				sprintf(sz,"%s\\%s", pszDir, pszFileName);
				pszFileName += strlen(pszFileName)+1;

				pPlaylist->Add( sz, !bAdded );
				bAdded = TRUE;
			}
		}
		if ( bAdded )
			OpenMovie( (char*)pPlaylist->Curr(), pOptions->bAutoPlay );

	}

	UnlockMouseHide();

}

void CMainWnd::OnPlayerClose()
{
	CloseMovie();
	UpdateButtons();
}

void CMainWnd::OnPlayerPlay()
{
	PlayMovie();
	UpdateButtons();
	UpdateStatus( TRUE, FALSE );
}


void CMainWnd::OnPlayerPause()
{
	PauseMovie();
	UpdateButtons();
	UpdateStatus( TRUE, FALSE );
}

void CMainWnd::OnPlayerPlayPause()
{
	BOOL bPlaying = stateMovie & VCD_PLAYING;

	if ( bPlaying ) PauseMovie();
	else PlayMovie();

	UpdateButtons();
	UpdateStatus( TRUE, FALSE );
}


void CMainWnd::OnPlayerStop()
{
	StopMovie();
	UpdateButtons();
	UpdateStatus();
}

void CMainWnd::OnPlayerRewindBack()
{
	if ( pMovie ) {

		double duration = pMovie->GetDuration(), 
				curr = pMovie->GetCurrentPosition(), currorg = curr;

		curr -= pOptions->dRewindSecs;
		if ( curr<0 ) curr = 0;

		if (curr == currorg ) return;

		dMoviePosition = 100.0/duration*curr;

		pMovie->SeekToPosition( curr, TRUE, FALSE );
		if ( pSkin->SetScrollbarValue( SP_POSITION, dMoviePosition ) )
			pSkin->DrawScrollbar( m_hWnd, SP_POSITION );

		UpdateStatus( FALSE );
	}
}

void CMainWnd::OnPlayerRewindForward()
{
	if ( pMovie ) {

		double duration = pMovie->GetDuration(), 
				curr = pMovie->GetCurrentPosition(), currorg = curr;

		curr += pOptions->dRewindSecs;
		if ( curr>duration ) curr = duration;

		if (curr == currorg ) return;

		dMoviePosition = 100.0/duration*curr;

		pMovie->SeekToPosition( curr, TRUE, FALSE );
		if ( pSkin->SetScrollbarValue( SP_POSITION, dMoviePosition) )
			pSkin->DrawScrollbar( m_hWnd, SP_POSITION );

		UpdateStatus( FALSE );
	}
}

void CMainWnd::OnPlayerFullscreenWindowed()
{
	SetMode( MODE_FULLSCREEN_SWITCH );
}

void CMainWnd::OnPlayerNext()
{
	if ( pPlaylist->IsNext() ) {
		OpenMovie( (char*)pPlaylist->Next(), pOptions->bAutoPlay );
	} 

}

void CMainWnd::OnPlayerPrev()
{
	if ( pPlaylist->IsPrev() ) {
		OpenMovie( (char*)pPlaylist->Prev(), pOptions->bAutoPlay );
	} 

}


void	CMainWnd::OnPlayerSubtitlesSwitch()
{
	pOptions->bShowSubtitles = !pOptions->bShowSubtitles ;
	pSubtitles->EnableSubtitles( pOptions->bShowSubtitles );
	UpdateButtons();
}


void	CMainWnd::OnPlayerPlaylistSwitch()
{
	ShowPlaylist( !bShowPlaylist );
}

void CMainWnd::ShowPlaylist(BOOL bShow)
{
	bShowPlaylist = bShow;
	pPlaylist->ShowWindow(bShowPlaylist?SW_SHOW:SW_HIDE);
	if ( modeId==MODE_FULLSCREEN && bShowPlaylist ) ShowMouse(TRUE);
	UpdateButtons();
}

void	CMainWnd::OnPlayerPanScanSwitch()
{
	pOptions->bPanScanEnabled = !pOptions->bPanScanEnabled;
	if ( pMovie ) {
		pMovie->EnablePanScan( pOptions->bPanScanEnabled );
		RepositionMovie();
	}
	UpdateButtons();
}

void	CMainWnd::OnPlayerPanScanMode(double val)
{
	if ( pMovie ) {
		pOptions->valPanScan = val;
		pMovie->SetPanScanValue( val );
		RepositionMovie();
	}
	UpdateButtons();
}

void CMainWnd::OnPlayerModeNormalCompact()
{
	if ( modeId==MODE_NORMAL ) SetMode( MODE_COMPACT );
	else if ( modeId==MODE_COMPACT ) SetMode( MODE_NORMAL );
}

void CMainWnd::OnPlayerOptions(int idd)
{
	if ( !pDlgOptions->IsWindowVisible() )
	{
		// Center options dialog on current movie window.

		RECT rcSrc, rcDest;
		if ( modeId==MODE_FULLSCREEN ) 
			pFullscreenWnd->GetWindowRect( &rcSrc );
		else
			GetWindowRect( &rcSrc );
		pDlgOptions->GetWindowRect( &rcDest );

		int x = (RECTW(rcSrc)-RECTW(rcDest))/2,
			 y = (RECTH(rcSrc)-RECTH(rcDest))/2;
		pDlgOptions->SetWindowPos( NULL, rcSrc.left+x, rcSrc.top+y,
			0,0, SWP_NOZORDER|SWP_NOSIZE );
	}

	pDlgOptions->ShowOptions( idd );
}

//-------------------------------------------------------------------
//-------------------- UpdateButtons() ------------------------------
//-------------------------------------------------------------------
void CMainWnd::UpdateButtons( BOOL bRedraw, BOOL bForceRedrawAll )
{

	if ( !pSkin->IsDefined() ) return;

	int buttonId[] = {
		BSTATE_ENABLED,									//BW_CLOSE
		BSTATE_ENABLED,									//BW_MINIMIZE
		BSTATE_ENABLED|BSTATE_ALLOW_SWITCH|
		(bMaximized?BSTATE_DRAW_SECOND:BSTATE_DRAW_FIRST),			//BW_MAXIMIZE,
		BSTATE_ENABLED,									//BW_SYSMENU,
		BSTATE_ENABLED,									//BW_RESIZE,
		BSTATE_ENABLED,									//BW_CHANGE_SKIN,
		BSTATE_ENABLED,									//BW_ABOUT,

		BSTATE_DISABLED,									//BP_PLAY,
		BSTATE_DISABLED,									//BP_PAUSE,
		BSTATE_DISABLED|BSTATE_ALLOW_SWITCH,		//BP_PLAY_PAUSE,
		BSTATE_DISABLED,									//BP_REWIND_FORWARD,
		BSTATE_DISABLED,									//BP_REWIND_BACK,
		BSTATE_DISABLED,									//BP_STOP,

		pPlaylist->IsNext()?BSTATE_ENABLED:BSTATE_DISABLED,	//BP_NEXT_MOVIE,
		pPlaylist->IsPrev()?BSTATE_ENABLED:BSTATE_DISABLED,	//BP_PREV_MOVIE,

		BSTATE_ENABLED,									//BP_OPEN,
		BSTATE_DISABLED,									//BP_OPEN_SUBTITLES,
		BSTATE_DISABLED,									//BP_SUBTITLES_ON_OFF,
		BSTATE_DISABLED,									//BP_SUBTITLES_DELAY,
		BSTATE_ENABLED|BSTATE_ALLOW_SWITCH,			//BP_FULLSCREEN_WINDOWED,
		BSTATE_ENABLED|BSTATE_ALLOW_SWITCH,			//BP_MODE_NORMAL_COMPACT,
		BSTATE_DISABLED|BSTATE_ALLOW_SWITCH,									//BP_PANSCAN,
		BSTATE_ENABLED,									//BP_PANSCAN_MENU,
		BSTATE_ENABLED|BSTATE_ALLOW_SWITCH|(pOptions->bMuteVolume?BSTATE_DRAW_SECOND:BSTATE_DRAW_FIRST),			//BP_VOLUME_ON_OFF,
		BSTATE_ENABLED|BSTATE_ALLOW_SWITCH|
		(bShowPlaylist?BSTATE_DRAW_SECOND:BSTATE_DRAW_FIRST),									//BP_PLAYLIST_SHOW_HIDE
		BSTATE_ENABLED										//BP_OPTIONS
	};

	int scrollbarId[] = {
		SSTATE_DISABLED,									//SP_POSITION = 0,
		SSTATE_ENABLED										//SP_VOLUME	

	};
	double scrollbarVal[] = {
		0,													//SP_POSITION = 0,
		pOptions->dVolume								//SP_VOLUME	
	};

	BOOL	bMovieLoaded = stateMovie&VCD_LOADED,
			bMoviePlaying = stateMovie&VCD_PLAYING,
			bMoviePaused = stateMovie&VCD_PAUSED,
			bMovieStopped = stateMovie&VCD_STOPPED;

	if ( bMovieLoaded ) {
		buttonId[ BP_PLAY ] = BSTATE_ENABLED;
		buttonId[ BP_PLAY_PAUSE ] = BSTATE_ENABLED;
		buttonId[ BP_STOP ] = BSTATE_ENABLED;
		buttonId[ BP_REWIND_FORWARD ] = BSTATE_ENABLED;
		buttonId[ BP_REWIND_BACK ] = BSTATE_ENABLED;
		buttonId[ BP_OPEN_SUBTITLES ] = BSTATE_ENABLED;

		buttonId[ BP_PANSCAN_ON_OFF ] = BSTATE_ENABLED|BSTATE_ALLOW_SWITCH|
			(pOptions->bPanScanEnabled?BSTATE_DRAW_SECOND:BSTATE_DRAW_FIRST);

		buttonId[ BP_SUBTITLES_ON_OFF ] = BSTATE_ENABLED|BSTATE_ALLOW_SWITCH|
			(pOptions->bShowSubtitles?BSTATE_DRAW_SECOND:BSTATE_DRAW_FIRST);

		scrollbarId[ SP_POSITION ] = SSTATE_ENABLED;
		scrollbarVal[ SP_POSITION ] = dMoviePosition;
	}

	if ( bMoviePlaying ) {
		buttonId[ BP_PAUSE ] = BSTATE_ENABLED;
		buttonId[ BP_PLAY_PAUSE ] = BSTATE_ENABLED|BSTATE_DRAW_SECOND;
		buttonId[ BP_REWIND_FORWARD ] = BSTATE_ENABLED;
		buttonId[ BP_REWIND_BACK ] = BSTATE_ENABLED;
	}

	if ( bMoviePaused ) {
		buttonId[ BP_PLAY ] = BSTATE_ENABLED;
		buttonId[ BP_PLAY_PAUSE ] = BSTATE_ENABLED|BSTATE_DRAW_FIRST;
		buttonId[ BP_REWIND_FORWARD ] = BSTATE_ENABLED;
		buttonId[ BP_REWIND_BACK ] = BSTATE_ENABLED;
	}

	if ( bMovieStopped ) {
		buttonId[ BP_PLAY ] = BSTATE_ENABLED;
		buttonId[ BP_PLAY_PAUSE ] = BSTATE_ENABLED|BSTATE_DRAW_FIRST;
		buttonId[ BP_REWIND_FORWARD ] = BSTATE_ENABLED;
		buttonId[ BP_REWIND_BACK ] = BSTATE_ENABLED;
	}

	if ( pOptions->bMuteVolume ) {
		buttonId[ BP_VOLUME_ON_OFF ] = BSTATE_ENABLED|BSTATE_DRAW_SECOND;
	}

	if ( modeId==MODE_NORMAL || modeId==MODE_COMPACT ) {
		//modeId==MODE_WINDOWED

		buttonId[ BP_FULLSCREEN_WINDOWED ] &= ~BSTATE_DRAW_MASK;
		buttonId[ BP_FULLSCREEN_WINDOWED ] |= BSTATE_DRAW_FIRST;

	} else {
		//modeId==MODE_FULLSCREEN

		buttonId[ BW_MAXIMIZE ] = BSTATE_DISABLED;
		buttonId[ BP_FULLSCREEN_WINDOWED ] &= ~BSTATE_DRAW_MASK;
		buttonId[ BP_FULLSCREEN_WINDOWED ] |= BSTATE_DRAW_SECOND;
	}
	
	for (int i=0; i<pSkin->countButtons; i++) {

		BOOL redraw = pSkin->SetButtonState( i,  buttonId[i] );
		if ( redraw && bRedraw ) pSkin->DrawButton(m_hWnd, i);
	}

	int *pId = scrollbarId;
	double *pVal = scrollbarVal;
	for (i=0; i<pSkin->countScrollbars; i++) {

		BOOL redraw = pSkin->SetScrollbarState( i, *pId );
		if ( *pId&SSTATE_ENABLED)
			redraw |= pSkin->SetScrollbarValue( i, *pVal );

		if ( (redraw && bRedraw) || bForceRedrawAll ) 
			pSkin->DrawScrollbar( m_hWnd, i );

		pId++; pVal++;
	}

}

//-------------------------------------------------------------------
//--------------------- Movie functions -----------------------------
//-------------------------------------------------------------------

void CMainWnd::OpenMovie( TCHAR *achFileName, BOOL bPlay )
{
    // If we currently have a video loaded we need to discard it here.
    if(stateMovie & VCD_LOADED)
    {
        CloseMovie();
    }

    pMovie = new CMpegMovie(hWndMovie);
    if(pMovie && achFileName)
    {
			char sz[256], fname[_MAX_FNAME], fext[_MAX_EXT];
			_splitpath( achFileName, NULL, NULL, fname, fext );
			//sprintf(sz, "%s  '%s.%s'...", szText[ T_OPENING ], fname, fext );
			sprintf(sz, "%s  '%s.%s'...", transl(VCD_OPENING, TRANSL_MOVIESTATE), fname, fext );
			pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, sz );

        HRESULT hr = pMovie->OpenMovie(achFileName, FALSE);
        if(SUCCEEDED(hr))
        {

			  if ( modeId==MODE_FULLSCREEN || bMaximized )
			  {
			  }
			  else
			  {
				  long nw, nh;
				  pMovie->GetNativeMovieSize(&nw, &nh);
				  pSkin->ResizePlayArea(m_hWnd, (int*)&nw, (int*)&nh );
				  pSkin->DrawSkin( m_hWnd, TRUE);

				  sizeWnd.cx = nw; sizeWnd.cy = nh;
			  }


            stateMovie = (VCD_LOADED | VCD_STOPPED);

				pMovie->SetNotifyWindow( m_hWnd, WM_GRAPHNOTIFY );
				pMovie->SetMute( pOptions->bMuteVolume );
				pMovie->SetVolume( pOptions->dVolume );
				pMovie->EnablePanScan( pOptions->bPanScanEnabled );
				pMovie->SetPanScanValue( pOptions->valPanScan );
				pMovie->SetColorControl( pOptions->valBrightness, 
					pOptions->valContrast, pOptions->valSaturation );

				pMovie->SetTimeFormat(TIME_FORMAT_MEDIA_TIME);

				if (pSubtitles) pSubtitles->SetMovie( pMovie );
				OpenSubtitles( achFileName );
            RepositionMovie();
            pMovie->SetBorderClr(RGB(0x00, 0x00, 0x00));

				pFullscreenWnd->UpdateBackground( FALSE );


            if(bPlay) PlayMovie();


        }
        else
        {
            TCHAR Buffer[MAX_ERROR_TEXT_LEN];

            if(AMGetErrorText(hr, Buffer, MAX_ERROR_TEXT_LEN))
            {
					//::MessageBox(hWndMovie, Buffer,
                    //IdStr(STR_APP_TITLE), 
						  //"aa", MB_OK);
            }
            else
            {
                //MessageBox(hwndApp,
                  //  TEXT("Failed to open the movie.  Either the file was ")
                    //TEXT("not found or the wave device is in use."),
                    //IdStr(STR_APP_TITLE), MB_OK);
            }

            pMovie->CloseMovie();
            delete pMovie;
            pMovie = NULL;


				if ( modeId==MODE_FULLSCREEN ) {
					// If we are in fullscreen mode and failed to open movie,
					//  fulscreen background must be cleared.
					pFullscreenWnd->InvalidateRect(NULL, TRUE);
					pFullscreenWnd->UpdateWindow();
				}

        }
    }

   // Set scrollbar position at start always when opening movie.
	dMoviePosition = 0.0;

	UpdateButtons();
	UpdateStatus();

	// Regardles of movie opened me must redraw window.
	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}

void CMainWnd::RepositionMovie()
{
	if(pMovie)
	{
		
		RECT rc = pSkin->GetPlayArea( );
		
		pMovie->PutMoviePosition(rc.left, rc.top, 
			RECTW(rc), RECTH(rc) );

		HDC hdcWin = ::GetDC(NULL);
		pMovie->RepaintVideo(hWndMovie, hdcWin);
		::ReleaseDC(hWndMovie, hdcWin);

		if ( pSubtitles ) pSubtitles->Reposition(rc.left, rc.top,
									RECTW(rc), RECTH(rc) );

	}
}

void	CMainWnd::PlayMovie()
{
    BOOL fStopped = (stateMovie & VCD_STOPPED);
    BOOL fPaused  = (stateMovie & VCD_PAUSED);

    if((fStopped || fPaused))
    {
        if(pMovie)
        {
            pMovie->PlayMovie();
        }

        stateMovie &= ~(fStopped ? VCD_STOPPED : VCD_PAUSED);
        stateMovie |= VCD_PLAYING;
    }
}

void	CMainWnd::PauseMovie()
{
    BOOL fPlaying = (stateMovie & VCD_PLAYING);

    if(fPlaying)
    {
        if(pMovie)
        {
            pMovie->PauseMovie();
        }

        stateMovie &= ~VCD_PLAYING;
        stateMovie |= VCD_PAUSED;
    }
}

void	CMainWnd::StopMovie()
{
    BOOL fPlaying = (stateMovie & VCD_PLAYING);
    BOOL fPaused  = (stateMovie & VCD_PAUSED);
	 BOOL fStopped  = (stateMovie & VCD_STOPPED);

    if((fPlaying || fPaused))
    {
        if(pMovie)
        {
				dMoviePosition = 0;
            pMovie->StopMovie();
				pMovie->SeekToPosition( (REFTIME)0.001, TRUE, TRUE );
				
        }

        stateMovie &= ~(fPlaying ? VCD_PLAYING : VCD_PAUSED);
        stateMovie |= VCD_STOPPED;

    } else if ( fStopped ) {

			dMoviePosition = 0;
			pMovie->SeekToPosition( (REFTIME)0.001, TRUE, TRUE );
	 }
}

void CMainWnd::CloseMovie()
{
	if(pMovie)
   {
			if ( pSubtitles ) 
			{
				pSubtitles->SetMovie( NULL );
				pSubtitles->DeleteSubtitles();
			}

        stateMovie = VCD_NO_CD;
        pMovie->StopMovie();
        pMovie->CloseMovie();


        delete pMovie;
        pMovie = NULL;
    }

	pFullscreenWnd->UpdateBackground( TRUE );
	InvalidateRect(NULL, FALSE);
	UpdateButtons();
	UpdateWindow();
}

void CMainWnd::OnSysCommand(UINT nID, LPARAM lParam) 
{
	switch (nID) {
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			if ( stateMovie&VCD_PLAYING )
				return;
			break;
	}
	
	CWnd::OnSysCommand(nID, lParam);
}

LRESULT CMainWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	//if ( message == WM_GRAPHNOTIFY ) OnMovieNotify();
	switch ( message ) {
		case WM_GRAPHNOTIFY: 
			OnMovieNotify(); 
			return TRUE;

		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_MOUSEWHEEL:
		case WM_COMMAND:			
			if ( pTooltip ) { delete pTooltip; pTooltip = NULL; }	 
			break;

		case WM_DESTROY:
			KillTimer( timerId );
			OnWindowClose();
			break;

		case WM_EXITMENULOOP: OnExitMenuLoop( (BOOL)wParam ); break;

		case WM_DISPLAYCHANGE:
			OnDisplayChange( LOWORD(lParam), HIWORD(lParam), wParam );
			break;


		/*case WM_DDE_DATA:
			
			//ATOM atom;
			UINT lo, hi;
			UnpackDDElParam( WM_DDE_DATA, lParam, &lo, &hi );
			HGLOBAL hGlobal = (HGLOBAL)lo;
			char *psz; psz = (char*)GlobalLock( hGlobal );
			MessageBox(psz,"GLOBAL");
			GlobalUnlock( hGlobal );
			break;
			*/
	}

	return CWnd::DefWindowProc(message, wParam, lParam);
}


void CMainWnd::OnMovieNotify()
{
	long    lEventCode;

	if ( !pMovie ) return;

	while ( pMovie && (lEventCode = pMovie->GetMovieEventCode()) ) {

		switch( lEventCode )
		{
			//case EC_ERRORABORT:
			case EC_COMPLETE:
				OnTimer( TIMER_GLOBAL );

				// If loop than play movie again.
				if ( pOptions->bLoop )
				{
					pMovie->PlayMovie();
					
				} 
				// If no loop and there is next file in playlist open it.
				else if ( pPlaylist->IsNext() ) {
					OpenMovie( (char*)pPlaylist->Next(), pOptions->bAutoPlay );
				} 
				// If end playlist, and no loop defined in options,
				//  check rewind, if set rewind movie to start.
				else if ( pOptions->bAutoRewind ) 
				{
					StopMovie();

				} 
				// If no playlist, loop, and rewind just stop movie.
				else {	

					pMovie->StopMovie();

					stateMovie &= ~VCD_PLAYING;
					stateMovie &= ~VCD_PAUSED;
					stateMovie |= VCD_STOPPED;
				}
				
				UpdateButtons();
				break;

			default: break;
		}
	}
}

void CMainWnd::UpdateStatus(BOOL bUpdStatus, BOOL bUpdTime)
{

	if ( bUpdStatus )
	{
		if ( pMovie )
		{
			if ( stateMovie&VCD_PLAYING )
				//pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, szText[ T_PLAYING ] );
				pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, transl(VCD_PLAYING, TRANSL_MOVIESTATE) );
			else if ( stateMovie&VCD_PAUSED )
				//pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, szText[ T_PAUSED ] );
				pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, transl(VCD_PAUSED, TRANSL_MOVIESTATE) );
			else if ( stateMovie&VCD_STOPPED )
				//pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, szText[ T_STOPPED ] );
				pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, transl(VCD_STOPPED, TRANSL_MOVIESTATE) );

		}
		else
		{
			//pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, szText[ T_NOMOVIE] );
			pSkin->SetStatusText( m_hWnd, ST_MOVIESTATE, transl(VCD_NO_CD, TRANSL_MOVIESTATE) );
		}

	}

	if ( bUpdTime )
	{
		if ( pMovie )
		{
			double curr = pMovie->GetCurrentPosition(),
				duration = pMovie->GetDuration();

			curr += 0.49; duration += 0.49;
			int	h1 =  (int)curr / 3600,
					m1 = ((int)curr % 3600) / 60,
					s1 = ((int)curr % 3600) % 60,
					h2 =  (int)duration / 3600,
					m2 = ((int)duration % 3600) / 60,
					s2 = ((int)duration % 3600) % 60;

			char sz[32];
			sprintf(sz, "%01d:%02d:%02d/%01d:%02d:%02d",
								h1, m1, s1, h2, m2, s2);
			pSkin->SetStatusText( m_hWnd, ST_MOVIETIME, sz );

		} else {
			pSkin->SetStatusText( m_hWnd, ST_MOVIETIME, "0:00:00/0:00:00" );
		}
	}
}

void CMainWnd::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent != TIMER_GLOBAL ) return;

	// We are here every 40 milisecond.

	BOOL bPlaying = stateMovie & VCD_PLAYING;


	if ( pMovie && bPlaying ) {
		// Update scrollbar movie position, and status time.

		static double currLast = 0, currLastLong = 0;
		double curr = pMovie->GetCurrentPosition(), duration;
		if (curr != currLast ) {

			duration = pMovie->GetDuration();

			dMoviePosition = 100.0/duration*curr;

			if ( pSkin->SetScrollbarValue( SP_POSITION, dMoviePosition ) )
				pSkin->DrawScrollbar( m_hWnd, SP_POSITION );


			currLast = curr;

			if ( fabs(curr - currLastLong) > 0.25 ) {

				// We are here every movie 0.25s, not often.
				currLastLong = curr;

				UpdateStatus(FALSE);
			}
		}

	}

	

	// When fullscreen hide mouse and control window.
	if ( modeId == MODE_FULLSCREEN ) 
	{
		DWORD time = GetTickCount();
		
		if ( time-timeMouseMove > pOptions->dwMilisecondsToHideMouse )
		{
			// Hide mouse (mouse is hidden only if bAllowHideMouse is set).

			if ( pSkin->IsDefined() ) {

				POINT pt; GetCursorPos( &pt );
				RECT rc; GetWindowRect( &rc );

				// Hide control window and mouse if mouse not over control
				//  window by dwMiliseconds to hide;
				if ( !PtInRect(&rc, pt) && bAllowHideMouse) {

					// Hide control window.
					ShowWindow( SW_HIDE );

					// Hide mouse (mouse is hidden only if bAllowHideMouse is set).
					if ( !pPlaylist->IsWindowVisible() && 
						  !pDlgOptions->IsWindowVisible() &&
						  (!pFiltersProp || (pFiltersProp && !pFiltersProp->IsWindowVisible() ) )) 
						ShowMouse( FALSE );

				}
			} 
			else ShowMouse(FALSE);

		}	
	}

	// Enable tooltip.
	static int idTooltipButtonOver = -1;
	static int timeTooltip = 0;
	if ( idButtonOver>=0 && !pressedLeft && 
			!pressedRight && !pressedMiddle ) {

		int time = GetTickCount();
		if ( idTooltipButtonOver==-1 ) {
			if ( pTooltip ) { delete pTooltip; pTooltip = NULL; }
			timeTooltip = time;

		} else if ( idTooltipButtonOver==idButtonOver ) {

			if ( time-timeTooltip > 500 && !pTooltip ) {

				POINT pt; GetCursorPos( &pt );
				RECT rc; GetWindowRect( &rc );
				if ( PtInRect(&rc, pt) ) {

					// When create CTooltip window, it send WM_ACTIVATE,
					//  and if pTooltip is NULL we disable tooltip,
					//  so here we must set pTooltip to temporay TRUE;
					pTooltip = (CTooltip*)1;

					pTooltip = new CTooltip( this, 
						//szText[ T_TT_CLOSE+idTooltipButtonOver ],
						transl( idTooltipButtonOver, TRANSL_TOOLTIPS ),
						pSkin->IsButtonEnabled(idTooltipButtonOver) );

					timeTooltip = time+1000*120;

				} else { timeTooltip = time+1000*120; }
			}

		} else { 
			if ( pTooltip ) { delete pTooltip; pTooltip = NULL; }
			timeTooltip = time; }
		idTooltipButtonOver = idButtonOver;

	} else { 

		if ( pTooltip ) { delete pTooltip; pTooltip = NULL; }

		if ( idButtonPress==idTooltipButtonOver && idButtonPress>=0 )
		{
			// User pressed button, but don't move mouse outside button,
			//  so we can't draw tooltip (unless user don't move
			//	 mouse for 2 minutes).
			timeTooltip = GetTickCount()+1000*120;
		}
		else 
		{
			timeTooltip = GetTickCount();
			idTooltipButtonOver = -1;
		}
	}


	// If we move mouse from button direct out of window client rect,
	//  windows don't send WM_MOUSEMOVE, and button stay in state
	//  BSTATE_MOUSE_OVER but whould be in state BSTATE_MOUSE_NORMAL,
	//  so we must correct it here.
	if ( idButtonOver>=0 && idButtonPress==-1 ) {

		POINT pt; GetCursorPos( &pt );
		RECT rc; GetWindowRect( &rc );
		if ( !PtInRect(&rc, pt) ) {
			if ( pSkin->SetButtonState( idButtonOver, BSTATE_MOUSE_NORMAL ) )
				pSkin->DrawButton(m_hWnd, idButtonOver);
			idButtonOver = -1;
		}

	}
	
	CWnd::OnTimer(nIDEvent);
}

void CMainWnd::AllowHideMouse( BOOL bAllow )
{

	bAllowHideMouse = bAllow;
	if ( !bAllowHideMouse ) ShowMouse( TRUE );

}

void CMainWnd::ShowMouse( BOOL bShow )
{
		static int count =0;

	// Show cursor.
	if ( bShow && bMouseHidden )
	{
		bMouseHidden = FALSE;
		ShowCursor( TRUE );
	}

	// Hide cursor.
	if ( !bShow && !bMouseHidden && bAllowHideMouse )
	{
		bMouseHidden = TRUE;
		ShowCursor( FALSE );
	}

}

BOOL bPrevAllowState;
void CMainWnd::LockMouseHide()
{
	bPrevAllowState = bAllowHideMouse;
	bAllowHideMouse = FALSE;
	ShowMouse(TRUE);
}
void	CMainWnd::UnlockMouseHide()
{
	bAllowHideMouse = bPrevAllowState;
}


BOOL CMainWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD wNotifyCode = HIWORD(wParam); // notification code 
	WORD wID = LOWORD(wParam);

	if ( wNotifyCode )
	{	// Notify from accelerator.
		
		switch ( wID )
		{
			case KEY_PLAYPAUSE: OnPlayerPlayPause(); break;

			case KEY_STOP: OnPlayerStop(); break;

			case KEY_REWINDFORWARD: OnPlayerRewindForward(); break;
			case KEY_REWINDBACK: OnPlayerRewindBack(); break;

			case KEY_VOLUMEUP: SetVolume( 0, 1 ); break;
			case KEY_VOLUMEDOWN: SetVolume( 0, -1 ); break;

			case KEY_GOWINDOWED: SetMode( MODE_GO_WINDOWED ); break;
			case KEY_GOFULLSCREEN: SetMode( MODE_GO_FULLSCREEN ); break;


			case KEY_EXIT: OnWindowClose(); break;

			case KEY_PANSCAN: OnPlayerPanScanSwitch(); break;
			case KEY_PREVMOVIE: OnPlayerPrev(); break;
			case KEY_NEXTMOVIE: OnPlayerNext(); break;
			case KEY_SUBTITLES: OnPlayerSubtitlesSwitch(); break;
			case KEY_OPENMOVIE: OnPlayerOpen(); break;
			case KEY_OPENSUBTITLES: OnPlayerOpenSubtitles(); break;
			case KEY_OPTIONS: OnPlayerOptions(); break;
			case KEY_MOVIEPROP: OnPlayerProperties(); break;
			case KEY_SKINBROWSER: OnPlayerOptions(IDD_PP_SKINBROWSER ); break;
			case KEY_PLAYLIST: OnPlayerPlaylistSwitch(); break;

		}

	}
	else
	{ // Notify from menu.

		switch ( wID )
		{
			// This message is send on start application and from 
			//  playlist window.
			case IDM_CALLBACK_OPENFROMPLS:
				//if ( pPlaylist->IsNext() ) {
				if ( pPlaylist->IsCurr() ) {
					InvalidateRect(NULL); UpdateWindow();

					//OpenMovie( (char*)pPlaylist->Next(), pOptions->bAutoPlay );
					OpenMovie( (char*)pPlaylist->Curr(), pOptions->bAutoPlay );
				}
				break;
			case IDM_CALLBACK_CLOSEDPLS: ShowPlaylist(FALSE); break;


			case IDM_PLAYPAUSE: OnPlayerPlayPause(); break;
			case IDM_STOP: OnPlayerStop(); break;
			case IDM_REWINDBACK: OnPlayerRewindBack(); break;
			case IDM_REWINDFORWARD: OnPlayerRewindForward(); break;
			case IDM_PREVMOVIE: OnPlayerPrev(); break;
			case IDM_NEXTMOVIE: OnPlayerNext(); break;
			case IDM_FULLSCREEN: OnPlayerFullscreenWindowed(); break;
			case IDM_PANSCAN: OnPlayerPanScanSwitch(); break;
			case IDM_PS_AUTO: OnPlayerPanScanMode(0); break;
			case IDM_PS_WIDE: OnPlayerPanScanMode(-169); break;
			case IDM_PS_ZOOM10: OnPlayerPanScanMode(10); break;
			case IDM_PS_ZOOM20: OnPlayerPanScanMode(20); break;
			case IDM_PS_ZOOM25: OnPlayerPanScanMode(25); break;
			case IDM_PS_ZOOM30: OnPlayerPanScanMode(30); break;
			case IDM_PS_ZOOM35: OnPlayerPanScanMode(35); break;
			case IDM_PS_ZOOM40: OnPlayerPanScanMode(40); break;
			case IDM_PS_ZOOM50: OnPlayerPanScanMode(50); break;
			case IDM_SHOWSUBTITLES: OnPlayerSubtitlesSwitch(); break;
			case IDM_OPENMOVIE: OnPlayerOpen(); break;
			case IDM_OPENSUBTITLES: OnPlayerOpenSubtitles(); break;
			case IDM_OPTIONS: OnPlayerOptions(); break;
			case IDM_SKINBROWSER: OnPlayerOptions(IDD_PP_SKINBROWSER ); break;
			case IDM_PLAYLIST: OnPlayerPlaylistSwitch(); break;
			case IDM_EXIT: OnWindowClose(); break;

			case IDM_FILTERSPROP: OnPlayerProperties(); break;

			case IDM_CALLBACK_SKINCHANGED:
				int ret; ret = LoadXMLSkin(); // Load new skin
				static char szError[128]; 
				if ( ret==1 )
					sprintf( szError, " Skin changed OK .");
				else
				{
					if ( ret==0 )
						sprintf( szError, " Failed to initialize new skin - you should restart windows ." );
					else
						sprintf( szError, " Failes to load skin. Error in 'player.xml' in line %i .", ret-0x10 );
				}
				
				// When changing skin in fullscreen hide (again) control 
				//  window after allowed time.
				timeMouseMove = GetTickCount(); 
				::PostMessage( (HWND)lParam, WM_COMMAND, IDM_CALLBACK_SKINCHANGED, (LONG)szError );
				wParam = 0;
				break;

			// Command from OptionsSubtitle when APPLY.
			case IDM_CALLBACK_FONTCHANGED:
				if(pMovie)
				{
					RECT rc = pSkin->GetPlayArea( );
					if ( pSubtitles ) {
						
						// Inform subtitles that options has changed.
						pSubtitles->UpdateOptions();

						// Repaint subtitles.
						pSubtitles->Reposition(rc.left, rc.top,
											RECTW(rc), RECTH(rc), TRUE );

					}
			case IDM_CALLBACK_ALWAYSONTOPCHANGED:
				if ( pOptions->bAlwaysOnTop ) 
				{
					SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE );
					pFullscreenWnd->SetWindowPos( &wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE );
				}
				else
				{
					SetWindowPos( &wndNoTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE );
					pFullscreenWnd->SetWindowPos( &wndNoTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE );
				}
				break;

			case IDM_CALLBACK_OFFSETSUBTITLESCHANGED:
				if ( pSubtitles )
				{
					pSubtitles->UpdateOptions();
					pSubtitles->OffsetSubtitles();
				}
				break;

			case IDM_CALLBACK_SAVESUBTITLES:
				if ( pSubtitles ) SaveSubtitles();
				break;

			case IDM_CALLBACK_SUBTITLESPOSCHANGED:
				if ( pSubtitles ) {
					pSubtitles->UpdateOptions();
					pSubtitles->UpdateVerticalPos();
				}
				break;

			case IDM_CALLBACK_LANGUAGECHANGED:
				InitLanguage();
				if ( pPlaylist ) pPlaylist->PostMessage( WM_COMMAND, 
					IDM_CALLBACK_LANGUAGECHANGED, 0 );
				if ( pFiltersProp && pFiltersProp->IsWindowVisible() )
					pFiltersProp->PostMessage( WM_COMMAND, 
						IDM_CALLBACK_LANGUAGECHANGED, 0 );

				return TRUE; // Don't process this message any further.

			case IDM_CALLBACK_OPENFROMMUTEX:
			{
				HANDLE handle = OpenFileMapping(  FILE_MAP_WRITE, FALSE, 
					"The Imp Shared File In Memory" );
				char *psz = (char*)MapViewOfFile( handle, FILE_MAP_WRITE, 0, 0, 0 );

				BOOL bAddedFile = FALSE;
				///char *psz = (char*)lParam, *pszbuff = new char[ strlen(psz)+1 ];
				char *pszbuff = new char[ strlen(psz)+1 ];
				while ( *psz ) {
					while ( *psz && *psz==' ') psz++;	//skip begining spaces

					if ( *psz=='"' ) {
			
						psz++; char *pszstart = psz;
						while ( *psz && *psz!='"' ) psz++;
						char c=*psz; *psz = 0; 
						strcpy( pszbuff, pszstart );
						*psz = c; if ( *pszstart ) psz++;

					} else if (*psz ) {

						char *pszstart = psz;
						while ( *psz && *psz!=' ' ) psz++;
						char c=*psz; *psz = 0; 
						strcpy( pszbuff, pszstart );
						*psz = c;

					} else *pszbuff = 0;

					if ( *pszbuff ) 
					{
						if ( *pszbuff=='/' )
						{
				
							if ( !_stricmp(pszbuff+1, "NoLoop") )
								pOptions->bLoop = FALSE;

						}
						else
						{
							pPlaylist->Add(pszbuff, !bAddedFile);
							bAddedFile = TRUE;
						}
					}

				}
				UnmapViewOfFile( psz );
				CloseHandle( handle );

				delete[] pszbuff;
				if ( bAddedFile ) OpenMovie( (char*)pPlaylist->Curr(), pOptions->bAutoPlay );

			}
			

			}
			break;
		}
		
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

BOOL CMainWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	SetVolume( 0, zDelta );
	
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CMainWnd::OnParentNotify(UINT message, LPARAM lParam) 
{
	CWnd::OnParentNotify(message, lParam);
	
	if ( pTooltip && message==WM_CLOSE && (HWND)lParam==pTooltip->m_hWnd) {
		delete pTooltip; pTooltip = NULL;
	}
}

void CMainWnd::OnDropFiles(HDROP hDropInfo) 
{
	 int cch = DragQueryFile(hDropInfo, 0, NULL, 0) + 1;
    TCHAR * pName = new TCHAR[cch];

    DragQueryFile(hDropInfo, 0, pName, cch);

	 // Open the movie.
    OpenMovie(pName, pOptions->bAutoPlay );

	pPlaylist->Add( pName );

    // Free up used resources.
    delete [] pName;	
	
	CWnd::OnDropFiles(hDropInfo);
}

void CMainWnd::OnMButtonUp(UINT nFlags, CPoint point) 
{
	SetMode( MODE_FULLSCREEN_SWITCH );
	
	CWnd::OnMButtonUp(nFlags, point);
}

void CMainWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CWnd::OnWindowPosChanged(lpwndpos);

	if ( modeId!=MODE_FULLSCREEN && idButtonPress==-1) {
		//pFullscreenWnd->SetWindowPos(NULL,lpwndpos->x, lpwndpos->y,
		//	0,0, SWP_NOSIZE|SWP_NOZORDER);
	}
	
}

void CMainWnd::OnExitMenuLoop( BOOL bIsTrackPopupMenu )
{
	UnlockMouseHide();
	bQuickMenu = FALSE;
}

void CMainWnd::OnDisplayChange( int cxScreen, int cyScreen, int cBitsPerPixel )
{
	sizeScreen.cx = GetSystemMetrics( SM_CXSCREEN );
	sizeScreen.cy = GetSystemMetrics( SM_CYSCREEN );
	sizeMaximized.cx = GetSystemMetrics( SM_CXMAXIMIZED );
	sizeMaximized.cy = GetSystemMetrics( SM_CYMAXIMIZED );


	pskinNormal->CreateBackground( m_hWnd );
	pskinNormal->ShareBackgroundWith( pskinCompact );
	pskinNormal->ShareBackgroundWith( pskinFullscreen );


	if ( pMovie ) pMovie->DisplayModeChanged();

	if ( modeId&MODE_FULLSCREEN )
	{

		if ( bFsConstPos )
		{

			if ( bStoredFsAllowResize ) {
	
				long w,h; pSkin->GetSkinSize( &w, &h);
				w = sizeScreen.cx;
				pSkin->SetAllowResize(TRUE);

				pSkin->Resize(m_hWnd, &w, &h, TRUE );
				pSkin->SetAllowResize(FALSE);

			} else {

				pSkin->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );
			}

		} else {
			pSkin->Resize(m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );
		}

		// Resize change play area, so set it to fullscreen again.
		pSkin->SetPlayArea( 0,0, sizeScreen.cx, sizeScreen.cy );

		if ( bFsConstPos )
		{
			// Set skin window pos at bottom-left corner.
			long w,h; pSkin->GetSkinSize( &w, &h);
			SetWindowPos( NULL, 0, sizeScreen.cy-h, 0,0, 
				SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|
				SWP_NOCOPYBITS|SWP_NOOWNERZORDER);
		}
		else
		{
			RECT rc; GetWindowRect( &rc );
			if ( rc.top>sizeScreen.cy ) rc.top = sizeScreen.cy-(rc.bottom-rc.top);
			if ( rc.left>sizeScreen.cx ) rc.left = sizeScreen.cx-(rc.right-rc.left);
			SetWindowPos( NULL, rc.left, rc.top, 0,0, 
				SWP_NOSIZE|SWP_NOZORDER|SWP_NOSENDCHANGING|
				SWP_NOCOPYBITS|SWP_NOOWNERZORDER);
		}

		RepositionMovie();

		return;
	}

	if ( bMaximized )
	{
		sizeWnd = sizeMaximized;
		if ( pOptions->bMaximizeOverTaskMenu ) sizeWnd = sizeScreen;

		int hidePixels = (sizeMaximized.cx-sizeScreen.cx)/2;
		if ( pOptions->bMaximizeOverTaskMenu ) {
			sizeWnd.cx += hidePixels*2;
			sizeWnd.cy += hidePixels*2;
		}
		SetWindowPos( NULL, -hidePixels, -hidePixels, 0,0,
			SWP_NOZORDER|SWP_NOSIZE|SWP_NOSENDCHANGING|SWP_NOOWNERZORDER
			|SWP_NOCOPYBITS);
		pSkin->Resize( m_hWnd, &sizeWnd.cx, &sizeWnd.cy );
		RepositionMovie();
		InvalidateRect(NULL); UpdateWindow();
		
		return;
	}

	pSkin->Resize( m_hWnd, &sizeWnd.cx, &sizeWnd.cy, TRUE );
	RepositionMovie();
	InvalidateRect(NULL); UpdateWindow();

}

void CMainWnd::OpenSubtitles( char *pszFileName, BOOL bAutoSearch )
{
	
	BOOL bLoaded = FALSE;
	pSubtitles->DeleteSubtitles();

	char *pszPath;

	if ( !bAutoSearch ) {
		bLoaded = pSubtitles->ReadSubtitles( pszFileName );
		pszPath = pszFileName;
	}
	else
	{
		char path[_MAX_PATH], drive[_MAX_DRIVE], 
			dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

	   _splitpath( pszFileName, drive, dir, fname, ext );
		_makepath( path, drive, dir, fname, "txt" );
		bLoaded = pSubtitles->ReadSubtitles( path );

		pszPath = path;
	}

	if ( bLoaded ) strcpy( pOptions->szLastSubtitlesPath, pszPath );

	pOptions->bSubtitlesLoaded = bLoaded;

	// If option dialog is opened with visible property page
	//  Subtitles->Time offset, we must inform that we tried
	//  load subtitles.
	if ( pDlgOptions ) pDlgOptions->PostMessage( WM_COMMAND,
		IDM_CALLBACK_TRIEDLOADSUBTITLES, 0 );

}

void CMainWnd::SaveSubtitles()
{
	if ( !pSubtitles ) return;

	char szFileName[MAX_PATH];

	char szDefExt[] = "txt";
	char szFilter[] = {	
		"Text files|*.txt|All files|*.*||",
	};
	for(char *p = szFilter; *p; p++) if(*p == '|') *p = '\0' ;
	

	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndMovie;
	ofn.Flags = OFN_EXPLORER;

	ofn.lpstrInitialDir = pOptions->szLastSubtitlesPath;
	ofn.lpstrFilter = szFilter;      

	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	strcpy( szFileName, pOptions->szLastSubtitlesPath );
	ofn.lpstrDefExt = szDefExt;

	if( GetSaveFileName(&ofn) )
	{
		pSubtitles->WriteOffsetedSubtitles( szFileName );
	}
}

void CMainWnd::InitLanguage()
{
	UpdateStatus();
	if ( pTooltip ) { delete pTooltip; pTooltip = NULL; }
}

void CMainWnd::OnPlayerProperties()
{
	if ( pMovie )
	{
		if ( pFiltersProp ) delete pFiltersProp;
		pFiltersProp = new CFilterProp( pOptions, pMovie );
		pFiltersProp->Create(IDD_DIALOG_FILTERSPROP,this);

		RECT rcSrc, rcDest;
		if ( modeId==MODE_FULLSCREEN ) 
			pFullscreenWnd->GetWindowRect( &rcSrc );
		else
			GetWindowRect( &rcSrc );
		pFiltersProp->GetWindowRect( &rcDest );

		int x = (RECTW(rcSrc)-RECTW(rcDest))/2,
			y = (RECTH(rcSrc)-RECTH(rcDest))/2;
		pFiltersProp->SetWindowPos( NULL, rcSrc.left+x, rcSrc.top+y,
			0,0, SWP_NOZORDER|SWP_NOSIZE );

		pFiltersProp->ShowWindow(SW_SHOW);
	}

}


void CMainWnd::OnPlayerOpenSubtitles()
{
	if ( !pMovie ) return;

	char szFileName[MAX_PATH];

	char szDefExt[] = "txt";
	char szFilter[] = {	
		"Text files|*.txt|All files|*.*||",
	};
	for(char *p = szFilter; *p; p++) if(*p == '|') *p = '\0' ;

	OPENFILENAME ofn;
	ZeroMemory( &ofn, sizeof(ofn) );
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWndMovie;
	ofn.Flags = OFN_FILEMUSTEXIST |OFN_EXPLORER |
			OFN_SHAREAWARE | OFN_PATHMUSTEXIST;

	ofn.lpstrInitialDir = pOptions->szLastMoviePath;
	ofn.lpstrFilter = szFilter;      

	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	//strcpy( szFileName, pOptions->szLastSubtitlesPath );
	//ofn.lpstrDefExt = szDefExt;

	ZeroMemory(szFileName, sizeof(szFileName));

	LockMouseHide();
	if( GetOpenFileName(&ofn) )
	{
		OpenSubtitles( szFileName, FALSE );
	}

	UnlockMouseHide();
}
