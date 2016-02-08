#if !defined(AFX_FILTERPROP_H__DA055408_A8AD_4CB3_B20B_EC51C8932118__INCLUDED_)
#define AFX_FILTERPROP_H__DA055408_A8AD_4CB3_B20B_EC51C8932118__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterProp.h : header file
//

#include "config.h"
#include "vcdplyer.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterProp dialog

class CFilterProp : public CDialog
{
	CMpegMovie	*pMovie;
	stConfig	*pOptions,
					*pOptionsTemp;

	IGraphBuilder	*pGB;

	void InitLanguage();

// Construction
public:
	CFilterProp(stConfig *poptions, CMpegMovie *pmovie, CWnd* pParent = NULL);   // standard constructor
	~CFilterProp();

	void SetColorControl();


// Dialog Data
	//{{AFX_DATA(CFilterProp)
	enum { IDD = IDD_DIALOG_FILTERSPROP };
	CButton	m_BDefault;
	CButton	m_BCancel;
	CButton	m_BOK;
	CStatic	m_TTWinXPOnly;
	CButton	m_TTTitleFilters;
	CButton	m_TTTitlePicture;
	CStatic	m_TTSaturation;
	CStatic	m_TTContrast;
	CStatic	m_TTBrightness;
	CButton	m_Apply;
	CStatic	m_stcSaturation;
	CStatic	m_stcContrast;
	CStatic	m_stcBrightness;
	CSliderCtrl	m_Saturation;
	CSliderCtrl	m_Contrast;
	CSliderCtrl	m_Brightness;
	CButton	m_ButtonProp;
	CListBox	m_ListFilters;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HRESULT EnumFilters();
	IBaseFilter *FindFilterFromName(LPTSTR szNameToFind);
	void ShowProperties();

	// Generated message map functions
	//{{AFX_MSG(CFilterProp)
	afx_msg void OnButtonProp();
	afx_msg void OnDblclkListFilters();
	afx_msg void OnSelchangeListFilters();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnButtonDef();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERPROP_H__DA055408_A8AD_4CB3_B20B_EC51C8932118__INCLUDED_)
