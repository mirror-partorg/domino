
#include "libav.h"
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <unistd.h>
#include <dolib.h>

#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
#include <libavutil/parseutils.h>
#include <libavutil/mathematics.h>

#include <libavutil/fifo.h>
#include <libavutil/avassert.h>
#include <libavutil/imgutils.h>
#include <libavutil/avstring.h>

#if HAVE_SYS_RESOURCE_H
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#elif HAVE_GETPROCESSTIMES
#include <windows.h>
#endif
#if HAVE_GETPROCESSMEMORYINFO
#include <windows.h>
#include <psapi.h>
#endif

#if USE_AVFILTER
#ifndef USE_LIBAV
# include <libavfilter/avcodec.h>
#endif
# include <libavfilter/avfilter.h>
# include <libavfilter/avfiltergraph.h>
#ifndef USE_LIBAV
# include <libavfilter/buffersink.h>
#else
# include <libavfilter/buffersrc.h>
#endif
//# include <libavfilter/vsrc_buffer.h>
#endif


#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <time.h>

#define WARN_RES __attribute__((warn_unused_result))


#define VSYNC_AUTO       -1
#define VSYNC_PASSTHROUGH 0
#define VSYNC_CFR         1
#define VSYNC_VFR         2

typedef struct _StreamMap StreamMap;
typedef struct _AudioChannelMap AudioChannelMap;

typedef struct _InputStream InputStream;
typedef struct _InputFile InputFile;
typedef struct _OutputStream OutputStream;
typedef struct _OutputFile OutputFile;

int av_buffersrc_buffer(AVFilterContext *s, AVFilterBufferRef *buf);


struct _LibAV {

    char       *frame_rate;
    char       *frame_size;
    char       *frame_pix_fmt;

    char       *output_path;
    char       *output_filename;

    char       *output_rate;
    char       *output_size;

    /* input options */
    int64_t input_ts_offset;
    int rate_emu;

    /* output options */

    AudioChannelMap *audio_channel_maps;
    int           nb_audio_channel_maps;

    int metadata_global_manual;
    int metadata_streams_manual;

    int64_t recording_time;
    uint64_t limit_filesize;
    float mux_preload;
    float mux_max_delay;

    int video_disable;
    int audio_disable;
    int data_disable;

    /* indexed by output file stream index */

    int          *streamid_map;
    int          nb_streamid_map;

    char         *video_codec_name;
    char         *audio_codec_name;
    char         *frame_aspect_ratio;
#ifdef USE_AVFILTER
    char         *filters;
#endif
    AVDictionary *format_opts;
    AVDictionary *codec_opts;
    struct SwsContext *sws_opts;

    int exit_on_error;
    int64_t video_size;
    int64_t audio_size;
    int64_t extra_size;
    int nb_frames_dup;
    int nb_frames_drop;

    InputStream *input_streams;
    int         nb_input_streams;
    InputFile   *input_files;
    int         nb_input_files;

    OutputStream *output_streams;
    int        nb_output_streams;
    OutputFile   *output_files;
    int        nb_output_files;

    outfile_complected_t complect_cb;
    void                *complect_data;


    uint8_t *audio_buf;
    unsigned int allocated_audio_buf_size;


    int test;

};

/* select an input stream for an output stream */
struct _StreamMap {
    int disabled;           /** 1 is this mapping is disabled by a negative map */
    int file_index;
    int stream_index;
    int sync_file_index;
    int sync_stream_index;
};

struct _AudioChannelMap{
    int  file_idx,  stream_idx,  channel_idx; // input
    int ofile_idx, ostream_idx;               // output
};
/**
 * select an input file for an output file
 */
typedef struct MetadataMap {
    int  file;      ///< file index
    char type;      ///< type of metadata to copy -- (g)lobal, (s)tream, (c)hapter or (p)rogram
    int  index;     ///< stream/chapter/program number
} MetadataMap;


#define MAX_STREAMS 1024    /* arbitrary sanity check value */

static int input_sync;

static float dts_delta_threshold = 10;

static int print_stats = 1;

typedef struct FrameBuffer {
    uint8_t *base[4];
    uint8_t *data[4];
    int  linesize[4];

    int h, w;
    enum PixelFormat pix_fmt;

    int refcount;
    InputStream *ist;
    struct FrameBuffer *next;
} FrameBuffer;

struct _InputStream {
    int file_index;
    AVStream *st;
    int discard;             /* true if stream data should be discarded */
    int decoding_needed;     /* true if the packets must be decoded in 'raw_fifo' */
    AVCodec *dec;
    AVFrame *decoded_frame;
    AVFrame *filtered_frame;

    int64_t       start;     /* time when read started */
    int64_t       next_pts;  /* synthetic pts for cases where pkt.pts
                                is not defined */
    int64_t       pts;       /* current pts */
    double ts_scale;
    int is_start;            /* is 1 at the start and after a discontinuity */
    int showed_multi_packet_warning;
    AVDictionary *opts;

    /* a pool of free buffers for decoded data */
    FrameBuffer *buffer_pool;
    int dr1;
};

struct _InputFile {
    AVFormatContext *ctx;
    int eof_reached;      /* true if eof reached */
    int ist_index;        /* index of first stream in av->input_streams */
    int buffer_size;      /* current total buffer size */
    int64_t ts_offset;
    int nb_streams;       /* number of stream that ffmpeg is aware of; may be different
                             from ctx.nb_streams if new streams appear during av_read_frame() */
    int rate_emu;
};

struct _OutputStream {
    int file_index;          /* file index */
    int index;               /* stream index in the output file */
    int source_index;        /* InputStream index */
    AVStream *st;            /* stream in the output file */
    int encoding_needed;     /* true if encoding needed for this stream */
    int frame_number;
    /* input pts and corresponding output pts
       for A/V sync */
    InputStream *sync_ist; /* input stream to sync against */
    int64_t sync_opts;       /* output frame counter, could be changed to some true timestamp */ // FIXME look at frame_number
    AVBitStreamFilterContext *bitstream_filters;
    AVCodec *enc;
    int64_t max_frames;
    AVFrame *output_frame;

    /* video only */
    int video_resample;
    AVFrame resample_frame;              /* temporary frame for image resampling */
    struct SwsContext *img_resample_ctx; /* for image resampling */
    int resample_height;
    int resample_width;
    int resample_pix_fmt;
    AVRational frame_rate;
    int force_fps;
    int top_field_first;

    float frame_aspect_ratio;

    /* forced key frames */
    int64_t *forced_kf_pts;
    int forced_kf_count;
    int forced_kf_index;

    float rematrix_volume;
    AVFifoBuffer *fifo;     /* for compression: one audio fifo per codec */
    //FILE *logfile;

    struct SwrContext *swr;

    int64_t sws_flags;
    AVDictionary *opts;
    int is_past_recording_time;
    int stream_copy;
    const char *attachment_filename;
    int copy_initial_nonkeyframes;

#if USE_AVFILTER
    AVFilterContext *output_video_filter;
    AVFilterContext *input_video_filter;
    AVFilterBufferRef *picref;
    char *avfilter;
    AVFilterGraph *graph;
#endif

};

struct _OutputFile {
    AVFormatContext *ctx;
    AVDictionary *opts;
    int ost_index;       /* index of the first stream in av->output_streams */
    int64_t recording_time; /* desired length of the resulting file in microseconds */
    int64_t start_time;     /* start time in microseconds */
    uint64_t limit_filesize;
};


static int assert_avoptions_(AVDictionary *m) WARN_RES;
static int assert_codec_experimental_(AVCodecContext *c, int encoder) WARN_RES;
static int write_frame(AVFormatContext *s, AVPacket *pkt, OutputStream *ost, int exit_on_error) WARN_RES;
static int libav_encode_audio_frame(LibAV *av, AVFormatContext *s, OutputStream *ost,
                              const uint8_t *buf, int buf_size) WARN_RES;
static int libav_do_audio_out(LibAV *av, AVFormatContext *s, OutputStream *ost,
                         InputStream *ist, AVFrame *decoded_frame) WARN_RES;
static int do_video_resample(OutputStream *ost,
                              InputStream *ist,
                              AVFrame *in_picture,
                              AVFrame **out_picture) WARN_RES;
static int libav_do_video_out(LibAV *av, AVFormatContext *s,
                         OutputStream *ost,
                         InputStream *ist,
                         AVFrame *in_picture,
                         int *frame_size, float quality) WARN_RES;
static int libav_flush_encoders(LibAV *av, OutputStream *ost_table, int nb_ostreams) WARN_RES;
static int libav_do_streamcopy(LibAV *av, InputStream *ist, OutputStream *ost, const AVPacket *pkt) WARN_RES;
static int libav_transcode_audio(LibAV *av, InputStream *ist, AVPacket *pkt, int *got_output) WARN_RES;
static AVCodec *find_codec_or_die_(const char *name, enum AVMediaType type, int encoder) WARN_RES;
static int libav_add_input_streams(LibAV *av, AVFormatContext *ic) WARN_RES;
static int parse_matrix_coeffs(uint16_t *dest, const char *str) WARN_RES;
#ifdef USE_AVFILTER
static int configure_video_filters(InputStream *ist, OutputStream *ost) WARN_RES;
#endif
//static int64_t libav_getutime(void);

//static int file_indx = 0;

//static int file_reopen(LibAV *av, OutputFile *out);


static void init_opts(LibAV *av)
{
    av->sws_opts = sws_getContext(16, 16, 0, 16, 16, 0, SWS_BICUBIC, NULL, NULL, NULL);
    av->format_opts = NULL;
    av->codec_opts = NULL;
}

static void uninit_opts(LibAV *av)
{
    sws_freeContext(av->sws_opts);
    av->sws_opts = NULL;
    av_dict_free(&av->format_opts);
    av->format_opts = NULL;
    av_dict_free(&av->codec_opts);
    av->codec_opts = NULL;
}

static void print_error(const char *filename, int err)
{
    char errbuf[128];
    const char *errbuf_ptr = errbuf;

    if (av_strerror(err, errbuf, sizeof(errbuf)) < 0)
        errbuf_ptr = strerror(AVUNERROR(err));
    av_log(NULL, AV_LOG_ERROR, "%s: %s\n", filename, errbuf_ptr);
}

static int check_stream_specifier(AVFormatContext *s, AVStream *st, const char *spec)
{
    if (*spec <= '9' && *spec >= '0') /* opt:index */
        return strtol(spec, NULL, 0) == st->index;
    else if (*spec == 'v' || *spec == 'a' || *spec == 's' || *spec == 'd' ||
             *spec == 't') { /* opt:[vasdt] */
        enum AVMediaType type;

        switch (*spec++) {
        case 'v': type = AVMEDIA_TYPE_VIDEO;      break;
        case 'a': type = AVMEDIA_TYPE_AUDIO;      break;
        case 's': type = AVMEDIA_TYPE_SUBTITLE;   break;
        case 'd': type = AVMEDIA_TYPE_DATA;       break;
        case 't': type = AVMEDIA_TYPE_ATTACHMENT; break;
        default: abort(); // never reached, silence warning
        }
        if (type != st->codec->codec_type)
            return 0;
        if (*spec++ == ':') { /* possibly followed by :index */
            int i, index = strtol(spec, NULL, 0);
            for (i = 0; i < s->nb_streams; i++)
                if (s->streams[i]->codec->codec_type == type && index-- == 0)
                   return i == st->index;
            return 0;
        }
        return 1;
    } else if (*spec == 'p' && *(spec + 1) == ':') {
        int prog_id, i, j;
        char *endptr;
        spec += 2;
        prog_id = strtol(spec, &endptr, 0);
        for (i = 0; i < s->nb_programs; i++) {
            if (s->programs[i]->id != prog_id)
                continue;

            if (*endptr++ == ':') {
                int stream_idx = strtol(endptr, NULL, 0);
                return stream_idx >= 0 &&
                    stream_idx < s->programs[i]->nb_stream_indexes &&
                    st->index == s->programs[i]->stream_index[stream_idx];
            }

            for (j = 0; j < s->programs[i]->nb_stream_indexes; j++)
                if (st->index == s->programs[i]->stream_index[j])
                    return 1;
        }
        return 0;
    } else if (!*spec) /* empty specifier, matches everything */
        return 1;

    av_log(s, AV_LOG_ERROR, "Invalid stream specifier: %s.\n", spec);
    return AVERROR(EINVAL);
}
static AVDictionary *filter_codec_opts(AVDictionary *opts, AVCodec *codec,
                                AVFormatContext *s, AVStream *st)
{
    AVDictionary    *ret = NULL;
    AVDictionaryEntry *t = NULL;
    int            flags = s->oformat ? AV_OPT_FLAG_ENCODING_PARAM
                                      : AV_OPT_FLAG_DECODING_PARAM;
    char          prefix = 0;
    const AVClass    *cc = avcodec_get_class();

    if (!codec)
        return NULL;

    switch (codec->type) {
    case AVMEDIA_TYPE_VIDEO:
        prefix  = 'v';
        flags  |= AV_OPT_FLAG_VIDEO_PARAM;
        break;
    case AVMEDIA_TYPE_AUDIO:
        prefix  = 'a';
        flags  |= AV_OPT_FLAG_AUDIO_PARAM;
        break;
    case AVMEDIA_TYPE_SUBTITLE:
        prefix  = 's';
        flags  |= AV_OPT_FLAG_SUBTITLE_PARAM;
        break;
    default:
        break;
    }

    while ( (t = av_dict_get(opts, "", t, AV_DICT_IGNORE_SUFFIX)) != 0 ) {
        char *p = strchr(t->key, ':');

        /* check stream specification in opt name */
        if (p)
            switch (check_stream_specifier(s, st, p + 1)) {
            case  1: *p = 0; break;
            case  0:         continue;
            default:         return NULL;
            }

        if (av_opt_find(&cc, t->key, NULL, flags, AV_OPT_SEARCH_FAKE_OBJ) ||
            (codec && codec->priv_class &&
             av_opt_find(&codec->priv_class, t->key, NULL, flags,
                         AV_OPT_SEARCH_FAKE_OBJ)))
            av_dict_set(&ret, t->key, t->value, 0);
        else if (t->key[0] == prefix &&
                 av_opt_find(&cc, t->key + 1, NULL, flags,
                             AV_OPT_SEARCH_FAKE_OBJ))
            av_dict_set(&ret, t->key + 1, t->value, 0);

        if (p)
            *p = ':';
    }
    return ret;
}
static AVDictionary **setup_find_stream_info_opts(AVFormatContext *s,
                                           AVDictionary *codec_opts)
{
    int i;
    AVDictionary **opts;

    if (!s->nb_streams)
        return NULL;
    opts = av_mallocz(s->nb_streams * sizeof(*opts));
    if (!opts) {
        av_log(NULL, AV_LOG_ERROR,
               "Could not alloc memory for stream options.\n");
        return NULL;
    }
    for (i = 0; i < s->nb_streams; i++)
        opts[i] = filter_codec_opts(codec_opts, avcodec_find_decoder(s->streams[i]->codec->codec_id),
                                    s, s->streams[i]);
    return opts;
}
#define FREE_STR(a) { if (a) { free(a); a = NULL; }; }

static void libav_reset_options(LibAV *av, int is_input)
{

    //av_freep(&o->stream_maps);
    av_freep(&av->audio_channel_maps);
    av_freep(&av->streamid_map);



    av->mux_max_delay  = 0.7;
    av->limit_filesize = UINT64_MAX;

    FREE_STR(av->frame_rate)
    FREE_STR(av->frame_size);
    FREE_STR(av->frame_pix_fmt);
    av->input_ts_offset = 0;
    av->rate_emu=0;
    av->nb_audio_channel_maps = 0;
    av->metadata_global_manual=0;
    av->metadata_streams_manual=0;

    if ( !is_input )
        av->recording_time=INT64_MAX;
    av->limit_filesize=0;
    av->mux_preload=0;

    av->video_disable=0;
    av->audio_disable=0;
    av->data_disable=0;

    FREE_STR(av->video_codec_name);
    FREE_STR(av->audio_codec_name);
    FREE_STR(av->frame_aspect_ratio);

    FREE_STR(av->output_filename);
    FREE_STR(av->output_path);
    FREE_STR(av->output_rate);
    FREE_STR(av->output_size);


    uninit_opts(av);
    init_opts(av);
}

