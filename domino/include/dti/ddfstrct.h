
/***************************************************************************
**
**  Copyright 2014 Actian Corporation All Rights Reserved
**
***************************************************************************/
/***************************************************************************
   ddfstrct.h - DDF Catalog data structures
   This file contains data structures for catalog API
***************************************************************************/

#ifndef _DDFSTRCT_H_
#define _DDFSTRCT_H_

#include "btitypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Don't pad our structures */
#pragma pack(push, 8)

//====================
//DATA STRUCTURES
//====================

#define DATABASENAME_LEN    20

#define TABLENAME_LEN_V1    20
#define COLUMNNAME_LEN_V1   20
#define INDEXNAME_LEN_V1    20
#define PATH_LEN_V1         64

#define TABLENAME_LEN_V2    248
#define COLUMNNAME_LEN_V2   248
#define INDEXNAME_LEN_V2    248
#define PATH_LEN_V2         250

#ifndef USE_PRE_V95_INTERFACE
#define TABLENAME_LEN       TABLENAME_LEN_V2
#define COLUMNAME_LEN       COLUMNNAME_LEN_V2
#define INDEXNAME_LEN       INDEXNAME_LEN_V2
#define PATH_LEN            PATH_LEN_V2
#else
#define TABLENAME_LEN       TABLENAME_LEN_V1
#define COLUMNAME_LEN       COLUMNNAME_LEN_V1
#define INDEXNAME_LEN       INDEXNAME_LEN_V1
#define PATH_LEN            PATH_LEN_V1
#endif

#define ERROR_LEN  255
#define ISR_LEN 16
#define FREESPACE_THRESHOLD 5

//error codes
typedef enum 
{
   PCM_Success  = 0,
   PCM_errFailed  = 1,
   PCM_errMemoryAllocation  = 2,
   PCM_errDatabaseNotFound  = 3,
#define PCM_errDictionaryNotFound PCM_errDatabaseNotFound
   PCM_errDatabaseAlreadyOpen  = 4,
#define PCM_errDictionaryAlreadyOpen PCM_errDatabaseAlreadyOpen
   PCM_errDatabaseNotOpen  = 5,
#define PCM_errDictionaryNotOpen PCM_errDatabaseNotOpen
   PCM_errInvalidDatabaseHandle = 6,
#define PCM_errInvalidDictionaryHandle  PCM_errInvalidDatabaseHandle
   PCM_errTableNotFound  = 7,
   PCM_errInvalidTableName  = 8,
   PCM_errInvalidColumnName  = 9,
   PCM_errInvalidDataType  = 10 ,
   PCM_errDuplicateColumnName  = 11,
   PCM_errInvalidDataSize  = 12,
   PCM_errInvalidColumnOrder  = 13,
   PCM_errInvalidIndexName  = 14,
   PCM_errColumnNotFound  = 15,
   PCM_errTooManySegments  = 16,
   PCM_errStringTooShort = 17,
   PCM_errDictionaryAlreadyExists = 18,
   PCM_errDirectoryError = 19,
   PCM_errSessionSecurityError = 20,
   PCM_errDuplicateTable = 21,
   PCM_errDuplicateIndex = 22,
   PCM_errISRNotSupported = 23,
   PCM_errCouldNotLoadCobolSchemaExec = 24,  /*Could not load cobolschemaexec dll*/
   PCM_errInvalidIndex = 25,
   PCM_errFirstSegmentNumberMustBeZero = 26,
   PCM_errInvalidNameLength = 27,
   PCM_errInternalProtocolError = 28,
   PCM_errInvalidAccountName = 29,
   PCM_errUserAlreadyExists = 30,
   PCM_errGroupNotEmpty = 31,
   PCM_errGroupAlreadyExists = 32,
   PCM_errUserAlreadyPartOfGroup = 33,
   PCM_errUserNotPartOfGroup = 34,
   PCM_errNotAllowedToDropAdministrator = 35,
   PCM_errDatabaseHasNoSecurity = 36,
   PCM_errInvalidPassword = 37,  //New error code for Invalid Password
   PCM_SuccessWithInfo = 38,
   PCM_errLast         // mark the end of the errors. 

}
PRESULT;

//TABLEINFO flag value can take the following values or their combination:
#define B_FLAG_TRUE_NULLABLE 64 // table is true nullable.
//When the table is created a one byte null indicator is added before each column that is nullable.

//COLUMNINFO flag value can take the following values or their combination:
#define B_FLAG_CASE_SENSITIVE  1 // Case Sensitive - column values are case sensitive on comparisons and as part of index segments 
#define B_FLAG_NULLABLE  4 // Nullable - if the table is created as true nullable then a one byte null indicator column is added before the column value to indicate whether the column value is null.
#define B_FLAG_NTEXT 2048 // Wide-character data type -- causes B_TYPE_BLOB to be treated as wide-char, rather than character data
#define B_FLAG_BINARY	4096   //Binary data type -- causes types B_TYPE_BLOB or B_TYPE_STRING to be treated as binary, rather than character data

