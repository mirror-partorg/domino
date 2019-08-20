
#ifndef DTILICENSE_H
#define DTILICENSE_H
/*************************************************************************
 **
 **  Copyright 2014 Actian Corporation All Rights Reserved
 **
 *************************************************************************/
/*************************************************************************
 *   DTILICENSE.H
 *
 *
 *   This header prototypes the License Manager functions
 *   of the PSQL Distributed Tuning Interface.
 *
 *     The following functions are found in this file:
 *
 *       PvDeleteLicense()
 *       PvAddLicense()
 *
 *************************************************************************/

#include "dticonst.h"

// the size for dti buffer where the size of str buffer is stored
#define CHAR_SIZE_BUFFER_SIZE 2

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BTI_API PvDeleteLicense (BTI_LONG  hConnection, BTI_CHAR_PTR licenses)
 * Description: removes the corresponding license from the computer specified with
 *              the connection.
 *
 * Returns: P_OK - on sucess,
 *          P_E_FAIL  if the call fails with a general error
 *          PS_E_LIC_NOT_FOUND - the license is not installed
 *          PS_E_LIC_INVALID - the license is invalid
 * Parameters:
 *              IN: hConnection - the connection of interest
 *                  license - the license key to be removed
 */
BTI_API PvDeleteLicense (BTI_LONG  hConnection, BTI_CHAR_PTR license);

/*
 * BTI_API PvAddLicense (BTI_LONG  hConnection, BTI_CHAR_PTR licenses)
 * Description: installes the specified license from the computer specified with
 *              the connection.
 *
 * Returns: P_OK - on sucess,
 *          P_E_FAIL  if the call fails with a general error
 *          PS_E_LIC_ALREADY_INSTALLED - the license is installed
 *          PS_E_LIC_INVALID - the license is invalid
 * Parameters:
 *              IN: hConnection - the connection of interest
 *                  license - the license key to be removed
 */
BTI_API PvAddLicense (BTI_LONG  hConnection, BTI_CHAR_PTR license);

// Product codes for licenses returned by PvGetProductsInfo().
// Note that PSQL v2000 and older are not in this list because
// those licenses are in a different format
#define PV_PC_AUDITMASTER_V6                  50 /* AuditMaster v6 */
#define PV_PC_AUDITMASTER_V7                  51 /* AuditMaster v7 */
#define PV_PC_AUDITMASTER_V8                  52 /* AuditMaster v8 */
#define PV_PC_AUDITMASTER_V12                 53 /* AuditMaster v12 */

#define PV_PC_DX_SERVER_DATA_SYNC_V2          70 /* DataExchange Server: Data Synchronization v2 */
#define PV_PC_DX_SERVER_DATA_SYNC_V3          71 /* DataExchange Server: Data Synchronization v3 */
#define PV_PC_DX_SERVER_DATA_SYNC_V4          72 /* DataExchange Server: Data Synchronization v4 */
#define PV_PC_DX_SERVER_DATA_SYNC_V5          73 /* DataExchange Server: Data Synchronization v5 */
#define PV_PC_DX_SERVER_DATA_SYNC_V12         74 /* DataExchange Server: Data Synchronization v12 */

#define PV_PC_DX_SERVER_RT_BACKUP_V2          75 /* DataExchange Server: Real-Time Backup v2 */
#define PV_PC_DX_SERVER_RT_BACKUP_V3          76 /* DataExchange Server: Real-Time Backup v3 */
#define PV_PC_DX_SERVER_RT_BACKUP_V4          77 /* DataExchange Server: Real-Time Backup v4 */
#define PV_PC_DX_SERVER_RT_BACKUP_V5          78 /* DataExchange Server: Real-Time Backup v5 */
#define PV_PC_DX_SERVER_RT_BACKUP_V12         79 /* DataExchange Server: Real-Time Backup v12 */

#define PV_PC_DX_WGE_DATA_SYNC_V2             80 /* DataExchange Workgroup: Data Synchronization v2 */
#define PV_PC_DX_WGE_DATA_SYNC_V3             81 /* DataExchange Workgroup: Data Synchronization v3 */
#define PV_PC_DX_WGE_DATA_SYNC_V4             82 /* DataExchange Workgroup: Data Synchronization v4 */
#define PV_PC_DX_WGE_DATA_SYNC_V5             83 /* DataExchange Workgroup: Data Synchronization v5 */
#define PV_PC_DX_WGE_DATA_SYNC_V12            84 /* DataExchange Workgroup: Data Synchronization v12 */

#define PV_PC_DX_WGE_RT_BACKUP_V2             85 /* DataExchange Workgroup: Real-Time Backup v2 */
#define PV_PC_DX_WGE_RT_BACKUP_V3             86 /* DataExchange Workgroup: Real-Time Backup v3 */
#define PV_PC_DX_WGE_RT_BACKUP_V4             87 /* DataExchange Workgroup: Real-Time Backup v4 */
#define PV_PC_DX_WGE_RT_BACKUP_V5             88 /* DataExchange Workgroup: Real-Time Backup v5 */
#define PV_PC_DX_WGE_RT_BACKUP_V12            89 /* DataExchange Workgroup: Real-Time Backup v12 */