static int alloc_buffer(AVCodecContext *s, InputStream *ist, FrameBuffer **pbuf)
{
    FrameBuffer  *buf = av_mallocz(sizeof(*buf));
    int ret, i;
    const int pixel_size = av_pix_fmt_descriptors[s->pix_fmt].comp[0].step_minus1+1;
    int h_chroma_shift, v_chroma_shift;
    int edge = 32; // XXX should be avcodec_get_edge_width(), but that fails on svq1
    int w = s->width, h = s->height;

    if (!buf)
        return AVERROR(ENOMEM);

    if (!(s->flags & CODEC_FLAG_EMU_EDGE)) {
        w += 2*edge;
        h += 2*edge;
    }

    avcodec_align_dimensions(s, &w, &h);
    if ((ret = av_image_alloc(buf->base, buf->linesize, w, h,
                              s->pix_fmt, 32)) < 0) {
        av_freep(&buf);
        return ret;
    }
    /* XXX this shouldn't be needed, but some tests break without this line
     * those decoders are buggy and need to be fixed.
     * the following tests fail:
     * bethsoft-vid, cdgraphics, ansi, aasc, fraps-v1, qtrle-1bit
     */
    memset(buf->base[0], 128, ret);

    avcodec_get_chroma_sub_sample(s->pix_fmt, &h_chroma_shift, &v_chroma_shift);
    for (i = 0; i < FF_ARRAY_ELEMS(buf->data); i++) {
        const int h_shift = i==0 ? 0 : h_chroma_shift;
        const int v_shift = i==0 ? 0 : v_chroma_shift;
        if (s->flags & CODEC_FLAG_EMU_EDGE)
            buf->data[i] = buf->base[i];
        else
            buf->data[i] = buf->base[i] +
                           FFALIGN((buf->linesize[i]*edge >> v_shift) +
                                   (pixel_size*edge >> h_shift), 32);
    }
    buf->w       = s->width;
    buf->h       = s->height;
    buf->pix_fmt = s->pix_fmt;
    buf->ist     = ist;

    *pbuf = buf;
    return 0;
}

static void free_buffer_pool(InputStream *ist)
{
    FrameBuffer *buf = ist->buffer_pool;
    while (buf) {
        ist->buffer_pool = buf->next;
        av_freep(&buf->base[0]);
        av_free(buf);
        buf = ist->buffer_pool;
    }
}

static void unref_buffer(InputStream *ist, FrameBuffer *buf)
{
    av_assert0(buf->refcount);
    buf->refcount--;
    if (!buf->refcount) {
        buf->next = ist->buffer_pool;
        ist->buffer_pool = buf;
    }
}

static int codec_get_buffer(AVCodecContext *s, AVFrame *frame)
{
    InputStream *ist = s->opaque;
    FrameBuffer *buf;
    int ret, i;

    if(av_image_check_size(s->width, s->height, 0, s))
        return -1;

    if (!ist->buffer_pool && (ret = alloc_buffer(s, ist, &ist->buffer_pool)) < 0)
        return ret;

    buf              = ist->buffer_pool;
    ist->buffer_pool = buf->next;
    buf->next        = NULL;
    if (buf->w != s->width || buf->h != s->height || buf->pix_fmt != s->pix_fmt) {
        av_freep(&buf->base[0]);
        av_free(buf);
        ist->dr1 = 0;
        if ((ret = alloc_buffer(s, ist, &buf)) < 0)
            return ret;
    }
    buf->refcount++;

    frame->opaque        = buf;
    frame->type          = FF_BUFFER_TYPE_USER;
    frame->extended_data = frame->data;
    frame->pkt_pts       = s->pkt ? s->pkt->pts : AV_NOPTS_VALUE;

    for (i = 0; i < FF_ARRAY_ELEMS(buf->data); i++) {
        frame->base[i]     = buf->base[i];  // XXX h264.c uses base though it shouldn't
        frame->data[i]     = buf->data[i];
        frame->linesize[i] = buf->linesize[i];
    }

    return 0;
}

static void codec_release_buffer(AVCodecContext *s, AVFrame *frame)
{
    InputStream *ist = s->opaque;
    FrameBuffer *buf = frame->opaque;
    int i;

    if(frame->type!=FF_BUFFER_TYPE_USER)
        return avcodec_default_release_buffer(s, frame);

    for (i = 0; i < FF_ARRAY_ELEMS(frame->data); i++)
        frame->data[i] = NULL;

    unref_buffer(ist, buf);
}

