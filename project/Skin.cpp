
#include "stdafx.h"
#include "skin.h"


//create HRGN from bitmap alpha ( bitmap format must be RGBA )
//producess smallest possible region
//very fast (about 0,015s from image 640x480x32 on P700)
HRGN CSkin::CreateRgn(cGImage *pImage)
{

	// If bitmap bacgkround don't have alpha, create
	//  rectangular region (image rectangle).
	if ( pImage->bpp != 32 ) {

		int rects = 1, width = pImage->w, height = pImage->h,
			 rgndatasize = rects*sizeof(RECT) + sizeof(RGNDATAHEADER);

		BYTE *pRgnDataBuff = new BYTE [ rgndatasize ];
		prgnData = (RGNDATA*) pRgnDataBuff;

		prgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
		prgnData->rdh.iType = RDH_RECTANGLES;
		prgnData->rdh.nCount = rects;
		prgnData->rdh.nRgnSize = 0;
		prgnData->rdh.rcBound.left = 0;
		prgnData->rdh.rcBound.right = width;
		prgnData->rdh.rcBound.top = 0;
		prgnData->rdh.rcBound.bottom = height;

		RECT *prc = (RECT*)prgnData->Buffer;
		SetRect( prc, 0,0, width, height );

		hRgn = ExtCreateRegion(NULL, rgndatasize, prgnData);
		return hRgn;
	}

	#define MAX_RECT 4096

	unsigned char *pRGBA = (unsigned char *) pImage->GetPixels(),
					*palha = pRGBA+3,
					*palhaend = pRGBA+(pImage->w*pImage->h*4);
	int height = pImage->h,
		width = pImage->w,
		linebytes = pImage->w * 4,
		x = 0, y=0, i;

	int rect_started = FALSE;

	RECT rc[MAX_RECT], *prc = rc, *prcend = prc+MAX_RECT,
		 *rcprevlineend = NULL;
	DWORD dwTicks = GetTickCount();

	int rects = 0;

	#define CHECK_SAME_RECT(prc, prevline, dw)	if (prevline)		\
	{RECT *pl = prevline-1;											\
	 while (pl>=rc && prc->bottom==pl->bottom+1) {					\
		if (prc->left==pl->left && prc->right==pl->right)			\
			{ prc->top = pl->top; pl->left=pl->right=0;}			\
		pl--;														\
	 }																\
	}		//endof #define

	while (palha<palhaend && prc<prcend)
	{
		if (*palha) {
			if (!rect_started) {
				prc->left = x;
				prc->top = y;
				prc->bottom = y+1;
				rect_started = TRUE;
			}
		}
		else {
			if (rect_started) {
				prc->right = x;
				CHECK_SAME_RECT(prc, rcprevlineend, rects_prevline)
				prc++;
				rect_started = FALSE;
			}
		}

		palha += 4;
		x++; 
		if (x==width) { 			
			if (rect_started){
				prc->right = x;
				CHECK_SAME_RECT(prc, rcprevlineend, rects_prevline)
				prc++;
				rect_started = FALSE;
			}
			rcprevlineend = prc;
			x=0; y++; 
		}
	}
	if (rect_started){
		prc->right = x;
		CHECK_SAME_RECT(prc, rcprevlineend, rects_prevline)
		prc++; rects++;
	}

	rects = prc-rc;
	int rgndatasize = rects*sizeof(RECT) + sizeof(RGNDATAHEADER);
	unsigned char *prgndata = 
		new unsigned char [ rgndatasize ];
	RGNDATA *pRgnData = (RGNDATA*)prgndata;

	RECT *prcSource = rc, *prcDest = (RECT*)pRgnData->Buffer;
	for (i=0; i<rects; i++) {
		if (prcSource->left!=prcSource->right) {
			prcDest->left = prcSource->left;
			prcDest->right = prcSource->right;
			prcDest->top = prcSource->top;
			prcDest->bottom = prcSource->bottom;
			prcDest++;
		}
		prcSource++;
	}
	rects = prcDest - (RECT*)pRgnData->Buffer;


	pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType = RDH_RECTANGLES;
	pRgnData->rdh.nCount = rects;
	pRgnData->rdh.nRgnSize = 0;
	pRgnData->rdh.rcBound.left = 0;
	pRgnData->rdh.rcBound.right = width;
	pRgnData->rdh.rcBound.top = 0;
	pRgnData->rdh.rcBound.bottom = height;

	//copy region data to global variable prgnData
	rgndatasize = rects*sizeof(RECT) + sizeof(RGNDATAHEADER);
	BYTE *pRgnDataBuff = new BYTE [ rgndatasize ];
	prgnData = (RGNDATA*) pRgnDataBuff;
	memcpy( prgnData, prgndata, rgndatasize );


	hRgn = ExtCreateRegion(NULL, rgndatasize, pRgnData);

	delete[] prgndata;


	#undef MAX_RECT
	#undef CHECK_SAME_RECT

	return hRgn;

}

//resize region to size newWidth and newHeight
//and return new size also in newWidth and newHeight
//return FALSE on ERROR
BOOL CSkin::ResizeRgn(long *newWidth,long *newHeight, BOOL bForceResize )
{
	long width = *newWidth, height = *newHeight;

	if ( !allowResize && !bForceResize ) return FALSE;

	if (width==sizeCurr.cx && height==sizeCurr.cy && !bForceResize)
		return TRUE;

	if ( width< sizeMinimal.cx ) 
		width = sizeMinimal.cx;
	if ( height< sizeMinimal.cy ) 
		height = sizeMinimal.cy;



	int rects = prgnData->rdh.nCount;
	int buffsize = sizeof(RGNDATAHEADER) + sizeof(RECT)*rects;
	BYTE *buff = new BYTE [ buffsize ];
	memcpy(buff, prgnData, buffsize);

	RGNDATA *prd = (RGNDATA*)buff;

	struct stRECT { LONG x1, y1, x2, y2; };
	struct stRECTX { LONG x, y1, none, y2; };
	struct stRECTY { LONG x1, y, x2, none; };

	stRECT *rc = (stRECT*)&prd->Buffer;

	int cx = (width - sizeOrg.cx), cy = (height - sizeOrg.cy);
	int cleft = countRectsLeft, cright = countRectsRight, 
		 ctop = countRectsTop, cbottom = countRectsBottom;


	
	for (int i=0; i<rects; i++) {

		stRECTY	*LEFT = (stRECTY*)rcResizeLeft, 
					*RIGHT = (stRECTY*)rcResizeRight;

		stRECTX	*TOP = (stRECTX*)rcResizeTop, 
					*BOTTOM = (stRECTX*)rcResizeBottom;


		int  widthResize = 0, widthOffset = 0;
		if ( rc->y1 <= TOP->y2 ) {

			//int rectNum = 0, cxdiv = cx/ctop, cxmod = cx%ctop;
			int rectNum = 0;
			while ( rectNum<ctop && TOP->x<rc->x2 ) 
			{
				int cxdiv = cx/ctop, cxmod = cx%ctop;

				if ( TOP->x >= rc->x1 ) widthResize += cxdiv+cxmod;
				else widthOffset += cxdiv+cxmod;

				cxmod = 0;
				TOP++; rectNum++;
			}


		} else {

			//int rectNum = 0, cxdiv = cx/cbottom, cxmod = cx%cbottom;
			int rectNum = 0;
			while ( rectNum<cbottom && BOTTOM->x<rc->x2 ) 
			{
				int cxdiv = cx/cbottom, cxmod = cx%cbottom;

				if ( BOTTOM->x >= rc->x1 ) widthResize += cxdiv+cxmod;
				else widthOffset += cxdiv+cxmod;

				cxmod = 0;
				BOTTOM++; rectNum++;
			}
		}

		int  rectNum = 0, heightResize = 0, heightOffset = 0;
		if ( rc->x1 <= LEFT->x2 ) {

			//int rectNum = 0, cydiv = cy/cleft, cymod = cy%cleft;
			int rectNum = 0;
			while ( rectNum<cleft && LEFT->y<rc->y2 ) 
			{
				int cydiv = cy/cleft, cymod = cy%cleft;

				if ( LEFT->y >= rc->y1 ) heightResize += cydiv+cymod;
				else heightOffset += cydiv+cymod;

				cymod = 0;
				LEFT++; rectNum++;
			}


		} else {

			int rectNum = 0;
			while ( rectNum<cright && RIGHT->y<rc->y2 ) 
			{
				int cydiv = cy/cright, cymod = cy%cright;

				if ( RIGHT->y >= rc->y1 ) heightResize += cydiv+cymod;
				else heightOffset += cydiv+cymod;

				cymod = 0;
				RIGHT++; rectNum++;
			}
		}

		rc->x1 += widthOffset; rc->x2 += widthOffset+widthResize;
		rc->y1 += heightOffset; rc->y2 += heightOffset+heightResize;

		rc++;
	}


	DeleteObject( hRgn );

	prd->rdh.rcBound.right = width;
	prd->rdh.rcBound.bottom = height;

	hRgn = ExtCreateRegion(NULL, buffsize, prd);
	


	delete[] buff;

	*newWidth = width;
	*newHeight = height;

	return TRUE;
}

