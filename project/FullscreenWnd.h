#if !defined(AFX_FULLSCREENWND_H__D6037264_7060_40F1_9949_1D9CF82AC786__INCLUDED_)
#define AFX_FULLSCREENWND_H__D6037264_7060_40F1_9949_1D9CF82AC786__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FullscreenWnd.h : header file
//
#define VCD_PLAYING          0x0001
#define VCD_LOADED           0x0080

/////////////////////////////////////////////////////////////////////////////
// CFullscreenWnd window

class CFullscreenWnd : public CWnd
{

	HWND hwndParent;
	BOOL bUpdateBackground;
	BOOL bKeepInitMouseMsg;

	DWORD *pmovieState;

// Construction
public:
	CFullscreenWnd();

// Attributes
public:

// Operations
public:

	BOOL	ShowWindow( int nCmdShow );
	CWnd*	SetParent( CWnd* pWndNewParent );
	void	UpdateBackground(BOOL bUpdate);

	void	PutInfoMovieState( DWORD *pMovieState );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFullscreenWnd)
	public:
	virtual BOOL Create( LPCTSTR lpszWindowName, CWnd* pParentWnd);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFullscreenWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFullscreenWnd)
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FULLSCREENWND_H__D6037264_7060_40F1_9949_1D9CF82AC786__INCLUDED_)