static int assert_avoptions_(AVDictionary *m)
{
    AVDictionaryEntry *t;
    if ((t = av_dict_get(m, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
        av_log(NULL, AV_LOG_FATAL, "Option %s not found.\n", t->key);
        return FALSE;
    }
    return TRUE;
}

static int assert_codec_experimental_(AVCodecContext *c, int encoder)
{
    const char *codec_string = encoder ? "encoder" : "decoder";
    AVCodec *codec;
    if (c->codec->capabilities & CODEC_CAP_EXPERIMENTAL &&
        c->strict_std_compliance > FF_COMPLIANCE_EXPERIMENTAL) {
        av_log(NULL, AV_LOG_FATAL, "%s '%s' is experimental and might produce bad "
                "results.\nAdd '-strict experimental' if you want to use it.\n",
                codec_string, c->codec->name);
        codec = encoder ? avcodec_find_encoder(c->codec->id) : avcodec_find_decoder(c->codec->id);
        if (!(codec->capabilities & CODEC_CAP_EXPERIMENTAL))
            av_log(NULL, AV_LOG_FATAL, "Or use the non experimental %s '%s'.\n",
                   codec_string, codec->name);
        return FALSE;
    }
    return TRUE;
}

static void choose_sample_fmt(AVStream *st, AVCodec *codec)
{
    if (codec && codec->sample_fmts) {
        const enum AVSampleFormat *p = codec->sample_fmts;
        for (; *p != -1; p++) {
            if (*p == st->codec->sample_fmt)
                break;
        }
        if (*p == -1) {
            //if((codec->capabilities & CODEC_CAP_LOSSLESS) &&
            //    av_get_sample_fmt_name(st->codec->sample_fmt) > av_get_sample_fmt_name(codec->sample_fmts[0]))
            //    av_log(NULL, AV_LOG_ERROR, "Conversion will not be lossless.\n");
            if(av_get_sample_fmt_name(st->codec->sample_fmt))
            av_log(NULL, AV_LOG_WARNING,
                   "Incompatible sample format '%s' for codec '%s', auto-selecting format '%s'\n",
                   av_get_sample_fmt_name(st->codec->sample_fmt),
                   codec->name,
                   av_get_sample_fmt_name(codec->sample_fmts[0]));
            st->codec->sample_fmt = codec->sample_fmts[0];
        }
    }
}

static void choose_sample_rate(AVStream *st, AVCodec *codec)
{
    if (codec && codec->supported_samplerates) {
        const int *p  = codec->supported_samplerates;
        int best      = 0;
        int best_dist = INT_MAX;
        for (; *p; p++) {
            int dist = abs(st->codec->sample_rate - *p);
            if (dist < best_dist) {
                best_dist = dist;
                best      = *p;
            }
        }
        if (best_dist) {
            av_log(st->codec, AV_LOG_WARNING, "Requested sampling rate unsupported using closest supported (%d)\n", best);
        }
        st->codec->sample_rate = best;
    }
}

static void choose_pixel_fmt(AVStream *st, AVCodec *codec)
{
    if (codec && codec->pix_fmts) {
        const enum PixelFormat *p = codec->pix_fmts;
        //int has_alpha= av_pix_fmt_descriptors[st->codec->pix_fmt].nb_components % 2 == 0;
        enum PixelFormat best= codec->pix_fmts[0];
        if (st->codec->strict_std_compliance <= FF_COMPLIANCE_UNOFFICIAL) {
            if (st->codec->codec_id == CODEC_ID_MJPEG) {
                p = (const enum PixelFormat[]) { PIX_FMT_YUVJ420P, PIX_FMT_YUVJ422P, PIX_FMT_YUV420P, PIX_FMT_YUV422P, PIX_FMT_NONE };
            } else if (st->codec->codec_id == CODEC_ID_LJPEG) {
                p = (const enum PixelFormat[]) { PIX_FMT_YUVJ420P, PIX_FMT_YUVJ422P, PIX_FMT_YUVJ444P, PIX_FMT_YUV420P,
                                                 PIX_FMT_YUV422P, PIX_FMT_YUV444P, PIX_FMT_BGRA, PIX_FMT_NONE };
            }
        }
        for (; *p != PIX_FMT_NONE; p++) {
            //best= avcodec_find_best_pix_fmt2(best, *p, st->codec->pix_fmt, has_alpha, NULL);
            if (*p == st->codec->pix_fmt)
                break;
        }
        if (*p == PIX_FMT_NONE) {
            if (st->codec->pix_fmt != PIX_FMT_NONE)
                av_log(NULL, AV_LOG_WARNING,
                       "Incompatible pixel format '%s' for codec '%s', auto-selecting format '%s'\n",
                       av_pix_fmt_descriptors[st->codec->pix_fmt].name,
                       codec->name,
		    av_pix_fmt_descriptors[best].name);
            st->codec->pix_fmt = best;
        }
    }
}


static double libav_get_sync_ipts(LibAV *av, const OutputStream *ost)
{
    const InputStream *ist = ost->sync_ist;
    OutputFile *of = &av->output_files[ost->file_index];
    return (double)(ist->pts - of->start_time) / AV_TIME_BASE;
}

static int write_frame(AVFormatContext *s, AVPacket *pkt, OutputStream *ost, int exit_on_error)
{
    AVBitStreamFilterContext *bsfc = ost->bitstream_filters;
    AVCodecContext          *avctx = ost->st->codec;
    int ret;

    /*
     * Audio encoders may split the packets --  #frames in != #packets out.
     * But there is no reordering, so we can limit the number of output packets
     * by simply dropping them here.
     * Counting encoded video frames needs to be done separately because of
     * reordering, see do_video_out()
     */
    if (!(avctx->codec_type == AVMEDIA_TYPE_VIDEO && avctx->codec)) {
        if (ost->frame_number >= ost->max_frames)
            return TRUE;
        ost->frame_number++;
    }

    while (bsfc) {
        AVPacket new_pkt = *pkt;
        int a = av_bitstream_filter_filter(bsfc, avctx, NULL,
                                           &new_pkt.data, &new_pkt.size,
                                           pkt->data, pkt->size,
                                           pkt->flags & AV_PKT_FLAG_KEY);
        if (a > 0) {
            av_free_packet(pkt);
            new_pkt.destruct = av_destruct_packet;
        } else if (a < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to open bitstream filter %s for stream %d with codec %s",
                   bsfc->filter->name, pkt->stream_index,
                   avctx->codec ? avctx->codec->name : "copy");
            print_error("", a);
            if ( exit_on_error )
                return FALSE;
        }
        *pkt = new_pkt;

        bsfc = bsfc->next;
    }

    ret = av_interleaved_write_frame(s, pkt);
    if (ret < 0) {
        print_error("av_interleaved_write_frame()", ret);
        return FALSE;
    }
    return TRUE;
}

static void generate_silence(uint8_t* buf, enum AVSampleFormat sample_fmt, size_t size)
{
    int fill_char = 0x00;
    if (sample_fmt == AV_SAMPLE_FMT_U8)
        fill_char = 0x80;
    memset(buf, fill_char, size);
}


static int libav_encode_audio_frame(LibAV *av, AVFormatContext *s, OutputStream *ost,
                              const uint8_t *buf, int buf_size)
{
    AVCodecContext *enc = ost->st->codec;
    AVFrame *frame = NULL;
    AVPacket pkt;
    int ret, got_packet;

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    if (buf) {
        if (!ost->output_frame) {
            ost->output_frame = avcodec_alloc_frame();
            if (!ost->output_frame) {
                av_log(NULL, AV_LOG_FATAL, "out-of-memory in encode_audio_frame()\n");
                return -1;
            }
        }
        frame = ost->output_frame;
        if (frame->extended_data != frame->data)
            av_freep(&frame->extended_data);
        avcodec_get_frame_defaults(frame);

        frame->nb_samples  = buf_size /
                             (enc->channels * av_get_bytes_per_sample(enc->sample_fmt));
        if ((ret = avcodec_fill_audio_frame(frame, enc->channels, enc->sample_fmt,
                                            buf, buf_size, 1)) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Audio encoding failed\n");
            return -1;
        }
    }

    got_packet = 0;
    if (avcodec_encode_audio2(enc, &pkt, frame, &got_packet) < 0) {
        av_log(NULL, AV_LOG_FATAL, "Audio encoding failed\n");
        return -1;
    }

    ret = pkt.size;

    if (got_packet) {
        pkt.stream_index = ost->index;
        if (pkt.pts != AV_NOPTS_VALUE)
            pkt.pts      = av_rescale_q(pkt.pts,      enc->time_base, ost->st->time_base);
        if (pkt.duration > 0)
            pkt.duration = av_rescale_q(pkt.duration, enc->time_base, ost->st->time_base);

        if ( !write_frame(s, &pkt, ost, av->exit_on_error) )
            return -1;

        av->audio_size += pkt.size;

        av_free_packet(&pkt);
    }

    if (frame)
        ost->sync_opts += frame->nb_samples;

    return ret;
}

static int libav_do_audio_out(LibAV *av, AVFormatContext *s, OutputStream *ost,
                         InputStream *ist, AVFrame *decoded_frame)
{
    uint8_t *buftmp;
    int64_t audio_buf_size, size_out;

    int frame_bytes;
    AVCodecContext *enc = ost->st->codec;
    AVCodecContext *dec = ist->st->codec;
    int osize = av_get_bytes_per_sample(enc->sample_fmt);
    int isize = av_get_bytes_per_sample(dec->sample_fmt);
    uint8_t *buf = decoded_frame->data[0];
    int size     = decoded_frame->nb_samples * dec->channels * isize;
    int64_t allocated_for_size = size;

    audio_buf_size  = (allocated_for_size + isize * dec->channels - 1) / (isize * dec->channels);
    audio_buf_size  = (audio_buf_size * enc->sample_rate + dec->sample_rate) / dec->sample_rate;
    audio_buf_size  = audio_buf_size * 2 + 10000; // safety factors for the deprecated resampling API
    audio_buf_size  = FFMAX(audio_buf_size, enc->frame_size);
    audio_buf_size *= osize * enc->channels;

    if (audio_buf_size > INT_MAX) {
        av_log(NULL, AV_LOG_FATAL, "Buffer sizes too large\n");
        return FALSE;
    }

    av_fast_malloc(&av->audio_buf, &av->allocated_audio_buf_size, audio_buf_size);
    if (!av->audio_buf) {
        av_log(NULL, AV_LOG_FATAL, "Out of memory in do_audio_out\n");
        return FALSE;
    }


    ost->sync_opts = lrintf(libav_get_sync_ipts(av, ost) * enc->sample_rate) -
                                av_fifo_size(ost->fifo) / (enc->channels * osize); // FIXME wrong

    buftmp = buf;
    size_out = size;

    /* now encode as many frames as possible */
    if (!(enc->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)) {
        /* output resampled raw samples */
        if (av_fifo_realloc2(ost->fifo, av_fifo_size(ost->fifo) + size_out) < 0) {
            av_log(NULL, AV_LOG_FATAL, "av_fifo_realloc2() failed\n");
            return FALSE;
        }
        av_fifo_generic_write(ost->fifo, buftmp, size_out, NULL);

        frame_bytes = enc->frame_size * osize * enc->channels;

        while (av_fifo_size(ost->fifo) >= frame_bytes) {
            av_fifo_generic_read(ost->fifo, av->audio_buf, frame_bytes, NULL);
            if ( libav_encode_audio_frame(av, s, ost, av->audio_buf, frame_bytes)  < 0 )
                return FALSE;
        }
    } else {
        if ( libav_encode_audio_frame(av, s, ost, buftmp, size_out) < 0 )
            return FALSE;
    }
    return TRUE;
}

static int bit_buffer_size = 1024 * 256;
static uint8_t *bit_buffer = NULL;

static int do_video_resample(OutputStream *ost,
                              InputStream *ist,
                              AVFrame *in_picture,
                              AVFrame **out_picture)
{
#if USE_AVFILTER
    *out_picture = in_picture;
#else
    AVCodecContext *dec = ist->st->codec;
    AVCodecContext *enc = ost->st->codec;
    int resample_changed = ost->resample_width   != in_picture->width  ||
                           ost->resample_height  != in_picture->height ||
                           ost->resample_pix_fmt != in_picture->format;

    *out_picture = in_picture;
    if (resample_changed) {
        av_log(NULL, AV_LOG_INFO,
               "Input stream #%d:%d frame changed from size:%dx%d fmt:%s to size:%dx%d fmt:%s / frm size:%dx%d fmt:%s\n",
               ist->file_index, ist->st->index,
               ost->resample_width, ost->resample_height, av_get_pix_fmt_name(ost->resample_pix_fmt),
               dec->width         , dec->height         , av_get_pix_fmt_name(dec->pix_fmt),
               in_picture->width, in_picture->height, av_get_pix_fmt_name(in_picture->format));
        ost->resample_width   = in_picture->width;
        ost->resample_height  = in_picture->height;
        ost->resample_pix_fmt = in_picture->format;
    }

    ost->video_resample = in_picture->width   != enc->width  ||
                          in_picture->height  != enc->height ||
                          in_picture->format  != enc->pix_fmt;

    if (ost->video_resample) {
        *out_picture = &ost->resample_frame;
        if (!ost->img_resample_ctx || resample_changed) {
            /* initialize the destination picture */
            if (!ost->resample_frame.data[0]) {
                avcodec_get_frame_defaults(&ost->resample_frame);
                if (avpicture_alloc((AVPicture *)&ost->resample_frame, enc->pix_fmt,
                                    enc->width, enc->height)) {
                    av_log(NULL, AV_LOG_FATAL, "Cannot allocate temp picture, check pix fmt\n");
                    return FALSE;
                }
            }
            /* initialize a new scaler context */
            sws_freeContext(ost->img_resample_ctx);
            ost->img_resample_ctx = sws_getContext(in_picture->width, in_picture->height, in_picture->format,
                                                   enc->width, enc->height, enc->pix_fmt,
                                                   ost->sws_flags, NULL, NULL, NULL);
            if (ost->img_resample_ctx == NULL) {
                av_log(NULL, AV_LOG_FATAL, "Cannot get resampling context\n");
                return FALSE;
            }
        }
        sws_scale(ost->img_resample_ctx, (const uint8_t *const*)in_picture->data, in_picture->linesize,
              0, ost->resample_height, (*out_picture)->data, (*out_picture)->linesize);
    }
#endif
    return TRUE;
}


static int libav_do_video_out(LibAV *av, AVFormatContext *s,
                         OutputStream *ost,
                         InputStream *ist,
                         AVFrame *in_picture,
                         int *frame_size, float quality)
{
    int nb_frames, i, ret, format_video_sync;
    AVFrame *final_picture;
    AVCodecContext *enc;
    double sync_ipts;
    double duration = 0;

    enc = ost->st->codec;

    if (ist->st->start_time != AV_NOPTS_VALUE && ist->st->first_dts != AV_NOPTS_VALUE) {
        duration = FFMAX(av_q2d(ist->st->time_base), av_q2d(ist->st->codec->time_base));
        if(ist->st->avg_frame_rate.num)
            duration= FFMAX(duration, 1/av_q2d(ist->st->avg_frame_rate));

        duration /= av_q2d(enc->time_base);
    }

    sync_ipts = libav_get_sync_ipts(av, ost) / av_q2d(enc->time_base);

    /* by default, we output a single frame */
    nb_frames = 1;

    *frame_size = 0;

    format_video_sync = (s->oformat->flags & AVFMT_VARIABLE_FPS) ? ((s->oformat->flags & AVFMT_NOTIMESTAMPS) ? VSYNC_PASSTHROUGH : VSYNC_VFR) : 1;

    if (format_video_sync != VSYNC_PASSTHROUGH) {
        double vdelta = sync_ipts - ost->sync_opts + duration;
        // FIXME set to 0.5 after we fix some dts/pts bugs like in avidec.c
        if (vdelta < -1.1)
            nb_frames = 0;
        else if (format_video_sync == VSYNC_VFR) {
            if (vdelta <= -0.6) {
                nb_frames = 0;
            } else if (vdelta > 0.6)
                ost->sync_opts = lrintf(sync_ipts);
        } else if (vdelta > 1.1)
            nb_frames = lrintf(vdelta);
        if (nb_frames == 0) {
            ++av->nb_frames_drop;
            av_log(NULL, AV_LOG_VERBOSE, "*** drop!\n");
        } else if (nb_frames > 1) {
            av->nb_frames_dup += nb_frames - 1;
            av_log(NULL, AV_LOG_VERBOSE, "*** %d dup!\n", nb_frames - 1);
        }
    } else
        ost->sync_opts = lrintf(sync_ipts);

    nb_frames = FFMIN(nb_frames, ost->max_frames - ost->frame_number);
    if (nb_frames <= 0)
        return TRUE;

    if ( !do_video_resample(ost, ist, in_picture, &final_picture) )
        return FALSE;

    /* duplicates frame if needed */
    for (i = 0; i < nb_frames; i++) {
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.stream_index = ost->index;

        if (s->oformat->flags & AVFMT_RAWPICTURE &&
            enc->codec->id == CODEC_ID_RAWVIDEO) {
            /* raw pictures are written as AVPicture structure to
               avoid any copies. We support temporarily the older
               method. */
            enc->coded_frame->interlaced_frame = in_picture->interlaced_frame;
            enc->coded_frame->top_field_first  = in_picture->top_field_first;
            pkt.data   = (uint8_t *)final_picture;
            pkt.size   =  sizeof(AVPicture);
            pkt.pts    = av_rescale_q(ost->sync_opts, enc->time_base, ost->st->time_base);
            pkt.flags |= AV_PKT_FLAG_KEY;

            if ( !write_frame(s, &pkt, ost, av->exit_on_error) )
                return FALSE;
        } else {
            AVFrame big_picture;

            big_picture = *final_picture;
            /* better than nothing: use input picture interlaced
               settings */
            big_picture.interlaced_frame = in_picture->interlaced_frame;
            if (ost->st->codec->flags & (CODEC_FLAG_INTERLACED_DCT|CODEC_FLAG_INTERLACED_ME)) {
                if (ost->top_field_first == -1)
                    big_picture.top_field_first = in_picture->top_field_first;
                else
                    big_picture.top_field_first = !!ost->top_field_first;
            }

            /* handles same_quant here. This is not correct because it may
             FPRINTF  not be a global option */
            big_picture.quality = quality;
            if (!enc->me_threshold)
                big_picture.pict_type = 0;
//            big_picture.pts = AV_NOPTS_VALUE;
            big_picture.pts = ost->sync_opts;
//            big_picture.pts= av_rescale(ost->sync_opts, AV_TIME_BASE*(int64_t)enc->time_base.num, enc->time_base.den);
// av_log(NULL, AV_LOG_DEBUG, "%"PRId64" -> encoder\n", ost->sync_opts);
            if (ost->forced_kf_index < ost->forced_kf_count &&
                big_picture.pts >= ost->forced_kf_pts[ost->forced_kf_index]) {
                big_picture.pict_type = AV_PICTURE_TYPE_I;
                ost->forced_kf_index++;
            }
            ret = avcodec_encode_video(enc,
                                       bit_buffer, bit_buffer_size,
                                       &big_picture);
            if (ret < 0) {
                av_log(NULL, AV_LOG_FATAL, "Video encoding failed\n");
                return FALSE;
            }

            if (ret > 0) {
                pkt.data = bit_buffer;
                pkt.size = ret;
                if (enc->coded_frame->pts != AV_NOPTS_VALUE)
                    pkt.pts = av_rescale_q(enc->coded_frame->pts, enc->time_base, ost->st->time_base);
/*av_log(NULL, AV_LOG_DEBUG, "encoder -> %"PRId64"/%"PRId64"\n",
   pkt.pts != AV_NOPTS_VALUE ? av_rescale(pkt.pts, enc->time_base.den, AV_TIME_BASE*(int64_t)enc->time_base.num) : -1,
   pkt.dts != AV_NOPTS_VALUE ? av_rescale(pkt.dts, enc->time_base.den, AV_TIME_BASE*(int64_t)enc->time_base.num) : -1);*/

                if (enc->coded_frame->key_frame)
                    pkt.flags |= AV_PKT_FLAG_KEY;
                if ( !write_frame(s, &pkt, ost, av->exit_on_error) )
                    return FALSE;
                *frame_size = ret;
                av->video_size += ret;
            }
        }
        ost->sync_opts++;
        /*
         * For video, number of frames in == number of packets out.
         * But there may be reordering, so we can't throw away frames on encoder
         * flush, we need to limit them here, before they go into encoder.
         */
        ost->frame_number++;
    }
    return TRUE;
}

static double psnr(double d)
{
    return -10.0 * log(d) / log(10.0);
}

static void libav_print_report(LibAV *av, OutputFile *output_files,
                         OutputStream *ost_table, int nb_ostreams,
                         int is_last_report, int64_t timer_start, int64_t cur_time)
{
    char buf[1024];
    OutputStream *ost;
    AVFormatContext *oc;
    int64_t total_size;
    AVCodecContext *enc;
    int frame_number, vid, i;
    double bitrate;
    int64_t pts = INT64_MAX;
    static int64_t last_time = -1;
    int hours, mins, secs, us;

    if (!print_stats && !is_last_report)
        return;

    if (!is_last_report) {
        if (last_time == -1) {
            last_time = cur_time;
            return;
        }
        if ((cur_time - last_time) < 500000)
            return;
        last_time = cur_time;
    }


    oc = output_files[0].ctx;

    total_size = avio_size(oc->pb);
    if (total_size < 0) { // FIXME improve avio_size() so it works with non seekable output too
        total_size = avio_tell(oc->pb);
        if (total_size < 0)
            total_size = 0;
    }

    buf[0] = '\0';
    vid = 0;
    for (i = 0; i < nb_ostreams; i++) {
        float q = -1;
        ost = &ost_table[i];
        enc = ost->st->codec;
        if (!ost->stream_copy && enc->coded_frame)
            q = enc->coded_frame->quality / (float)FF_QP2LAMBDA;
        if (vid && enc->codec_type == AVMEDIA_TYPE_VIDEO) {
            snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "q=%2.1f ", q);
        }
        if (!vid && enc->codec_type == AVMEDIA_TYPE_VIDEO) {
            float t = (cur_time-timer_start) / 1000000.0;

            frame_number = ost->frame_number;
            snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "frame=%5d fps=%3d q=%3.1f ",
                     frame_number, (t > 1) ? (int)(frame_number / t + 0.5) : 0, q);
            if (is_last_report)
                snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "L");
            if (enc->flags&CODEC_FLAG_PSNR) {
                int j;
                double error, error_sum = 0;
                double scale, scale_sum = 0;
                char type[3] = { 'Y','U','V' };
                snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "PSNR=");
                for (j = 0; j < 3; j++) {
                    if (is_last_report) {
                        error = enc->error[j];
                        scale = enc->width * enc->height * 255.0 * 255.0 * frame_number;
                    } else {
                        error = enc->coded_frame->error[j];
                        scale = enc->width * enc->height * 255.0 * 255.0;
                    }
                    if (j)
                        scale /= 4;
                    error_sum += error;
                    scale_sum += scale;
                    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "%c:%2.2f ", type[j], psnr(error / scale));
                }
                snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "*:%2.2f ", psnr(error_sum / scale_sum));
            }
            vid = 1;
        }
        // compute min output value
        pts = FFMIN(pts, av_rescale_q(ost->st->pts.val,  ost->st->time_base, AV_TIME_BASE_Q));
    }

    secs = pts / AV_TIME_BASE;
    us = pts % AV_TIME_BASE;
    mins = secs / 60;
    secs %= 60;
    hours = mins / 60;
    mins %= 60;

    bitrate = pts ? total_size * 8 / (pts / 1000.0) : 0;

    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
             "size=%8.0fkB time=", total_size / 1024.0);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
             "%02d:%02d:%02d.%02d ", hours, mins, secs,
             (100 * us) / AV_TIME_BASE);
    snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf),
             "bitrate=%6.1fkbits/s", bitrate);

    if (av->nb_frames_dup || av->nb_frames_drop)
        snprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), " dup=%d drop=%d",
                av->nb_frames_dup, av->nb_frames_drop);

    av_log(NULL, AV_LOG_INFO, "%s    \r", buf);

    fflush(stderr);

    if (is_last_report) {
        int64_t raw = av->audio_size + av->video_size + av->extra_size;
        av_log(NULL, AV_LOG_INFO, "\n");
        av_log(NULL, AV_LOG_INFO, "video:%1.0fkB audio:%1.0fkB global headers:%1.0fkB muxing overhead %f%%\n",
               av->video_size / 1024.0,
               av->audio_size / 1024.0,
               av->extra_size / 1024.0,
               100.0 * (total_size - raw) / raw
        );
        if(av->video_size + av->audio_size + av->extra_size == 0){
            av_log(NULL, AV_LOG_WARNING, "Output file is empty, nothing was encoded (check -ss / -t / -frames parameters if used)\n");
        }
    }
}

static int libav_flush_encoders(LibAV *av, OutputStream *ost_table, int nb_ostreams)
{
    int i, ret;

    for (i = 0; i < nb_ostreams; i++) {
        OutputStream   *ost = &ost_table[i];
        AVCodecContext *enc = ost->st->codec;
        AVFormatContext *os = av->output_files[ost->file_index].ctx;
        int stop_encoding = 0;

        if (!ost->encoding_needed)
            continue;

        if (ost->st->codec->codec_type == AVMEDIA_TYPE_AUDIO && enc->frame_size <= 1)
            continue;
        if (ost->st->codec->codec_type == AVMEDIA_TYPE_VIDEO && (os->oformat->flags & AVFMT_RAWPICTURE) && enc->codec->id == CODEC_ID_RAWVIDEO)
            continue;

        for (;;) {
            AVPacket pkt;
            int fifo_bytes;
            av_init_packet(&pkt);
            pkt.data = NULL;
            pkt.size = 0;

            switch (ost->st->codec->codec_type) {
            case AVMEDIA_TYPE_AUDIO:
                fifo_bytes = av_fifo_size(ost->fifo);
                if (fifo_bytes > 0) {
                    /* encode any samples remaining in fifo */
                    int frame_bytes = fifo_bytes;

                    av_fifo_generic_read(ost->fifo, av->audio_buf, fifo_bytes, NULL);

                    /* pad last frame with silence if needed */
                    if (!(enc->codec->capabilities & CODEC_CAP_SMALL_LAST_FRAME)) {
                        frame_bytes = enc->frame_size * enc->channels *
                                      av_get_bytes_per_sample(enc->sample_fmt);
                        if (av->allocated_audio_buf_size < frame_bytes)
                            return FALSE;
                        generate_silence(av->audio_buf+fifo_bytes, enc->sample_fmt, frame_bytes - fifo_bytes);
                    }
                    if ( libav_encode_audio_frame(av, os, ost, av->audio_buf, frame_bytes) < 0 )
                        return FALSE;
                } else {
                    /* flush encoder with NULL frames until it is done
                       returning packets */
                    if ( libav_encode_audio_frame(av, os, ost, NULL, 0) == 0 ) {
                        stop_encoding = 1;
                        break;
                    }
                }
                break;
            case AVMEDIA_TYPE_VIDEO:
                ret = avcodec_encode_video(enc, bit_buffer, bit_buffer_size, NULL);
                if (ret < 0) {
                    av_log(NULL, AV_LOG_FATAL, "Video encoding failed\n");
                    return FALSE;
                }
                av->video_size += ret;
                if (enc->coded_frame && enc->coded_frame->key_frame)
                    pkt.flags |= AV_PKT_FLAG_KEY;
                if (ret <= 0) {
                    stop_encoding = 1;
                    break;
                }
                pkt.stream_index = ost->index;
                pkt.data = bit_buffer;
                pkt.size = ret;
                if (enc->coded_frame && enc->coded_frame->pts != AV_NOPTS_VALUE)
                    pkt.pts = av_rescale_q(enc->coded_frame->pts, enc->time_base, ost->st->time_base);
                if ( !write_frame(os, &pkt, ost, av->exit_on_error) )
                    return FALSE;
                break;
            default:
                stop_encoding = 1;
            }
            if (stop_encoding)
                break;
        }
    }
    return TRUE;
}

