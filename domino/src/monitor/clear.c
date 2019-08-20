/*************************************************************************
**
**  Copyright 1982-2008 Pervasive Software Inc. All Rights Reserved
**
*************************************************************************/
/***************************************************************************
  mkdeinfo.c
    This is a simple sample designed to allow you to confirm your
    ability to compile, link, and execute a DTI application for
    your target environment using your compiler tools.
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dti.h>
#include <dolib.h>
//#include "../misc/send_error.h"

#define MAX_SEC_WAIT 10

static int log_func(const char *mgs, int level, void *user_data)
{
    fprintf(stderr, "%s\n", mgs);
    if ( level == LOG_ERR || level == LOG_CRIT ) {
		//send_error_message_width_host_info_printf(mgs);
        return FALSE;
    }
    return TRUE;
}
static void print_time()
{
    char *pos;
    //int sz;
    time_t t;
    t = time(NULL);
    pos = ctime(&t);
    //sz = strlen(pos);
    //pos[sz-1]=' ';
    fprintf(stderr, "%s", pos);
}

int main(int argc, char* argv[])
{

    BTI_SINT      rc;
    BTI_CHAR      szBuffer[512];
    BTI_ULONG     ulSize      = sizeof(szBuffer);
    BTI_LONG      lConnection = 0xFFFFFFFF;
    BTI_CHAR_PTR  uName = "";
    BTI_CHAR_PTR  pword = "";
    BTI_CHAR_PTR  svrName = "";
    //BTI_CHAR_PTR  svr = "";
    int transaction = TRUE;

    do_log_set_log_func(log_func, NULL);

    rc = PvStart(0);
    print_time();

    if ( rc!= P_OK ) {
        do_log(LOG_ERR,"Error on PvStart. Error code: %d", rc);
        return rc;
    }
    if ( argc > 3 ) {
        svrName = argv[1];
        uName = argv[2];
        pword = argv[3];
        if ( argc > 4 ) 
            transaction = strcmp(argv[4], "localonly");

    } else {
        if ( argv[1] != NULL ) {
            svrName = argv[1];
        } else {
            svrName = "localhost";
        }
    }
    rc = PvConnectServer(svrName,uName,pword,&lConnection);
    if ( rc != P_OK ) {
        do_log(LOG_ERR, "Error on PvConnectServer \"%s\". Error code: %d", svrName, rc);
        return rc;
    }
    do_log(LOG_INFO, "PvConnectServer OK.");

    rc = PvGetServerName(lConnection, &ulSize, szBuffer);
    if ( rc != P_OK ) {
        do_log(LOG_ERR, "Error on PvGetServerName \"%s\". Error code: %d", svrName, rc);
        return rc;
    }
    do_log(LOG_INFO, "PvGetServerName OK. ServerName: %s", szBuffer);
    unsigned long count = 0;
    // This sample disconnects all active Mkde connections
    //BTI_SINT status = 0;
    PVCLIENTID clientId;
    PVMKDECLIENTINFO pClientInfo;
    rc = PvGetMkdeClientsData(lConnection, (BTI_ULONG_PTR)&count);
    if ( rc!= P_OK ) {
        do_log(LOG_ERR, "Error on PvGetClientsData \"%s\". Error code: %d\n", svrName, rc);
        return rc;
    }
    while ( count > 0 ) {
        rc = PvGetMkdeClientId(lConnection, 0, &clientId);
        if ( rc!= P_OK ) {
            do_log(LOG_ERR, "Error on PvGetMkdeClientId \"%s\". Error code: %d\n", svrName, rc);
            return rc;
        }

        rc = PvGetMkdeClientInfo(lConnection, &clientId, &pClientInfo);
        if ( rc!= P_OK ) {
            do_log(LOG_ERR, "Error on PvGetMkdeClientInfo \"%s\". Error code: %d\n", svrName, rc);
            return rc;
        }
        if ( (!transaction && !strcmp(pClientInfo.netAddress, "Local")) || (transaction && pClientInfo.transState) ) {
            rc = PvDisconnectMkdeClient(lConnection, &clientId);
            if ( rc!= P_OK ) {
                do_log(LOG_ERR, "PvDisconnectMkdeClient \"%s\". Error code: %d\n", svrName, rc);
                return rc;
            }
            do_log(LOG_INFO, "disconnect client %s@%s", pClientInfo.userName, pClientInfo.netAddress);
        }
        rc = PvGetMkdeClientsData(lConnection, (BTI_ULONG_PTR)&count);
        if ( rc!= P_OK ) {
            do_log(LOG_ERR, "Error on PvGetClientsData \"%s\". Error code: %d\n", svrName, rc);
            return rc;
        }
    }
    PvFreeMkdeClientsData(lConnection);

    rc = PvStop(0);
    if ( rc!= P_OK ) {
        do_log(LOG_ERR, "Error on PvStop \"%s\". Error code: %d\n", svrName, rc);
        return rc;
    }
    do_log(LOG_INFO,"PvStop OK.\n");
    return rc;
}

