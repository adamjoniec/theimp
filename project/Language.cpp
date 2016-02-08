
#include "stdafx.h"
#include "language.h"
#include "skindefinition.h"
#include "xmlparser.h"
#include "resource.h"

#define VCD_PLAYING          0x0001
#define VCD_STOPPED          0x0002
#define VCD_PAUSED           0x0004
#define VCD_NO_CD            0x0100
#define VCD_OPENING			  0x10000

struct stTranslation
{ int id; char *psz; };

stTranslation langDefault[] = {

	//////////////////////////////////////////////////////////////////
	TRANSL_MOVIESTATE, "",
	//////////////////////////////////////////////////////////////////

	VCD_NO_CD, "No movie loaded",
	VCD_OPENING, "Opening file",
	VCD_PLAYING, "Playing",
	VCD_PAUSED, "Paused",
	VCD_STOPPED, "Stopped",


	//////////////////////////////////////////////////////////////////
	TRANSL_TOOLTIPS, "",
	//////////////////////////////////////////////////////////////////

	BW_CLOSE, "Close" ,
	BW_MINIMIZE, "Minimize" ,
	BW_MAXIMIZE, "Maximize / Restore",
	BW_SYSMENU, "Menu",
	BW_RESIZE, "Resize" ,
	BW_CHANGE_SKIN, "Change skin" ,
	BW_ABOUT, "About" ,

	BP_PLAY, "Play",
	BP_PAUSE, "Pause",
	BP_PLAY_PAUSE, "Play / Pause",
	BP_REWIND_FORWARD, "Fast Forward",
	BP_REWIND_BACK, "Rewind",
	BP_STOP, "Stop",

	BP_NEXT_MOVIE, "Next movie",
	BP_PREV_MOVIE, "Prev movie",

	BP_OPEN, "Open movie",
	BP_OPEN_SUBTITLES, "Open subtitles",
	BP_SUBTITLES_ON_OFF, "Subtitles On / Off",
	BP_SUBTITLES_DELAY, "Subtitles delay",
	BP_FULLSCREEN_WINDOWED, "Fullscreen / Windowed",
	BP_MODE_NORMAL_COMPACT, "Mode Normal / Compact",
	BP_PANSCAN_ON_OFF, "PanScan On / Off",
	BP_PANSCAN_MENU, "Setup PanScan",
	BP_VOLUME_ON_OFF, "Volume On / Off",
	BP_PLAYLIST_SHOW_HIDE,"Show / Hide playlist",
	BP_OPTIONS, "Options",


	//////////////////////////////////////////////////////////////////
	TRANSL_QMENU, "",
	//
	// unchecked#checked
	//////////////////////////////////////////////////////////////////
	IDM_PLAYPAUSE,"Play / Pause",	//\tSpace
	IDM_STOP,"Stop",//\tV
	IDM_REWINDBACK,"Rewind",//\tArrow left
	IDM_REWINDFORWARD,"Fast Forward",//\tArrow right
	IDM_PREVMOVIE,"Prev Movie",
	IDM_NEXTMOVIE,"Next Movie",
	IDM_PLAYLIST,"Playlist",
	IDM_FULLSCREEN,"Go Fullscreen#Go Windowed",//\tAlt+Enter#\tEscape
	IDM_PANSCAN,"PanScan",
	11, "PanScan Mode",
	IDM_PS_AUTO,"Auto",
	IDM_PS_WIDE,"Wide (16:9)",
	IDM_PS_ZOOM10,"Zoom 10%",
	IDM_PS_ZOOM20,"Zoom 20%",
	IDM_PS_ZOOM25,"Zoom 25%",
	IDM_PS_ZOOM30,"Zoom 30%",
	IDM_PS_ZOOM35,"Zoom 35%",
	IDM_PS_ZOOM40,"Zoom 40%",
	IDM_PS_ZOOM50,"Zoom 50%",
	IDM_SHOWSUBTITLES,"Show Subtitles#Hide Subtitles",
	IDM_OPENMOVIE,"Open Movie...",
	IDM_OPENSUBTITLES,"Open Subtitles...",
	IDM_OPTIONS,"Options...",
	IDM_FILTERSPROP,"Movie Properties",
	IDM_SKINBROWSER,"Skin Browser...",
	IDM_EXIT,"Exit",


	//////////////////////////////////////////////////////////////////
	TRANSL_QMENU_PLS, "",
	//////////////////////////////////////////////////////////////////
	IDD_DIALOG_PLAYLIST, "Playlist",
	ID_QMPLS_OPEN,"Open playlist",
	ID_QMPLS_SAVE,"Save playlist",
	ID_QMPLS_CLEARPLS,"Clear playlist",
	ID_QMPLS_ADDFILES,"Add file(s)",
	ID_QMPLS_REMOVEFILE,"Remove selected file",
	ID_QMPLS_CLOSE, "Close playlist",


	//////////////////////////////////////////////////////////////////
	TRANSL_KEYS, "",
	//////////////////////////////////////////////////////////////////	
	FSHIFT, "Shift",
	FCONTROL, "Ctrl",
	FALT, "Alt",
	VK_SPACE, "Space",
	VK_LEFT, "Left",
	VK_RIGHT, "Right",
	VK_UP, "Up",
	VK_DOWN, "Down",
	VK_RETURN, "Enter",
	VK_ESCAPE, "Esc",
	VK_INSERT, "Insert",
	VK_DELETE, "Delete",
	VK_HOME, "Home",
	VK_END, "End",
	VK_PRIOR, "PageUp",
	VK_NEXT, "PageDown",
	VK_BACK, "Bakspace",

	//////////////////////////////////////////////////////////////////
	TRANSL_OPTIONS, "",
	//////////////////////////////////////////////////////////////////
	IDD_DIALOG_OPTIONS, "Options",
	IDC_BUTTON_CLOSE, "Close",
	IDC_BUTTON_DEFAULT, "Default",
	IDC_BUTTON_APPLY, "Apply",

	IDD_PP_SETUP, "Setup",
	IDD_PP_FILETYPES, "File types",
	IDD_PP_OPTIONS, "Options",
	IDD_PP_PLAY, "Play",
	IDD_PP_STARTUP, "Startup",
	IDD_PP_SUBTITLES, "Subtitles",
	IDD_PP_SUBTITLES_GENERAL, "Additional",
	IDD_PP_TIMECORRECTION, "Time correction",
	IDD_PP_SKINBROWSER, "Skin browser",
	IDD_PP_ABOUT, "About",

	
	//////////////////////////////////////////////////////////////////
	TRANSL_PP_SETUP, "",
	//////////////////////////////////////////////////////////////////
	//IDD_PP_SETUP, "Setup",
	IDC_TEXTT_TITLECURRLANG, "Current language",
	

	//////////////////////////////////////////////////////////////////
	TRANSL_PP_FILETYPES, "",
	//////////////////////////////////////////////////////////////////
	IDC_TEXTT_TITLEASSOCIATED, "Associated extensions",
	IDC_TEXTT_TITLEICON, "Icon",
	IDC_BUTTON_ALL, "Select all",
	IDC_BUTTON_NONE, "Select none",
	IDC_TEXTT_REGISTERONSTART, "Register types when 'The Imp' starts",
	
	//////////////////////////////////////////////////////////////////
	TRANSL_PP_OPTIONS, "",
	//////////////////////////////////////////////////////////////////
	IDC_TEXTT_TITLEGENERAL, "General",
	IDC_TEXTT_TITLEPLAYLIST, "Playlist",
	IDC_CHECK_ALWAYSONTOP, "Always on top",
	IDC_CHECK_MAXOVERTASK, "Maximize over task menu",
	IDC_TEXTT_HIDEAFTER, "Hide cursor and control window when fullscreen after:",
	IDC_TEXTT2_HIDEAFTER, "[ ms ]",
	IDC_TEXTT_MAXPLSFILES, "Maximum playlist files:",
	IDC_CHECK_HIDEPLSMLEFT, "Hide PLS on mouse left-button click",

	
	//////////////////////////////////////////////////////////////////
	TRANSL_PP_PLAY, "",
	//////////////////////////////////////////////////////////////////
	IDC_TEXTT_TITLESEEKING, "Seeking options",
	IDC_TEXTT_TITLEGENERAL, "General options",
	IDC_TEXTT_REWINDBY, "Rewind \\ Fast forward movie by",
	IDC_TEXTT2_REWINDBY, "seconds",
	IDC_CHECK_FASTSEEK, "Speed up seek and rewind",
	IDC_TEXTT_FASTSEEK, "( experimental, use on your own risk )",
	IDC_CHECK_LOOP, "Loop playback",
	IDC_CHECK_AUTOREWIND, "Auto rewind movie",
	IDC_CHECK_AUTOPLAY, "Auto play movie when loaded",


	//////////////////////////////////////////////////////////////////
	TRANSL_PP_STARTUP, "",
	//////////////////////////////////////////////////////////////////
	IDC_TEXTT_OPENONMOVIE, "Open program in",
	IDC_TEXTT2_OPENONMOVIE, "mode when no movie to open",
	IDC_TEXTT2_OPENWITHMOVIE, "mode when double-clicking or dragging a movie",
	0, "Normal",
	1, "Compact",
	2, "Fullscreen",
	3, "Last closed",


	//////////////////////////////////////////////////////////////////
	TRANSL_PP_SUBTITLES, "",
	//////////////////////////////////////////////////////////////////
	IDC_BUTTON_CHANGEFONT, "Change font...",
	IDC_TEXT_FONTCOL, "Font color ...",
	IDC_TEXT_OUTLINECOL, "Outline color ...",
	IDC_TEXT_BKGRNDCOL, "Background color ...",
	IDC_TEXTT_MODESEL, "Transparent mode:",
	IDC_TEXTT_OUTLINESEL, "Font outline:",
	IDC_CHECK_TRANSPARENTBKGRND, "Transparent background",
	IDC_CHECK_SHORTBKGRND, "Short background",
	IDC_CHECK_RGNPRECISE, "Region mode precise",
	0,"Overlay (faster)",
	1,"Regions (more compatible)",
	2,"None",
	3,"Thin",
	4,"Bold",
	5,"Example text",


	//////////////////////////////////////////////////////////////////
	TRANSL_PP_SUBTITLES_GENERAL, "",
	//////////////////////////////////////////////////////////////////
	IDD_TEXTT_TITLETIMEPROP, "Time properties",
	IDC_TEXTT_TITLEADDITIONAL, "Additional options",
	IDD_TEXTT_TIMETOHIDE, "Hide subtitles after:",
	IDD_TEXTT2_TIMETOHIDE, "seconds",
	IDD_TEXTT_TEXTROWS, "Text rows:",
	IDD_TEXTT_ROWDISTANCE, "Row distance:",
	IDD_TEXTT_MROWDIR, "Multi-row direction:",
	0,"Top - Bottom",
	1,"Bottom - Top",
	IDC_CHECK_AUTOSIZE, "Autosize",
	IDC_TEXTT_TITLEPOSITION, "Position",
	
	//////////////////////////////////////////////////////////////////
	TRANSL_PP_SUBTITLES_TIME, "",
	//////////////////////////////////////////////////////////////////
	IDC_CHECK_OFFSET, "Offset subtitles",
	IDC_RADIO_SPEEDUP, "Speed up",
	IDC_RADIO_DELAY, "Delay",
	IDC_TEXTT_TIME, "subtitles by",
	IDC_TEXTT2_TIME, "[ hh:mm:ss ]",
	IDC_BUTTON_SAVEAS, "Save modified subtitles as...",


	//////////////////////////////////////////////////////////////////
	TRANSL_PP_SKINBROWSER, "",
	//////////////////////////////////////////////////////////////////
	IDC_TEXTT_TITLESKINIFNO, "Skin info",
	IDC_TEXT_PREVNOTAVAIL, "Preview Not Available",
	IDC_TEXTT_VERSION, "Version:",
	IDC_TEXTT_NAME, "Name:",
	IDC_TEXTT_AUTHOR, "Author:",
	IDC_TEXTT_COMMENT, "Comment:",


	//////////////////////////////////////////////////////////////////
	TRANSL_FILTERPROP, "",
	//////////////////////////////////////////////////////////////////
	IDD_DIALOG_FILTERSPROP, "Movie properties",
	IDC_TEXTT_TITLEPICTURE, "(*) Picture settings",
	IDC_TEXTT_WINXPONLY, "(*) WinXP only",
	IDC_TEXTT_BRIGHTNESS, "Brightness:",
	IDC_TEXTT_CONTRAST, "Contrast:",
	IDC_TEXTT_SATURATION, "Saturation:",
	IDC_TEXTT_TITLESFILTERS, "Used filters with property page",
	IDC_BUTTON_PROP, "Properties",
	IDOK, "OK",
	IDCANCEL, "Cancel",


	TRANSL_END, ""
};

