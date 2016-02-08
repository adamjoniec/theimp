#if !defined(AFX_OPTIONSABOUT_H__005EEB7A_9784_464E_9A69_01FBD218536D__INCLUDED_)
#define AFX_OPTIONSABOUT_H__005EEB7A_9784_464E_9A69_01FBD218536D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsAbout.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsAbout dialog

class COptionsAbout : public CDialog
{
	stConfig		*pOptions;
	CWnd			*pControlWnd;

	HBITMAP hBitmap;
	RECT rcDest;
	HFONT hFont;

	int iLast;

	cGImage *pimgAbout, *pimgAboutScroller,
		*pimgAboutMail, *pimgAboutRegisterIt, *pimgAboutWWW;

	void	DrawAbout(HDC hdc);

//	void CALLBACK OnMTimer( UINT uID, UINT uMsg,     
//						DWORD dwUser, DWORD dw1, DWORD dw2 );
	MMRESULT mmTimer;

// Construction
public:
	COptionsAbout(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent, 
		cGImage *pimgAbout,cGImage  *pimgAboutScroller,
		cGImage *pimgAboutWWW, cGImage *pimgAboutRegisterIt, cGImage *pimgAboutMail );   // standard constructor

	void	DrawScroller( HDC hdc );
// Dialog Data
	//{{AFX_DATA(COptionsAbout)
	enum { IDD = IDD_PP_ABOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsAbout)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsAbout)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSABOUT_H__005EEB7A_9784_464E_9A69_01FBD218536D__INCLUDED_)
