#include "storage.h"
#include "../misc/mythread.h"
#include <dolib.h>
#include <libsmbclient.h>
#include <errno.h>
#include <glib.h>
//#include "version.h"
#include "send_error.h"
#include <dirent.h>
#include <limits.h>
#include <sys/vfs.h>
//#include <talloc.h>
#ifdef USE_POSTGRESQL
#include <libpq-fe.h>
#endif

#define COPY_BUFFER_LEN 1024*1024*10
#define MAX_WAIT_THREAD 20

typedef struct {
    char *filename;
    struct tm start_time;
    int duration;
    double correction;
    char *name;
    int  rename_only;
    int  copy_complect;
} queue_item_t;

typedef enum {
    STORAGE_SAMBA = 0,
    STORAGE_FILE,
    STORAGE_NULL,
    STORAGE_NO
} storage_kind_t;

static const char *scheme_names[STORAGE_NO] = {"smb", "file", "null"};
typedef struct {
    SMBCCTX  *smb_context;
    smbc_opendir_fn smbc_opendir;
    smbc_rmdir_fn smbc_rmdir;
    smbc_mkdir_fn smbc_mkdir;
    smbc_readdir_fn smbc_readdir;
    smbc_closedir_fn smbc_closedir;
    smbc_rename_fn smbc_rename;
    smbc_write_fn smbc_write;
    smbc_creat_fn smbc_creat;
    smbc_close_fn smbc_close;
    smbc_stat_fn  smbc_stat;
    smbc_statvfs_fn  smbc_statvfs;
    smbc_unlink_fn smbc_unlink;
} samba_context_t;

struct _storage_t {
    do_list_t *queue;
    int max_queue_len;
    char *buff;
    char *url;
    char *fs;
#ifdef USE_POSTGRESQL
    char *conninfo;
    PGconn *conn;
#endif
    storage_kind_t kind;
    mymutex_t *mutex;
    mythread_t *thread;
    int thread_stoped;
    char *filename;
    int error;
    char *prog_name;
    int no_thread;

//    TALLOC_CTX *frame;

    samba_context_t samba;
};

static void storage_run(storage_t *storage);
static int storage_init_storage(storage_t *storage);
static int storage_queue(storage_t *storage);

static int scan_procs(const char *name);



static char *get_scheme_from_url(const char *url, char **tail)
{
    char *uri;
    char *ch;
    uri = (char*)url;

 	ch = strchr (uri, ':');
 	if ( ch ) {
 	    char *retval;
        retval = do_strndup(uri, ch - uri + 1);
        retval[ch - uri] = '\0';
        if ( *(ch+1) == '/' && *(ch+2) == '/' ) {
            *tail = do_strdup(ch + 3);
        }
        return retval;
 	}
 	return NULL;
}
static char *get_fs_from_url(const char *url, storage_kind_t kind)
{
    char *uri;
    char *ch;
    uri = (char*)url;
    char *tail;
    //char *pos;

    ch = strchr (uri, ':');
    if ( ch ) {
        char *retval;
        retval = do_strndup(uri, ch - uri + 1);
        retval[ch - uri] = '\0';
        tail = ch + 1;
        if ( *(ch+1) == '/' && *(ch+2) == '/' ) {
            tail = ch + 3;
        }
    }
    else
        tail = uri;
    switch ( kind ) {
        case STORAGE_FILE:
            return do_strdup(tail);
        case STORAGE_SAMBA:
            ch = strchr (tail, '/');
            do_log(LOG_INFO, "%s %s",tail ,ch);
            if ( ch ) {
                tail = ch + 1;
                ch = strchr (tail, '/');
            }
            if ( ch ) {
                char *ret;
                ret = do_strdup(uri);
                ret[ch - uri] = '\0';
                return ret;
            }
            break;
        case STORAGE_NULL:
            return NULL;
        case STORAGE_NO:
            break;
    }
    return NULL;
}

