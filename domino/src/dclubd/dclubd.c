#include <getopt.h>
#include <signal.h>
#include <locale.h>

#include <dolib.h>
#include "version.h"
#include "option.h"
#include <domino.h>
#include "dclubinfo.h"
#include "dclub.h"
#include "../misc/mproc_state.h"

static option_t *opt = NULL;
static do_alias_t *alias = NULL;
static do_rpc_t *rpc = NULL;
static dclub_info_t *dclub_info = NULL;
static dclub_t *dclub = NULL;


const char *dclubd_name = NAME;
#ifdef _WIN32
#include <windows.h>
LPCTSTR dclubd_service_name = (LPCTSTR)NAME;
LPCTSTR dclubd_service_display = (LPCTSTR)"Dclub Server";
LPCTSTR dclubd_service_description = (LPCTSTR)"Dclub Server: Discount server for dclub pluggin of Profi-t by Pilot";
LPCTSTR dclubd_service_depends = NULL;
#endif


static int init()
{
    if (!option_read_config(opt))
        exit(1);

    if (!opt->run_foreground)
        do_log_set_type(dclubd_name, opt->log_type, opt->log_file);
    else
        do_log_set_stdout();

    if  (!domino_init(opt->domino_config_file, FALSE, DO_PARAM_END)) {
        exit(1);
    }

    if (!rpc)
            rpc = do_rpc_new(dclubd_name,
                         DCLUBD_RELEASE " (" ADATE ")",
                         opt->tcp_port, opt->telnet_tcp_port,
                         do_mproc_state_get(),
                         domino_key_file());
    if (alias)
        do_alias_free(alias);

    if ((alias = domino_alias_new(opt->alias_name)) == NULL)
        exit(1);

    if (dclub_info)
        dclub_info_free(dclub_info);
    dclub_info = dclub_info_new(opt);
    do_rpc_set_extended(rpc, dclub_info, &dclub_exec_command, dclub_info_get_cmd(), dclub_info_get_cmd_count());

    if (dclub)
        dclub_free(dclub);
    dclub = dclub_new(opt, dclub_info, alias);

    do_mproc_state_set_state(DO_STATE_RUN);
    return 1;
}

static int dclubd_main(int argc, char *argv[])
{
    init(argc, argv);
    while (do_mproc_state_get_state() != DO_STATE_STOP) {

        switch (do_mproc_state_get_state()) {
            case DO_STATE_RUN:
                if (!do_alias_conected(alias))
                   do_alias_open(alias, 0, DB_FOR_DCLUB);
                else
                   dclub_run(dclub);
                break;
            case DO_STATE_RESTARTING:
                init(argc, argv);
                break;
            case DO_STATE_PAUSING:
                do_mproc_state_set_state(DO_STATE_PAUSE);
                break;
            case DO_STATE_RESUMING:
                do_mproc_state_set_state(DO_STATE_RUN);
                break;
            case DO_STATE_STOPING:
                do_mproc_state_set_state(DO_STATE_STOP);
                break;
            default:
                break;
        }
        if (do_mproc_state_get_state() == DO_STATE_STOP)
            break;
        DO_SEC_SLEEP(1/10.);
    }
    dclub_free(dclub);
    do_alias_free(alias);
    option_free(opt);
    domino_finalize();
    dclub_info_free(dclub_info);
    do_rpc_free(rpc);
    return 1;
}

int main(int argc, char *argv[])
{
#ifdef MEMORY_DEBUG
    initmemdebug();
#endif
    setlocale(LC_ALL, "");

    do_log_set_type(dclubd_name, DO_LOG_TYPE_SYSLOG, "");
    do_mproc_state_new();
    opt = option_new();
    if (option_parse_options(opt, argc, argv))
       exit(1);
    if (opt->show_version) {
        printf("dclubd " DCLUBD_RELEASE " (" ADATE ") \n");
        exit(0);
    }
    if (opt->create_db) {
        do_log_set_stdout();
        if (!option_read_config(opt))
           exit(1);
        if ((alias = domino_alias_new(NULL)) == NULL)
           exit(1);
        if (!do_alias_discount_create(alias))
            exit(1);
        do_log(LOG_DEBUG, "Data file discount create succes");
        exit(0);
    }
    if (opt->only_check) {
        do_log_set_stdout();
        if (option_read_config(opt))
           dclub_check_config(opt);
            exit(1);
    }

    if (!opt->run_foreground) {
#ifdef _LINUX
        if (!option_read_config(opt))
               exit(1);
            do_daemon_start(argc, argv, dclubd_main, opt->user, opt->pid_file);
#elif defined (_WIN32)
        do_service_start(opt->service_install, dclubd_main, dclubd_service_name, dclubd_service_display,
                        dclubd_service_description, dclubd_service_depends);
#endif
    }
    else {
        do_log_set_stdout();
        dclubd_main(argc, argv);
    }
#ifdef MEMORY_DEBUG
    showmemdebug();
#endif
    return 0;
}
