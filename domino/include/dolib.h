
#ifndef DO_LIB_H
#define DO_LIB_H

#if !defined(_WIN32) && !defined(_LINUX)
#error You must define one of the following: _WIN32, _LINUX
#endif

#if     __GNUC__ >= 4
#define DO_GNUC_NULL_TERMINATED __attribute__((__sentinel__))
#else
#define DO_GNUC_NULL_TERMINATED
#endif

#include <def.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifndef HAVE_SYSLOG_H
#include "./win32/syslog.h"
#endif
#include <iconv.h>
#ifdef _WIN32
#include <windows.h>
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define DO_N_ELEMENTS(arr) (sizeof (arr) / sizeof ((arr)[0]))

typedef enum {
    DO_LOG_TYPE_STDOUT,
    DO_LOG_TYPE_SYSLOG,
    DO_LOG_TYPE_FILE
} do_log_type_t;
typedef int  (*do_log_func)(const char *mgs, int level, void *user_data);
#define ZONE __FILE__,__LINE__

typedef struct _do_list_t do_list_t;
typedef struct _do_list_t do_sort_list_t;
typedef struct _do_list_t do_string_list_t;
struct _do_list_t {
    void **list;
    int count;
};
typedef int  (*do_list_cmp_func) (void *, void *, void *user_data);
typedef int  (*do_list_foreach_func)(void *, void *user_data);

typedef enum {
    do_lx_equal,
    do_lx_question,
    do_lx_cinteger,
    do_lx_cstring,
    do_lx_cdate,
    do_lx_minus,
    do_lx_plus,
    do_lx_creal,
    do_lx_ident,
    do_lx_colon,
    do_lx_comma,
    do_lx_end,
    do_lx_error
} do_lex_t;

#define DO_MAX_LEXEM_LEN 1024
typedef struct {
    int crnt;
    int crntln;
    char lexem[DO_MAX_LEXEM_LEN + 1];
    char *text;
    do_lex_t lex;
    iconv_t conv;
} do_scanner_t;


typedef struct _do_data_t do_data_t;
#define DO_EXCH_DATA_TYPE_RAW           1   // raw (default)
#define DO_EXCH_DATA_TYPE_OK            2   // empty
#define DO_EXCH_DATA_TYPE_ERROR         3   // string
#define DO_EXCH_DATA_TYPE_INFO_MESSAGE  4   // string
#define DO_EXCH_DATA_TYPE_INFO_PERCENT  5   // int32_t for recent
#define DO_EXCH_DATA_TYPE_INFO_NUMBER   6   // int32_t for number

#define DO_EXCH_DATA_TYPE_NO            7   //

#define DO_IS_EXCH_DATA_INFO(a) (a == DO_EXCH_DATA_TYPE_INFO_MESSAGE || \
                                   a == DO_EXCH_DATA_TYPE_INFO_PERCENT || \
                                   a == DO_EXCH_DATA_TYPE_INFO_NUMBER)
// change DO_DATA_CHECK
#ifdef ZLIB
#define DO_EXCH_FLAG_ZIP 1  // data is packed by zlib. Prefix - unit32_t unpack_size
#define UNPACK_SIZE_TYPE uint32_t
#endif
#ifdef GCRYPT
#define DO_EXCH_FLAG_AES 2  // data is crypted by AES.
#define DO_EXCH_FLAG_MD5 4  // Prefix - MD5 HASH
#endif
#define DO_EXCH_FLAG_LOCALE 8
#define DO_EXCH_FLAG_NO    16
#define DO_EXCH_LOCALE_NAME_SIZE 80
#define DO_EXCH_OK "ok"
// change DO_DATA_CHECK

typedef uint32_t do_exch_flag_t;
typedef uint32_t do_size_t;

typedef struct PACK_STRUCT {
    uint32_t size;
    uint32_t flags;
    uint32_t type;
} do_exch_header_t;

#define DO_EXCH_HEADER_CHECK(header) (header.type > 0 && header.type < DO_EXCH_DATA_TYPE_NO && header.flags < DO_EXCH_FLAG_NO)

static const uint32_t do_exch_support_flags = 0
#ifdef ZLIB
                      + DO_EXCH_FLAG_ZIP