storage_t *storage_new(int queue_len, const char *prog_name)
{
    storage_t *storage;
    storage = do_malloc0(sizeof(storage_t));
    storage->queue = do_list_new(FALSE);
    storage->max_queue_len = queue_len;
    storage->buff = (char*)do_malloc(COPY_BUFFER_LEN);
    storage->kind = STORAGE_NO;
    storage->mutex = mymutex_new();
    storage->prog_name = do_strdup(prog_name);
//    storage->frame = talloc_stackframe();

    storage->no_thread =FALSE;
    return storage;
}
int storage_set_file_storage_param(storage_t *storage, const char *url, const char *filename)
{
    char *scheme, *tail = NULL;
    int i;
    storage_stop(storage);

    storage->kind = STORAGE_NO;
    scheme = get_scheme_from_url(url, &tail);

    for ( i = 0; i < STORAGE_NO && scheme; i++ )
        if ( !strcmp(scheme, scheme_names[i]) ) {
            storage->kind = (storage_kind_t)i;
            break;
        }
    if ( !scheme || storage->kind == STORAGE_NO ) {
        if ( !scheme )
            do_log(LOG_ERR, "Not define scheme if url \"%s\"", url);
        else {
            do_log(LOG_ERR, "Scheme \"%s\" not support", scheme);
            do_free(scheme);
        }
        if ( tail )
            do_free(tail);
        return FALSE;
    }
    if ( scheme )
        do_free(scheme);
    if ( storage->url )
        do_free(storage->url);
    storage->url = NULL;
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            storage->url = do_strdup(url);
            break;
        case STORAGE_FILE:
            if ( !tail ) {
                do_log(LOG_ERR, "Not define path for url \"%s\"", url);
                return FALSE;
            }
            storage->url = do_strdup(tail);
            break;
        case STORAGE_NULL:
            storage->url = do_strdup("");
            break;
        case STORAGE_NO:
            break;
    }
    if ( tail )
        do_free(tail);
    if ( storage->filename )
        do_free(storage->filename);
    if ( filename )
        storage->filename = do_strdup(filename);
    else {
        char hostname[1024];
        if ( gethostname(hostname, 1023) )
            strcpy(hostname,   "unknow");
        storage->filename = do_strdup(hostname);
    }
    if ( !storage_init_storage(storage) ) {
        storage->error = TRUE;
        return FALSE;
    }
    storage->fs = get_fs_from_url(storage->url, storage->kind);
    do_log(LOG_INFO, "parse url \"%s\":%d -> \"%s\"",storage->url,storage->kind, storage->fs);
    return TRUE;
}
#ifdef USE_POSTGRESQL
int storage_set_db_storage_param(storage_t *storage, const char *conninfo)
{
    if ( storage->conninfo )
        do_free(storage->conninfo);
    storage->conninfo = do_strdup(conninfo);
    return TRUE;
}
#endif
static void queue_item_free(queue_item_t *item)
{
    do_free(item->filename);
    if ( item->name )
        do_free(item->name);
    do_free(item);
}
void storage_free(storage_t *storage)
{
    if ( !storage )
        return;
    storage_stop(storage);
    mymutex_lock(storage->mutex);
    do_list_foreach(storage->queue, (do_list_foreach_func)queue_item_free, NULL);
    do_list_free(storage->queue);
    mymutex_unlock(storage->mutex);
    if ( storage->thread )
        mythread_free(storage->thread);
    do_free(storage->buff);
    if ( storage->filename )
        do_free(storage->filename);
    if ( storage->url )
        do_free(storage->url);
    if ( storage->prog_name )
        do_free(storage->prog_name);
    if ( storage->fs )
        do_free(storage->fs);
//    TALLOC_FREE(storage->frame);
    do_free(storage);
}
int storage_add_file(storage_t *storage, const char *filename, struct tm start_time, int secs, double correction)
{
    return storage_add_file2(storage, filename, start_time, secs, correction, NULL, FALSE);
}
int  storage_add_file2(storage_t *storage, const char *filename, struct tm start_time, int secs, double correction, const char *name, int rename_only)
{
    if ( storage->error ) {
        do_log(LOG_ERR, "storage in error");
        return FALSE;
    }

    mymutex_lock(storage->mutex);
    if ( storage->queue->count >= storage->max_queue_len ) {
        do_log(LOG_ERR, "storage max queue length");
        mymutex_unlock(storage->mutex);
        return FALSE;
    }
    queue_item_t item;
    item.filename = do_strdup(filename);
    item.start_time = start_time;
    item.duration = secs;
    item.correction = correction;
    if ( name )
        item.name = do_strdup(name);
    else
        item.name = NULL;
    item.rename_only = rename_only;
    item.copy_complect = FALSE;
    do_list_add_alloc(storage->queue, &item, sizeof(item));
    mymutex_unlock(storage->mutex);

    if ( !storage->no_thread ) {
        if ( !storage->thread ) {
            storage->thread = mythread_new((mythread_proc_t*)storage_run, storage);
            if ( !storage->thread ) {
                do_log(LOG_ERR, "storage not create thread");
                storage->error = TRUE;
                return FALSE;
            }
        }
    }
    else {
        return storage_queue(storage);
    }
    return TRUE;
}
int storage_stop(storage_t *storage)
{
    int i;
    if ( !storage )
        return TRUE;
    if ( !storage->thread || !mythread_is_running(storage->thread) )
        return TRUE;
    storage->thread_stoped = TRUE;
    for ( i = 0; i < MAX_WAIT_THREAD; i++ ) {
        if ( !mythread_is_running(storage->thread) )
            return TRUE;
        DO_SEC_SLEEP(1);
    }
    return FALSE;
}

static char	*workgroup	= "LEKAR";
static char	*username	= "kassa";
static char	*password	= "kassa";

static void smbc_auth_fn(
                const char      *server,
		const char      *share,
		char            *wrkgrp, int wrkgrplen,
		char            *user,   int userlen,
		char            *passwd, int passwdlen){

    (void) server;
    (void) share;
    (void) wrkgrp;
    (void) wrkgrplen;

    strncpy(wrkgrp, workgroup, wrkgrplen - 1); wrkgrp[wrkgrplen - 1] = 0;
    strncpy(user, username, userlen - 1); user[userlen - 1] = 0;
    strncpy(passwd, password, passwdlen - 1); passwd[passwdlen - 1] = 0;
#ifdef DEBUG
    do_log(LOG_INFO, "Samba authorized");
#endif
}


static int storage_init_samba_contex(samba_context_t *samba)
{


    int err = smbc_init(smbc_auth_fn,  4);
    if (err < 0) {
    	do_log(LOG_ERR, "Initializing the smbclient library ...: %s", strerror(errno));
    	return FALSE;
    }

    samba->smb_context = smbc_new_context ();
    if ( !samba->smb_context ) {
        do_log(LOG_ERR, "Failed to allocate smb context");
        return FALSE;
    }
    smbc_setDebug (samba->smb_context, 4);
    smbc_setOptionDebugToStderr (samba->smb_context, 1);
    smbc_setFunctionAuthData(samba->smb_context, smbc_auth_fn);

    if ( !smbc_init_context (samba->smb_context) ) {
        do_log(LOG_ERR, "Failed to initialize smb context\n");
        smbc_free_context (samba->smb_context, 0);
        return FALSE;
    }
    smbc_set_context(samba->smb_context);

    samba->smbc_opendir = smbc_getFunctionOpendir(samba->smb_context);
    samba->smbc_readdir = smbc_getFunctionReaddir(samba->smb_context);
    samba->smbc_rmdir = smbc_getFunctionRmdir(samba->smb_context);
    samba->smbc_closedir = smbc_getFunctionClosedir(samba->smb_context);
    samba->smbc_stat = smbc_getFunctionStat(samba->smb_context);
    samba->smbc_write = smbc_getFunctionWrite(samba->smb_context);
    samba->smbc_creat = smbc_getFunctionCreat(samba->smb_context);
    samba->smbc_close = smbc_getFunctionClose(samba->smb_context);
    samba->smbc_rename = smbc_getFunctionRename(samba->smb_context);
    samba->smbc_statvfs = smbc_getFunctionStatVFS(samba->smb_context);
    samba->smbc_unlink = smbc_getFunctionUnlink(samba->smb_context);



    return TRUE;

}
#ifdef USE_POSTGRESQL
static void pg_log(PGconn *conn, const char *msg)
{
    do_log(LOG_ERR, "%s.%s", msg, PQerrorMessage(conn));
}
static int connect_db(storage_t *storage)
{
    if ( storage->conninfo ) {
        storage->conn=PQconnectdb(storage->conninfo);
        if ( PQstatus(storage->conn) != CONNECTION_OK ) {
            pg_log(storage->conn, "Connect");
            send_error_message_width_host_info_printf("%s: Error connect to \"%s\"", storage->prog_name, storage->conninfo);
            return FALSE;
        }
    }
    return TRUE;
}
static void close_db(storage_t *storage)
{
    if ( storage->conninfo )
        PQfinish(storage->conn);
}
#endif
static int storage_init_storage(storage_t *storage)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            if ( !storage_init_samba_contex(&storage->samba) ) {
                return FALSE;
            }
            break;
        case STORAGE_FILE:
        case STORAGE_NULL:
            break;
        case STORAGE_NO:
            do_log(LOG_ERR, "Undefine storage type");
            return FALSE;
    }
