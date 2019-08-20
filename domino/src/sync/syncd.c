#include <getopt.h>
#include <signal.h>
#include <locale.h>

#include <domino.h>
#include "version.h"
#include "option.h"
#include "sync_info.h"
#include "sync_server.h"
#include "../misc/mproc_state.h"

static option_t *opt = NULL;
static do_alias_t *alias = NULL;
static do_rpc_t *rpc = NULL;

//!!#ifndef USE_MINIMAL
static sync_info_t *sync_info = NULL;
static sync_server_t *sync_server = NULL;
//!!#endif

const char *syncd_name = NAME;
#ifdef _WIN32
#include <windows.h>
LPCTSTR syncd_service_name = (LPCTSTR)NAME;
LPCTSTR syncd_service_display = (LPCTSTR)"Sync Server";
LPCTSTR syncd_service_description = (LPCTSTR)"Sync Server: for domino exchange";
LPCTSTR syncd_service_depends = NULL;
#endif


#ifdef _LINUX
static void sig_stop(int signo) {
    do_log(LOG_INFO, "stoping sigal");
    do_mproc_state_set_state(DO_STATE_STOPING);
}

static void install_signal_handlers(void) {
    sigset_t sig_set;


    sigemptyset(&sig_set);

    sigaddset(&sig_set, SIGCHLD);
    sigaddset(&sig_set, SIGINT);
    sigaddset(&sig_set, SIGQUIT);
    sigaddset(&sig_set, SIGILL);
    sigaddset(&sig_set, SIGABRT);
    sigaddset(&sig_set, SIGFPE);
    sigaddset(&sig_set, SIGSEGV);
    sigaddset(&sig_set, SIGALRM);
    sigaddset(&sig_set, SIGTERM);
    sigaddset(&sig_set, SIGHUP);
    sigaddset(&sig_set, SIGUSR2);
#ifdef SIGSTKFLT
    sigaddset(&sig_set, SIGSTKFLT);
#endif /* SIGSTKFLT */
#ifdef SIGIO
    sigaddset(&sig_set, SIGIO);
#endif /* SIGIO */
#ifdef SIGBUS
    sigaddset(&sig_set, SIGBUS);
#endif /* SIGBUS */

  signal(SIGHUP, sig_stop);
  signal(SIGUSR1, sig_stop);
  signal(SIGTERM, sig_stop);

  sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
}
#endif

static int init()
{
    if (!option_read_config(opt))
        exit(1);
    if (!opt->run_foreground)
        do_log_set_type(syncd_name, opt->log_type, opt->log_file);
    else
        do_log_set_stdout();

//!!#ifndef USE_MINIMAL

    if  (!domino_init(opt->domino_config_file, FALSE, DO_PARAM_END))
        exit(1);

//#endif

    if (!rpc)
        rpc = do_rpc_new(syncd_name, SYNCD_RELEASE " (" ADATE ")",
                         opt->tcp_port, opt->telnet_tcp_port,
                         do_mproc_state_get(),
                         domino_key_file());
    if (alias)
        do_alias_free(alias);

    if (opt->client_host)
        if ((alias = domino_alias_new(opt->alias_name)) == NULL)
            exit(1);

//!!#ifndef USE_MINIMAL
    if (sync_info)
        sync_info_free(sync_info);
    sync_info = sync_info_new(opt);

    do_rpc_set_extended(rpc, sync_info, &sync_exec_command, sync_info_get_cmd(), sync_info_get_cmd_count());

    if (sync_server)
        sync_server_free(sync_server);

    if (opt->client_host)
        sync_server = sync_server_new(opt, sync_info, alias);
//!!#endif
    do_mproc_state_set_state(DO_STATE_RUN);
    return 1;
}

static int syncd_main(int argc, char *argv[])
{
    init(argc, argv);
    int sleep_run=0;
    while (do_mproc_state_get_state() != DO_STATE_STOP) {

        switch (do_mproc_state_get_state()) {
            case DO_STATE_RUN:
//!!#ifndef USE_MINIMAL
                if ( opt->handbook_replic1 || opt->stock_replic1 || opt->document_replic1 ) {
                    if ( !do_alias_conected(alias) ) {
                       do_alias_open(alias, 1);
                       if (do_alias_conected(alias)) {
                           protocol_rec_t protocol;
                           do_protocol_step(alias, &protocol, DO_STEP_LAST);
                       }
                    }
                    else
                    {
                        if ( !sleep_run ) {
                            sync_server_run(sync_server);
                            sleep_run = 20;
                        }
                        else
                            sleep_run--;
            	    }
                }
//!!#endif
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
        DO_SEC_SLEEP(1/10.);//!!
    }
//!!#ifndef USE_MINIMAL
    sync_server_free(sync_server);
    if (opt->client_host)
        do_alias_free(alias);
//!!#endif
    option_free(opt);
    domino_finalize();
//!!#ifndef USE_MINIMAL
    sync_info_free(sync_info);
//!!#endif
    do_rpc_free(rpc);
    return 1;
}

int main(int argc, char *argv[])
{
#ifdef MEMORY_DEBUG
    initmemdebug();
#endif
    setlocale(LC_ALL, "");

    do_log_set_type(syncd_name, DO_LOG_TYPE_SYSLOG, "");
    do_mproc_state_new();

#ifdef _LINUX
    install_signal_handlers();
#endif
#ifdef DEBUG_BTI
    domino_set_debug_btr(TRUE);
#endif
    opt = option_new();
    if ( option_parse_options(opt, argc, argv) )
       exit(1);
    if (opt->show_version) {
        printf("syncd " SYNCD_RELEASE " (" ADATE ") \n");
        exit(0);
    }
    if (opt->only_check) {
        do_log_set_stdout();
        if (option_read_config(opt))

//!!#ifndef USE_MINIMAL
           sync_server_check_config(opt);
//!!#else
//!!            ;
//!!#endif

            exit(1);
    }
    //opt->run_foreground = 1;
    if (!opt->run_foreground) {
#ifdef _LINUX
        if (!option_read_config(opt))
               exit(1);
            do_daemon_start(argc, argv, syncd_main, opt->user, opt->pid_file);
#elif defined (_WIN32)
        do_service_start(opt->service_install, syncd_main, syncd_service_name, syncd_service_display,
                        syncd_service_description, syncd_service_depends);
#endif
    }
    else {
        do_log_set_stdout();
        syncd_main(argc, argv);
    }
#ifdef MEMORY_DEBUG
    showmemdebug();
#endif
    return 0;
}
