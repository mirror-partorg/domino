#include <getopt.h>
#include <signal.h>
#include <locale.h>

#include <dolib.h>
#include "../../config/version.h"
#include "option.h"
#include <domino.h>
#include <artix.h>
#include "exchangeinfo.h"
#include "exchange.h"
#include "../misc/mproc_state.h"
//#include "../misc/artix_send.h"

static option_t *opt = NULL;
static do_alias_t *alias = NULL;
static do_rpc_t *rpc = NULL;
static exchange_info_t *exchange_info = NULL;
static exchange_t *exchange = NULL;

const char *exchanged_name = NAME;
#ifdef _WIN32
#include <windows.h>
LPCTSTR exchanged_service_name = (LPCTSTR)NAME;
LPCTSTR exchanged_service_display = (LPCTSTR)"Exchange Server";
LPCTSTR exchanged_service_description = (LPCTSTR)"Exchange Server: exchange for Artix";
LPCTSTR exchanged_service_depends = NULL;
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


static void init_local()
{
    opt->nretail_price = domino_nretail_price(alias, domino_unit());
}
static int init()
{
    if (!option_read_config(opt))
        exit(1);


    if (!opt->run_foreground)
        do_log_set_type(exchanged_name, opt->log_type, opt->log_file);
    else
        do_log_set_stdout();

    if  (!domino_init(opt->domino_config_file, FALSE, DO_PARAM_END))
        exit(1);

    if (!rpc)
        rpc = do_rpc_new(exchanged_name,
                         VERSION " (" ADATE ")",
                         opt->tcp_port, opt->telnet_tcp_port,
                         do_mproc_state_get(),
                         domino_key_file());
    if (alias)
        do_alias_free(alias);

    if ((alias = domino_alias_new(NULL)) == NULL)
        exit(1);

    if (exchange_info)
        exchange_info_free(exchange_info);
    exchange_info = exchange_info_new(opt);
    do_rpc_set_extended(rpc, exchange_info, &exchange_exec_command, exchange_info_get_cmd(), exchange_info_get_cmd_count());

    if (exchange)
        exchange_free(exchange);
    exchange = exchange_new(opt, exchange_info, alias);
    do_proc_state_set_state(do_mproc_state_get(), DO_STATE_RUN);
    return 1;
}

static int exchanged_main(int argc, char *argv[])
{
    init(argc, argv);
    int full_update = 0;
    while (do_mproc_state_get_state() != DO_STATE_STOP) {

        switch (do_mproc_state_get_state()) {
            case DO_STATE_RUN:
                if (!do_alias_conected(alias)) {
                   if (!do_alias_open(alias, 0, DB_FOR_EXCHANGE))
                       exchange_send_error(exchange, "База данных недоступна");
                   else {
                       init_local();
                       full_update = 1;
                   }
                }
                if (do_alias_conected(alias)) {
                   exchange_run(exchange, full_update);
                   full_update = 0;
                }
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
        DO_SEC_SLEEP(1);
    }
    exchange_free(exchange);
    do_alias_free(alias);
    option_free(opt);
    domino_finalize();
    exchange_info_free(exchange_info);
    do_rpc_free(rpc);
    return 1;
}
//static int load_only(option_t *opt);

int main(int argc, char *argv[])
{
#ifdef MEMORY_DEBUG
    initmemdebug();
#endif
#ifdef _LINUX
    install_signal_handlers();
#endif
    setlocale(LC_ALL, "");
    do_mproc_state_new();

    do_log_set_type(exchanged_name, DO_LOG_TYPE_SYSLOG, NULL);

    opt = option_new();
    if (option_parse_options(opt, argc, argv))
       exit(1);
    if (opt->show_version) {
        printf("exchanged " VERSION " (" ADATE ") \n");
        exit(0);
    }
/*    if ( opt->load_only ) {
        if ( !option_read_config(opt) )
            exit(1);
        do_log_set_stdout();
        return !load_only(opt);
    }
*/
    if (!opt->run_foreground) {
#ifdef _LINUX
        if (!option_read_config(opt))
               exit(1);
            do_daemon_start(argc, argv, exchanged_main, NULL, opt->pid_file);
#elif defined (_WIN32)
        do_service_start(opt->service_install, exchanged_main, exchanged_service_name, exchanged_service_display,
                        exchanged_service_description, exchanged_service_depends);
#endif
    }
    else {
        do_log_set_stdout();
        exchanged_main(argc, argv);
    }
#ifdef MEMORY_DEBUG
    showmemdebug();
#endif
    do_mproc_state_free();
    return 0;
}

static void proc_persent(do_proc_state_t *proc_state, void *arg)
{
    int proc;
    proc = do_proc_state_get_persent(proc_state);
    if ( proc )
       printf("%d%%\n", proc);
}
/*
static int load_only(option_t *opt)
{
    int res;
    artix_t *artix;
    do_alias_t *alias;
    do_proc_state_t *proc_state;
    if ( !domino_init(opt->domino_config_file, FALSE, DO_PARAM_END) )
        return FALSE;
    artix = artix_new(opt->artix_host,
                      opt->artix_cs,
                      opt->artix_port,
                      opt->artix_user,
                      opt->artix_password,
                      FALSE);
    if ( !artix )
        return  FALSE;
    if ( opt->load_objects == -1 ) {
        printf("Abort artix load\n");
        error_message(artix->info, "Сброшенно вручную");
        res = artix_info_set_upload_state(artix->info, upsError);
        if ( res )
            printf("Load aborted\n");
        else
            printf("Error abort load\n");
        artix_free(artix);
        return res;
    }

    if ( (alias = domino_alias_new(NULL)) == NULL ) {
        artix_free(artix);
        return FALSE;
    }
    if ( !do_alias_open(alias, TRUE) ) {
        artix_free(artix);
        do_alias_free(alias);
        return FALSE;
    }
    proc_state = do_proc_state_new();
    do_proc_state_set_callback_percent(proc_state, proc_persent, NULL);
    do_proc_state_set_state(proc_state, DO_STATE_RUN);
    printf("Start load %s %s %s\n", (opt->load_objects&ARTIX_SEND_PRODUCT)?"product":"",(opt->load_objects&ARTIX_SEND_CASHIER)?"cashier":"",(opt->load_objects&ARTIX_SEND_KEYBOARD)?"keyboard":"");
    res = artix_send(artix, alias, proc_state, domino_unit(), 
               opt->load_objects,
               NULL);
    if ( res )
        printf("Load completed\n");
    else
        printf("Error load\n");
    do_proc_state_free(proc_state);
    artix_free(artix);
    do_alias_free(alias);
    return res;
}
*/