/*
 * Check whether a packet from ist should be written into ost at this time
 */
static int libav_check_output_constraints(LibAV *av, InputStream *ist, OutputStream *ost)
{
    OutputFile *of = &av->output_files[ost->file_index];
    int ist_index  = ist - av->input_streams;

    if (ost->source_index != ist_index)
        return 0;

    if (of->start_time && ist->pts < of->start_time)
        return 0;

    if (of->recording_time != INT64_MAX &&
        av_compare_ts(ist->pts, AV_TIME_BASE_Q, of->recording_time + of->start_time,
                      (AVRational){ 1, 1000000 }) >= 0) {
        ost->is_past_recording_time = 1;
        return 0;
    }

    return 1;
}

static int libav_do_streamcopy(LibAV *av, InputStream *ist, OutputStream *ost, const AVPacket *pkt)
{
    OutputFile *of = &av->output_files[ost->file_index];
    int64_t ost_tb_start_time = av_rescale_q(of->start_time, AV_TIME_BASE_Q, ost->st->time_base);
    AVPicture pict;
    AVPacket opkt;

    av_init_packet(&opkt);

    if ((!ost->frame_number && !(pkt->flags & AV_PKT_FLAG_KEY)) &&
        !ost->copy_initial_nonkeyframes)
        return TRUE;

    /* force the input stream PTS */
    if (ost->st->codec->codec_type == AVMEDIA_TYPE_AUDIO)
        av->audio_size += pkt->size;
    else if (ost->st->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
        av->video_size += pkt->size;
        ost->sync_opts++;
    }

    opkt.stream_index = ost->index;
    if (pkt->pts != AV_NOPTS_VALUE)
        opkt.pts = av_rescale_q(pkt->pts, ist->st->time_base, ost->st->time_base) - ost_tb_start_time;
    else
        opkt.pts = AV_NOPTS_VALUE;

    if (pkt->dts == AV_NOPTS_VALUE)
        opkt.dts = av_rescale_q(ist->pts, AV_TIME_BASE_Q, ost->st->time_base);
    else
        opkt.dts = av_rescale_q(pkt->dts, ist->st->time_base, ost->st->time_base);
    opkt.dts -= ost_tb_start_time;

    opkt.duration = av_rescale_q(pkt->duration, ist->st->time_base, ost->st->time_base);
    opkt.flags    = pkt->flags;

    // FIXME remove the following 2 lines they shall be replaced by the bitstream filters
    if (  ost->st->codec->codec_id != CODEC_ID_H264
       && ost->st->codec->codec_id != CODEC_ID_MPEG1VIDEO
       && ost->st->codec->codec_id != CODEC_ID_MPEG2VIDEO
       ) {
        if (av_parser_change(ist->st->parser, ost->st->codec, &opkt.data, &opkt.size, pkt->data, pkt->size, pkt->flags & AV_PKT_FLAG_KEY))
            opkt.destruct = av_destruct_packet;
    } else {
        opkt.data = pkt->data;
        opkt.size = pkt->size;
    }
    if (of->ctx->oformat->flags & AVFMT_RAWPICTURE) {
        /* store AVPicture in AVPacket, as expected by the output format */
        avpicture_fill(&pict, opkt.data, ost->st->codec->pix_fmt, ost->st->codec->width, ost->st->codec->height);
        opkt.data = (uint8_t *)&pict;
        opkt.size = sizeof(AVPicture);
        opkt.flags |= AV_PKT_FLAG_KEY;
    }

    if ( !write_frame(of->ctx, &opkt, ost, av->exit_on_error) )
        return FALSE;
    ost->st->codec->frame_number++;
    av_free_packet(&opkt);
    return TRUE;
}

static void libav_rate_emu_sleep(LibAV *av, InputStream *ist)
{
    if (av->input_files[ist->file_index].rate_emu) {
        int64_t pts = av_rescale(ist->pts, 1000000, AV_TIME_BASE);
        int64_t now = av_gettime() - ist->start;
        if (pts > now)
            usleep(pts - now);
    }
}

static int libav_transcode_audio(LibAV *av, InputStream *ist, AVPacket *pkt, int *got_output)
{
    AVFrame *decoded_frame;
    AVCodecContext *avctx = ist->st->codec;
    int i, ret;

    if (!ist->decoded_frame && !(ist->decoded_frame = avcodec_alloc_frame()))
        return AVERROR(ENOMEM);
    else
        avcodec_get_frame_defaults(ist->decoded_frame);
    decoded_frame = ist->decoded_frame;

    ret = avcodec_decode_audio4(avctx, decoded_frame, got_output, pkt);
    if (ret < 0) {
        return ret;
    }

    if (!*got_output) {
        /* no audio frame */
        return ret;
    }

    /* if the decoder provides a pts, use it instead of the last packet pts.
       the decoder could be delaying output by a packet or more. */
    if (decoded_frame->pts != AV_NOPTS_VALUE)
        ist->next_pts = decoded_frame->pts;

    /* increment next_pts to use for the case where the input stream does not
       have timestamps or there are multiple frames in the packet */
    ist->next_pts += ((int64_t)AV_TIME_BASE * decoded_frame->nb_samples) /
                     avctx->sample_rate;

    libav_rate_emu_sleep(av, ist);

    for (i = 0; i < av->nb_output_streams; i++) {
        OutputStream *ost = &av->output_streams[i];

        if (!libav_check_output_constraints(av, ist, ost) || !ost->encoding_needed)
            continue;
        if ( !libav_do_audio_out(av, av->output_files[ost->file_index].ctx, ost, ist, decoded_frame) ) {
            ret = -1;
            break;
        }
    }

    return ret;
}
#ifdef USE_AVFILTER
static void filter_release_buffer(AVFilterBuffer *fb)
{
    FrameBuffer *buf = fb->priv;
    av_free(fb);
    unref_buffer(buf->ist, buf);
}
#ifdef USE_LIBAV
int get_filtered_video_frame(AVFilterContext *ctx, AVFrame *frame,
                             AVFilterBufferRef **picref_ptr, AVRational *tb)
{
    int ret;
    AVFilterBufferRef *picref;

    if ((ret = avfilter_request_frame(ctx->inputs[0])) < 0)
        return ret;
    if (!(picref = ctx->inputs[0]->cur_buf))
        return AVERROR(ENOENT);
    *picref_ptr = picref;
    ctx->inputs[0]->cur_buf = NULL;
    *tb = ctx->inputs[0]->time_base;

    memcpy(frame->data,     picref->data,     sizeof(frame->data));
    memcpy(frame->linesize, picref->linesize, sizeof(frame->linesize));
    frame->interlaced_frame    = picref->video->interlaced;
    frame->top_field_first     = picref->video->top_field_first;
    frame->key_frame           = picref->video->key_frame;
    frame->pict_type           = picref->video->pict_type;
    frame->sample_aspect_ratio = picref->video->pixel_aspect;

    return 1;
}
#endif
#endif
static int libav_transcode_video(LibAV *av, InputStream *ist, AVPacket *pkt, int *got_output, int64_t *pkt_pts, int64_t *pkt_dts)
{
    AVFrame *decoded_frame, *filtered_frame = NULL;
    int i, ret = 0;
    int duration=0;
    //int64_t *best_effort_timestamp;

#if USE_AVFILTER
    int frame_available = 1;
    //!!!AVRational *frame_sample_aspect;
#endif

    if (!ist->decoded_frame && !(ist->decoded_frame = avcodec_alloc_frame()))
        return AVERROR(ENOMEM);
    else
        avcodec_get_frame_defaults(ist->decoded_frame);
    decoded_frame = ist->decoded_frame;
    pkt->pts  = *pkt_pts;
    pkt->dts  = *pkt_dts;
    *pkt_pts  = AV_NOPTS_VALUE;

    //fprintf(stderr, "!!!! %d\n", AV_TIME_BASE_Q);
    if (pkt->duration) {
        duration = av_rescale_q(pkt->duration, ist->st->time_base, AV_TIME_BASE_Q);
    } else if(ist->st->codec->time_base.num != 0) {
        int ticks= ist->st->parser ? ist->st->parser->repeat_pict+1 : ist->st->codec->ticks_per_frame;
        duration = ((int64_t)AV_TIME_BASE *
                          ist->st->codec->time_base.num * ticks) /
                          ist->st->codec->time_base.den;
    }

    if(*pkt_dts != AV_NOPTS_VALUE && duration) {
        *pkt_dts += duration;
    }else
        *pkt_dts = AV_NOPTS_VALUE;

    ret = avcodec_decode_video2(ist->st->codec,
                                decoded_frame, got_output, pkt);
    if (ret < 0)
        return ret;

    if (!*got_output) {
        /* no picture yet */
        return ret;
    }


    ist->next_pts += duration;
    pkt->size = 0;

#if USE_AVFILTER
    //!!!frame_sample_aspect = av_opt_ptr(avcodec_get_frame_class(), decoded_frame, "sample_aspect_ratio");
    for( i=0; i< av->nb_output_streams; i++) {
        OutputStream *ost = ost = &av->output_streams[i];
        if ( libav_check_output_constraints(av, ist, ost) && ost->encoding_needed ) {
            //!!!if (!frame_sample_aspect->num)
                //!!!*frame_sample_aspect = ist->st->sample_aspect_ratio;
            decoded_frame->pts = ist->pts;
            if (ist->dr1 && decoded_frame->type==FF_BUFFER_TYPE_USER) {
                FrameBuffer      *buf = decoded_frame->opaque;
                AVFilterBufferRef *fb = avfilter_get_video_buffer_ref_from_arrays(
                                            decoded_frame->data, decoded_frame->linesize,
                                            AV_PERM_READ | AV_PERM_PRESERVE,
                                            ist->st->codec->width, ist->st->codec->height,
                                            ist->st->codec->pix_fmt);

                avfilter_copy_frame_props(fb, decoded_frame);
                fb->pts                 = ist->pts;
                fb->buf->priv           = buf;
                fb->buf->free           = filter_release_buffer;

                buf->refcount++;
                av_buffersrc_buffer(ost->input_video_filter, fb);
            } else {
#ifdef USE_LIBAV
                AVRational sar;
                if (ist->st->sample_aspect_ratio.num)
                    sar = ist->st->sample_aspect_ratio;
                else
                    sar = ist->st->codec->sample_aspect_ratio;

                if((av_vsrc_buffer_add_frame(ost->input_video_filter, decoded_frame, ist->pts, sar)) < 0){
                    av_log(0, AV_LOG_FATAL, "Failed to inject frame into filter network\n");
                    ret = -1;
                    break;
                }
#else
                if((av_vsrc_buffer_add_frame(ost->input_video_filter, decoded_frame, AV_VSRC_BUF_FLAG_OVERWRITE)) < 0){
                    av_log(0, AV_LOG_FATAL, "Failed to inject frame into filter network\n");
                    ret = -1;
                    break;
                }
#endif
            }
        }
    }
#endif
    libav_rate_emu_sleep(av, ist);

    for (i = 0; i < av->nb_output_streams; i++) {
        OutputStream *ost = &av->output_streams[i];
        int frame_size;

        if (!libav_check_output_constraints(av, ist, ost) || !ost->encoding_needed)
            continue;

#if USE_AVFILTER
        if (ost->input_video_filter) {
#ifdef USE_LIBAV
            frame_available = avfilter_poll_frame(ost->output_video_filter->inputs[0]);
#else
            frame_available = av_buffersink_poll_frame(ost->output_video_filter);
#endif
        }
        while (frame_available) {
            if (ost->output_video_filter) {
                AVRational ist_pts_tb;
#ifdef USE_LIBAV
            if (!ist->filtered_frame && !(ist->filtered_frame = avcodec_alloc_frame())) {
                //!!av_free(buffer_to_free);
                return AVERROR(ENOMEM);
            } else
                avcodec_get_frame_defaults(ist->filtered_frame);
            filtered_frame = ist->filtered_frame;
            if (ost->output_video_filter)
                get_filtered_video_frame(ost->output_video_filter, filtered_frame, &ost->picref,  &ist_pts_tb);
            if (ost->picref)
                ist->pts = av_rescale_q(ost->picref->pts, ist_pts_tb, AV_TIME_BASE_Q);
            if (ost->picref->video && !ost->frame_aspect_ratio)
                ost->st->codec->sample_aspect_ratio = ost->picref->video->pixel_aspect;
#else
                ist_pts_tb = ost->output_video_filter->inputs[0]->time_base;
                if (av_buffersink_get_buffer_ref(ost->output_video_filter, &ost->picref, 0) < 0){
                    av_log(0, AV_LOG_WARNING, "AV Filter told us it has a frame available but failed to output one\n");
                    goto cont;
                }
                if (!ist->filtered_frame && !(ist->filtered_frame = avcodec_alloc_frame())) {
                    //!!av_free(buffer_to_free);
                    return AVERROR(ENOMEM);
                } else
                    avcodec_get_frame_defaults(ist->filtered_frame);
                filtered_frame = ist->filtered_frame;
                *filtered_frame= *decoded_frame; //for me_threshold
                if (ost->picref) {
                    avfilter_fill_frame_from_video_buffer_ref(filtered_frame, ost->picref);
                    ist->pts = av_rescale_q(ost->picref->pts, ist_pts_tb, AV_TIME_BASE_Q);
                }
#endif
            }
            if (ost->picref->video && !ost->frame_aspect_ratio) {
#ifdef USE_LIBAV
                ost->st->codec->sample_aspect_ratio = ost->picref->video->pixel_aspect;
#else
                ost->st->codec->sample_aspect_ratio = ost->picref->video->sample_aspect_ratio;
#endif
            }
#else
            filtered_frame = decoded_frame;
#endif
            if ( !libav_do_video_out(av, av->output_files[ost->file_index].ctx, ost, ist, filtered_frame, &frame_size,
                                     ost->st->codec->global_quality) ) {
                ret = -1;
                break;
            }
#if USE_AVFILTER
#ifdef USE_LIBAV
            frame_available = ost->output_video_filter && avfilter_poll_frame(ost->output_video_filter->inputs[0]);
#else
            cont:
            frame_available = ost->output_video_filter && av_buffersink_poll_frame(ost->output_video_filter);
#endif
            avfilter_unref_buffer(ost->picref);
        }
#endif

    }

    return ret;
}