#ifdef USE_POSTGRESQL
//    connect_db(storage);
#endif
    return TRUE;
}
static int storage_close_storage(storage_t *storage)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            smbc_free_context(storage->samba.smb_context, 1);
            break;
        case STORAGE_FILE:
        case STORAGE_NULL:
            break;
        case STORAGE_NO:
            break;
    }
#ifdef USE_POSTGRESQL
//	close_db(storage);
#endif
    return TRUE;
}
static int storage_file_storage_is_available(storage_t *storage)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA: {
            /*SMBCFILE *fd;
            if ( (fd = storage->samba.smbc_opendir(storage->samba.smb_context, storage->url)) == NULL ) {
                do_log(LOG_WARNING, "smb_opendir \"%s\" %s\n", storage->url, strerror(errno));
                send_error_message_width_host_info_printf("%s: smb_opendir %s %s\n", storage->prog_name, storage->url, strerror(errno));
                return FALSE;
            }
            if ( storage->samba.smbc_closedir(storage->samba.smb_context, fd) < 0 ) {
                do_log(LOG_WARNING, "smbc_closedir \"%s\" %s\n", storage->url, strerror(errno));
                return FALSE;
            }*/
            struct stat buf;

            if ( storage->samba.smbc_stat(storage->samba.smb_context, storage->url, &buf) != 0 ) {
                do_log(LOG_ERR,"bla1");
                do_log(LOG_WARNING, "stat \"%s\" %s\n", storage->url, strerror(errno));
                send_error_message_width_host_info_printf("%s: stat %s %s\n", storage->prog_name, storage->url, strerror(errno));
                return FALSE;
            }

            break;
        }
        case STORAGE_FILE: {
            /*DIR  *fd;
            if ( (fd = opendir(storage->url)) == NULL )  {
                do_log(LOG_WARNING, "opendir \"%s\" %s\n", storage->url, strerror(errno));
                send_error_message_width_host_info_printf("%s: opendir %s %s\n", storage->prog_name, storage->url, strerror(errno));
                return FALSE;
            }
            closedir(fd);*/
            struct stat buf;
            if ( stat(storage->url, &buf) != 0 ) {
                do_log(LOG_ERR,"bla");
                do_log(LOG_WARNING, "stat \"%s\" %s\n", storage->url, strerror(errno));
                send_error_message_width_host_info_printf("%s: stat %s %s\n", storage->prog_name, storage->url, strerror(errno));
                return FALSE;
            }

            break;
        }
        case STORAGE_NULL:
            break;
        case STORAGE_NO:
            break;
    }
    return TRUE;
}
static int storage_file_storage_exists(storage_t *storage, char *path)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA: {
            /*SMBCFILE *fd;
            if ( (fd = storage->samba.smbc_opendir(storage->samba.smb_context, storage->url)) == NULL ) {
                do_log(LOG_WARNING, "smb_opendir \"%s\" %s\n", storage->url, strerror(errno));
                send_error_message_width_host_info_printf("%s: smb_opendir %s %s\n", storage->prog_name, storage->url, strerror(errno));
                return FALSE;
            }
            if ( storage->samba.smbc_closedir(storage->samba.smb_context, fd) < 0 ) {
                do_log(LOG_WARNING, "smbc_closedir \"%s\" %s\n", storage->url, strerror(errno));
                return FALSE;
            }*/
            struct stat buf;

            if ( storage->samba.smbc_stat(storage->samba.smb_context, path, &buf) != 0 ) {
                return FALSE;
            }
            break;
        }
        case STORAGE_FILE: {
            /*DIR  *fd;
            if ( (fd = opendir(storage->url)) == NULL )  {
                do_log(LOG_WARNING, "opendir \"%s\" %s\n", storage->url, strerror(errno));
                send_error_message_width_host_info_printf("%s: opendir %s %s\n", storage->prog_name, storage->url, strerror(errno));
                return FALSE;
            }
            closedir(fd);*/
            struct stat buf;
            if ( stat(path, &buf) != 0 ) {
                return FALSE;
            }

            break;
        }
        case STORAGE_NULL:
            break;
        case STORAGE_NO:
            break;
    }
    return TRUE;
}
static int storage_file_storage_copy_samba(storage_t *storage, const char *infilename, const char *outfilename)
{
    FILE *fp;
    SMBCFILE *file;
    SMBCCTX *smb_context;
    smb_context = storage->samba.smb_context;

    if ( (fp = fopen(infilename, "rb")) == NULL )  {
        do_log(LOG_ERR, "Error opening file \"%s\": %s\n", infilename, strerror(errno));
        if ( errno == ENOENT ) {
            do_log(LOG_WARNING, "file not found \"%s\"", infilename);
            return FALSE;
        }
        storage->error = TRUE;
        return FALSE;
    }
    file = storage->samba.smbc_creat(smb_context, outfilename, 0);
    if ( !file ) {
        do_log(LOG_WARNING, "smb_creat \"%s\" %s\n", outfilename, strerror(errno));
        return FALSE;
    }

    int ret;
    size_t size;
    ret = TRUE;

    while ( !feof(fp) ) {
        size = fread(storage->buff, 1, COPY_BUFFER_LEN, fp);
        if ( size < 0 ) {
            do_log(LOG_ERR, "Error read file \"%s\": %s\n", infilename, strerror(errno));
            storage->error = TRUE;
            ret = FALSE;
            break;
        }
        if ( !size )
            break;
        if ( storage->samba.smbc_write(smb_context, file, storage->buff, size) != size ) {
            do_log(LOG_WARNING, "smbc_write \"%s\" %s\n", outfilename, strerror(errno));
            ret = FALSE;
            break;
        }
    }
    if ( storage->samba.smbc_close(smb_context, file) < 0 ) {
        do_log(LOG_WARNING, "smbc_close \"%s\" %s\n", outfilename, strerror(errno));
        ret = FALSE;
    }
    fclose(fp);
    return ret;

}

