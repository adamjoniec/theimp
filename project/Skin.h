#if !defined( FILE_SKIN_H )
#define FILE_SKIN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "skindefinition.h"

#define SAFE_DELETE(p) if (p) { delete p; p=NULL; }
#define SAFE_DELETE_ARRAY(p) if (p) { delete[] p; p=NULL; }

enum BUTTONSTATEID {

	// draw state
	BSTATE_DRAW_FIRST				= 1,			// -------------001	default
	BSTATE_DRAW_SECOND			= 2,			// -------------010
	BSTATE_DRAW_DISABLED			= 3,			// -------------011
	BSTATE_DRAW_MASK				= 7,			// 0000000000000111

	BSTATE_MOUSE_NORMAL			= 8,			// ----------001---	def
	BSTATE_MOUSE_OVER				= 16,			// ----------010---
	BSTATE_MOUSE_PRESS			= 24,			// ----------011---
	BSTATE_MOUSE_MASK				= 56,			// 0000000000111000

	// allow or denied switch
	BSTATE_ALLOW_SWITCH			= 0x40,
	BSTATE_DENIED_SWITCH			= 0x80,		//							def

	// enable or disable button
	// disable set BSTATE_DRAW_DISABLED
	// enable set DBSTATE_DRAW_FIRST and BSTATE_MOUSE_NORMAL
	BSTATE_ENABLED					= 0x100,		//							def
	BSTATE_DISABLED				= 0x200,

	// switch draw state from first to second or from second to first
	// but don't change MOUSE_STATE
	BSTATE_SWITCH					= 0x400,

	// used only once to set button's position when
	// loading skin and calling AddButton()
	BSTATE_DEFPOSITION			= 0x1000
};


enum SCROLLBARSTATEID {

	// Scrollbar is made up of two buttons: bar and slider.
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
	SSTATE_CORRECT_SLIDER		= 0x2000
};


enum DRAWSCROLLBARID {
	DRAW_SCROLLBAR_BAR = 0,
	DRAW_SCROLLBAR_SLIDER,
	DRAW_SCROLLBAR_BOTH
};


enum DRAWBUTTONID {
	DRAW_BUTTON_SCREEN = 0,
	DRAW_BUTTON_BACKGROUND,
	DRAW_BUTTON_BOTH
};

enum BUTTONSHAPEID {
	SHAPE_RECT = 0,
	SHAPE_SINGLEALPHA,
	SHAPE_MULTIALPHA
};

class CSkin {


	//skin's size
	SIZE sizeCurr, sizeOrg;

	//screen resolution
	SIZE sizeScreen;

	BOOL allowMoveWindow;	//constant position ? ON:OFF


public:

	//skin region (window shape)
	HRGN		hRgn;
	RGNDATA	*prgnData;

	//skin background (can be shared between many skins)
	HBITMAP hBackground;
	HDC hdcBackground;


	//resize rectangles
	#define MAX_RESIZE_RECTS 16

	BOOL	allowResize;
	RECT	rcResizeLeft[ MAX_RESIZE_RECTS ],
			rcResizeRight[ MAX_RESIZE_RECTS ],
			rcResizeTop[ MAX_RESIZE_RECTS ],
			rcResizeBottom[ MAX_RESIZE_RECTS ];
	SIZE  sizeMinimal, sizeMaximal;

	int	countRectsLeft, countRectsRight, 
			countRectsTop, countRectsBottom;

	//play and status area
	RECT	rcPlayArea, rcPlayAreaOrg;

	struct stStatus {
		HFONT		hFont;
		DWORD		col;
		RECT		rcArea, rcAreaOrg;

		char		sz[256];

		stStatus():hFont(NULL), col(0xFFFFFF) { 
			SetRectEmpty(&rcArea); SetRectEmpty(&rcAreaOrg); *sz = 0; };
	};


public:

	cGImage		*pImage;
	char		**ppszImageId;
	int			countImages;


	struct stButtonState { 

		BOOL isDefined;
		int x1, y1, x2, y2, w, h, imgId;

		stButtonState(): isDefined(FALSE), x1(0), y1(0), x2(0), y2(0), 
							  w(0), h(0), imgId(0) {} 
	};

	struct stButton {
		//state first - enabled first button
		//state second - enabled second button
		//state third - button disabled

		int x1, y1, x2, y2, w, h;
		stButtonState	norm[2], press[2], over[2], disabled,
							*pnorm, *ppress, *pover,
							*pcurr;