/* pkt = NULL means EOF (needed to flush decoder buffers) */
static int libav_output_packet(LibAV *av, InputStream *ist,
                         OutputStream *ost_table, int nb_ostreams,
                         const AVPacket *pkt)
{
    int ret = 0, i;
    int got_output;
    int64_t pkt_dts = AV_NOPTS_VALUE;
    int64_t pkt_pts = AV_NOPTS_VALUE;

    AVPacket avpkt;

    if (ist->next_pts == AV_NOPTS_VALUE)
        ist->next_pts = ist->pts;

    if (pkt == NULL) {
        /* EOF handling */
        av_init_packet(&avpkt);
        avpkt.data = NULL;
        avpkt.size = 0;
        goto handle_eof;
    } else {
        avpkt = *pkt;
    }

    if (pkt->dts != AV_NOPTS_VALUE) {
        if (ist->st->codec->codec_type != AVMEDIA_TYPE_VIDEO || !ist->decoding_needed)
            ist->next_pts = ist->pts = av_rescale_q(pkt->dts, ist->st->time_base, AV_TIME_BASE_Q);
        pkt_dts = av_rescale_q(pkt->dts, ist->st->time_base, AV_TIME_BASE_Q);
    }
    if(pkt->pts != AV_NOPTS_VALUE)
        pkt_pts = av_rescale_q(pkt->pts, ist->st->time_base, AV_TIME_BASE_Q);

    // while we have more to decode or while the decoder did output something on EOF
    while (ist->decoding_needed && (avpkt.size > 0 || (!pkt && got_output))) {
    handle_eof:

        ist->pts = ist->next_pts;

        if (avpkt.size && avpkt.size != pkt->size) {
            av_log(NULL, ist->showed_multi_packet_warning ? AV_LOG_VERBOSE : AV_LOG_WARNING,
                   "Multiple frames in a packet from stream %d\n", pkt->stream_index);
            ist->showed_multi_packet_warning = 1;
        }

        switch (ist->st->codec->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            ret = libav_transcode_audio(av, ist, &avpkt, &got_output);
            break;
        case AVMEDIA_TYPE_VIDEO:
            ret = libav_transcode_video(av, ist, &avpkt, &got_output, &pkt_pts, &pkt_dts);
            break;
        default:
            return -1;
        }

        if (ret < 0)
            return ret;

        avpkt.dts=
        avpkt.pts= AV_NOPTS_VALUE;

        // touch data and size only if not EOF
        if (pkt) {
            if(ist->st->codec->codec_type != AVMEDIA_TYPE_AUDIO)
                ret = avpkt.size;
            avpkt.data += ret;
            avpkt.size -= ret;
        }
        if (!got_output) {
            continue;
        }
    }

    /* handle stream copy */
    if (!ist->decoding_needed) {
        libav_rate_emu_sleep(av, ist);
        ist->pts = ist->next_pts;
        switch (ist->st->codec->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            ist->next_pts += ((int64_t)AV_TIME_BASE * ist->st->codec->frame_size) /
                             ist->st->codec->sample_rate;
            break;
        case AVMEDIA_TYPE_VIDEO:
            if (pkt->duration) {
                ist->next_pts += av_rescale_q(pkt->duration, ist->st->time_base, AV_TIME_BASE_Q);
            } else if(ist->st->codec->time_base.num != 0) {
                int ticks= ist->st->parser ? ist->st->parser->repeat_pict + 1 : ist->st->codec->ticks_per_frame;
                ist->next_pts += ((int64_t)AV_TIME_BASE *
                                  ist->st->codec->time_base.num * ticks) /
                                  ist->st->codec->time_base.den;
            }
            break;
        default:
            break;
        }
    }
    for (i = 0; pkt && i < nb_ostreams; i++) {
        OutputStream *ost = &ost_table[i];

        if (!libav_check_output_constraints(av, ist, ost) || ost->encoding_needed)
            continue;

        if ( !libav_do_streamcopy(av, ist, ost, pkt) )
            return -1;
    }

    return 0;
}

static int libav_init_input_stream(LibAV *av, int ist_index, OutputStream *streams, int nb_streams,
                             char *error, int error_len)
{
    InputStream *ist = &av->input_streams[ist_index];
    if (ist->decoding_needed) {
        AVCodec *codec = ist->dec;
        if (!codec) {
            snprintf(error, error_len, "Decoder (codec ) not found for input stream #%d:%d",
                    /*avcodec_get_name(ist->st->codec->codec_id),*/ ist->file_index, ist->st->index);
            return AVERROR(EINVAL);
        }

        ist->dr1 = codec->capabilities & CODEC_CAP_DR1;
        if (codec->type == AVMEDIA_TYPE_VIDEO && ist->dr1) {
            ist->st->codec->get_buffer     = codec_get_buffer;
            ist->st->codec->release_buffer = codec_release_buffer;
            ist->st->codec->opaque         = ist;
        }

        if (!av_dict_get(ist->opts, "threads", NULL, 0))
            av_dict_set(&ist->opts, "threads", "auto", 0);
        if (avcodec_open2(ist->st->codec, codec, &ist->opts) < 0) {
            snprintf(error, error_len, "Error while opening decoder for input stream #%d:%d",
                    ist->file_index, ist->st->index);
            return AVERROR(EINVAL);
        }
        if ( !assert_codec_experimental_(ist->st->codec, 0) )
            return 1;
        if ( !assert_avoptions_(ist->opts) )
            return 1;
    }

    ist->pts = ist->st->avg_frame_rate.num ? - ist->st->codec->has_b_frames * AV_TIME_BASE / av_q2d(ist->st->avg_frame_rate) : 0;
    ist->next_pts = AV_NOPTS_VALUE;
    ist->is_start = 1;

    return 0;
}

static int libav_open_output_file(LibAV *av)
{
    int ret = 0;
    int i;
    AVFormatContext *oc;

    /* open files and write file headers */
    for (i = 0; i < av->nb_output_files; i++) {
        oc = av->output_files[i].ctx;
        //oc->interrupt_callback = int_cb;
        if (avformat_write_header(oc, &av->output_files[i].opts) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Could not write header for output file #%d (incorrect codec parameters ?)\n", i);
            ret = AVERROR(EINVAL);
            break;
        }
        /*if (strcmp(oc->oformat->name, "rtp")) {
            want_sdp = 0;
        }*/
    }

    return ret;
}

static int libav_transcode_init(LibAV *av)
{
    int ret = 0, i, j, k;
    AVFormatContext *oc;
    AVCodecContext *codec, *icodec;
    OutputStream *ost;
    InputStream *ist;
    char error[1024];
    //int want_sdp = 1;

    /* init framerate emulation */
    for (i = 0; i < av->nb_input_files; i++) {
        InputFile *ifile = &av->input_files[i];
        if (ifile->rate_emu)
            for (j = 0; j < ifile->nb_streams; j++)
                av->input_streams[j + ifile->ist_index].start = av_gettime();
    }

    /* output stream init */
    for (i = 0; i < av->nb_output_files; i++) {
        oc = av->output_files[i].ctx;
        if (!oc->nb_streams && !(oc->oformat->flags & AVFMT_NOSTREAMS)) {
            av_dump_format(oc, i, oc->filename, 1);
            av_log(NULL, AV_LOG_ERROR, "Output file #%d does not contain any stream\n", i);
            return AVERROR(EINVAL);
        }
    }

    /* for each output stream, we compute the right encoding parameters */
    for (i = 0; i < av->nb_output_streams; i++) {
        ost = &av->output_streams[i];
        oc  = av->output_files[ost->file_index].ctx;
        ist = &av->input_streams[ost->source_index];

        if (ost->attachment_filename)
            continue;

        codec  = ost->st->codec;
        icodec = ist->st->codec;

        ost->st->disposition          = ist->st->disposition;
        codec->bits_per_raw_sample    = icodec->bits_per_raw_sample;
        codec->chroma_sample_location = icodec->chroma_sample_location;

        if (ost->stream_copy) {
            uint64_t extra_size = (uint64_t)icodec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE;

            if (extra_size > INT_MAX) {
                return AVERROR(EINVAL);
            }

            /* if stream_copy is selected, no need to decode or encode */
            codec->codec_id   = icodec->codec_id;
            codec->codec_type = icodec->codec_type;

            if (!codec->codec_tag) {
                if (!oc->oformat->codec_tag ||
                     av_codec_get_id (oc->oformat->codec_tag, icodec->codec_tag) == codec->codec_id ||
                     av_codec_get_tag(oc->oformat->codec_tag, icodec->codec_id) <= 0)
                    codec->codec_tag = icodec->codec_tag;
            }

            codec->bit_rate       = icodec->bit_rate;
            codec->rc_max_rate    = icodec->rc_max_rate;
            codec->rc_buffer_size = icodec->rc_buffer_size;
            codec->field_order    = icodec->field_order;
            codec->extradata      = av_mallocz(extra_size);
            if (!codec->extradata) {
                return AVERROR(ENOMEM);
            }
            memcpy(codec->extradata, icodec->extradata, icodec->extradata_size);
            codec->extradata_size= icodec->extradata_size;

            codec->time_base = ist->st->time_base;
            if(!strcmp(oc->oformat->name, "avi")) {
                if ( av_q2d(icodec->time_base)*icodec->ticks_per_frame > 2*av_q2d(ist->st->time_base)
                    && av_q2d(ist->st->time_base) < 1.0/500 ) {
                    codec->time_base = icodec->time_base;
                    codec->time_base.num *= icodec->ticks_per_frame;
                    codec->time_base.den *= 2;
                }
            } else if(!(oc->oformat->flags & AVFMT_VARIABLE_FPS)
                      && strcmp(oc->oformat->name, "mov") && strcmp(oc->oformat->name, "mp4") && strcmp(oc->oformat->name, "3gp")
                      && strcmp(oc->oformat->name, "3g2") && strcmp(oc->oformat->name, "psp") && strcmp(oc->oformat->name, "ipod")
            ) {
                if( av_q2d(icodec->time_base)*icodec->ticks_per_frame > av_q2d(ist->st->time_base)
                    && av_q2d(ist->st->time_base) < 1.0/500 ) {
                    codec->time_base = icodec->time_base;
                    codec->time_base.num *= icodec->ticks_per_frame;
                }
            }
            av_reduce(&codec->time_base.num, &codec->time_base.den,
                        codec->time_base.num, codec->time_base.den, INT_MAX);

            switch (codec->codec_type) {
            case AVMEDIA_TYPE_AUDIO:
                codec->channel_layout     = icodec->channel_layout;
                codec->sample_rate        = icodec->sample_rate;
                codec->channels           = icodec->channels;
                codec->frame_size         = icodec->frame_size;
                codec->audio_service_type = icodec->audio_service_type;
                codec->block_align        = icodec->block_align;
                break;
            case AVMEDIA_TYPE_VIDEO:
                codec->pix_fmt            = icodec->pix_fmt;
                codec->width              = icodec->width;
                codec->height             = icodec->height;
                codec->has_b_frames       = icodec->has_b_frames;
                if (!codec->sample_aspect_ratio.num) {
                    codec->sample_aspect_ratio   =
                    ost->st->sample_aspect_ratio =
                        ist->st->sample_aspect_ratio.num ? ist->st->sample_aspect_ratio :
                        ist->st->codec->sample_aspect_ratio.num ?
                        ist->st->codec->sample_aspect_ratio : (AVRational){0, 1};
                }
                ost->st->avg_frame_rate = ist->st->avg_frame_rate;
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                codec->width  = icodec->width;
                codec->height = icodec->height;
                break;
            case AVMEDIA_TYPE_DATA:
            case AVMEDIA_TYPE_ATTACHMENT:
                break;
            default:
                abort();
            }
        } else {
            if (!ost->enc)
                ost->enc = avcodec_find_encoder(ost->st->codec->codec_id);

            ist->decoding_needed = 1;
            ost->encoding_needed = 1;

            switch (codec->codec_type) {
            case AVMEDIA_TYPE_AUDIO:
                ost->fifo = av_fifo_alloc(1024);
                if (!ost->fifo) {
                    return AVERROR(ENOMEM);
                }
                if (!codec->sample_rate)
                    codec->sample_rate = icodec->sample_rate;
                choose_sample_rate(ost->st, ost->enc);
                codec->time_base = (AVRational){ 1, codec->sample_rate };

                if (codec->sample_fmt == AV_SAMPLE_FMT_NONE)
                    codec->sample_fmt = icodec->sample_fmt;
                choose_sample_fmt(ost->st, ost->enc);

                if (!codec->channels) {
                    codec->channels = icodec->channels;
                    codec->channel_layout = icodec->channel_layout;
                }
                if (av_get_channel_layout_nb_channels(codec->channel_layout) != codec->channels)
                    codec->channel_layout = 0;

                break;
            case AVMEDIA_TYPE_VIDEO:
                if (codec->pix_fmt == PIX_FMT_NONE)
                    codec->pix_fmt = icodec->pix_fmt;
                choose_pixel_fmt(ost->st, ost->enc);

                if (ost->st->codec->pix_fmt == PIX_FMT_NONE) {
                    av_log(NULL, AV_LOG_FATAL, "Video pixel format is unknown, stream cannot be encoded\n");
                    return 1;
                }

                if (!codec->width || !codec->height) {
                    codec->width  = icodec->width;
                    codec->height = icodec->height;
                }

                ost->video_resample = codec->width   != icodec->width  ||
                                      codec->height  != icodec->height ||
                                      codec->pix_fmt != icodec->pix_fmt;
                if (ost->video_resample) {
                    codec->bits_per_raw_sample = 0;
                }

                ost->resample_height  = icodec->height;
                ost->resample_width   = icodec->width;
                ost->resample_pix_fmt = icodec->pix_fmt;

                if (!ost->frame_rate.num)
                    ost->frame_rate = ist->st->r_frame_rate.num ? ist->st->r_frame_rate : (AVRational) { 25, 1 };
                if (ost->enc && ost->enc->supported_framerates && !ost->force_fps) {
                    int idx = av_find_nearest_q_idx(ost->frame_rate, ost->enc->supported_framerates);
                    ost->frame_rate = ost->enc->supported_framerates[idx];
                }
                codec->time_base = (AVRational){ost->frame_rate.den, ost->frame_rate.num};

                if (   av_q2d(codec->time_base) < 0.001 && !(oc->oformat->flags & AVFMT_VARIABLE_FPS) ){

                    av_log(oc, AV_LOG_WARNING, "Frame rate very high for a muxer not effciciently supporting it.\n"
                                               "Please consider specifiying a lower framerate, a different muxer or -vsync 2\n");
                }
                for (j = 0; j < ost->forced_kf_count; j++)
                    ost->forced_kf_pts[j] = av_rescale_q(ost->forced_kf_pts[j],
                                                         AV_TIME_BASE_Q,
                                                         codec->time_base);

#if USE_AVFILTER
                if (configure_video_filters(ist, ost)) {
                    av_log(NULL, AV_LOG_FATAL, "Error opening filters!");
                    return -1;
                }
#endif
                break;
            case AVMEDIA_TYPE_SUBTITLE:
                break;
            default:
                abort();
                break;
            }
        }
        if (codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            /* maximum video buffer size is 8-bytes per pixel, plus DPX header size (1664)*/
            int size        = codec->width * codec->height;
            bit_buffer_size = FFMAX(bit_buffer_size, 9*size + 10000);
        }
    }

    if (!bit_buffer)
        bit_buffer = av_malloc(bit_buffer_size);
    if (!bit_buffer) {
        av_log(NULL, AV_LOG_ERROR, "Cannot allocate %d bytes output buffer\n",
               bit_buffer_size);
        return AVERROR(ENOMEM);
    }

    /* open each encoder */
    for (i = 0; i < av->nb_output_streams; i++) {
        ost = &av->output_streams[i];
        if (ost->encoding_needed) {
            AVCodec      *codec = ost->enc;
            if (!codec) {
                snprintf(error, sizeof(error), "Encoder (codec) not found for output stream #%d:%d",
                         /*avcodec_get_name(ost->st->codec->codec_id),*/ ost->file_index, ost->index);
                ret = AVERROR(EINVAL);
                goto dump_format;
            }
            if (!av_dict_get(ost->opts, "threads", NULL, 0))
                av_dict_set(&ost->opts, "threads", "auto", 0);
            if (avcodec_open2(ost->st->codec, codec, &ost->opts) < 0) {
                snprintf(error, sizeof(error), "Error while opening encoder for output stream #%d:%d - maybe incorrect parameters such as bit_rate, rate, width or height",
                        ost->file_index, ost->index);
                ret = AVERROR(EINVAL);
                goto dump_format;
            }
            if ( !assert_codec_experimental_(ost->st->codec, 1) )
                return 1;
            if ( !assert_avoptions_(ost->opts) )
                return 1;
            if (ost->st->codec->bit_rate && ost->st->codec->bit_rate < 1000)
                av_log(NULL, AV_LOG_WARNING, "The bitrate parameter is set too low."
                                             " It takes bits/s as argument, not kbits/s\n");
            av->extra_size += ost->st->codec->extradata_size;

            if (ost->st->codec->me_threshold)
                av->input_streams[ost->source_index].st->codec->debug |= FF_DEBUG_MV;
        }
    }

    /* init input streams */
    for (i = 0; i < av->nb_input_streams; i++)
        if ((ret = libav_init_input_stream(av, i, av->output_streams, av->nb_output_streams, error, sizeof(error))) < 0)
            goto dump_format;

    /* discard unused programs */
    for (i = 0; i < av->nb_input_files; i++) {
        InputFile *ifile = &av->input_files[i];
        for (j = 0; j < ifile->ctx->nb_programs; j++) {
            AVProgram *p = ifile->ctx->programs[j];
            int discard  = AVDISCARD_ALL;

            for (k = 0; k < p->nb_stream_indexes; k++)
                if (!av->input_streams[ifile->ist_index + p->stream_index[k]].discard) {
                    discard = AVDISCARD_DEFAULT;
                    break;
                }
            p->discard = discard;
        }
    }

    ret = libav_open_output_file(av);

 dump_format:
    /* dump the file output parameters - cannot be done before in case
       of stream copy */
    for (i = 0; i < av->nb_output_files; i++) {
        av_dump_format(av->output_files[i].ctx, i, av->output_files[i].ctx->filename, 1);
    }

    /* dump the stream mapping */
    av_log(NULL, AV_LOG_INFO, "Stream mapping:\n");
    for (i = 0; i < av->nb_output_streams; i++) {
        ost = &av->output_streams[i];

        if (ost->attachment_filename) {
            /* an attached file */
            av_log(NULL, AV_LOG_INFO, "  File %s -> Stream #%d:%d\n",
                   ost->attachment_filename, ost->file_index, ost->index);
            continue;
        }
        av_log(NULL, AV_LOG_INFO, "  Stream #%d:%d -> #%d:%d",
               av->input_streams[ost->source_index].file_index,
               av->input_streams[ost->source_index].st->index,
               ost->file_index,
               ost->index);
        if (ost->sync_ist != &av->input_streams[ost->source_index])
            av_log(NULL, AV_LOG_INFO, " [sync #%d:%d]",
                   ost->sync_ist->file_index,
                   ost->sync_ist->st->index);
        if (ost->stream_copy)
            av_log(NULL, AV_LOG_INFO, " (copy)");
        else
            av_log(NULL, AV_LOG_INFO, " (%s -> %s)", av->input_streams[ost->source_index].dec ?
                   av->input_streams[ost->source_index].dec->name : "?",
                   ost->enc ? ost->enc->name : "?");
        av_log(NULL, AV_LOG_INFO, "\n");
    }

    if ( ret ) {
        av_log(NULL, AV_LOG_ERROR, "%s\n", error);
        return ret;
    }

    return 0;
}

