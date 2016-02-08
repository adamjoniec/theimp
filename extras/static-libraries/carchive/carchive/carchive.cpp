#include "carchive.h"
#include "unarch.h"
#include "unzip.h"


cArchive::cArchive()
{
	archiveType = ARCHIVE_NONE;
	parchiveFile = NULL;
}

cArchive::~cArchive()
{
	CloseArchive();
}

int cArchive::OpenZIP(char *szPath)
{
	CloseArchive();

	parchiveFile = (unzFile)unzOpen(szPath);

	if ( parchiveFile==NULL)
		return archiveType;

	archiveType = ARCHIVE_ZIP;


	return archiveType;
}

int cArchive::OpenARCH(char *szPath)
{
	CloseArchive();

	parchiveFile = new CArch(szPath);
	CArch *pArch = (CArch*) parchiveFile;

	if ( !pArch->bOpened ) {
		delete pArch;
		parchiveFile = NULL;

		return archiveType;
	}

	archiveType = ARCHIVE_ARCH;


	return archiveType;
}

int cArchive::OpenArchive(char *szPath)
{
	if ( OpenZIP(szPath)==ARCHIVE_NONE	)	//check if ZIP
		;//OpenARCH(szPath);					//if not ZIP check if ARCH
			//ARCH format dont have id, so it can't be used.

	return archiveType;
}

int cArchive::IsOpenedArchive()
{
	return archiveType;
}

BOOL cArchive::CloseArchive()
{
	if (archiveType==ARCHIVE_ZIP) {

		unzClose( (unzFile)parchiveFile );

		parchiveFile = NULL;

	}
	else if (archiveType==ARCHIVE_ARCH) {

		CArch *pArch = (CArch*) parchiveFile;

		delete pArch;
		parchiveFile = NULL;

	}

	archiveType = ARCHIVE_NONE;

	return TRUE;
}


void *cArchive::ReadFile(char *szFilename, unsigned long *retFilesize)
{
	if (!szFilename) return NULL;

	DWORD dwFilesize = SizeFile(szFilename);

	BYTE *pFilebuff = new BYTE [ dwFilesize ];
	if (ReadFile(szFilename, pFilebuff, retFilesize) )
		return pFilebuff;

	delete []pFilebuff;

	return NULL;
}

int cArchive::ReadFile(char *szFilename, BYTE *pFilebuff,
								 DWORD *retFilesize)
{
	if (!szFilename) return FALSE;

	if (archiveType==ARCHIVE_ZIP)
	{
		unzFile file = (unzFile)parchiveFile;

		*retFilesize = 0;

		if (unzLocateFile(file, szFilename, FALSE) != UNZ_OK)
			return FALSE;
		
		unz_file_info file_info;

		unzGetCurrentFileInfo( file, &file_info, NULL, 0, NULL, 0,
								NULL, 0 );

		*retFilesize = file_info.uncompressed_size;
		
		if ( unzOpenCurrentFile(file) != UNZ_OK )
			return FALSE;

		if ( unzReadCurrentFile(file, (void*)pFilebuff, *retFilesize) 
				!= (int)*retFilesize ) 
			return FALSE;

		if ( unzCloseCurrentFile(file) == UNZ_CRCERROR )
			return FALSE;


		return TRUE;

	} 
	else if (archiveType==ARCHIVE_ARCH)
	{
		CArch *pArch = (CArch*) parchiveFile;
		*retFilesize = pArch->Size(szFilename);
		return pArch->Read(szFilename, (void*)pFilebuff);
	}
	
	return FALSE;
}

unsigned long cArchive::SizeFile(char *szFilename)
{
	if (!szFilename) return NULL;

	unsigned long retFilesize = 0;

	if (archiveType==ARCHIVE_ZIP)
	{
		unzFile file = (unzFile)parchiveFile;
		
		if (unzLocateFile(file, szFilename, FALSE) != UNZ_OK)
			return 0;
		
		unz_file_info file_info;

		unzGetCurrentFileInfo( file, &file_info, NULL, 0, NULL, 0,
								NULL, 0 );

		retFilesize = file_info.uncompressed_size;
	} 
	else if (archiveType==ARCHIVE_ARCH)
	{
		CArch *pArch = (CArch*) parchiveFile;
		retFilesize = pArch->Size(szFilename);
	}
	
	return retFilesize;	
}

unsigned long cArchive::CompressedSizeFile(char *szFilename)
{
	if (!szFilename) return NULL;

	unsigned long retFilesize = 0;

	if (archiveType==ARCHIVE_ZIP)
	{
		unzFile file = (unzFile)parchiveFile;
		
		if (unzLocateFile(file, szFilename, FALSE) != UNZ_OK)
			return 0;
		
		unz_file_info file_info;

		unzGetCurrentFileInfo( file, &file_info, NULL, 0, NULL, 0,
								NULL, 0 );

		retFilesize = file_info.compressed_size;
	} 
	else if (archiveType==ARCHIVE_ARCH)
	{
		CArch *pArch = (CArch*) parchiveFile;
		retFilesize = pArch->CompressedSize(szFilename);
	}
	
	return retFilesize;	

	return 0;
}