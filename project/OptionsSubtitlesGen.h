#if !defined(AFX_OPTIONSSUBTITLESGEN_H__69599490_F9DE_4BD0_B4C5_ED8639C529FF__INCLUDED_)
#define AFX_OPTIONSSUBTITLESGEN_H__69599490_F9DE_4BD0_B4C5_ED8639C529FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSubtitlesGen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitlesGen dialog

class COptionsSubtitlesGen : public CDialog
{
	stConfig *pOptions;

	CWnd	*pControlWnd;


	int		iSubtitlesRows,
				iSubtitlesRowsDistance, //[pixel]
				iSubtitlesMultiRowMode,
				iSubtitlesHideAfter;	//[seconds]
	double	valSubtitlesDistanceFromBottom; //[percent]
	BOOL		bSbtFontAutosize;



	void		InitDialog();
	void		SetDepedencies();
	void		InitLanguage();

// Construction
public:
	COptionsSubtitlesGen(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsSubtitlesGen)
	enum { IDD = IDD_PP_SUBTITLES_GENERAL };
	CStatic	m_TTTextRows;
	CStatic	m_TTTimeToHide;
	CStatic	m_TT2TimeToHide;
	CStatic	m_TT2RowDistance;
	CButton	m_TTTitleTimeProp;
	CStatic	m_TTRowDistance;
	CStatic	m_TTMRowDir;
	CButton	m_TTTitlePosition;
	CButton	m_TTTitleAdditional;
	CButton	m_TTTitle;
	CSpinButtonCtrl	m_SPHideAfter;
	CSpinButtonCtrl	m_SPTextRows;
	CSpinButtonCtrl	m_SPRowDistance;
	CEdit	m_EHideAfter;
	CEdit	m_ERowDistance;
	CComboBox	m_CMRowDir;
	CButton	m_CKAutosize;
	CButton	m_BDefault;
	CButton	m_BApply;
	CEdit	m_ETextRows;
	CScrollBar	m_SBPosition;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSubtitlesGen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsSubtitlesGen)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditTextrows();
	afx_msg void OnButtonDefault();
	afx_msg void OnButtonApply();
	afx_msg void OnChangeEditDistance();
	afx_msg void OnChangeEditTimetohide();
	afx_msg void OnCheckAutosize();
	afx_msg void OnSelchangeComboMrowdir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSUBTITLESGEN_H__69599490_F9DE_4BD0_B4C5_ED8639C529FF__INCLUDED_)
