#if !defined(AFX_OPTIONSOPTIONS_H__F7534EB2_6CEE_4E1B_9E6E_F69F8EAB69BE__INCLUDED_)
#define AFX_OPTIONSOPTIONS_H__F7534EB2_6CEE_4E1B_9E6E_F69F8EAB69BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsOptions dialog

class COptionsOptions : public CDialog
{
	stConfig	*pOptions;
	CWnd			*pControlWnd;


	DWORD		dwMilisecondsToHideMouse;
	int		maxPlaylistFiles;
	BOOL		bHidePLSOnLButtonUp;
	BOOL		bMaximizeOverTaskMenu;
	BOOL		bAlwaysOnTop;

	void		InitDialog();
	void		SetDepedencies();

	void		InitLanguage();

// Construction
public:
	COptionsOptions(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsOptions)
	enum { IDD = IDD_PP_OPTIONS };
	CStatic	m_TT2HideAfter;
	CButton	m_TTTitlePlaylist;
	CButton	m_TTTitleGeneral;
	CButton	m_TTTitle;
	CStatic	m_TTMaxPLSFiles;
	CStatic	m_TTHideAfter;
	CButton	m_CKAlwaysOnTop;
	CButton	m_CKHidePLSOnClick;
	CSpinButtonCtrl	m_SPMaxPLSFiles;
	CSpinButtonCtrl	m_SPHideAfter;
	CEdit	m_EMaxPLSFiles;
	CEdit	m_EHideAfter;
	CButton	m_CKMaxOverTask;
	CButton	m_BDefault;
	CButton	m_BApply;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsOptions)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditHideafter();
	afx_msg void OnChangeEditMaxplsfiles();
	afx_msg void OnCheckAlwaysontop();
	afx_msg void OnCheckHideplsmleft();
	afx_msg void OnCheckMaxovertask();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSOPTIONS_H__F7534EB2_6CEE_4E1B_9E6E_F69F8EAB69BE__INCLUDED_)
