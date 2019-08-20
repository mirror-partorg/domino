/**
 * @file   ole.c
 * @author Alex Ott, Victor B Wagner
 * @date   Wed Jun 11 12:33:01 2003
 * Version: $Id: ole.c,v 1.2 2006/02/25 15:28:14 vitus Exp $
 * Copyright: Victor B Wagner, 1996-2003 Alex Ott, 2003
 *
 * @brief  Parsing structure of MS Office compound document
 *
 * This file is part of catdoc project
 * and distributed under GNU Public License
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dolib.h>
#include "ole.h"
#include "numutils.h"

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

struct _ole_t {
    FILE *f;
    int tmp_f;
    long int sectorSize, shortSectorSize;
    /* BBD Info */
    long int  bbdNumBlocks;
    unsigned char *BBD;
    /* SBD Info */
    long int sbdNumber, sbdStart, sbdLen;
    unsigned char *SBD;
    oleEntry *rootEntry;
    /* Properties Info */
    long propCurNumber, propLen, propNumber, propStart;
    unsigned char *properties;
    long int fileLength;
};


int ole_seek(ole_t *ole, FILE *stream, long offset, int whence);
long ole_tell(FILE *stream);
int ole_eof(FILE *stream);
size_t ole_read(ole_t *ole, void *ptr, size_t size, size_t nmemb, FILE *stream);

static unsigned char ole_sign[]={0xD0,0xCF,0x11,0xE0,0xA1,0xB1,0x1A,0xE1,0};