static int storage_file_storage_mkdir_samba(storage_t *storage, const char *infilename)
{
    SMBCCTX *smb_context;
    smb_context = storage->samba.smb_context;
    int res;

    res = storage->samba.smbc_mkdir(smb_context, infilename, ACCESSPERMS);

    if ( res != 0 ) {
        do_log(LOG_WARNING, "smbmc_mkdir \"%s\" %s [%d]", infilename, strerror(errno), errno);
    }
    return TRUE;
}
static int storage_file_storage_mkdir_file(storage_t *storage, const char *infilename)
{
    if ( mkdir(infilename, ACCESSPERMS) != 0 ) {
        do_log(LOG_WARNING, "mkdir \"%s\" %s\n", infilename, strerror(errno));
        return FALSE;
    }
    return TRUE;
}
static int storage_file_storage_rename_samba(storage_t *storage, const char *infilename, const char *outfilename)
{
    SMBCCTX *smb_context;
    smb_context = storage->samba.smb_context;
    int res;

    struct stat buf;

    if ( storage->samba.smbc_stat(smb_context, infilename, &buf) != 0 ) {
                do_log(LOG_ERR,"bla2");
        do_log(LOG_WARNING, "stat \"%s\" %s %d", infilename, strerror(errno), errno);
        return FALSE;
    }
#ifdef DEBUG
    else
	do_log(LOG_INFO, "size %.2f %s", buf.st_size/1024./1024., infilename);
#endif

    res = storage->samba.smbc_rename(smb_context, infilename, smb_context, outfilename);

    if ( res != 0 ) {
        do_log(LOG_WARNING, "smbmc_rename \"%s\" \"%s\" %s [%d]", infilename, outfilename, strerror(errno), errno);
	    if ( storage->samba.smbc_stat(smb_context, infilename, &buf) != 0 ) {
                do_log(LOG_ERR,"bla3");
    	    do_log(LOG_WARNING, "stat \"%s\" %s %d", infilename, strerror(errno), errno);
	    }
	    if ( storage->samba.smbc_stat(smb_context, outfilename, &buf) != 0 ) {
                do_log(LOG_ERR,"bla4");
    	    do_log(LOG_ERR, "stat \"%s\" %s %d", outfilename, strerror(errno), errno);
        	return FALSE;
	    }
    }
    return TRUE;
}
static int storage_file_storage_rename_file(storage_t *storage, const char *infilename, const char *outfilename)
{
    if ( rename(infilename, outfilename) != 0 ) {
        do_log(LOG_WARNING, "rename \"%s\" \"%s\" %s\n", infilename, outfilename, strerror(errno));
        return FALSE;
    }
    return TRUE;
}
static int storage_file_storage_copy_file(storage_t *storage, const char *infilename, const char *outfilename)
{
    FILE *fp, *of;
    do_log(LOG_INFO, "copy file \"%s\" to \"%s\"", infilename, outfilename);
    if ( (fp = fopen(infilename, "rb")) == NULL )  {
        do_log(LOG_ERR, "Error opening file \"%s\": %s\n", infilename, strerror(errno));
        if ( errno == ENOENT ) {
            do_log(LOG_WARNING, "file not found \"%s\"", infilename);
            return FALSE;
        }
        storage->error = TRUE;
        return FALSE;
    }
    if ( (of = fopen(outfilename, "wb+")) == NULL )  {
        do_log(LOG_ERR, "Error opening file \"%s\": %s\n", outfilename, strerror(errno));
        return FALSE;
    }

    int ret;
    size_t size;
    ret = TRUE;

    while ( !feof(fp) ) {
        size = fread(storage->buff, 1, COPY_BUFFER_LEN, fp);
        if ( size < 0 ) {
            do_log(LOG_ERR, "Error read file \"%s\": %s\n", infilename, strerror(errno));
            storage->error = TRUE;
            ret = FALSE;
            break;
        }
        if ( !size )
            break;
        if ( fwrite(storage->buff, 1, size, of) != size ) {
            do_log(LOG_WARNING, "Error write file \"%s\" %s\n", outfilename, strerror(errno));
            ret = FALSE;
            break;
        }
    }
    fclose(fp);
    fclose(of);
    do_log(LOG_INFO, "successful copy file \"%s\" to \"%s\"", infilename, outfilename);
    return ret;

}
static int storage_file_storage_copy(storage_t *storage, const char *infilename, const char *outfilename)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            return storage_file_storage_copy_samba(storage, infilename, outfilename);
        case STORAGE_FILE:
            return storage_file_storage_copy_file(storage, infilename, outfilename);
        case STORAGE_NULL:
            return TRUE;
        case STORAGE_NO:
            return FALSE;
    }
    return FALSE;
}
static int storage_file_storage_rename(storage_t *storage, const char *infilename, const char *outfilename)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            return storage_file_storage_rename_samba(storage, infilename, outfilename);
        case STORAGE_FILE:
            return storage_file_storage_rename_file(storage, infilename, outfilename);
        case STORAGE_NULL:
            return TRUE;
        case STORAGE_NO:
            return FALSE;
    }
    return FALSE;
}

static int storage_file_storage_mkdir(storage_t *storage, const char *infilename)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            return storage_file_storage_mkdir_samba(storage, infilename);
        case STORAGE_FILE:
            return storage_file_storage_mkdir_file(storage, infilename);
        case STORAGE_NULL:
            return TRUE;
        case STORAGE_NO:
            return FALSE;
    }
    return FALSE;
}

