
/***************************************************************************
**
**  Copyright 2014 Actian Corporation All Rights Reserved
**
***************************************************************************/
/***************************************************************************
   ddf.h - DDF Catalog API
   This file contains function headers
   for functions exported through the DTI catalog API
***************************************************************************/

#ifndef _DDF_H_
#define _DDF_H_

#include "ddfstrct.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Don't pad our structures */
#pragma pack(push, 8)

//====================
// FUNCTION PROTOTYPES
//====================

////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvOpenDictionary
*
*Opens an existing dictionary.  This function should be called before any other.  
*Given an absolute path to the dictionary or data source names
*it returns a dictionary handle, 
*that will be used for any subsequent calls to any functions.  
*Note that multiple dictionaries can be opened at one time. 
*PvCloseDictionary should be called to free the resources.
*
*Parameters:
*	[in] path 					Absolute path to the dictionary files.
*	[in, out] dbHandle 		Handle to be used in subsequent calls
*	[in] user 					User name needed to open the dictionary, can be set to NULL
*	[in] password 			Used in conjuction with user name to open the dictionary files, can also be NULL
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errMemoryAllocation
*	PCM_errDictionaryNotFound
*	PCM_errDictionaryAlreadyOpen
*/
//////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvOpenDictionary
	(
	const BTI_CHAR* path, 
	BTI_WORD* dbHandle, 
	const BTI_CHAR* user, 
	const BTI_CHAR* password
	);

////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvCreateDictionary
*
*Given an absolute path to the dictionary it creates a new set of dictionary files, 
*and returns a handle to the newly create dictionary
*that will be used for any subsequent calls to any functions. 
*if the given directory does not exist and attempt will be made to 
*create the directory.  If the attempt fails PCM_errDirectoryError is returned. 
*PvCloseDictionary should be called to free the resources.
*
*Parameters:
*	[in] path 					Absolute path to the dictionary files.
*	[in, out] dbHandle 		Handle to be used in subsequent calls
*	[in] user 					User name needed to open the dictionary, can be set to NULL
*	[in] password 			Used in conjuction with user name to open the dictionary files, can also be NULL
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errMemoryAllocation
*	PCM_errPathNotFound
*	PCM_errDirectoryError
*
*Postcondition:
*	PvCloseDictionary needs to be called to free all resources.
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvCreateDictionary
	(
	const BTI_CHAR* path, 
	BTI_WORD* dbHandle,
	const BTI_CHAR* user, 
	const BTI_CHAR* password
	);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvCloseDictionary
*
*Given a handle of an opened dictionary closes the open dictionary.  
*Since multiple dictionaries can be open,
*this function needs to be called for every opened or newly created dictionary.
*
*Preconditions:
*	Dictionary had to be opened or created using PvOpenDictionary or PvCreateDictionary functions.
*	
*Parameters:
*	[in] dbHandle   handle of an open or newly created dictionary returned by PvOpenDictionary
*									or PvCreateDictionary.
*
*Return Value:
*	PCM_Success
*	PCM_errFailed
*	PCM_errMemoryAllocation
*	PCM_errDictionaryNotOpen
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvCloseDictionary
	(
	BTI_WORD dbHandle
	); 

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvGetTableNames
*
*Returns table names of all the tables in the open data dictionary.  
*Dictionary handle specifies which dictionary information is returned.
*
*Precondition:
*   Dictionary was successfully opened using PvOpenDatabase().
*
*Parameters:
*   [in] dbHandle         handle of an open dictionary returned by PvOpenDatabase
*   [out] tableList       array of TABLEMAP structures that contains table names
*   [in, out] tableCount  number of table names returned in tableList
*
*Return Values:
*   PCM_Success
*   PCM_errFailed
*   PCM_errMemoryAllocation
*   PCM_errInvalidDictionaryHandle
*
*Postcondition:
*   tableList array will need to be released using PvFreeTableNames
*   (see memory management)
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_PRE_V95_INTERFACE
#define PvGetTableNames PvGetTableNames2
#else
#define PvGetTableNames1 PvGetTableNames
#endif