// Resize skin, or update background (if resize not allowed),
//  but don't draw skin on screen ( you must call DrawSkin() )
BOOL CSkin::Resize(HWND hWnd, long *newWidth, long *newHeight, BOOL bForceResize)
{
	if ( !IsDefined() ) return FALSE;

	
	if ( !hWnd ) return FALSE;

	if ( *newWidth< sizeMinimal.cx ) *newWidth = sizeMinimal.cx;
	if ( *newHeight< sizeMinimal.cy ) *newHeight = sizeMinimal.cy;

	if ( *newWidth> sizeMaximal.cx ) *newWidth = sizeMaximal.cx;
	if ( *newHeight> sizeMaximal.cy ) *newHeight = sizeMaximal.cy;

	long width = *newWidth, height = *newHeight, i;

	if ( width==sizeCurr.cx && height==sizeCurr.cy && !bForceResize )
		return FALSE;

	if ( !allowResize ) {

		if ( !bForceResize ) return FALSE;

		width = sizeOrg.cx; height = sizeOrg.cy;
	}

	HDC hdc = GetDC( hWnd );
	if ( !hdc ) return FALSE;


	ResizeRgn(&width, &height, bForceResize);

	HDC hdcSource = CreateCompatibleDC( hdc );
	SelectObject( hdcSource, pImage->hBitmap );

	*newWidth = width; *newHeight = height;
	sizeCurr.cx = width; sizeCurr.cy = height;

	struct stRECT { LONG x1, y1, x2, y2; };
	struct stRECTX { LONG x, y1, none, y2; };
	struct stRECTY { LONG x1, y, x2, none; };

	stRECTY	*LEFT = (stRECTY*)rcResizeLeft, 
				*RIGHT = (stRECTY*)rcResizeRight;

	stRECTX	*TOP = (stRECTX*)rcResizeTop, 
				*BOTTOM = (stRECTX*)rcResizeBottom;

	int cx = (width - sizeOrg.cx), cy = (height - sizeOrg.cy);
	int cleft = countRectsLeft, cright = countRectsRight, 
		 ctop = countRectsTop, cbottom = countRectsBottom;


	//resize top edge (resize width of skin)
	int x=0, y=TOP->y1, w=0, h=TOP->y2-y;

	//int xOffset = 0, cxdiv = cx/ctop, cxmod = cx%ctop;
	int xOffset = 0;
	for (i=0; i<ctop; i++) {
		int cxdiv = cx/ctop, cxmod = cx%ctop;

		int dx = TOP->x+xOffset, dy = TOP->y1, dw = 1, dh = TOP->y2 - dy,
			 sx = TOP->x, sy = TOP->y1;

		for (int j=0; j<(cxdiv+cxmod); j++)
			BitBlt( hdcBackground, dx++, dy, dw, dh, 
							hdcSource, sx, sy, SRCCOPY );

		x = TOP->x;
		TOP++;
		xOffset += cxdiv+cxmod; cxmod = 0;
		
		if ((i+1)<ctop) w = TOP->x-x -1;
		else w = sizeOrg.cx - x;
		BitBlt( hdcBackground, x+xOffset, y, w+1, h, 
							hdcSource, x, y, SRCCOPY );
	}
	TOP = (stRECTX*)rcResizeTop;
	BitBlt( hdcBackground, 0, 0, rcResizeTop->right, rcResizeTop->bottom, 
						hdcSource, 0, 0, SRCCOPY );
	
	//resize bottom edge (resize width) of skin)
	//xOffset = 0, cxdiv = cx/cbottom, cxmod = cx%cbottom;
	xOffset = 0;
	x=0, y=BOTTOM->y1, w=0, h=BOTTOM->y2-y;
	for (i=0; i<cbottom; i++) {
		int cxdiv = cx/cbottom, cxmod = cx%cbottom;

		int dx = BOTTOM->x+xOffset, dy = BOTTOM->y1, dw = 1, dh = BOTTOM->y2 - dy,
			 sx = BOTTOM->x, sy = BOTTOM->y1;

		for (int j=0; j<(cxdiv+cxmod); j++)
			BitBlt( hdcBackground, dx++, dy+cy, dw, dh, 
							hdcSource, sx, sy, SRCCOPY );

		x = BOTTOM->x;
		BOTTOM++;
		xOffset += cxdiv+cxmod; cxmod = 0;
		
		if (i+1<cbottom) w = BOTTOM->x-x -1;
		else w = sizeOrg.cx - x;
		BitBlt( hdcBackground, x+xOffset, y+cy, w+1, h, 
							hdcSource, x, y, SRCCOPY );
	}
	BOTTOM = (stRECTX*)rcResizeBottom;
	BitBlt( hdcBackground, 0, y+cy, BOTTOM->x, h, 
						hdcSource, 0, y, SRCCOPY );

	//resize left edge (resize height od skin)
	//int yOffset = 0, cydiv = cy/cleft, cymod = cy%cleft;
	int yOffset = 0;
	x=LEFT->x1, y=0, w=LEFT->x2-x, h=0;
	for (i=0; i<cleft; i++) {
		int cydiv = cy/cleft, cymod = cy%cleft;

		int dx = LEFT->x1, dy = LEFT->y, dw = LEFT->x2-x, dh = 1,
			 sx = LEFT->x1, sy = LEFT->y;

		for (int j=0; j<(cydiv+cymod); j++)
			BitBlt( hdcBackground, dx, dy++, dw, dh, 
							hdcSource, sx, sy, SRCCOPY );

		y = LEFT->y;
		LEFT++;
		yOffset += cydiv+cymod; cymod = 0;
		
		if (i+1<cleft) h = LEFT->y-y -1;
		else h = sizeOrg.cy - y - (rcResizeBottom->bottom-rcResizeBottom->top);
		BitBlt( hdcBackground, x, y+yOffset, w, h+1, 
							hdcSource, x, y, SRCCOPY );
	}
	LEFT = (stRECTY*)rcResizeLeft;
	h = LEFT->y; if (h>BOTTOM->y1+yOffset) h = BOTTOM->y1+yOffset;
	BitBlt( hdcBackground, 0, 0, LEFT->x2, h, 
						hdcSource, 0, 0, SRCCOPY );

	//resize right edge (resize height od skin)
	//yOffset = 0, cydiv = cy/cleft, cymod = cy%cleft;
	yOffset = 0;
	x=RIGHT->x1, y=0, w=RIGHT->x2-x, h=0;
	for (i=0; i<cright; i++) {
		int cydiv = cy/cleft, cymod = cy%cleft;

		int dx = RIGHT->x1, dy = RIGHT->y, dw = RIGHT->x2-x, dh = 1,
			 sx = RIGHT->x1, sy = RIGHT->y;

		for (int j=0; j<(cydiv+cymod); j++)
			BitBlt( hdcBackground, dx+cx, dy++, dw, dh, 
							hdcSource, sx, sy, SRCCOPY );

		y = RIGHT->y;
		RIGHT++;
		yOffset += cydiv+cymod; cymod = 0;
		
		if (i+1<cright) h = LEFT->y-y -1;
		else h = sizeOrg.cy - y - (rcResizeBottom->bottom-rcResizeBottom->top);
		BitBlt( hdcBackground, x+cx, y+yOffset, w, h+1, 
							hdcSource, x, y, SRCCOPY );
	}
	RIGHT = (stRECTY*)rcResizeRight;
	h = RIGHT->y; if (h>BOTTOM->y1+yOffset) h = BOTTOM->y1+yOffset;
	BitBlt( hdcBackground, x+cx, 0, w, h, 
						hdcSource, x, 0, SRCCOPY );


	//draw resized background is done
	//now we must offset rectangles and scrollbars
	for (int j=0; j<4; j++) {

		stButton *pb = pButton;
		stScrollbar *ps = pScrollbar;
		stStatus *pstatus = pStatus;
		//RECT *prcStatus = rcStatusArea;

		int k=0;
		stRECT *rc;
		
		switch(j) {
			case 0: //offset buttons
				k=countButtons; rc = (stRECT*)&pb->rcPosition; break;
			case 1: //offset and resize scrollbars
				k=countScrollbars; rc = (stRECT*)&ps->rcPosition; break;
			case 2: //offset and resize rcPlayArea
				k=1; rc = (stRECT*)&rcPlayAreaOrg; break;
			case 3: //offset and resize rcStatusArea[2]
				//k=2; rc =(stRECT*)rcStatusAreaOrg; break;
				k=countStatuses; rc =(stRECT*)&pstatus->rcAreaOrg; break;
		}

		for (i=0; i<k; i++) {

			stRECTY	*LEFT = (stRECTY*)rcResizeLeft, 
						*RIGHT = (stRECTY*)rcResizeRight;

			stRECTX	*TOP = (stRECTX*)rcResizeTop, 
						*BOTTOM = (stRECTX*)rcResizeBottom;

		
			int  widthResize = 0, widthOffset = 0;
			if ( rc->y1 <= TOP->y2 ) {

				//int rectNum = 0, cxdiv = cx/ctop, cxmod = cx%ctop;
				int rectNum = 0;
				while ( rectNum<ctop && TOP->x<rc->x2 ) 
				{
					int cxdiv = cx/ctop, cxmod = cx%ctop;

					if ( TOP->x >= rc->x1 ) widthResize += cxdiv+cxmod;
					else widthOffset += cxdiv+cxmod;

					cxmod = 0;
					TOP++; rectNum++;
				}


			} else {

				//int rectNum = 0, cxdiv = cx/cbottom, cxmod = cx%cbottom;
				int rectNum = 0;
				while ( rectNum<cbottom && BOTTOM->x<rc->x2 ) 
				{
					int cxdiv = cx/cbottom, cxmod = cx%cbottom;

					if ( BOTTOM->x >= rc->x1 ) widthResize += cxdiv+cxmod;
					else widthOffset += cxdiv+cxmod;

					cxmod = 0;
					BOTTOM++; rectNum++;
				}
			}

			int  rectNum = 0, heightResize = 0, heightOffset = 0;
			if ( rc->x1 <= LEFT->x2 ) {

				//int rectNum = 0, cydiv = cy/cleft, cymod = cy%cleft;
				int rectNum = 0;
				while ( rectNum<cleft && LEFT->y<rc->y2 ) 
				{
					int cydiv = cy/cleft, cymod = cy%cleft;

					if ( LEFT->y >= rc->y1 ) heightResize += cydiv+cymod;
					else heightOffset += cydiv+cymod;
	
					cymod = 0;
					LEFT++; rectNum++;
				}


			} else {

				int rectNum = 0;
				while ( rectNum<cright && RIGHT->y<rc->y2 ) 
				{
					int cydiv = cy/cright, cymod = cy%cright;

					if ( RIGHT->y >= rc->y1 ) heightResize += cydiv+cymod;
					else heightOffset += cydiv+cymod;
	
					cymod = 0;
					RIGHT++; rectNum++;
				}
			}

			switch (j) {
				case 0:	//offset buttons
					pb->x1 = rc->x1 + widthOffset; 
					pb->x2 = rc->x2 + widthOffset;
					pb->y1 = rc->y1 + heightOffset; 
					pb->y2 = rc->y2 + heightOffset;
					pb++;
					rc = (stRECT*)&pb->rcPosition;
					break;
				case 1:	//offset and resize scrollbars
					ps->buttonBar.x1 = rc->x1 + widthOffset; 
					ps->buttonBar.x2 = rc->x2 + widthOffset+widthResize;
					ps->buttonBar.y1 = rc->y1 + heightOffset; 
					ps->buttonBar.y2 = rc->y2 + heightOffset+heightResize;
					SetScrollbarValue(i, ps->dValue );
					ps->prevSliderPos.x1 = ps->buttonSlider.x1;
					ps->prevSliderPos.y1 = ps->buttonSlider.y1;
					ps++;
					rc = (stRECT*)&ps->rcPosition;
					break;
				case 2:	//offset and resize rcPlayArea
					rcPlayArea.left =  rc->x1 + widthOffset;
					rcPlayArea.right =  rc->x2 + widthOffset+widthResize;
					rcPlayArea.top =  rc->y1 + heightOffset;
					rcPlayArea.bottom =  rc->y2 + heightOffset+heightResize;
					break;
				case 3:	//offset and resize rcStatus
					if (!rc->x2 || !rc->y2) {
						pstatus++; rc = (stRECT*)&pstatus->rcAreaOrg; break; }

					RECT *prcStatus; prcStatus = &pstatus->rcArea;

					prcStatus->left =  rc->x1 + widthOffset;
					prcStatus->right =  rc->x2 + widthOffset+widthResize;
					prcStatus->top =  rc->y1 + heightOffset;
					prcStatus->bottom =  rc->y2 + heightOffset+heightResize;
					pstatus++; rc = (stRECT*)&pstatus->rcAreaOrg;
					break;
			}

		}
	}

	//define resize rects for scrollbars
	stRECT *prc;
	stScrollbar *ps = pScrollbar;
	for (i=0; i<countScrollbars; i++) {
		
		ps->countResizeRects = 0;
		prc = (stRECT*)&ps->rcPosition;

		TOP = (stRECTX*)rcResizeTop;
		for (int j=0; j<ctop; j++) {

			if ( prc->x1 <= TOP->x && prc->x2 >= TOP->x &&
				prc->y1< TOP->y2 && ps->countResizeRects<MAX_RESIZE_RECTS) 
			{

				stRECT *prcRes = (stRECT*)&ps->rcResize[ps->countResizeRects];

				prcRes->x1 = TOP->x - prc->x1; prcRes->x2 = prc->x1 + 1;
				prcRes->y1 = prc->y1; prcRes->y2 = prc->y2;
				ps->countResizeRects++;

			}
			TOP++;
			
		}

		ps++;		
	}

	//define resize rects for scrollbars (BOTTOM)	
	ps = pScrollbar;
	for (i=0; i<countScrollbars; i++) {
		
		prc = (stRECT*)&ps->rcPosition;
		
		BOTTOM = (stRECTX*)rcResizeBottom;
		for (int j=0; j<cbottom; j++) {

			if ( prc->x1 <= BOTTOM->x && prc->x2 >= BOTTOM->x &&
				prc->y2> BOTTOM->y1 && ps->countResizeRects<MAX_RESIZE_RECTS) 
			{

				stRECT *prcRes = (stRECT*)&ps->rcResize[ps->countResizeRects];

				prcRes->x1 = BOTTOM->x; prcRes->x2 = BOTTOM->x+1;
				prcRes->y1 = prc->y1; prcRes->y2 = prc->y2;
				ps->countResizeRects++;

			}
			BOTTOM++;
		}

		ps++;
	}

	//we must delete source dc, so other functions can use source bitmap
	DeleteDC( hdcSource );


	for (i=0; i<countButtons; i++)
		DrawButton( hdc, i, DRAW_BUTTON_BACKGROUND );

	for (i=0; i<countScrollbars; i++)
		DrawScrollbar( hdc, i, DRAW_SCROLLBAR_BAR);


	ReleaseDC( hWnd, hdc );

	//when resized all elements, resize window
	RECT rc; GetWindowRect(hWnd,  &rc );
	SetWindowPos(hWnd, NULL, rc.left, rc.top, 
						width, height, SWP_NOZORDER );
	//set new region
	SetWindowRgn(hWnd, hRgn, TRUE );

	//and draw change of region shape
	//UpdateWindow(hWnd); //=> ERROR WHEN CHANGIN SKIN 


	return TRUE;
}