BOOL IsLang( char *pszFileName, CString &retLanguage, CString &retAuthor )
{
	cArchive zip; 
	CXMLParser XMLFile;
	BOOL bIsLang = FALSE;

	char szLangDefinitionId[] = "translated";
	

	char name[_MAX_FNAME], ext[_MAX_EXT];
	_splitpath( pszFileName, NULL, NULL, name, ext );

	retLanguage.Empty(); retAuthor.Empty();

	if ( !_stricmp(ext, ".zip") )
	{
		// Propably zip file.
		zip.OpenZIP( pszFileName );
		char sz[_MAX_PATH]; _makepath( sz, NULL, NULL, name, "xml" );
		XMLFile.OpenXMLFile( sz, &zip );
	}
	else if ( !_stricmp(ext, ".xml") )
	{
		XMLFile.OpenXMLFile( pszFileName );
	}

	if ( !XMLFile.IsOpened() )  return bIsLang;


	int iLevel;
	CPascalStr *pstrId;
	if ( !XMLFile.NextElement( &pstrId, &iLevel ) )
	{
		if ( !pstrId->cmp( szLangDefinitionId ) )
		{
			CPascalStr *pstrName, *pstrNameVal;
			while ( XMLFile.NextAttrib( &pstrName, &pstrNameVal ) )
			{
				if ( !pstrName->cmp("language") )
				{
					retLanguage = pstrNameVal->psz();
					bIsLang = TRUE;
				}
				else if ( !pstrName->cmp("author") )
				{
					retAuthor = pstrNameVal->psz();
				}
			}
		}
	}


	return bIsLang;
}
stTranslation *langCurrent = NULL;