#ifdef USE_POSTGRESQL
static int  storage_db_insert_info(storage_t *storage, const char *cam, const char *filename, struct tm start_time, struct tm end_time, double correction)
{
    if ( !storage->conninfo )
        return TRUE;

    if ( !connect_db(storage) ) {
        return FALSE;
    }

    PGresult *res;
    int status;
    int ret = TRUE;
    char sql[1024];
    char correction_str[50];
    char *ch;
    sprintf(correction_str, "%49f", correction);
 	ch = strchr (correction_str, ',');
 	if ( ch ) *ch = '.';
 	time_t t1, t2;
 	t1 = mktime(&start_time);
 	t2 = mktime(&end_time);


 	if ( t1 != t2 ) {
        sprintf(sql, "SELECT * FROM pieces_insert_with_correction('%s','%04d-%02d-%02d %02d:%02d:%02d','%04d-%02d-%02d %02d:%02d:%02d','%s',%s)",
                    cam,
                    start_time.tm_year+1900, start_time.tm_mon+1, start_time.tm_mday,
                    start_time.tm_hour, start_time.tm_min, start_time.tm_sec,
                    end_time.tm_year+1900, end_time.tm_mon+1, end_time.tm_mday,
                    end_time.tm_hour, end_time.tm_min, end_time.tm_sec,
                    filename,
                    correction_str
                    );
 	}
    else {
        sprintf(sql, "SELECT * FROM pieces_insert_with_correction('%s','%04d-%02d-%02d %02d:%02d:%02d',NULL,'%s',%s)",
                    cam,
                    start_time.tm_year+1900, start_time.tm_mon+1, start_time.tm_mday,
                    start_time.tm_hour, start_time.tm_min, start_time.tm_sec,
                    filename,
                    correction_str
                    );

    }

    res=PQexec(storage->conn, sql);
    status = PQresultStatus(res);

    if ( status != PGRES_COMMAND_OK &&
         status != PGRES_TUPLES_OK ) {
        pg_log(storage->conn, "Select");
        ret = FALSE;
    }
    PQclear(res);
    close_db(storage);
    return ret;
}
#endif

static int storage_queue(storage_t *storage)
{
    if ( !storage->queue->count  )
        return TRUE;

    if ( !storage_file_storage_is_available(storage) ) {
        return FALSE;
    }
    struct tm end_tm;
    time_t t_end;
    char *outname;
    char *filename;
    char *fullfilename;
    char *path;
    char *path_file;
    queue_item_t *item;

    item = storage->queue->list[0];
    t_end = mktime(&item->start_time) + item->duration;
    end_tm = *localtime(&t_end);

    filename = (char*)malloc(strlen(item->name ? item->name : storage->filename)+50);
    if ( item->duration )
        sprintf(filename, "%s-%04d%02d%02d%02d%02d%02d-%04d%02d%02d%02d%02d%02d.avi",
            item->name ? item->name : storage->filename,
            item->start_time.tm_year+1900, item->start_time.tm_mon+1, item->start_time.tm_mday,
            item->start_time.tm_hour, item->start_time.tm_min, item->start_time.tm_sec,
            end_tm.tm_year+1900, end_tm.tm_mon+1, end_tm.tm_mday, end_tm.tm_hour, end_tm.tm_min, end_tm.tm_sec
            );
    else
        sprintf(filename, "%s-%04d%02d%02d%02d%02d%02d.avi",
            item->name ? item->name : storage->filename,
            item->start_time.tm_year+1900, item->start_time.tm_mon+1, item->start_time.tm_mday,
            item->start_time.tm_hour, item->start_time.tm_min, item->start_time.tm_sec
            );
    path_file = do_strdup_printf("%4.4d%2.2d%2.2d", item->start_time.tm_year+1900, item->start_time.tm_mon+1, item->start_time.tm_mday);
    path = do_strdup_printf("%s/%s", storage->url, path_file);
    if ( !storage_file_storage_exists(storage, path)  ) {
        if ( !storage_file_storage_mkdir(storage, path) ) {
            send_error_message_width_host_info_printf("%s: Error mkdir \"%s\"", storage->prog_name, path);
            do_free(path);
            do_free(path_file);
            return FALSE;
        }

    }
    outname = do_strdup_printf("%s/%s",path, filename);
    do_free(path);
    fullfilename = do_strdup_printf("%s/%s",path_file,filename);
    do_free(path_file);
    if ( !item->copy_complect ) {
        if ( item->rename_only ) {
            if ( !storage_file_storage_rename(storage, item->filename, outname) )
                send_error_message_width_host_info_printf("%s: Error rename files \"%s\" \"%s\"", storage->prog_name, item->filename, outname);
        }
        else {
            if ( !storage_file_storage_copy(storage, item->filename, outname) )
                send_error_message_width_host_info_printf("%s: Error copy files \"%s\" \"%s\"", storage->prog_name, item->filename, outname);
        }
    }
#ifdef USE_POSTGRESQL
    int i;
    for ( i = 0; i < 10; i++ ) {
	if ( !storage_db_insert_info(storage, item->name ? item->name : storage->filename, fullfilename, item->start_time, end_tm, item->correction) ) {
	    do_log(LOG_ERR, "Error insert into db \"%s\"", storage->conninfo);
    	    send_error_message_width_host_info_printf("%s: Error insert into db \"%s\"", storage->prog_name, storage->conninfo);
    	    if ( i == 9 ) {
            do_free(fullfilename);
    		do_free(filename);
		do_free(outname);
		item->copy_complect = TRUE;
		return FALSE;
	    }
    	    DO_SEC_SLEEP(10);
    	}
    	else
    	    break;
    }
#endif
    do_free(fullfilename);
    do_free(filename);
    do_free(outname);
    if ( storage->error )
        return FALSE;
    int ret = TRUE;
    if (!item->rename_only &&  remove(item->filename) == -1) {
        do_log(LOG_ERR, "remove file \"%s\" %d %s\n", item->filename, errno, strerror(errno));
        if ( errno != ENOENT ) {
            storage->error = TRUE;
            return FALSE;
        }
        ret = FALSE;
    }
    mymutex_lock(storage->mutex);
    queue_item_free(item);
    do_list_delete(storage->queue, 0);
    mymutex_unlock(storage->mutex);
    return ret;
}