ole_t *ole_init(FILE *f)
{
    ole_t *ole;
    ole = do_malloc0(sizeof(ole_t));

	unsigned char oleBuf[BBD_BLOCK_SIZE];
	unsigned char *tmpBuf;
	int ret = 0, i;
	long int sbdMaxLen, sbdCurrent, propMaxLen, propCurrent, mblock, msat_size;
	oleEntry *tEntry;

	if ( fseek(f, 0, SEEK_SET) == -1 ) {
		if ( errno == ESPIPE ) {
			// We got non-seekable file, create temp file
			if( (ole->f = tmpfile()) == NULL ) {
				do_log(LOG_ERR, "Can't create tmp file \"%s\"", strerror(errno));
				ole_finish(ole);
				return NULL;
			}
			ole->tmp_f = 1;
			while ( !feof(f) ) {
				ret = fread(oleBuf, 1, BBD_BLOCK_SIZE, f);
				if ( fwrite(oleBuf, 1, ret, ole->f) != ret ) {
				    do_log(LOG_ERR, "Can't write to file \"%s\"", strerror(errno));
				    ole_finish(ole);
				    return NULL;
				}
			}
			fseek(ole->f, 0, SEEK_SET);
		}
		else {
			do_log(LOG_ERR, "Can't seek in file \"%s\"", strerror(errno));
			ole_finish(ole);
			return NULL;
		}
	}
	else
		ole->f = f;

	fseek(ole->f, 0, SEEK_END);
	ole->fileLength = ftell(ole->f);
// 	do_log(LOG_INFO, "fileLength=%ld\n", fileLength);
	fseek(ole->f, 0, SEEK_SET);
	ret = fread(oleBuf, 1, BBD_BLOCK_SIZE, ole->f);
	if ( ret != BBD_BLOCK_SIZE ) {
	    ole_finish(ole);
		return NULL;
	}
	if ( strncmp((char*)oleBuf, (char*)ole_sign, 8) != 0 ) {
	    ole_finish(ole);
		return NULL;
	}
 	ole->sectorSize = 1 << get_uint16((char*)oleBuf, 0x1e);
	ole->shortSectorSize = 1 << get_uint16((char*)oleBuf, 0x20);

// Read BBD into memory
	ole->bbdNumBlocks = get_uint32((char*)oleBuf, 0x2c);
	if ( (ole->BBD = do_malloc(ole->bbdNumBlocks*ole->sectorSize)) == NULL ) {
	    ole_finish(ole);
		return NULL;
	}

	if ( (tmpBuf = do_malloc(MSAT_ORIG_SIZE)) == NULL ) {
	    ole_finish(ole);
		return NULL;
	}
	memcpy(tmpBuf, oleBuf+0x4c, MSAT_ORIG_SIZE);
	mblock = get_int32((char*)oleBuf, 0x44);
	msat_size = get_int32((char*)oleBuf, 0x48);

// 	do_log(LOG_INFO, "msat_size=%ld\n", msat_size);

	i = 0;
	while ( (mblock >= 0) && (i < msat_size) ) {
		unsigned char *newbuf;
// 		do_log(LOG_INFO, "i=%d mblock=%ld\n", i, mblock);
		if ( (newbuf = do_realloc(tmpBuf, ole->sectorSize*(i+1)+MSAT_ORIG_SIZE)) != NULL ) {
			tmpBuf = newbuf;
		}
		else {
			do_log(LOG_ERR, "MSAT realloc error");
			do_free(tmpBuf);
			ole_finish(ole);
			return NULL;
		}

		fseek(ole->f, 512+mblock*ole->sectorSize, SEEK_SET);
		if ( fread(tmpBuf+MSAT_ORIG_SIZE+(ole->sectorSize-4)*i,
						 1, ole->sectorSize, ole->f) != ole->sectorSize ) {
			do_log(LOG_ERR, "Error read MSAT!");
			do_free(tmpBuf);
			ole_finish(ole);
			return NULL;
		}

		i++;
		mblock = get_int32((char*)tmpBuf, MSAT_ORIG_SIZE+(ole->sectorSize-4)*i);
	}

// 	do_log(LOG_INFO, "bbdNumBlocks=%ld\n", bbdNumBlocks);
	for ( i = 0; i < ole->bbdNumBlocks; i++ ) {
		long int bbdSector = get_int32((char*)tmpBuf, 4*i);

		if ( bbdSector >= ole->fileLength/ole->sectorSize || bbdSector < 0 ) {
			do_log(LOG_ERR, "Bad BBD entry!");
			ole_finish(ole);
			do_free(tmpBuf);
			return NULL;
		}
		fseek(ole->f, 512+bbdSector*ole->sectorSize, SEEK_SET);
		if ( fread(ole->BBD+i*ole->sectorSize, 1, ole->sectorSize, ole->f) != ole->sectorSize ) {
			do_log(LOG_ERR, "Can't read BBD!");
			free(tmpBuf);
			ole_finish(ole);
			return NULL;
		}
	}
	do_free(tmpBuf);

/* Read SBD into memory */
	ole->sbdLen = 0;
	sbdMaxLen=10;
	sbdCurrent = ole->sbdStart = get_int32((char*)oleBuf, 0x3c);
	if ( ole->sbdStart > 0 ) {
		if( (ole->SBD=malloc(ole->sectorSize*sbdMaxLen)) == NULL ) {
			ole_finish(ole);
			return NULL;
		}
		while ( TRUE ) {
			fseek(ole->f, 512+sbdCurrent*ole->sectorSize, SEEK_SET);
			if ( fread(ole->SBD+ole->sbdLen*ole->sectorSize, 1, ole->sectorSize, ole->f) != ole->sectorSize ) {
			    do_log(LOG_ERR, "Can't read in file \"%s\"", strerror(errno));
			    ole_finish(ole);
			    return NULL;
			}
			ole->sbdLen++;
			if ( ole->sbdLen >= sbdMaxLen ) {
				unsigned char *newSBD;

				sbdMaxLen+=5;
				if ( (newSBD=do_realloc(ole->SBD, ole->sectorSize*sbdMaxLen)) != NULL ) {
					ole->SBD = newSBD;
				}
				else {
					do_log(LOG_ERR, "SBD realloc error");
					ole_finish(ole);
					return NULL;
				}
			}
			sbdCurrent = get_int32((char*)ole->BBD, sbdCurrent*4);
			if ( sbdCurrent < 0 ||
				 sbdCurrent >= ole->fileLength/ole->sectorSize )
				break;
		}
		ole->sbdNumber = (ole->sbdLen*ole->sectorSize)/ole->shortSectorSize;
//   		do_log(LOG_INFO, "sbdLen=%ld sbdNumber=%ld\n",sbdLen, sbdNumber);
	}
	else
		ole->SBD = NULL;

// Read property catalog into memory
	ole->propLen = 0;
	propMaxLen = 5;
	propCurrent = ole->propStart = get_int32((char*)oleBuf, 0x30);
	if ( ole->propStart >= 0 ) {
		if( (ole->properties = malloc(propMaxLen*ole->sectorSize)) == NULL ) {
			ole_finish(ole);
			return NULL;
		}
		while ( TRUE ) {
//  			do_log(LOG_INFO, "propCurrent=%ld\n",propCurrent);
			fseek(ole->f, 512+propCurrent*ole->sectorSize, SEEK_SET);
			if ( fread(ole->properties+ole->propLen*ole->sectorSize,
				  1, ole->sectorSize, ole->f) != ole->sectorSize ) {
			    do_log(LOG_ERR, "Can't read in file \"%s\"", strerror(errno));
			    ole_finish(ole);
			    return NULL;
			}
				  ;
			ole->propLen++;
			if ( ole->propLen >= propMaxLen ) {
				unsigned char *newProp;

				propMaxLen+=5;
				if ( (newProp = do_realloc(ole->properties, propMaxLen*ole->sectorSize)) != NULL )
					ole->properties = newProp;
				else {
					do_log(LOG_ERR, "Properties realloc error");
					ole_finish(ole);
					return NULL;
				}
			}

			propCurrent = get_int32((char*)ole->BBD, propCurrent*4);
			if ( propCurrent < 0 ||
			     propCurrent >= ole->fileLength/ole->sectorSize ) {
                break;
			}
		}
//  		do_log(LOG_INFO, "propLen=%ld\n",propLen);
		ole->propNumber = (ole->propLen*ole->sectorSize)/PROP_BLOCK_SIZE;
		ole->propCurNumber = 0;
	}
	else {
		ole_finish(ole);
		return NULL;
	}

// Find Root Entry
	while ( (tEntry=(oleEntry*)ole_readdir(ole)) != NULL) {
		if (tEntry->type == oleRootDir ) {
			ole->rootEntry=tEntry;
			break;
		}
		ole_close((FILE*)tEntry);
	}
	ole->propCurNumber = 0;
	fseek(ole->f, 0, SEEK_SET);
	if ( !ole->rootEntry ) {
		do_log(LOG_ERR, "Broken OLE structure. Cannot find root entry in this file!");
		ole_finish(ole);
		return NULL;
	}
	return ole;
}

