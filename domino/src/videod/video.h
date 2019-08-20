#ifndef VIDEO_H_INCLUDED
#define VIDEO_H_INCLUDED

#include "option.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _video_t video_t;

video_t *video_new(option_t *opt);
void     video_free(video_t *video);
void     video_run(video_t *video);
void     video_set_cam_up(video_t *video, const char *cam, int up);
void     video_reinit(video_t *video, option_t *opt);
void     video_set_reload(video_t *video);
int      clear_old(video_t *video);

#ifdef __cplusplus
}
#endif


#endif // VIDEO_H_INCLUDED
