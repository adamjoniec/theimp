/*-----------------------------------------------------------------------------
	Lempel-Zip compression and decompression library	
	very fast(faster than zlib), but with worse compression level

										[-writed by Adam Joniec-]
-----------------------------------------------------------------------------*/

/*	lzcompress()
		pSource - pointer to source data
		dwSourceSize - size of source data
		pDest - pointer to memory when compressed data will be stored,
				memory for compressed data must be larger then pSource memory 
				at 256 bytes 
		dwDestSize - this is the return size of compressed data				
*/
extern void lzcompress(	unsigned char *pSource,  unsigned long dwSourceSize,
						unsigned char *pDest,  unsigned long *dwDestSize );

/*	lzuncompress()
		pSource - pointer to source compressed data
		dwSourceSize - size of compressed data
		pDest - pointer to memory when decompressed data will be stored,
				memory size needed for decompressed data must be remembered when 
				compression was done
		dwDestSize - ths is return size o decompressed data
					 [you must know it before calling LZDecompress to
					  allocate memory for pDest] 
*/
extern void lzuncompress( unsigned char *pSource,  unsigned long dwSourceSize, 
						  unsigned char *pDest,  unsigned long *dwDestSize );