static inline int rightOleType(unsigned char *oleBuf)
{
	return (oleBuf[0x42] == 1 || oleBuf[0x42] == 2 ||
			oleBuf[0x42] == 3 || oleBuf[0x42] == 5 );
}
static inline oleType getOleType(unsigned char *oleBuf)
{
	return (oleType)((unsigned char)oleBuf[0x42]);
}

FILE *ole_readdir(ole_t *ole)
{
	int i, nLen;
	unsigned char *oleBuf;
	oleEntry *e=NULL;
	long int chainMaxLen, chainCurrent;

	if ( ole->properties == NULL || ole->propCurNumber >= ole->propNumber || ole->f == NULL )
		return NULL;

	oleBuf = ole->properties + ole->propCurNumber*PROP_BLOCK_SIZE;
	if( !rightOleType(oleBuf) )
		return NULL;
	if ( (e = (oleEntry*)do_malloc(sizeof(oleEntry))) == NULL ) {
		do_log(LOG_ERR, "Can't allocate memory");
		return NULL;
	}

	e->dirPos=oleBuf;
	e->type=getOleType(oleBuf);
	e->file=ole->f;
	e->startBlock=get_int32((char*)oleBuf,0x74);
	e->blocks=NULL;

	nLen=get_uint16((char*)oleBuf,0x40);
	for ( i=0 ; i < nLen /2; i++ )
		e->name[i]=(char)oleBuf[i*2];
	e->name[i]='\0';
	ole->propCurNumber++;
	e->length=get_uint32((char*)oleBuf,0x78);
// Read sector chain for object
	chainMaxLen = 25;
	e->numOfBlocks = 0;
	chainCurrent = e->startBlock;
	e->isBigBlock = (e->length >= 0x1000) || !strcmp(e->name, "Root Entry");
// 	do_log(LOG_INFO, "e->name=%s e->length=%ld\n", e->name, e->length);
// 	do_log(LOG_INFO "e->startBlock=%ld BBD=%p\n", e->startBlock, BBD);
	if ( e->startBlock >= 0 &&
		 e->length >= 0 &&
		( e->startBlock <=
		  ole->fileLength/(e->isBigBlock ? ole->sectorSize : ole->shortSectorSize)) ) {
		if ( (e->blocks = do_malloc(chainMaxLen*sizeof(long int))) == NULL ) {
		    ole_close((FILE*)e);
			return NULL;
		}
		while ( TRUE ) {
// 			do_log(LOG_INFO, "chainCurrent=%ld\n", chainCurrent);
			e->blocks[e->numOfBlocks++] = chainCurrent;
			if (e->numOfBlocks >= chainMaxLen) {
				long int *newChain;
				chainMaxLen+=25;
				if ( (newChain = do_realloc(e->blocks,
									  chainMaxLen*sizeof(long int))) != NULL )
					e->blocks = newChain;
				else {
					do_log(LOG_ERR, "Properties realloc error");
					ole_close((FILE*)e);
					return NULL;
				}
			}
			if ( e->isBigBlock ) {
				chainCurrent = get_int32((char*)ole->BBD, chainCurrent*4);
			}
			else if ( ole->SBD != NULL ) {
				chainCurrent = get_int32((char*)ole->SBD, chainCurrent*4);
			}
			else {
				chainCurrent=-1;
			}
			if ( chainCurrent <= 0 ||
			     chainCurrent >= ( e->isBigBlock ?
								 ((ole->bbdNumBlocks*ole->sectorSize)/4)
								 : ((ole->sbdNumber*ole->shortSectorSize)/4) ) ||
			   (e->numOfBlocks >
				e->length/(e->isBigBlock ? ole->sectorSize : ole->shortSectorSize))) {
//   				do_log(LOG_INFO, "chain End=%ld\n", chainCurrent);
				break;
			}
		}
	}

	if ( e->length > (e->isBigBlock ? ole->sectorSize : ole->shortSectorSize)*e->numOfBlocks)
        e->length = (e->isBigBlock ? ole->sectorSize : ole->shortSectorSize)*e->numOfBlocks;
// 	do_log(LOG_INFO, "READDIR: e->name=%s e->numOfBlocks=%ld length=%ld\n",
// 					e->name, e->numOfBlocks, e->length);

	return (FILE*)e;
}