#endif
#ifdef GCRYPT
                      + DO_EXCH_FLAG_AES + DO_EXCH_FLAG_MD5
#endif
                + DO_EXCH_FLAG_LOCALE
                ;
typedef struct _do_rpc_client_t do_rpc_client_t;
typedef struct _do_rpc_t do_rpc_t;

typedef struct {
    char *name;
    char *desc;
    char *help;
    int  level;
    int  data;
} do_rpc_command_t;
typedef struct {
    do_lex_t type;
    union {
        int       integer;
        char     *string;
        struct tm date;
        double    real;
    } value;
} do_rpc_command_param_t;
typedef void (do_rpc_extended_func)(int client_id, void *owner, int command, do_list_t *params, do_data_t *data, do_data_t *out);


typedef enum {
    DO_STATE_STOP = 0,
    DO_STATE_RUN,
    DO_STATE_PAUSE,
    DO_STATE_STOPING,
    DO_STATE_STARTING,
    DO_STATE_PAUSING,
    DO_STATE_RESUMING,
    DO_STATE_RESTARTING,
    DO_STATE_UNDEFINE,
} do_state_t;

typedef struct _do_proc_state_t do_proc_state_t;
typedef void (do_proc_state_callback_t)(do_proc_state_t *proc_state, void *arg);

typedef int (do_main_handler_t)(int argc, char** argv);
typedef struct _do_xls_t do_xls_t;

#define DO_UTF_CHARSET "UTF-8"
#define DO_WINDOWS_CHARSET "CP1251"


#include <time.h>
#include <stdio.h>
#include <unistd.h>

//#if defined(_WIN32)
//#include <windows.h>
//#define usleep(a) Sleep(a/1000.)
//#else
//#include <unistd.h>
//#endif
#define DO_SEC_SLEEP(a) do_sleep_ms(a*1000)

