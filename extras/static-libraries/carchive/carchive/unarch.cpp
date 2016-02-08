/* unarch.cpp -- IO on .arch files using zlib and lz

   arch - archive file, format created by 
			Adam Joniec, single file in archive can by compresed,
			stored(no compression) or 
			stored with xor (simple cipher)
   
*/

#include "unarch.h"

CArch::CArch(char *szFile)
{
	unsigned long dwBytesRead;
	unsigned char *pfat;
	DWORD dwCompressedFATSize;

	bOpened = false;
	hFile = CreateFile( szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	bOpened = TRUE;

	DWORD dwFileSize = SetFilePointer(hFile, 0, NULL, FILE_END);
	
	SetFilePointer(hFile, dwFileSize-8, NULL, FILE_BEGIN);
	ReadFile( hFile, &dwFATSize, 4, &dwBytesRead, NULL);
	ReadFile( hFile, &dwCompressedFATSize, 4, &dwBytesRead, NULL);
	pFAT = new unsigned char [dwFATSize+10];
	pfat = new unsigned char [dwCompressedFATSize];
	
	SetFilePointer(hFile, dwFileSize-8-dwCompressedFATSize, NULL, FILE_BEGIN);
	ReadFile( hFile, pfat, dwCompressedFATSize, &dwBytesRead, NULL);

	uncompress(pFAT, &dwFATSize, pfat, dwCompressedFATSize);

	
	delete pfat;

	dwBitSize = 256*1024;
	dwBuffSize = (unsigned int)( (float)dwBitSize*1.01f+256.0f );
	pBuff = new unsigned char [dwBuffSize];

}
CArch::~CArch()
{
	if (!bOpened)
		return;
	delete pBuff;
	CloseHandle(hFile);
	delete pFAT;
}
unsigned long CArch::CompressedSize(char *szFile)
{
	char szName[256];
	stFAT *pfat;
	char *sz;
	
	if (!bOpened)
		return 0;

	pfat = (stFAT*)pFAT;
	sz = szName;
	while ( *szFile != '\\' && *szFile != '/' && *szFile != 0)
		*sz++ = *szFile++;
	*sz++ = 0;
	while (pfat->cType != -1)
	{
		if ( _stricmp(szName,pfat->szName) )
			pfat = (stFAT*)(pFAT + pfat->dwNext);
		else if(*szFile)
		{
			if (!pfat->cType)
				return 0;
			szFile++;
			sz = szName;
			while ( *szFile != '\\' && *szFile != '/' && *szFile != 0)
				*sz++ = *szFile++;
			*sz++ = 0;
			pfat = (stFAT*)(pFAT + pfat->dwSubdir);
		}
		else if(!pfat->cType)
			return pfat->dwCompressedSize;
	}
	return 0;
}
unsigned long CArch::Size(char *szFile)
{
	
	char szName[256];
	stFAT *pfat;
	char *sz;
	
	if (!bOpened)
		return 0;

	pfat = (stFAT*)pFAT;
	sz = szName;
	while ( *szFile != '\\' && *szFile != '/' && *szFile != 0)
		*sz++ = *szFile++;
	*sz++ = 0;
	while (pfat->cType != -1)
	{
		if ( _stricmp(szName,pfat->szName) )
			pfat = (stFAT*)(pFAT + pfat->dwNext);
		else if(*szFile)
		{
			if (!pfat->cType)
				return 0;
			szFile++;
			sz = szName;
			while ( *szFile != '\\' && *szFile != '/' && *szFile != 0)
				*sz++ = *szFile++;
			*sz++ = 0;
			pfat = (stFAT*)(pFAT + pfat->dwSubdir);
		}
		else if(!pfat->cType)
			return pfat->dwSize;
	}
	return 0;

}

void *CArch::Read(char *szFile)
{
	BYTE *pFilebuff = new BYTE [ Size(szFile) ];

	if ( Read(szFile, pFilebuff) )
		return pFilebuff;

	delete pFilebuff;

	return NULL;
}

int CArch::Read(char *szFile, void *pFilebuff)
{
	unsigned long dwBytesRead;
	char szName[256];
	stFAT *pfat;
	char *sz;
	
	if (!bOpened)
		return FALSE;

	pfat = (stFAT*)pFAT;
	sz = szName;
	while ( *szFile != '\\' && *szFile != '/' && *szFile != 0)
		*sz++ = *szFile++;
	*sz++ = 0;
	while (pfat->cType != -1)
	{
		if ( _stricmp(szName,pfat->szName) )
			pfat = (stFAT*)(pFAT + pfat->dwNext);
		else if(*szFile)
		{
			if (!pfat->cType)
				return FALSE;
			szFile++;
			sz = szName;
			while ( *szFile != '\\' && *szFile != '/' && *szFile != 0)
				*sz++ = *szFile++;
			*sz++ = 0;
			pfat = (stFAT*)(pFAT + pfat->dwSubdir);
		}
		else if(!pfat->cType)
		{
			unsigned long dwBuffout = pfat->dwSize;
			unsigned char *pBuffout = (BYTE*)pFilebuff,
							*pBuffoutset = pBuffout;
			unsigned long dwCompBitSize, dwTempBitSize;
			

			unsigned long dw, dwBits = dwBuffout / dwBitSize;
			SetFilePointer(hFile, pfat->dwData, NULL, FILE_BEGIN);
			dw=0; while (dw<dwBits)
			{
				switch(pfat->cCompression)
				{
					case CompNone:
						ReadFile( hFile, pBuffout, dwBitSize, &dwBytesRead, NULL);
						pBuffout += dwBitSize;
						break;
					case CompXOR:
						ReadFile( hFile, pBuffout, dwBitSize, &dwBytesRead, NULL);
						dwCompBitSize = dwBitSize;
						while (dwCompBitSize--) *pBuffout++ ^= dwCompBitSize;
						break;
					case CompLZ:
						ReadFile( hFile, &dwCompBitSize, 4, &dwBytesRead, NULL);
						ReadFile( hFile, pBuff, dwCompBitSize, &dwBytesRead, NULL);
						dwTempBitSize = dwBitSize;
						lzuncompress(pBuff, dwCompBitSize, pBuffout, &dwTempBitSize);
						pBuffout += dwBitSize;
						break;
					case CompZLIB:
						ReadFile( hFile, &dwCompBitSize, 4, &dwBytesRead, NULL);
						ReadFile( hFile, pBuff, dwCompBitSize, &dwBytesRead, NULL);
						dwTempBitSize = dwBitSize;
						uncompress(pBuffout, &dwTempBitSize, pBuff, dwCompBitSize);
						pBuffout += dwBitSize;
						break;
				}
				dw++;
			}
			if (dwBuffout % dwBitSize)
			{
				dwTempBitSize = dwBuffout % dwBitSize;
				switch(pfat->cCompression)
				{
					case CompNone:
						ReadFile( hFile, pBuffout, dwTempBitSize, &dwBytesRead, NULL);
						pBuffout += dwTempBitSize;
						break;
					case CompXOR:
						ReadFile( hFile, pBuffout, dwTempBitSize, &dwBytesRead, NULL);
						dwCompBitSize = dwTempBitSize;
						while (dwCompBitSize--) *pBuffout++ ^= dwCompBitSize;
						break;
					case CompLZ:
						ReadFile( hFile, &dwCompBitSize, 4, &dwBytesRead, NULL);
						ReadFile( hFile, pBuff, dwCompBitSize, &dwBytesRead, NULL);
						dwTempBitSize = dwBitSize;
						lzuncompress(pBuff, dwCompBitSize, pBuffout, &dwTempBitSize);
						pBuffout += dwBitSize;
						break;
					case CompZLIB:
						ReadFile( hFile, &dwCompBitSize, 4, &dwBytesRead, NULL);
						ReadFile( hFile, pBuff, dwCompBitSize, &dwBytesRead, NULL);
						dwTempBitSize = dwBitSize;
						uncompress(pBuffout, &dwTempBitSize, pBuff, dwCompBitSize);
						pBuffout += dwBitSize;
						break;
				}
			}
			return TRUE;
		}
			
	}
	return FALSE;
}