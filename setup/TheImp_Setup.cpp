// TheImp_Setup.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	HRSRC hRsrc = FindResource( hInstance, MAKEINTRESOURCE(IDR_SETUP), "RAWDATA" );
	HGLOBAL hResData = LoadResource( hInstance, hRsrc );
	DWORD size = SizeofResource( hInstance, hRsrc );
	char *zip = (char*)LockResource( hResData );

	char szTempPath[_MAX_PATH], szTempFileZip[_MAX_PATH];
	GetTempPath (  _MAX_PATH, szTempPath ); 
	GetTempFileName( szTempPath, "imp", 0, szTempFileZip );

	FILE *f = fopen( szTempFileZip, "wb" );
	fwrite( zip, size, 1, f ); fclose(f);

	char szTempFile[_MAX_PATH];
	GetTempFileName( szTempPath, "imp", 0, szTempFile );
	DeleteFile( szTempFile );


	if ( !CreateDirectory( szTempFile, NULL ) )
		return 0;

	cArchive *pArchive = new cArchive();
	pArchive->OpenArchive( szTempFileZip );

	if ( !pArchive->IsOpenedArchive() )
		return 0;

	char *szFiles[] = {
		"layout.bin", "setup.bmp", "_sys1.cab",
		"_user1.cab", "data1.cab", "lang.dat",
		"os.dat", "_setup.dll", "pmbit.dll",
		"_inst32i.ex_", "_isdel.exe", "Setup.exe",
		"Setup.ini", "setup.ins", "setup.lid", "Data.tag"
	};
	int files = sizeof(szFiles)/sizeof(char*);

	for ( int i=0; i<files; i++ )
	{

		DWORD filesize;
		BYTE *buff = (BYTE*)pArchive->ReadFile( szFiles[i], &filesize );
		if ( buff )
		{
			char szTemp[_MAX_PATH];
			_makepath( szTemp, "", szTempFile, szFiles[i], NULL );

			FILE *f = fopen( szTemp, "wb" );
			fwrite( buff, 1, filesize, f ); fclose(f);

			delete[] buff;
			
		}
	}

	delete pArchive;
	DeleteFile( szTempFileZip );

	char szTemp[_MAX_PATH+2];
	_makepath( szTemp, "", szTempFile, "Setup.exe", NULL );

	SetCurrentDirectory( szTempFile );
	WinExec( szTemp, SW_SHOW );

/*	STARTUPINFO startupinfo; GetStartupInfo( &startupinfo );
	PROCESS_INFORMATION processinf;
	BOOL ret = CreateProcess( "C:\\WINDOWS\\TEMP\\impF016.TMP\\Setup.exe", "", NULL, NULL, FALSE, 
		CREATE_DEFAULT_ERROR_MODE, NULL, szTempFile, 
		&startupinfo, &processinf );

	//if ( ret ) WaitForInputIdle( processinf.hProcess, INFINITE );
*/

/*	Sleep( 1000 );

	_makepath( szTemp, "", szTempFile, "_isdel.exe", NULL );
	while ( !DeleteFile(szTemp) ) Sleep( 1000 );


	for ( i=0; i<files; i++ )
	{
		char szTemp[_MAX_PATH];
		_makepath( szTemp, "", szTempFile, szFiles[i], NULL );
		DeleteFile( szTemp );
	}
	RemoveDirectory( szTempFile );
*/	

	return 0;
}