		BOOL bIsFirstDef, bIsSecondDef, bIsDisabledDef;

		BOOL bEnabled, bAllowSwitch;
		int drawId, mouseId, shapeId;

		//oryginal position of button
		RECT rcPosition;	

		stButton(): x1(0), y1(0), x2(0), y2(0), w(0), h(0),
						pnorm(norm), ppress(press), pover(over),
						pcurr(norm), 
						bIsFirstDef(FALSE),
						bIsSecondDef(FALSE),
						bIsDisabledDef(FALSE),
						bEnabled(TRUE), 
						bAllowSwitch(FALSE),
						drawId(BSTATE_DRAW_FIRST),
						mouseId(BSTATE_MOUSE_NORMAL),
						shapeId(SHAPE_RECT)
						{ SetRectEmpty(&rcPosition); }
	};

	struct stScrollbar {
		// buttonBar is always in state DRAW_FIRST, MOUSE_NORMAL
		// buttonSlider is always in state DRAW_FIRST

		stButton buttonBar, buttonSlider;
		stButtonState prevSliderPos;

		BOOL bEnabled, bIsDefined;
		int sliderDrawId, sliderMouseId,
			 barDrawId, barMouseId,
			 orientationId;	// defined only SSTATE_HORIZONTAL

		int marginLeft, marginRight;

		double dValue;	//position of slider ( from 0 to 100 )
			
		//oryginal position of scrollbar bar
		RECT rcPosition;

		//rectangles uses when resize scrollbar (only horizontal)
		int countResizeRects;
		RECT rcResize[ MAX_RESIZE_RECTS ];


		stScrollbar(): bEnabled(TRUE), bIsDefined(FALSE),
							sliderDrawId(SSTATE_DRAW_SLIDER),
							barDrawId(SSTATE_DRAW_BAR), barMouseId(SSTATE_MOUSE_NORMAL),
							orientationId(SSTATE_HORIZONTAL),
							marginLeft(0), marginRight(0),
							countResizeRects(0), dValue(0) {}
	};

	stButton	*pButton;
	char		**ppszButtonId;
	int			countButtons;

	stScrollbar *pScrollbar;
	char		**ppszScrollbarId;
	int		countScrollbars;

	stStatus			*pStatus;
	char				**ppszStatusId;
	int				countStatuses;


public:


	void SetLocation(char *pszDirectory, cArchive *pArchiveZIP, 
						BOOL bIsZIP );

	//images functions
	int	AddImage(int imgId, char *pszImagename, HWND hWnd,
					char *pszSkindir, cArchive *pSkinZIP, BOOL bCreateRgn);
	int	GetImageId(char *pszId);

	//buttons function
	int AddButton(int buttonId, int buttonState, int x, int y, 
					int w, int h, int imageId, int shapeId);
	int GetButtonId(char *pszId);
	int GetButtonIdFromPoint(POINT pt);
	int GetButtonState(char *pszState);
	int SetButtonState(stButton *pbutton, int buttonState);
	int SetButtonState(int buttonId, int buttonState);

	int DrawButton(HDC hDc, int buttonId, int drawId=DRAW_BUTTON_BOTH);
	int DrawButton(HWND hWnd, int buttonId, int drawId=DRAW_BUTTON_BOTH);
	int DrawButton(HDC hDC, stButton *pbutton, int drawId=DRAW_BUTTON_BOTH);
	int DrawButton(HWND hWnd, stButton *pbutton, int drawId=DRAW_BUTTON_BOTH);

	int IsButtonSwitched( int buttonId );
	int IsButtonEnabled( int buttonId );

	//scrollbars functions
	int AddScrollbar( int scrollbarId, int scrollbarState,
							int x, int y, int w, int h, int imageId, 
							int marginLeft=0, int marginRight=0);
	int GetScrollbarId(char *pszId);
	int GetScrollbarIdFromPoint(POINT pt);
	int GetScrollbarState(char *pszState);

	int DrawScrollbar(HDC hDC, int scrollbarId, int drawId=DRAW_SCROLLBAR_SLIDER);
	int DrawScrollbar(HWND hWnd, int scrollbarId, int drawId=DRAW_SCROLLBAR_SLIDER);

		
	int OnScrollbarMove( int scrollbarId, POINT pt);
	int SetScrollbarState( int scrollbarId, int scrollbarState );

