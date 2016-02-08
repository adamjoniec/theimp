
#include "stdafx.h"
#include "config.h"


stConfig defConfig = {

	///////////////////////////////////////////////////////////////
	// BEGIN_CONFIG
	0x04030201, 
	///////////////////////////////////////////////////////////////

	// dwId
	('I')+('M'<<8)+('P'<<16),

	// 100 == version 1.00
	100,

	//szAppName
	"",

	//szAppPath
	"",

	// bRegisterOnStart
	TRUE,

	// szRegisteredExts
	"asf|avi|dat|divx|m1v|mov|mpa|mpe|mpeg|mpg|wmv|imp",

	// iRegisteredExtsIcon
	5,

	//szSkinDirectory
	TEXT("skins\\"),

	//szSkinFileName
	"bestplayer.zip",

	//bSkinInZIP
	TRUE,

	// rcWindowPos
	{0,0,0,0},	//center

	//bFastSeek
	FALSE,

	//bFastRewind
	FALSE,

	//bAutoRewind
	TRUE,

	//bLoop
	FALSE,

	//dVolume
	100.0,

	//bAutoPlay
	TRUE,

	//bMuteVolume
	FALSE,

	//dRewindSecs
	10.0,

	//modeId, prevModeId
	1, 1,	//MODE_NORMAL, MODE_NORMAL
		
	//dwMiliscondsToHideMouse
	600,

	//	maxPlaylistFiles
	99,

	// szLastLoadedPath
	"",

	// bShowSubtitles
	TRUE,

	// bPanScanEnabled
	TRUE,

	//valPanScan
	0,					//Auto

	//bHidePLSOnLButtonUp
	TRUE,

	// bMaximizeOverTaskMenu
	TRUE,

	// bAlwaysOnTop
	FALSE,

	// modeIdStartWhenMovie, modeIdStartWhenNoMovie
	4, 0,

	// valBrightness, valContrast, valSaturation
	0, 50, 50,


	///////////////////////
	// Subtitles options //
	///////////////////////

	// iSubtitlesMode, iSubtitlesRows,
	1,/*SUBTITLES_OVERLAY*/ 3,
	// iSubtitlesRowsDistance, //[pixel]
	0,
	// iSubtitlesMultiRowMode
	1,/*SUBTITLESMR_BOTTOM_TOP*/
	//iSubtitlesHideAfter
	4,
	// bSubtitlesBkgrndShort, bSubtitlesRgnPrecise
	TRUE, FALSE,
	// valSubtitlesDistanceFromBottom //[percent]
	11,
	//pszSbtFontName
	"Verdana",
	//iSbtFontSize, iSbtFontWeight, iSbtFontOutline
	16, FW_BOLD, 0,/*FO_BOLD*/
	// bSbtFontAutosize, bSbtFontBkgrndTransparent;
	TRUE, TRUE,
	// colSbtFont, colSbtFontBkgrnd, colSbtFontOutline;
	0xFFFFFF, 0x000000, 0x000000,

	// szLastMoviePath[_MAX_PATH];
	"",

	"languages\\",

	// szLanguageFileName[_MAX_PATH]
	"", // NULL==DEFAULT LANGUAGE


	///////////////////////////////////////////////////////////////
	// END_CONFIG
	0x01020304, 
	///////////////////////////////////////////////////////////////


	// This options don't be written

	// bSubtitlesLoaded
	FALSE,
	// bSubtitlesOffsetTime
	FALSE,
	// valSubtitlesOffsetTime
	0
};



// Read config, return TRUE if read correctly, 
//  or FALSE when only fill config with default values.
BOOL ReadConfig( char *pszFileName, stConfig *pConfig )
{
	cFile File;

	// Calculate size of writable part of config.
	char *cfgbegin = (char*) &defConfig.BEGIN_CONFIG,
		*cfgend = (char*) &defConfig.END_CONFIG;
	DWORD dwConfigSize = cfgend-cfgbegin;	// don't read END_CONFIG

	// Initially set config to default values.
	memcpy( pConfig, &defConfig, sizeof(stConfig) );

	// Try to read config.
	File.Open( pszFileName, "r" );
	if ( !File.bOpened ) return FALSE;

	// File config size must be smaller or equal to our config.
	DWORD dwFileSize = File.Size();
	if ( dwFileSize > dwConfigSize ) return FALSE;

	File.Read( pConfig, dwFileSize );

	// If unknow id set default values and return;
	if ( pConfig->dwId != ('I')+('M'<<8)+('P'<<16) ) 
	{
		memcpy( pConfig, &defConfig, sizeof(stConfig) );
		return FALSE;
	}

	return TRUE;
}

