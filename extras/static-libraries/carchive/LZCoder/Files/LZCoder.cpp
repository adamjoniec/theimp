#include <stdio.h>
#include <windows.h>
#include <cfile.h>
#include <lzcd.h.>
#include <lzwcd.h>

#pragma hdrstop

enum
{
	CompNone = 0,
	CompXOR,
	CompLZ,
	CompLZW
};

#pragma pack(push)

#pragma pack(1)
struct stFAT
{
	char cType;
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
#define ItemSize sizeof(stFAT)	// 32 bytes

#pragma pack(pop)

stFAT *pFAT;
unsigned int dwItemsCount;


/*	ItemsCount()
		szPath - path when the directories and files will be search
		return int - number founded directories*2 +  number of founded files
*/
int ItemsCount(char *szPath)
{
	HANDLE hs;
	WIN32_FIND_DATA sts;
	int i;

	hs = FindFirstFile( szPath, &sts );
	FindNextFile( hs, &sts );
	for (i=0; FindNextFile(hs, &sts); i++)
	{
		if ( (sts.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			  ==FILE_ATTRIBUTE_DIRECTORY )
			i++;
	}
	return i;
}

void PrintInfo()
{
	printf("\nLZCoder 0.19        Copyright (C) Adam Joniec                     [-SAuron-]");
	printf("\n\nUses:   LZCoder <command> -<option 1> -<option N> <archive> <directory>");
	printf("\n\n <Commands>");
	printf("\n   c        create archive");
	printf("\n   x        extract archive");
	printf("\n   t        test archive");
	printf("\n <Options>");
	printf("\n   none    select no compression");
	printf("\n   xor     select no compression, xor code");
	printf("\n   lz      select Lempel-Zip compression");
	printf("\n   lzw     select Lempel-Zip-Welch compression");
	printf("\n   dnone<directory>     select no compression for directory");
	printf("\n   dxor<directory>      select no compression for directory, xor code");
	printf("\n   dlz<directory>       select Lempel-Zip compression for directory");
	printf("\n   dlzw<directory>      select Lempel-Zip-Welch compression for directory");
	printf("\n\n <archive>    name of output archive");
	printf("\n <directory>  directory to compress [it will be ");
	printf("\n               treaten as root directory]");
	printf("\n\nExample:  LZCoder c -lz -dnoneC:\\Graphic\\Jpeg graphic.lz C:\\Graphic");

	printf("\n");
}

#include "decompress.h"


void main(int argc, char **argv)
{
	HANDLE hSearch[64];
	WIN32_FIND_DATA stSearchInfo[16];
	unsigned int dwSubdir[16];
	BOOL bDirFounded[64];
	char szPath[64][256];

	char szPathName[256];
	char szOutputName[256];
	char szSpecialCompressionPath[64][256];
	char cSpecialCompressionType[64];
	int iSpecialCompressionCount;
	unsigned int dwDir, dwItem;
	unsigned int dwDataOffset;
	BOOL bFound;
	cFile *File;
	char cDefaultCompression;

	int iOption = 0;

	DWORD dwBitSize = 256 * 1024;


	cDefaultCompression = CompLZW;

	if (argc<3){
		PrintInfo();
		return;
	}

	argv++;  iOption++;
	if (_stricmp(*argv,"t")==0 && argc>2)
	{

		argv++;
		cLZFAT *File = new cLZFAT(*argv);
		if (!File->bOpened)
		{
			printf("\nFile %s doesn't exist.",*argv);
			return;
		}
		printf("\n Testing archive: %s\n",*argv);
		File->Test(*argv);
		return;
	}

	if (argc < 4)
	{
		PrintInfo();
		return;
	}

	if (_stricmp(*argv,"x")==0)
	{

		argv++;
		cLZFAT *File = new cLZFAT(*argv);
		if (!File->bOpened)
		{
			printf("\nFile %s doesn't exist.",*argv);
			return;
		}
		printf("\n Unpacking archive: %s\n",*argv);
		argv++; File->Decompress(*argv);
		return;
	}



	if (_stricmp(*argv,"c")!=0)
	{
		PrintInfo();
		return;;
	}

	argv++;  iOption++;
	iSpecialCompressionCount = 0;
	while(*argv[0] == '-' && argc>iOption-2)
	{
		char *argvpath;

		if (_stricmp(*argv,"-none")==0)
			cDefaultCompression = CompNone;
		if (_stricmp(*argv,"-xor")==0)
			cDefaultCompression = CompXOR;
		if (_stricmp(*argv,"-lz")==0)
			cDefaultCompression = CompLZ;
		if (_stricmp(*argv,"-lzw")==0)
			cDefaultCompression = CompLZW;

		if (_strnicmp(*argv,"-dnone",6)==0)
		{
			argvpath = *argv+6;
			strcpy(szSpecialCompressionPath[iSpecialCompressionCount], argvpath);
			cSpecialCompressionType[iSpecialCompressionCount] = CompNone;
			iSpecialCompressionCount++;
			//printf("\nNone - %s",szSpecialCompressionPath[iSpecialCompressionCount-1]);
		}
		if (_strnicmp(*argv,"-dxor",5)==0)
		{
			argvpath = *argv+5;
			strcpy(szSpecialCompressionPath[iSpecialCompressionCount], argvpath);
			cSpecialCompressionType[iSpecialCompressionCount] = CompXOR;
			iSpecialCompressionCount++;
			//printf("\nXOR  - %s",szSpecialCompressionPath[iSpecialCompressionCount-1]);
		}
		if (_strnicmp(*argv,"-dlz",4)==0)
		{
			argvpath = *argv+4;
			strcpy(szSpecialCompressionPath[iSpecialCompressionCount], argvpath);
			cSpecialCompressionType[iSpecialCompressionCount] = CompLZ;
			iSpecialCompressionCount++;
			//printf("\nLZ   - %s",szSpecialCompressionPath[iSpecialCompressionCount-1]);
		}
		if (_strnicmp(*argv,"-dlzw",5)==0)
		{
			argvpath = *argv+5;
			strcpy(szSpecialCompressionPath[iSpecialCompressionCount], argvpath);
			cSpecialCompressionType[iSpecialCompressionCount] = CompLZW;
			iSpecialCompressionCount++;
			//printf("\nLZW  - %s",szSpecialCompressionPath[iSpecialCompressionCount-1]);
		}
		argv++;  iOption++;
	}

	strcpy(szOutputName,*argv);
	argv++;

	dwDir = 0;  dwItem = 0;  dwDataOffset = 0;

	sprintf(szPath[dwDir],"%s\\*",*argv);

	printf("\n Creating archive: %s\n",szOutputName);

	hSearch[dwDir] = FindFirstFile( szPath[dwDir], &stSearchInfo[dwDir] );
	FindNextFile( hSearch[dwDir], &stSearchInfo[dwDir] );

	dwItemsCount = ItemsCount(szPath[dwDir]);
	dwItemsCount++;
	while ( 1 )
	{
		bFound = FindNextFile( hSearch[dwDir], &stSearchInfo[dwDir]);
		if (dwDir == 0  &&  bFound == 0)
			break;

		if (bFound)
		{
			if (stSearchInfo[dwDir].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				dwDir++;
				sprintf(szPath[dwDir],"%s",szPath[dwDir-1]);
				szPath[dwDir][strlen(szPath[dwDir])-1] = 0;
				strcat(szPath[dwDir],stSearchInfo[dwDir-1].cFileName);
				strcat(szPath[dwDir], "\\*");
				hSearch[dwDir] = FindFirstFile( szPath[dwDir], &stSearchInfo[dwDir] );
				FindNextFile( hSearch[dwDir], &stSearchInfo[dwDir] );
				dwItemsCount += ItemsCount( szPath[dwDir] );
			}
			else
			{
			}
		}
		else
		{
			FindClose( hSearch[dwDir] );
			dwDir--;
		}
	}
	FindClose( hSearch[dwDir] );

	pFAT = new stFAT[dwItemsCount];
	memset(pFAT,0,dwItemsCount*ItemSize);
	memset(bDirFounded,0,sizeof(BOOL));

	File = new cFile(szOutputName,"w");

	hSearch[dwDir] = FindFirstFile( szPath[dwDir], &stSearchInfo[dwDir] );
	FindNextFile( hSearch[dwDir], &stSearchInfo[dwDir] );

	
	while ( 1 )
	{
		bFound = FindNextFile( hSearch[dwDir], &stSearchInfo[dwDir]);
		if (dwDir == 0  &&  bFound == 0 && bDirFounded[0] ==1)
		{
			pFAT[dwItem].cType = -1;
			break;
		}

		if (bFound)
		{
			if ((stSearchInfo[dwDir].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				if (bDirFounded[dwDir]==0)
				{
					pFAT[dwItem].cType = 1;
					strcpy( pFAT[dwItem].szName, stSearchInfo[dwDir].cFileName);
					pFAT[dwItem].dwSubdir = dwItem+1;
					dwItem++;
					dwSubdir[dwDir] = dwItem - 1;
					dwDir++;
					
					sprintf(szPath[dwDir],"%s",szPath[dwDir-1]);
					szPath[dwDir][strlen(szPath[dwDir])-1] = 0;
					strcat(szPath[dwDir],stSearchInfo[dwDir-1].cFileName);
					strcat(szPath[dwDir], "\\*");
					hSearch[dwDir] = FindFirstFile( szPath[dwDir], &stSearchInfo[dwDir] );
					FindNextFile( hSearch[dwDir], &stSearchInfo[dwDir] );
					
				}
			}
			else
			{
				if (bDirFounded[dwDir]==1)
				{
					cFile *FIn;
					unsigned char *buff1,*buff2;
					unsigned long buff1size, buff2size, buffsize;
					char cThisFileCompression;
					int i;
					DWORD dw, dwi;


					cThisFileCompression = cDefaultCompression;
					strcpy(szPathName,szPath[dwDir]);
					szPathName[strlen(szPathName)-2] = 0;
					for (i=0; i<iSpecialCompressionCount; i++)
						if (_strnicmp(szSpecialCompressionPath[i],szPathName,
							strlen(szSpecialCompressionPath[i])) == 0)
							cThisFileCompression = cSpecialCompressionType[i];

					strcpy(szPathName,szPath[dwDir]);
					szPathName[strlen(szPathName)-1] = 0;
					strcat(szPathName, stSearchInfo[dwDir].cFileName );

					char *szCompTypes[] = {"None","XOR ","LZ  ","LZW "};
					printf("\n Adding [%s]  -  %-54s",szCompTypes[cThisFileCompression],szPathName);
					FIn = new cFile(szPathName,"r");
					if (!FIn->bOpened)
					{
						printf(	"\n ERROR: file %s used by other application\n  exiting ...\n",
								szPathName);
						return;
					}
					buff1size = FIn->dwSize;
					buff1 = new unsigned char[dwBitSize];
					unsigned char *buff1set = buff1;

					pFAT[dwItem].cType = 0;
					pFAT[dwItem].cCompression = cThisFileCompression;
					strcpy( pFAT[dwItem].szName, stSearchInfo[dwDir].cFileName);
					pFAT[dwItem].dwNext = dwItem+1;
					pFAT[dwItem].dwData = dwDataOffset;
					switch (cThisFileCompression)
					{
						case CompNone:
							buff2size = 0;
							dw=0; while (dw<(buff1size / dwBitSize))
							{
								FIn->Read(buff1, dwBitSize);
								File->Write(buff1, dwBitSize);
								buff2size += dwBitSize;
								dw++;
							}
							if (buff1size % dwBitSize)
							{
								buffsize = buff1size % dwBitSize;
								FIn->Read(buff1, buffsize);
								File->Write(buff1, buffsize);
								buff2size += buffsize;
							}
							pFAT[dwItem].dwSize = buff1size;
							pFAT[dwItem].dwCompressedSize = buff2size;	//size file on disk
							dwDataOffset += buff2size;
							break;

						case CompXOR:
							buff2size = 0;
							dw=0; while (dw<(buff1size / dwBitSize))
							{
								FIn->Read(buff1, dwBitSize);

								buff2 = buff1;  dwi = dwBitSize;								
								while(dwi--) *buff2++ ^= dwi;

								File->Write(buff1, dwBitSize);
								buff2size += dwBitSize;
								dw++;
							}
							if (buff1size % dwBitSize)
							{
								buffsize = buff1size % dwBitSize;
								FIn->Read(buff1, buffsize);
								
								buff2 = buff1;  dwi = buffsize;								
								while(dwi--) *buff2++ ^= dwi;
								
								File->Write(buff1, buffsize);
								buff2size += buffsize;
							}
							pFAT[dwItem].dwSize = buff1size;
							pFAT[dwItem].dwCompressedSize = buff2size;	//size file on disk
							dwDataOffset += buff2size;
							break;

						case CompLZ:
							buff2size = 0;
							buffsize = dwBitSize + 256;
							buff2 = new unsigned char[buffsize];
							dw=0; while (dw<(buff1size / dwBitSize))
							{
								FIn->Read(buff1, dwBitSize);
								buffsize = dwBitSize + 256;
								LZCompress(buff1,dwBitSize, buff2, &buffsize);
								
								File->Write(&buffsize, 4);
								File->Write(buff2, buffsize);
								buff2size += buffsize+4;
								dw++;
							}
							if (buff1size % dwBitSize)
							{								
								FIn->Read(buff1, buff1size % dwBitSize);
								buffsize = dwBitSize + 256;
								LZCompress(buff1,buff1size % dwBitSize, buff2, &buffsize);

								File->Write(&buffsize,4);
								File->Write(buff2, buffsize);
								buff2size += buffsize+4;
							}
							pFAT[dwItem].dwSize = buff1size;
							pFAT[dwItem].dwCompressedSize = buff2size;	//size file on disk
							dwDataOffset += buff2size;

							delete buff2;
							break;

						case CompLZW:
							buff2size = 0;
							buffsize = (unsigned long) ( (float)dwBitSize * 1.1 );
							buff2 = new unsigned char[buffsize];
							dw=0; while (dw<(buff1size / dwBitSize))
							{
								FIn->Read(buff1, dwBitSize);
								buffsize = (unsigned long) ( (float)dwBitSize * 1.1 );
								LZWCompress(buff2, &buffsize, buff1,dwBitSize,9);
								
								File->Write(&buffsize, 4);
								File->Write(buff2, buffsize);
								buff2size += buffsize+4;
								dw++;
							}
							if (buff1size % dwBitSize)
							{								
								FIn->Read(buff1, buff1size % dwBitSize);
								buffsize = (unsigned long) ( (float)dwBitSize * 1.1 );
								LZWCompress(buff2, &buffsize, buff1,buff1size % dwBitSize, 9);

								File->Write(&buffsize,4);
								File->Write(buff2, buffsize);
								buff2size += buffsize+4;
							}
							pFAT[dwItem].dwSize = buff1size;
							pFAT[dwItem].dwCompressedSize = buff2size;	//size file on disk
							dwDataOffset += buff2size;

							delete buff2;
							break;

						default:
							break;
					}

					delete buff1set;
					delete FIn;
					printf("    OK");
					dwItem++;
				}
			}
		}
		else
		{
			if (bDirFounded[dwDir] == 0)
			{
				bDirFounded[dwDir] = 1;
				FindClose(hSearch[dwDir]);
				hSearch[dwDir] = FindFirstFile( szPath[dwDir], &stSearchInfo[dwDir] );
				FindNextFile( hSearch[dwDir], &stSearchInfo[dwDir] );				
			}
			else
			{
				bDirFounded[dwDir] = 0;

				pFAT[dwItem].cType = -1;
				dwItem++;

				FindClose( hSearch[dwDir] );

				dwDir--;
				pFAT[ dwSubdir[dwDir] ].dwNext = dwItem;
			}
		}
	}
	FindClose( hSearch[dwDir] );


	unsigned int i,j;
	unsigned long *pFATItemTrueSize;
	pFATItemTrueSize = new unsigned long[dwItemsCount+1];
	pFATItemTrueSize[0] = 0;
	j = 0;
	for (i=0; i<dwItemsCount; i++)
	{
		if (pFAT[i].cType == -1)
			j +=  1;
		else
			j += 19+strlen(pFAT[i].szName);
		pFATItemTrueSize[i+1] = j;
	}
	unsigned char *pFATTrue,*p;
	pFATTrue = new unsigned char[pFATItemTrueSize[dwItemsCount]];
	p = pFATTrue;
	for (i=0; i<dwItemsCount; i++)
	{
		pFAT[i].dwNext = pFATItemTrueSize[pFAT[i].dwNext];
		if (pFAT[i].cType == 1)
			pFAT[i].dwSubdir = pFATItemTrueSize[pFAT[i].dwSubdir];

		memcpy(p,&pFAT[i],pFATItemTrueSize[i+1]-pFATItemTrueSize[i]);
		p = pFATTrue + pFATItemTrueSize[i+1];
	}
	unsigned char *pCompFATTrue;
	unsigned long dwCompFATSize;

	dwCompFATSize = (int)((float)pFATItemTrueSize[dwItemsCount]*1.01)+512;
	pCompFATTrue = new unsigned char [dwCompFATSize];

	LZWCompress(pCompFATTrue,&dwCompFATSize,pFATTrue,pFATItemTrueSize[dwItemsCount],9);
	File->Write(pCompFATTrue,dwCompFATSize);
	File->Write(&pFATItemTrueSize[dwItemsCount],4);
	File->Write(&dwCompFATSize,4);

	printf("\n");
	delete pCompFATTrue;
	delete pFATTrue;
	delete pFATItemTrueSize;
	delete File;

	delete pFAT;
}