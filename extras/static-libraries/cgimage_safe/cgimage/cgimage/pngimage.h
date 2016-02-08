#if !defined ( FILE_PNGIMAGE_H )
#define FILE_PNGIMAGE_H 

#include "../png/png.h"

struct GIMAGE
{
	int width, height, bpp;
	unsigned char *pdata;
};

int check_if_png_file(char *file_name);
int read_png_from_file(GIMAGE *gi, char *file_name, int windows_compatible);
int check_if_png_file_in_memory(void *file_in_memory);
int read_png_from_file_in_memory(GIMAGE *gi, 
	void *file_in_memory, int file_size, int windows_compatible);

#endif