//COLUMNINFO dataType can take the following values:
#define B_TYPE_STRING 0
#define B_TYPE_INTEGER 1
#define B_TYPE_FLOAT 2
#define B_TYPE_DATE 3
#define B_TYPE_TIME 4
#define B_TYPE_DECIMAL 5
#define B_TYPE_MONEY 6
#define B_TYPE_LOGICAL 7
#define B_TYPE_NUMERIC 8
#define B_TYPE_BFLOAT 9
#define B_TYPE_LSTRING 10
#define B_TYPE_ZSTRING 11
#define B_TYPE_NOTE 12
#define B_TYPE_LVAR 13
#define B_TYPE_BINARY 14
#define B_TYPE_AUTOINC 15
#define B_TYPE_BIT 16
#define B_TYPE_NUMERSTS 17
#define B_TYPE_NUMERSA 18
#define B_TYPE_CURRENCY 19
#define B_TYPE_TIMESTAMP 20
#define B_TYPE_BLOB 21
#define B_TYPE_GDECIMAL 22
#define B_TYPE_WSTRING 25
#define B_TYPE_WZSTRING 26
#define B_TYPE_DATETIME 30
#define B_TYPE_GUID     27

//INDEXINFO flags can be a combination of these values
#define B_FLAG_DUPLICATES 1 // Duplicates Allowed
#define B_FLAG_MODIFIABLE 2 // Modifiable
#define B_FLAG_SORT_DESCENDING 64 // Sort Descending
#define B_FLAG_PARTIAL    512     // Partial Index


//structure used to pass table names
typedef struct TABLEMAP2tag
{
    char tableName[TABLENAME_LEN_V2 + 1];
} TABLEMAP2;

typedef struct TABLEMAP1tag
{
    char tableName[TABLENAME_LEN_V1 + 1];
} TABLEMAP1;

#ifndef USE_PRE_V95_INTERFACE
#define TABLEMAP TABLEMAP2
#else
#define TABLEMAP TABLEMAP1
#endif

//structure used to pass table properties
typedef struct TABLEINFO2tag
{
    char        tableName [TABLENAME_LEN_V2+1]; // dictionary tableName
    char        dataLocation [PATH_LEN_V2+1];   // actual [path/name]of data file
    BTI_WORD    flags;                          // flags of the table
    BTI_INT     overwrite;                      // indicates whether data file will be overwritten if one already exists.
} TABLEINFO2;

typedef struct TABLEINFO1tag
{
    char        tableName [TABLENAME_LEN_V1+1]; // dictionary tableName
    char        dataLocation [PATH_LEN_V1+1];   // actual [path/name]of data file
    BTI_WORD    flags;                          // flags of the table
    BTI_INT     overwrite;                      // indicates whether data file will be overwritten if one already exists.
} TABLEINFO1;

#ifndef USE_PRE_V95_INTERFACE
#define TABLEINFO TABLEINFO2
#else
#define TABLEINFO TABLEINFO1
#endif

//structure used to pass column information
typedef struct COLUMNMAP3tag
{
    BTI_WORD  index;                        //ordinal of the column position
    char      name[COLUMNNAME_LEN_V2 + 1];  //column name
    BTI_WORD  dataType;                     //data type
    BTI_WORD  size;                         //field length                                             
    BTI_WORD  fieldOffset;                  //offset within the record
    BTI_WORD  decimal;                      //decimal places
    BTI_WORD  flags;                        //field flags
    char      isrName[ISR_LEN + 1];         //international sorting rule name
} COLUMNMAP3;

typedef struct COLUMNMAP2tag
{
    BTI_WORD  index;                        //ordinal of the column position
    char      name[COLUMNNAME_LEN_V2 + 1];  //column name
    BTI_WORD  dataType;                     //data type
    BTI_WORD  size;                         //field length
    BTI_WORD  decimal;                      //decimal places
    BTI_WORD  flags;                        //field flags
    char      isrName[ISR_LEN + 1];         //international sorting rule name
} COLUMNMAP2;

typedef struct COLUMNMAP1tag
{
    BTI_WORD  index;                        //ordinal of the column position
    char      name[COLUMNNAME_LEN_V1 + 1];  //column name
    BTI_WORD  dataType;                     //data type
    BTI_WORD  size;                         //field length
    BTI_WORD  decimal;                      //decimal places
    BTI_WORD  flags;                        //field flags
    char      isrName[ISR_LEN + 1];         //international sorting rule name
} COLUMNMAP1;

#ifndef USE_PRE_V95_INTERFACE
#define COLUMNMAP COLUMNMAP3
#else
#define COLUMNMAP COLUMNMAP1
#endif

