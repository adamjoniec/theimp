
#if !defined ( FILE_LANGUAGE_H )
#define FILE_LANGUAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum TRANSL_SECTIONS {
	TRANSL_NEXTSECTION = 0x1000000,
	TRANSL_MOVIESTATE,
	TRANSL_TOOLTIPS ,
	TRANSL_QMENU,
	TRANSL_QMENU_PLS,
	TRANSL_KEYS,
	TRANSL_OPTIONS,
	TRANSL_PP_SETUP,
	TRANSL_PP_FILETYPES,
	TRANSL_PP_OPTIONS,
	TRANSL_PP_PLAY,
	TRANSL_PP_STARTUP,
	TRANSL_PP_SUBTITLES,
	TRANSL_PP_SUBTITLES_GENERAL,
	TRANSL_PP_SUBTITLES_TIME,
	TRANSL_PP_SKINBROWSER,
	TRANSL_FILTERPROP,
	TRANSL_END
};

char *transl( int id, int section );


void InitLanguage();
BOOL IsLang( char *pszFileName, CString &retLanguage, CString &retAuthor );
BOOL ChangeLang( char *pszFileName );

#define TRANSLW( window, id, section ) \
	window.SetWindowText( transl(id, section) );

#endif // !defined ( FILE_LANGUAGE_H )