// Resize skin size to match to play area size to new play area size.
// Return new size of skin.
BOOL CSkin::ResizePlayArea(HWND hWnd, int *nwPlayArea, int *nhPlayArea )
{
	long w = *nwPlayArea+(sizeOrg.cx-(rcPlayAreaOrg.right-rcPlayAreaOrg.left) ),
		 h = *nhPlayArea+(sizeOrg.cy-(rcPlayAreaOrg.bottom-rcPlayAreaOrg.top) );



	BOOL ret = Resize( hWnd, &w, &h );

	*nwPlayArea = w; *nhPlayArea = h;

	return ret;
}


//return numeric eqvivalent of string Id
//or -1 when invalid string Id
int CSkin::GetButtonId(char *pszId)
{
	for (int i=0; i<countButtons && _stricmp(pszId, ppszButtonId[i]); i++) ;
	if (i<countButtons)
		return i;

	return -1;
}
int CSkin::GetButtonIdFromPoint(POINT pt)
{
	stButton *pbutton = pButton;
	for (int i=0; i<countButtons && 
				  !(pt.x>pbutton->x1 && pt.x<pbutton->x2 &&
				   pt.y>pbutton->y1 && pt.y<pbutton->y2)
			; i++) 
		pbutton++;

	if (i<countButtons) switch ( pbutton->shapeId )
	{
		case SHAPE_RECT:
			return i;

		case SHAPE_SINGLEALPHA:
		{
			cGImage *pimage = pImage+pbutton->pnorm->imgId;
			int x = pbutton->pnorm->x1+(pt.x-pbutton->x1),
				y = pbutton->pnorm->y1+(pt.y-pbutton->y1);

			if ( pimage->GetAlphaPixel(x,y) ) return -1;
		
			return i;
		}

		case SHAPE_MULTIALPHA:
		{
			cGImage *pimage = pImage+pbutton->pcurr->imgId;
			int x = pbutton->pcurr->x1+(pt.x-pbutton->x1),
				y = pbutton->pcurr->y1+(pt.y-pbutton->y1);

			if ( pimage->GetAlphaPixel(x,y) ) return -1;
		
			return i;
		}
	}

	return -1;
}

