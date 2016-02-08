#if !defined(AFX_SPLASHSCREEN_H__F48E6CE5_C82D_4457_8D0F_246F20BAF844__INCLUDED_)
#define AFX_SPLASHSCREEN_H__F48E6CE5_C82D_4457_8D0F_246F20BAF844__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashScreen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplashScreen window

class CSplashScreen : public CWnd
{
// Construction
public:
	CSplashScreen();

	cGImage *pimg;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashScreen)
	public:
	virtual BOOL Create();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplashScreen();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplashScreen)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHSCREEN_H__F48E6CE5_C82D_4457_8D0F_246F20BAF844__INCLUDED_)