int ole_open(FILE *stream) {
	oleEntry *e=(oleEntry *)stream;
	if ( e->type != oleStream)
		return -2;

	e->ole_offset=0;
	e->file_offset= ftell(e->file);
	return 0;
}

static long int calcFileBlockOffset(ole_t *ole, oleEntry *e, long int blk)
{
	long int res;
	if ( e->isBigBlock ) {
		res = 512+e->blocks[blk]*ole->sectorSize;
	} else {
		long int sbdPerSector=ole->sectorSize/ole->shortSectorSize;
		long int sbdSecNum=e->blocks[blk]/sbdPerSector;
		long int sbdSecMod=e->blocks[blk]%sbdPerSector;
// 		do_log(LOG_INFO, "calcoffset: e->name=%s e->numOfBlocks=%ld length=%ld sbdSecNum=%ld rootEntry->blocks=%p\n",
// 						e->name, e->numOfBlocks, e->length, sbdSecNum, rootEntry->blocks);
		res=512 + ole->rootEntry->blocks[sbdSecNum]*ole->sectorSize + sbdSecMod*ole->shortSectorSize;
	}
	return res;
}

size_t ole_read(ole_t *ole, void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	oleEntry *e = (oleEntry*)stream;
	long int llen = size*nmemb, rread=0, i;
	long int blockNumber, modBlock, toReadBlocks, toReadBytes, bytesInBlock;
	long int ssize;				/**< Size of block */
	long int newoffset;
	unsigned char *cptr = ptr;
	if( e->ole_offset+llen > e->length )
		llen= e->length - e->ole_offset;

	ssize = (e->isBigBlock ? ole->sectorSize : ole->shortSectorSize);
	blockNumber=e->ole_offset/ssize;
// 	do_log(LOG_INFO, "blockNumber=%ld e->numOfBlocks=%ld llen=%ld\n",
// 			blockNumber, e->numOfBlocks, llen);
	if ( blockNumber >= e->numOfBlocks || llen <=0 )
		return 0;

	modBlock=e->ole_offset%ssize;
	bytesInBlock = ssize - modBlock;
	if(bytesInBlock < llen) {
		toReadBlocks = (llen-bytesInBlock)/ssize;
		toReadBytes = (llen-bytesInBlock)%ssize;
	}
	else {
		toReadBlocks = toReadBytes = 0;
	}
// 	do_log(LOG_INFO, "llen=%ld toReadBlocks=%ld toReadBytes=%ld bytesInBlock=%ld blockNumber=%ld modBlock=%ld\n",
// 			llen, toReadBlocks, toReadBytes, bytesInBlock, blockNumber, modBlock);
	newoffset = calcFileBlockOffset(ole, e,blockNumber)+modBlock;
	if ( e->file_offset != newoffset ) {
		fseek(e->file, e->file_offset=newoffset, SEEK_SET);
	}
	rread=fread(ptr, 1, min(llen,bytesInBlock), e->file);
	e->file_offset += rread;
	for(i=0; i<toReadBlocks; i++) {
		int readbytes;
		blockNumber++;
		newoffset = calcFileBlockOffset(ole,e,blockNumber);
		if (newoffset != e->file_offset);
		fseek(e->file, e->file_offset=newoffset , SEEK_SET);
		readbytes=fread(cptr+rread, 1, min(llen-rread, ssize), e->file);
		rread +=readbytes;
		e->file_offset +=readbytes;
	}
	if(toReadBytes > 0) {
		int readbytes;
		blockNumber++;
		newoffset = calcFileBlockOffset(ole, e,blockNumber);
		fseek(e->file, e->file_offset=newoffset, SEEK_SET);
        readbytes=fread(cptr+rread, 1, toReadBytes,e ->file);
		rread +=readbytes;
		e->file_offset +=readbytes;
	}
//	do_log(LOG_INFO, "ole_offset=%ld rread=%ld llen=%ld\n",
//	e->ole_offset, rread, llen);
	e->ole_offset+=rread;
	return rread;
}