#define PV_PC_BACKUPAGENT_V1                  100 /* Backup Agent v1 */
#define PV_PC_BACKUPAGENT_V2                  110 /* Backup Agent v2 */
#define PV_PC_BACKUPAGENT_V3                  111 /* Backup Agent v3 */
#define PV_PC_BACKUPAGENT_V12                 112 /* Backup Agent v12 */

#define PV_PC_PSQL_SERVER_V8                  11
#define PV_PC_PSQL_WORKGROUP_V8               101
#define PV_PC_PSQL_SERVER_V9                  300 
#define PV_PC_PSQL_WORKGROUP_V9               301
#define PV_PC_PSQL_SERVER_V10                 325
#define PV_PC_PSQL_WORKGROUP_V10              326
#define PV_PC_PSQL_SERVER_V11                 425
#define PV_PC_PSQL_WORKGROUP_V11              426
#define PV_PC_PSQL_VX_SERVER_V11              427
#define PV_PC_PSQL_SERVER_V12                 525 /* Version 12.0 - Q4 2014 */
#define PV_PC_PSQL_WORKGROUP_V12              526 /* Version 12.0 - Q4 2014 */
#define PV_PC_PSQL_VX_SERVER_V12              527 /* Version 12.0 - Q4 2014 */
#define PV_PC_PSQL_BTRIEVE_V12                528 /* Version 12.1 - Q3 2015 */

/*
 * BTI_API PvGetProductsInfo (BTI_LONG hConnection, BTI_CHAR_PTR productInfo, BTI_ULONG_PTR pBufSize)
 * Description: get xml string with info on all PSQL products license mgr knows about.
 * See the PV_PC_* defines above for possible values in the productcode XML element.
 *
 * Returns: DBU_SUCCESS - on sucess,
 *          P_E_FAIL  if the call fails with a general error
 * Parameters:
 *              IN: hConnection - the connection of interest
 *              IN, OUT: pBufSize - the size of the buffer and the size of the buffer needed
 *              OUT: productInfo - the xml string with all products
 *
 */
BTI_API PvGetProductsInfo (BTI_LONG  hConnection, BTI_CHAR_PTR productInfo, BTI_ULONG_PTR pBufSize);

/*
 * BTI_API PvValidateLicenses (BTI_LONG hConnection)
 * Description: initiate a license audit on the computer indicated by the connection.
 *
 * Returns: PS_OK - on success, otherwise failure.
 *
 * Parameters:
 *              IN: hConnection - the connection of interest
 */
BTI_API PvValidateLicenses (BTI_LONG  hConnection);

/*
 * BTI_API PvRegisterOEMLicMsgUTF8 (BTI_LONG hConnection, BTI_INT oemID, BTI_INT appID, BTI_CHAR_PTR pMessage)
 * Description: registers an OEM-specific license message
 *
 * Returns: PS_OK - on sucess
 *          P_E_OUT_OF_RANGE - OEM or app ID is invalid.
 *          P_E_NULL_PTR - message is null
 *          P_E_FAIL  if the call fails with a general error
 * Parameters:
 *          IN: hConnection - the connection of interest
 *          IN: oemID - OEM ID to which the message applies. Cannot be 0 or 2 which are reserved.
 *          IN: appID - App ID to which the message applies, or -1 if none
 *          IN: pMessage - the UTF8 message string to set.
 */
BTI_API PvRegisterOEMLicMsgUTF8(BTI_LONG hConnection, BTI_INT oemID, BTI_INT appID, BTI_CHAR_PTR pMessage);

/*
 * BTI_API PvRegisterOEMForLicEventsUTF8 (BTI_INT oemID, BTI_INT appID, BTI_INT eventID, BTI_CHAR_PTR pCommand)
 * Description:  Registers an OEM for different license events - failed (101), 
 *               disabled (102), active (103)
 *
 * Returns: PS_OK            - on success
 *          P_E_OUT_OF_RANGE - OEM, app, or event ID is invalid
 *          P_E_NULL_PTR     - message is null
 *          P_E_FAIL         - if the call fails with a general error
 *
 * Parameters:
 *          IN: oemID       - OEM ID to which the message applies. Cannot be 0 or 2 which are reserved.
 *          IN: appID       - application ID to which the message applies or -1 for all oemID apps
 *          IN: eventID     - event ID to which the command applies (101, 102, or 103)
 *          IN: pCommand    - the UTF8 command string to run when license event occurs
 */
BTI_API PvRegisterOEMForLicEventsUTF8(BTI_INT oemID, BTI_INT appID, BTI_INT eventID, BTI_CHAR_PTR pCommand);

#ifdef __cplusplus
}
#endif

#endif // DTILICENSE_H

