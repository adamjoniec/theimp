// XPPlayer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "XPPlayer.h"
#include "MainWnd.h"
#include "config.h"
#include "language.h"
#include "accelerators.h"
#include "splashscreen.h"
#include "timeout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//void CheckExeSum(char *pszPath,char *pszName)
#define CheckExeSum(pszPath,pszName)\
{\
	char sz[_MAX_PATH+2]; _makepath(sz,"",pszPath,pszName,"exe");\
	FILE *f = fopen( sz, "rb" );\
	if ( f ) {\
		fseek(f,0,SEEK_END); int s=ftell(f)-4; fseek(f,0,SEEK_SET);\
		unsigned char *buff = new unsigned char [s]; \
		fread( buff, s ,1, f); \
		int filesum; fread( &filesum, 4, 1, f); fclose(f);\
		unsigned char *b = buff; int sum=0;\
		for (int i=0; i<s; i++,b++) sum += *b;\
		delete buff; if (sum!=filesum) return FALSE;\
	} else return FALSE;\
}\

/*#define CheckExeSum(pszPath,pszName)\
{\
	char sz[_MAX_PATH]; _makepath(sz,"",pszPath,pszName,"exe");\
	cFile *pFile = new cFile();\
	if ( pFile->Open((char*)sz,"r") ) {\
		int s = pFile->Size();\
		unsigned char *buff = new unsigned char [s]; pFile->Read( buff, s-4 );\
		int filesum; pFile->Read( &filesum, 4 );\
		delete pFile;\
		unsigned char *b = buff; int sum=0;\
		for (int i=0; i<s; i++,b++) sum += *b;\
		delete buff; if (sum!=filesum) return FALSE;\
	} else return FALSE;\
}\*/


  // Expire after 18.12.2002 .

	// Get current OS version.
	// If win9x get date from bios.
//void CheckDateExpired(CWnd *pParentWnd)

//{
#define ShowExpired {\
CSplashScreen *psplash = new CSplashScreen();\
psplash->Create(); Sleep( 6000 ); \
CTimeOut *pto = new CTimeOut();\
pto->DoModal(); delete pto; delete psplash;\
goto endCheckDateExpired; }

#define LIMIT 24400
#define CheckDateExpired \
{\
	\
	int day, month, year;\
	\
	OSVERSIONINFO osvi;\
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);\
	GetVersionEx (&osvi);\
	\
	if ( osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS )\
	{\
		int day, month, year;\
	\
		_outp(0x70,7); day = _inp(0x71);\
		day = ((day&0x0F))+(((day&0xF0)>>4)*10);\
	\
		_outp(0x70,8); month = _inp(0x71);\
		month = ((month&0x0F))+(((month&0xF0)>>4)*10);\
	\
		_outp(0x70,9); year = _inp(0x71);\
		year = 2000+((year&0x0F))+(((year&0xF0)>>4)*10);\
	\
		_asm {\
			push day\
			push month\
			push year\
			push day\
			push month\
			push year\
		}\
	\
		SYSTEMTIME st;\
		GetSystemTime( &st );\
	}\
	else \
	{\
	\
		int day, month, year;\
	\
		SYSTEMTIME st;\
		GetLocalTime( &st );\
		day = st.wDay;\
		month = st.wMonth;\
		year = st.wYear;\
	\
		_asm {\
			push day\
			push month\
			push year\
			push day\
			push month\
			push year\
		}\
	\
		GetSystemTime( &st );\
	\
	}\
	\
	_asm { \
		pop year\
		pop month\
		pop day\
	}\
	\
	srand( GetTickCount() );\
	int r1 = rand()%9,r2 = rand()%9,r3 = rand()%9,r4 = rand()%9,\
		i1=0,i2=0,i3=0,i4=0;\
	\
	int days=(year<<4)-(year<<2)+(month<<5)-(month<<1)+day;\
	\
	int limit = LIMIT;\
	int arrlimit[10], arrdays[10];\
	\
	int y,m,d;\
	_asm { \
		pop y\
		pop m\
		pop d\
	}\
	if ( days>LIMIT ) ShowExpired\
	\
	if ( days>limit ) goto showExpired1;\
	\
	for (i1=0; i1<r1; i1++) {\
	\
		if ( rand()%2 ) arrdays[i1] = days+i1;\
		else arrdays[i1] = (y<<4)-(y<<2)+(m<<5)-(m<<1)+d+i1;\
	\
	}\
	\
	if ( (y<<4)-(y<<2)+(m<<5)-(m<<1)+d+i1>limit ) ShowExpired\
	\
	while (i2<r2) {\
	\
		if ( i2%3 ) arrlimit[i2] = limit+i2;\
		else arrlimit[i2] = LIMIT+i2;\
	\
		i2++;\
	}\
	\
	do {\
		int d=arrdays[rand()%(5)+1], l=arrlimit[rand()%(5)+1];\
		if (rand()%2) d=(y<<4)-(y<<2)+(m<<5)-(m<<1)+d+i1;\
		if ( d>=l ) goto showExpired2;\
	\
		i3++;\
	} while (i3<r3);\
	\
	goto endCheckDateExpired;\
	\
	int expired;\
	showExpired1: expired = 1; ShowExpired\
	showExpired2: expired = 2; ShowExpired\
	\
	endCheckDateExpired:\
}\


