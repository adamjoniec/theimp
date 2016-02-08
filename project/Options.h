
#if !defined ( FILE_OPTIONS_H )
#define FILE_OPTIONS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "optionsskinbrowser.h"
#include "optionssubtitles.h"
#include "optionssubtitlesgen.h"
#include "optionsabout.h"
#include "optionsfiletypes.h"
#include "optionsoptions.h"
#include "optionsplay.h"
#include "optionssetup.h"
#include "optionssubtitlestime.h"
#include "optionsstartup.h"

// COptions dialog

class COptions : public CDialog
{
	CDialog		*pPropertyPage;
	stConfig		*pOptions;
	int			idPropertyPage;

	CWnd			*pControlWnd;
	int			idStartPropertyPage;

	void	InitLanguage();

	// 'About' variables.
	cGImage *pimgAbout, *pimgAboutScroller,
		*pimgAboutMail, *pimgAboutRegisterIt, *pimgAboutWWW;

// Construction
public:
	COptions(stConfig *pOpt, CWnd *pControlWnd, int idStartPropertyPage=-1, CWnd* pParent = NULL);   // standard constructor

	void ShowOptions( int idStartPropertyPage=-1 );

// Dialog Data
	//{{AFX_DATA(COptions)
	enum { IDD = IDD_DIALOG_OPTIONS };
	CButton	m_BClose;
	CTreeCtrl	m_TreePage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptions)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreePage(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonClose();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined ( FILE_OPTIONS_H )/////////////////////////////////////////////////////////////////////////////
