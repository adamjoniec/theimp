// MListBox.cpp : implementation file
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "MListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMListBox

CMListBox::CMListBox()
{
}

CMListBox::~CMListBox()
{
}


BEGIN_MESSAGE_MAP(CMListBox, CListBox)
	//{{AFX_MSG_MAP(CMListBox)
	ON_WM_RBUTTONUP()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMListBox message handlers

void CMListBox::OnRButtonUp(UINT nFlags, CPoint point) 
{

	
	CListBox::OnRButtonUp(nFlags, point);
}

LRESULT DoNotify(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   LPNMLISTVIEW  pnm    = (LPNMLISTVIEW)lParam;

   switch (pnm->hdr.code){
      case NM_CUSTOMDRAW:{
         LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lParam;

         /*
         CDDS_PREPAINT is at the beginning of the paint cycle. You 
         implement custom draw by returning the proper value. In this 
         case, we're requesting item-specific notifications.
         */
         if(lplvcd->nmcd.dwDrawStage == CDDS_PREPAINT)
            // Request prepaint notifications for each item.
            return CDRF_NOTIFYITEMDRAW;

         /*
         Because we returned CDRF_NOTIFYITEMDRAW in response to
         CDDS_PREPAINT, CDDS_ITEMPREPAINT is sent when the control is
         about to paint an item.
         */
         if(lplvcd->nmcd.dwDrawStage == CDDS_ITEMPREPAINT){
            /*
            To change the font, select the desired font into the 
            provided HDC. We're changing the font for every third item
            in the control, starting with item zero.
            */
            //if(!(lplvcd->nmcd.dwItemSpec % 3))
              // SelectObject(lplvcd->nmcd.hdc, g_hNewFont);
            //else
              // return(CDRF_DODEFAULT);

            /*
            To change the text and background colors in a list view 
            control, set the clrText and clrTextBk members of the 
            NMLVCUSTOMDRAW structure to the desired color.

            This differs from most other controls that support 
            CustomDraw. To change the text and background colors for 
            the others, call SetTextColor and SetBkColor on the provided HDC.
            */
            lplvcd->clrText = RGB(150, 75, 150);
            lplvcd->clrTextBk = RGB(255,255,255);

            /*
            We changed the font, so we're returning CDRF_NEWFONT. This
            tells the control to recalculate the extent of the text.
            */
            return CDRF_NEWFONT;
            }
         }
      
      default:
         break;
   }

   return 0;
}
LRESULT CMListBox::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND hWnd = GetParent()->m_hWnd;

	switch ( message )
	{
		case WM_MOUSEMOVE:
		case WM_RBUTTONUP:
			::PostMessage(hWnd,message,wParam,lParam);
			break;

		//case WM_NOTIFY:
		//	break;

	}

	
	return CListBox::DefWindowProc(message, wParam, lParam);
}



HBRUSH CMListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
	static HBRUSH hBrush = NULL;
	//if ( !hBrush ) hBrush = CreateSolidBrush(RGB(0,0,0));
	//return hBrush;
	
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return NULL;
}
