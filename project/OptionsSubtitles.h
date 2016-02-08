#if !defined(AFX_OPTIONSSUBTITLES_H__B2EB8F9D_1D54_4F2C_A4E9_2427BBDC99F4__INCLUDED_)
#define AFX_OPTIONSSUBTITLES_H__B2EB8F9D_1D54_4F2C_A4E9_2427BBDC99F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSubtitles.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsSubtitles dialog

class COptionsSubtitles : public CDialog
{
	stConfig	*pOptions;
	CWnd			*pControlWnd;

	HFONT			hFont;
	LOGFONT		LogFont;

	// Duplicated from pOptions.
	int		iSubtitlesMode;
	BOOL		bSubtitlesBkgrndShort,
				bSubtitlesRgnPrecise;
	char		szSbtFontName[32];
	int		iSbtFontSize, iSbtFontWeight, iSbtFontOutline;
	BOOL		bSbtFontBkgrndTransparent;
	DWORD		colSbtFont, colSbtFontBkgrnd, colSbtFontOutline;


	void		InitDialog();
	void		SetDepedencies();
	void		DrawExampleText( HDC hDC=NULL, BOOL bChanged=FALSE );
	void		InitLanguage();


	char szExampleText[64];

// Construction
public:
	COptionsSubtitles(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsSubtitles)
	enum { IDD = IDD_PP_SUBTITLES };
	CStatic	m_TOutlinecol;
	CButton	m_TTTitle;
	CButton	m_CKTranspBkgrnd;
	CComboBox	m_CModesel;
	CComboBox	m_COutlinesel;
	CStatic	m_TBkgrndcol;
	CStatic	m_TTOutlinesel;
	CStatic	m_TTModesel;
	CStatic	m_TFontcol;
	CButton	m_CKShortbkgrnd;
	CButton	m_CKRgnprecise;
	CButton	m_BDefault;
	CButton	m_BChangeFont;
	CButton	m_BApply;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSubtitles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsSubtitles)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnCheckRgnprecise();
	afx_msg void OnCheckShortbkgrnd();
	afx_msg void OnCheckTransparentbkgrnd();
	afx_msg void OnSelchangeComboModesel();
	afx_msg void OnSelchangeComboOutlinesel();
	afx_msg void OnButtonChangefont();
	afx_msg void OnTextBkgrndcol();
	afx_msg void OnTextFontcol();
	afx_msg void OnTextOutlinecol();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSUBTITLES_H__B2EB8F9D_1D54_4F2C_A4E9_2427BBDC99F4__INCLUDED_)