/////////////////////////////////////////////////////////////////////////////
// CXPPlayerApp

BEGIN_MESSAGE_MAP(CXPPlayerApp, CWinApp)
	//{{AFX_MSG_MAP(CXPPlayerApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXPPlayerApp construction

CXPPlayerApp::CXPPlayerApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CXPPlayerApp object

CXPPlayerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CXPPlayerApp initialization

HWND hwndCtrl, hwndFs, hwndPlaylist, hwndSubtitles, hwndSubtitlesFs;


void CXPPlayerApp::ShowSplashScreen()
{
	CSplashScreen *psplash = new CSplashScreen();

	psplash->Create();
	Sleep( 2000 );

	delete psplash;
}

BOOL CXPPlayerApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	HANDLE hInstanceMutex = CreateMutex(NULL,TRUE,
										"The Imp Synchronization Mutex");

	if( GetLastError() == ERROR_ALREADY_EXISTS)
	{
		if ( hInstanceMutex ) CloseHandle( hInstanceMutex );

		HWND hPrevApp = FindWindow(NULL, "The Imp");
		if ( hPrevApp ) 
		{
			ShowWindow(hPrevApp, SW_RESTORE);
			SetForegroundWindow(hPrevApp);

			int len = strlen(m_lpCmdLine);
			if ( !len ) return 0;

			HANDLE handle = CreateFileMapping(  (HANDLE)0xFFFFFFFF, NULL, 
				PAGE_READWRITE, 0, len+1, "The Imp Shared File In Memory" );
			if ( !handle || GetLastError()== ERROR_ALREADY_EXISTS ) {
				CloseHandle( handle ) ;
				return 0;
			}
			char *psz = (char*)MapViewOfFile( handle, FILE_MAP_WRITE, 0, 0, 0 );

			if ( psz ) {

				strcpy( psz, m_lpCmdLine );

				SendMessage( hPrevApp, WM_COMMAND, 
					IDM_CALLBACK_OPENFROMMUTEX, NULL );

				UnmapViewOfFile( psz );
			}
			CloseHandle( handle );

		}

		return 0;
	}

	stConfig Options;

	// Get The Imp.exe path.
	char path[_MAX_PATH], pathtemp[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], 
		*pszPath = path, *pszcl = GetCommandLine();
	if (*pszcl=='"') pszcl++; while (*pszcl && *pszcl!='"' ) pszcl++; *pszcl=0;
	_splitpath( GetCommandLine(), drive, dir, NULL, NULL );
	_makepath( path, drive, dir, NULL, NULL ); if ( *pszPath == '"' ) pszPath++; 

	// Load config file.
	char szConfig[_MAX_PATH]; 
	_makepath( szConfig, NULL, pszPath, "config", NULL );
	if ( !ReadConfig( szConfig, &Options ) )
		ShowSplashScreen();

	// Set app path
	strcpy(Options.szAppPath, pszPath);

	// Set app name.
	strcpy( Options.szAppName, "The Imp" );

	if ( Options.modeIdStartWhenNoMovie ) 
		Options.modeId = Options.modeIdStartWhenNoMovie;
	

	InitLanguage();
	_makepath( pathtemp, NULL, Options.szAppPath, Options.szLanguageDirectory, NULL );
	_makepath( path, NULL, pathtemp, Options.szLanguageFileName, NULL );
	ChangeLang( path );

	// Check exe sum. Correct exe sum must be set after compiling app.
	CheckExeSum(Options.szAppPath, Options.szAppName);


	if ( Options.bRegisterOnStart )
		RegisterFiletypes( &Options, Options.szRegisteredExts );

	// Load accelerators.
	hAccel = CreateAcceleratorTable( accel, GetKeysCount() );


	// Create playlist.
	CPlaylist *pPlaylist = new CPlaylist( );
	pPlaylist->Create(Options.maxPlaylistFiles, TRUE, &Options);


	// Parse command line.
	BOOL bAddedFile = FALSE;
	char *psz = m_lpCmdLine, *pszbuff = new char[ strlen(psz)+1 ];
	while ( *psz ) {
		while ( *psz && *psz==' ') psz++;	//skip begining spaces

		
		if ( *psz=='"' ) {
			
			psz++; char *pszstart = psz;
			while ( *psz && *psz!='"' ) psz++;
			char c=*psz; *psz = 0; 
			strcpy( pszbuff, pszstart );
			*psz = c; if ( *pszstart ) psz++;

		} else if (*psz ) {

			char *pszstart = psz;
			while ( *psz && *psz!=' ' ) psz++;
			char c=*psz; *psz = 0; 
			strcpy( pszbuff, pszstart );
			*psz = c;

		} else *pszbuff = 0;

		if ( *pszbuff ) 
		{
			if ( *pszbuff=='/' )
			{
				
				if ( !_stricmp(pszbuff+1, "NoLoop") )
					Options.bLoop = FALSE;

			}
			else
			{
				pPlaylist->Add(pszbuff, !bAddedFile);
				bAddedFile = TRUE;
			}
		}

	}
	delete[] pszbuff;
	if ( bAddedFile ) 
	{
		if ( Options.modeIdStartWhenMovie )
			Options.modeId = Options.modeIdStartWhenMovie;
	}

	//CheckDateExpired;
{
	
	int day, month, year;
	
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
	GetVersionEx (&osvi);
	
	if ( osvi.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS )
	{
		int day, month, year;
	
		_outp(0x70,7); day = _inp(0x71);
		day = ((day&0x0F))+(((day&0xF0)>>4)*10);
	
		_outp(0x70,8); month = _inp(0x71);
		month = ((month&0x0F))+(((month&0xF0)>>4)*10);
	
		_outp(0x70,9); year = _inp(0x71);
		year = 2000+((year&0x0F))+(((year&0xF0)>>4)*10);
	
		_asm {
			push day
			push month
			push year
			push day
			push month
			push year
		}
	
		SYSTEMTIME st;
		GetSystemTime( &st );
	}
	else 
	{
	
		int day, month, year;
	
		SYSTEMTIME st;
		GetLocalTime( &st );
		day = st.wDay;
		month = st.wMonth;
		year = st.wYear;
	
		_asm {
			push day
			push month
			push year
			push day
			push month
			push year
		}
	
		GetSystemTime( &st );
	
	}
	
	_asm { 
		pop year
		pop month
		pop day
	}
	
	srand( GetTickCount() );
	int r1 = rand()%9,r2 = rand()%9,r3 = rand()%9,r4 = rand()%9,
		i1=0,i2=0,i3=0,i4=0;
	
	int days=(year<<4)-(year<<2)+(month<<5)-(month<<1)+day;
	
	int limit = LIMIT;
	int arrlimit[10], arrdays[10];
	
	int y,m,d;
	_asm { 
		pop y
		pop m
		pop d
	}
	if ( days>LIMIT ) ShowExpired
	
	if ( days>limit ) goto showExpired1;
	
	for (i1=0; i1<r1; i1++) {
	
		if ( rand()%2 ) arrdays[i1] = days+i1;
		else arrdays[i1] = (y<<4)-(y<<2)+(m<<5)-(m<<1)+d+i1;
	
	}
	
	if ( (y<<4)-(y<<2)+(m<<5)-(m<<1)+d+i1>limit ) ShowExpired
	
	while (i2<r2) {
	
		if ( i2%3 ) arrlimit[i2] = limit+i2;
		else arrlimit[i2] = LIMIT+i2;
	
		i2++;
	}
	
	do {
		int d=arrdays[rand()%(5)+1], l=arrlimit[rand()%(5)+1];
		if (rand()%2) d=(y<<4)-(y<<2)+(m<<5)-(m<<1)+d+i1;
		//if ( d>=l ) goto showExpired2;
	
		i3++;
	} while (i3<r3);
	
	goto endCheckDateExpired;
	
	int expired;
	showExpired1: expired = 1; ShowExpired
	//showExpired2: expired = 2; ShowExpired
	
	endCheckDateExpired: ;
}

	// Create two popup windows, the main widndow is wdnFs, because
	//  it didn't have defined parent window when create.
	
	// Create fullscreen window ( it is uses as main window ).
	CFullscreenWnd wndFs;
	wndFs.Create( Options.szAppName, NULL );


	// Create skin window ( it behaves as child window but is draw 
	//  everywhere, not only over parent window ).
	CMainWnd wnd;
	m_pMainWnd = &wnd;
	wnd.pPlaylist = pPlaylist;
	

	if ( !wnd.Create( Options.szAppName, &wndFs, &Options, NULL ) ) 
	{
		//Initialization error

		MessageBox(NULL, wnd.szError, "Application error", 
			MB_OK|MB_ICONERROR);

		return FALSE;	
	}


	pPlaylist->CreateWnd( &wnd );


	hwndCtrl = wnd.m_hWnd;
	hwndFs = wndFs.m_hWnd;
	hwndPlaylist = pPlaylist->m_hWnd;
	hwndSubtitles = wnd.pSubtitles->m_hWnd;
	hwndSubtitlesFs = wnd.pSubtitles->pSubtitlesFs->m_hWnd;

	SetForegroundWindow(hwndCtrl);


	PostMessage( wnd.m_hWnd, WM_COMMAND, IDM_CALLBACK_OPENFROMPLS, NULL );
	int ret = Run();


	// Delete and save playlist.
	delete pPlaylist;

	// Save config file.
	WriteConfig( szConfig, &Options );

	DestroyAcceleratorTable( hAccel );

	ReleaseMutex(hInstanceMutex);
	CloseHandle(hInstanceMutex);

	return ret;
}



BOOL CXPPlayerApp::PreTranslateMessage(MSG* pMsg) 
{
	if ( m_pMainWnd )
	{
		HWND hWndActive = GetActiveWindow();
		if ( hWndActive && hAccel )
		{

			if ( hWndActive==hwndCtrl || hWndActive==hwndFs ||
			   hWndActive==hwndSubtitles || hWndActive==hwndSubtitlesFs )	
			{
 				if ( TranslateAccelerator( hWndActive, hAccel, pMsg) )
					return TRUE;
			}

			else if ( hWndActive==hwndPlaylist )
			{
				if ( TranslateAccelerator( hWndActive, hAccel, pMsg) )
					return CWinApp::PreTranslateMessage(pMsg);
			}

		}

		switch ( pMsg->message )
		{
			case WM_SYSCOMMAND:

				switch ( pMsg->wParam )
				{

					case SC_MAXIMIZE:
						// Maximize window.
						((CMainWnd*)m_pMainWnd)->OnWindowMaximize();
						return TRUE;

					case SC_MINIMIZE:
						// Minimize window.
						((CMainWnd*)m_pMainWnd)->OnWindowMinimize();
						return TRUE;
				}


				break;
		}

	}
	
	return CWinApp::PreTranslateMessage(pMsg);
}
