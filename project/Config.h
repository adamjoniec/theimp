
#if !defined ( FILE_CONFIG_H )
#define FILE_CONFIG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct stConfig {

	DWORD		BEGIN_CONFIG;

	DWORD		dwId;
	DWORD		dwVersionId;

	char		szAppName[32];
	char		szAppPath[_MAX_PATH];

	BOOL		bRegisterOnStart;
	char		szRegisteredExts[4096];
	int		iRegisteredExtsIcon;
	

	char		szSkinDirectory[_MAX_PATH];
	char		szSkinFileName[_MAX_PATH];
	BOOL		bSkinInZIP;

	RECT		rcWindowPos;

	BOOL		bFastSeek, bFastRewind, bAutoRewind, bLoop;

	double	dVolume;

	BOOL		bAutoPlay;
	BOOL		bMuteVolume;

	double	dRewindSecs;

	int		modeId, prevModeId;

	DWORD		dwMilisecondsToHideMouse;

	int		maxPlaylistFiles;
	char		szLastMoviePath[_MAX_PATH];

	BOOL		bShowSubtitles;
	BOOL		bPanScanEnabled;
	double	valPanScan;

	BOOL		bHidePLSOnLButtonUp;

	BOOL		bMaximizeOverTaskMenu;

	BOOL		bAlwaysOnTop;

	int		modeIdStartWhenMovie,
				modeIdStartWhenNoMovie;

	int		valBrightness, valContrast, valSaturation;

	///////////////////////
	// Subtitles options //
	///////////////////////
	//  subtitles state
	int		iSubtitlesMode, iSubtitlesRows,
				iSubtitlesRowsDistance, //[pixel]
				iSubtitlesMultiRowMode,
				iSubtitlesHideAfter;	//[seconds]
	BOOL		bSubtitlesBkgrndShort,
				bSubtitlesRgnPrecise;
	double	valSubtitlesDistanceFromBottom; //[percent]

	//  subtitles font state
	char		szSbtFontName[32];
	int		iSbtFontSize, iSbtFontWeight, iSbtFontOutline;
	BOOL		bSbtFontAutosize, bSbtFontBkgrndTransparent;
	DWORD		colSbtFont, colSbtFontBkgrnd, colSbtFontOutline;

	char		szLastSubtitlesPath[_MAX_PATH];

	char		szLanguageDirectory[_MAX_PATH];
	char		szLanguageFileName[_MAX_PATH];

	DWORD		END_CONFIG;

	// subtitles time offset, always
	// used only to communitace with options
	BOOL		bSubtitlesLoaded;
	BOOL		bSubtitlesOffsetTime;
	float		valSubtitlesOffsetTime;

};

extern stConfig defConfig;

BOOL	ReadConfig( char *pszFileName, stConfig *pConfig );
BOOL	WriteConfig( char *pszFileName, stConfig *pConfig );

BOOL	RegisterFiletypes( stConfig *pConfig, char *pszExts );
BOOL	UnregisterFiletypes( stConfig *pConfig, char *pszExts );

#endif // !defined ( FILE_CONFIG_H )