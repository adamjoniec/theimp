#include "pngimage.h"
#include <carchive.h>


#define PNG_BYTES_TO_CHECK 4

//return nonzero if png
int check_if_png_file(char *file_name)
{
	char buf[PNG_BYTES_TO_CHECK];


	FILE *fp = fopen(file_name, "rb");
	if (fp == NULL)
		return 0;

	/* Read in some of the signature bytes */
	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) 
	{
		fclose(fp);
		return 0;
	}
	

	/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	Return nonzero (true) if they match */

	return(!png_sig_cmp((unsigned char*)buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

int read_png_from_file(GIMAGE *gi, char *file_name, int windows_compatible)
{
   png_structp png_ptr;
   png_infop info_ptr;
   unsigned int sig_read = 0;
   png_uint_32 width, height;
   int bit_depth, color_type, interlace_type, pixel_depth;
   FILE *fp;

   if ((fp = fopen(file_name, "rb")) == NULL)
      return 0;


 /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
	*
	*	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
	*				png_voidp user_error_ptr, user_error_fn, user_warning_fn);
	*
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
									NULL, NULL, NULL);

   if (png_ptr == NULL)
   {
      fclose(fp);
      return 0;
   }

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      fclose(fp);
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return 0;
   }

    /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      fclose(fp);
      /* If we get here, we had a problem reading the file */
      return 0;
   }

    /* Set up the input control as standard C streams */
	png_init_io(png_ptr, fp);


    /* The call to png_read_info() gives us all of the information from the
     * PNG file before the first IDAT (image data chunk).
	 */
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);


	/**** Set up the data transformations ****/

	/* Expand paletted or RGB images with transparency to full alpha channels
	 * so the data will be available as RGBA quartets.
     */
	pixel_depth = 24;

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
          png_set_gray_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (png_get_valid(png_ptr, info_ptr,PNG_INFO_tRNS)) {
		pixel_depth += 8;
		png_set_tRNS_to_alpha(png_ptr);
	} else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
				color_type == PNG_COLOR_TYPE_RGB_ALPHA )
		pixel_depth += 8;

    if (windows_compatible)
        png_set_bgr(png_ptr);



    /* Allocate the memory to hold the image using the fields of info_ptr. */
	int row_bytes = width * pixel_depth / 8;
	if (windows_compatible && row_bytes%4) row_bytes = (row_bytes/4)*4 + 4;
	png_bytep image_data = new unsigned char[ row_bytes * height ];

	png_bytep *row_pointers = new png_bytep[height];

	for (unsigned int row = 0; row < height; row++)
	{
		row_pointers[row] = image_data + (row_bytes * row);
	}


	/* Read the entire image in one go */
	png_read_image(png_ptr, row_pointers);

    /* read rest of file, and get additional chunks in info_ptr  */ 
	png_read_end(png_ptr, info_ptr);
	

	gi->width = width; 
	gi->height = height;
	gi->pdata = image_data;
	gi->bpp = pixel_depth;


   /* At this point you have read the entire image */

   /* clean up after the read, and free any memory allocated  */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	delete[] row_pointers;

	/* close the file */
	fclose(fp);



   /* that's it */
   return 1;

}





///////////////////////////////////////////////////////////////////////
/////////////////// Functions to read PNG from memory /////////////////
///////////////////////////////////////////////////////////////////////

//return nonzero if png
int check_if_png_file_in_memory(void *file_in_memory)
{
	char buf[PNG_BYTES_TO_CHECK];


	if (file_in_memory == NULL)
		return 0;

	memcpy(buf, file_in_memory, PNG_BYTES_TO_CHECK); 

	/* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
	Return nonzero (true) if they match */

	return(!png_sig_cmp((unsigned char*)buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

struct st_file_in_memory {
	unsigned char *file_begin, *file_end;
};

void read_data_from_file_in_memory(png_structp read_ptr, png_bytep data, 
						   png_size_t length)
{
	st_file_in_memory *mp = (st_file_in_memory*)read_ptr->io_ptr;

	if ( mp->file_begin < mp->file_end )
	{
		memcpy(data, mp->file_begin, length );
		data = png_bytep( mp->file_begin );
		mp->file_begin += length;
	}

}

int read_png_from_file_in_memory(GIMAGE *gi, 
	void *file_in_memory, int file_size, int windows_compatible)
{
   png_structp png_ptr;
   png_infop info_ptr;
   unsigned int sig_read = 0;
   png_uint_32 width, height;
   int bit_depth, color_type, interlace_type, pixel_depth;
   st_file_in_memory mp;

   mp.file_begin = (unsigned char*)file_in_memory;
   mp.file_end = (unsigned char*)(file_in_memory) + file_size;

   if (file_in_memory == NULL || file_size == 0)
      return 0;


 /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
	*
	*	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
	*				png_voidp user_error_ptr, user_error_fn, user_warning_fn);
	*
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
									NULL, NULL, NULL);

   if (png_ptr == NULL)
   {
      delete[] file_in_memory;
      return 0;
   }

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
		delete[] file_in_memory;		
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      return 0;
   }

    /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      delete[] file_in_memory;
      /* If we get here, we had a problem reading the file */
      return 0;
   }

    /* Set up the input control as standard C streams */
	//png_init_io(png_ptr, fp);
	png_set_read_fn(png_ptr, &mp, read_data_from_file_in_memory);


    /* The call to png_read_info() gives us all of the information from the
     * PNG file before the first IDAT (image data chunk).
	 */
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);


	/**** Set up the data transformations ****/

	/* Expand paletted or RGB images with transparency to full alpha channels
	 * so the data will be available as RGBA quartets.
     */
	pixel_depth = 24;

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
          png_set_gray_to_rgb(png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    if (png_get_valid(png_ptr, info_ptr,PNG_INFO_tRNS)) {
		pixel_depth += 8;
		png_set_tRNS_to_alpha(png_ptr);
	} else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
				color_type == PNG_COLOR_TYPE_RGB_ALPHA )
		pixel_depth += 8;

    if (windows_compatible)
        png_set_bgr(png_ptr);



    /* Allocate the memory to hold the image using the fields of info_ptr. */
	int row_bytes = width * pixel_depth / 8;
	if (windows_compatible && row_bytes%4) row_bytes = (row_bytes/4)*4 + 4;
	png_bytep image_data = new unsigned char[ row_bytes * height ];

	png_bytep *row_pointers = new png_bytep[height];

	for (unsigned int row = 0; row < height; row++)
	{
		row_pointers[row] = image_data + (row_bytes * row);
	}


	/* Read the entire image in one go */
	png_read_image(png_ptr, row_pointers);

    /* read rest of file, and get additional chunks in info_ptr  */ 
	png_read_end(png_ptr, info_ptr);
	

	gi->width = width; 
	gi->height = height;
	gi->pdata = image_data;
	gi->bpp = pixel_depth;


   /* At this point you have read the entire image */

   /* clean up after the read, and free any memory allocated  */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	delete[] row_pointers;


   /* that's it */
   return 1;

}