int CSkin::GetButtonState(char *pszState)
{
	char szButtonState[][16] = {
		"normal", "over", "press", 
		"normal.second", "over.second", "press.second", "disabled" };

	int buttonState = 0;
	while( buttonState<7 && _stricmp(pszState, szButtonState[buttonState]) )
		buttonState++;

	switch ( buttonState ) {

		case 0: return BSTATE_DRAW_FIRST | BSTATE_MOUSE_NORMAL;
		case 1: return BSTATE_DRAW_FIRST | BSTATE_MOUSE_OVER;
		case 2: return BSTATE_DRAW_FIRST | BSTATE_MOUSE_PRESS;

		case 3: return BSTATE_DRAW_SECOND | BSTATE_MOUSE_NORMAL;
		case 4: return BSTATE_DRAW_SECOND | BSTATE_MOUSE_OVER;
		case 5: return BSTATE_DRAW_SECOND | BSTATE_MOUSE_PRESS;

		case 6: return BSTATE_DRAW_DISABLED;
	}

	return -1;
}

int CSkin::GetImageId(char *pszId)
{
	for (int i=0; i<countImages && _stricmp(pszId, ppszImageId[i]); i++) ;
	if (i<countImages)
		return i;

	return -1;
}



// return TRUE when added button or FALSE when didn't
int CSkin::AddButton(int buttonId, int buttonState,
								int x, int y, int w, int h,
								int imageId, int shapeId)
{
	int id = buttonId;
	if (id==-1) return FALSE;

	int imgId = imageId;
	if (imgId==-1) return FALSE;

	int drawId = buttonState & BSTATE_DRAW_MASK,
		mouseId = buttonState & BSTATE_MOUSE_MASK;

	//define button position
	if ( buttonState & BSTATE_DEFPOSITION ) {

		if (w && h) {
			pButton[id].x1 = x; pButton[id].y1 = y;
			pButton[id].x2 = x+w; pButton[id].y2 = y+h;
			pButton[id].w = w; pButton[id].h = h;
			pButton[id].shapeId = shapeId;

			return TRUE;
		}

		return FALSE;
	}


	int num;
	stButtonState *pstate;

	switch ( drawId ) {

		case BSTATE_DRAW_FIRST: num = 0; break;

		case BSTATE_DRAW_SECOND: num = 1; break;

		case BSTATE_DRAW_DISABLED:
			mouseId = 0;
			pstate = &pButton[id].disabled; 
			pButton[id].bIsDisabledDef = TRUE; break;
	}

	switch ( mouseId ) {

		case BSTATE_MOUSE_NORMAL: 
			pstate = &pButton[id].norm[num]; 
			if (num==0) pButton[id].bIsFirstDef = TRUE;
			if (num==1) pButton[id].bIsSecondDef = TRUE;
			break;

		case BSTATE_MOUSE_OVER: pstate = &pButton[id].over[num]; break;

		case BSTATE_MOUSE_PRESS: pstate = &pButton[id].press[num]; break;
	}


	if ( !w || w>pButton[id].w ) w = pButton[id].w;
	if ( !h || h>pButton[id].h ) h = pButton[id].h;

	pstate->x1 = x; pstate->x2 = x+w;
	pstate->y1 = y; pstate->y2 = y+h;
	pstate->w = w; pstate->h = h;
	pstate->imgId = imgId;
	pstate->isDefined = TRUE;

	return TRUE;
}

int CSkin::GetScrollbarState(char *pszState)
{
	char szButtonState[][16] = {
		"normal", "over", "press", "disabled",
		"slider.normal", "slider.over", "slider.press", "slider.disabled" };

	int buttonState = 0;
	while( buttonState<8 && _stricmp(pszState, szButtonState[buttonState]) )
		buttonState++;

	switch( buttonState ) {
		
		case 0: return SSTATE_DRAW_BAR|SSTATE_MOUSE_NORMAL;
		case 1: return SSTATE_DRAW_BAR|SSTATE_MOUSE_OVER;			// NIY
		case 2: return SSTATE_DRAW_BAR|SSTATE_MOUSE_PRESS;			// NIY
		case 3: return SSTATE_DRAW_BAR_DIS;								// NIY

		case 4: return SSTATE_DRAW_SLIDER|SSTATE_MOUSE_NORMAL;
		case 5: return SSTATE_DRAW_SLIDER|SSTATE_MOUSE_OVER;
		case 6: return SSTATE_DRAW_SLIDER|SSTATE_MOUSE_PRESS;
		case 7: return SSTATE_DRAW_SLIDER_DIS;							//NIY
	}

	return -1;
}

int CSkin::GetScrollbarId(char *pszId)
{
	for (int i=0; i<countScrollbars && _stricmp(pszId, ppszScrollbarId[i]); i++) ;
	if (i<countScrollbars)
		return i;

	return -1;
}

int CSkin::GetScrollbarIdFromPoint(POINT pt)
{
	if ( !IsDefined() ) return -1;

	stScrollbar *pscrollbar = pScrollbar;
	stButton *pb = &pscrollbar->buttonBar,
				*ps = &pscrollbar->buttonSlider;

	for ( int i=0; i<countScrollbars && 
			!( pt.x>=pb->x1 && pt.x<=pb->x2 && pt.y>=pb->y1 && pt.y<=pb->y2 ) &&
			!( pt.x>=ps->x1 && pt.x<=ps->x2 && pt.y>=ps->y1 && pt.y<=ps->y2 ); 
			i++ ) {

		pscrollbar++;
		pb = &pscrollbar->buttonBar;
		ps = &pscrollbar->buttonSlider;
	}
	if (i<countScrollbars)
		return i;

	return -1;
}


int CSkin::AddScrollbar(int scrollbarId, int scrollbarState,
											 int x, int y, int w, int h, int imageId, 
											 int marginLeft, int marginRight)
{
	int id = scrollbarId;
	if ( id==-1 ) return FALSE;

	int imgId = imageId;
	if (imgId==-1) return FALSE;

	stScrollbar *pscrollbar = pScrollbar + id;
	stButton *pbuttonBar = &pscrollbar->buttonBar,
				*pbuttonSlider = &pscrollbar->buttonSlider;


	//define scrollbar position
	if ( scrollbarState & SSTATE_DEFPOSITION )
	{
		if (w && h) {
			pbuttonBar->x1 = x; pbuttonBar->y1 = y;
			pbuttonBar->x2 = x+w; pbuttonBar->y2 = y+h;
			pbuttonBar->w = w; pbuttonBar->h = h;

			pscrollbar->marginLeft = marginLeft;
			pscrollbar->marginRight = marginRight;

			return TRUE;
		}
		return FALSE;
	}

	// if scrollbar isn't define return
	if ( !pbuttonBar->w || !pbuttonBar->h )
		return FALSE;

	//correct slider after loading
	if ( scrollbarState & SSTATE_CORRECT_SLIDER )
	{
		if ( !pbuttonBar->bIsFirstDef ) return FALSE;

		stButtonState *ps;

		ps = pbuttonSlider->pnorm;
		if (ps->w && ps->h) { w = ps->w; h = ps->h; }
		else return FALSE;

		ps = pbuttonSlider->pover;
		if (!ps->w || !ps->h) { 
			ps->w = w; ps->h = h; 
			ps->x2 = ps->x1 + w; ps->y2 = ps->y1 + h;
		}

		ps = pbuttonSlider->ppress;
		if (!ps->w || !ps->h) { 
			ps->w = w; ps->h = h; 
			ps->x2 = ps->x1 + w; ps->y2 = ps->y1 + h;
		}


		pbuttonSlider->w = w; 
		pbuttonSlider->h = h;

		pbuttonSlider->x1 = pbuttonBar->x1 + pscrollbar->marginLeft;
		pbuttonSlider->x2 = pbuttonSlider->x1 + w;
		pbuttonSlider->y1 = pbuttonBar->y1 - ( (h-pbuttonBar->h)/2 );
		pbuttonSlider->y2 = pbuttonSlider->y1 + h;
			
		pbuttonSlider->bIsFirstDef = TRUE;

		pscrollbar->bIsDefined = TRUE;

		return TRUE;
	}


	//add scrollbar state
	// bar have only NORMAL state ( haven't OVER and PRESS states )
	int drawId = scrollbarState & SSTATE_DRAW_MASK,
		mouseId = scrollbarState & SSTATE_MOUSE_MASK;


	stButtonState *pstate = NULL;

	switch ( drawId ) {

		case SSTATE_DRAW_BAR:
			pstate = &pbuttonBar->norm[0];
			pbuttonBar->bIsFirstDef = TRUE;
			break;

		case SSTATE_DRAW_BAR_SEL:
			pstate = &pbuttonBar->norm[1];
			pbuttonBar->bIsSecondDef = TRUE;
			break;

		case SSTATE_DRAW_BAR_DIS:
			pstate = &pbuttonBar->disabled; 
			pbuttonBar->bIsDisabledDef = TRUE; 
			break;

		case SSTATE_DRAW_SLIDER:
			switch ( mouseId ) {
				case SSTATE_MOUSE_NORMAL:
					pstate = &pbuttonSlider->norm[0]; break;
				case SSTATE_MOUSE_OVER:
					pstate = &pbuttonSlider->over[0]; break;
				case SSTATE_MOUSE_PRESS:
					pstate = &pbuttonSlider->press[0]; break;
			}
			break;

		case SSTATE_DRAW_SLIDER_DIS:
			pstate = &pbuttonSlider->disabled;
			pbuttonSlider->bIsDisabledDef = TRUE;
			break;
	}

	if ( !pstate ) return FALSE;


	if ( drawId==SSTATE_DRAW_BAR || drawId==SSTATE_DRAW_BAR_SEL ||
		drawId==SSTATE_DRAW_BAR_DIS ) {
		if ( !w || w>pbuttonBar->w ) w = pbuttonBar->w;
		if ( !h || h>pbuttonBar->h ) h = pbuttonBar->h;
	}

	pstate->x1 = x; pstate->x2 = x+w;
	pstate->y1 = y; pstate->y2 = y+h;
	pstate->w = w; pstate->h = h;
	pstate->imgId = imgId;
	pstate->isDefined = TRUE;

	return TRUE;
}

