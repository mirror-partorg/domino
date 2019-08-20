
#ifndef DTISECURITY_H
#define DTISECURITY_H
/*************************************************************************
 **
 **  Copyright 2014 Actian Corporation All Rights Reserved
 **
 *************************************************************************/
/*************************************************************************
 *   SECURITY.H
 *
 *
 *   This header prototypes the database security functions
 *   of the PSQL Distributed Tuning Interface.
 *
 *     The following functions are found in this file:
 *
 *       PvSecureDatabase()
 *       PvUnSecureDatabase()
 *       PvIsDatabaseSecured()
 *
 *************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BTI_API PvSecureDatabase (BTI_LONG hConnection, BTI_CHAR_PTR dbName,
 *                           BTI_CHAR_PTR userName, BTI_CHAR_PTR userPwd)
 * Description: Secures the specified database with the given Master password
 *              The user name is not used currently.
 *
 * Returns: P_OK - on sucess,
 *          P_E_FAIL  if the call fails with a general error
 * Parameters:
 *              IN: hConnection - the connection of interest
 *                  dbName      - database name
 *                  userName    - user name (currently ignored)
 *                  userPwd     - password (Master password assumed)
 */
BTI_API PvSecureDatabase (BTI_LONG hConnection, BTI_CHAR_PTR dbName,
                          BTI_CHAR_PTR userName, BTI_CHAR_PTR userPwd);

/*
 * BTI_API PvUnSecureDatabase (BTI_LONG hConnection, BTI_CHAR_PTR dbName,
 *                             BTI_CHAR_PTR userName, BTI_CHAR_PTR userPwd)
 * Description: Unsecures the specified database with the given Master password
 *              The user name is not used currently.
 *
 * Returns: P_OK - on sucess,
 *          P_E_FAIL  if the call fails with a general error
 * Parameters:
 *              IN: hConnection - the connection of interest
 *                  dbName      - database name
 *                  userName    - user name (currently ignored)
 *                  userPwd     - password (Master password assumed)
 */
BTI_API PvUnSecureDatabase (BTI_LONG hConnection, BTI_CHAR_PTR dbName,
                            BTI_CHAR_PTR userName, BTI_CHAR_PTR userPwd);

/*
 * BTI_API PvIsDatabaseSecured (BTI_LONG hConnection, BTI_CHAR_PTR dbName,
 *                             BTI_ULONG_PTR secured)
 * Description: Checks if given database is secured or not.
 *
 * Returns: P_OK - on sucess,
 *          P_E_FAIL  if the call fails with a general error
 * Parameters:
 *              IN: hConnection - the connection of interest
 *                  dbName      - database name
 *              OUT: secured    - 1 if secured, 0 if not
 */
BTI_API PvIsDatabaseSecured (BTI_LONG hConnection, BTI_CHAR_PTR dbName,
                             BTI_ULONG_PTR secured);

#ifdef __cplusplus
}
#endif

#endif // DTISECURITY_H

