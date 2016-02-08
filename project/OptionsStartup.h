#if !defined(AFX_OPTIONSSTARTUP_H__8AF659EB_A765_46FD_89B6_6E6D8541F3CC__INCLUDED_)
#define AFX_OPTIONSSTARTUP_H__8AF659EB_A765_46FD_89B6_6E6D8541F3CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsStartup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsStartup dialog


class COptionsStartup : public CDialog
{
	stConfig	*pOptions;
	CWnd			*pControlWnd;

	void		InitDialog();
	void		SetDepedencies();
	void		InitLanguage();

	int		modeIdStartWhenMovie,
				modeIdStartWhenNoMovie;

	enum SKINMODEID {
		MODE_NONE			= 0x0,
		MODE_NORMAL			= 0x1,
		MODE_COMPACT		= 0x2,
		MODE_FULLSCREEN	= 0x4,
	};

// Construction
public:
	COptionsStartup(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsStartup)
	enum { IDD = IDD_PP_STARTUP };
	CStatic	m_TT2OpenWithMovie;
	CStatic	m_TT2OpenNoMovie;
	CButton	m_TTTitle;
	CStatic	m_TTOpenWithMovie;
	CStatic	m_TTOpenNoMovie;
	CComboBox	m_COpenWithMovie;
	CComboBox	m_COpenNoMovie;
	CButton	m_BDefault;
	CButton	m_BApply;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsStartup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsStartup)
	afx_msg void OnSelchangeComboOpennomovie();
	afx_msg void OnSelchangeComboOpenwithmovie();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSTARTUP_H__8AF659EB_A765_46FD_89B6_6E6D8541F3CC__INCLUDED_)