	int SetScrollbarValue( int scrollbarId, double val );
	int GetScrollbarValue( int scrollbarId, double *pVal);

	int IsScrollbarEnabled( int scrollbarId );

	// Status area.
	int AddStatus( char *pszId, int x, int y, int w, int h, 
								int size, DWORD col );
	int SetStatusText( HDC hDC, int statusId, char *pszText );
	int SetStatusText( HWND hWnd, int statusId, char *pszText );
	int DrawStatus( HDC hDC, int statusId, BOOL bRedrawBackground=TRUE );
	int DrawStatus( HWND hWnd, int statusId, BOOL bRedrawBackground=TRUE );

	//region functions
	HRGN CreateRgn(cGImage *pImage);
	BOOL ResizeRgn(long *newWidth, long *newHeight, BOOL bForceResize = FALSE);


	//global functions
	HDC CreateBackground(HWND hWnd );
	void ShareBackgroundWith( CSkin *pSkin);
	RECT	GetEqualPlayAreas( CSkin *pSkin );
	RECT	GetPlayArea();




	//skin initialize after loading from XML config
	BOOL Initialize();

	BOOL Resize(HWND hWnd, long *newWidth, long *newHeight, 
					BOOL bForceResize = FALSE);
	BOOL ResizePlayArea(HWND hWnd, int *nwPlayArea, int *nhPlayArea );

	BOOL GetSkinSize(long *dwWidth, long *dwHeight);

	BOOL DrawSkin(HDC hDC, BOOL bErasePaintArea=TRUE, BOOL bOnlyBackground=FALSE);
	BOOL DrawSkin(HWND hWnd, BOOL bErasePaintArea=TRUE, BOOL bOnlyBackground=FALSE);


	void SetAllowMove(BOOL bAllow);
	BOOL AllowMove();
	void SetAllowResize(BOOL bAllow);
	BOOL AllowResize();
	void SetPlayArea( RECT *prc );
	void SetPlayArea( int x, int y, int w, int h );

	void SetMinimalSize( int w, int h);

	BOOL	IsDefined();

	//constructor must be defined here because it uses static 
	//string variables (look skin definitions)
	CSkin::CSkin() 
	{
		countImages = sizeof(imgId) / sizeof (char*);
		pImage = new cGImage[countImages];
		ppszImageId = new char*[countImages];
		for (int i=0; i<countImages; i++) ppszImageId[i] = imgId[i];

		countButtons = sizeof(buttonId) / sizeof (char*);
		pButton = new stButton[countButtons];
		ppszButtonId = new char*[countButtons];
		for (i=0; i<countButtons; i++) ppszButtonId[i] = buttonId[i];

		countScrollbars = sizeof( scrollbarId ) / sizeof (char*);
		pScrollbar = new stScrollbar[ countScrollbars ];
		ppszScrollbarId = new char*[countScrollbars];
		for (i=0; i<countScrollbars;i++) ppszScrollbarId[i] = scrollbarId[i];

		countStatuses = sizeof( statusId ) / sizeof (char*);
		pStatus = new stStatus[ countStatuses ];
		ppszStatusId = new char*[ countStatuses ];
		for (i=0; i<countStatuses;i++) ppszStatusId[i] = statusId[i];


		allowResize = FALSE;
		countRectsLeft = 0; countRectsRight = 0;
		countRectsTop = 0; countRectsBottom = 0;

		SIZE sizeNull = {0,0};
		sizeCurr = sizeNull; sizeOrg = sizeNull;
		sizeMinimal = sizeNull; sizeMaximal = sizeNull;

		RECT rcNULL = {0,0,0,0};			
		rcPlayArea = rcNULL; rcPlayAreaOrg = rcNULL;

		hRgn = NULL; prgnData = NULL;

		hBackground = NULL;
		hdcBackground = NULL;

		allowMoveWindow = TRUE;
	}

	CSkin::~CSkin() 
	{

		SAFE_DELETE_ARRAY(pImage); SAFE_DELETE_ARRAY(ppszImageId);
		SAFE_DELETE_ARRAY(pButton); SAFE_DELETE_ARRAY(ppszButtonId);
		SAFE_DELETE_ARRAY(pScrollbar); SAFE_DELETE_ARRAY(ppszScrollbarId);

		BYTE *pData = (BYTE*)prgnData; SAFE_DELETE_ARRAY(pData);
	}


};//END CSkin



#endif // !defined( FILE_SKIN_H )