void InitLanguage()
{
	/// Init langCurrent only once.
	int countTransl = sizeof( langDefault )/sizeof(stTranslation);
	langCurrent = new stTranslation[ countTransl ];

	stTranslation *pdef = langDefault, *pcur = langCurrent;
	for ( int i=0; i<countTransl; i++ )
	{
		pcur->id = pdef->id;
		pcur->psz = new char [ strlen(pdef->psz)+1 ];
		strcpy( pcur->psz, pdef->psz );

		pcur++; pdef++;
	}

}

void DeleteLangCurr()
{
	int countTransl = sizeof( langDefault )/sizeof(stTranslation);

	stTranslation *pcur = langCurrent;
	for ( int i=0; i<countTransl; i++, pcur++ )
		if ( pcur->psz ) { 
			delete[] pcur->psz;
			pcur->psz = NULL;
		}
}

void SelectDef()
{
		DeleteLangCurr();

		stTranslation *pdef = langDefault, *pcur = langCurrent;
		int countTransl = sizeof( langDefault )/sizeof(stTranslation);
		for ( int i=0; i<countTransl; i++ )
		{
			pcur->id = pdef->id;
			pcur->psz = new char [ strlen(pdef->psz)+1 ];
			strcpy( pcur->psz, pdef->psz );

			pcur++; pdef++;
		}

}

