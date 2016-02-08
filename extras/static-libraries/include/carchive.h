#if !defined ( FILE_CARCHIVE_H_INCLUDED )
#define FILE_CARCHIVE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include <windows.h>

class cArchive
{

	int archiveType;
	void* parchiveFile;

public:

	enum ARCHIVE_TYPE {
		ARCHIVE_NONE = 0,
		ARCHIVE_ZIP,		//zip file
		ARCHIVE_ARCH		//my filetype
	};

	cArchive();
	~cArchive();

	//opening archive ZIP or ARCH
	//return 0 when error and no archive opened
	//		 1 when ZIP opened
	//		 2 when ARCH opened	int OpenedArchive();
	int OpenArchive(char *szPath);

	//return values same as OpenArchive
	int IsOpenedArchive();

	//closing archive
	BOOL CloseArchive();

	int OpenZIP(char *szPath);
	int OpenARCH(char *szPath);


	//allocate memory, read whole file and
	//return pointer to allocated memory
	//return NULL when no file found
	//retFilesize - here return size of file
	void *FileRead(char *szFilename, unsigned long *retFilesize);

	//return filesize
	unsigned long FileSize(char *szFilename);

	//return compressed filesize in archive file
	unsigned long FileCompressedSize(char *szFilename);	

};

#endif // !defined ( FILE_CARCHIVE_H_INCLUDED )