#include "cgimage.h"
#include "pngimage.h"

int cGImage::LoadPNG(char *pszFilename)
{
	GIMAGE gi;

	if (pPixels) return 0;

	int ret = read_png_from_file(&gi, pszFilename, TRUE);

	if (ret) {
		w = gi.width;
		h = gi.height;
		bpp = gi.bpp;
		pPixels = gi.pdata;
	}

	return ret;
}

int cGImage::LoadPNG(void *pFileBuff, int iFilesize)
{
	GIMAGE gi;

	if (pPixels || !pFileBuff || !iFilesize) return 0;

	int ret = read_png_from_file_in_memory(&gi, pFileBuff, iFilesize, TRUE);

	if (ret) {
		w = gi.width;
		h = gi.height;
		bpp = gi.bpp;
		pPixels = gi.pdata;
	}

	return ret;
}

int cGImage::LoadPNG(char *pszFilename, cArchive *pArchive)
{
	GIMAGE gi;

	if (pPixels || !pArchive || !pszFilename) return 0;

	DWORD dwFilesize = 0;
	BYTE *pFileBuff = 
			(BYTE*)pArchive->ReadFile(pszFilename, &dwFilesize);

	if (!pFileBuff || !dwFilesize) return 0;

	int ret = read_png_from_file_in_memory(&gi, pFileBuff, dwFilesize, TRUE);

	if (ret) {
		w = gi.width;
		h = gi.height;
		bpp = gi.bpp;
		pPixels = gi.pdata;
	}

	delete[] pFileBuff;

	return ret;
}

int cGImage::LoadImage(char *pszFilename)
{
	if (pPixels || !pszFilename) return 0;

	if ( check_if_png_file(pszFilename) ) 
		return LoadPNG(pszFilename);

	return 0;
}

int cGImage::LoadImage(void *pFileBuff, int iFilesize)
{
	if (pPixels || !pFileBuff || !iFilesize) return 0;

	if (iFilesize>4 && check_if_png_file_in_memory(pFileBuff) )
		return LoadPNG(pFileBuff, iFilesize);

	return 0;
}

int cGImage::LoadImage(char *pszFilename, cArchive *pArchive)
{
	if (pPixels || !pArchive || !pszFilename) return 0;

	DWORD dwFilesize = 0;
	BYTE *pFileBuff = 
			(BYTE*)pArchive->ReadFile(pszFilename, &dwFilesize);

	if (!pFileBuff || !dwFilesize) return 0;

	int ret = 0;
	if (dwFilesize>4 && check_if_png_file_in_memory(pFileBuff) )
		ret = LoadPNG(pFileBuff, dwFilesize);

	delete[]pFileBuff;

	return ret;
}

HBITMAP cGImage::ConvertToBitmap(HWND hWnd)
{
	if ( pPixels == NULL || hBitmap || bpp<24 )
		return hBitmap;

	BITMAPINFO binfo;

	
	binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biWidth = w;
	binfo.bmiHeader.biHeight = -h;
	binfo.bmiHeader.biPlanes = 1;
	binfo.bmiHeader.biBitCount = bpp;

	binfo.bmiHeader.biCompression = BI_RGB;


	HDC hDC = GetDC(hWnd);;
	hBitmap = (	HBITMAP)CreateDIBitmap(GetDC(hWnd), &binfo.bmiHeader, 
				CBM_INIT, pPixels, &binfo, DIB_RGB_COLORS);

	ReleaseDC(hWnd, hDC);

	delete[] pPixels;
	pPixels = NULL;

	return hBitmap;
}

void * cGImage::GetPixels()
{
	return pPixels;
}

int cGImage::Create1bppAlpha()
{
	// Alpha should have DWORD line assigment, so we can later, create
	//  bitmap from it, direction is Top-Bottom.

	if ( !pPixels || bpp!=32 || pAlpha ) return FALSE;

	int linebytes = w/8 + 4 - (w/8)%4;

	pAlpha = new unsigned char[linebytes*h];
	ZeroMemory( pAlpha, linebytes*h );

	for ( int y=0; y<h; y++ )
	{
		unsigned char *psrc = pPixels + y*(w*4) + 3,
			*pdest = pAlpha + y*linebytes,
			bit = 1;

		for ( int x=0; x<w; x++ )
		{
			if ( *psrc ) *pdest |= bit; psrc += 4;
			bit <<= 1; if (!bit) { bit++; pdest++; }
		}
	}

	alphabpp = 1;
	alphalinebytes = linebytes;

	return TRUE;
}

// return 0 if full opaque, or 255 if full transparent
int cGImage::GetAlphaPixel(int x, int y)
{
	if ( !pAlpha ) return 0;

	unsigned char *pa = pAlpha;
	switch ( alphabpp )
	{
		case 1:
			pa += (y*alphalinebytes + x/8);
			return (*pa&(1<<(x%8)))?0:255;
		break;
	}

	return 0;
}