// Structure used to pass table stat info
// This is deprecated with P.SQL 9.40 release.  Use TABLESTAT2
// with the PvGetTableStat2 API.
typedef struct TABLESTATtag
{
   char tableName [TABLENAME_LEN_V1 + 1];
   char tableLocation [PATH_LEN_V1 + 1];
   char dictionaryPath [PATH_LEN_V1 + 1];
   float fileVersion;
   BTI_WORD recordLength;
   BTI_WORD pageSize;
   BTI_WORD numberOfRecords;
   BTI_WORD numberOfIndexes;
   BTI_WORD numberOfDuplicatePtrs;
   BTI_WORD numberOfUnusedPages;
   unsigned char variableRecords;
   unsigned char varRecBlankTruncation;
   unsigned char recordCompression;
   unsigned char keyOnlyFile;
   unsigned char indexBalancing;
   char freespaceThreshold[FREESPACE_THRESHOLD];
   unsigned char usesAlternateCollatingSeq;
   unsigned char systemData;
} TABLESTAT;

// TBD. These need to be reconciled with v2 metadata support.
#define TABLESTAT_TABLENAME_LEN     256
#define TABLESTAT_LOCATION_PATH_LEN 256

// Structure used with the PvGetTableStat2 API.
typedef struct TABLESTAT2tag
{
   // Name of the table.
   BTI_CHAR tableName [TABLESTAT_TABLENAME_LEN];
   // Path of the physical file.
   BTI_CHAR tableLocation [TABLESTAT_LOCATION_PATH_LEN];
   // Dictionary path.
   BTI_CHAR dictionaryPath [TABLESTAT_LOCATION_PATH_LEN];
   // Record Length.
   BTI_WORD recordLength;
   // Logical page size of the physical file.
   BTI_WORD pageSize;
   // Number of keys in the file.
   BTI_WORD numberOfIndexes;
   // Available slots for linked duplicate keys.
   BTI_WORD numberOfDuplicatePtrs;
   // Number of unused pre-allocated pages.
   BTI_WORD numberOfUnusedPages;
   // Free space threshold (percentage). Applicable for files with variable
   // length records.
   BTI_INT  freespaceThreshold;
   // Number of records in the file.
   BTI_LONG numberOfRecords;
   // File format. Same value as returned by Btrieve STAT operation.
   // 0x95 = 9.5 file format
   BTI_BYTE fileVersion;

   // The following bytes mainly represent some of the file flags that the 
   // Btrieve STAT operation returns. Value 0 means the attribute is absent,
   // Value 1 means the attribute is present.

   // Whether file has variable length records.
   BTI_BYTE variableRecords;
   // Whether file has trailing blank truncated in variable length records.
   BTI_BYTE varRecBlankTruncation;
   // Whether file has compressed records.
   BTI_BYTE recordCompression;
   // Whether file is a key only file or not.
   BTI_BYTE keyOnlyFile;
   // Whether file has index balancing turned on.
   BTI_BYTE indexBalancing;
   // Whether file uses an ACS or not. This is not a STAT file flag.
   BTI_BYTE usesAlternateCollatingSeq;
   // Whether file has system data or not.
   BTI_BYTE systemData;
   // Whether file has compressed pages or not.
   BTI_BYTE pageCompression;
   // Reserved for future use.
   BTI_BYTE reserved[8];
} TABLESTAT2;

//structure used to pass index information
typedef struct INDEXMAP2tag
{
    BTI_WORD    index;                             // Btrieve index number
    BTI_WORD    segment;                           // segment index number
    char        columnName[COLUMNNAME_LEN_V2 + 1]; // index into associated field
    char        indexName[INDEXNAME_LEN_V2 + 1];   // index name
    BTI_WORD    flags;                             // index attributes
} INDEXMAP2;

typedef struct INDEXMAP1tag
{
    BTI_WORD    index;                             // Btrieve index number
    BTI_WORD    segment;                           // segment index number
    char        columnName[COLUMNNAME_LEN_V1 + 1]; // index into associated field
    char        indexName[INDEXNAME_LEN_V1 + 1];   // index name
    BTI_WORD    flags;                             // index attributes
} INDEXMAP1;

#ifndef USE_PRE_V95_INTERFACE
#define INDEXMAP INDEXMAP2
#else
#define INDEXMAP INDEXMAP1
#endif

/*
===========================================
Memory Management
=============================================
The following rules apply:

-For [in] only parameters, the client (caller) is responsible 
 for allocating and freeing the memory.

-For [out] only parameters, the server (callee) is responsible 
 for allocating the memory, and the client (caller) 
 is responsible for freeing the memory, but in the case of this component
 the server also provides the deallocation functions
 (see PvFreeTable, PvFreeTableNames)

-For [in, out] parameters, the client (caller) allocates the memory 
 and is also responsible for freeing the memory.  
 However, the component (callee) has the option of 
 reallocating memory if it needs to.

==============================================*/


#pragma pack(pop)

#ifdef __cplusplus
 }
#endif

#endif // _DDFSTRCT_H_