// If pszFilename==NULL || pszFileName=="" change language to def.
BOOL ChangeLang( char *pszFileName )
{
	if ( pszFileName==NULL || *pszFileName==0 )
	{
		SelectDef();

		return TRUE;
	}


	cArchive zip; 
	CXMLParser XMLFile;
	BOOL bIsLang = FALSE;

	char szLangDefinitionId[] = "translated";
	

	char name[_MAX_FNAME], ext[_MAX_EXT];
	_splitpath( pszFileName, NULL, NULL, name, ext );

	if ( !_stricmp(ext, ".zip") )
	{
		// Propably zip file.
		zip.OpenZIP( pszFileName );
		char sz[_MAX_PATH]; _makepath( sz, NULL, NULL, name, "xml" );
		XMLFile.OpenXMLFile( sz, &zip );
	}
	else if ( !_stricmp(ext, ".xml") )
	{
		XMLFile.OpenXMLFile( pszFileName );
	}

	if ( !XMLFile.IsOpened() )  return bIsLang;


	int iLevel;
	CPascalStr *pstrId;
	if ( !XMLFile.NextElement( &pstrId, &iLevel ) )
	{
		if ( !pstrId->cmp( szLangDefinitionId ) )
		{
			while ( !XMLFile.NextElement(&pstrId, &iLevel) )
			{
				if ( !pstrId->cmp("translations") && iLevel==2 ) break;
			}

			SelectDef(); bIsLang = TRUE;
			while ( !XMLFile.NextElement(&pstrId, &iLevel) && iLevel==2 )
			{
				if ( !pstrId->cmp("translation") || !pstrId->cmp("t") )
				{

					int itrans = 0;
					CString from, to; from.Empty(); to.Empty();
					CPascalStr *pstrName, *pstrNameVal;
					while ( XMLFile.NextAttrib( &pstrName, &pstrNameVal ) )
					{
						if ( !pstrName->cmp("from") ) 
							from = pstrNameVal->psz();
						else if ( !pstrName->cmp("to") ) 
							to = pstrNameVal->psz();
					}
					if ( !from.IsEmpty() && !to.IsEmpty() )
					{
						stTranslation *pt = langDefault, *curr=langCurrent;
						while ( pt->id<TRANSL_END ) {
							if ( !from.CompareNoCase(curr->psz) )
							{
								if ( curr->psz ) delete[] curr->psz;
								curr->psz = new char[ to.GetLength()+1 ];
								strcpy( curr->psz, to );
								break;
							}
							pt++, curr++;
						}

						
					}
				}
			}
		}
	}


	return bIsLang;

}

char *transl( int id, int section )
{
	static char *sznull = "";

	if ( !langCurrent ) return sznull;

	stTranslation *pt = langCurrent;

	// First find correct section.
	while ( pt->id!=-1 && pt->id!=section ) pt++;

	// Than find correct id within current section.
	while ( (++pt)->id<TRANSL_NEXTSECTION && pt->id!=id ) ;

	// If id founded return current text representing this id,
	if ( pt->id<TRANSL_NEXTSECTION ) return pt->psz;

	return sznull;
}