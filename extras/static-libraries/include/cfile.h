#if !defined ( FILE_CFILE_H_INCLUDED )
#define FILE_CFILE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <windows.h>

class cFile
{
protected:
	HANDLE hFile;

public:
/*
	cFile();
	cFile(char *filename,char *mode);
	~cFile();

	int Open(char *filename,char *mode);
	void Close();

	int Read(void *buffer, int count);

	void *Read(DWORD *retFilesize);

	int Write(void *buffer, int count);
	int Seek(int offset, int origin);
	int CurrentSize();
*/


	DWORD dwSize;
	int bOpened;

	cFile::cFile()
	{
		hFile = NULL;
		bOpened = FALSE;
	}
	cFile::cFile(char *filename, char *mode)
	{
		hFile = NULL;
		bOpened = FALSE;
		Open(filename, mode);
	}

	//mode: "r", "w", "+", "r+", "w+", "rb+", "wb+"
	int cFile::Open(char *filename, char *mode)
	{
		DWORD dwAccess, dwCreation;

		if (!filename || !mode) return FALSE;

		if ( !strlen(mode) ) return FALSE;

		Close();

		switch(*mode)
		{
			case 'r': 
			case 'R':
				dwAccess = GENERIC_READ; dwCreation = OPEN_EXISTING; 
				break;

			case 'w': 
			case 'W':
				dwAccess = GENERIC_WRITE; dwCreation = CREATE_ALWAYS; 
				break;

			default: 
				return FALSE;
		}

		mode += strlen(mode)-1;
		if (*mode == '+')
			dwAccess = GENERIC_READ | GENERIC_WRITE;
		hFile = CreateFile( filename, dwAccess, 0, NULL, dwCreation, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		dwSize = GetFileSize(hFile, NULL);
		bOpened = TRUE;

		return TRUE;
	}

	void cFile::Close()
	{
		if (hFile) CloseHandle(hFile);
		hFile = NULL;
		bOpened = FALSE;
	}

	int cFile::Read(void *buffer, int count)
	{
		if (!hFile) return FALSE;

		DWORD dwBytesRead;	

		return ReadFile( hFile, buffer, count, &dwBytesRead, NULL);
	}

	//allocate memory, read whole file and return pointer to file_in_memory
	void *cFile::Read(DWORD *retFilesize)
	{
		if (!hFile) return NULL;

		DWORD dwBytesRead;

		BYTE *pFileBuff = new BYTE[ CurrentSize() ];

		if ( ReadFile( hFile, pFileBuff, CurrentSize(), &dwBytesRead, NULL) )
			return pFileBuff;

		delete[] pFileBuff;

		return NULL;
	}

	int cFile::Write(void *buffer, int count)
	{
		if(!hFile) return FALSE;

		DWORD dwBytesWritten;

		return WriteFile(hFile, buffer, count, &dwBytesWritten, NULL);
	}

	int cFile::Seek(int offset, int origin)
	{
		if(!hFile) return FALSE;

		if (origin == FILE_BEGIN || origin == FILE_CURRENT || origin == FILE_END)
			return SetFilePointer(hFile, offset, NULL, origin);
		else
			return FALSE;
	}

	int cFile::CurrentSize()
	{
		if(!hFile) return 0;

		dwSize = GetFileSize(hFile, NULL);
		return dwSize;
	}

	cFile::~cFile()
	{
		Close();
	}

};



#endif // !defined ( FILE_CFILE_H_INCLUDED )