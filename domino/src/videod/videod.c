#include <getopt.h>
#include <signal.h>
#include <locale.h>

#include <dolib.h>
#include "version.h"
#include "option.h"
#include <domino.h>
#include "videoinfo.h"
#include "video.h"
#include "../misc/mproc_state.h"

static option_t *opt = NULL;
static do_rpc_t *rpc = NULL;
static video_info_t *video_info = NULL;
static video_t *video = NULL;


#ifdef _WIN32
#include <windows.h>
LPCTSTR videod_service_name = (LPCTSTR)program_name;
LPCTSTR videod_service_display = (LPCTSTR)"video Server";
LPCTSTR videod_service_description = (LPCTSTR)"video Server: Check video";
LPCTSTR videod_service_depends = NULL;
#endif


static int init()
{
    if (!option_read_config(opt))
        exit(1);

    if (!opt->run_foreground)
        do_log_set_type(program_name, opt->log_type, opt->log_file);
    else
        do_log_set_stdout();

    if ( opt->send_error_to )
       send_error_set_sendto(opt->send_error_to);

    if ( !rpc && !opt->clear_only )
            rpc = do_rpc_new(program_name,
                         VIDEOD_RELEASE " (" ADATE ")",
                         opt->tcp_port, opt->telnet_tcp_port,
                         do_mproc_state_get(),
                         opt->passwd_file);

    if (video_info)
        video_info_free(video_info);
    video_info = video_info_new(opt);
    if ( !opt->clear_only )
        do_rpc_set_extended(rpc, video_info, &video_exec_command, video_info_get_cmd(), video_info_get_cmd_count());

    if (video)
        video_free(video);
    video = video_new(opt);
    video_info_set(video_info, video);

    do_mproc_state_set_state(DO_STATE_RUN);
    return 1;
}

static int videod_main(int argc, char *argv[])
{
    init(argc, argv);
    double sleep = 0;

    if (!opt->clear_only) {
        while (do_mproc_state_get_state() != DO_STATE_STOP) {

            switch ( do_mproc_state_get_state() ) {
                case DO_STATE_RUN:
                    if ( sleep <= 0 ) {
                        video_run(video);
                        sleep = opt->time_sleep;
                    }
                    sleep -= 1/3.;
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
            if ( do_mproc_state_get_state() == DO_STATE_STOP )
                break;

            if ( opt->ext_program && opt->ext_program[0] != '\0' ) {
                int err;
                err = system(opt->ext_program);
                if ( err ) {
                    do_log(LOG_ERR, "command %s return %d", opt->ext_program, err);
                }
            }
            DO_SEC_SLEEP(1/3.);
        }
    }
    else
       clear_old(video);
    video_info_set(video_info, NULL);
    video_free(video);
    option_free(opt);
    video_info_free(video_info);
    if ( !opt->clear_only )
        do_rpc_free(rpc);
    return 1;
}
static void install_signal_handlers(void);

int main(int argc, char *argv[])
{
#ifdef MEMORY_DEBUG
    initmemdebug();
#endif
    setlocale(LC_ALL, "");
#ifdef _LINUX
    install_signal_handlers();
#endif

    do_log_set_type(program_name, DO_LOG_TYPE_SYSLOG, "");
    do_mproc_state_new();
    opt = option_new();
    if (option_parse_options(opt, argc, argv))
       exit(1);
    if (opt->show_version) {
        printf("videod " VIDEOD_RELEASE " (" ADATE ") \n");
        exit(0);
    }
    if (!opt->run_foreground) {
#ifdef _LINUX
        if (!option_read_config(opt))
               exit(1);
            do_daemon_start(argc, argv, videod_main, NULL, opt->pid_file);
#elif defined (_WIN32)
        do_service_start(opt->service_install, videod_main, videod_service_name, videod_service_display,
                        videod_service_description, videod_service_depends);
#endif
    }
    else {
        do_log_set_stdout();
        videod_main(argc, argv);
    }
#ifdef MEMORY_DEBUG
    showmemdebug();
#endif
    return 0;
}

#ifdef _LINUX
static void sig_restart(int signo);

static void install_signal_handlers(void) {
    sigset_t sig_set;


    sigemptyset(&sig_set);

    sigaddset(&sig_set, SIGCHLD);
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
#endif
#ifdef SIGIO
    sigaddset(&sig_set, SIGIO);
#endif
#ifdef SIGBUS
    sigaddset(&sig_set, SIGBUS);
#endif
    signal(SIGINT, sig_restart);
    signal(SIGHUP, sig_restart);
    signal(SIGUSR1, sig_restart);
    signal(SIGTERM, sig_restart);

    sigprocmask(SIG_UNBLOCK, &sig_set, NULL);
}

static void sig_restart(int signo) {
    do_log(LOG_INFO, "Get signal %d", signo);
    if ( signo == SIGHUP ) {
	if ( video )
	    video_set_reload(video);
	return;
    }
    exit(1);
}
#endif
