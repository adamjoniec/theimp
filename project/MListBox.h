#if !defined(AFX_MLISTBOX_H__801A7ECC_1B1E_4E95_888D_919FB1F3A1B6__INCLUDED_)
#define AFX_MLISTBOX_H__801A7ECC_1B1E_4E95_888D_919FB1F3A1B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MListBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMListBox window

class CMListBox : public CListBox
{

// Construction
public:
	CMListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMListBox)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMListBox();


	// Generated message map functions
protected:
	//{{AFX_MSG(CMListBox)
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MLISTBOX_H__801A7ECC_1B1E_4E95_888D_919FB1F3A1B6__INCLUDED_)