/*
 * The following code is the main loop of the file converter
 */
static int libav_transcode(LibAV *av)
{
    int ret, i;
    AVFormatContext *is, *os;
    OutputStream *ost;
    InputStream *ist;
    uint8_t *no_packet;
    int no_packet_count = 0;
    int64_t timer_start;

    if (!(no_packet = av_mallocz(av->nb_input_files)))
        return -1;

    ret = libav_transcode_init(av);
    if (ret < 0)
        goto fail;

    timer_start = av_gettime();
    ret = 0;
    for ( ; ; ) {
        int file_index, ist_index;
        AVPacket pkt;
        int64_t ipts_min;
        double opts_min;
        int64_t cur_time= av_gettime();

        ipts_min = INT64_MAX;
        opts_min = 1e100;
//        av_log(NULL, AV_LOG_DEBUG, "frame");

        /* select the stream that we must read now by looking at the
           smallest output pts */
        file_index = -1;
        for (i = 0; i < av->nb_output_streams; i++) {
            OutputFile *of;
            int64_t ipts;
            double  opts;
            uint64_t pos = 0;
            ost = &av->output_streams[i];
            of = &av->output_files[ost->file_index];
            os = av->output_files[ost->file_index].ctx;
            ist = &av->input_streams[ost->source_index];
            if ( os->pb ) {
                pos = avio_tell(os->pb);
            }
            if (ost->is_past_recording_time || no_packet[ist->file_index] ||
               (pos >= of->limit_filesize) ) {

                if ( (pos >= of->limit_filesize) )
                    continue;
                else
                    continue;
            }
            opts = ost->st->pts.val * av_q2d(ost->st->time_base);
            ipts = ist->pts;
            if (!av->input_files[ist->file_index].eof_reached) {
                if (ipts < ipts_min) {
                    ipts_min = ipts;
                    if (input_sync)
                        file_index = ist->file_index;
                }
                if (opts < opts_min) {
                    opts_min = opts;
                    if (!input_sync) file_index = ist->file_index;
                }
            }
            if (ost->frame_number >= ost->max_frames) {
                int j;
                for (j = 0; j < of->ctx->nb_streams; j++)
                    av->output_streams[of->ost_index + j].is_past_recording_time = 1;
                continue;
            }
        }
        /* if none, if is finished */
        if (file_index < 0) {
            if (no_packet_count) {
                no_packet_count = 0;
                memset(no_packet, 0, av->nb_input_files);
                usleep(10000);
                continue;
            }
            break;
        }

        /* read a frame from it and output it in the fifo */
        is  = av->input_files[file_index].ctx;
        ret = av_read_frame(is, &pkt);
        if ( ret == AVERROR_INVALIDDATA ) {
            av_log(NULL, AV_LOG_INFO, "Error read frame!!!!\n");
            break;
        }
        if (ret == AVERROR(EAGAIN)) {
            no_packet[file_index] = 1;
            no_packet_count++;
            continue;
        }
        if (ret < 0) {
            av->input_files[file_index].eof_reached = 1;
            continue;
        }

        no_packet_count = 0;
        memset(no_packet, 0, av->nb_input_files);

        /* the following test is needed in case new streams appear
           dynamically in stream : we ignore them */
        if (pkt.stream_index >= av->input_files[file_index].nb_streams)
            goto discard_packet;
        ist_index = av->input_files[file_index].ist_index + pkt.stream_index;
        ist = &av->input_streams[ist_index];
        if (ist->discard)
            goto discard_packet;

        if (pkt.dts != AV_NOPTS_VALUE)
            pkt.dts += av_rescale_q(av->input_files[ist->file_index].ts_offset, AV_TIME_BASE_Q, ist->st->time_base);
        if (pkt.pts != AV_NOPTS_VALUE)
            pkt.pts += av_rescale_q(av->input_files[ist->file_index].ts_offset, AV_TIME_BASE_Q, ist->st->time_base);

        if (pkt.pts != AV_NOPTS_VALUE)
            pkt.pts *= ist->ts_scale;
        if (pkt.dts != AV_NOPTS_VALUE)
            pkt.dts *= ist->ts_scale;

        if (pkt.dts != AV_NOPTS_VALUE && ist->next_pts != AV_NOPTS_VALUE
            && (is->iformat->flags & AVFMT_TS_DISCONT)) {
            int64_t pkt_dts = av_rescale_q(pkt.dts, ist->st->time_base, AV_TIME_BASE_Q);
            int64_t delta   = pkt_dts - ist->next_pts;
            if((delta < -1LL*dts_delta_threshold*AV_TIME_BASE ||
                (delta > 1LL*dts_delta_threshold*AV_TIME_BASE &&
                 ist->st->codec->codec_type != AVMEDIA_TYPE_SUBTITLE) ||
                pkt_dts+1<ist->pts)) {
                av->input_files[ist->file_index].ts_offset -= delta;
                av_log(NULL, AV_LOG_DEBUG,
                       "timestamp discontinuity %"PRId64", new offset= %"PRId64"\n",
                       delta, av->input_files[ist->file_index].ts_offset);
                pkt.dts-= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
                if (pkt.pts != AV_NOPTS_VALUE)
                    pkt.pts-= av_rescale_q(delta, AV_TIME_BASE_Q, ist->st->time_base);
            }
        }

        if ( libav_output_packet(av, ist, av->output_streams, av->nb_output_streams, &pkt) < 0 ) {

            av_log(NULL, AV_LOG_ERROR, "Error while decoding stream #%d:%d\n",
                   ist->file_index, ist->st->index);
            if (av->exit_on_error)
                return -1;
            av_free_packet(&pkt);
            continue;
        }

    discard_packet:
        av_free_packet(&pkt);

        /* dump report by using the output first video and audio streams */
        if ( av->test )
            libav_print_report(av, av->output_files, av->output_streams, av->nb_output_streams, 0, timer_start, cur_time);
    }

    /* at the end of stream, we must flush the decoder buffers */
    for (i = 0; i < av->nb_input_streams; i++) {
        ist = &av->input_streams[i];
        if (ist->decoding_needed) {
            libav_output_packet(av, ist, av->output_streams, av->nb_output_streams, NULL);
        }
    }
    if ( !libav_flush_encoders(av, av->output_streams, av->nb_output_streams) )
        return -1;


    /* write the trailer if needed and close file */
    for (i = 0; i < av->nb_output_files; i++) {
        os = av->output_files[i].ctx;
        av_write_trailer(os);
    }

    /* dump report by using the first video and audio streams */
    libav_print_report(av, av->output_files, av->output_streams, av->nb_output_streams, 1, timer_start, av_gettime());

    /* close each encoder */
    for (i = 0; i < av->nb_output_streams; i++) {
        ost = &av->output_streams[i];
        if (ost->encoding_needed) {
            av_freep(&ost->st->codec->stats_in);
            avcodec_close(ost->st->codec);
        }
    }

    /* close each decoder */
    for (i = 0; i < av->nb_input_streams; i++) {
        ist = &av->input_streams[i];
        if (ist->decoding_needed) {
            avcodec_close(ist->st->codec);
        }
    }
#if USE_AVFILTER
        avfilter_graph_free(&ost->graph);
#endif
    /* finished ! */
    //!!ret = 0;

 fail:
    av_freep(&bit_buffer);
    av_freep(&no_packet);

    if (av->output_streams) {
        for (i = 0; i < av->nb_output_streams; i++) {
            ost = &av->output_streams[i];
            if (ost) {
                if (ost->stream_copy)
                    av_freep(&ost->st->codec->extradata);
                av_fifo_free(ost->fifo); /* works even if fifo is not
                                             initialized but set to zero */
                av_free(ost->resample_frame.data[0]);
                av_free(ost->forced_kf_pts);
                if (ost->video_resample)
                    sws_freeContext(ost->img_resample_ctx);
                av_dict_free(&ost->opts);
            }
        }
    }
    return ret;
}
static AVCodec *find_codec_or_die_(const char *name, enum AVMediaType type, int encoder)
{
    const char *codec_string = encoder ? "encoder" : "decoder";
    AVCodec *codec;

    codec = encoder ?
        avcodec_find_encoder_by_name(name) :
        avcodec_find_decoder_by_name(name);
    if (!codec) {
        av_log(NULL, AV_LOG_FATAL, "Unknown %s '%s'\n", codec_string, name);
        return NULL;
    }
    if (codec->type != type) {
        av_log(NULL, AV_LOG_FATAL, "Invalid %s type '%s'\n", codec_string, name);
        return NULL;
    }
    return codec;
}

static AVCodec *choose_decoder(AVFormatContext *s, AVStream *st)
{
    /*char *codec_name = NULL;

    MATCH_PER_STREAM_OPT(codec_names, str, codec_name, s, st);
    if (codec_name) {
        AVCodec *codec = find_codec_or_die(codec_name, st->codec->codec_type, 0);
        st->codec->codec_id = codec->id;
        return codec;
    } else*/
    return avcodec_find_decoder(st->codec->codec_id);
}

void *grow_array(void *array, int elem_size, int *size, int new_size)
{
    if (new_size >= INT_MAX / elem_size) {
        av_log(NULL, AV_LOG_ERROR, "Array too big.\n");
        return NULL;
    }
    if (*size < new_size) {
        uint8_t *tmp = av_realloc(array, new_size*elem_size);
        if (!tmp) {
            av_log(NULL, AV_LOG_ERROR, "Could not alloc buffer.\n");
            return NULL;
        }
        memset(tmp + *size*elem_size, 0, (new_size-*size) * elem_size);
        *size = new_size;
        return tmp;
    }
    return array;
}

/**
 * Add all the streams from the given input file to the global
 * list of input streams.
 */
static int libav_add_input_streams(LibAV *av, AVFormatContext *ic)
{
    int i;

    for (i = 0; i < ic->nb_streams; i++) {
        AVStream *st = ic->streams[i];
        AVCodecContext *dec = st->codec;
        InputStream *ist;

        av->input_streams = grow_array(av->input_streams, sizeof(*av->input_streams), &av->nb_input_streams, av->nb_input_streams + 1);
        if ( !av->input_streams )
            return FALSE;
        ist = &av->input_streams[av->nb_input_streams - 1];
        ist->st = st;
        ist->file_index = av->nb_input_files;
        ist->discard = 1;
        ist->opts = filter_codec_opts(av->codec_opts, choose_decoder(ic, st), ic, st);

        ist->ts_scale = 1.0;

        ist->dec = choose_decoder(ic, st);

        switch (dec->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            if (!ist->dec)
                ist->dec = avcodec_find_decoder(dec->codec_id);
            if (av->audio_disable)
                st->discard = AVDISCARD_ALL;
            break;
        case AVMEDIA_TYPE_VIDEO:
            if(!ist->dec)
                ist->dec = avcodec_find_decoder(dec->codec_id);
            if (dec->lowres) {
                dec->flags |= CODEC_FLAG_EMU_EDGE;
            }

            if (av->video_disable)
                st->discard = AVDISCARD_ALL;
            /*else if (video_discard)
                st->discard = video_discard;*/
            break;
        case AVMEDIA_TYPE_DATA:
            if (av->data_disable)
                st->discard= AVDISCARD_ALL;
            break;
        case AVMEDIA_TYPE_ATTACHMENT:
        case AVMEDIA_TYPE_UNKNOWN:
            break;
        default:
            abort();
        }
    }
    return TRUE;
}
static int libav_add_input_file(LibAV *av, const char *format, const char *filename)
{
    AVFormatContext *ic;
    AVInputFormat *file_iformat = NULL;
    int err, i, ret;
    int64_t timestamp;
    AVDictionary **opts;
    int orig_nb_streams;                     // number of streams before avformat_find_stream_info

    if (format) {
        if (!(file_iformat = av_find_input_format(format))) {
            av_log(NULL, AV_LOG_FATAL, "Unknown input format: '%s'\n", format);
            return FALSE;
        }
    }

    if (!strcmp(filename, "-"))
        filename = "pipe:";


    /* get default parameters from command line */
    ic = avformat_alloc_context();
    if (!ic) {
        print_error(filename, AVERROR(ENOMEM));
        return FALSE;
    }
    if (av->frame_rate) {
        av_dict_set(&av->format_opts, "framerate", av->frame_rate, 0);
    }
    if (av->frame_size) {
        av_dict_set(&av->format_opts, "video_size", av->frame_size, 0);
    }
    if (av->frame_pix_fmt)
        av_dict_set(&av->format_opts, "pixel_format", av->frame_pix_fmt, 0);
    ic->video_codec_id   = av->video_codec_name ?
        find_codec_or_die_(av->video_codec_name   , AVMEDIA_TYPE_VIDEO   , 0)->id : CODEC_ID_NONE;
    ic->audio_codec_id   = av->audio_codec_name ?
        find_codec_or_die_(av->audio_codec_name   , AVMEDIA_TYPE_AUDIO   , 0)->id : CODEC_ID_NONE;
    ic->subtitle_codec_id=  CODEC_ID_NONE;
    ic->flags |= AVFMT_FLAG_NONBLOCK;

    /* open the input file with generic avformat function */
    err = avformat_open_input(&ic, filename, file_iformat, &av->format_opts);
    if (err < 0) {
        print_error(filename, err);
        return FALSE;
    }
    if ( !assert_avoptions_(av->format_opts) )
        return FALSE;

    /* apply forced codec ids */
    for (i = 0; i < ic->nb_streams; i++)
        choose_decoder(ic, ic->streams[i]);

    /* Set AVCodecContext options for avformat_find_stream_info */
    opts = setup_find_stream_info_opts(ic, av->codec_opts);
    orig_nb_streams = ic->nb_streams;

    /* If not enough info to get the stream parameters, we decode the
       first frames to get it. (used in mpeg case for example) */
    ret = avformat_find_stream_info(ic, opts);
    if (ret < 0) {
        av_log(NULL, AV_LOG_FATAL, "%s: could not find codec parameters\n", filename);
        avformat_close_input(&ic);
        return FALSE;
    }

    timestamp = 0;
    /* add the stream start time */
    if (ic->start_time != AV_NOPTS_VALUE)
        timestamp += ic->start_time;


    /* update the current parameters so that they match the one of the input stream */
    if ( !libav_add_input_streams(av, ic) )
        return FALSE;

    /* dump the file content */
    av_dump_format(ic, av->nb_input_files, filename, 0);

    av->input_files = grow_array(av->input_files, sizeof(*av->input_files), &av->nb_input_files, av->nb_input_files + 1);
    av->input_files[av->nb_input_files - 1].ctx        = ic;
    av->input_files[av->nb_input_files - 1].ist_index  = av->nb_input_streams - ic->nb_streams;
    av->input_files[av->nb_input_files - 1].ts_offset  = av->input_ts_offset - timestamp;
    av->input_files[av->nb_input_files - 1].nb_streams = ic->nb_streams;
    av->input_files[av->nb_input_files - 1].rate_emu   = av->rate_emu;

/*    for (i = 0; i < av->nb_dump_attachment; i++) {
        int j;

        for (j = 0; j < ic->nb_streams; j++) {
            AVStream *st = ic->streams[j];

            if (check_stream_specifier(ic, st, av->dump_attachment[i].specifier) == 1)
                dump_attachment(st, av->dump_attachment[i].u.str);
        }
    }*/

    for (i = 0; i < orig_nb_streams; i++)
        av_dict_free(&opts[i]);
    av_freep(&opts);

    libav_reset_options(av, 1);
    return TRUE;
}

