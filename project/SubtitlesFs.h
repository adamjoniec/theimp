#if !defined(AFX_SUBTITLESFS_H__517A1FE5_98F2_4441_BEC5_041E912FEA80__INCLUDED_)
#define AFX_SUBTITLESFS_H__517A1FE5_98F2_4441_BEC5_041E912FEA80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubtitlesFs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSubtitlesFs window

enum SUBTITLESMODE {
	SUBTITLES_VMR = 0,
	SUBTITLES_OVERLAY,
	SUBTITLES_RGN
	
};

enum FONTOUTLINE {
	FO_BOLD = 0,
	FO_THIN,
	FO_NONE
};

// Define if first row of subtitles text goes from TOP to BOTTOM,
//  or from BOTTOM to TOP.
enum SUBTITLESMRMODE {
	SUBTITLESMR_TOP_BOTTOM = 0,
	SUBTITLESMR_BOTTOM_TOP
};


class CSubtitlesFs : public CWnd
{
	HBITMAP	*phbmBackground;
	int		*pSubtitlesMode;

// Construction
public:
	CSubtitlesFs();

// Attributes
public:

// Operations
public:

	void SetBitmapBackground( HBITMAP *phbmBckgrnd )
	{ phbmBackground = phbmBckgrnd; }
	void SetMode( int *pMode ) { pSubtitlesMode = pMode; };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubtitlesFs)
	public:
	virtual BOOL Create(CWnd* pParentWnd);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSubtitlesFs();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSubtitlesFs)
	afx_msg void OnPaint();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBTITLESFS_H__517A1FE5_98F2_4441_BEC5_041E912FEA80__INCLUDED_)