static void storage_run(storage_t *storage)
{
    /*if ( !storage_init_storage(storage) ) {
        storage->error = TRUE;
        return;
    }*/
    while ( TRUE ) {
        if ( storage->error ||
             storage->thread_stoped )
             break;

        if ( !storage->queue->count ) {
            DO_SEC_SLEEP(1);
            continue;
        }
        if ( !storage_queue(storage) )
            DO_SEC_SLEEP(10);
    }
    storage_close_storage(storage);
}
void storage_set_no_thread(storage_t *storage, int no_thread)
{
    storage->no_thread = no_thread;
}
static size_t storage_get_free_space(storage_t *storage);
//static int storage_delete_file(storage_t *storage, const char *filename);
int storage_clean_storage(storage_t *storage, size_t minimum, size_t reserve)
{

    size_t size=0;
    size = storage_get_free_space(storage);
    do_log(LOG_INFO, "storage capacity %.2fGib min %.2fGib", size/1024./1024./1024.,minimum/1024./1024./1024.);
    if ( size > minimum )
        return TRUE;

    if ( !storage->conninfo )
        return TRUE;
    if ( !connect_db(storage) )
        return FALSE;

    PGresult *res;
    int status;
    int ret = TRUE;
    char sql[1024];
    int nrows, i;
#ifdef DEBUG
    do_log(LOG_INFO, "clean storage");
#endif

    sprintf(sql, "SELECT filename,cam,time_start FROM pieces ORDER BY time_start LIMIT 10000");

    res=PQexec(storage->conn, sql);
    status = PQresultStatus(res);

    if ( status != PGRES_COMMAND_OK &&
         status != PGRES_TUPLES_OK ) {
        pg_log(storage->conn, "Select");
        ret = FALSE;
    }

    nrows = PQntuples(res);
    for ( i = 0; i < nrows && ret; i++ ) {
        char *filename, *path;
        PGresult *res1;
        filename  = PQgetvalue(res, i, 0);
        path = do_malloc0(strlen(storage->url)+strlen(filename)+20);
        sprintf(path, "%s/%s", storage->url, filename);
        if ( !storage_delete_file(storage, path) ) {
            ret = FALSE;
            break;
        }
        sprintf(sql, "DELETE FROM pieces WHERE cam='%s' AND time_start='%s'", PQgetvalue(res, i, 1), PQgetvalue(res, i, 2));
        res1 = PQexec(storage->conn, sql);
        status = PQresultStatus(res1);
        PQclear(res1);
        if ( status != PGRES_COMMAND_OK &&
             status != PGRES_TUPLES_OK ) {
            pg_log(storage->conn, "Select");
            ret = FALSE;
            break;
        }
        size = storage_get_free_space(storage);
        if ( size > minimum + reserve )
            break;
#ifdef DEBUG
        if ( !(i % 100) )
            do_log(LOG_INFO, "clean %d records. capacity %.2fMib", i, size/1024./1024./1024.);
#endif
    }
#ifdef DEBUG
    if ( !(i % 100) )
         do_log(LOG_INFO, "clean %d records. capacity %.2fMib", i, size/1024./1024./1024.);
#endif
    PQclear(res);
    close_db(storage);
    return ret;

}
static size_t storage_get_samba_free_space(storage_t *storage);
static size_t storage_get_file_free_space(storage_t *storage);

size_t storage_get_free_space(storage_t *storage)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            return storage_get_samba_free_space(storage);
        case STORAGE_FILE:
            return storage_get_file_free_space(storage);
        case STORAGE_NULL:
        case STORAGE_NO:
            return SIZE_MAX;
    }
    return SIZE_MAX;
}

static size_t storage_get_samba_free_space(storage_t *storage)
{
    struct statvfs st;

    if ( storage->samba.smbc_statvfs(storage->samba.smb_context, storage->fs, &st) != 0 ) {
                do_log(LOG_ERR,"bla5");
        do_log(LOG_WARNING, "stat \"%s\" %s\n", storage->fs, strerror(errno));
        send_error_message_width_host_info_printf("%s: statvfs %s %s\n", storage->prog_name, storage->fs, strerror(errno));
        return SIZE_MAX;
    }
    return st.f_bsize * st.f_bfree * ((st.f_frsize == 0) ? 1 : st.f_frsize);

}
static size_t storage_get_file_free_space(storage_t *storage)
{
    struct statfs st;
    if ( statfs(storage->fs, &st) != 0 ) {
                do_log(LOG_ERR,"bla6");
        do_log(LOG_WARNING, "stat \"%s\" %s\n", storage->fs, strerror(errno));
        send_error_message_width_host_info_printf("%s: statvfs %s %s\n", storage->prog_name, storage->fs, strerror(errno));
        return SIZE_MAX;
    }
    size_t size;
//    size = (st.f_bsize * st.f_bfree);
    size = (st.f_bsize * st.f_bavail);
    return size;
}
static int storage_samba_delete_file(storage_t *storage, const char *filename);
static int storage_file_delete_file(storage_t *storage, const char *filename);
int storage_delete_file(storage_t *storage, const char *filename)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            return storage_samba_delete_file(storage, filename);
        case STORAGE_FILE:
            return storage_file_delete_file(storage, filename);
        case STORAGE_NULL:
        case STORAGE_NO:
            return TRUE;
    }
    return TRUE;
}
static int storage_samba_delete_file(storage_t *storage, const char *filename)
{
    if ( storage->samba.smbc_unlink(storage->samba.smb_context, filename) != 0 ) {
        if ( errno == ENOENT ) {
            do_log(LOG_WARNING, "file not found \"%s\"", filename);
            return FALSE;
        }
        do_log(LOG_WARNING, "unlink \"%s\" %s\n", filename, strerror(errno));
        send_error_message_width_host_info_printf("%s: unlink %s %s\n", storage->prog_name, filename, strerror(errno));
        return FALSE;
    }
    return TRUE;
}
static int storage_file_delete_file(storage_t *storage, const char *filename)
{
    if ( remove(filename) != 0 ) {
        if ( errno == ENOENT )
            return TRUE;
        do_log(LOG_WARNING, "remove \"%s\" %s\n", filename, strerror(errno));
        send_error_message_width_host_info_printf("%s: remove %s %s\n", storage->prog_name, filename, strerror(errno));
        return FALSE;
    }
    return TRUE;
}

static int storage_get_list_fileinfo_file(storage_t *storage, const char *path,
                     do_string_list_t *masks, int depth,
                     struct tm *tm, do_list_t *files);
static int storage_get_list_fileinfo_samba(storage_t *storage, const char *path,
                     do_string_list_t *masks, int depth,
                     struct tm *tm, do_list_t *files);

