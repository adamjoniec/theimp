#if !defined(AFX_SUBTITLES_H__45D00EC7_2B66_488E_BBF1_679F5E3951E1__INCLUDED_)
#define AFX_SUBTITLES_H__45D00EC7_2B66_488E_BBF1_679F5E3951E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Subtitles.h : header file
//

#include "vcdplyer.h"
#include "config.h"
#include "subtitlesfs.h"

#define TIMER_SUBTITLES		2

/////////////////////////////////////////////////////////////////////////////
// CSubtitles window


class CSubtitles : public CWnd
{

	// Keep one subtitles text line with time in ad out.
	struct stSubtitle{
		DWORD dwType;
		DWORD dwFrameIn, dwFrameOut;
		float fTimeIn, fTimeOut, fconstTimeIn, fconstTimeOut;
		char *psz;
	};
	DWORD dwSubtitlesNum, dwSubtitlesType;
	stSubtitle *pSubtitles, *psbCurrent;

	char *pszText;		//subtitles text file
	char *psz;			//pointer to current position of char in text
	BOOL ReadNumber(DWORD *num, BOOL bReturn = FALSE);
	char *EndLine(char *sz);


	stConfig	*pOptions;
	CMpegMovie	*pMovie;

	RECT			rcMoviePos;

	HFONT				hFont;
	HBITMAP			hbmBackground;
	//CSubtitlesFs	*pSubtitlesFs;
	CWnd				*pThis;
	int				iFontOutlinePix;


	int			timerId;
	BOOL			bLoaded, bEnabled, bFullscreen;


	// Subtitles state, duplicated from Options.
	//  subtitles state
	int		iSubtitlesMode, iSubtitlesRows,
				iSubtitlesRowsDistance, //[pixel]
				iSubtitlesMultiRowMode,
				iSubtitlesHideAfter;
	BOOL		bSubtitlesBkgrndShort,
				bSubtitlesRgnPrecise;
	double	valSubtitlesDistanceFromBottom; //[percent]
	//  subtitels font state
	char		*pszFontName;
	int		iFontSize, iFontWeight, iFontOutline;
	BOOL		bFontAutosize, bFontBkgrndTransparent;
	DWORD		colFont, colFontBkgrnd, colFontOutline;

	BOOL		bSubtitlesOffsetTime;
	float		valSubtitlesOffsetTime;



	void	CreateBackground(int cx=0, int cy=0);
	void	CreateBkgrndUsingOverlay(int cx, int cy);
	void	CreateBkgrndUsingRgn(int cx, int cy);
	void	CreateBkgrndUsingVMR(int cx, int cy);

	void	ShowBackground( BOOL bShow );

	// Convert subtitles from frames to seconds.
	void	ConvertSubtitles(double fps);

	// Find correct subtitles text line depending on current
	//  movie position.
	void	CheckSubtitles(double time);

// Construction
public:
	CSubtitles();

	CSubtitlesFs	*pSubtitlesFs;

	// Set movie when loaded to get correct fps, duration, 
	//  current position, ...
	void	SetMovie( CMpegMovie *pMovie );

	// Read subtitles, before calling it call SetMovie with valid movie.
	BOOL ReadSubtitles(char *psz);
	// Delete current subtitles.
	void DeleteSubtitles();

	// Reposition subtitles when window move or resized.
	void Reposition(int x, int y, int cx, int cy, BOOL bForceRedraw=FALSE);

	// Switch subtiles beetween windowed and fullscreen subtitles.
	void	SetFullscreen( BOOL bFs );

	// Enable or disable subtitles visibility.
	void	EnableSubtitles( BOOL bEnable );

	// Call this when subtitles option(s) has changed.
	void	UpdateOptions();

	// Delay or speed up showing subtitles ( before calling this you
	//  may want call UpdateOptions().
	void	OffsetSubtitles();

	BOOL	WriteOffsetedSubtitles( const char *pszFileName );

	void	UpdateVerticalPos();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubtitles)
	public:
	virtual BOOL Create( stConfig *pOpt, CSubtitlesFs *pSubtitlesFs, CWnd* pParentWnd=NULL, CMpegMovie *pMovie=NULL );
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSubtitles();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSubtitles)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBTITLES_H__45D00EC7_2B66_488E_BBF1_679F5E3951E1__INCLUDED_)