#ifndef USE_PRE_V95_INTERFACE
PRESULT DDFAPICALLTYPE PvGetTableNames2
    (
    BTI_WORD dbHandle,  
    TABLEMAP2** tableList,
    BTI_WORD* tableCount
    );
#endif

PRESULT DDFAPICALLTYPE PvGetTableNames1
    (
    BTI_WORD dbHandle,  
    TABLEMAP1** tableList,
    BTI_WORD* tableCount
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvFreeTableNames
*
*Frees memory allocated in PvGetTableNames call
*
*Precondition:
*   Memory was succesfully allocated in PvGetTableNames call
*
*Parameters:	
*   [in, out] tableList		array of TABLEMAP structures
*
*Return Values:
*   PCM_Success
*   PCM_errFailed
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_PRE_V95_INTERFACE
#define PvFreeTableNames PvFreeTableNames2
#else
#define PvFreeTableNames1 PvFreeTableNames
#endif

#ifndef USE_PRE_V95_INTERFACE
PRESULT DDFAPICALLTYPE PvFreeTableNames2
    (
    TABLEMAP2* tableList	
    );
#endif

PRESULT DDFAPICALLTYPE PvFreeTableNames1
    (
    TABLEMAP1* tableList	
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvGetTable
*
*Returns table attributes for a given table.
*Dictionary handle specifies which dictionary contains the requested table information.
*
*Precondition:
*	Dictionary was successfully opened using PvOpenDictionary()
*
*Parameters:
*	[in] dbHandle  handle of an open dictionary returned by PvOpenDictionary
*	[in] tableName  number of table names returned in tableList
*	[out] tableProps		structure containing table information
*	[out] columnList	array of columns defined in the table
*	[out] columnCount number of columns in columnList
*	[out] indexList	array of segments defined in the table
*	[out] indexCount	number of segment in the indexList array
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errMemoryAllocation
*	PCM_errInvalidDictionaryHandle
*	PCM_errTableNotFound
*
*Postcondition:
*	tableProps, indexList and columnList will need to be released using PvFreeTable
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_PRE_V95_INTERFACE
#define PvGetTable PvGetTable3
#else
#define PvGetTable1 PvGetTable
#endif

#ifndef USE_PRE_V95_INTERFACE
PRESULT DDFAPICALLTYPE PvGetTable3
    (
    BTI_WORD dbHandle, 
    const BTI_CHAR* tableName, 
    TABLEINFO2** tableProps,
    COLUMNMAP3** columnList,
    BTI_WORD* columnCount,
    INDEXMAP2** indexList,
    BTI_WORD* indexCount
    );
#endif

PRESULT DDFAPICALLTYPE PvGetTable1
    (
    BTI_WORD dbHandle, 
    const BTI_CHAR* tableName, 
    TABLEINFO1** tableProps,
    COLUMNMAP1** columnList,
    BTI_WORD* columnCount,
    INDEXMAP1** indexList,
    BTI_WORD* indexCount
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvGetTableStat
*
*Returns table statistics for a given table.
*Dictionary handle specifies which dictionary contains the requested table information.
*
*Precondition:
*	Dictionary was successfully opened using PvOpenDictionary()
*
*Parameters:
*	[in] dbHandle  handle of an open dictionary returned by PvOpenDictionary
*	[in] ttableName  specifes the table name whose stat information is retrieved 
*	[out] tableStat		structure containing table statistics information
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errMemoryAllocation
*	PCM_errInvalidDictionaryHandle
*	PCM_errTableNotFound
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvGetTableStat
	(
	BTI_WORD dbHandle, 
	const BTI_CHAR* tableName, 
	TABLESTAT* tableStat
	); 

/*
*PvGetTableStat2
*
*Returns table statistics for a given table.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase()
*
*Parameters:
*	[in] dbHandle  handle of an open dictionary returned by PvOpenDatabase et al.
*	[in] ttableName  specifes the table name whose stat information is retrieved 
*	[out] tableStat		structure containing table statistics information (version 2)
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errMemoryAllocation
*	PCM_errInvalidDictionaryHandle
*	PCM_errTableNotFound
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvGetTableStat2
	(
	BTI_WORD dbHandle, 
	const BTI_CHAR* tableName, 
	TABLESTAT2* tableStat
	); 
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvFreeTable
*
*Frees memory allocated in PvGetTable call
*
*Precondition:
*	Memory was succesfully allocated in PvGetTable call
*
*Parameters:	
*	[in, out] tableProps		pointer to TABLEINFO structure
*	[in, out] columnList    pointer to an array of COLUMNMAP structures
*	[in, out] indexList       pointer to an array of INDEXMAP strucures
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_PRE_V95_INTERFACE
#define PvFreeTable PvFreeTable3
#else
#define PvFreeTable1 PvFreeTable
#endif

#ifndef USE_PRE_V95_INTERFACE
PRESULT DDFAPICALLTYPE PvFreeTable3
    (
    TABLEINFO2* tableProps,
    COLUMNMAP3* columnList,
    INDEXMAP2* indexList
    );
#endif

PRESULT DDFAPICALLTYPE PvFreeTable1
    (
    TABLEINFO1* tableProps,
    COLUMNMAP1* columnList,
    INDEXMAP1* indexList
    );


/*
 * PvOpenDatabase
 * PvOpenDatabase allows PCatalog to open databases by name rather than by path.
 *
 * Parameters:
 *
 * hConnection	[in]	Handle from PvConnectServer().
 * dbName	    	[in]	Name of database to open.
 * User		[in]	Database user name.
 * password	[in]	Password for user.
 * dbHandle	[out]	returned dataBase handle.
 *
 * Return Values
 *
 * P_OK				Successful                                  
 * P_E_INVALID_HANDLE		Invalid connection handle                   
 * P_E_NULL_PTR			Call with NULL pointer                      
 * P_E_ACCESS_RIGHT		Insufficient access right for the operation 
 * P_E_FAIL			Failed to open the database               
 * PCM_errSessionSecurityError	Invalid user name or password
 * 
*/
PRESULT DDFAPICALLTYPE PvOpenDatabase
    (
    BTI_LONG hConnection,
    const BTI_CHAR* dbName, 
    const BTI_CHAR* user, 
    const BTI_CHAR* password,
    BTI_WORD* dbHandle
    );
////////////////////////////////////////////////////////////////////////////////////////////////
//

/*
 * PvCloseDatabase
 * Closes an open database handle.
 *
 * Paramters:
 *
 * dbHandle	[in]		Handle of an open database 
 *
 * Return Values:
 * PCM_SUCCESS			Successful                                  
 * PCM_errFailed			Operation was not successful .                   
 * PCM_errMemoryAllocation		An error occurred during memory allocation.                      
 * PCM_errDictionaryNotOpen	Specified database was not open for a given dbHandle.
 * 
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvCloseDatabase
    (
    BTI_WORD dbHandle
    ); 
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvAddTable
*
*Creates a new table in the existing dictionary and a data file at the location specified in table properties.
*This function has to be provided with table information, columns and indexes.  
*indexCount and indexList are optional parameters because
*indexes are not required to create a table.  
*This function will fail if a table with the same name is already present in the specified dictionary.
*
*Precondition:
*	Dictionary was successfully opened using PvOpenDatabase().
*	Table Properties are set up correctly and an array of at least one column is passed in.
*
*Parameters:
*	[in] dbHandle  handle of an open dictionary returned by PvOpenDatabase
*	[in] tableProps  properties of the table to be created
*	[in] columnCount		number of columns in the following columnList array.
*	[in] columnList		array of column definitions
*	[in] indexCount  number of indexes in the following indexList array.
*	[in] indexList		array of index definitions
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidDictionaryHandle
*	PCM_errMemoryAllocation
*	PCM_errInvalidColumnName
*	PCM_errInvalidDataType
*	PCM_errDuplicateColumnName
*	PCM_errInvalidDataSize
*	PCM_errInvalidColumnOrder
*	PCM_errInvalidIndexName
*	PCM_errColumnNotFound
*
*Postcondition:
*	User will need to allocate and release COLUMNMAP and INDEXMAP arrays and TABLEINFO structure
*	used to describe the table
*	(see memory management)
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_PRE_V95_INTERFACE
#define PvAddTable PvAddTable3
#else
#define PvAddTable1 PvAddTable
#endif

#ifndef USE_PRE_V95_INTERFACE
PRESULT DDFAPICALLTYPE PvAddTable3
    (
    BTI_WORD dbHandle, 
    TABLEINFO2* tableProps, 
    COLUMNMAP3* columnList,
    BTI_WORD columnCount, 
    INDEXMAP2* indexList,
    BTI_WORD indexCount
    );
#endif

PRESULT DDFAPICALLTYPE PvAddTable1
    (
    BTI_WORD dbHandle, 
    TABLEINFO1* tableProps, 
    COLUMNMAP1* columnList,
    BTI_WORD columnCount, 
    INDEXMAP1* indexList,
    BTI_WORD indexCount
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvDropTable
*
*Drops the specified table from the open dictionary specified by dictionary handle.
*
*Precondition:
*	Dictionary was successfully opened using PvOpenDictionary().
*	A table specified by tableName exists in dictionary specified by dbHandle.
*
*Parameters:
*	[in] dbHandle  handle of an open dictionary returned by PvOpenDictionary
*	[in] tableName  name of the table where the columns will be created
*   [in] keepData  indicates whether the data file will be delete or not
                            if 0 the data file associated with the table will be deleted
                            if not 0 the data file not be deleted
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidDictionaryHandle
*	PCM_errTableNotFound
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvDropTable
	(
	BTI_WORD dbHandle, 
	const BTI_CHAR* tableName,
	BTI_WORD keepData
	);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvDropIndexByName
*
*Drops the index from the ddf file and the data file, given its name.
*
*Precondition:
*	Dictionary was successfully opened using PvOpenDictionary().
*	A table specified by tableName exists in dictionary specified by dbHandle.
*	index with the given name exists in the table.
*
*Parameters:
*	[in] dbHandle  handle of an open dictionary returned by PvOpenDictionary
*	[in] tableName	 specifes the table on which the operation will be performed
*	[in] indexName  name of the index name to be dropped
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidDictionaryHandle
*	PCM_errTableNotFound
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvDropIndexByName
	(
	BTI_WORD dbHandle, 
	const BTI_CHAR* tableName, 
	const BTI_CHAR* indexName
	);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvDropIndex
*
*Drops the index from the ddf file and the data file, given the index number.
*
*Precondition:
*	Dictionary was successfully opened using PvOpenDictionary().
*	A table specified by tableName exists in dictionary specified by dbHandle.
*	index with the given number exists in the table.
*
*Parameters:
*	[in] dbHandle  handle of an open dictionary returned by PvOpenDictionary
*	[in] tableName	 specifes the table on which the operation will be performed
*	[in] indexNimber  number of the index to be dropped.
*   [in] renumber indicates whether the remaining indexes should be renumbered.
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidDictionaryHandle
*	PCM_errTableNotFound
*	PCM_errInvalidIndex
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvDropIndex
	(
	BTI_WORD dbHandle, 
	const BTI_CHAR* tableName, 
	BTI_WORD indexNumber, 
	BTI_INT renumber
	);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvDropUser
*
*Drop an existing user from the database.
*This function will fail if a user with the same name is not already present in
*the specified database.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase() as user 'Master'.
*   And the associated database has database-level security enabled.
*
*Parameters:
*	[in] dbHandle  handle of an open database returned by PvOpenDatabase
*	[in] user      Database user name.
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidAccountName                     User does not exist
*	PCM_errNotAllowedToDropAdministrator          Attempt to Drop Master User
*	PCM_errDatabaseHasNoSecurity                  Database has no Security
*	PCM_errSessionSecurityError                   Database opened with insufficient privilege
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvDropUser
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* User
    );
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvRemoveUserFromGroup
*
*Remove an existing user from an existing group.  This function will fail if the 
*specified group or user do not already exist in the database.  This function 
*will fail if the user is not a member of the specified group.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase() as user 'Master'.  
*   And the associated database has database-level security enabled.  The user
*   and group already exist.  The user is a member of this group.
*
*Parameters:
*	[in] dbHandle  handle of an open database returned by PvOpenDatabase
*	[in] user      Database user name.
*	[in] group     Database group name.  
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidAccountName               User does not exist
*	PCM_errUserNotPartOfGroup               User not being part the group
*	PCM_errDatabaseHasNoSecurity            Database Has no Security
*	PCM_errSessionSecurityError             Database opened with insufficient privilege
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvRemoveUserFromGroup
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* user,
    const BTI_CHAR* group
    );
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvAddUserToGroup
*Add an existing user to an existing group.This function will fail if the 
*specified group or user do not already exist in the database.This function 
*will fail if the user is a member of another group.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase() as user 'Master'.  
*   And the associated database has database-level security enabled.  The user
*   and group already exist.  The user is not a member of another group.
*
*Parameters:
*	[in] dbHandle  handle of an open database returned by PvOpenDatabase
*	[in] user      Database user name.
*	[in] group     Database group name.  
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*   PCM_errInvalidAccountName               Invalid User Name
*	PCM_errUserAlreadyPartOfGroup           User already Part of the Group
*	PCM_errDatabaseHasNoSecurity            Database has no Security
*	PCM_errSessionSecurityError             Database opened with insufficient privilege
*	
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvAddUserToGroup
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* user,
    const BTI_CHAR* group
    );
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvCreateUser
*Creates a new user in the existing database.Optionally set a password for the new user 
*and assign him to an existing group.  This function will fail if a user with the same name 
*already exists in the specified database.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase() as user 'Master'.  
*   And the associated database has database-level security enabled.
*
*Parameters:
*	[in] dbHandle  handle of an open database returned by PvOpenDatabase
*	[in] user      Database user name.
*	[in] password  User password. If set to NULL, no password will be set.
*	[in] group     Database group name that user will be added to.  
*                  If set to NULL, user will not be assigned to a group.
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidAccountName               Invalid user name
*	PCM_errUserAlreadyExists                User already exists
*	PCM_errDatabaseHasNoSecurity            Database has no security
*	PCM_errSessionSecurityError             Database opened with insufficient privilege
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvCreateUser
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* User,
    const BTI_CHAR* Password,
    const BTI_CHAR* Group
    );
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvAlterUserName
*
*Alter an existing user's name.  This function will fail if 
*a user with the same name is not already present in the specified database.
*This function will fail if newName is set to NULL.  This
*function will fail if the newName is already present in the database.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase() as user 'Master'.
*   And the associated database has database-level security enabled.
*
*Parameters:
*	[in] dbHandle    Handle to an open database returned by PvOpenDatabase
*	[in] user        Database user name.
*	[in] newName     New database user name.
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*   PCM_errInvalidAccountName           Invalid user name
*	PCM_errUserAlreadyExists            New user name already exists
*	PCM_errDatabaseHasNoSecurity        Database has no security
*	PCM_errSessionSecurityError         Database opened with insufficient privilege
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvAlterUserName
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* User,
    const BTI_CHAR* newName
    );
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvAlterUserPassword
*
*Alter an existing user's password.  This function will fail if 
*a user with the same name is not already present in the specified database.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase() as user 'Master'.
*   And the associated database has database-level security enabled.
*
*Parameters:
*	[in] dbHandle    Handle to an open database returned by PvOpenDatabase
*	[in] user        Database user name.
*	[in] newPassword New user password.  If set to NULL, the password is cleared.
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidAccountName           Invalid user name
*	PCM_errDatabaseHasNoSecurity        Database has no security
*	PCM_errSessionSecurityError         Database opened with insufficient privilege
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvAlterUserPassword
    (
    BTI_WORD dbHandle, 
    const BTI_CHAR* User,
    const BTI_CHAR* Password
    );
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvCreateGroup
*
*Creates a new user group in the existing database.This function will fail 
*if a group with the same name already exists in the specified database.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase() as user 'Master'.  
*   And the associated database has database-level security enabled.
*
*Parameters:
*	[in] dbHandle  handle of an open database returned by PvOpenDatabase
*	[in] group     Database group name.  
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidAccountName           Invalid user name
*	PCM_errDatabaseHasNoSecurity        Database has no security
*	PCM_errSessionSecurityError         Database opened with insufficient privilege
*	PCM_errGroupAlreadyExists           Group already exists
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvCreateGroup
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* Group
    );
//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvDropGroup
*
*Drop an existing group.  This function will fail if the specified group does 
*not already exist in the database.This function will fail if the group is
*populated with members.
*
*Precondition:
*	Database was successfully opened using PvOpenDatabase() as user 'Master'.  
*   And the associated database has database-level security enabled.  The group
*   already exist.  The group has no members.
*
*Parameters:
*	[in] dbHandle  handle of an open database returned by PvOpenDatabase
*	[in] group     Database group name.  
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidAccountName           Invalid user name
*	PCM_errDatabaseHasNoSecurity        Database has no security
*	PCM_errSessionSecurityError         Database opened with insufficient privilege
*	PCM_errGroupNotEmpty                An user is associated with this group
*
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvDropGroup
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* Group
    );                          
/*
*PvAddIndex
*
*Adds indexes specified in indexList to the existing table and to the underlying data file.
*
*Precondition:
*	Dictionary was successfully opened using PvOpenDictionary() or created using PvCreateDictionary().
*	Specified table exists and the columns used to define index segments exist in the table.
*
*Parameters:
*	[in] dbHandle  handle of an open dictionary returned by PvOpenDictionary
*	[in] tableName	name of the table where the indexes will be added
*	[in] indexList		array of column definitions
*	[in] indexCount  number of indexes in the following indexList array.
*
*Return Values:
*	PCM_Success
*	PCM_errFailed
*	PCM_errInvalidDictionaryHandle
*	PCM_errTableNotFound
*	PCM_errMemoryAllocation
*	PCM_errInvalidColumnName
*	PCM_errInvalidIndexName
*	PCM_errColumnNotFound
*
*Postcondition:
*	User will need to allocate and release INDEXMAP array
*	used to describe the indexes
*	(see memory management module.)
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef USE_PRE_V95_INTERFACE
#define PvAddIndex PvAddIndex2
#else
#define PvAddIndex1 PvAddIndex
#endif

#ifndef USE_PRE_V95_INTERFACE
PRESULT DDFAPICALLTYPE PvAddIndex2
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* tableName,
    INDEXMAP2* indexList,
    BTI_WORD indexCount
    );
#endif

PRESULT DDFAPICALLTYPE PvAddIndex1
    (
    BTI_WORD dbHandle,
    const BTI_CHAR* tableName,
    INDEXMAP1* indexList,
    BTI_WORD indexCount
    );

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
*PvGetError
*
*Returns an error description string, describing last error.  The errorDesc string is allocated
*by the caller.  The maximum size of the error description is specified in ERROR_LEN. 
*
*Parameters:
*	[in, out]errorDesc  string that will contain the error description
*	[in, out]size				size of errorDesc.  If the size is not big enough to contain the
*										error description.  An error returned and the required size is specified in size.
*
*Return Values:
*	PCM_Success
*	PCM_errStringTooShort
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////
PRESULT DDFAPICALLTYPE PvGetError
	(
	BTI_CHAR* errorDesc, 
	BTI_WORD* size
	);


BTI_API DDFAPICALLTYPE PvCobolSchemaExec(
   BTI_WORD       dbHandle,
   BTI_CHAR_PTR   XMLString,
   BTI_LONG*      errorCode,
   BTI_CHAR_PTR*  errorMsg
);


#pragma pack(pop)

#ifdef __cplusplus
 }
#endif

#endif

