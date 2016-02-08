// XPPlayerDlg.h : header file
//

#if !defined(AFX_XPPLAYERDLG_H__76E4288C_42DB_4321_A16E_968ED64D62BD__INCLUDED_)
#define AFX_XPPLAYERDLG_H__76E4288C_42DB_4321_A16E_968ED64D62BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CXPPlayerDlg dialog

class CXPPlayerDlg : public CDialog
{
// Construction
public:
	CXPPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXPPlayerDlg)
	enum { IDD = IDD_XPPLAYER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXPPlayerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CXPPlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPPLAYERDLG_H__76E4288C_42DB_4321_A16E_968ED64D62BD__INCLUDED_)