int ole_eof(FILE *stream)
{
	oleEntry *e=(oleEntry*)stream;
//	do_log(LOG_INFO, "EOF: e->ole_offset=%ld  e->length=%ld\n",
//	e->ole_offset,  e->length);
	return (e->ole_offset >=  e->length);
}

void ole_finish(ole_t *ole)
{
    if ( ole->f && ole->tmp_f ) fclose(ole->f);
	if ( ole->BBD != NULL ) do_free(ole->BBD);
	if ( ole->SBD != NULL ) do_free(ole->SBD);
	if ( ole->properties != NULL ) do_free(ole->properties);
	if ( ole->rootEntry != NULL ) ole_close((FILE*)ole->rootEntry);
	do_free(ole);
}

int ole_close(FILE *stream)
{
	oleEntry *e=(oleEntry*)stream;
	if(e == NULL)
		return -1;
	if (e->blocks != NULL)
		free(e->blocks);
	free(e);
	return 0;
}

int ole_seek(ole_t *ole, FILE *stream, long offset, int whence)
{

	oleEntry *e=(oleEntry*)stream;
	long int new_ole_offset=0, new_file_offset;
	int ssize, modBlock, blockNumber;

	switch(whence) {
	case SEEK_SET:
		new_ole_offset=offset;
		break;

	case SEEK_CUR:
		new_ole_offset=e->ole_offset+offset;
		break;

	case SEEK_END:
		new_ole_offset=e->length+offset;
		break;

	default:
		errno=EINVAL;
		return -1;
	}
	if(new_ole_offset<0)
		new_ole_offset=0;
	if(new_ole_offset >= e->length)
		new_ole_offset=e->length;

	ssize = (e->isBigBlock ? ole->sectorSize : ole->shortSectorSize);
	blockNumber=new_ole_offset/ssize;
	if ( blockNumber >= e->numOfBlocks )
		return -1;

	modBlock=new_ole_offset%ssize;
	new_file_offset = calcFileBlockOffset(ole, e, blockNumber)+modBlock;
	fseek(e->file, e->file_offset=new_file_offset, SEEK_SET);
	e->ole_offset=new_ole_offset;

	return 0;
}

long ole_tell(FILE *stream) {
	oleEntry *e=(oleEntry*)stream;
	return e->ole_offset;
}


