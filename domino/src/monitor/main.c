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
#include "../misc/send_error.h"

#define MAX_SEC_WAIT 10

static int log_func(const char *mgs, int level, void *user_data)
{
    fprintf(stderr, "%s\n", mgs);
    if ( level == LOG_ERR || level == LOG_CRIT ) {
		send_error_message_width_host_info_printf(mgs);
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
    BTI_CHAR_PTR  svr = "";

    PVMKDEUSAGE usageinfo;
    int count;
    int ret = 0;

    do_log_set_log_func(log_func, NULL);
    print_time();

    rc = PvStart(0);

    if ( rc!= P_OK ) {
        do_log(LOG_ERR,"Error on PvStart. Error code: %d", rc);
        return rc;
    }
    if ( argc > 2 ) {
        svrName = argv[1];
        uName = argv[2];
        pword = argv[3];
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
        //return rc;
    }
    else
        do_log(LOG_INFO, "PvConnectServer OK.");

    if ( rc == P_OK ) {
	    rc = PvGetServerName(lConnection, &ulSize, szBuffer);
	    if ( rc != P_OK ) {
            do_log(LOG_ERR, "Error on PvGetServerName \"%s\". Error code: %d", svrName, rc);
	    }
	    else
            do_log(LOG_INFO, "PvGetServerName OK. ServerName: %s", szBuffer);
    }
    if ( rc == P_OK ) {
        rc = PvGetMkdeUsage(lConnection, &usageinfo);
        if ( rc != P_OK ) {
            do_log(LOG_ERR, "Error on PvGetMkdeUsage \"%s\". Error code: %d", svrName, rc);
        }
    if ( rc == P_OK ) {
        if ( usageinfo.maxLicenses <= usageinfo.currentLicensesInUse ) {
           do_log(LOG_ERR, "Количество использованных лицензий %d из %d на \"%s\"", usageinfo.currentLicensesInUse, usageinfo.maxLicenses, svrName);
        }
        do_log(LOG_INFO, "Current licenses       = %d",   usageinfo.currentLicensesInUse);
        do_log(LOG_INFO, "Peak licenses          = %d",   usageinfo.peakLicensesInUse);
        do_log(LOG_INFO, "Max licenses           = %d\n",  usageinfo.maxLicenses);
        do_log(LOG_INFO, "Current files          = %d",   usageinfo.curFilesInUse);
        do_log(LOG_INFO, "Peak files             = %d",   usageinfo.peakFilesInUse);
        do_log(LOG_INFO, "Max files              = %d\n", usageinfo.maxFiles);

        do_log(LOG_INFO, "Current handles        = %d",   usageinfo.curHandlesInUse);
        do_log(LOG_INFO, "Peak handles           = %d",   usageinfo.peakHandlesInUse);
        do_log(LOG_INFO, "Max handles            = %d\n", usageinfo.maxHandles);

        count = MAX_SEC_WAIT;
        while ( usageinfo.curTransInUse > 1 && count > 0 ) {
           fprintf(stderr, "Количество транзакций %d. Ожидание...\n", usageinfo.curTransInUse);
           DO_SEC_SLEEP(1);
           count--;
        }
        if ( usageinfo.curTransInUse > 1 ) {
           do_log(LOG_ERR, "Количество транзакций %d на \"%s\"", usageinfo.curTransInUse, svrName);
           ret = 1;
        }
        do_log(LOG_INFO, "Current transactions   = %d",   usageinfo.curTransInUse);
        do_log(LOG_INFO, "Peak transactions      = %d",   usageinfo.peakTransInUse);
        do_log(LOG_INFO, "Max transactions       = %d\n", usageinfo.maxTrans);

        do_log(LOG_INFO, "Current clients        = %d",   usageinfo.curClients);
        do_log(LOG_INFO, "Peak clients           = %d",   usageinfo.peakClients);
        do_log(LOG_INFO, "Max clients            = %d\n", usageinfo.maxClients);

        do_log(LOG_INFO, "Current threads        = %d",   usageinfo.curThreads);
        do_log(LOG_INFO, "Peak threads           = %d",   usageinfo.peakThreads);
        do_log(LOG_INFO, "Max threads            = %d\n", usageinfo.maxThreads);

        count = MAX_SEC_WAIT;
        while ( usageinfo.curLocksInUse > 1 && count > 0 ) {
           DO_SEC_SLEEP(1);
           count--;
        }
        if ( usageinfo.curLocksInUse > 1 ) {
           do_log(LOG_ERR, "Количество блокировок %d на \"%s\"", usageinfo.curLocksInUse, svrName);
           ret = 2;
        }
        do_log(LOG_INFO, "Current locks          = %d\n",   usageinfo.curLocksInUse);
        do_log(LOG_INFO, "Peak locks             = %d\n\n", usageinfo.peakLocksInUse);
        }
    }
    rc = PvStop(0);
    if ( rc!= P_OK ) {
        do_log(LOG_ERR, "Error on PvStop \"%s\". Error code: %d\n", svrName, rc);
        return rc;
    }
    do_log(LOG_INFO,"PvStop OK.\n");
    return ret || (rc != P_OK ? 115 : 0);
}

