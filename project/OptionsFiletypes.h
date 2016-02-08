#if !defined(AFX_OPTIONSFILETYPES_H__111CCDD5_929A_461F_AB0B_88A1953D78EC__INCLUDED_)
#define AFX_OPTIONSFILETYPES_H__111CCDD5_929A_461F_AB0B_88A1953D78EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsFiletypes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsFiletypes dialog

class COptionsFiletypes : public CDialog
{
	stConfig		*pOptions;
	CWnd			*pControlWnd;

	HICON			hIcon;

	BOOL		bRegisterOnStart;
	int		iRegisteredExtsIcon;

	void		InitIcon();
	void		InitDialog();
	void		SetDepedencies();

	void		InitLanguage();

// Construction
public:
	COptionsFiletypes(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsFiletypes)
	enum { IDD = IDD_PP_FILETYPES };
	CStatic	m_TTRegisterOnStart;
	CButton	m_TTTitle;
	CButton	m_TTTitleAssociated;
	CButton	m_TTTitleIcon;
	CSliderCtrl	m_SIcon;
	CStatic	m_STIcon;
	CListBox	m_LExtensions;
	CButton	m_CKRegisterOnStart;
	CButton	m_BSelectNone;
	CButton	m_BDefault;
	CButton	m_BApply;
	CButton	m_BSelectAll;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsFiletypes)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsFiletypes)
	afx_msg void OnButtonAll();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDefault();
	afx_msg void OnButtonNone();
	afx_msg void OnCheckRegisteronstart();
	afx_msg void OnTexttRegisteronstart();
	afx_msg void OnSelchangeListExtensions();
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSFILETYPES_H__111CCDD5_929A_461F_AB0B_88A1953D78EC__INCLUDED_)
