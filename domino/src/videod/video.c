#include "video.h"
#include "version.h"
#include <libpq-fe.h>
#include <dolib.h>
#include <glib.h>
#include <errno.h>

#include "../misc/send_error.h"

#include "../misc/storage.h"
#include "../misc/socket.h"
#include <unistd.h>


struct _video_t
{
    storage_t *storage;
    char *conninfo;
    PGconn *conn;
    do_sort_list_t *cams;
    int time_wait;
    char *storage_url;
    size_t  min_free;
    size_t  reserve_free;
    int check_indx;
    int reload;
};



typedef struct {
    char *name;
    char *host;
    int   port;
    char *path;
    char *mask_filename;
    do_string_list_t *masks;
    int   duration;
    int   daemon;

    char  *user;
    char  *password;

    int   stopped;
    int   error;
    time_t start_time;


    struct tm last_piece;
} cam_info_t;

#define RUNNING (do_mproc_state_get_state() == DO_STATE_RUN)

static void pg_log(PGconn *conn, const char *msg);
static int video_connect_db(video_t *video);
static void video_close_db(video_t *video);
static void cam_info_free(cam_info_t *cam);
static int video_update_cam_info(video_t *video);

inline static int cam_info_cmd_func(cam_info_t *cam1,cam_info_t *cam2) { return strcmp(cam1->name, cam2->name); } ;


video_t *video_new(option_t *opt)
{
    video_t *video;
    video = do_malloc0(sizeof(video_t));
    video->storage = storage_new(1000, program_name);

    storage_set_db_storage_param(video->storage, opt->conninfo);
    storage_set_file_storage_param(video->storage, opt->storage_url, NULL);
    storage_set_no_thread(video->storage, TRUE);

    video->storage_url = do_strdup(opt->storage_url);
    video->conninfo = do_strdup(opt->conninfo);
    video->cams = do_sort_list_new(FALSE, TRUE, (do_list_cmp_func)cam_info_cmd_func, NULL);
    video->time_wait = opt->time_wait_piece;
    video->min_free = opt->storage_minimum_free;
    video->reserve_free = opt->storage_reserve_free;

    return video;
}
void video_free(video_t *video)
{
    storage_free(video->storage);
    do_list_foreach((do_list_t*)video->cams, (do_list_foreach_func)cam_info_free, NULL);
    do_sort_list_free(video->cams);
    do_free(video->conninfo);
    do_free(video->storage_url);
    do_free(video);
}
void video_reinit(video_t *video, option_t *opt)
{
    video_update_cam_info(video);
}
static int check_daemon(cam_info_t *cam);
static int check_last_piece(video_t *video, cam_info_t *cam);
static int check_host(const char *host, int port);
static int get_pieces(video_t *video, cam_info_t *cam);
int clear_old(video_t *video);


