/*-----------------------------------------------------------------------------
	Header to use with JPEG.LIB.

functions:		int ReadJPEG (char * buffer, stJPEG *Image)
						[buffer] - buffer with compressed JPEG, it's freing after
						   decompression
						[stJPEG] - structure with decompressed JPEG
							{
								DWORD	dwWidth, dwHeight;
								unsigned char ucData; 
									[ucData] - pointer to decompressed data BGR (blue,green,red)
							}


				HBITMAP ConvertJPEGtoBITMAP(stJPEG *Info, HWND hWnd)
						[stJPEG] - structure with decompresed JPEG
						[hWnd] - handle to window when bitmap will be show
					

																Adam Joniec [-SAuron-]
																	saur@cad.pl
-----------------------------------------------------------------------------*/


#include <windows.h>
#include "jpegdata.h"


int jwidthline;

struct stJPEG
{
	DWORD dwWidth, dwHeight;
	unsigned char *ucData;
};


static void output_init (decompress_info_ptr cinfo)
{
	jwidthline = cinfo->image_width * 3;
	if ( jwidthline%4 )
		jwidthline = ((jwidthline/4)*4)+4;

	cinfo->output_image = new char[jwidthline * cinfo->image_height];	
	cinfo->output_imageptr = cinfo->output_image + (jwidthline*cinfo->image_height) - jwidthline;

}
static void put_color_map (decompress_info_ptr cinfo, int num_colors, JSAMPARRAY colormap)
{
}
static void put_pixel_rows (decompress_info_ptr cinfo, int num_rows, JSAMPIMAGE pixel_data)
{
register char *image;
register JSAMPROW ptr0, ptr1, ptr2;
register int col, row;

	for (row = 0; row < num_rows; row++) 
	{
		ptr0 = pixel_data[0][row];
		ptr1 = pixel_data[1][row];
		ptr2 = pixel_data[2][row];
		image = cinfo->output_imageptr;
		for (col = 0; col < cinfo->image_width; col++) 
		{
			*image = *ptr2;
			image++; ptr2++;

			*image = *ptr1;
			image++; ptr1++;

			*image = *ptr0;
			image++; ptr0++;
		}	
		cinfo->output_imageptr -= jwidthline;
	}
}

static void output_term (decompress_info_ptr cinfo)
{

}
static void d_ui_method_selection (decompress_info_ptr cinfo)
{
	cinfo->methods->output_init = output_init;
	cinfo->methods->put_color_map = put_color_map;
	cinfo->methods->put_pixel_rows = put_pixel_rows;
	cinfo->methods->output_term = output_term;
}

int ReadJPEG (char * buffer, stJPEG *Image)
{
	struct Decompress_info_struct cinfo;
	struct Decompress_methods_struct dc_methods;
	struct External_methods_struct e_methods;


	cinfo.methods = &dc_methods;
	cinfo.emethods = &e_methods;
	jselerror(cinfo.emethods);
	jselmemmgr(cinfo.emethods);
	cinfo.methods->d_ui_method_selection = d_ui_method_selection;
	j_d_defaults(&cinfo, TRUE);
	jselrjfif(&cinfo);

	cinfo.input_filebuffer = buffer;

	jpeg_decompress(&cinfo);

	delete cinfo.input_filebuffer;

	Image->dwWidth = cinfo.image_width;
	Image->dwHeight = cinfo.image_height;
	Image->ucData = (unsigned char*)cinfo.output_image;

	return 1;


}

HBITMAP ConvertJPEGtoBITMAP(stJPEG *Info, HWND hWnd)
{
	HBITMAP hBitmap;
	BITMAPINFOHEADER bhinfo;
	BITMAPINFO binfo;

	bhinfo.biSize = sizeof(BITMAPINFOHEADER);
	bhinfo.biWidth = Info->dwWidth;
	bhinfo.biHeight = Info->dwHeight;
	bhinfo.biPlanes = 1;
	bhinfo.biBitCount = 24;
	bhinfo.biCompression = BI_RGB;

	memcpy(&binfo.bmiHeader, &bhinfo, bhinfo.biSize);
	hBitmap = (	HBITMAP)CreateDIBitmap(GetDC(hWnd), &bhinfo, 
				CBM_INIT, Info->ucData, &binfo, DIB_RGB_COLORS);


	return hBitmap;
}
