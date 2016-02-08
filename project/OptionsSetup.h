#if !defined(AFX_OPTIONSSETUP_H__D4D19A0E_F7FB_4269_8D64_3D7A5CEB9445__INCLUDED_)
#define AFX_OPTIONSSETUP_H__D4D19A0E_F7FB_4269_8D64_3D7A5CEB9445__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSetup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsSetup dialog

class COptionsSetup : public CDialog
{
	stConfig	*pOptions;
	CWnd			*pControlWnd;

	char		*pszCurrLangFile;

	void		InitDialog();
	void		SetDepedencies();

	void		InitLanguage();

// Construction
public:
	COptionsSetup(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsSetup)
	enum { IDD = IDD_PP_SETUP };
	CButton	m_TTTitleCurrLang;
	CButton	m_TTTitle;
	CStatic	m_TTTranslAuthor;
	CButton	m_BDefault;
	CButton	m_BApply;
	CComboBox	m_CCurrLang;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsSetup)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeComboCurrlang();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSETUP_H__D4D19A0E_F7FB_4269_8D64_3D7A5CEB9445__INCLUDED_)
