/*
	supported graphics formats: PNG

	Using:
			include this file
			add library files:  cgimage.lib (cgimaged.lib for debug)
								carchive.lib (for loading from compressed files)
*/

#if !defined ( FILE_CGIMAGE_H_INCLUDED )
#define FILE_CGIMAGE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <carchive.h>


class cGImage
{

	//image is in up-down direction
	//always RGB or RGBA
	unsigned char *pPixels;

public:

	int x, y, bpp;		//bpp - may be:
						//				24 - R8G8B8
						//				32 - R8G8B8A8
	HBITMAP hBitmap;


	cGImage() {
		pPixels = NULL;
		hBitmap = NULL;
	}

	~cGImage() {
		if (hBitmap) DeleteObject(hBitmap);
		if (pPixels) delete[] pPixels;
	}


	//load image from file pszFilename
	int LoadImage(char *pszFilename);

	//load image from file in memory(pFileBuff)
	//file size is iFilesize
	int LoadImage(void *pFileBuff, int iFilesize);

	//load image from compressed file in archive
	int LoadImage(char *pszFilename, cArchive *pArchive);



	int LoadPNG(char *pszFilename);
	int LoadPNG(void *pFilebuff, int iFilesize);
	int LoadPNG(char *pszFilename, cArchive *pArchive);

	//convert RGBA pixels to bitmap and free pPixels
	//from now cGImage keep only hBitmap
	//return HBITMAP same as hBitmap
	HBITMAP ConvertToBitmap(HWND hWnd);

	void *GetPixels();


};

#endif // !defined ( FILE_CGIMAGE_H_INCLUDED )