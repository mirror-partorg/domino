#ifndef _DTICONST_H_INCLUDED
/*************************************************************************
**
**  Copyright 2014 Actian Corporation All Rights Reserved
**
*************************************************************************/
/*************************************************************************
*   DTICONST.H
*
*
*   This header establishes defined constants for status codes that may
*   be returned by functions prototyped in connect.h, config.h, monitor.h,
*   and catalog.h, of the PSQL Distributed Tuning Interface.
*
*************************************************************************/

#define     P_OK                    0
#define     P_E_INVALID_HANDLE      7001        // Invalid connection handle
#define     P_E_NULL_PTR            7002        // Function is called with NULL pointer
#define     P_E_BUFFER_TOO_SMALL    7003        // The array size is too small
#define     P_E_FAIL                7004        // General failure code 
#define     P_E_INVALID_DATA_TYPE   7005        // The setting requested is of different type
#define     P_E_OUT_OF_RANGE        7006        // The setting value to be set is out of range
#define     P_E_INVALID_SELECTION   7007        // At least one selection item is invalid
#define     P_E_INVALID_SEQUENCE    7008        // Sequence number is not valid
#define     P_E_DATA_UNAVAILABLE    7009        // Data related to open files not available
#define     P_E_INVALID_CLIENT      7010        // Invalid client ID

#define     P_E_ACCESS_RIGHT        7011	    // Insufficient access right for the operation
#define     P_E_DUPLICATE_NAME      7012	    // Named database already exists on the server
#define     P_E_NOT_EXIST           7013	    // Named database does not exist

#define	    P_E_NOT_INITED          7014	    // PvStart(0) not called. No DTI session has been started.

#define     P_E_FILE_NOT_OPEN       7015        // The specified file is not currently open

#define     P_E_DICTIONARY_ALREADY_EXISTS       7016    // DDF files already exist at the dictionary path
#define     P_E_SHARED_DDF_EXIST                7017    // The dictionary path is being used by another database

// DSN Errors
#define     P_E_INVALID_NAME        7018        // Invalid DSN name. DSN name cannot be longer than 20 characters and cannot contain invalid characters such as []{}()?*=!@,;
#define     P_E_DSN_ALREADY_EXIST   7019        // The specified DSN already exists
#define     P_E_DSN_DOES_NOT_EXIST  7020        // The specified DSN does not exist
#define     P_E_INVALID_OPEN_MODE   7021        // Invalid open mode for the DSN

#define	    P_E_COMPONENT_NOT_LOADED    7022	// Component not loaded
#define		P_E_DICT_IN_USE				7023	// Data dictionary is in use.
#define     P_E_INVALID_DATABASE_TYPE   7024    // security policy must be one of the above:
                                                // PSQL_DB_POLICY_CLASSIC, 
                                                // PSQL_DB_POLICY_MIXED, or PSQL_DB_POLICY_DBSEC
                                                // the error returned if the security is not one
                                                // of the above
// PvConnectServer Error codes
#define P_E_SERVER_NOT_FOUND					7025
#define P_E_REQUESTR_NOT_LOADED					7026
#define P_E_SERVER_TABLE_FULL					7027
#define P_E_CLIENT_CONNECTIONS_LIMIT_REACHED	7028
#define P_E_PERMISSION_ERROR					7029
#define P_E_NO_MEMORY						    7030
#define P_E_NO_AVAILABLE_TRANSPORT				7031
#define P_E_CONNECTION_FAILURE					7032
#define P_E_CONNECTION_LOST				    	7033



#define     P_E_DB_NAME_TOO_LONG		7034	// Database Name too long
#define     P_E_NOT_ENOUGH_DATAPATHS	7035	// Number of datapaths specified are less than that of source database
#define		P_E_DUPLICATE_FILENAME		7036	// The MKD file being copied already exists at that location
#define		P_E_INVALID_PASSWORD		7037	// Password for the source database is invalid
#define     P_E_INVALID_DESTINATION_DATAPATH 7038 //Invalid destination datapath
#define     P_E_INVALID_DICTIONARY_PATH 7039    //Invalid destination Dictionary path
#define     P_E_INVALID_FILTER_OPTION   7040    // Invalid filtering option
#define     P_E_ABSOLUTE_PATH_IN_DDF    7041
#define     P_E_BOUND_DATABASE_DDF_CREATE 7042 