static void choose_encoder(const char *codec_name, AVFormatContext *s, OutputStream *ost)
{
    if (!codec_name) {
        ost->st->codec->codec_id = av_guess_codec(s->oformat, NULL, s->filename,
                                                  NULL, ost->st->codec->codec_type);
	ost->enc = avcodec_find_encoder(ost->st->codec->codec_id);
    } else if (!strcmp(codec_name, "copy"))
        ost->stream_copy = 1;
    else {
        ost->enc = find_codec_or_die_(codec_name, ost->st->codec->codec_type, 1);
        ost->st->codec->codec_id = ost->enc->id;
    }
}

static OutputStream *libav_new_output_stream(LibAV *av, AVFormatContext *oc, enum AVMediaType type)
{
    OutputStream *ost;
    AVStream *st = avformat_new_stream(oc, NULL);
    int idx      = oc->nb_streams - 1;

    if (!st) {
        av_log(NULL, AV_LOG_FATAL, "Could not alloc stream.\n");
        return NULL;
    }

    if (oc->nb_streams - 1 < av->nb_streamid_map)
        st->id = av->streamid_map[oc->nb_streams - 1];

    av->output_streams = grow_array(av->output_streams, sizeof(*av->output_streams), &av->nb_output_streams,
                                av->nb_output_streams + 1);
    ost = &av->output_streams[av->nb_output_streams - 1];
    ost->file_index = av->nb_output_files;
    ost->index      = idx;
    ost->st         = st;
    st->codec->codec_type = type;
    choose_encoder((type == AVMEDIA_TYPE_AUDIO) ? av->audio_codec_name: av->video_codec_name, oc, ost);

    if ( ost->enc ) {
       ost->opts = filter_codec_opts(av->codec_opts, ost->enc, oc, st);
    }

    avcodec_get_context_defaults3(st->codec, ost->enc);
    st->codec->codec_type = type; // XXX hack, avcodec_get_context_defaults2() sets type to unknown for stream copy

    ost->max_frames = INT64_MAX;
    if (oc->oformat->flags & AVFMT_GLOBALHEADER)
        st->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

    av_opt_get_int(av->sws_opts, "sws_flags", 0, &ost->sws_flags);
    return ost;
}

static double parse_frame_aspect_ratio(const char *arg)
{
    int x = 0, y = 0;
    double ar = 0;
    const char *p;
    char *end;

    p = strchr(arg, ':');
    if (p) {
        x = strtol(arg, &end, 10);
        if (end == p)
            y = strtol(end + 1, &end, 10);
        if (x > 0 && y > 0)
            ar = (double)x / (double)y;
    } else
        ar = strtod(arg, NULL);

    if (!ar) {
        av_log(NULL, AV_LOG_FATAL, "Incorrect aspect ratio specification.\n");
        return 1;
    }
    return ar;
}

static int parse_matrix_coeffs(uint16_t *dest, const char *str)
{
    int i;
    const char *p = str;
    for (i = 0;; i++) {
        dest[i] = atoi(p);
        if (i == 63)
            break;
        p = strchr(p, ',');
        if (!p) {
            av_log(NULL, AV_LOG_FATAL, "Syntax error in matrix \"%s\" at coeff %d\n", str, i);
            return FALSE;
        }
        p++;
    }
    return TRUE;
}

static OutputStream *libav_new_video_stream(LibAV *av, AVFormatContext *oc)
{
    AVStream *st;
    OutputStream *ost;
    AVCodecContext *video_enc;

    ost = libav_new_output_stream(av, oc, AVMEDIA_TYPE_VIDEO);
    if ( !ost )
        return NULL;
    st  = ost->st;
    video_enc = st->codec;

    if (!ost->stream_copy) {
        const char *p = NULL;
        char *intra_matrix = NULL, *inter_matrix = NULL;
        int i;

        if (av->frame_rate && av_parse_video_rate(&ost->frame_rate, av->frame_rate) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Invalid framerate value: %s\n", av->frame_rate);
            return NULL;
        }

        if (av->frame_size && av_parse_video_size(&video_enc->width, &video_enc->height, av->frame_size) < 0) {
            av_log(NULL, AV_LOG_FATAL, "Invalid frame size: %s.\n", av->frame_size);
            return NULL;
        }

        if (av->frame_aspect_ratio)
            ost->frame_aspect_ratio = parse_frame_aspect_ratio(av->frame_aspect_ratio);

        video_enc->bits_per_raw_sample = 0;
        if (av->frame_pix_fmt && (video_enc->pix_fmt = av_get_pix_fmt(av->frame_pix_fmt)) == PIX_FMT_NONE) {
            av_log(NULL, AV_LOG_FATAL, "Unknown pixel format requested: %s.\n", av->frame_pix_fmt);
            return NULL;
        }
        st->sample_aspect_ratio = video_enc->sample_aspect_ratio;

        if (intra_matrix) {
            if (!(video_enc->intra_matrix = av_mallocz(sizeof(*video_enc->intra_matrix) * 64))) {
                av_log(NULL, AV_LOG_FATAL, "Could not allocate memory for intra matrix.\n");
                return NULL;
            }
            if ( !parse_matrix_coeffs(video_enc->intra_matrix, intra_matrix) )
                return NULL;
        }
        if (inter_matrix) {
            if (!(video_enc->inter_matrix = av_mallocz(sizeof(*video_enc->inter_matrix) * 64))) {
                av_log(NULL, AV_LOG_FATAL, "Could not allocate memory for inter matrix.\n");
                return NULL;
            }
            if ( !parse_matrix_coeffs(video_enc->inter_matrix, inter_matrix) )
                return NULL;
        }
        for (i = 0; p; i++) {
            int start, end, q;
            int e = sscanf(p, "%d,%d,%d", &start, &end, &q);
            if (e != 3) {
                av_log(NULL, AV_LOG_FATAL, "error parsing rc_override\n");
                return NULL;
            }
            /* FIXME realloc failure */
            video_enc->rc_override =
                av_realloc(video_enc->rc_override,
                           sizeof(RcOverride) * (i + 1));
            video_enc->rc_override[i].start_frame = start;
            video_enc->rc_override[i].end_frame   = end;
            if (q > 0) {
                video_enc->rc_override[i].qscale         = q;
                video_enc->rc_override[i].quality_factor = 1.0;
            }
            else {
                video_enc->rc_override[i].qscale         = 0;
                video_enc->rc_override[i].quality_factor = -q/100.0;
            }
            p = strchr(p, '/');
            if (p) p++;
        }
        video_enc->rc_override_count = i;
        if (!video_enc->rc_initial_buffer_occupancy)
            video_enc->rc_initial_buffer_occupancy = video_enc->rc_buffer_size * 3 / 4;
        video_enc->intra_dc_precision = 0;
        ost->top_field_first = -1;
#if USE_AVFILTER
        if ( av->filters )
            ost->avfilter = av_strdup(av->filters);
#endif
    }

    return ost;
}

static OutputStream *libav_new_audio_stream(LibAV *av, AVFormatContext *oc)
{
    AVStream *st;
    OutputStream *ost;
    AVCodecContext *audio_enc;

    ost = libav_new_output_stream(av, oc, AVMEDIA_TYPE_AUDIO);
    st  = ost->st;

    audio_enc = st->codec;
    audio_enc->codec_type = AVMEDIA_TYPE_AUDIO;

    return ost;
}

int my_avformat_alloc_output_context2(AVFormatContext **avctx, AVOutputFormat *oformat,
                                   const char *format, const char *filename)
{
    AVFormatContext *s = avformat_alloc_context();
    int ret = 0;

    *avctx = NULL;
    if (!s)
        goto nomem;

    if (!oformat) {
        if (format) {
            oformat = av_guess_format(format, NULL, NULL);
            if (!oformat) {
                av_log(s, AV_LOG_ERROR, "Requested output format '%s' is not a suitable output format\n", format);
                ret = AVERROR(EINVAL);
                goto error;
            }
        } else {
            oformat = av_guess_format(NULL, filename, NULL);
            if (!oformat) {
                ret = AVERROR(EINVAL);
                av_log(s, AV_LOG_ERROR, "Unable to find a suitable output format for '%s'\n",
                       filename);
                goto error;
            }
        }
    }

    s->oformat = oformat;
    if (s->oformat->priv_data_size > 0) {
        s->priv_data = av_mallocz(s->oformat->priv_data_size);
        if (!s->priv_data)
            goto nomem;
        if (s->oformat->priv_class) {
            *(const AVClass**)s->priv_data= s->oformat->priv_class;
            av_opt_set_defaults(s->priv_data);
        }
    } else
        s->priv_data = NULL;

    if (filename)
        av_strlcpy(s->filename, filename, sizeof(s->filename));
    *avctx = s;
    return 0;
nomem:
    av_log(s, AV_LOG_ERROR, "Out of memory\n");
    ret = AVERROR(ENOMEM);
error:
    avformat_free_context(s);
    return ret;
}

static int libav_add_output_file(LibAV *av, const char *filename)
{
    AVFormatContext *oc;
    int i, err;
    AVOutputFormat *file_oformat;
    OutputStream *ost;
    InputStream  *ist;

    if (!strcmp(filename, "-"))
        filename = "pipe:";

    err = my_avformat_alloc_output_context2(&oc, NULL, NULL, filename);
    if (!oc) {
        print_error(filename, err);
        return FALSE;
    }
    file_oformat= oc->oformat;

#define NEW_STREAM(type, index)\
    if (index >= 0) {\
        ost = libav_new_ ## type ## _stream(av, oc);\
        ost->source_index = index;\
        ost->sync_ist     = &av->input_streams[index];\
        av->input_streams[index].discard = 0;\
    }

    /* video: highest resolution */
    if (!av->video_disable && oc->oformat->video_codec != CODEC_ID_NONE) {
        int area = 0, idx = -1;
        for (i = 0; i < av->nb_input_streams; i++) {
            ist = &av->input_streams[i];
            if (ist->st->codec->codec_type == AVMEDIA_TYPE_VIDEO &&
                ist->st->codec->width * ist->st->codec->height > area) {
                area = ist->st->codec->width * ist->st->codec->height;
                idx = i;
            }
        }
        NEW_STREAM(video, idx);
    }

    /* audio: most channels */
    if (!av->audio_disable && oc->oformat->audio_codec != CODEC_ID_NONE) {
        int channels = 0, idx = -1;
        for (i = 0; i < av->nb_input_streams; i++) {
            ist = &av->input_streams[i];
            if (ist->st->codec->codec_type == AVMEDIA_TYPE_AUDIO &&
                ist->st->codec->channels > channels) {
                channels = ist->st->codec->channels;
                idx = i;
            }
        }
        NEW_STREAM(audio, idx);
    }
    av->output_files = grow_array(av->output_files, sizeof(*av->output_files), &av->nb_output_files, av->nb_output_files + 1);
    av->output_files[av->nb_output_files - 1].ctx       = oc;
    av->output_files[av->nb_output_files - 1].ost_index = av->nb_output_streams - oc->nb_streams;
    av->output_files[av->nb_output_files - 1].recording_time = av->recording_time;
    av->output_files[av->nb_output_files - 1].start_time     = 0;
    av->output_files[av->nb_output_files - 1].limit_filesize = av->limit_filesize;
    av_dict_copy(&av->output_files[av->nb_output_files - 1].opts, av->format_opts, 0);

    /* check filename in case of an image number is expected */
    if (oc->oformat->flags & AVFMT_NEEDNUMBER) {
        if (!av_filename_number_test(oc->filename)) {
            print_error(oc->filename, AVERROR(EINVAL));
            return FALSE;
        }
    }

    if (!(oc->oformat->flags & AVFMT_NOFILE)) {
        /* test if it already exists to avoid losing precious files */

        /* open the file */
        if ((err = avio_open2(&oc->pb, filename, AVIO_FLAG_WRITE,
                              &oc->interrupt_callback,
                              &av->output_files[av->nb_output_files - 1].opts)) < 0) {
            print_error(filename, err);
            return FALSE;
        }
    }

    if (av->mux_preload) {
        uint8_t buf[64];
        snprintf((char*)buf, sizeof(buf), "%d", (int)(av->mux_preload*AV_TIME_BASE));
        av_dict_set(&av->output_files[av->nb_output_files - 1].opts, "preload", (char*)buf, 0);
    }
    oc->max_delay = (int)(av->mux_max_delay * AV_TIME_BASE);

    /* copy global metadata by default */
    if (!av->metadata_global_manual && av->nb_input_files){
        av_dict_copy(&oc->metadata, av->input_files[0].ctx->metadata,
                     AV_DICT_DONT_OVERWRITE);
        if(av->recording_time != INT64_MAX)
            av_dict_set(&oc->metadata, "duration", NULL, 0);
    }
    if (!av->metadata_streams_manual)
        for (i = av->output_files[av->nb_output_files - 1].ost_index; i < av->nb_output_streams; i++) {
            InputStream *ist;
            if (av->output_streams[i].source_index < 0)         /* this is true e.g. for attached files */
                continue;
            ist = &av->input_streams[av->output_streams[i].source_index];
            av_dict_copy(&av->output_streams[i].st->metadata, ist->st->metadata, AV_DICT_DONT_OVERWRITE);
        }
    //libav_reset_options(av, 0);
    return TRUE;
}

LibAV *libav_new(int vesbose)
{
    LibAV *o;
    o = malloc(sizeof(LibAV));
    memset(o, 0, sizeof(LibAV));
    libav_reset_options(o, 0);
    av_log_set_flags(AV_LOG_SKIP_REPEATED);
    if ( vesbose )
        av_log_set_level(AV_LOG_VERBOSE);
    else
        av_log_set_level(0);
    avcodec_register_all();
    avdevice_register_all();
    avformat_network_init();
#if USE_AVFILTER
    avfilter_register_all();
#endif
    av_register_all();

    return o;
}
void libav_set_limit_filesize(LibAV *av, uint64_t limit_filesize)
{
    av->limit_filesize = limit_filesize;
}
void libav_set_video_codec_name(LibAV *av, const char *name)
{
    av->video_codec_name = strdup(name);
}
void libav_set_audio_codec_name(LibAV *av, const char *name)
{
    av->audio_codec_name = strdup(name);
}
void libav_set_frame_pix_fmt(LibAV *av, const char *pix_fmt)
{
    av->frame_pix_fmt = strdup(pix_fmt);
}
void libav_set_frame_acpect_ratio(LibAV *av, const char *acpect_ratio)
{
    av->frame_aspect_ratio = strdup(acpect_ratio);
}

