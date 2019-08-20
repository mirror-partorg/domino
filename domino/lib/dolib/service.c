#include <stdio.h>
#include <dolib.h>
#include <errno.h>
#include <string.h>
#include "../misc/define.h"


#ifdef _LINUX
#include <fcntl.h>
#include <unistd.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>


DO_EXPORT int do_daemon_start(int argc, char** argv, do_main_handler_t *wrapper, char *username, char *pid_file)
{
   int i, fd;
   pid_t cpid, sid;
   mode_t oldmask;
   struct passwd *user = NULL;

   if (username)
	if((user = getpwnam(username)) == NULL) {
	    fprintf(stderr, "ERROR: Can't get information about user %s.\n", username);
	    return 0;
	}

   if ((cpid = fork()) < 0) {
      fprintf(stderr, "ERROR: Cannot fork to become daemon\n");
      return 0;
   }
   else if (cpid > 0) {
      //fprintf(stderr, "Parent close\n");
      return 0;
   }

   if (pid_file) {
      FILE *fp = fopen (pid_file, "w");
      if (!fp)
	    fprintf(stderr, "Open error on pid file %s\n", pid_file);
      else {
        unsigned int pid = getpid ();
        fprintf(fp, "%u\n", pid);
        if (fclose (fp))
    	    fprintf(stderr, "Close error on pid file %s\n", pid_file);

      }
   }


   if ((sid = setsid()) < 0) {
        fprintf(stderr, "ERROR: set sid\n");
        return 0;
   }

   if (user) {
        if(setgid(user->pw_gid)) {
             fprintf(stderr, "ERROR: setgid(%d) failed.\n", (int) user->pw_gid);
             return 0;
        }

        if(setuid(user->pw_uid)) {
             fprintf(stderr, "ERROR: setuid(%d) failed.\n", (int) user->pw_uid);
             return 0;
        }
   }

   closelog();

   for (i=0; i<sysconf(_SC_OPEN_MAX); i++) {
/*      if (opt->debug_level && (i == STDIN_FILENO ||
           i == STDOUT_FILENO || i == STDERR_FILENO))
         continue;*/
      close(i);
   }

   if (chdir("/"))
      fprintf(stderr, "ERROR: chdir %s\n", strerror(errno));

   oldmask = umask(002);
   oldmask |= 002;
   umask(oldmask);

   fd = open("/dev/null", O_RDONLY);
   if (fd > 2) {
      close(fd);
   } else {
      for (i = 1; fd + i <= 2; i++)
         dup2(fd, fd + i);
   }
   if (wrapper) wrapper(argc, argv);
   return 1;
}
#elif defined(_WIN32)
#include <dolib.h>
#include <windows.h>

static do_proc_state_t *state = NULL;

LPCTSTR service_name = NULL;
LPCTSTR service_display = NULL;
LPCTSTR service_description = NULL;
LPCTSTR service_depends = NULL;
SERVICE_STATUS service_status;
SERVICE_STATUS_HANDLE service_status_handle = 0;
do_main_handler_t *service_wrapper = NULL;

DO_EXPORT void do_service_set_proc_state(do_proc_state_t *proc)
{
    state = proc;
}

void WINAPI service_main(int argc, char *argv[]);

VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwCheckPoint, DWORD dwWaitHint)
{
    if (!service_status_handle)
        return;

    service_status.dwCurrentState = dwCurrentState;
    service_status.dwWaitHint = dwWaitHint;
    service_status.dwCheckPoint = dwCheckPoint;
    service_status.dwWaitHint   = dwWaitHint;

    if (!SetServiceStatus(service_status_handle, &service_status)) {
        switch (GetLastError()) {
            case ERROR_INVALID_DATA:
                do_log(LOG_ERR, "SetServiceStatus error. The specified service status structure is invalid.");
                break;
            case ERROR_INVALID_HANDLE:
                do_log(LOG_ERR, "SetServiceStatus error. The specified handle is invalid.");
                break;
            default:
                do_log(LOG_ERR, "SetServiceStatus error. Unknow error %d", GetLastError());
        }
    }
}
#ifdef DEBUG
static char *namestate[] = {"","SERVICE_CONTROL_STOP","SERVICE_CONTROL_PAUSE","SERVICE_CONTROL_CONTINUE",
"SERVICE_CONTROL_INTERROGATE","SERVICE_CONTROL_SHUTDOWN","SERVICE_CONTROL_PARAMCHANGE","SERVICE_CONTROL_NETBINDADD",
"SERVICE_CONTROL_NETBINDREMOVE","SERVICE_CONTROL_NETBINDENABLE","SERVICE_CONTROL_NETBINDDISABLE","SERVICE_CONTROL_DEVICEEVENT",
"SERVICE_CONTROL_HARDWAREPROFILECHANGE","SERVICE_CONTROL_POWEREVENT","SERVICE_CONTROL_SESSIONCHANGE"};
#endif
static void WINAPI service_ctrl_handler(DWORD Opcode)
{
    if (Opcode <= SERVICE_CONTROL_SESSIONCHANGE)
        do_log_debug(ZONE, "SCManager send signal %s", namestate[Opcode]);
    else
        do_log_debug(ZONE, "SCManager send signal %d", Opcode);
    switch(Opcode)
    {
        case SERVICE_CONTROL_PAUSE:
            if (state)
                do_proc_state_set_state(state, DO_STATE_PAUSING);
            return;
        case SERVICE_CONTROL_CONTINUE:
            if (state)
                do_proc_state_set_state(state, DO_STATE_RESUMING);
            return;
        case SERVICE_CONTROL_STOP:
            if (state)
                do_proc_state_set_state(state, DO_STATE_STOPING);
            return;
        case SERVICE_CONTROL_INTERROGATE:
            break;
    }
    return;
}

