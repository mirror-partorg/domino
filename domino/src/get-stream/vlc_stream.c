#include "vlc_stream.h"
#include <vlc/vlc.h>
#include <stdio.h>
#include <dolib.h>
#include "../misc/send_error.h"
#include "version.h"
#include <errno.h>


#define G_N_ELEMENTS(arr) (sizeof (arr) / sizeof ((arr)[0]))
//#define LOG_ERR_PREF "get-stream-post.log"
#ifdef _LINUX
#define DEV_NULL "/dev/null"
#else
#define DEV_NULL ""
#endif


const char *vlc_common_args[] = {
        "--no-xlib", "--no-stats", "--no-osd", "--no-video-title-show",
        "--ignore-config","--no-sub-autodetect-file",
        "--network-caching=1200",
        "--sout-transcode-fps=15",
        //"--no-audio",
        //"--rtsp-tcp1"

        };

struct _vlc_stream_t {
    libvlc_media_player_t *media_player[2];
    libvlc_instance_t *vlc_inst[2];
    char *tmp_filename[2];
    int verbose;
    int piece_duration;
    int overlay_duration;
    int overlay_duration_real;
    do_string_list_t *post_command;
    char *log_err;

    outfile_complected_t complect_cb;
    void                *complect_data;

    int terminated;
    int running;
    char *cam_name;

};
static char *get_tmp_avi_filename(int i, time_t t1, time_t t2, const char *name);
static char *get_command(do_string_list_t *list, const char *input, const char *output, int test, const char *log_err);
static char *get_temp_dir();

vlc_stream_t *vlc_stream_new(const char *url, const char *transcode_param, const char *name, int verbose)
{
    vlc_stream_t *stream;
    char *args[G_N_ELEMENTS(vlc_common_args) + 2];
    int i;
    int args_count;

    args_count = G_N_ELEMENTS(vlc_common_args) + 1;
    stream = do_malloc0(sizeof(vlc_stream_t));
    stream->piece_duration = 60*5;
    stream->overlay_duration = 20;
    stream->overlay_duration_real = 17;
    if ( !vlc_stream_set_post_command(stream, "mencoder -fps 16.5 -ss 16 <input> -ovc copy -oac copy -o <output>") ) {
        vlc_stream_free(stream);
        return NULL;
    }
    for ( i = 0; i < G_N_ELEMENTS(vlc_common_args); i++ )
        args[i] = do_strdup(vlc_common_args[i]);
    args[G_N_ELEMENTS(vlc_common_args)] = malloc(1024);
    if ( name )
        stream->cam_name = do_strdup(name);
    else
        stream->cam_name = do_strdup("video");
    char *tmp_dir;
    tmp_dir = get_temp_dir();
    stream->log_err = do_malloc0(sizeof(tmp_dir) + sizeof(program_name) + sizeof(stream->cam_name) + 25);
    sprintf(stream->log_err, "%s/%s-%s.log", tmp_dir, program_name, stream->cam_name);
    if ( verbose ) {
        args_count++;
        args[args_count-1] = do_strdup("--verbose=11111");
    }

    for ( i = 0; i < 2; i++ ) {
        libvlc_media_t *media;
        stream->tmp_filename[i] = get_tmp_avi_filename(i, 0, 0, stream->cam_name);
        if ( transcode_param && transcode_param[0] != '\0' )
            sprintf(args[G_N_ELEMENTS(vlc_common_args)],
            "--sout=#transcode{%s}:std{access=file,mux=avi,dst=%s}", transcode_param, stream->tmp_filename[i]);
        else
            sprintf(args[G_N_ELEMENTS(vlc_common_args)],
            "--sout=#std{access=file,mux=avi,dst=%s}", stream->tmp_filename[i]);

        stream->vlc_inst[i] = libvlc_new(args_count, (const char**)args);
        if ( !stream->vlc_inst[i] ) {
            const char *err = libvlc_errmsg();
            if ( err )
                do_log(LOG_ERR, "Can't create vlc instance: %s", err);
            else
                do_log(LOG_ERR, "Can't create vlc instance.");
            send_error_message_width_host_info_printf("%s %s Can't create vlc instance.", program_name, stream->cam_name);
            vlc_stream_free(stream);
            return NULL;
        }
        stream->media_player[i] = libvlc_media_player_new(stream->vlc_inst[i]);
        if ( !stream->media_player[i] ) {
            const char *err = libvlc_errmsg();
            if ( err )
                do_log(LOG_ERR, "Can't create vlc media player: %s", err);
            else
                do_log(LOG_ERR, "Can't create vlc media player.");
            send_error_message_width_host_info_printf("%s %s Can't create vlc media player.", program_name, stream->cam_name);
            vlc_stream_free(stream);
            return NULL;
        }
        media = libvlc_media_new_location(stream->vlc_inst[i], url);
        libvlc_media_player_set_media(stream->media_player[i], media);
        libvlc_media_release(media);
    }
    for ( i = 0; i < args_count; i++ )
        do_free(args[i]);

    return stream;
}
void vlc_stream_free(vlc_stream_t *stream)
{
    int i;
    for ( i = 0; i < 2; i++ ) {
        if ( stream->media_player[i] )
            libvlc_media_player_release(stream->media_player[i]);
        if ( stream->vlc_inst[i] )
            libvlc_release(stream->vlc_inst[i]);
        if ( stream->tmp_filename[i] )
            do_free(stream->tmp_filename[i]);
    }
    if ( stream->post_command )
        do_string_list_free(stream->post_command);
    if ( stream->cam_name )
        do_free(stream->cam_name);
    if ( stream->log_err )
        do_free(stream->log_err);
    do_free(stream);
}

