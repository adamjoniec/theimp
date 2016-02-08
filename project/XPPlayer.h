// XPPlayer.h : main header file for the XPPLAYER application
//

#if !defined( FILE_XPPLAYER_H )
#define FILE_XPPLAYER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"		// main symbols
#include "playlist.h"
#include "subtitles.h"

/////////////////////////////////////////////////////////////////////////////
// CXPPlayerApp:
// See XPPlayer.cpp for the implementation of this class
//

class CXPPlayerApp : public CWinApp
{
	HACCEL hAccel;

public:
	CXPPlayerApp();

	void	ShowSplashScreen();
	void	ShowExpired();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXPPlayerApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXPPlayerApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined( FILE_XPPLAYER_H )