int storage_get_list_fileinfo(storage_t *storage, const char *path,
                     do_string_list_t *masks, int depth,
                     struct tm *tm, do_list_t *files)
{
    switch ( storage->kind ) {
        case STORAGE_SAMBA:
            return storage_get_list_fileinfo_samba(storage,path,masks,depth,tm,files);
        case STORAGE_FILE:
            return storage_get_list_fileinfo_file(storage,path,masks,depth,tm,files);
        case STORAGE_NULL:
            return TRUE;
        case STORAGE_NO:
            return TRUE;
    }
    return FALSE;
}
int storage_get_list_fileinfo_file(storage_t *storage, const char *path_,
                     do_string_list_t *masks, int depth,
                     struct tm *tm, do_list_t *files)
{

    GRegex     *re = NULL;
    GMatchInfo *match_info = NULL;
    GError     *error = NULL;
    int ret;
    ret = TRUE;

    re = g_regex_new(masks->list[depth],
                     G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, &error);
    if ( !re ) {
        g_error_free(error);
        do_log(LOG_ERR, "Format \"%s\" error : %s", masks->list[depth], error->message);
        send_error_message_width_host_info_printf("%s: Format \"%s\" error : %s", storage->prog_name, masks->list[depth], error->message);
        return FALSE;
    }

    DIR *fd;
    //do_log(LOG_INFO, "opendir \"%s\"", path_);
    char *path;
    path = (char*)path_;
    const char *fprefix="file://";
    if ( !strncmp(path, fprefix, strlen(fprefix)) )
        path+=strlen(fprefix);
    //do_log(LOG_DEBUG, "opendir %s", path);
    if ( (fd = opendir(path)) == NULL ) {
        do_log(LOG_WARNING, "opendir \"%s\" %s\n", path, strerror(errno));
        send_error_message_width_host_info_printf("%s: smb_opendir %s %s\n", storage->prog_name, path, strerror(errno));
        g_regex_unref(re);
        return FALSE;
    }
    struct dirent *dirent;
    int empty = TRUE;

    while ( (dirent = readdir(fd)) != NULL) {

		if (
             !strcmp (dirent->d_name, ".") || !strcmp (dirent->d_name, ".."))
			continue;

        empty = FALSE;
        g_regex_match (re, dirent->d_name, 0, &match_info);

        if ( g_match_info_matches (match_info) ) {
            //do_log(LOG_DEBUG, "re file %s TRUE", dirent->d_name);
            char *value;
            value = g_match_info_fetch_named(match_info, "year");
            if ( value )
                tm->tm_year = atoi(value) - 1900;
            value = g_match_info_fetch_named(match_info, "month");
            if ( value )
                tm->tm_mon = atoi(value) - 1;
            value = g_match_info_fetch_named(match_info, "day");
            if ( value )
                tm->tm_mday = atoi(value);
            value = g_match_info_fetch_named(match_info, "hour");
            if ( value )
                tm->tm_hour = atoi(value);
            value = g_match_info_fetch_named(match_info, "minute");
            if ( value )
                tm->tm_min = atoi(value);
            value = g_match_info_fetch_named(match_info, "second");
            if ( value )
                tm->tm_sec = atoi(value);
            if ( depth < masks->count - 1 ) {
                if ( dirent->d_type == DT_DIR ) {
                    char *new_path;
                    new_path = do_malloc0(strlen(path)+strlen(dirent->d_name)+10);
                    sprintf(new_path,"%s/%s", path, dirent->d_name);
                    storage_get_list_fileinfo(storage, new_path, masks, depth+1, tm, files);
                    g_free(new_path);
                }
            }
            else {
#ifdef DEBUG
                do_log(LOG_INFO, "%s", dirent->d_name);
#endif
                if ( tm->tm_year == -1 ||
                     tm->tm_mon == -1 ||
                     tm->tm_mday == -1 ||
                     tm->tm_hour == -1 ||
                      tm->tm_min == -1 ) {
                    do_log(LOG_ERR, "Time not define \"%s\"", path);
                    send_error_message_width_host_info_printf("%s: Time not define \"%s\"", storage->prog_name, path);
                    ret = FALSE;
                    break;
                }
                fileinfo_t info;
                info.tm = *tm;
                int pid;
                //info.path = do_strdup(path);
                info.filename = do_malloc0(strlen(path)+strlen(dirent->d_name)+10);
                sprintf(info.filename,"%s/%s", path, dirent->d_name);
                pid = scan_procs(dirent->d_name);
                if ( pid == -1 ) {
                     do_list_add_alloc(files, &info, sizeof(info));
                }
                else {
#ifdef DEBUG
                     do_log(LOG_INFO, "file \"%s\" is locked pid %d", info.filename, pid);
#endif
                     do_free(info.filename);
                }
            }
        }
        /*else {
            do_log(LOG_DEBUG, "re file %s FALSE", dirent->d_name);
        }*/
        g_match_info_free(match_info);
	}
    g_regex_unref(re);
    if ( closedir(fd) < 0 ) {
        do_log(LOG_WARNING, "closedir \"%s\" %s\n", path, strerror(errno));
        return FALSE;
    }
#ifdef DELETE_DIR
    if ( empty && depth && tm->tm_year != -1 && tm->tm_mon != -1 && tm->tm_mday != -1 ) {
        time_t t1 = time(NULL) - 360;
        struct tm tm1;
        tm1 = *localtime(&t1);
        if ( tm1.tm_year > tm->tm_year ||
             tm1.tm_mon  > tm->tm_mon ||
             tm1.tm_mday > tm->tm_mday ) {
            if ( rmdir(path) < 0 ) {
                do_log(LOG_WARNING, "rmdir \"%s\" %s\n", path, strerror(errno));
            }
        }
    }
#endif
    return ret;
}
int storage_get_list_fileinfo_samba(storage_t *storage, const char *path,
                     do_string_list_t *masks, int depth,
                     struct tm *tm, do_list_t *files)
{

    GRegex     *re = NULL;
    GMatchInfo *match_info = NULL;
    GError     *error = NULL;
    int ret;
    ret = TRUE;

    re = g_regex_new(masks->list[depth],
                     G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, &error);
    if ( !re ) {
        g_error_free(error);
        do_log(LOG_ERR, "Format \"%s\" error : %s", masks->list[depth], error->message);
        send_error_message_width_host_info_printf("%s: Format \"%s\" error : %s", storage->prog_name, masks->list[depth], error->message);
        return FALSE;
    }

