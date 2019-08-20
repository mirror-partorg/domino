/*****************************************************************/
/* Utilities to convert various numeric types from the Windows   */
/* (Little endian) format to native types                        */
/*                                                               */
/* This file is part of catdoc project                           */
/* (c) Victor Wagner 1996-2003, (c) Alex Ott 2003	             */
/*****************************************************************/


/********************************************************************/
/* Reads 2-byte LSB  int from buffer at given offset platfom-indepent
 * way
 *********************************************************************/
 #include "numutils.h"

uint16_t get_uint16(void *buffer, int offset) {
	return (uint16_t)((unsigned char*)buffer)[offset]|((uint16_t)((unsigned char*)buffer)[offset+1]<<8);
}
/********************************************************************/
/* Reads 4-byte LSB  int from ((unsigned char*)buffer) at given offset almost platfom-indepent
 * way
 *********************************************************************/
int32_t get_int32(void *buffer, int offset) {
	return (int32_t)((unsigned char*)buffer)[offset]|((int32_t)((unsigned char*)buffer)[offset+1]<<8L)
		|((int32_t)((unsigned char*)buffer)[offset+2]<<16L)|((int32_t)((unsigned char*)buffer)[offset+3]<<24L);
}

uint32_t get_uint32(void *buffer, int offset) {
	return (uint32_t)((unsigned char*)buffer)[offset]|((uint32_t)((unsigned char*)buffer)[offset+1]<<8L)
		|((uint32_t)((unsigned char*)buffer)[offset+2]<<16L)|((uint32_t)((unsigned char*)buffer)[offset+3]<<24L);
}
double get_double(void *buffer, int offset)
{
	union { char cc[8];
		double d;} dconv;
	char *d,*s;
	int i;
# ifdef WORDS_BIGENDIAN
	for(s=(char*)buffer+offset+8,d=dconv.cc,i=0;
			i<8;i++) *(d++)=*(--s);
# else
	for(s=(char*)buffer+offset,d=dconv.cc,i=0;
			i<8;i++) *(d++)=*(s++);
# endif
    return dconv.d;
}
