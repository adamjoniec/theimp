// TimeOut.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "TimeOut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeOut dialog


CTimeOut::CTimeOut(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeOut::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeOut)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pFont = NULL;
}


void CTimeOut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeOut)
	DDX_Control(pDX, IDC_CAPTION, m_TCaption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeOut, CDialog)
	//{{AFX_MSG_MAP(CTimeOut)
	ON_BN_CLICKED(IDC_BUTTON_REGISTER, OnButtonRegister)
	ON_BN_CLICKED(IDC_BUTTON_THEIMP, OnButtonTheimp)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeOut message handlers

void CTimeOut::OnButtonRegister() 
{
	ShellExecute(m_hWnd,
		"open", "http://www.theimp.prv.pl/register.html",
		NULL,NULL,SW_SHOWDEFAULT);
}

void CTimeOut::OnButtonTheimp() 
{
	ShellExecute(m_hWnd,
		"open", "http://www.theimp.prv.pl/",
		NULL,NULL,SW_SHOWDEFAULT);		
}

HBRUSH CTimeOut::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( pWnd==&m_TCaption )
	{
		if ( !pFont )
		{
			pFont = new CFont();
			pFont->CreatePointFont( 180, "MS Sans Serif" );
		}
		pDC->SelectObject( pFont );
		pDC->SetTextColor( RGB(255,0,0) );
	}
	
	return hbr;
}