    SMBCFILE *fd;
    if ( (fd = storage->samba.smbc_opendir(storage->samba.smb_context, path)) == NULL ) {
        do_log(LOG_WARNING, "smb_opendir \"%s\" %s\n", path, strerror(errno));
        send_error_message_width_host_info_printf("%s: smb_opendir %s %s\n", storage->prog_name, path, strerror(errno));
        g_regex_unref(re);
        return FALSE;
    }
    struct smbc_dirent *dirent;
    int empty = TRUE;

    while ( (dirent = storage->samba.smbc_readdir(storage->samba.smb_context, fd)) != NULL) {

		if ( !(dirent->smbc_type == SMBC_FILE || dirent->smbc_type == SMBC_DIR) ||
             !strcmp (dirent->name, ".") || !strcmp (dirent->name, ".."))
			continue;

        empty = FALSE;
        g_regex_match (re, dirent->name, 0, &match_info);

        if ( g_match_info_matches (match_info) ) {
            char *value;
            value = g_match_info_fetch_named(match_info, "year");
            if ( value )
                tm->tm_year = atoi(value) - 1900;
            value = g_match_info_fetch_named(match_info, "month");
            if ( value )
                tm->tm_mon = atoi(value) - 1;
            value = g_match_info_fetch_named(match_info, "day");
            if ( value )
                tm->tm_mday = atoi(value);
            value = g_match_info_fetch_named(match_info, "hour");
            if ( value )
                tm->tm_hour = atoi(value);
            value = g_match_info_fetch_named(match_info, "minute");
            if ( value )
                tm->tm_min = atoi(value);
            value = g_match_info_fetch_named(match_info, "second");
            if ( value )
                tm->tm_sec = atoi(value);
            if ( depth < masks->count - 1 ) {
                if ( dirent->smbc_type == SMBC_DIR ) {
                    char *new_path;
                    new_path = do_malloc0(strlen(path)+strlen(dirent->name)+10);
                    sprintf(new_path,"%s/%s", path, dirent->name);
                    storage_get_list_fileinfo(storage, new_path, masks, depth+1, tm, files);
                    g_free(new_path);
                }
            }
            else {
#ifdef DEBUG
                do_log(LOG_INFO, "%s", dirent->name);
#endif
                if ( tm->tm_year == -1 ||
                     tm->tm_mon == -1 ||
                     tm->tm_mday == -1 ||
                     tm->tm_hour == -1 ||
                      tm->tm_min == -1 ) {
                    do_log(LOG_ERR, "Time not define \"%s\"", path);
                    send_error_message_width_host_info_printf("%s: Time not define \"%s\"", storage->prog_name, path);
                    ret = FALSE;
                    break;
                }
                fileinfo_t info;
                info.tm = *tm;
                //info.path = do_strdup(path);
                info.filename = do_malloc0(strlen(path)+strlen(dirent->name)+10);
                sprintf(info.filename,"%s/%s", path, dirent->name);
                do_list_add_alloc(files, &info, sizeof(info));
            }
        }
        g_match_info_free(match_info);
	}
    g_regex_unref(re);
    if ( storage->samba.smbc_closedir(storage->samba.smb_context, fd) < 0 ) {
        do_log(LOG_WARNING, "smbc_closedir \"%s\" %s\n", path, strerror(errno));
        return FALSE;
    }
    if ( empty && depth && tm->tm_year != -1 && tm->tm_mon != -1 && tm->tm_mday != -1 ) {
        time_t t1 = time(NULL) - 360;
        struct tm tm1;
        tm1 = *localtime(&t1);
        if ( tm1.tm_year > tm->tm_year ||
             tm1.tm_mon  > tm->tm_mon ||
             tm1.tm_mday > tm->tm_mday ) {
            if ( storage->samba.smbc_rmdir(storage->samba.smb_context, path) < 0 ) {
                do_log(LOG_WARNING, "smbc_rmdir \"%s\" %s\n", path, strerror(errno));
            }
        }

    }

    return ret;
}
void fileinfo_free(fileinfo_t *i)
{
//    do_free(i->path);
    do_free(i->filename);
    do_free(i);
}
#ifdef _LINUX
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


#define MAX_PATHNAME 1024

static int
check_dir(const pid_t pid, const char *dirname, const char *name)
{
	DIR *dirp;
	struct dirent *direntry;
	char dirpath[MAX_PATHNAME];
	char filepath[MAX_PATHNAME];
	char filename[MAX_PATHNAME];
    int res = -1;
	snprintf(dirpath, MAX_PATHNAME, "/proc/%d/%s", pid, dirname);
	if ((dirp = opendir(dirpath)) == NULL)
		return res;

	while ((direntry = readdir(dirp)) != NULL) {
		if (direntry->d_name[0] < '0' || direntry->d_name[0] > '9')
			continue;
        int n;

		snprintf(filepath, MAX_PATHNAME, "/proc/%d/%s/%s",
			 pid, dirname, direntry->d_name);
 		n=readlink(filepath, filename, MAX_PATHNAME-1);
 		filename[n]='\0';
        if ( strlen(filename) >= strlen(name) ) {
	    if ( !strcmp(filename+(strlen(filename)-strlen(name)), name) ) {
            res = pid;
            break;
            }
    	    }
	}
	closedir(dirp);
	return res;
}
static int
scan_procs(const char *name)
{
	DIR *topproc_dir;
	struct dirent *topproc_dent;
	pid_t pid, my_pid;

	if ((topproc_dir = opendir("/proc")) == NULL) {
		fprintf(stderr, "Cannot open /proc directory: %s\n",
			strerror(errno));
		return -1;
	}
	my_pid = getpid();
	int res = -1;
	while ((topproc_dent = readdir(topproc_dir)) != NULL) {

		if (topproc_dent->d_name[0] < '0' || topproc_dent->d_name[0] > '9')	// Not a process
			continue;
		pid = atoi(topproc_dent->d_name);
		// Dont print myself
		if (pid == my_pid)
			continue;
		//uid = getpiduid(pid);

		res = check_dir(pid, "fd", name);
		if ( res != -1 )
            break;

	}
	closedir(topproc_dir);
	return res;
}
#else
static int scan_proc(const char *name) {return -1;}
#endif