// Write config, return TRUE if ok, 
// or false when file can't be created.
BOOL WriteConfig( char *pszFileName, stConfig *pConfig )
{
	cFile File;

	// Calculate size of writable part of config.
	char *cfgbegin = (char*) &defConfig.BEGIN_CONFIG,
		*cfgend = (char*) &defConfig.END_CONFIG;
	DWORD dwConfigSize = cfgend-cfgbegin; 	// don't write END_CONFIG

	File.Open( pszFileName, "w" );
	if ( !File.bOpened ) return FALSE;

	File.Write( pConfig, dwConfigSize );


	return TRUE;
}


// Register files with 'The Imp', pszExts must be in format
//  "ext|ext|ext|ext"
BOOL RegisterFiletypes( stConfig *pConfig, char *pszExts )
{
	CRegKey rkTheImp, rkDefIcon, rkShell, rkOpen, rkCommand;
	char szVal[] = "The Imp . Associated file",
		  szPrevId[] = "The Imp . Previous associated file",
		  sz[_MAX_PATH*2];

	// The Imp . Associated file
	//	|
	//	-- DefaultIcon						TheImpPath,1
	// |
	//	-- shell								open
	//   |
	//   -- open
	//      |
	//      -- command					TheImpPath "%1" "%2" "%3" "%4" "%5" "%6" "%7" "%8"

	rkTheImp.Create( HKEY_CLASSES_ROOT, szVal );
	rkDefIcon.Create( rkTheImp.m_hKey, "DefaultIcon" );

	_makepath( sz, "", pConfig->szAppPath, pConfig->szAppName, "exe" );
	sprintf( sz+strlen(sz), ",%i", pConfig->iRegisteredExtsIcon );
	rkDefIcon.SetValue( sz );

	rkShell.Create( rkTheImp.m_hKey, "shell" );
	rkShell.SetValue( "open" );

	rkOpen.Create( rkShell.m_hKey, "open" );
	rkCommand.Create( rkOpen.m_hKey, "command"  );

	char sztemp[_MAX_PATH*2];
	_makepath( sztemp, "", pConfig->szAppPath, pConfig->szAppName, "exe" );
	sprintf( sz, "%c%s%c", '"', sztemp, '"' );
	
	for (int i=1; i<10; i++ ) // "%1" "%2" "%3" "%4" "%5" "%6" "%7" "%8" "%9"
		sprintf( sz+strlen(sz),  " %c%c%i%c ", '"', '%', i, '"'  );
	rkCommand.SetValue(  sz );


	while ( *pszExts )
	{
		char *psz = sz; *psz++ = '.';
		while ( *pszExts && *pszExts!='|' ) *psz++ = *pszExts++; 
		*psz = 0; pszExts++;

		CRegKey rkExt;

		rkExt.Create( HKEY_CLASSES_ROOT, sz );

		DWORD dw = _MAX_PATH*2;
		char szCurrVal[_MAX_PATH*2]="";
		rkExt.QueryValue( szCurrVal, "", &dw );

		if ( strcmp(szCurrVal, szVal) )
		{
			rkExt.SetValue( szVal );
			rkExt.SetValue( szCurrVal, szPrevId );
		}
		
	}


	return TRUE;
}

// Unregister files and restore previous associated
//  pszExts must be in format "ext|ext|ext|ext".
BOOL UnregisterFiletypes( stConfig *pConfig, char *pszExts )
{
	CRegKey rkTheImp, rkDefIcon, rkShell, rkOpen, rkCommand;
	char szVal[] = "The Imp . Associated file",
		  szPrevId[] = "The Imp . Previous associated file",
		  sz[_MAX_PATH*2];

	while ( *pszExts )
	{
		char *psz = sz; *psz++ = '.';
		while ( *pszExts && *pszExts!='|' ) *psz++ = *pszExts++; 
		*psz = 0; pszExts++;

		CRegKey rkExt;

		rkExt.Create( HKEY_CLASSES_ROOT, sz );

		DWORD dw = _MAX_PATH*2;
		char szCurrVal[_MAX_PATH*2];
		rkExt.QueryValue( szCurrVal, "", &dw );

		if ( !strcmp(szCurrVal, szVal) )
		{
			char szPrevVal[_MAX_PATH*2]; dw = _MAX_PATH*2;
			int ret = rkExt.QueryValue( szPrevVal, szPrevId, &dw );

			if ( ret == ERROR_SUCCESS )
			{
				rkExt.SetValue( szPrevVal );
				rkExt.DeleteValue( szPrevId );
			}
		}	
		
	}

	return TRUE;
}