void video_run(video_t *video)
{
    //!!if ( !video->cams->count || video->reload ) {
        video_update_cam_info(video);
        video->reload = FALSE;
        video->check_indx = 0;
    //!!}

    int i;
    if ( video->check_indx == 4 )
        video->check_indx = 0;
    for ( i = 0; i < video->cams->count && !video->check_indx; i++ ) {
        cam_info_t *cam;
        cam = video->cams->list[i];
#ifdef DEBUG
	do_log(LOG_INFO, "check cam %s (%s) (%s)", cam->name, cam->stopped ? "stop":"run", cam->error ? "error" : "ok" );
#endif
        if ( cam->stopped )
            continue;

        if ( cam->daemon ) {
#ifdef DEBUG
	     do_log(LOG_INFO, "check daemon for cam %s", cam->name);
#endif
            if ( !check_daemon(cam) ) {
#ifdef DEBUG
   	        do_log(LOG_ERR, "%s : daemon %s:%d cam %s not run", program_name, cam->host, cam->port, cam->name);
#endif
                //if ( !cam->error )
                    send_info_message_width_host_info_printf("%s : daemon %s:%d cam %s not run", program_name, cam->host, cam->port, cam->name);
                cam->error = TRUE;
                continue;
            }
#ifdef DEBUG
	     do_log(LOG_INFO, "check daemon for cam %s ok", cam->name);
#endif
        }
        else {
#ifdef DEBUG
	     do_log(LOG_INFO, "check host for cam %s", cam->name);
#endif
            if ( !check_host(cam->host, cam->port) ) {
#ifdef DEBUG
	        do_log(LOG_ERR, "%s : host %s:%d cam %s is down", program_name, cam->host, cam->port, cam->name);
#endif
                //if ( !cam->error )
                    send_info_message_width_host_info_printf("%s : host %s:%d cam %s is down", program_name, cam->host, cam->port, cam->name);
                cam->error = TRUE;
                continue;
            }
#ifdef DEBUG
	     do_log(LOG_INFO, "check host for cam %s ok", cam->name);
#endif
        }
        if ( !check_last_piece(video, cam) ) {
#ifdef DEBUG
            do_log(LOG_ERR,"%s : no evailable video from cam %s more %d secs.", program_name, cam->name, video->time_wait);
#endif
            //if ( !cam->error )
                send_info_message_width_host_info_printf("%s : no evailable video from cam %s more %d secs.", program_name, cam->name, video->time_wait);
            cam->error = TRUE;
            continue;
        }
        cam->error = FALSE;
    }
    video->check_indx++;

    for ( i = 0; i < video->cams->count; i++ ) {
        cam_info_t *cam;
        cam = video->cams->list[i];
        if ( cam->daemon )
            continue;
        get_pieces(video, cam);
    }

    clear_old(video);
}
void video_set_cam_up(video_t *video, const char *name, int up)
{
    cam_info_t cam;
    int indx;
    cam.name = (char*)name;
    if ( do_sort_list_find(video->cams, &indx, &cam) ) {
	cam_info_t *item;
        item = ((cam_info_t*)video->cams->list[indx]);
#ifdef DEBUG
        do_log(LOG_INFO, "cam %s %s", ((cam_info_t*)video->cams->list[indx])->name, up ? "started" : "stopped");
#endif
        item->stopped = !up;
	item->error = FALSE;
	item->start_time = time(NULL);

	if ( !video_connect_db(video) )
    	    return;
        PGresult *res;
	int status;
        char sql[1024];
	//int nrows, i;
#ifdef DEBUG
        do_log(LOG_INFO, "update cam info");
#endif
        sprintf(sql, "UPDATE cam_source_info SET stopped=%s WHERE cam='%s'",
                      item->stopped ? "TRUE" : "FALSE", item->name);

	res=PQexec(video->conn, sql);
        status = PQresultStatus(res);

	if ( status != PGRES_COMMAND_OK &&
    	     status != PGRES_TUPLES_OK ) {
    	    pg_log(video->conn, "Select");
	}

	PQclear(res);
	video_close_db(video);
    }
}
static void pg_log(PGconn *conn, const char *msg)
{
    do_log(LOG_ERR, "%s.%s", msg, PQerrorMessage(conn));
}
static int video_connect_db(video_t *video)
{
    if ( video->conninfo ) {
        video->conn=PQconnectdb(video->conninfo);
        if ( PQstatus(video->conn) != CONNECTION_OK ) {
            pg_log(video->conn, "Connect");
            send_error_message_width_host_info_printf("%s: Error connect to \"%s\"", program_name, video->conninfo);
            return FALSE;
        }
    }
    return TRUE;
}
static void video_close_db(video_t *video)
{
    if ( video->conninfo )
        PQfinish(video->conn);
}
static void cam_info_free(cam_info_t *cam)
{
    do_free(cam->name);
    if ( cam->masks )
        do_string_list_free(cam->masks);
    if ( cam->host )
        do_free(cam->host);
    if ( cam->path )
        do_free(cam->path);
    if ( cam->mask_filename )
        do_free(cam->mask_filename);
    if ( cam->user )
        do_free(cam->user);
    if ( cam->password )
        do_free(cam->password);

    do_free(cam);
}
static void make_masks(cam_info_t *cam);
static int video_update_cam_info(video_t *video)
{
#ifdef DEBUG
    do_log(LOG_INFO, "videod reload");
#endif
    if ( !video_connect_db(video) )
        return FALSE;

    PGresult *res;
    int status;
    int ret = TRUE;
    char sql[1024];
    int nrows, i;
    cam_info_t cam, *crnt;

    sprintf(sql, "SELECT cam,daemon,host,port,path,mask_filename,duration,auth,stopped FROM cam_source_info");

    res=PQexec(video->conn, sql);
    status = PQresultStatus(res);

    if ( status != PGRES_COMMAND_OK &&
         status != PGRES_TUPLES_OK ) {
        pg_log(video->conn, "Select");
        ret = FALSE;
    }

    nrows = PQntuples(res);
    for ( i = 0; i < nrows && ret; i++ ) {
        int indx;
        cam.name = PQgetvalue(res, i, 0);
        if ( !do_sort_list_find(video->cams, &indx, &cam) ) {
            crnt = do_malloc0(sizeof(cam_info_t));
            crnt->name = do_strdup(cam.name);
            do_sort_list_insert(video->cams, indx, crnt);
        }
        else {
            crnt = video->cams->list[indx];
            if ( crnt->host ) do_free(crnt->host);
            if ( crnt->path ) do_free(crnt->path);
            if ( crnt->mask_filename ) do_free(crnt->mask_filename);
            crnt->host = crnt->path = crnt->mask_filename = crnt->user = crnt->password = NULL;

        }
        crnt->daemon = !strcmp(PQgetvalue(res, i, 1), "t");
        crnt->stopped = !strcmp(PQgetvalue(res, i, 8), "t");

        crnt->host = do_strdup(PQgetvalue(res, i, 2));
        crnt->port = atoi(PQgetvalue(res, i, 3));
        if ( !crnt->daemon ) {
            crnt->path = do_strdup(PQgetvalue(res, i, 4));
            crnt->mask_filename = do_strdup(PQgetvalue(res, i, 5));
            make_masks(crnt);
        }
        crnt->duration = atoi(PQgetvalue(res, i, 6));
        char *auth, *ch;
        auth = PQgetvalue(res, i, 7);
        ch = strchr(auth, ':');
        if ( ch ) {
            crnt->user = do_strndup(auth, ch - auth);
            crnt->password = do_strdup(ch + 1);
        }
        else {
            crnt->user = do_strdup("domino");
            crnt->password = do_strdup("domino");
        }
    }
    PQclear(res);
    video_close_db(video);
    return ret;
}
static int check_daemon(cam_info_t *cam)
{
    int res;
    do_rpc_client_t *client;
    client = do_rpc_client_new(cam->host, cam->port, cam->user, cam->password, FALSE);
    res = client != NULL;
    if ( res )
        res = do_rpc_client_connect(client);
    if ( res ) {
        do_data_t *out;
        out = do_data_new();
        res = do_rpc_client_send_command(client, "state", "", NULL, out);
        if ( res ) {
            char *str;
            str = do_data_str(out);
            res = !strcmp(str, "run");
            do_free(str);
        }
        do_data_free(out);
    }
    if ( client )
        do_rpc_client_free(client);
    return res;
}
static int check_host(const char *host, int port)
{
    struct hostent *hp = gethostbyname(host);
    if ( !hp ) {
        do_log(LOG_ERR, "Unknown host %s", host);
        return FALSE;
    }
    sockaddr_in_t dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    memcpy(&dest.sin_addr, hp->h_addr, sizeof(dest.sin_addr));
    dest.sin_port = htons(port);
    socket_t sock;
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (invalid_socket(sock)) {
        do_log(LOG_ERR, "socket not open " SOCKERROR_SUFFIX);
        return FALSE;
    }
    if (connect(sock, (struct sockaddr*) &dest, sizeof(dest)) == -1) {
        do_log(LOG_ERR, "Connect not open " SOCKERROR_SUFFIX);
        close_socket(sock);
        return FALSE;
    }
    close_socket(sock);
    return TRUE;
}
static int check_last_piece(video_t *video, cam_info_t *cam)
{
    if ( !video_connect_db(video) )
        return FALSE;

    PGresult *res;
    int status;
    int ret = TRUE;
    char sql[1024];
    int nrows;

    sprintf(sql, "SELECT extract(epoch from time_start)::bigint FROM pieces WHERE cam='%s' ORDER BY time_start DESC LIMIT 1", cam->name);

    res=PQexec(video->conn, sql);
    status = PQresultStatus(res);

    if ( status != PGRES_COMMAND_OK &&
         status != PGRES_TUPLES_OK ) {
        pg_log(video->conn, "Select");
        ret = FALSE;
    }

    nrows = PQntuples(res);
    if ( nrows == 1 ) {
        time_t t = time(NULL);
        int64_t value = atoll(PQgetvalue(res, 0, 0));
        ret = (value > t - video->time_wait) || ( t < cam->start_time + video->time_wait);
    }
    else
        ret = FALSE;
    PQclear(res);
    video_close_db(video);
    return ret;
}
static int get_pieces(video_t *video, cam_info_t *cam)
{
    if ( !cam->path || cam->path[0] == '\0' )
        return TRUE;

    struct tm tm;
    int i, ret = TRUE;
    memset(&tm, 0, sizeof(tm));
    tm.tm_isdst = 0; // no daylight
    tm.tm_year = tm.tm_mon = tm.tm_mday = tm.tm_hour = tm.tm_min = -1;
    do_list_t *files;
    files = do_list_new(FALSE);
    storage_get_list_fileinfo(video->storage, cam->path, cam->masks, 0, &tm, files);
    int rename_only = FALSE;
    for ( i = 0; i < files->count; i++ ) {
        fileinfo_t *file;
        file = files->list[i];
        if ( !storage_add_file2(video->storage, file->filename, file->tm, cam->duration, 1, cam->name, rename_only) ) {
            ret = FALSE;
            do_log(LOG_ERR, "Error add file to queue");
            send_error_message_width_host_info_printf("%s: Error add file to queue", program_name);
            break;
        }
        else {
            if ( !rename_only ) {
                if ( !storage_delete_file(video->storage, file->filename) ) {
                    ret = FALSE;
                    do_log(LOG_ERR, "Error add rename file");
                    send_error_message_width_host_info_printf("%s: Error rename file", program_name);
                    break;
                }
            }

        }
    }
    if ( ret ) {

    }
    do_list_foreach(files, (do_list_foreach_func)fileinfo_free, NULL);
    do_list_free(files);

    return ret;
}

static void make_masks(cam_info_t *cam)
{
    if ( cam->masks )
        do_string_list_free(cam->masks);
    if ( !cam->mask_filename || cam->mask_filename[0] == '\0' ) {
        cam->masks = NULL;
        return;
    }
    cam->masks = do_string_list_new();
    char *mask, *ch, *tail;
    mask = do_strdup(cam->mask_filename);
    tail = mask;
    while ( (ch = strchr(tail, '/')) != NULL ) {
        ch[0] = '\0';
        do_string_list_add_alloc(cam->masks, tail);
        tail = ch+1;
    }
    do_string_list_add_alloc(cam->masks, tail);
    do_free(mask);
}
int clear_old(video_t *video)
{
    int ret;
    ret = storage_clean_storage(video->storage, video->min_free, video->reserve_free);
    if ( !ret ) {
        do_log(LOG_ERR, "Not clean storage");
        send_error_message_width_host_info_printf("%s: Not clean storage", program_name);
    }
    return ret;
}
void video_set_reload(video_t *video)
{
    video->reload = TRUE;
}