// return TRUE if need redraw
int CSkin::OnScrollbarMove(int scrollbarId, POINT pt)
{
	if ( !IsDefined() ) return FALSE;

	BOOL bChanged = FALSE;

	if ( scrollbarId<0 || scrollbarId>countScrollbars )
		return bChanged;

	stScrollbar *pscrollbar  = pScrollbar + scrollbarId;
	stButton *pbuttonBar = &pscrollbar->buttonBar;
	stButton *pbuttonSlider = &pscrollbar->buttonSlider;

	int x1, x2, w = pbuttonSlider->w;

	x1 = pt.x - (w / 2);
	x2 = x1 + w;
	if (x1< (pbuttonBar->x1+pscrollbar->marginLeft) ) {
		x1 = pbuttonBar->x1+pscrollbar->marginLeft;
		x2 = x1 + w;
	}
	if (x2> (pbuttonBar->x2-pscrollbar->marginRight) ) {
		x2 = pbuttonBar->x2-pscrollbar->marginRight;
		x1 = x2 - w;
	}

	stButtonState *ps = &pscrollbar->prevSliderPos;
	ps->x1 = pbuttonSlider->x1; ps->y1 = pbuttonSlider->y1;
	ps->w = pbuttonSlider->w; ps->h = pbuttonSlider->h;

	pbuttonSlider->x1 = x1;
	pbuttonSlider->x2 = x2;

	float f100 = (float)(pbuttonBar->x2 - pbuttonBar->x1 - 
				pscrollbar->marginLeft - pscrollbar->marginRight - w),
			fpercent = (float)(x1 - (pbuttonBar->x1+pscrollbar->marginLeft));

	pscrollbar->dValue = fpercent / f100 * 100.0f;


	return TRUE;
}

// return TRUE if slider must be redraw
int CSkin::SetScrollbarValue( int scrollbarId, double val )
{

	if ( !IsDefined() ) return FALSE;

	BOOL bChanged = FALSE;

	if ( scrollbarId<0 || scrollbarId>=countScrollbars )
		return bChanged;

	if ( val < 0 ) val = 0;
	if ( val >100 ) val = 100;

	stScrollbar *pscrollbar  = pScrollbar + scrollbarId;
	stButton *pbuttonBar = &pscrollbar->buttonBar;
	stButton *pbuttonSlider = &pscrollbar->buttonSlider;


	float f100 = (float)(pbuttonBar->x2 - pbuttonBar->x1 - 
				pscrollbar->marginLeft - pscrollbar->marginRight - 
				pbuttonSlider->w),
			fx1 = (float)( f100 * val / 100.0 );

	int h = pbuttonSlider->h, 
		 x1 = pbuttonBar->x1 + pscrollbar->marginLeft + (int)fx1,
		 x2 = x1 + pbuttonSlider->w,
		 y1 = pbuttonBar->y1 - ( (h-pbuttonBar->h)/2 ),
		 y2 = y1 + h;

	// val may be equal to pscrollbar->dValue but position may be diffrent
	if ( x1 != pbuttonSlider->x1 || x2 != pbuttonSlider->x2 ||
		  y1 != pbuttonSlider->y1 || y2 != pbuttonSlider->y2 )
	{

		stButtonState *ps = &pscrollbar->prevSliderPos;
		ps->x1 = pbuttonSlider->x1; ps->y1 = pbuttonSlider->y1;
		ps->w = pbuttonSlider->w; ps->h = pbuttonSlider->h;

		pbuttonSlider->x1 = x1;
		pbuttonSlider->x2 = x2;

		pbuttonSlider->y1 = y1;
		pbuttonSlider->y2 = y1;

		bChanged = TRUE;
	}

	// val may be diffrent from pscrollbar->dValue
	// but position may be the same as was
	pscrollbar->dValue = val;

	return bChanged;

}

int CSkin::GetScrollbarValue(int scrollbarId, double *pVal)
{
	if ( !IsDefined() ) return FALSE;

	if ( scrollbarId<0 || scrollbarId>=countScrollbars )
		return FALSE;

	*pVal = pScrollbar[scrollbarId].dValue;

	return TRUE;
}

int CSkin::IsScrollbarEnabled( int scrollbarId )
{
	if ( !IsDefined() ) return FALSE;

	if ( scrollbarId<0 || scrollbarId>=countScrollbars )
		return FALSE;

	return pScrollbar[ scrollbarId ].bEnabled;
}


/*	// Scrollbar is made up of two buttons: bar and slider.
	//	 Bar have two states: BAR and DISABLED, state BAR_SEL uses
	//   button's state SECOND NORMAL to draw bitmap over bar (selection).
	//  Slider is regular button without state SECOND.

	// draw state
	SSTATE_DRAW_BAR				= 1,			// -------------001	default
	SSTATE_DRAW_BAR_SEL			= 2,			// -------------010
	SSTATE_DRAW_BAR_DIS			= 3,			// -------------011
	SSTATE_DRAW_SLIDER			= 4,			// -------------100
	SSTATE_DRAW_SLIDER_DIS		= 5,			// -------------101
	SSTATE_DRAW_MASK				= 7,			// 0000000000000111

	SSTATE_MOUSE_NORMAL			= 8,			// ----------001---	def
	SSTATE_MOUSE_OVER				= 16,			// ----------010---
	SSTATE_MOUSE_PRESS			= 24,			// ----------011---
	SSTATE_MOUSE_MASK				= 56,			// 0000000000111000

	// enable or disable scrollbar
	// when disabled slider's position is constant
	// disable don't change MOUSE_STATE
	SSTATE_ENABLED					= 0x100,		//							def
	SSTATE_DISABLED				= 0x200,

	SSTATE_HORIZONTAL				= 0x400,		//							def
	SSTATE_VERTICAL				= 0x800,		// NOT IMPLEMENTED YET

	// used only once to set button's position when
	// loading skin and calling AddButton()
	SSTATE_DEFPOSITION			= 0x1000,
	SSTATE_CORRECT_SLIDER		= 0x2000*/
int CSkin::SetScrollbarState( int scrollbarId,  int scrollbarState )
{
	if ( !IsDefined() ) return FALSE;

	BOOL bChanged = FALSE;

	if ( scrollbarId <0 || scrollbarId>=countScrollbars )
		return bChanged;

	stScrollbar *pscrollbar  = pScrollbar + scrollbarId;
	stButton *pbuttonBar = &pscrollbar->buttonBar;
	stButton *pbuttonSlider = &pscrollbar->buttonSlider;

	if ( !pscrollbar->bIsDefined ) return bChanged;

	if ( scrollbarState&SSTATE_ENABLED && !pscrollbar->bEnabled ) {
		
		bChanged |= SetButtonState( pbuttonBar, BSTATE_ENABLED );
		bChanged |= SetButtonState( pbuttonSlider, BSTATE_ENABLED );
		pscrollbar->bEnabled = TRUE;

	} else
	if ( scrollbarState&SSTATE_DISABLED && pscrollbar->bEnabled ) {
	
		bChanged |= SetButtonState( pbuttonBar, BSTATE_DISABLED );
		bChanged |= SetButtonState( pbuttonSlider, BSTATE_DISABLED );
		bChanged |= SetScrollbarValue( scrollbarId, 0 );
		pscrollbar->bEnabled = FALSE;
		
	}

	if ( !pscrollbar->bEnabled) return bChanged;

	int drawId = scrollbarState & SSTATE_DRAW_MASK,
		 mouseId = scrollbarState & SSTATE_MOUSE_MASK;

	if ( drawId==SSTATE_DRAW_SLIDER ) {

		switch ( mouseId ) {
			case SSTATE_MOUSE_NORMAL:
				bChanged |= SetButtonState( pbuttonSlider, BSTATE_MOUSE_NORMAL );
				break;
			case SSTATE_MOUSE_OVER:
				bChanged |= SetButtonState( pbuttonSlider, BSTATE_MOUSE_OVER );
				break;
			case SSTATE_MOUSE_PRESS:
				bChanged |= SetButtonState( pbuttonSlider, BSTATE_MOUSE_PRESS );
				break;
		}

	}

	return bChanged;
}


