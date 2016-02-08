#if !defined(AFX_TOOLTIP_H__6D971432_5E7B_4FDC_9886_424944093F3E__INCLUDED_)
#define AFX_TOOLTIP_H__6D971432_5E7B_4FDC_9886_424944093F3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Tooltip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTooltip window

class CTooltip : public CWnd
{
	CFont	Font;
	char	szToolTip[32];
	BOOL	bDrawEnabled, bPainted;
	SIZE	size;

	HWND	hWndParent;

// Construction
public:

	CTooltip(CWnd *pWndParent, char *szTip, BOOL bEnabled);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTooltip)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTooltip();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTooltip)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIP_H__6D971432_5E7B_4FDC_9886_424944093F3E__INCLUDED_)