#ifdef __cplusplus
extern "C"
{
#endif

void    do_log_set_type(const char *ident, do_log_type_t type, const char* filename);
#define do_log_set_stdout() do_log_set_type(NULL, DO_LOG_TYPE_STDOUT, NULL)
void    do_log(int level, const char *msgfmt, ...);
void    do_log_set_log_func(do_log_func func, void *user_data);
#ifdef DEBUG
void    do_log_debug(const char *file, int line, const char *msgfmt, ...);
#else
#define do_log_debug(...)
#endif


do_scanner_t *do_scanner_init(do_scanner_t *scanner, char *text, iconv_t conv);
do_lex_t      do_scanner_step(do_scanner_t *scanner);
do_scanner_t *do_scanner_copy(do_scanner_t *dest, do_scanner_t *src);
char         *do_scanner_pos(do_scanner_t *scanner);

int do_authenticate(const char *name, const char *password, int *userlevel);
void do_set_passwd_filename(const char *filename);
int do_useradd(const char *name, const char *password, int userlevel);
int do_userdel(const char *name, const char *password, int userlevel);

char *do_strdup_printf(const char *msgfmt, ...);
char *do_strdup_path_join(const char *separator, const char *first_element, ...) DO_GNUC_NULL_TERMINATED;
#define DOMINO_CONFIG(a) do_strdup_path_join(NULL, DOMINO_PATH, a, NULL)
#define DO_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

#ifdef MEMORY_DEBUG
void *do_malloc_(const char *file, int line, size_t size);
void *do_malloc0_(const char *file, int line, size_t size);
void *do_realloc_(const char *file, int line, void *buf, size_t size);
void *do_calloc_(const char *file, int line, size_t size1, size_t size2);
void  do_free_(const char *file, int line, void *ptr);
char *do_strdup_(const char *file, int line, const char* str);
char *do_strndup_(const char *file, int line, const char* str, size_t size);
void  do_initmemdebug();
void  do_showmemdebug();
#define do_malloc(a) do_malloc_(__FILE__,__LINE__,a)
#define do_malloc0(a) do_malloc0_(__FILE__,__LINE__,a)
#define do_realloc(a,b) do_realloc_(__FILE__,__LINE__,a,b)
#define do_calloc(a,b) do_calloc_(__FILE__,__LINE__,a,b)
#define do_free(a) do_free_(__FILE__,__LINE__,a)
#define do_strdup(a) do_strdup_(__FILE__,__LINE__,a)
#define do_strndup(a,b) do_strndup_(__FILE__,__LINE__,a,b)
#else
#define do_malloc(a) malloc(a)
#define do_malloc0(size) memset(malloc(size), 0, size)
#define do_realloc(a,b) realloc(a,b)
#define do_calloc(a,b) calloc(a,b)
#define do_free(a) free(a)
#define do_strdup(a) strdup(a)

#ifdef _WIN32
const char *do_get_libdir();
#define do_strndup(a,b) do_strndup_(a,b)
char *do_strndup_(const char* str, size_t size);
#else
#define do_strndup(a,b) strndup(a,b)
#endif
#endif

do_list_t *do_list_new(int freenode);
void       do_list_free(do_list_t *list);
void       do_list_clear(do_list_t *list);
void      *do_list_add(do_list_t *list, void *node);
void      *do_list_add_alloc(do_list_t *list, void *node, int size);
void      *do_list_insert(do_list_t *list, int indx, void *node);
void       do_list_foreach(do_list_t *list, do_list_foreach_func func, void *data);
void       do_list_delete(do_list_t *list, int indx);
do_string_list_t *do_string_list_new();
do_string_list_t *do_string_list_new_from_csv(const char *csv);
do_string_list_t *do_string_list_dup(do_string_list_t *list);

void              do_string_list_free(do_string_list_t *list);
char             *do_string_list_to_string(do_string_list_t *list);
char             *do_string_list_to_csv(do_string_list_t *list);
void              do_string_list_delete(do_string_list_t *list, int indx);
void              do_string_list_clear(do_string_list_t *list);
char             *do_string_list_insert(do_list_t *list, int indx, char *str);
char             *do_string_list_add_alloc(do_string_list_t *list, const char *str);
char             *do_string_list_addn_alloc(do_string_list_t *list, const char* str, int n);
char             *do_string_list_add(do_string_list_t *list, char *str);
char             *do_string_list_addf_alloc(do_string_list_t *list, const char* cformat, ...);
int               do_string_list_find(do_string_list_t *list, int* indx, const char *str);

do_sort_list_t *do_sort_list_new(int freenode, int uniq, do_list_cmp_func cmp_func, void *user_data);
void            do_sort_list_free(do_sort_list_t *list);
void            do_sort_list_clear(do_sort_list_t *list);
int             do_sort_list_add(do_sort_list_t *list, void *node);
int             do_sort_list_add_alloc(do_sort_list_t *list, void *node, size_t size);
void           *do_sort_list_insert(do_sort_list_t *list, int indx, void *node);
void           *do_sort_list_insert_new(do_sort_list_t *list, int indx, void *node, size_t size);
int             do_sort_list_find(do_sort_list_t *list, int *indx, void *node);
void            do_sort_list_set_cmp_proc(do_sort_list_t *list, do_list_cmp_func proc);
void            do_sort_list_set_cmpn_proc(do_sort_list_t *list, do_list_cmp_func proc);
int             do_sort_list_findn(do_sort_list_t *list, int *indx, void *node);
void            do_sort_list_delete(do_sort_list_t *list, int indx);
void            do_sort_list_delete_node(do_sort_list_t *list, void *node);



do_data_t *do_data_new();
do_data_t *do_data_new_from_src(void *src, size_t size);
int        do_data_set(do_data_t *data, void *src, size_t size);
int        do_data_set_data(do_data_t *data, void *src, size_t size);
int        do_data_type(do_data_t *data);
int        do_data_send_flags(do_data_t *data);
int        do_data_set_type(do_data_t *data, int type);
void*      do_data_get(do_data_t *data);
size_t     do_data_size(do_data_t *data);
void      *do_data_add(do_data_t *data, void *src, size_t size);
int        do_data_add_do_data(do_data_t *data, do_data_t *src);
void       do_data_set_send_flags(do_data_t *data, int flags);
int        do_data_set_err(do_data_t *data, const char *str);
int        do_data_set_errf(do_data_t *data, const char *cformat, ...);
int        do_data_set_str(do_data_t *data, const char *str);
int        do_data_set_strf(do_data_t *data, const char *cformat, ...);
int        do_data_add_str(do_data_t *data, const char *str);
int        do_data_add_strf(do_data_t *data, const char *cformat, ...);
int        do_data_copy(do_data_t *dst, do_data_t *src);
void       do_data_free(do_data_t *data);
int        do_data_alloc(do_data_t *data, size_t size);
int        do_data_realloc(do_data_t *data, size_t size);
void       do_data_clear(do_data_t *data);
//int do_data_safe_cp1251(do_data_t *data, const char *filename);
char      *do_data_error(do_data_t *data);
int        do_data_is_error(do_data_t *data);
char      *do_data_str(do_data_t *data);
int        do_data_read_from_file(do_data_t *data, const char* filename);
int        do_data_write_to_file(do_data_t *data, const char* filename);
int        do_data_list_count(do_data_t *data);
int        do_data_list_item_size(do_data_t *data, int indx);
void      *do_data_list_item(do_data_t *data, int indx);

do_rpc_client_t *do_rpc_client_new(const char *host, int port, const char *user, const char *password, int use_utf);
void             do_rpc_client_free(do_rpc_client_t *do_rpc_client);
int              do_rpc_client_crypt_init_key(do_rpc_client_t *do_rpc_client, const char *keyfile);
int              do_rpc_client_connect(do_rpc_client_t *do_rpc_client);
int              do_rpc_client_disconnect(do_rpc_client_t *do_rpc_client);
int              do_rpc_client_send_command(do_rpc_client_t *do_rpc_client, const char *command, const char *params, do_data_t *data, do_data_t *recv_data);

do_rpc_t *do_rpc_new(const char* name, char* ver,
                     int tcpport, int telnet_tcpport,
                     do_proc_state_t *proc_state,
                     const char* crypt_key_file);

void      do_rpc_free(do_rpc_t *ctrl);
int       do_rpc_close(do_rpc_t *ctrl, int wait);
void      do_rpc_set_extended(do_rpc_t *ctrl, void* owner, do_rpc_extended_func *func,
                              do_rpc_command_t *commands, int command_count);
void      do_rpc_send_percent(do_rpc_t *ctrl, int handle, int perc);
void      do_rpc_send_number(do_rpc_t *ctrl, int handle, int quant);
void      do_rpc_send_msg(do_rpc_t *ctrl, int handle, const char* cformat, ...);
void      do_sleep_ms(int milliseconds);

do_proc_state_t *do_proc_state_new();
do_state_t       do_proc_state_get_state(do_proc_state_t *do_proc_state);
void             do_proc_state_set_state(do_proc_state_t *do_proc_state, do_state_t state);
void             do_proc_state_set_callback_state(do_proc_state_t *do_proc_state, do_proc_state_callback_t *change_state, void* arg);
void             do_proc_state_set_callback_percent(do_proc_state_t *do_proc_state, do_proc_state_callback_t *change_percent, void *arg);
int              do_proc_state_get_persent(do_proc_state_t *do_proc_state);
void             do_proc_state_set_persent(do_proc_state_t *do_proc_state, int percent);
void             do_proc_state_free(do_proc_state_t *do_proc_state);
const char      *do_proc_state_get_name(do_state_t state);

#ifdef _LINUX
int   do_daemon_start(int argc, char** argv, do_main_handler_t *wrapper, char *username, char *pid_file);
#elif defined(_WIN32)
int   do_service_start(int install, do_main_handler_t *wrapper, LPCTSTR name, LPCTSTR display, LPCTSTR description, LPCTSTR depends);
void  do_service_set_proc_state(do_proc_state_t *proc);
#endif

int   do_save_to_file(const char* filename, void *buf, size_t len);
char *do_read_from_file(const char* filename);

do_xls_t   *do_xls_new(const char *filename, int use_utf8);
void        do_xls_free(do_xls_t *xls);
int         do_xls_worksheet_count(do_xls_t *xls);
int         do_xls_row_count(do_xls_t *xls, int worksheet);
const char *do_xls_cell_str(do_xls_t *xls, int worksheet, int row, int col);

char* do_strtowupper(const char* value);

int do_isworktime();
char *do_get_config_filename(char *localname, char *filename);

#ifdef __cplusplus
}
#endif

#endif