int CSkin::DrawScrollbar(HDC hDC, int scrollbarId, int drawId)
{
	if ( !IsDefined() ) return FALSE;

	if ( scrollbarId<0 || scrollbarId>=countScrollbars || !hDC )
		return FALSE;

	stScrollbar *pscrollbar = pScrollbar + scrollbarId;
	if ( !pscrollbar->bIsDefined ) return FALSE;

	if ( drawId==DRAW_SCROLLBAR_BAR || drawId==DRAW_SCROLLBAR_BOTH )
	{
		if ( !pscrollbar->countResizeRects || 
			  (sizeCurr.cx==sizeOrg.cx && sizeCurr.cy==sizeOrg.cy) )
			DrawButton(hDC, &pscrollbar->buttonBar );

		else {
			//skin has been resized so we must draw resized scrollbar

			struct stRECT { LONG x1, y1, x2, y2; };
			struct stRECTX { LONG x, y1, none, y2; };
			struct stRECTY { LONG x1, y, x2, none; };

			HDC hdcSource = CreateCompatibleDC( hDC );
			HBITMAP hbm = pImage[pscrollbar->buttonBar.pcurr->imgId ].hBitmap;
			int iii = (int)SelectObject( hdcSource, 
				 pImage[ pscrollbar->buttonBar.pcurr->imgId ].hBitmap );

			int cx = (sizeCurr.cx - sizeOrg.cx), 
				cy = (sizeCurr.cy - sizeOrg.cy);

			stButtonState *SRC = (stButtonState*)pscrollbar->buttonBar.pcurr;
			stRECT *POS = (stRECT*)&pscrollbar->rcPosition;
			stRECTX *TB = (stRECTX*)pscrollbar->rcResize; //TOP & BOTTOM
			int count = pscrollbar->countResizeRects;


			int x=0, y=SRC->y1, w=0, h=SRC->y2-y;

			int xOffset = 0, cxdiv = cx/count, cxmod = cx%count;
			for (int i=0; i<count; i++) {

				int dx = POS->x1 + (TB->x-POS->x1)+xOffset, 
					dy = TB->y1, dw = 1, dh = TB->y2 - TB->y1,
					sx = SRC->x1+(TB->x-POS->x1), sy = SRC->y1;

				for (int j=0; j<(cxdiv+cxmod); j++)
					BitBlt( hdcBackground, dx++, dy+cy, dw, dh, 
							hdcSource, sx, sy, SRCCOPY );

				x = TB->x-POS->x1;
				TB++;
				xOffset += cxdiv+cxmod; cxmod = 0;
		
				if ((i+1)<count) w = TB->x-x -1;
				else w = (POS->x2-POS->x1) - x;
				if ( x+xOffset < 0) {
					int dx = -(x+xOffset);
					w -= dx; x += dx;
					if (w<0) w=0;
				}
				BitBlt( hdcBackground, x+xOffset+POS->x1, dy+cy, w, h, 
									hdcSource, x+SRC->x1, y, SRCCOPY );
			}
			TB = (stRECTX*)pscrollbar->rcResize;
			BitBlt( hdcBackground, POS->x1, POS->y1+cy, 
							TB->x-POS->x1, TB->y2-TB->y1, 
								hdcSource, SRC->x1, SRC->y1, SRCCOPY );

			DeleteDC( hdcSource );

		}
	}

	if ( drawId==DRAW_SCROLLBAR_SLIDER || drawId==DRAW_SCROLLBAR_BOTH )
	{
		stButton *pslider = &pscrollbar->buttonSlider;
		stButtonState *ps = &pscrollbar->prevSliderPos;


		if ( ps->w && ps->h && 
			  (ps->x1!=pslider->x1 || ps->y1!=pslider->y1) ) 
		{
			BitBlt(hDC, ps->x1, ps->y1, ps->w, ps->h,
						hdcBackground, ps->x1, ps->y1, SRCCOPY );
		}

		DrawButton(hDC, &pscrollbar->buttonSlider, DRAW_BUTTON_SCREEN );
	}

	return TRUE;
}

int CSkin::DrawScrollbar(HWND hWnd, int scrollbarId, int drawId)
{
	if ( !IsDefined() ) return FALSE;

	if (!hWnd) return FALSE;

	int ret;

	HDC hDC = GetDC(hWnd);
	if (!hDC ) return FALSE;

	ret = DrawScrollbar(hDC, scrollbarId, drawId);

	ReleaseDC(hWnd, hDC);

	return ret;
}

int CSkin::AddImage(int imgId, char *pszImagename, HWND hWnd,
							  char *pszSkindir, cArchive *pSkinZIP,
							  BOOL bCreateRgn = 0)
							  
{

	if ( !pszSkindir && !pSkinZIP )
		return FALSE;

	if (pSkinZIP) {

		if ( !pImage[imgId].LoadImage(pszImagename, pSkinZIP) ) 
			return FALSE;

	} else {

		char szPath[_MAX_PATH];
		_makepath( szPath, NULL, pszSkindir, pszImagename, NULL );

		if ( !pImage[imgId].LoadImage(szPath) ) 
			return FALSE;

	}

	if (imgId == IMAGE_BACKGROUND ) {
		sizeOrg.cx = pImage[imgId].w;
		sizeOrg.cy = pImage[imgId].h;
		sizeCurr = sizeOrg;

		CreateRgn( &pImage[imgId] );
	}

	pImage[imgId].Create1bppAlpha();
	pImage[imgId].ConvertToBitmap(hWnd);

	return TRUE;
}


int CSkin::DrawButton(HDC hDC, stButton *pbutton, int drawId)
{
	if ( !IsDefined() ) return FALSE;

	if ( !pbutton || !pbutton->pcurr->isDefined || !hDC )
		return FALSE;

	int ret = TRUE;

	stButton *pdest = pbutton;
	stButtonState *pstateSrc = pbutton->pcurr;
	
	HDC hSourceDC = CreateCompatibleDC(hDC);
	SelectObject(hSourceDC, pImage[ pstateSrc->imgId ].hBitmap );

	if ( drawId==DRAW_BUTTON_SCREEN || drawId==DRAW_BUTTON_BOTH )
		ret &= BitBlt(hDC, pdest->x1, pdest->y1,
							pstateSrc->w, pstateSrc->h,
							hSourceDC, pstateSrc->x1, pstateSrc->y1, SRCCOPY );

	if ( drawId==DRAW_BUTTON_BACKGROUND || drawId==DRAW_BUTTON_BOTH )
		ret &= BitBlt(hdcBackground, pdest->x1, pdest->y1,
							pstateSrc->w, pstateSrc->h,
							hSourceDC, pstateSrc->x1, pstateSrc->y1, SRCCOPY );


	DeleteDC(hSourceDC);

	return ret;
}

int CSkin::DrawButton(HWND hWnd, stButton *pbutton, int drawId)
{
	if (!hWnd) return FALSE;

	int ret;

	HDC hDC = GetDC(hWnd);
	if (!hDC ) return FALSE;

	ret = DrawButton(hDC, pbutton);

	ReleaseDC(hWnd, hDC);

	return ret;
}

int CSkin::DrawButton(HDC hDC, int buttonId, int drawId)
{
	if ( buttonId<0 || buttonId>=countButtons || !hDC )
		return FALSE;

	int ret = DrawButton( hDC, (pButton+buttonId) );

	return ret;
}

int CSkin::DrawButton(HWND hWnd, int buttonId, int drawId)
{

	if ( buttonId<0 || buttonId>=countButtons || !hWnd )
		return FALSE;

	int ret = DrawButton( hWnd, (pButton+buttonId) );

	return ret;

}

int CSkin::SetButtonState(int buttonId, int buttonState)
{
	if (buttonId<0 || buttonId>countButtons)
		return FALSE;

	return SetButtonState( (pButton+buttonId), buttonState );
}


