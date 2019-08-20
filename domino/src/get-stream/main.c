
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "vlc_stream.h"
#include "../misc/storage.h"
#include "../misc/send_error.h"
#include "version.h"
#include <dolib.h>
#include <locale.h>
#include "option.h"
#include <errno.h>
#include "../misc/mproc_state.h"


#define MAX_SEC_WAIT 20

#ifdef _LINUX
#include <signal.h>
#include <syslog.h>

#include <fcntl.h>
//#include <unistd.h>
//#include <sys/types.h>
#include <sys/stat.h>


static void install_signal_handlers(void);
static void sig_restart(int signo);
enum PIPES {READ, WRITE};
int initpipe[2];
static int daemon_start(int argc, char** argv, do_main_handler_t *wrapper, char *pid_file);
static void send_to_pipe(int i);

#endif

static vlc_stream_t *stream = NULL;
static storage_t *storage = NULL;
static option_t *opt = NULL;
static do_rpc_t *rpc = NULL;
static do_rpc_client_t *rpc_client = NULL;
static int server_send_status(const char *status);
static int program_main(int argc, char *argv[]);
static int copy_file_func(const char *filename, struct tm start_time, int secs, double correction, storage_t *storage);
static int init();

int main( int argc, char *argv[] ) {

    setlocale(LC_ALL, "");
    do_mproc_state_new();

    do_log_set_stdout();
#ifdef _LINUX
    install_signal_handlers();
#endif
    opt = option_new();
    if (option_parse_options(opt, argc, argv))
        exit(1);

    if (opt->show_version) {
        printf(program_name " " PROGRAM_RELEASE " (" ADATE ") \n");
        exit(0);
    }
    do_log_set_type(program_name, DO_LOG_TYPE_SYSLOG, NULL);

    if (!opt->run_foreground) {
#ifdef _LINUX
        if ( !option_read_config(opt) ) {
            send_error_message_width_host_info_printf("%s: CRITICAL. Error read option.", program_name);
            exit(1);
        }
        if ( pipe(initpipe) < 0 ) {
            do_log(LOG_ERR, "Error init pipe %s\n", strerror(errno));
            send_error_message_width_host_info_printf("%s: CRITICAL. Error init pipe", program_name);
            exit(1);
        }
        if ( !daemon_start(argc, argv, program_main, opt->pid_file) )
            return 1;
#elif defined (_WIN32)
        do_service_start(opt->service_install, exchanged_main, exchanged_service_name, exchanged_service_display,
                        exchanged_service_description, exchanged_service_depends);
#endif
    }
    else {
        do_log_set_stdout();
        send_error_message_set_disable_send();
        program_main(argc, argv);
    }
    return 0;
}
#ifdef _LINUX
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
    if ( stream )
        vlc_stream_free(stream);

    if ( storage )
        storage_free(storage);
    if ( rpc_client )
        server_send_status("stop");
    if ( signo == SIGTERM ) {
#ifdef DEBUG
        send_info_message_width_host_info_printf("%s %s has terminated", program_name, opt->cam_name ? opt->cam_name : "");
#endif
    }
    else
        send_error_message_width_host_info_printf("%s %s was breaked for signal %d", program_name, opt->cam_name ? opt->cam_name : "", signo);
    if ( rpc )
        do_rpc_free(rpc);
    do_rpc_client_free(rpc_client);
    do_mproc_state_set_state(DO_STATE_STOP);
    exit(1);
}
#endif
static int copy_file_func(const char *filename, struct tm start_tm, int secs, double correction, storage_t *storage)
{
    int ret;
    ret = storage_add_file(storage, filename, start_tm, secs, correction);
    if ( !ret ) {
        do_log(LOG_ERR, "Cant add \"%s\" to queue", filename);
        send_error_message_width_host_info_printf("%s %s:  Cant add \"%s\" to queue", program_name, opt->cam_name ? opt->cam_name : "", filename);
    }
    return ret;
}
#ifdef _LINUX
int daemon_start(int argc, char** argv, do_main_handler_t *wrapper,  char *pid_file)
{
    int i, fd;
    pid_t cpid, sid;
    mode_t oldmask;
    if ( (cpid = fork()) < 0 ) {
        fprintf(stderr, "ERROR: Cannot fork to become daemon\n");
        return FALSE;
    }
    else if ( cpid > 0 ) {
        int flag;
        close (initpipe[WRITE]);
        if ( read(initpipe[READ], (char*)&flag, sizeof(int)) < 0 ) {
            fprintf(stderr, "read from pipe error: %s\n", strerror(errno));
            flag = FALSE;
        }
        close (initpipe[READ]);
        if ( flag == FALSE )
            return FALSE;
        return TRUE;
    }

    if ( pid_file ) {
        FILE *fp = fopen (pid_file, "w");
        if (!fp)
            fprintf(stderr, "Open error on pid file %s", pid_file);
        else {
            unsigned int pid = getpid ();
            fprintf(fp, "%u\n", pid);
            if (fclose (fp))
                fprintf(stderr, "Close error on pid file %s", pid_file);

        }
    }

    if ( (sid = setsid() ) < 0) {
        fprintf(stderr, "ERROR: set sid\n");
        return FALSE;
    }

    closelog();

    for (i=0; i<sysconf(_SC_OPEN_MAX); i++) {
        if ( i == initpipe[WRITE] ||
             i == initpipe[READ] )
           continue;
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
    return TRUE;
}
#endif
static int program_main(int argc, char *argv[])
{
    if ( !init(argc, argv) ) {
        send_error_message_width_host_info_printf("%s %s: CRITICAL. Daemon not init.", program_name, opt->cam_name ? opt->cam_name : "");
#ifdef _LINUX
        if ( !opt->run_foreground )
            send_to_pipe(FALSE);
#endif
        return 1;
    }
#ifdef _LINUX
    if ( !opt->run_foreground )
        send_to_pipe(TRUE);
#endif
    int ret = 0;

    if ( rpc_client )
        server_send_status("start");
#ifdef DEBUG
    send_info_message_width_host_info_printf("%s %s has started", program_name, opt->cam_name ? opt->cam_name : "");
#endif
    if ( vlc_stream_run(stream, opt->only_check) < 0 )
        ret = 1;

    if ( !opt->only_check )
         storage_free(storage);
    vlc_stream_free(stream);
    if ( rpc )
        do_rpc_free(rpc);
    option_free(opt);
    if ( ret )
        send_error_message_width_host_info_printf("%s %s: CRITICAL. Daemon close with error", program_name, opt->cam_name ? opt->cam_name : "");

    send_error_message_width_host_info_printf("%s %s: Daemon close", program_name, opt->cam_name ? opt->cam_name : "");
    return ret;
}
#ifdef _LINUX
static void send_to_pipe(int i)
{
    close (initpipe[READ]);
    if ( write(initpipe[WRITE], (char*)&i, sizeof(int)) != sizeof(int) ) {
        do_log(LOG_ERR, "write to pipe error: %s", strerror(errno));
    }
    close (initpipe[WRITE]);
}
#endif
static int init()
{
    if (!option_read_config(opt))
        return FALSE;

    if (!opt->run_foreground)
        do_log_set_type(program_name, opt->log_type, opt->log_file);
    else
        do_log_set_stdout();
    if ( opt->disable_send_error )
        send_error_message_set_disable_send();

    stream = vlc_stream_new(opt->video_url, opt->transcode_str, opt->cam_name, FALSE);
    if ( !stream )
        return FALSE;

    if ( !opt->only_check ) {
        storage = storage_new(STORAGE_DEFAULT_QUEUE_LEN, program_name);
        if ( !storage_set_file_storage_param(storage, opt->storage_url, opt->cam_name) )
            return FALSE;
#ifdef USE_POSTGRESQL
        if (opt->conninfo)
            storage_set_db_storage_param(storage, opt->conninfo);
#endif
        if ( !rpc && opt->tcp_port )
            rpc = do_rpc_new(program_name,
                         PROGRAM_RELEASE " (" ADATE ")",
                         opt->tcp_port, 0,
                         do_mproc_state_get(),
                         opt->passwd_file);
        if ( !rpc_client && opt->server_host )
            rpc_client = do_rpc_client_new(opt->server_host, opt->server_port, opt->server_user, opt->server_password, FALSE);
    }
    if ( opt->piece_duration )
        vlc_stream_set_piece_duration(stream, opt->piece_duration);
    if ( opt->overlay_duration ) {
        if ( !opt->overlay_duration_real )
            opt->overlay_duration_real = opt->overlay_duration - 3;
        vlc_stream_set_overlay_duration(stream, opt->overlay_duration, opt->overlay_duration_real);
    }
    if ( opt->proc_command )
    if ( !vlc_stream_set_post_command(stream, opt->proc_command) ) {
        do_log(LOG_ERR, "Postprocessing command not correct");
        return 1;
    }
    if ( !opt->only_check )
        vlc_stream_set_complection_func(stream, (outfile_complected_t)copy_file_func, storage);

    do_mproc_state_set_state(DO_STATE_RUN);
    return TRUE;
}
static int server_send_status(const char *status)
{
    int res;
    do_log(LOG_INFO, "send %s", status);
    if ( !do_rpc_client_connect(rpc_client) )
        return FALSE;
    char param[1024];
    snprintf(param, sizeof(param)-1, "\"%s\" \"%s\"", opt->cam_name, status);
    res = do_rpc_client_send_command(rpc_client, "change_status", param, NULL, NULL);
    do_rpc_client_disconnect(rpc_client);
    return res;
}
