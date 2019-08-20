#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

#include <time.h>
#include <dolib.h>

typedef struct _storage_t storage_t;
#define STORAGE_DEFAULT_QUEUE_LEN 10

typedef struct {
    struct tm tm;
    //char *path;
    char *filename;
} fileinfo_t;

#ifdef __cplusplus
extern "C"
{
#endif

storage_t *storage_new(int queue_len, const char *prog_name);

// url support:
// 1) smb://[[domain;]username[:password]@]server/share/path
// 2) file://path
// 3) null:

int storage_set_file_storage_param(storage_t *storage, const char *url, const char *filename);

#ifdef USE_POSTGRESQL
int storage_set_db_storage_param(storage_t *storage, const char *conninfo);
#endif
int  storage_add_file(storage_t *storage, const char *filename, struct tm start_time, int duration_sec, double correction);
void storage_set_no_thread(storage_t *storage, int no_thread);

int  storage_add_file2(storage_t *storage, const char *filename, struct tm start_time, int duration_sec, double correction, const char *name, int rename_only);
int  storage_clean_storage(storage_t *storage, size_t minimum, size_t reserve);
int  storage_delete_file(storage_t *storage, const char *filename);


int  storage_get_list_fileinfo(storage_t *storage, const char *path, do_string_list_t *masks, int depth, struct tm *tm, do_list_t *files);
void fileinfo_free(fileinfo_t *i);


void storage_free(storage_t *storage);
int  storage_stop(storage_t *storage);

#ifdef __cplusplus
}
#endif



#endif // STORAGE_H_INCLUDED