#define     P_E_INVALID_CHAR_SENT       7043
#define     P_E_INVALID_CHAR_RCVD       7044
#define     P_E_INTERNAL_BUFFER_TOO_SMALL 7045

#define     P_E_INVALID_TRANSLATE_OPTION  7046
#define     P_E_INVALID_CODEPAGE        7047


//License Manager Errors
#define LICENSE_MGR_BASE                  (7000 + 100)

// no more licenses of this type is available
#define P_E_LIC_MAX_USERS                 7063

// 7064: No license obtained for the product
#define P_E_NO_LICENSE_OBTAINED           7064

// 7065: A product has not been obtained
#define P_E_NO_PRODUCT_OBTAINED           7065

#define P_E_LIC_INVALID_CHARACTER         (LICENSE_MGR_BASE + 1)

// Illegal license type is defined
#define P_E_LIC_TYPE_ILLEGAL              (LICENSE_MGR_BASE + 2)

// License key length is too long
#define P_E_LIC_KEY_LENGTH                (LICENSE_MGR_BASE + 8)

// no such a license exists
// trying to delete the license that has not been created
#define P_E_LIC_NOT_FOUND                 (LICENSE_MGR_BASE + 9)

// There is at least one temporary license that has already expired
// there is no other than temporary licenses installed on the system
#define P_E_LIC_EXPIRED                   (LICENSE_MGR_BASE + 10)

// the license is a time bomb and cannot be removed
// Temporary licenses cannot be removed.  The license becomes invalid after its expiration date.
#define P_E_LIC_TIMEBOMB                  (LICENSE_MGR_BASE + 11)

// the same license has already been installed
// License is already installed.
#define P_E_LIC_ALREADY_INSTALLED         (LICENSE_MGR_BASE + 12)

// the provided license is not valid
#define P_E_LIC_INVALID                   (LICENSE_MGR_BASE + 13)

// invalid product id
#define P_E_LIC_E_INVALID_PRODUCT         (LICENSE_MGR_BASE + 15)

// The database engine is not running.  Unable to show, apply, or remove a license.
// Verify that the database engine is running and that network communications are functioning.
#define P_E_SERVER_NOT_RUNNING            (LICENSE_MGR_BASE + 18)

// The local database engine is not running.  Unable to show, apply, or remove a license.
#define P_E_LOCAL_SERVER_NOT_RUNNING      (LICENSE_MGR_BASE + 19)

// The license is not removable.
#define P_E_LICENSE_NOT_REMOVABLE         (LICENSE_MGR_BASE + 20)

// Your trial license has expired.  No other license is available
// PSQL V8 cannot be installed without a valid license.
#define P_E_LIC_NO_ACTIVE_LICENSE         (LICENSE_MGR_BASE + 22)

// A User Count Increase cannot be applied without a permanent license.
#define P_E_LIC_NO_PERM_LIC               (LICENSE_MGR_BASE + 24)

// Cannot apply a permanent license when one already exists.
#define P_E_LIC_ALREADY_EXISTS            (LICENSE_MGR_BASE + 25)

// Cannot increase the user count beyond the maximum allowed by the product licensing.
#define P_E_LIC_MAX_COUNT_VIOLATION       (LICENSE_MGR_BASE + 26)

// License product version does not match the engine.
#define P_E_LIC_PSQL_VERSION_MISMATCH     (LICENSE_MGR_BASE + 30)

// License bitness or OS does not match the engine.
#define P_E_LIC_INCOMPATIBLE_PLATFORM     (LICENSE_MGR_BASE + 31)

// License product engine type does not match the engine.
#define P_E_LIC_PSQL_ENGINE_MISMATCH      (LICENSE_MGR_BASE + 32)

// A UCI of the license being deleted cannot be deauthed
#define P_E_LIC_UCI_CANNOT_BE_DEAUTHED    (LICENSE_MGR_BASE + 33)

#define _DTICONST_H_INCLUDED
#endif // #ifndef _DTICONST_H_INCLUDED