void WINAPI service_main(int argc, char *argv[])
{
    service_status.dwServiceType        = SERVICE_WIN32;
    service_status.dwCurrentState       = SERVICE_START_PENDING;
    service_status.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    service_status.dwWin32ExitCode      = 0;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwCheckPoint         = 0;
    service_status.dwWaitHint           = 0;

    service_status_handle = RegisterServiceCtrlHandler(service_name, service_ctrl_handler);
    if (service_status_handle == (SERVICE_STATUS_HANDLE)0) {
        switch (GetLastError()) {
            case ERROR_INVALID_NAME:
                do_log(LOG_ERR, "RegisterServiceCtrlHandler error. The specified service name is invalid.");
                break;
            case ERROR_SERVICE_DOES_NOT_EXIST:
                do_log(LOG_ERR, "RegisterServiceCtrlHandler error. The specified service does not exist.");
                break;
            default:
                do_log(LOG_ERR, "RegisterServiceCtrlHandler error. Unknow error %d", GetLastError());
        }
        return;
    }
    ReportSvcStatus(SERVICE_START_PENDING, 0, 0, 2000);
    if (service_wrapper) service_wrapper(argc, argv);
    ReportSvcStatus(SERVICE_STOPPED, 0, 0, 0);

    return;
}
#define SYSLOGDLL "syslogmes.dll"
BOOL install_event_message(const char *service_name)
{

    HKEY hk;
    DWORD dwData, dwDisp;
    TCHAR szPathName[MAX_PATH];
    TCHAR szBuf[MAX_PATH + 16];

    GetModuleFileName(NULL, szPathName, sizeof(szPathName));
    sprintf(szBuf, "\"%s\"", szPathName);
    //size_t cchSize = MAX_PATH;
    int i;

    sprintf(szBuf, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", service_name);

    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szBuf, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hk, &dwDisp)) {
        MessageBox(NULL, "Could not create the registry key", "Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    strcpy(szBuf, szPathName);

    for ( i = lstrlen(szBuf); i > 0; i--)
      if (szBuf[i] == '\\') {
        szBuf[i + 1] = 0;
        break;
      }
    strcat(szBuf, SYSLOGDLL);

    if (RegSetValueEx(hk, "EventMessageFile", 0, REG_EXPAND_SZ,  (LPBYTE) szBuf,
          (DWORD) (lstrlen(szBuf)+1)*sizeof(TCHAR)))  {
        MessageBox(NULL, "Could not set the event message file.", "Error", MB_OK | MB_ICONERROR);
        RegCloseKey(hk);
        return FALSE;
    }


    dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE;

    if (RegSetValueEx(hk, "TypesSupported", 0, REG_DWORD, (LPBYTE) &dwData, sizeof(DWORD)))  {
        MessageBox(NULL, "Could not set the supported types.", "Error", MB_OK | MB_ICONERROR);
        RegCloseKey(hk);
        return FALSE;
    }

    RegCloseKey(hk);
    return TRUE;
}

BOOL install_service()
{

    TCHAR szPathName[MAX_PATH];
    TCHAR szBuf[MAX_PATH + 16];
    HANDLE schSCManager, schService;
    SERVICE_DESCRIPTION sdServiceDescription = { (LPTSTR)service_description };

    GetModuleFileName(NULL, szPathName, sizeof(szPathName));
    sprintf(szBuf, "\"%s\"", szPathName);

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);


    if (schSCManager == NULL) {
        switch (GetLastError()) {
            case ERROR_ACCESS_DENIED:
                MessageBox(NULL, "OpenSCManager error. The requested access was denied.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_DATABASE_DOES_NOT_EXIST:
                MessageBox(NULL, "OpenSCManager error. The specified database does not exist.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_PARAMETER:
                MessageBox(NULL, "OpenSCManager error. A specified parameter is invalid.", "Error", MB_OK | MB_ICONERROR);
                break;
            default:
                sprintf(szBuf, "OpenSCManager error. Unknow error %d", (int)GetLastError());
                MessageBox(NULL, szBuf, "Error", MB_OK | MB_ICONERROR);
        }
        return FALSE;
    }
    schService = CreateService(schSCManager,
        service_name,       // service name (alias)
        service_display,    // service name to display
        SERVICE_ALL_ACCESS,        // desired access
        SERVICE_WIN32_OWN_PROCESS, // service type
        SERVICE_AUTO_START,        // start type
        SERVICE_ERROR_NORMAL,      // error control type
        szBuf,                     // service's binary
        NULL,                      // no load ordering group
        NULL,                      // no tag identifier
        service_depends,    // dependencies
        NULL,                      // LocalSystem account
        NULL);                     // no password

    if (schService == NULL) {
        switch (GetLastError()) {
            case ERROR_ACCESS_DENIED:
                MessageBox(NULL, "The handle to the SCM database does not have the SC_MANAGER_CREATE_SERVICE access right.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_CIRCULAR_DEPENDENCY:
                MessageBox(NULL, "A circular service dependency was specified.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_DUPLICATE_SERVICE_NAME:
                MessageBox(NULL, "The display name already exists in the service control manager database either as a service name or as another display name.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_HANDLE:
                MessageBox(NULL, "The handle to the specified service control manager database is invalid.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_NAME:
                MessageBox(NULL, "The specified service name is invalid.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_PARAMETER:
                MessageBox(NULL, "CreateService error. A parameter that was specified is invalid.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_SERVICE_ACCOUNT:
                MessageBox(NULL, "The user account name specified in the lpServiceStartName parameter does not exist.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_SERVICE_EXISTS:
                MessageBox(NULL, "The specified service already exists in this database.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_SERVICE_MARKED_FOR_DELETE:
                MessageBox(NULL, "The specified service has already been marked for deletion.", "Error", MB_OK | MB_ICONERROR);
                break;
            default:
                sprintf(szBuf, "CreateService error. Unknow error %d", (int)GetLastError());
		MessageBox(NULL, szBuf, "Error", MB_OK | MB_ICONERROR);
        }
        return FALSE;
    }
    if (!ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, (LPVOID)&sdServiceDescription)) {
        sprintf(szBuf, "ChangeServiceConfig error %d", (int)GetLastError());
        MessageBox(NULL, szBuf, "Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    CloseServiceHandle(schService);
    return install_event_message(service_name);

}
BOOL delete_service()
{
    HANDLE schSCManager;
    SC_HANDLE hService;
    char buf[1024];

    schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);

    if (schSCManager == NULL) {
        switch (GetLastError()) {
            case ERROR_ACCESS_DENIED:
                MessageBox(NULL, " OpenSCManager error. The requested access was denied.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_DATABASE_DOES_NOT_EXIST:
                MessageBox(NULL, " OpenSCManager error. The specified database does not exist.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_PARAMETER:
                MessageBox(NULL, " OpenSCManager error. A specified parameter is invalid.", "Error", MB_OK | MB_ICONERROR);
                break;
            default:
                sprintf(buf, " OpenSCManager error. Unknow error %d", (int)GetLastError());
                MessageBox(NULL, buf, "Error", MB_OK | MB_ICONERROR);

        }
        return FALSE;
    }

    hService=OpenService(schSCManager, service_name, SERVICE_ALL_ACCESS);

    if (hService == NULL) {
        switch (GetLastError()) {
            case ERROR_ACCESS_DENIED:
                MessageBox(NULL, " The handle does not have access to the service.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_HANDLE:
                MessageBox(NULL, " CreateService error. The specified handle is invalid.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_NAME:
                MessageBox(NULL, " The specified service name is invalid.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_SERVICE_DOES_NOT_EXIST:
                MessageBox(NULL, " The specified service does not exist.", "Error", MB_OK | MB_ICONERROR);
                break;
            default :
                MessageBox(NULL, " OpenService error. Unknow error", "Error", MB_OK | MB_ICONERROR);
        }
        return FALSE;
    }

    if(DeleteService(hService)==0) {
        switch (GetLastError()) {
            case ERROR_ACCESS_DENIED:
                MessageBox(NULL, " The handle does not have the DELETE access right.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_HANDLE:
                MessageBox(NULL, " The specified handle is invalid.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_SERVICE_MARKED_FOR_DELETE:
                MessageBox(NULL, " The specified service has already been marked for deletion.", "Error", MB_OK | MB_ICONERROR);
                break;
            default:
                sprintf(buf, " DeleteService error. Unknow error %d", (int)GetLastError());
                MessageBox(NULL, buf, "Error", MB_OK | MB_ICONERROR);
        }
        return FALSE;
    }
    if(CloseServiceHandle(hService)==0)
        return FALSE;

    TCHAR szBuf[MAX_PATH + 16];
    //size_t cchSize = MAX_PATH;
    sprintf(szBuf, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s", service_name);
    if (RegDeleteKey(HKEY_LOCAL_MACHINE, szBuf)) {
        MessageBox(NULL, "Could not delete the registry key", "Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    return TRUE;
}

DO_EXPORT int do_service_start(int install, do_main_handler_t *wrapper, LPCTSTR name, LPCTSTR display, LPCTSTR description, LPCTSTR depends)
{
    service_wrapper = wrapper;
    service_name = name;
    service_display = display;
    service_description = description;
    service_depends = depends;
    char buf[1024];

    if (install == 1)    {
        // service installation requested
        if(install_service()) {
            sprintf(buf, " Service %s installed sucessfully.", service_name);
            MessageBox(NULL, buf, "Information", MB_OK | MB_ICONINFORMATION);
	}
//         else {
//             sprintf(buf, " Error installing service %s.", service_name);
//             MessageBox(NULL, buf, "Information", MB_OK | MB_ICONERROR );
//         }
        return 0;
    }
    if (install == -1)    {
        // service removal requested
        if(delete_service()) {
            sprintf(buf, " Service %s uninstalled sucessfully.", service_name);
            MessageBox(NULL, buf, "Information", MB_OK | MB_ICONINFORMATION);
	}
//         else {
//             sprintf(buf, " Error uninstalling service %s.", service_name);
//             MessageBox(NULL, buf, "Information", MB_OK | MB_ICONERROR );
//         }
        return 0;
    }
    do_log(LOG_DEBUG, "Starting as a service.");

    SERVICE_TABLE_ENTRY DispatchTable[] = {{(LPTSTR)service_name, (LPSERVICE_MAIN_FUNCTIONA)service_main}, {NULL, NULL}};
    TCHAR szPathName[MAX_PATH]; LPTSTR slash = NULL;
    GetModuleFileName(NULL, szPathName, sizeof(szPathName));
    // Set working directory to the service path
    if((slash = strrchr(szPathName, '\\')) != '\0') {
        *slash = 0;
        SetCurrentDirectory(szPathName);
    }
    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        switch (GetLastError()) {
            case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
                MessageBox(NULL, " Program is being run as a console application rather than as a service.", "Error", MB_OK | MB_ICONERROR);
                break;
            case ERROR_INVALID_DATA:
                do_log(LOG_ERR, "StartServiceCtrlDispatcher error. Invalid data");
                break;
            case ERROR_SERVICE_ALREADY_RUNNING:
                do_log(LOG_ERR, "StartServiceCtrlDispatcher error. Service already running");
                break;
            default :
                do_log(LOG_ERR, "StartServiceCtrlDispatcher error. Unknow error");
        }
    }
    do_log(LOG_DEBUG, "Start ok");
    return 0;
}

#endif
