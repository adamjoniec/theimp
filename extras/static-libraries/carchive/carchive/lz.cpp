/*-----------------------------------------------------------------------------
	Lempel-Zip compression and decompression library,
	very fast(faster than zlib), but with worse compression level
-----------------------------------------------------------------------------*/

#define UCHAR unsigned char
#define UWORD unsigned short
#define ULONG unsigned long
#define FLAG_BYTES    4 
#define FLAG_COMPRESS 0
#define FLAG_COPY     1

void FastCopy(UCHAR *pScr,UCHAR *pDst,int len)
{
	while (len--) *pDst++=*pScr++;
}


/*	lzcompress()
		pSource - pointer to source data
		dwSourceSize - size of source data
		pDest - pointer to memory when compressed data will be stored,
				memory for compressed data must be larger then pSource memory 
				at 256 bytes 
		dwDestSize - this is the return size of compressed data				
*/

void lzcompress(UCHAR *pSource, ULONG dwSourceSize,UCHAR* pDest, ULONG *dwDestSize)
{
#define PS *p++!=*s++
#define ITEMMAX 16

UCHAR *pScr = pSource,   *pDst = pDest;
UCHAR *pScr_post = pSource+dwSourceSize,*pDst_post=pDest+dwSourceSize;
UCHAR *pScr_max1=pScr_post-ITEMMAX,*pScr_max16=pScr_post-16*ITEMMAX;
UCHAR *hash[4096],*p_control; 
UWORD control=0,control_bits=0;

 *pDst=FLAG_COMPRESS; pDst+=FLAG_BYTES; p_control=pDst; pDst+=2;
	while(1)
	{
		UCHAR *p,*s; UWORD unroll=16,len,index; ULONG offset;
		if (pDst>pDst_post) goto overrun;
		if (pScr>pScr_max16)
		{
			unroll=1;
			if (pScr>pScr_max1)
			{
				if (pScr==pScr_post) break; goto literal;
			}
		}
	begin_unrolled_loop:

		index=((40543*((((pScr[0]<<4)^pScr[1])<<4)^pScr[2]))>>4) & 0xFFF;
		p=hash[index]; hash[index]=s=pScr; offset=s-p;
		if (offset>4095 || p<pSource || offset==0 || PS || PS || PS)
		{
			literal: *pDst++=*pScr++; control>>=1; control_bits++;
		}
		else
        {
			PS || PS || PS || PS || PS || PS || PS ||
			PS || PS || PS || PS || PS || PS || s++; 
			len=s-pScr-1;
			*pDst++=(UCHAR)(((offset&0xF00)>>4)+(len-1)); 
			*pDst++=(UCHAR)(offset&0xFF);
			pScr+=len; control=(control>>1)|0x8000; control_bits++;
		}

		if (--unroll) goto begin_unrolled_loop;

		if (control_bits==16)
		{
			*p_control=control&0xFF; *(p_control+1)=control>>8;
			p_control=pDst; pDst+=2; control=control_bits=0;
		}
	}
	control>>=16-control_bits;
	*p_control++=control&0xFF; *p_control++=control>>8;
	if (p_control==pDst) pDst-=2;
	*dwDestSize=pDst-pDest;
	return;

overrun: 

	FastCopy(pSource,pDest+FLAG_BYTES,dwSourceSize);
	*pDest=FLAG_COPY; *dwDestSize=dwSourceSize+FLAG_BYTES;
}


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
void lzuncompress(UCHAR *pSource, ULONG dwSourceSize, UCHAR *pDest, ULONG *dwDestSize)
{
UWORD controlbits = 0, control;
UCHAR *pScr = pSource + FLAG_BYTES;
UCHAR *pDst = pDest;
UCHAR *pScr_post = pSource + dwSourceSize;

	if (*pSource==FLAG_COPY)
	{
		FastCopy(pSource + FLAG_BYTES, pDest, dwSourceSize - FLAG_BYTES);
		*dwDestSize = dwSourceSize - FLAG_BYTES; 
		return;
	}
	while (pScr!=pScr_post)
	{
		if (controlbits==0)
		{
			control=*pScr++; 
			control|=(*pScr++)<<8; 
			controlbits=16;
		}
		if (control&1)
		{
			UWORD offset, len; 
			UCHAR *p;

			offset=(*pScr&0xF0)<<4; 
			len=1+(*pScr++&0xF);
			offset+=*pScr++&0xFF; 
			p=pDst-offset;
			while (len--) *pDst++=*p++;
		}
		else
			*pDst++=*pScr++;

		control>>=1; 
		controlbits--;
	}
	*dwDestSize = pDst-pDest;
}