void vlc_stream_set_piece_duration(vlc_stream_t *stream, int sec)
{
    stream->piece_duration = sec;
}
void vlc_stream_set_overlay_duration(vlc_stream_t *stream, int sec, int real_sec)
{
    stream->overlay_duration = sec;
    stream->overlay_duration_real = real_sec;
}
int vlc_stream_set_post_command(vlc_stream_t *stream, const char *command)
{

    char *ch_in, *ch_out, *crnt;
    ch_in = strstr(command, "<input>");
    ch_out = strstr(command, "<output>");

    if ( !ch_in || !ch_out )
        return FALSE;

    if ( stream->post_command )
        do_string_list_free(stream->post_command);

    crnt = (char*)command;
    stream->post_command = do_string_list_new();

    do_string_list_addn_alloc(stream->post_command, crnt, ((ch_in < ch_out) ? ch_in: ch_out) - crnt);
    if ( ch_in < ch_out ) {
        crnt = ch_in + strlen("<input>");
        do_string_list_add_alloc(stream->post_command, "<input>");
    }
    else {
        crnt = ch_out + strlen("<output>");
        do_string_list_add_alloc(stream->post_command, "<output>");
    }
    do_string_list_addn_alloc(stream->post_command, crnt, ((ch_in > ch_out) ? ch_in: ch_out) - crnt);
    if ( ch_in > ch_out ) {
        crnt = ch_in + strlen("<input>");
        do_string_list_add_alloc(stream->post_command, "<input>");
    }
    else {
        crnt = ch_out + strlen("<output>");
        do_string_list_add_alloc(stream->post_command, "<output>");
    }
    do_string_list_add_alloc(stream->post_command, crnt);
    return TRUE;
}

int vlc_stream_run(vlc_stream_t *stream, int test_only)
{
    int indx = 0;
    int i;
    time_t t_start[2];
    time_t t_stop[2];
    if ( libvlc_media_player_play(stream->media_player[indx]) ) {
        const char *err = libvlc_errmsg();
        if ( err )
            do_log(LOG_ERR, "Can't start play vlc media player: %s", err);
        else
            do_log(LOG_ERR, "Can't start play vlc media player.");
        send_error_message_width_host_info_printf("%s %s Can't start play vlc media player.", program_name, stream->cam_name);

        stream->running = FALSE;
        return FALSE;
    }
    stream->running = TRUE;
    t_start[indx] = time(NULL) + stream->overlay_duration_real;

#define SLEEP(sec, indx1, indx2 ) \
        for ( i = 0; i < sec; i++ ) { \
            sleep(1); \
            if ( stream->terminated ) { \
                stream->running = FALSE; \
                return TRUE; \
            } \
            if ( libvlc_media_player_get_state(stream->media_player[indx1]) == libvlc_Error ) { \
                do_log(LOG_ERR, "VLC media player error"); \
                send_error_message_width_host_info_printf("%s %s VLC media player error.", program_name, stream->cam_name); \
                stream->running = FALSE; \
                return FALSE; \
            } \
            if ( indx2 != -1 && libvlc_media_player_get_state(stream->media_player[indx2]) == libvlc_Error ) { \
                do_log(LOG_ERR, "VLC media player error"); \
                send_error_message_width_host_info_printf("%s %s VLC media player error.", program_name, stream->cam_name); \
                stream->running = FALSE; \
                return FALSE; \
            } \
        }

    SLEEP(stream->overlay_duration, indx, -1);

    while ( !stream->terminated  ) {

        SLEEP(stream->piece_duration - stream->overlay_duration, indx, -1);

        if ( libvlc_media_player_play(stream->media_player[!indx]) ) {
            const char *err = libvlc_errmsg();
            if ( err )
                do_log(LOG_ERR, "Can't start play vlc media player: %s", err);
            else
                do_log(LOG_ERR, "Can't start play vlc media player.");
            send_error_message_width_host_info_printf("%s %s Can't start play vlc media player.", program_name, stream->cam_name);

            stream->running = FALSE;
            return FALSE;
        }
        t_start[!indx] = time(NULL) + stream->overlay_duration_real;

        SLEEP( stream->overlay_duration, indx, !indx );

        libvlc_media_player_stop(stream->media_player[indx]);
        t_stop[indx] = time(NULL);
        t_start[!indx] = t_stop[indx];

        char *filename;
        char *command;
        filename = get_tmp_avi_filename(0, t_start[indx], t_stop[indx], stream->cam_name);
        command = get_command(stream->post_command, stream->tmp_filename[indx], filename, test_only, stream->log_err);
        if ( system (command) ) {
            do_log(LOG_ERR, "Can't run command \"%s\"", command);
            if ( !test_only ) {
                char *error;
                error = do_read_from_file(stream->log_err);
                //send_error_message_width_host_info_printf("%s %s: Can't run command \"%s\"\n%s", program_name, stream->cam_name, command, error ? error : "");
                do_log(LOG_ERR, "%s Can't run command \"%s\"\n%s",  stream->cam_name, command, error ? error : "");
                if ( error )
                    do_free(error);
            }
/*            if ( rename(stream->tmp_filename[indx], filename) == -1 ) {
                do_log(LOG_ERR, "Error rename file (%s->%s): %s\n", stream->tmp_filename[indx], filename, strerror(errno));
                return 0;
            }*/
            /*stream->running = FALSE;
            do_free(command);
            return FALSE;
            */
        }
        else {
            if ( stream->complect_cb ) {
                struct tm tm;
                tm = *localtime(&t_start[indx]);
                if ( !stream->complect_cb(filename, tm, t_stop[indx]-t_start[indx], 1, stream->complect_data) ) {
                    do_free(filename);
                    stream->running = FALSE;
                    return FALSE;
                }
            }
        }
        do_free(command);
        indx = !indx;
        if ( test_only ) {
            do_log(LOG_INFO, "Make file \"%s\"", filename);
            do_free(filename);
            stream->running = FALSE;
            return TRUE;
        }
        do_free(filename);
    }
    stream->running = FALSE;
    return TRUE;
}

