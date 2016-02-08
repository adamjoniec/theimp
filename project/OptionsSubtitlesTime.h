#if !defined(AFX_OPTIONSSUBTITLESTIME_H__71E90709_5709_4AA2_A9D2_06E54F980E19__INCLUDED_)
#define AFX_OPTIONSSUBTITLESTIME_H__71E90709_5709_4AA2_A9D2_06E54F980E19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSubtitlesTime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitlesTime dialog

class COptionsSubtitlesTime : public CDialog
{
	stConfig	*pOptions;
	CWnd			*pControlWnd;

	BOOL		bSubtitlesLoaded, bSubtitlesOffsetTime;
	float		valSubtitlesOffsetTime;

	void		InitDialog();
	void		SetDepedencies();
	void		InitLanguage();

// Construction
public:
	COptionsSubtitlesTime(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsSubtitlesTime)
	enum { IDD = IDD_PP_SUBTITLES_TIME };
	CButton	m_TTTitleOffset;
	CButton	m_TTTitle;
	CButton	m_RSpeedUp;
	CButton	m_RDelay;
	CStatic	m_TT2Time;
	CStatic	m_TTTime;
	CDateTimeCtrl	m_DTTime;
	CButton	m_CKOffset;
	CButton	m_BSaveAs;
	CButton	m_BDefault;
	CButton	m_BApply;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSubtitlesTime)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsSubtitlesTime)
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDefault();
	afx_msg void OnButtonSaveas();
	afx_msg void OnCheckOffset();
	afx_msg void OnDatetimechangeDatetimeTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRadioDelay();
	afx_msg void OnRadioSpeedup();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSUBTITLESTIME_H__71E90709_5709_4AA2_A9D2_06E54F980E19__INCLUDED_)
