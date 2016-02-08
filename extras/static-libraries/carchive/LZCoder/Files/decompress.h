class cLZFAT
{
private:

#pragma pack(push)
#pragma pack(1)
struct stFAT
{
	char cType;			//0-file, 1-directory
	char cCompression;
	unsigned long dwNext;
	union
	{
		unsigned long dwData;
		unsigned long dwSubdir;
	};
	unsigned long dwSize;
	unsigned long dwCompressedSize;
	char szName[256];
};
#pragma pack(pop)

enum
{
	CompNone = 0,
	CompXOR,
	CompLZ,
	CompLZW
};

	unsigned char *pFAT;
	unsigned long dwItemsCount;
	unsigned long dwFATSize;

	unsigned long dwBitSize;
	unsigned long dwBuffSize;
	unsigned char *pBuff;
	HANDLE hFile;

public:
	cLZFAT(char *szFile);
	~cLZFAT();
	void *Read(char *szFile);
	unsigned int Size(char *szFile);
	unsigned int CompressedSize(char *szFile);

	void Decompress(char *szFolder);
	void Test(char *szFolder);

	int bOpened;
};

cLZFAT::cLZFAT(char *szFile)
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

	LZWDecompress(pFAT, &dwFATSize, pfat, dwCompressedFATSize);

	
	delete pfat;

	dwBitSize = 256*1024;
	dwBuffSize = (unsigned int)( (float)dwBitSize*1.01f+256.0f );
	pBuff = new unsigned char [dwBuffSize];

}
cLZFAT::~cLZFAT()
{
	if (!bOpened)
		return;
	delete pBuff;
	CloseHandle(hFile);
	delete pFAT;
}
unsigned int cLZFAT::CompressedSize(char *szFile)
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
unsigned int cLZFAT::Size(char *szFile)
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

void *cLZFAT::Read(char *szFile)
{
	unsigned long dwBytesRead;
	char szName[256];
	stFAT *pfat;
	char *sz;
	
	if (!bOpened)
		return NULL;

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
				return NULL;
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
			unsigned char *pBuffout = new unsigned char[dwBuffout] ,
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
						LZDecompress(pBuff, dwCompBitSize, pBuffout, &dwTempBitSize);
						pBuffout += dwBitSize;
						break;
					case CompLZW:
						ReadFile( hFile, &dwCompBitSize, 4, &dwBytesRead, NULL);
						ReadFile( hFile, pBuff, dwCompBitSize, &dwBytesRead, NULL);
						dwTempBitSize = dwBitSize;
						LZWDecompress(pBuffout, &dwTempBitSize, pBuff, dwCompBitSize);
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
						LZDecompress(pBuff, dwCompBitSize, pBuffout, &dwTempBitSize);
						pBuffout += dwBitSize;
						break;
					case CompLZW:
						ReadFile( hFile, &dwCompBitSize, 4, &dwBytesRead, NULL);
						ReadFile( hFile, pBuff, dwCompBitSize, &dwBytesRead, NULL);
						dwTempBitSize = dwBitSize;
						LZWDecompress(pBuffout, &dwTempBitSize, pBuff, dwCompBitSize);
						pBuffout += dwBitSize;
						break;
				}
			}
			return pBuffoutset;
		}
			
	}
	return NULL;
}

void cLZFAT::Decompress(char *szFolder)
{
	char szName[256];
	stFAT *pfat;
	char *sz;

	DWORD dwSubs[16];
	DWORD dwSub = 0;

	if (!bOpened)
		return;

	sprintf(szName,"%s\\",szFolder);
	CreateDirectory(szName,NULL);

	pfat = (stFAT*)pFAT;
	while ( !(pfat->cType == -1 && dwSub == 0))
	{
		if (pfat->cType == 1)
		{
			//directory
			sz = szName+strlen(szName);
			sprintf(sz,"%s\\",pfat->szName);
			CreateDirectory(szName,NULL);

			dwSubs[dwSub] = pfat->dwNext;
			pfat = (stFAT*)(pFAT + pfat->dwSubdir);			
			dwSub++;
		}
		else if(pfat->cType == 0)
		{
			//file to decompres

			sz = szName+strlen(szName);
			strcpy(sz,pfat->szName);

			for (char *szr = szName; *szr != '\\'; szr++); szr++;
			char *buffer = (char*)Read(szr);
			DWORD dwSize = Size(szr);

			FILE *hFile = fopen(szName,"wb");
			fwrite(buffer, 1, dwSize, hFile);
			fclose(hFile);

			delete buffer;

			
			printf("\n Unpacking - %s",szName);
			*sz = 0;
			pfat = (stFAT*)(pFAT + pfat->dwNext);

		}
		if (pfat->cType == -1 && dwSub)
		{
			sz = szName+strlen(szName)-2;
			for (; *sz != '\\'; sz--); sz++; *sz = 0;
			dwSub--;
			pfat = (stFAT*)(pFAT + dwSubs[dwSub]);
		}

	}
	printf("\n");
}
void cLZFAT::Test(char *szFolder)
{
	char szName[256];
	stFAT *pfat;
	char *sz;

	DWORD dwSubs[16];
	DWORD dwSub = 0;

	if (!bOpened)
		return;

	sprintf(szName,"%s\\",szFolder);

	pfat = (stFAT*)pFAT;
	while ( !(pfat->cType == -1 && dwSub == 0))
	{
		if (pfat->cType == 1)
		{
			//directory
			sz = szName+strlen(szName);
			sprintf(sz,"%s\\",pfat->szName);

			dwSubs[dwSub] = pfat->dwNext;
			pfat = (stFAT*)(pFAT + pfat->dwSubdir);			
			dwSub++;
		}
		else if(pfat->cType == 0)
		{
			//file to decompres

			sz = szName+strlen(szName);
			strcpy(sz,pfat->szName);

			for (char *szr = szName; *szr != '\\'; szr++); szr++;
			char *buffer = (char*)Read(szr);
			DWORD dwSize = Size(szr);

			delete buffer;

			
			printf("\n Testing - %s",szName);
			*sz = 0;
			pfat = (stFAT*)(pFAT + pfat->dwNext);

		}
		if (pfat->cType == -1 && dwSub)
		{
			sz = szName+strlen(szName)-2;
			for (; *sz != '\\'; sz--); sz++; *sz = 0;
			dwSub--;
			pfat = (stFAT*)(pFAT + dwSubs[dwSub]);
		}

	}
	printf("\n");
}
