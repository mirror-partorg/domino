#ifndef LIBAV_H_INCLUDED
#define LIBAV_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


typedef struct _LibAV LibAV;
typedef int (*outfile_complected_t)(const char *filename, struct tm start, int secs,  double correction, void *user_date);

#ifdef __cplusplus
extern "C"
{
#endif

LibAV *libav_new(int vesbose);

int    libav_add_option(LibAV *av, const char *opt, const char *arg);
void   libav_close(LibAV *av);

int    libav_add_input(LibAV *av, const char *format, const char *filename, const char *rate, const char *size);
//int    libav_add_output(LibAV *av, const char *filename, const char *rate, const char *size);
int    libav_run(LibAV *av, int test);

void   libav_set_limit_filesize(LibAV *av, uint64_t limit_filesize);
void   libav_set_video_codec_name(LibAV *av, const char *name);
void   libav_set_audio_codec_name(LibAV *av, const char *name);
void   libav_set_frame_pix_fmt(LibAV *av, const char *pix_fmt);
void   libav_set_frame_acpect_ratio(LibAV *av, const char *acpect_ratio);

void   libav_set_output_path(LibAV *av, const char *path);
void   libav_set_output_filename(LibAV *av, const char *name);
void   libav_set_output_rate(LibAV *av, const char *rate);
void   libav_set_output_size(LibAV *av, const char *size);

void   libav_set_complection_func(LibAV *av, outfile_complected_t func, void *user_data);

void   libav_set_video_disable(LibAV *av, int disable);
void   libav_set_audio_disable(LibAV *av, int disable);

#ifdef USE_AVFILTER
void   libav_set_filters(LibAV *av, const char *filters);
#endif

//int64_t libav_getutime(void);

#ifdef __cplusplus
}
#endif


#endif // LIBAV_H_INCLUDED
