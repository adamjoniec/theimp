#if !defined(AFX_PLAYLIST_H__CDCCE03A_4872_4BD7_97D5_D3651B9A81E4__INCLUDED_)
#define AFX_PLAYLIST_H__CDCCE03A_4872_4BD7_97D5_D3651B9A81E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Playlist.h : header file
//

#include "mlistbox.h"
#include "config.h"
/////////////////////////////////////////////////////////////////////////////
// CPlaylist dialog

class CPlaylist : public CDialog
{
	int size, count, curr;
	CRecentFileList *pRecentFileList;

	CMListBox m_List;

	HBRUSH	hBrush;
	HWND hwndParent;

	CMenu *pMenu;

	BOOL bQuickMenu;

	// When loading playlist it can have options ( ex. /NoLoop )
	//  so we set this options here.
	stConfig	*pConfig;

	void	AddPlaylist(LPCTSTR lpszPathName, BOOL bSetCurr=TRUE);

	void	AddFiles();
	void	OpenPlaylist();
	void	SavePlaylist();

	void InitLanguage();
// Construction
public:
	CPlaylist(CWnd* pParent = NULL);   // standard constructor
	~CPlaylist();


	void Create( int nSize, BOOL bReadList, stConfig *pConfig=NULL );
	void CreateWnd( CWnd* pParentWnd );

	void ReadList();
	void WriteList();
	void ClearList();
	void Add(LPCTSTR lpszPathName, BOOL bSetCurr=TRUE);
	void Remove(int num);
	void GoToLast();
	int CountFiles();
	BOOL IsNext();
	BOOL IsPrev();
	BOOL IsCurr();	
	LPCTSTR Curr();
	LPCTSTR Prev();
	LPCTSTR Next();



	void	ShowQuickMenu();
	BOOL ShowWindow( int nCmdShow );

	virtual void OnOK();
	virtual void OnCancel();

// Dialog Data
	//{{AFX_DATA(CPlaylist)
	enum { IDD = IDD_DIALOG_PLAYLIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlaylist)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPlaylist)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDblclkListPlaylist();
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnQmplsClose();
	afx_msg void OnQmplsClearpls();
	afx_msg void OnQmplsAddfiles();
	afx_msg void OnQmplsRemovefile();
	afx_msg void OnSelchangeListPlaylist();
	afx_msg void OnPlsDeleteselfile();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnQmplsOpen();
	afx_msg void OnQmplsSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYLIST_H__CDCCE03A_4872_4BD7_97D5_D3651B9A81E4__INCLUDED_)