void vlc_stream_stop(vlc_stream_t *stream)
{
    stream->terminated = TRUE;
}
int vlc_stream_is_running(vlc_stream_t *stream)
{
    return stream->running;
}
static char *get_temp_dir()
{
    char *ch;
    ch = getenv ("TMPDIR");
    if ( ch == NULL || *ch == '\0')
        ch = getenv ("TMP");
    if ( ch == NULL || *ch == '\0')
        ch = getenv ("TEMP");
    if ( ch == NULL || *ch == '\0')
        ch = "/tmp";

    return strdup(ch);

}
static char *get_tmp_avi_filename(int i, time_t t1, time_t t2, const char *name)
{
    char s[1024];
    char *tmp_dir;
    tmp_dir = get_temp_dir();
    if ( !t1 ) {
        time_t t = time(NULL);
        struct tm tm;
        tm = *localtime(&t);
        sprintf(s, "%s/%s-%04d%02d%02d%02d%02d%02d-%d.avi", tmp_dir, name,
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, i);
    }
    else {
        struct tm tm;
        tm = *localtime(&t1);
        struct tm tm2;
        tm2 = *localtime(&t2);
        sprintf(s, "%s/%s-%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d.avi", tmp_dir, name,
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
                tm2.tm_year+1900, tm2.tm_mon+1, tm2.tm_mday, tm2.tm_hour, tm2.tm_min, tm2.tm_sec
                );
    }
    do_free(tmp_dir);
    return do_strdup(s);
}
static char *get_command(do_string_list_t *list, const char *input, const char *output, int test, const char *log_err)
{
    int i;
    size_t size;
    char *res, *ch;
    size = 0;
    for ( i = 0; i < list->count; i++ ) {
        if ( !strcmp(list->list[i], "<input>") ) {
            size += strlen(input);
        }
        else
        if ( !strcmp(list->list[i], "<output>") ) {
            size += strlen(output);
        }
        else
            size += strlen(list->list[i]);
    }
    if ( !test ) {
        size+=strlen(log_err) + strlen(DEV_NULL) + 10;
    }
    size++;
    res = do_malloc0(size);
    ch = res;
    for ( i = 0; i < list->count; i++ ) {
        if ( !strcmp(list->list[i], "<input>") ) {
            strcpy(ch, input);
            ch += strlen(input);
        }
        else
        if ( !strcmp(list->list[i], "<output>") ) {
            strcpy(ch, output);
            ch += strlen(output);
        }
        else {
            strcpy(ch, list->list[i]);
            ch += strlen(list->list[i]);
        }
    }
    if ( !test ) {
        if ( DEV_NULL[0] != '\0' ) {
            strcpy(ch, " >"DEV_NULL);
            ch += strlen(" >"DEV_NULL);
        }
        sprintf(ch, " 2>%s", log_err);
        ch += strlen(" 2>") + strlen(log_err);
    }
    ch[0] = '\0';
    return res;
}

void vlc_stream_set_complection_func(vlc_stream_t *stream, outfile_complected_t func, void *user_data)
{
    stream->complect_cb = func;
    stream->complect_data = user_data;
}