#define FLAGS(o) ((o)->type == AV_OPT_TYPE_FLAGS) ? AV_DICT_APPEND : 0
int libav_add_option(LibAV *av, const char *opt, const char *arg)
{
    const AVOption *oc, *of, *os;
    char opt_stripped[128];
    const char *p;
    const AVClass *cc = avcodec_get_class(), *fc = avformat_get_class(), *sc;

    if (!(p = strchr(opt, ':')))
        p = opt + strlen(opt);
    av_strlcpy(opt_stripped, opt, FFMIN(sizeof(opt_stripped), p - opt + 1));

    if ((oc = av_opt_find(&cc, opt_stripped, NULL, 0,
                         AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ)) ||
        ((opt[0] == 'v' || opt[0] == 'a' || opt[0] == 's') &&
         (oc = av_opt_find(&cc, opt + 1, NULL, 0, AV_OPT_SEARCH_FAKE_OBJ))))
        av_dict_set(&av->codec_opts, opt, arg, FLAGS(oc));
    if ((of = av_opt_find(&fc, opt, NULL, 0,
                          AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ)))
        av_dict_set(&av->format_opts, opt, arg, FLAGS(of));
    sc = sws_get_class();
    if ((os = av_opt_find(&sc, opt, NULL, 0,
                          AV_OPT_SEARCH_CHILDREN | AV_OPT_SEARCH_FAKE_OBJ))) {
        // XXX we only support sws_flags, not arbitrary sws options
        int ret = av_opt_set(av->sws_opts, opt, arg, 0);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Error setting option %s.\n", opt);
            return ret;
        }
    }
    if (oc || of || os)
        return 0;
    av_log(NULL, AV_LOG_ERROR, "Unrecognized option '%s'\n", opt);
    return AVERROR_OPTION_NOT_FOUND;
}
/*
static int64_t libav_getutime(void)
{
#if HAVE_GETRUSAGE
    struct rusage rusage;

    getrusage(RUSAGE_SELF, &rusage);
    return (rusage.ru_utime.tv_sec * 1000000LL) + rusage.ru_utime.tv_usec;
#elif HAVE_GETPROCESSTIMES
    HANDLE proc;
    FILETIME c, e, k, u;
    proc = GetCurrentProcess();
    GetProcessTimes(proc, &c, &e, &k, &u);
    return ((int64_t) u.dwHighDateTime << 32 | u.dwLowDateTime) / 10;
#else
    return av_gettime();
#endif
}*/
void libav_close(LibAV *av)
{
    int i;
    if ( !av )
        return;

    /* close files */
    for (i = 0; i < av->nb_output_files; i++) {
        AVFormatContext *s = av->output_files[i].ctx;
        if (!(s->oformat->flags & AVFMT_NOFILE) && s->pb)
            avio_close(s->pb);
        avformat_free_context(s);
        av_dict_free(&av->output_files[i].opts);
    }
    for (i = 0; i < av->nb_output_streams; i++) {
        AVBitStreamFilterContext *bsfc = av->output_streams[i].bitstream_filters;
        while (bsfc) {
            AVBitStreamFilterContext *next = bsfc->next;
            av_bitstream_filter_close(bsfc);
            bsfc = next;
        }
        av->output_streams[i].bitstream_filters = NULL;

        if (av->output_streams[i].output_frame) {
            AVFrame *frame = av->output_streams[i].output_frame;
            if (frame->extended_data != frame->data)
                av_freep(&frame->extended_data);
            av_freep(&frame);
        }
    }
    for (i = 0; i < av->nb_input_files; i++) {
        avformat_close_input(&av->input_files[i].ctx);
    }
    for (i = 0; i < av->nb_input_streams; i++) {
        av_freep(&av->input_streams[i].decoded_frame);
        av_freep(&av->input_streams[i].filtered_frame);
        av_dict_free(&av->input_streams[i].opts);
        free_buffer_pool(&av->input_streams[i]);
    }
    av_freep(&av->input_streams);
    av_freep(&av->input_files);
    av_freep(&av->output_streams);
    av_freep(&av->output_files);

    uninit_opts(av);
    av_free(av->audio_buf);
    av->allocated_audio_buf_size = 0;
    libav_reset_options(av, 0);
#if USE_AVFILTER
    avfilter_uninit();
#endif

}

int libav_add_input(LibAV *av, const char *format, const char *filename, const char *rate, const char *size)
{
    if ( rate )
        av->frame_rate = strdup(rate);
    if ( size )
        av->frame_size = strdup(size);
    return libav_add_input_file(av, format, filename);
}
static int libav_add_output(LibAV *av, const char *filename, const char *rate, const char *size)
{
    if ( rate )
        av->frame_rate = strdup(rate);
    if ( size )
        av->frame_size = strdup(size);
    return libav_add_output_file(av, filename);
}
static void libav_close_out(LibAV *av)
{
    int i;
    /* close files */
    for (i = 0; i < av->nb_output_files; i++) {
        AVFormatContext *s = av->output_files[i].ctx;
        if (!(s->oformat->flags & AVFMT_NOFILE) && s->pb)
            avio_close(s->pb);
        avformat_free_context(s);
        av_dict_free(&av->output_files[i].opts);
    }
    for (i = 0; i < av->nb_output_streams; i++) {
        AVBitStreamFilterContext *bsfc = av->output_streams[i].bitstream_filters;
        while (bsfc) {
            AVBitStreamFilterContext *next = bsfc->next;
            av_bitstream_filter_close(bsfc);
            bsfc = next;
        }
        av->output_streams[i].bitstream_filters = NULL;

        if (av->output_streams[i].output_frame) {
            AVFrame *frame = av->output_streams[i].output_frame;
            if (frame->extended_data != frame->data)
                av_freep(&frame->extended_data);
            av_freep(&frame);
        }
    }
    av_freep(&av->output_streams);
    av_freep(&av->output_files);
    av->nb_output_streams=0;
    av->nb_output_files=0;
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
int libav_run(LibAV *av, int test)
{
    av->test = test;
    if ( !av->output_path ) {
        av->output_path = get_temp_dir();
    }
    if ( !av->output_filename )
        av->output_filename = strdup("cam");
    if ( !av->output_rate )
        av->output_rate = strdup("10");
    if ( !av->output_size )
        av->output_size = strdup("640x480");
    time_t t1 = 0, t2;
    int64_t tu1 = 0, tu2;

    for ( ;; ) {
        char *filename;
        struct tm start_tm;
        int ret;
        int secs;
        double correction;

        if ( !t1 )
            t1 = time(NULL);
        if ( !tu1 )
            tu1 = av_gettime();

        start_tm = *localtime(&t1);
        filename = malloc(strlen(av->output_path)+strlen(av->output_filename)+30);
        sprintf(filename, "%s/%s-%04d%02d%02d%02d%02d%02d.avi", av->output_path, av->output_filename,
                start_tm.tm_year+1900, start_tm.tm_mon+1, start_tm.tm_mday, start_tm.tm_hour, start_tm.tm_min, start_tm.tm_sec);

        secs = 0;
        libav_add_output(av, filename, av->output_rate, av->output_size);

        ret = libav_transcode(av);
        t2 = time(NULL);
        tu2 = av_gettime();

        if ( av->audio_size + av->video_size > 0 ) {
            int64_t pts = INT64_MAX;
            OutputStream *ost;
            ost = &av->output_streams[0];
            pts = FFMIN(pts, av_rescale_q(ost->st->pts.val,
                                      ost->st->time_base, AV_TIME_BASE_Q));
            secs = t2 - t1;
            correction = (tu2 - tu1) / (double)pts;
        }
        t1 = t2;
        tu1 = tu2;
        libav_close_out(av);
        if ( ret == 0 && secs > 0 && av->complect_cb ) {
            if ( !av->complect_cb(filename, start_tm, secs, correction, av->complect_data) )
                ret = -1;
        }
        if ( secs > 0 )
            av_log(NULL, AV_LOG_INFO, "File \"%s\" complect\n", filename);
        free(filename);
        if ( ret < 0 )
            return FALSE;
        if ( av->test )
            break;
    }
    return TRUE;
}
void libav_set_output_path(LibAV *av, const char *path)
{
    FREE_STR(av->output_path);
    av->output_path = strdup(path);
}
void libav_set_output_filename(LibAV *av, const char *name)
{
    FREE_STR(av->output_filename);
    av->output_filename = strdup(name);
}
void libav_set_output_rate(LibAV *av, const char *rate)
{
    FREE_STR(av->output_rate);
    av->output_rate = strdup(rate);
}
void libav_set_output_size(LibAV *av, const char *size)
{
    FREE_STR(av->output_size);
    av->output_size = strdup(size);
}
void libav_set_complection_func(LibAV *av, outfile_complected_t func, void *user_data)
{
    av->complect_cb = func;
    av->complect_data = user_data;
}
#if USE_AVFILTER 
int avfilter_graph_create_filter12(AVFilterContext **filt_ctx, AVFilter *filt,
                                 const char *name, const char *args, void *opaque,
                                 AVFilterGraph *graph_ctx)
{
    int ret;

    if ((ret = avfilter_open(filt_ctx, filt, name)) < 0)
        goto fail;
    if ((ret = avfilter_init_filter(*filt_ctx, args, opaque)) < 0)
        goto fail;
    if ((ret = avfilter_graph_add_filter(graph_ctx, *filt_ctx)) < 0)
        goto fail;
    return 0;

fail:
    if (*filt_ctx)
        avfilter_free(*filt_ctx);
    *filt_ctx = NULL;
    return ret;
}
#ifdef USE_LIBAV
typedef struct {
    enum PixelFormat pix_fmt;
} AVSinkContext;
static int avsink_init(AVFilterContext *ctx, const char *args, void *opaque)
{
    AVSinkContext *priv = ctx->priv;

    if (!opaque)
        return AVERROR(EINVAL);
    *priv = *(AVSinkContext *)opaque;

    return 0;
}

static void null_end_frame(AVFilterLink *inlink) { }

static int avsink_query_formats(AVFilterContext *ctx)
{
    AVSinkContext *priv = ctx->priv;
    enum PixelFormat pix_fmts[] = { priv->pix_fmt, PIX_FMT_NONE };

    avfilter_set_common_formats(ctx, avfilter_make_format_list(pix_fmts));
    return 0;
}
AVFilter avsink = {
    .name      = "avsink",
    .priv_size = sizeof(AVSinkContext),
    .init      = avsink_init,

    .query_formats = avsink_query_formats,

    .inputs    = (AVFilterPad[]) {{ .name          = "default",
                                    .type          = AVMEDIA_TYPE_VIDEO,
                                    .end_frame     = null_end_frame,
                                    .min_perms     = AV_PERM_READ, },
                                  { .name = NULL }},
    .outputs   = (AVFilterPad[]) {{ .name = NULL }},
};
#endif
static int configure_video_filters(InputStream *ist, OutputStream *ost)
{
    AVFilterContext *last_filter, *filter;
    /** filter graph containing all filters including input & output */
    AVCodecContext *codec = ost->st->codec;
    AVCodecContext *icodec = ist->st->codec;
    enum PixelFormat pix_fmts[] = { codec->pix_fmt, PIX_FMT_NONE };
    //AVBufferSinkParams *buffersink_params = av_buffersink_params_alloc();//!!
    AVRational sample_aspect_ratio;
    char args[255];
    int ret;

    ost->graph = avfilter_graph_alloc();

    if (ist->st->sample_aspect_ratio.num) {
        sample_aspect_ratio = ist->st->sample_aspect_ratio;
    } else
        sample_aspect_ratio = ist->st->codec->sample_aspect_ratio;

    snprintf(args, 255, "%d:%d:%d:%d:%d:%d:%d", ist->st->codec->width,
             ist->st->codec->height, ist->st->codec->pix_fmt, 1, AV_TIME_BASE,
             sample_aspect_ratio.num, sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&ost->input_video_filter, avfilter_get_by_name("buffer"),
                                       "src", args, NULL, ost->graph);
    if (ret < 0)
        return ret;

//#if FF_API_OLD_VSINK_API
#ifdef USE_LIBAV
    ret = avfilter_graph_create_filter(&ost->output_video_filter, &avsink,
                                       "out", NULL, pix_fmts, ost->graph);
#else
    ret = avfilter_graph_create_filter(&ost->output_video_filter, avfilter_get_by_name("buffersink"),
                                       "out", NULL, pix_fmts, ost->graph);
#endif
/*#else
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&ost->output_video_filter, avfilter_get_by_name("buffersink"),
                                       "out", NULL, buffersink_params, ost->graph);
#endif
    av_freep(&buffersink_params);*/

    if (ret < 0)
        return ret;
    last_filter = ost->input_video_filter;

    if (codec->width != icodec->width || codec->height != icodec->height) {
        snprintf(args, 255, "%d:%d:flags=0x%X",
                 codec->width,
                 codec->height,
                 (unsigned)ost->sws_flags);
        if ((ret = avfilter_graph_create_filter(&filter, avfilter_get_by_name("scale"),
                                                NULL, args, NULL, ost->graph)) < 0)
            return ret;
        if ((ret = avfilter_link(last_filter, 0, filter, 0)) < 0)
            return ret;
        last_filter = filter;
    }

    snprintf(args, sizeof(args), "flags=0x%X", (unsigned)ost->sws_flags);
    ost->graph->scale_sws_opts = av_strdup(args);

    if (ost->avfilter) {
#ifdef USE_LIBAV
        AVFilterInOut *outputs = av_malloc(sizeof(AVFilterInOut));
        AVFilterInOut *inputs  = av_malloc(sizeof(AVFilterInOut));

#else
        AVFilterInOut *outputs = avfilter_inout_alloc();
        AVFilterInOut *inputs  = avfilter_inout_alloc();
#endif

        outputs->name    = av_strdup("in");
        outputs->filter_ctx = last_filter;
        outputs->pad_idx = 0;
        outputs->next    = NULL;

        inputs->name    = av_strdup("out");
        inputs->filter_ctx = ost->output_video_filter;
        inputs->pad_idx = 0;
        inputs->next    = NULL;
#ifdef USE_LIBAV
        if ((ret = avfilter_graph_parse(ost->graph, ost->avfilter, inputs, outputs, NULL)) < 0)
#else
        if ((ret = avfilter_graph_parse(ost->graph, ost->avfilter, &inputs, &outputs, NULL)) < 0)
#endif
            return ret;
        av_freep(&ost->avfilter);
    } else {
        if ((ret = avfilter_link(last_filter, 0, ost->output_video_filter, 0)) < 0)
            return ret;
    }

    if ((ret = avfilter_graph_config(ost->graph, NULL)) < 0)
        return ret;

    codec->width  = ost->output_video_filter->inputs[0]->w;
    codec->height = ost->output_video_filter->inputs[0]->h;
    codec->sample_aspect_ratio = ost->st->sample_aspect_ratio =
        ost->frame_aspect_ratio ? // overridden by the -aspect cli option
        av_d2q(ost->frame_aspect_ratio * codec->height/codec->width, 255) :
        ost->output_video_filter->inputs[0]->sample_aspect_ratio;

    return 0;
}
#endif /* USE_AVFILTER */
void libav_set_video_disable(LibAV *av, int disable)
{
    av->video_disable = disable;
}
void libav_set_audio_disable(LibAV *av, int disable)
{
    av->audio_disable = disable;
}
#ifdef USE_AVFILTER
void   libav_set_filters(LibAV *av, const char *filters)
{
    if ( av->filters )
        free(av->filters);
    if ( filters )
        av->filters = strdup(filters);
    else
        av->filters = NULL;
}
#endif
