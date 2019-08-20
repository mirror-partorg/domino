/***************************************************************************
  Type definitions for Stat/Create structure
****************************************************************************/
#ifndef _BTRSTRCT_H
#define _BTRSTRCT_H

#include <btrieve.h>
typedef struct PACK_STRUCT
{
  BTI_SINT recLength;
  BTI_SINT pageSize;
  BTI_SINT indexCount;
  BTI_LONG recCount;
  BTI_SINT flags;
  BTI_SINT reserve;
  BTI_SINT usagePages;
} FILE_SPECS;

typedef struct PACK_STRUCT
{
  BTI_SINT recLength;
  BTI_SINT pageSize;
  BTI_BYTE numberIndexs;
  BTI_BYTE fileVersion;
  BTI_LONG numberRecords;
  BTI_SINT flags;
  BTI_BYTE numberDublicate;
  BTI_BYTE reserve;
  BTI_SINT usagePages;
} FILE_SPECS1;

typedef struct PACK_STRUCT
{
  BTI_SINT position;
  BTI_SINT length;
  BTI_SINT flags;
  BTI_CHAR reserved[4];
  BTI_CHAR type;
  BTI_CHAR null;
  BTI_CHAR notUsed[2];
  BTI_BYTE manualKeyNumber;
  BTI_BYTE acsNumber;
} KEY_SPECS;

typedef struct PACK_STRUCT
{
  FILE_SPECS fileSpecs;
  KEY_SPECS  keySpecs[100];
} FILE_CREATE_BUF;

typedef struct PACK_STRUCT
{
  FILE_SPECS1 fileSpecs;
  KEY_SPECS  keySpecs[100];
} FILE_INFO_BUF;

/***************************************************************************
  Structure type definitions for Get Next Extended operation
****************************************************************************/
typedef struct PACK_STRUCT
{
  BTI_SINT    descriptionLen;
  BTI_CHAR    currencyConst[2];
  BTI_SINT    rejectCount;
  BTI_SINT    numberTerms;
} GNE_HEADER;
/*
typedef struct PACK_STRUCT
{
  BTI_CHAR    fieldType;
  BTI_SINT    fieldLen;
  BTI_SINT    fieldOffset;
  BTI_CHAR    comparisonCode;
  BTI_CHAR    connector;
  BTI_CHAR value[3];
} TERM_HEADER;
*/
typedef struct PACK_STRUCT
{
  BTI_SINT    maxRecsToRetrieve;
  BTI_SINT    noFieldsToRetrieve;
} RETRIEVAL_HEADER;

typedef struct PACK_STRUCT
{
  BTI_SINT    fieldLen;
  BTI_SINT    fieldOffset;
} FIELD_RETRIEVAL_HEADER;

typedef struct PACK_STRUCT
{
    BTI_SINT      recLen;
    BTI_LONG      recPos;
} RETURNED_HEADER;

/***************************************************************************
  Type definitions for Client ID and version Structures
****************************************************************************/
typedef struct PACK_STRUCT
{
  BTI_CHAR networkAndNode[12];
  BTI_CHAR applicationID[2];
  BTI_WORD threadID;
} CLIENT_ID;

typedef struct PACK_STRUCT
{
  BTI_SINT  Version;
  BTI_SINT  Revision;
  BTI_CHAR  MKDEId;
} VERSION_STRUCT;

#endif
