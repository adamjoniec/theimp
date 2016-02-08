#if !defined(AFX_TIMEOUT_H__B4C526B8_9CC0_45FF_93CD_1CC23EC8295A__INCLUDED_)
#define AFX_TIMEOUT_H__B4C526B8_9CC0_45FF_93CD_1CC23EC8295A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeOut.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeOut dialog

class CTimeOut : public CDialog
{
	CFont *pFont;
// Construction
public:
	CTimeOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimeOut)
	enum { IDD = IDD_DIALOG_TIMEOUT };
	CStatic	m_TCaption;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeOut)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeOut)
	afx_msg void OnButtonRegister();
	afx_msg void OnButtonTheimp();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEOUT_H__B4C526B8_9CC0_45FF_93CD_1CC23EC8295A__INCLUDED_)
