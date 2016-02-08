#if !defined(AFX_OPTIONSPLAY_H__FEC62123_D314_42AF_B1E5_4FFA46FDDF07__INCLUDED_)
#define AFX_OPTIONSPLAY_H__FEC62123_D314_42AF_B1E5_4FFA46FDDF07__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsPlay.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsPlay dialog

class COptionsPlay : public CDialog
{
	stConfig	*pOptions;
	CWnd			*pControlWnd;


	BOOL		bFastSeek, bFastRewind, 
				bAutoRewind, bLoop, bAutoPlay;
	double	dRewindSecs;


	void		InitDialog();
	void		SetDepedencies();

	void		InitLanguage();

// Construction
public:
	COptionsPlay(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsPlay)
	enum { IDD = IDD_PP_PLAY };
	CButton	m_TTTitle;
	CStatic	m_TT2RewindBy;
	CButton	m_TTTitleSeeking;
	CButton	m_TTTitleGeneral;
	CStatic	m_TTRewindBy;
	CStatic	m_TTFastSeek;
	CSpinButtonCtrl	m_SPRewind;
	CEdit	m_ERewind;
	CButton	m_CKLoop;
	CButton	m_CKFastseek;
	CButton	m_CKAutorewind;
	CButton	m_CKAutoplay;
	CButton	m_BDefault;
	CButton	m_BApply;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsPlay)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsPlay)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDefault();
	afx_msg void OnCheckAutoplay();
	afx_msg void OnCheckAutorewind();
	afx_msg void OnCheckFastseek();
	afx_msg void OnCheckLoop();
	afx_msg void OnChangeEditRewindby();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSPLAY_H__FEC62123_D314_42AF_B1E5_4FFA46FDDF07__INCLUDED_)
