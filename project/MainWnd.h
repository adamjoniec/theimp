#if !defined(AFX_MAINWND_H__F262BA4B_D5A7_4D51_B622_862509BF0649__INCLUDED_)
#define AFX_MAINWND_H__F262BA4B_D5A7_4D51_B622_862509BF0649__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainWnd.h : header file
//
#include "vcdplyer.h"
#include "skin.h"
#include "options.h"
#include "fullscreenwnd.h"
#include "tooltip.h"
#include "filterprop.h"
#include "subtitles.h"


/////////////////////////////////////////////////////////////////////////////
// CMainWnd window

#define VCD_PLAYING          0x0001
#define VCD_STOPPED          0x0002
#define VCD_PAUSED           0x0004
#define VCD_SKIP_F           0x0008
#define VCD_SKIP_B           0x0010
#define VCD_FF               0x0020
#define VCD_RW               0x0040
#define VCD_SEEKING          (VCD_FF | VCD_RW)
#define VCD_LOADED           0x0080
#define VCD_NO_CD            0x0100
#define VCD_DATA_CD_LOADED   0x0200
#define VCD_EDITING          0x0400
#define VCD_PAUSED_AND_MOVED 0x0800
#define VCD_PLAY_PENDING     0x1000
#define VCD_WAS_PLAYING      0x2000
#define VCD_IN_USE           0x4000
#define VCD_STEPPING         0x8000
#define VCD_OPENING			  0x10000


// Dirsct show send this message when state movie changed.
#define WM_GRAPHNOTIFY  WM_APP + 7

#define TIMER_GLOBAL		1

class CMainWnd : public CWnd
{
	enum SKINMODEID {
		MODE_NONE			= 0x0,
		MODE_NORMAL			= 0x1,
		MODE_COMPACT		= 0x2,
		MODE_FULLSCREEN	= 0x4,

		MODE_GO_WINDOWED				= 0x100,
		MODE_GO_FULLSCREEN			= 0x200,
		MODE_FULLSCREEN_SWITCH		= 0x400,

	};

	class cstr{ 
		char *sz;
		public:
		char *s; int l;
		cstr() { sz=NULL; }
		~cstr() { if (sz) delete[] sz; sz=NULL;}
		cstr &operator =(const cstr &str) {s=str.s; l=str.l; sz=NULL; return *this;}
		void set(char *ps, int len) { s = ps; l = len; }
		int cmp(char *psz) {
			if ( l==(int)strlen(psz) ) return _strnicmp(s, psz, l); 
			return 1;
		}
		int cmp(cstr *pcstr) { 
			if ( l==pcstr->l ) return _strnicmp(s, pcstr->s, l); 
			return 1;
		}
		char *psz() { if (sz) delete[] sz; sz=new char[l+1];
			sz[l] = 0; strncpy(sz, s, l); return sz; }
	};
	int cfg_parse_element(char **buff, cstr *opt, int *opts, int *level );

	CFilterProp *pFiltersProp;

	HDC hdcBackground;
	//DWORD dwWidth, dwHeight;
	SIZE	sizeWnd;

	OPENFILENAME        ofn;
	stConfig *pOptions;

	DWORD timeWindowDelay;		//window reaction delay (default 15 ms)
	DWORD timeWindowReaction;	//used to keep last window reaction time


	//mouse and window variables
	DWORD timeMouseMove;

	CTooltip *pTooltip;

	CMenu		*pMenu;
	BOOL		bQuickMenu;

	cGImage	*pImgMenu;

	SIZE		sizeScreen, sizeMaximized;

	
	BOOL pressedLeft, pressedRight, pressedMiddle;
	POINT ptScreen, ptClient;
	BOOL bMovingWindow, bMaximized;
	RECT rcMaximized;	// rect used to restore window size and pos when restore from maximize
	int idButtonPress, idButtonOver;
	int idScrollbarPress, idScrollbarOver;

	


public:
	CPlaylist	*pPlaylist;
	CSubtitles	*pSubtitles;
	COptions		*pDlgOptions;

	CMainWnd();

	CWnd		*pSubtitlesWnd;


	// movie var.
	double	dMoviePosition;

	int		timerId;

	BOOL		bAllowHideMouse, bMouseHidden;

	
	CFullscreenWnd *pFullscreenWnd;

