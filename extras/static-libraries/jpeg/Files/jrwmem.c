#include "jinclude.h"



unsigned int JFREAD(char *file,void *buf,unsigned int sizeofbuf)
{	
	memcpy(buf, file+jindexr, sizeofbuf);
	jindexr += sizeofbuf;
	return sizeofbuf;
}