// return TRUE when button must be redraw after setting new state
int CSkin::SetButtonState(stButton *pbutton, int buttonState)
{
	if ( !IsDefined() ) return FALSE;

	BOOL bChanged = FALSE;

	// first state must be defined
	if ( !pbutton || !pbutton->bIsFirstDef ) return bChanged;

	// we always can set bAllowSwitch
	if ( buttonState&BSTATE_ALLOW_SWITCH ) pbutton->bAllowSwitch = TRUE;
	if ( buttonState&BSTATE_DENIED_SWITCH ) pbutton->bAllowSwitch = FALSE;

	// if flag BSTATE_ENABLE present enable button by selecting
	//  first draw and mouse state, then proceed other flags
	// if flag BSTATE_DISABLE present disable button by selecting
	//  disabled draw state (if don't exist first draw and mouse state),
	//  than return (don't proceed any additional flags)
	if ( buttonState&BSTATE_ENABLED && pbutton->bEnabled==FALSE ) {

		stButtonState *pcurr = pbutton->pcurr;

		pbutton->pnorm = pbutton->norm;
		pbutton->pover = pbutton->over;
		pbutton->ppress = pbutton->press;
		pbutton->pcurr = pbutton->pnorm;

		pbutton->drawId = BSTATE_DRAW_FIRST;
		pbutton->mouseId = BSTATE_MOUSE_NORMAL;
		pbutton->bEnabled = TRUE;

		if (pcurr != pbutton->pcurr ) bChanged = TRUE;
	} else
	if ( buttonState&BSTATE_DISABLED && pbutton->bEnabled==TRUE ) {

		stButtonState *pcurr = pbutton->pcurr;

		if ( pbutton->bIsDisabledDef ) pbutton->pcurr = &pbutton->disabled;
		else pbutton->pcurr = pbutton->norm;

		pbutton->drawId = BSTATE_DRAW_DISABLED;
		pbutton->bEnabled = FALSE;

		if (pcurr != pbutton->pcurr ) bChanged = TRUE;

	}

	// return if button disabled
	if ( !pbutton->bEnabled ) return bChanged;

	//switch button
	if ( (buttonState & BSTATE_SWITCH) && pbutton->bAllowSwitch ) {

		if ( pbutton->drawId==BSTATE_DRAW_FIRST && pbutton->bIsSecondDef ) {

			pbutton->drawId = BSTATE_DRAW_SECOND;
			pbutton->pnorm = pbutton->norm+1;
			pbutton->pover = pbutton->over+1;
			pbutton->ppress = pbutton->press+1;

			bChanged = TRUE;

		} else
		if ( pbutton->drawId==BSTATE_DRAW_SECOND && pbutton->bIsFirstDef ) {

			pbutton->drawId = BSTATE_DRAW_FIRST;
			pbutton->pnorm = pbutton->norm;
			pbutton->pover = pbutton->over;
			pbutton->ppress = pbutton->press;

			bChanged = TRUE;
		}

		switch ( pbutton->mouseId ) {
			case BSTATE_MOUSE_NORMAL:
				pbutton->pcurr = pbutton->pnorm; break;
			case BSTATE_MOUSE_OVER:
				pbutton->pcurr = pbutton->pover; break;
			case BSTATE_MOUSE_PRESS:
				pbutton->pcurr = pbutton->ppress; break;

		}

	}

	// select state drawId and mouseId
	// drawId is selected only if is defined
	// mouseId select BSTATE_MOUSE_NORMAL if requested mouse state
	//  isn't defined
	int drawId = buttonState & BSTATE_DRAW_MASK,
		 mouseId = buttonState & BSTATE_MOUSE_MASK;

	switch ( drawId ) {

		case BSTATE_DRAW_FIRST:

			if ( !pbutton->bIsFirstDef || pbutton->drawId==drawId) break;

			pbutton->drawId = BSTATE_DRAW_FIRST;
			pbutton->pnorm = pbutton->norm;
			pbutton->pover = pbutton->over;
			pbutton->ppress = pbutton->press;
			if ( !mouseId ) {
				pbutton->pcurr = pbutton->pnorm;
				pbutton->mouseId = BSTATE_MOUSE_NORMAL;
			}
			bChanged = TRUE;
			break;

		case BSTATE_DRAW_SECOND:

			if ( !pbutton->bIsSecondDef || pbutton->drawId==drawId) break;

			pbutton->drawId = BSTATE_DRAW_SECOND;
			pbutton->pnorm = pbutton->norm+1;
			pbutton->pover = pbutton->over+1;
			pbutton->ppress = pbutton->press+1;
			if ( !mouseId ) {
				pbutton->pcurr = pbutton->pnorm;
				pbutton->mouseId = BSTATE_MOUSE_NORMAL;
			}
			bChanged = TRUE;
			break;
	}


	switch ( mouseId ) {
		case BSTATE_MOUSE_PRESS:
			if ( pbutton->ppress->isDefined ) {

				if ( mouseId == pbutton->mouseId ) break;

				pbutton->pcurr = pbutton->ppress;
				pbutton->mouseId = BSTATE_MOUSE_PRESS;
				bChanged = TRUE;
				break;

			} else mouseId = BSTATE_MOUSE_NORMAL;

		case BSTATE_MOUSE_OVER:
			if ( pbutton->pover->isDefined ) {

				if ( mouseId == pbutton->mouseId ) break;

				pbutton->pcurr = pbutton->pover;
				pbutton->mouseId = BSTATE_MOUSE_OVER;
				bChanged = TRUE;
				break;

			} else mouseId = BSTATE_MOUSE_NORMAL;

		case BSTATE_MOUSE_NORMAL:
			if ( !pbutton->pnorm->isDefined || mouseId == pbutton->mouseId ) 
				break;

			pbutton->pcurr = pbutton->pnorm; 
			pbutton->mouseId = BSTATE_MOUSE_NORMAL;
			bChanged = TRUE;
			break;
	}

	return bChanged;
}

int CSkin::IsButtonSwitched( int buttonId )
{
	if ( !IsDefined() ) return FALSE;

	if ( buttonId<0 || buttonId>=countButtons)
		return FALSE;

	if ( pButton[buttonId].drawId == BSTATE_DRAW_SECOND )
		return TRUE;

	return FALSE;
}

int CSkin::IsButtonEnabled( int buttonId )
{
	if ( !IsDefined() ) return FALSE;

	if ( buttonId<0 || buttonId>=countButtons)
		return FALSE;

	return pButton[buttonId].bEnabled;
}

int CSkin::AddStatus( char *pszId, int x, int y, int w, int h, 
								int sizeFont, DWORD col )
{

	for (int i=0; i<countStatuses && _stricmp(pszId, ppszStatusId[i]); i++) ;

	if (i>=countStatuses) return FALSE;

	stStatus *ps = pStatus+i;

	ps->col = col;
	ps->rcArea.left = x;  ps->rcArea.top = y;
	ps->rcArea.right = x+w; ps->rcArea.bottom = y+h;
	ps->rcAreaOrg = ps->rcArea;

	HDC hDC = GetDC( GetDesktopWindow() );
	LOGFONT logfont= {

		-MulDiv(sizeFont, GetDeviceCaps(hDC, LOGPIXELSY), 72), //lfHeight
		0, 0, 0,							//lfWidth, lfEscapement, lfOrientation
		0,									//lfWeight(def)
		FALSE, FALSE, FALSE,			//lfItalic, lfUnderline, lfStrikeOut
		DEFAULT_CHARSET,				//lfCharSet 
		OUT_DEFAULT_PRECIS,			//lfOutPrecision 
		CLIP_DEFAULT_PRECIS,			//lfClipPrecision 
		DEFAULT_QUALITY,				//lfQuality 
		DEFAULT_PITCH|FF_DONTCARE,	//lfPitchAndFamily 
		"MS Sans Serif"				//lfFaceName

	};
	ReleaseDC( GetDesktopWindow(), hDC );

	

	ps->hFont = CreateFontIndirect( &logfont );


	return TRUE;
}

// Set status text.
// Return TRUE if text changed, if not FALSE.
int CSkin::SetStatusText( HDC hDC, int statusId, char *pszText )
{
	if ( statusId<0 || statusId>=countStatuses )
		return FALSE;

	stStatus *ps = pStatus+statusId;

	if ( !strcmp(ps->sz, pszText) ) return FALSE;

	ps->sz[255] = 0;
	strncpy( ps->sz, pszText, 255 );

	if ( hDC ) DrawStatus( hDC, statusId );

	return TRUE;

}
// Set status text.
// Return TRUE if text changed, if not FALSE.
int CSkin::SetStatusText( HWND hWnd, int statusId, char *pszText )
{
	if ( statusId<0 || statusId>=countStatuses )
		return FALSE;

	HDC hDC = GetDC( hWnd );
	int ret = SetStatusText( hDC, statusId, pszText );
	ReleaseDC( hWnd, hDC );

	return ret;

}

int CSkin::DrawStatus( HDC hDC, int statusId, BOOL bRedrawBackground )
{
	if ( statusId<0 || statusId>=countStatuses || !hDC ||
			!hDC )
		return FALSE;

	stStatus *ps = pStatus+statusId;

	int x = ps->rcArea.left, y = ps->rcArea.top,
		 w = ps->rcArea.right-x, h = ps->rcArea.bottom-y;

	if ( !w || !h ) return FALSE;

	if ( bRedrawBackground )
		BitBlt(hDC, x,y,w,h, hdcBackground, x,y, SRCCOPY );

	if ( !*ps->sz ) return TRUE;

	SetBkMode( hDC, TRANSPARENT );
	SetTextColor( hDC, ps->col );
	SelectObject( hDC, ps->hFont );

	ExtTextOut( hDC, x,y, ETO_CLIPPED, &ps->rcArea, 
					ps->sz, strlen(ps->sz), NULL );


	return TRUE;
}
int CSkin::DrawStatus( HWND hWnd, int statusId, BOOL bRedrawBackground )
{
	if ( statusId<0 || statusId>=countStatuses )
		return FALSE;

	HDC hDC = GetDC( hWnd );
	int ret = DrawStatus( hDC, statusId, bRedrawBackground );
	ReleaseDC( hWnd, hDC);

	return ret;
}