	int modeId, prevModeId;
	CSkin *pskinNormal, *pskinCompact, *pskinFullscreen,
			*pSkin, *pprevSkin;

	BOOL bMultipleFiles, bSubtitles;

	char szError[256];

	CMpegMovie *pMovie;
	DWORD stateMovie;
	HWND hWndMovie;


	BOOL		bShowPlaylist;


	int LoadXMLSkin();

	enum	MOUSEEVENTID {
		MOUSE_LBUTTON_DOWN = 0,
		MOUSE_LBUTTON_MOVE,
		MOUSE_LBUTTON_UP,
		MOUSE_LBUTTON_UP_NOT_OVER_BUTTON,
		MOUSE_OVER,
		MOUSE_OVER_BEGIN,
		MOUSE_OVER_END
	};

	// Used when going fullscreen to remember window state
	BOOL	bStoredFsAllowResize, bStoredMaximized;
	RECT	rcStoredFsPos, // store here fullscreen window size and pos (used when window can movie in fullscreen mode)
			rcStoredWindowedPos;	// store here window size and pos when mode is in mode WINDOWEDand go FULLSCREEN

	// Should be defined in skin.
	// If constant position skin in fs mode can't move, 
	//  and goes into left-bottom corner (def. TRUE).
	BOOL bFsConstPos;


	BOOL	SetMode( int skinModeId );

	BOOL	Maximize( BOOL maximize );
	void	OnWindowMove(BOOL bMoved);
	void	OnButton( int id, int mouseEventId );
	void	OnScrollbar( int id, int mouseEventId );

	void	OnWindowMinimize();
	void	OnWindowMaximize();
	void	OnWindowResize( int mouseEventId );

	void	OnWindowClose();
	void	OnPlayerOpen();
	void	OnPlayerOpenSubtitles();
	void	OnPlayerClose();
	void	OnPlayerPlay();
	void	OnPlayerPause();
	void	OnPlayerPlayPause();
	void	OnPlayerStop();
	void	OnPlayerVolumeOnOff();

	void	OnPlayerRewindForward();
	void	OnPlayerRewindBack();

	void	OnPlayerNext();
	void	OnPlayerPrev();

	void	OnPlayerFullscreenWindowed();
	void	OnPlayerModeNormalCompact();

	void	OnPlayerSubtitlesSwitch();
	void	OnPlayerPanScanSwitch();
	void	OnPlayerPanScanMode(double val);
	void	OnPlayerPlaylistSwitch();
	void	OnPlayerOptions(int idd=-1);


	void	OnPlayerVolume();								//scrollbar
	void  OnPlayerPosition( int mouseEventId );	//scrollbar

	void	OnPlayerProperties();

	//movie manipulation
	void	OpenMovie( TCHAR *achFileName, BOOL bPlay=FALSE );
	void	CloseMovie();

	void	OpenSubtitles( char *pszFileName, BOOL bAutoSearch=TRUE );
	void	SaveSubtitles();

	void	PlayMovie();
	void	PauseMovie();
	void	StopMovie();

	void	SetVolume( double vol, int wheelDelta=0 );

	void	OnMovieNotify();
	
	
	void	RepositionMovie();

	void	UpdateStatus(BOOL bUpdStatus=TRUE, BOOL bUpdTime=TRUE);

	void	UpdateButtons( BOOL bRedraw=TRUE, BOOL bForceRedrawAll=FALSE );
	void	UpdateMenu();

	void	AllowHideMouse(BOOL bAllow);
	void	ShowMouse( BOOL bShow );
	void	LockMouseHide();
	void	UnlockMouseHide();

	BOOL	SetMenuBitmap(char *pszId, cGImage *pimg, int x, int y, 
		int xChecked=-1, int yChecked=-1, BOOL bExistChecked=FALSE);

	void ShowPlaylist(BOOL bShow);

	void OnExitMenuLoop( BOOL bIsTrackPopupMenu );
	void OnDisplayChange( int cxScreen, int cyScreen, int cBitsPerPixel );


	void	InitLanguage();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainWnd)
	public:
	virtual BOOL Create( char *szWindowName, CWnd* pParentWnd, stConfig *pOpt, CSubtitles *pSubt);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
	
public:
	virtual ~CMainWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMainWnd)
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized );
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINWND_H__F262BA4B_D5A7_4D51_B622_862509BF0649__INCLUDED_)
