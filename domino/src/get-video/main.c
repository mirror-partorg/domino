
#include <signal.h>
#include <dolib.h>
#include "libav.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include "../misc/storage.h"
#include "../misc/send_error.h"
#include <libavutil/log.h>
#include "version.h"
#include "option.h"
#include "../misc/mproc_state.h"
#ifdef _LINUX
#include <fcntl.h>
#include <unistd.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#endif

static option_t *opt = NULL;
static LibAV *av = NULL;
static storage_t *storage;
static do_rpc_t *rpc = NULL;
static do_rpc_client_t *rpc_client = NULL;

#define WAIT_INIT_SEC 20

#ifdef _WIN32
#include <windows.h>
LPCTSTR program_service_name = (LPCTSTR)NAME;
LPCTSTR program_service_display = (LPCTSTR)"Get video from cam";
LPCTSTR program_service_description = (LPCTSTR)"Get video from cam";
LPCTSTR program_service_depends = NULL;
#endif
#ifdef _LINUX
enum PIPES {READ, WRITE};
int initpipe[2];
#endif

static int server_send_status(const char *status)
{
    int res;
    if ( !do_rpc_client_connect(rpc_client) )
        return FALSE;
    char param[1024];
    snprintf(param, sizeof(param)-1, "\"%s\" \"%s\"", opt->cam_name, status);
    res = do_rpc_client_send_command(rpc_client, "change_status", param, NULL, NULL);
    do_rpc_client_disconnect(rpc_client);
    return res;
}

static void sig_restart(int signo) {
    do_log(LOG_INFO, "Get signal %d", signo);
    if ( av )
        libav_close(av);
    if ( storage )
        storage_free(storage);
    if ( rpc_client )
        server_send_status("stop");
    if ( signo == SIGTERM ) {
#ifdef DEBUG
        send_info_message_width_host_info_printf("%s has terminated", program_name);
#endif
    }
    else
        send_error_message_width_host_info_printf("%s  was breaked for signal %d", program_name, signo);
    do_mproc_state_set_state(DO_STATE_STOP);
    if ( rpc )
        do_rpc_free(rpc);
    exit(1);
}

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
static int copy_file_func(const char *filename, struct tm start_time, int secs, double correction, storage_t *storage);

static int set_av_param(LibAV *av)
{
    int audio_disable, video_disable;

    audio_disable = !strcmp(opt->audio_format, NULL_FORMAT) || !strcmp(opt->audio_device, NULL_DEVICE);
    video_disable = !strcmp(opt->video_format, NULL_FORMAT) || !strcmp(opt->video_device, NULL_DEVICE);

    if ( audio_disable && video_disable ) {
        do_log(LOG_ERR, "No input device");
        return FALSE;
    }
    if ( !video_disable ) {
        libav_add_option(av, "b:v", opt->video_bitrate);
        if ( !libav_add_input(av, opt->video_format,  opt->video_device, opt->video_rate, opt->video_size) )
            return FALSE;
    }
    else
        libav_set_video_disable(av, TRUE);

    if ( !audio_disable ) {
        libav_add_option(av, "b:a", opt->audio_bitrate);
        if ( !libav_add_input(av, opt->audio_format, opt->audio_device, NULL, NULL) )
            return FALSE;

    }
    else
        libav_set_audio_disable(av, TRUE);

    libav_set_limit_filesize(av, opt->filesize);
    if ( !video_disable ) {
        if ( opt->video_rate )
            libav_set_output_rate(av, opt->video_rate);
        libav_set_output_size(av, opt->video_size);
        libav_set_video_codec_name(av, opt->video_codec);
        libav_set_frame_pix_fmt(av, opt->video_pixel_format);
        if ( opt->video_ratio )
            libav_set_frame_acpect_ratio(av, opt->video_ratio);
        libav_add_option(av, "b:v", opt->video_bitrate);
    }
    if ( !audio_disable ) {
        libav_set_audio_codec_name(av, opt->audio_codec);
        libav_add_option(av, "b:a", opt->audio_bitrate);
    }
#ifdef USE_AVFILTER
    if ( opt->filters )
        libav_set_filters(av, opt->filters);
#endif

    return TRUE;
}
#define MAX_DEBUG 1024
static const char *ignore_error[] ={"ioctl(VIDIOC_QBUF): Invalid argument"};

