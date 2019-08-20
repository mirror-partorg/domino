#ifndef VLC_STREAM_H_INCLUDED
#define VLC_STREAM_H_INCLUDED

#include <dolib.h>
#define WARN_RES __attribute__((warn_unused_result))

typedef struct _vlc_stream_t vlc_stream_t;
typedef int (*outfile_complected_t)(const char *filename, struct tm start, int secs,  double correction, void *user_date);

#ifdef __cplusplus
extern "C"
{
#endif

vlc_stream_t *vlc_stream_new(const char *url, const char *transcode_param, const char *name, int verbose);
void          vlc_stream_free(vlc_stream_t *stream);

void          vlc_stream_set_piece_duration(vlc_stream_t *stream, int sec);
void          vlc_stream_set_overlay_duration(vlc_stream_t *stream, int sec, int real_sec);
int           vlc_stream_set_post_command(vlc_stream_t *stream, const char *command) WARN_RES;

int           vlc_stream_run(vlc_stream_t *stream, int test_only);
void          vlc_stream_stop(vlc_stream_t *stream);
int           vlc_stream_is_running(vlc_stream_t *stream);

void          vlc_stream_set_complection_func(vlc_stream_t *stream, outfile_complected_t func, void *user_data);


#ifdef __cplusplus
}
#endif


#endif // VLC_STREAM_H_INCLUDED
