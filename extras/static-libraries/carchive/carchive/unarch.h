#if !defined ( FILE_UNARCH_H )
#define FILE_UNARCH_H

#include <windows.h>
#include "lz.h"
#include "../zlib/zlib.h"

class CArch
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
	CompZLIB
};

	unsigned char *pFAT;
	unsigned long dwItemsCount;
	unsigned long dwFATSize;

	unsigned long dwBitSize;
	unsigned long dwBuffSize;
	unsigned char *pBuff;
	HANDLE hFile;

public:
	CArch(char *szFile);
	~CArch();

	//allocates buffer, read whole file in it, and return pointer to it
	void *Read(char *szFile);
	int Read(char *szFile, void *pFilebuff);
	unsigned long Size(char *szFile);
	unsigned long CompressedSize(char *szFile);

	int bOpened;
};

#endif