#if !defined(AFX_OPTIONSSKINBROWSER_H__95B4683F_4FA0_4884_9A90_025215C38358__INCLUDED_)
#define AFX_OPTIONSSKINBROWSER_H__95B4683F_4FA0_4884_9A90_025215C38358__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsSkinBrowser.h : header file
//
#include "xmlparser.h"

/////////////////////////////////////////////////////////////////////////////
// COptionsSkinBrowser dialog

class COptionsSkinBrowser : public CDialog
{
	cGImage *pPreview;
	stConfig *pOptions;

	int iCurSel;

	void	DrawPreview( HDC hDC=NULL );

	void	InitLanguage();

	CWnd	*pControlWnd;
// Construction
public:
	COptionsSkinBrowser(stConfig *pOpt, CWnd *pControlWnd, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsSkinBrowser)
	enum { IDD = IDD_PP_SKINBROWSER };
	CListBox	m_ListSkins;
	CStatic	m_TTAuthor;
	CStatic	m_TTComment;
	CButton	m_TTTitleSkinInfo;
	CStatic	m_TVersion;
	CStatic	m_TPrevNotAvail;
	CStatic	m_TName;
	CStatic	m_TComment;
	CStatic	m_TAuthor;
	CButton	m_BSelectSkin;
	CStatic	m_TTVersion;
	CStatic	m_TTName;
	CButton	m_TTTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSkinBrowser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsSkinBrowser)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSblistSkins();
	afx_msg void OnDblclkSblistSkins();
	afx_msg void OnPaint();
	afx_msg void OnButtonSelectskin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSKINBROWSER_H__95B4683F_4FA0_4884_9A90_025215C38358__INCLUDED_)