HDC CSkin::CreateBackground( HWND hWnd )
{
	sizeScreen.cx = GetSystemMetrics( SM_CXSCREEN )+16,
	sizeScreen.cy = GetSystemMetrics( SM_CYSCREEN )+16;
	sizeMaximal = sizeScreen;

	HDC hDC = GetDC( hWnd );

	if ( !hBackground ) {

		hBackground = CreateCompatibleBitmap( hDC, sizeScreen.cx, sizeScreen.cy );

		hdcBackground = CreateCompatibleDC( hDC );
		SelectObject( hdcBackground, hBackground );

		SetBitmapDimensionEx( hBackground, sizeScreen.cx, sizeScreen.cy, NULL );

	} else {

		SIZE size;
		GetBitmapDimensionEx( hBackground, &size );

		if ( size.cx!=sizeScreen.cx || size.cy!=sizeScreen.cy ) {
			
			DeleteDC( hdcBackground );
			DeleteObject( hBackground );

			hBackground = CreateCompatibleBitmap( hDC, sizeScreen.cx, sizeScreen.cy );

			hdcBackground = CreateCompatibleDC( hDC );
			SelectObject( hdcBackground, hBackground );

			SetBitmapDimensionEx( hBackground, sizeScreen.cx, sizeScreen.cy, NULL );

		}
	}


	ReleaseDC( hWnd, hDC );
	
	if ( !hBackground ) 
		return NULL;

	return hdcBackground;
}

//background must be created for each visible skin
//if you create several skins and only one from it is visible
// at one time they should share background
void CSkin::ShareBackgroundWith(CSkin *pSkin)
{
	pSkin->hdcBackground = hdcBackground;
	pSkin->sizeMaximal = sizeMaximal;
}

BOOL CSkin::Initialize()
{
	if ( !IsDefined() ) return FALSE;


	stButton *pb = pButton;
	for (int i=0; i<countButtons; i++) {
		SetRect( &pb->rcPosition, pb->x1, pb->y1, pb->x2, pb->y2 );
		pb++;
	}

	stScrollbar *ps = pScrollbar;
	for (i=0; i<countScrollbars; i++) {
		SetRect( &ps->rcPosition, ps->buttonBar.x1, ps->buttonBar.y1,
					ps->buttonBar.x2, ps->buttonBar.y2 );
		ps++;
	}

	sizeScreen.cx = GetSystemMetrics( SM_CXSCREEN )+16,
	sizeScreen.cy = GetSystemMetrics( SM_CYSCREEN )+16;

	if ( !sizeMinimal.cx || !sizeMinimal.cy ) {
		sizeMinimal.cx = sizeOrg.cx/2;
		sizeMinimal.cy = sizeOrg.cy/2;
	}

	sizeMaximal = sizeScreen;


	// 'repair' resize rectangles (when rectangles are set, oryginal 
	//size of skin is unknown so bottom and right must be set here)
	for (i=0; i<countRectsBottom; i++)
		if (rcResizeBottom[i].top==0) {
			rcResizeBottom[i].top = sizeOrg.cy -  rcResizeBottom[i].bottom;
			rcResizeBottom[i].bottom = sizeOrg.cy;
		}
	for (i=0; i<countRectsRight; i++)
		if (rcResizeRight->left==0) {
			rcResizeRight[i].left = sizeOrg.cx -  rcResizeRight[i].right;
			rcResizeRight[i].right = sizeOrg.cx;
		}

	if ( countRectsTop && countRectsBottom && 
		 countRectsLeft && countRectsRight )
		allowResize = TRUE;
	else allowResize = FALSE;


	//define PlayArea from resizeDefor as a half of skin size
	RECT *prc = &rcPlayArea;
	if ( (prc->right-prc->left<=0) || (prc->bottom-prc->top<=0) )
	{
	
		prc->left = rcResizeLeft->right;
		prc->top = rcResizeTop->bottom;
		prc->right = rcResizeRight->left;
		prc->bottom = rcResizeBottom->top;

		//if resizeArea isn't defined set size PlayArea as
		//half skin size and place it in middle of skin
		if (!allowResize) {
			prc->left = sizeOrg.cx/4;
			prc->top = sizeOrg.cy/4;
			prc->right = prc->left + sizeOrg.cx/2;
			prc->bottom = prc->top + sizeOrg.cy/2;
		}

		rcPlayAreaOrg = rcPlayArea;
	}


	if ( !allowResize ) {
		// Set skin two resize var, which is necessary to
		//  draw background when call Resize() and skin don't
		//  allow resize.
		rcResizeTop->right = sizeOrg.cx;
		rcResizeTop->bottom = sizeOrg.cy;
	}


	return TRUE;
}

BOOL CSkin::GetSkinSize(long *width, long *height)
{
	*width = sizeCurr.cx;
	*height = sizeCurr.cy;

	if ( !IsDefined() ) return FALSE;

	return TRUE;
}


void CSkin::SetAllowMove(BOOL bAllow)
{
	allowMoveWindow = bAllow;
}

BOOL CSkin::AllowMove()
{
	return allowMoveWindow;
}
void CSkin::SetAllowResize(BOOL bAllow)
{
	allowResize = bAllow;
}

BOOL CSkin::AllowResize()
{
	return allowResize;
}

void CSkin::SetPlayArea( RECT *prc )
{
	CopyRect( &rcPlayArea, prc );
	rcPlayAreaOrg = rcPlayArea;
}

void CSkin::SetPlayArea( int x, int y, int w, int h )
{
	RECT rc = { x,y, x+w, y+h };
	rcPlayArea = rc;
	rcPlayAreaOrg = rcPlayArea;
}

void CSkin::SetMinimalSize( int w, int h )
{
	sizeMinimal.cx = w;
	sizeMinimal.cy = h;
}


BOOL CSkin::DrawSkin(HDC hDC, BOOL bErasePaintArea, BOOL bOnlyBackground)
{
	if ( !IsDefined() ) return FALSE;

	if ( !hDC ) return FALSE;

	CRect rc = rcPlayArea;

	int px = rcPlayArea.left, py = rcPlayArea.top,
		 pw = rcPlayArea.right-px, ph = rcPlayArea.bottom-py,
		 sw = sizeCurr.cx, sh = sizeCurr.cy;

	if ( bErasePaintArea )
		BitBlt( hDC, px, py, pw, ph, NULL, 0,0, BLACKNESS );

	if ( pw<sw && ph<sh ) {

		//   -------------------------
		BitBlt( hDC, 0, 0, sw, py, hdcBackground, 0, 0, SRCCOPY );
		//   |
		BitBlt( hDC, 0, py, px, ph, hdcBackground, 0, py, SRCCOPY );
		//                           |
		BitBlt( hDC, px+pw, py, sw-px-pw, ph, hdcBackground, px+pw, py, SRCCOPY );
		//   -------------------------
		BitBlt( hDC, 0, py+ph, sw, sh-py-ph, hdcBackground, 0, py+ph, SRCCOPY );

	} else {
		BitBlt( hDC, 0, 0, sw, sh, hdcBackground, 0,0, SRCCOPY );
	}


	if ( !bOnlyBackground ) {

		for (int i=0; i<countScrollbars; i++)
			DrawScrollbar( hDC, i, DRAW_SCROLLBAR_SLIDER);

		for (i=0; i<countStatuses; i++ )
			DrawStatus( hDC, i, FALSE );
	}

	return TRUE;
}

BOOL CSkin::DrawSkin(HWND hWnd, BOOL bErasePaintArea, BOOL bOnlyBackground)
{
	if ( !hWnd ) return FALSE;

	HDC hDC = GetDC( hWnd );
	DrawSkin( hDC, bErasePaintArea, bOnlyBackground );
	ReleaseDC(hWnd, hDC);	

	return TRUE;
}

BOOL CSkin::IsDefined()
{
	if ( hRgn ) return TRUE;
	else return FALSE;
}

RECT CSkin::GetEqualPlayAreas( CSkin *pSkin )
{

	int dx = pSkin->sizeOrg.cx - ( pSkin->rcPlayAreaOrg.right - pSkin->rcPlayAreaOrg.left ),
		dy = pSkin->sizeOrg.cy - ( pSkin->rcPlayAreaOrg.bottom - pSkin->rcPlayAreaOrg.top );

	RECT rc = { 
					rcPlayAreaOrg.left - pSkin->rcPlayAreaOrg.left,
					rcPlayAreaOrg.top - pSkin->rcPlayAreaOrg.top,
					dx + ( rcPlayArea.right - rcPlayArea.left ),
					dy + ( rcPlayArea.bottom - rcPlayArea.top )
	}; 
	rc.bottom += rc.top;
	rc.right += rc.left;

	return rc;

}

RECT CSkin::GetPlayArea()
{
	return rcPlayArea;
}