static void log_callback(void *ptr, int level, const char *fmt, va_list vl)
{
    static char message[MAX_DEBUG];
    vsnprintf(message, MAX_DEBUG - 1, fmt, vl);
    int m;
    if ( level < AV_LOG_WARNING )
        m = LOG_ERR;
    else
    if ( level < AV_LOG_INFO )
        m = LOG_WARNING;
    else
        m = LOG_INFO;
    if ( strlen(message) > 0 && message[strlen(message)-1] == '\n')
        message[strlen(message)-1] = '\0';
    for ( m = 0; m < DO_ARRAY_ELEMS(ignore_error); m++ )
	if ( !strcmp(message, ignore_error[m]) )
	    return;

    if ( m != LOG_INFO || opt->only_check )
        do_log(m, "%s", message);
    if ( m == LOG_ERR ) {
        char *msg;
        msg = do_malloc(strlen(message) + strlen(program_name) + 10);
        sprintf(msg, "%s: %s", program_name, message);
        send_error_message_with_host_info(msg);
        do_free(msg);
    }
}
static int init()
{
    do_mproc_state_set_state(DO_STATE_STARTING);
    if (!option_read_config(opt))
        return FALSE;

    if (!opt->run_foreground)
        do_log_set_type(program_name, opt->log_type, opt->log_file);
    else
        do_log_set_stdout();
    if ( opt->disable_send_error )
        send_error_message_set_disable_send();

    if ( !opt->only_check ) {
        storage = storage_new(STORAGE_DEFAULT_QUEUE_LEN, program_name);
        if ( !storage_set_file_storage_param(storage, opt->storage_url, opt->cam_name) )
            return FALSE;
//        do_log(LOG_INFO, "storage-url \"%s\"", opt->storage_url);
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
    av = libav_new(opt->debug_level);

    if ( !set_av_param(av) ) {
        libav_close(av);
        storage_free(storage);
        return FALSE;
    }
    if ( !opt->only_check )
        libav_set_complection_func(av, (outfile_complected_t)copy_file_func, storage);
    if ( opt->cam_name )
        libav_set_output_filename(av, opt->cam_name);

    do_mproc_state_set_state(DO_STATE_RUN);
    return TRUE;
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
static int program_main(int argc, char *argv[])
{
    if ( !init(argc, argv) ) {
        send_error_message_width_host_info_printf("%s: CRITICAL. Daemon not init.", program_name);
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
    send_info_message_width_host_info_printf("%s has started", program_name);
#endif
    if ( libav_run(av, opt->only_check) < 0 )
        ret = 1;

    do_mproc_state_set_state(DO_STATE_STOPING);

    if ( !opt->only_check )
         storage_free(storage);
    libav_close(av);
    option_free(opt);
    if ( ret )
        send_error_message_width_host_info_printf("%s: CRITICAL. Daemon close with error", program_name);

    send_error_message_width_host_info_printf("%s: Daemon close", program_name);
    do_mproc_state_set_state(DO_STATE_STOP);
    if ( rpc )
        do_rpc_free(rpc);
    return ret;
}
#ifdef _LINUX
int daemon_start(int argc, char** argv, do_main_handler_t *wrapper,/* char *username, char *groupname,*/ char *pid_file)
{
    int i, fd;
    pid_t cpid, sid;
    mode_t oldmask;
    //struct passwd *user = NULL;
    //struct group  *group = NULL;

    /*if ( username )
       if ( (user = getpwnam(username)) == NULL ) {
           fprintf(stderr, "ERROR: Can't get information about user %s.\n", username);
           return FALSE;
       }
    if ( groupname )
       if ( (group = getgrnam(groupname)) == NULL ) {
           fprintf(stderr, "ERROR: Can't get information about group %s.\n", groupname);
           return FALSE;
       }
    */
    if ( (cpid = fork()) < 0 ) {
        fprintf(stderr, "ERROR: Cannot fork to become daemon\n");
        return FALSE;
    }
    else if ( cpid > 0 ) {
        int flag;
        struct timeval tv;
        fd_set readset;
        int c;

        close (initpipe[WRITE]);
        tv.tv_sec = WAIT_INIT_SEC;
        tv.tv_usec = 0;

        FD_ZERO(&readset);
        FD_SET(initpipe[READ], &readset);

        c = select(FD_SETSIZE, &readset, NULL, NULL, &tv);

        switch (c) {
            case 0:
                send_error_message_width_host_info_printf("%s: CRITICAL. Initialization timed out", program_name);
                do_log(LOG_ERR, "read from pipe close for timeout");
                return FALSE;
            case 1:
                break;
            case -1:
            default:
                fprintf(stderr, "read from pipe error: %s\n", strerror(errno));
                return FALSE;
        }
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

    /*if ( user ) {
        gid_t gid;
        if ( group )
            gid = group->gr_gid;
        else
            gid = user->pw_gid;
        if ( setgid(gid) ) {
            fprintf(stderr, "ERROR: setgid(%d) failed.\n", (int) gid);
            return FALSE;
        }
        if ( setuid(user->pw_uid) ) {
            fprintf(stderr, "ERROR: setuid(%d) failed.\n", (int) user->pw_uid);
            return FALSE;
        }
    }*/

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
int main(int argc, char **argv)
{

    setlocale(LC_ALL, "");
    do_mproc_state_new();

    install_signal_handlers();

    av_log_set_callback(log_callback);

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
        if ( !daemon_start(argc, argv, program_main, /*opt->user, opt->group,*/ opt->pid_file) )
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
static int copy_file_func(const char *filename, struct tm start_tm, int secs, double correction, storage_t *storage)
{
    int ret;
    ret = storage_add_file(storage, filename, start_tm, secs, correction);
    if ( !ret ) {
        do_log(LOG_ERR, "Cant add \"%s\" to queue", filename);
        send_error_message_width_host_info_printf("%s: Cant add \"%s\" to queue", program_name, filename);
    }
    return ret;
}
