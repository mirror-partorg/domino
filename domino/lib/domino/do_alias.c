
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#include <domino.h>
#include <dolib.h>
#include <iconv.h>
#include <locale.h>
#include <btrieve.h>
#include "btr_struct.h"
#include "../../config/config.h"
#include "../../misc/iconv_.h"
#include "../../misc/define.h"
#include "../../misc/utf8.h"

#define USER_DB_NAME "USER"
#define USER_G_GROUP "G"
#define USER_G_USER   "U"


static const char *key[DO_DB_END] = {
    "DOCUMENT" ,
    "D_ORDER",
    "DLINK",
    "D_VIEW",
    "DOC_DATA",
    "DPROW",
    "DTYPE",
    "ENUM",
    "TOWAR",
    "T_LINK",
    "T_VIEW",
    "TOW_DATA",
    "BARCODES",
    "ACCOUNT",
    "SUBACCOUNT",
    "KLASS",
    "GRUPPA",
    "PODGRU",
    "PGRUPPA",
    "OTDEL",
    "PARTNER",
#ifdef USE_DB_PARNER_VIEW
    "P_VIEW",
#endif
    "PAR_DATA",
    "SKLAD",
    "STOCK",
    "PROWOD",
    "PROWOD2",
    "SALDO",
    "PROTOCOL",
    "USER",
#ifndef DOMINO78
#ifndef USE_OLD_CHECK
    "USHIFTS",
#else
    "SHIFTS",
#endif
    "CHECKSSUM",
    "CHECKS",
    "REALIZATION",
    "DISCOUNT",
#endif
    "STRUCT_FILE"};
#ifdef DEBUG_BTI
static const char* bti_oper[79] = {
"B_OPEN",
"B_CLOSE",
"B_INSERT",
"B_UPDATE",
"B_DELETE",
"B_GET_EQUAL",
"B_GET_NEXT",
"B_GET_PREVIOUS",
"B_GET_GT",
"B_GET_GE",
"B_GET_LT",
"B_GET_LE",
"B_GET_FIRST",
"B_GET_LAST",
"B_CREATE",
"B_STAT",
"B_EXTEND",
"B_SET_DIR",
"B_GET_DIR",
"B_BEGIN_TRAN",
"B_END_TRAN",
"B_ABORT_TRAN",
"B_GET_POSITION",
"B_GET_DIRECT",
"B_STEP_NEXT",
"B_STOP",
"B_VERSION",
"B_UNLOCK",
"B_RESET",
"B_SET_OWNER",
"B_CLEAR_OWNER",
"B_BUILD_INDEX",
"B_DROP_INDEX",
"B_STEP_FIRST",
"B_STEP_LAST",
"B_STEP_PREVIOUS",
"B_GET_NEXT_EXTENDED",
"B_GET_PREV_EXTENDED",
"B_STEP_NEXT_EXT",
"B_STEP_PREVIOUS_EXT",
"B_EXT_INSERT",
"B_MISC_DATA",
"B_CONTINUOUS",
"B_SEEK_PERCENT",
"B_GET_PERCENT",
"B_CHUNK_UPDATE",
"B_KEY_EQUAL",
"B_KEY_NEXT",
"B_KEY_PREV",
"B_KEY_GT",
"B_KEY_GE",
"B_KEY_LT",
"B_KEY_LE",
"B_KEY_FIRST",
"B_KEY_LAST",
"B_EXTENDED_STAT",
"B_LOGIN"};
#endif
#define DB_SQL "DB_SQL"
#define DB_HOST "DB_HOST"


typedef enum {
    CACHE_ACCOUNT,
    CACHE_SUBACCOUNT,

    CACHE_NO
} cache_t;
typedef struct  {
    do_sort_list_t *list[CACHE_NO];
} do_alias_cache_t;
struct _do_alias_t {
    int connect;
    int breakOnError;
    CLIENT_ID clientID;
    int  init_clientID;
    char *dbConfDir;
    char *aliasName;
    char *filename;
    char *host;
    char *dbname;
    BTI_BYTE dbBlock[DO_DB_END][BTI_BLOCK_SIZE];
    BTI_BYTE emptyBlock[BTI_BLOCK_SIZE];
    char *dbfilename[DO_DB_END];
    char *dbfilename_orig[DO_DB_END];
    int tran_stat;
    char *username;
    char *password;
    do_alias_cache_t *cache;
    iconv_t tdo_conv;
    iconv_t fdo_conv;
    int no_wait_look;
    int utf8;
    int dublicate;
};


DO_EXPORT const char *do_alias_username(do_alias_t *alias)
{
    return alias->username;
}
static void cache_free(do_alias_cache_t *cache);
static do_alias_cache_t *cache_new();
static int threadID = 0;


int do_alias_btr(do_alias_t *alias, BTI_WORD operation, file_db_t id, BTI_VOID_PTR dataBuffer, BTI_WORD_PTR dataLength, BTI_VOID_PTR keyBuffer, BTI_SINT keyNumber);
static void to_do_init(do_alias_t *alias, int utf);
static void from_do_init(do_alias_t *alias, int utf);
do_alias_t *do_alias_new(const char *dbConfDir, const char* aliasName, int breakOnError, const char *username, const char* password, int utf8);;
DO_EXPORT int do_alias_change_dbname(do_alias_t *alias, const char *dbname);

DO_EXPORT do_alias_t *do_alias_clone(do_alias_t *alias)
{
    do_alias_t *ret;
    ret = do_alias_new(alias->dbConfDir, alias->aliasName, alias->breakOnError, alias->username, alias->password, alias->utf8);
    if ( alias->host ) {
        if ( ret->host )
 	    do_free(ret->host);
        ret->host = do_strdup(alias->host);
    }
    if ( alias->dbname )
       do_alias_change_dbname(ret, alias->dbname);

    return ret;
}

static int initClientThread(CLIENT_ID  *clientID)
{
    VERSION_STRUCT  versionBuffer[3];

    memset(clientID->networkAndNode, 0, sizeof(clientID->networkAndNode));
    memcpy(clientID->applicationID, "MT", 2);
    clientID->threadID = threadID++;
    memset(versionBuffer, 0, sizeof(versionBuffer));

#ifdef BTRIVE_DEBUG
    BTI_WORD dataLen = sizeof(versionBuffer);
    BTI_BYTE posBlock[128];
    BTI_BYTE dataBuf[255];
    BTI_SINT status;

    do_log_debug(ZONE, "Get btrieve version");
    status = BTRVID(B_VERSION, posBlock, &versionBuffer, &dataLen, dataBuf, 0, (BTI_BUFFER_PTR)clientID);

    if (status != B_NO_ERROR)
    {
        do_log(LOG_ERR, "Btrieve not loaded. B_VERSION status = %d", status);
        return 0;
    }
    int i;
    do_log_debug(ZONE, "Btrieve Versions returned are: ");
    for ( i = 0; i < 3; i++ ) {
      if (versionBuffer[i].Version != 0)
      {
        do_log_debug(ZONE, "   %d.%d %c", versionBuffer[i].Version,
                versionBuffer[i].Revision,
                versionBuffer[i].MKDEId);
      }
    }
#endif
    return 1;
}


static int openTable(BTI_VOID_PTR  fileBlock, const char *host, const char *filename, CLIENT_ID *clientID)
{
    BTI_WORD dataLen = 0;
    BTI_SINT status;
    FILE_CREATE_BUF fileCreateBuf;
    BTI_BYTE dataBuf[256];
    char *fileName;
    if ( !host || host[0] == '\0' )
        fileName=do_strdup(filename);
    else {
        fileName=do_strdup_printf("//%s%s", host, filename);
    }
    memset(&fileCreateBuf, 0, sizeof(fileCreateBuf));
    dataLen = sizeof(fileCreateBuf);
    status = B_FILE_NOT_OPEN;//BTRVID(B_STAT, fileBlock, &fileCreateBuf, &dataLen, dataBuf, 0, (BTI_BUFFER_PTR)clientID);
    if (status != B_NO_ERROR) {
        do_log_debug(ZONE, "Open file %s", fileName);
        status = BTRVID(B_OPEN, fileBlock, &dataBuf, &dataLen, fileName, 0, (BTI_BUFFER_PTR)clientID);
        do_log_debug(ZONE, "Open file %s status %d", fileName, status);
        if (status != B_NO_ERROR) {
            do_log(LOG_ERR, "Btrieve B_OPEN %s status = %d", fileName, status);
            do_free (fileName);
            return FALSE;
        }
        do_log_debug(ZONE, "Stat file %s", fileName);
        status = BTRVID(B_STAT, fileBlock, &fileCreateBuf, &dataLen, dataBuf, 0, (BTI_BUFFER_PTR)clientID);
        if (status != B_NO_ERROR) {
            do_log(LOG_ERR, "Btrieve B_STAT %s status = %d", fileName, status);
            do_free (fileName);
            return FALSE;
        }
    }
    do_free (fileName);
    return status == B_NO_ERROR;
}

DO_EXPORT int do_alias_db_number_records(do_alias_t *alias, file_db_t id)
{
    BTI_WORD dataLen = 0;
    BTI_SINT status;
    FILE_INFO_BUF fileCreateBuf[10];
    BTI_BYTE dataBuf[256];
    memset(&fileCreateBuf, 0, sizeof(fileCreateBuf));
    dataLen = sizeof(fileCreateBuf);
    status = BTRVID(B_STAT, alias->dbBlock[id], &fileCreateBuf, &dataLen, dataBuf, 1, (BTI_BUFFER_PTR)(&alias->clientID));
    if (status == B_NO_ERROR)
        return fileCreateBuf[0].fileSpecs.numberRecords;
    else
        do_log(LOG_ERR, "Btrieve B_STATE status = %d file %s", status, alias->dbfilename[id]);
    return -1;
}
DO_EXPORT int do_product_view_number_records(do_alias_t *alias)
{
    return do_alias_db_number_records( alias, DO_DB_PRODUCT_VIEW);
}
DO_EXPORT int do_alias_conected(do_alias_t *alias)
{
    return alias->connect;
}
DO_EXPORT int do_barcode_number_records(do_alias_t *alias)
{
    return do_alias_db_number_records( alias, DO_DB_BARCODE);
}
DO_EXPORT int do_partner_number_records(do_alias_t *alias)
{
    return do_alias_db_number_records( alias, DO_DB_PARTNER);
}
DO_EXPORT int do_region_number_records(do_alias_t *alias)
{
    return do_alias_db_number_records( alias, DO_DB_REGION);
}
DO_EXPORT int do_class_number_records(do_alias_t *alias)
{
    return do_alias_db_number_records( alias, DO_DB_CLASS);
}
DO_EXPORT int do_group_number_records(do_alias_t *alias)
{
    return do_alias_db_number_records( alias, DO_DB_GROUP);
}
DO_EXPORT int do_subgroup_number_records(do_alias_t *alias)
{
    return do_alias_db_number_records( alias, DO_DB_SUBGROUP);
}
static int freeConnect(CLIENT_ID *clientID)
{
    BTI_WORD dataLen = 0;
    BTI_SINT status;
    BTI_BYTE dataBuf[256],dataBuf1[256],dataBuf2[256];
    status = BTRVID(B_RESET, &dataBuf1, &dataBuf2, &dataLen, &dataBuf, 0, (BTI_BUFFER_PTR)clientID);
    status = BTRVID(B_STOP, &dataBuf1, &dataBuf2, &dataLen, &dataBuf, 0, (BTI_BUFFER_PTR)clientID);
    if (status != B_NO_ERROR) {
            return 0;
    }
    return 1;
}

static int closeTable(BTI_VOID_PTR  fileBlock, CLIENT_ID *clientID)
{
    BTI_WORD dataLen = 0;
    BTI_SINT status;
    FILE_CREATE_BUF fileCreateBuf[10];
    BTI_BYTE dataBuf[255];
    BTI_BYTE dataBuf1[255];
    memset(&fileCreateBuf, 0, sizeof(fileCreateBuf));
    dataLen = sizeof(fileCreateBuf);
    status = BTRVID(B_STAT, fileBlock, &fileCreateBuf, &dataLen, dataBuf, 0, (BTI_BUFFER_PTR)clientID);
    if (status == B_NO_ERROR) {
        do_log_debug(ZONE, "close file");
        status = BTRVID(B_CLOSE, fileBlock, dataBuf, &dataLen, dataBuf1, 0, (BTI_BUFFER_PTR)clientID);
        if (status != B_NO_ERROR) {
            do_log(LOG_ERR, "Btrieve B_CLOSE status = %d", status);
            return 0;
        }
    }
    return 1;
}


static int read_alias_config(do_alias_t *alias)
{
    FILE *fp;
    char *filename = (char*)do_malloc(strlen(alias->dbConfDir) + strlen(alias->aliasName) + 2);
#ifdef _WIN32
    sprintf(filename, "%s\\%s", alias->dbConfDir, alias->aliasName);
#elif defined(_LINUX)
    sprintf(filename, "%s/%s", alias->dbConfDir, alias->aliasName);
#endif

    if ((fp = fopen(filename, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s", filename, strerror(errno));
        do_free(filename);
        return 0;
    }
    alias->filename = do_strdup(filename);

    size_t len = 1024;
    char* line = (char *)do_malloc(len + 1);
    file_db_t file_id;
    int i;
    int retval;
    int error;
    retval = 1;
    error = 0;

    for ( i = 0; i < DO_DB_END; i++ )
        alias->dbfilename[i] = do_strdup("");

    while (!feof(fp))  {
        char* cp, *name, *value, *tail;
        if (fgets(line, len + 1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = do_realloc(line, len * 2 + 1);
            if (!fgets(line + len, len + 1, fp)) {
                retval = feof(fp);
                error = 1;
                line[0] = '\0';
                break;
            }
            len *= 2;
        }
        if (error)
            break;
        cp = line;
        while (cp[0] == ' ') ++cp; // delspace
        /* A leading '#' or ';' signals a comment line.  */
        if (cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n' || cp[0] == '\r')
            continue;
        name = cp;
        while (cp[0] != '\0' && cp[0] != '\n' && cp[0] != ' ') ++cp;
        if (name == cp)
            continue;

        file_id = DO_DB_END;
        if ( strlen(DB_HOST) != (cp - name) || strncmp(DB_HOST, name, cp - name) ) {
            for (i = 0; i < DO_DB_END; i++)
                if (strlen(key[i]) == (cp - name) && !strncmp(key[i], name, cp - name)) {
                    file_id = i;
                    break;
                }
            if (file_id == DO_DB_END)
                continue;
        }
        while (cp[0] == ' ') ++cp; // delspace
        value = cp;
        while (cp[0] != '\0' && cp[0] != '\n') ++cp;
        tail = cp - 1;
        while (tail >= value && (tail[0] == ' ' || tail[0] == '\r')) tail --;
        if (value > tail)
            continue;
        if ( file_id == DO_DB_END ) {
            if ( alias->host )
                do_free(alias->host);
            alias->host = do_strndup(value, tail - value + 1);
        }
	else {
            if (alias->dbfilename[file_id])
               do_free(alias->dbfilename[file_id]);
            alias->dbfilename[file_id] = (char*) do_malloc( tail - value + 2 );
            strncpy(alias->dbfilename[file_id], value, tail - value + 1);
            alias->dbfilename[file_id][tail - value + 1] = '\0';
       }
    }
    do_free(filename);
    do_free(line);
    fclose(fp);
    return retval;
}

int write_alias_config(do_alias_t *alias)
{
    FILE *fp, *fo;
    int found = FALSE;
    if ((fp = fopen(alias->filename, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s", alias->filename, strerror(errno));
        return 0;
    }
    char *outfile;
    outfile = do_strdup_printf("%s.new", alias->filename);
    if ((fo = fopen(outfile, "w+")) == NULL)  {
        do_log(LOG_ERR, "Error write configuration file (%s): %s\n", outfile, strerror(errno));
        do_free(outfile);
        return 0;
    }
    size_t len = 1024;
    char* line = (char *)do_malloc(len + 1);
    int retval;
    int error;
    retval = 1;
    error = 0;
    while (!feof(fp))  {
        char* cp, *name;
        if (fgets(line, len + 1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = do_realloc(line, len * 2 + 1);
            if (!fgets(line + len, len + 1, fp)) {
                retval = feof(fp);
                error = 1;
                line[0] = '\0';
                break;
            }
            len *= 2;
        }
        if (error)
            break;
        cp = line;
        while (cp[0] == ' ') ++cp; // delspace
        /* A leading '#' or ';' signals a comment line.  */
        if (cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n' || cp[0] == '\r') {
            fprintf(fo, "%s", line);
            continue;
        }
        name = cp;
        while (cp[0] != '\0' && cp[0] != '\n' && cp[0] != ' ') ++cp;
        if (name == cp) {
            fprintf(fo, "%s", line);
            continue;
        }

        if ( strlen(DB_HOST) == (cp - name) && !strncmp(DB_HOST, name, cp - name) ) {
            fprintf(fo, "%s %s\n", DB_HOST, alias->host);
            found = TRUE;
        }
        else
            fprintf(fo, "%s", line);
    }
    if ( !found )
        fprintf(fo, "%s %s", DB_HOST, alias->host);

    do_free(line);
    fclose(fp);
    fclose(fo);
    char *tmpfile;
    tmpfile = do_strdup_printf("%s~", alias->filename);
    remove(tmpfile);
    if (rename (alias->filename, tmpfile) == -1 ) {
        do_log(LOG_ERR, "rename file %s to %s %s\n", alias->filename, strerror(errno));
        retval = FALSE;
    }
    if (retval && (rename (outfile, alias->filename) == -1)) {
        do_log(LOG_ERR, "rename file %s to %s %s\n", outfile, alias->filename, strerror(errno));
        retval = FALSE;
    }
    do_free(tmpfile);
    do_free(outfile);
    return retval;
}

do_alias_t *do_alias_new(const char *dbConfDir, const char* aliasName, int breakOnError, const char *username, const char* password, int utf8)
{
    do_alias_t *alias;
    if ( (alias = (do_alias_t *)do_malloc0(sizeof(do_alias_t))) == NULL)
        return alias;
    if (dbConfDir)
        alias->dbConfDir = do_strdup(dbConfDir);
    else
        alias->dbConfDir = DOMINO_CONFIG(DOMINO_ALIAS_RELATIVE_PATH);
    if (aliasName)
        alias->aliasName = do_strdup(aliasName);
    else
        alias->aliasName = do_strdup(DOMINO_LOCAL_ALIAS);
    alias->no_wait_look = 0;
    memset(&alias->dbBlock, 0, sizeof(alias->dbBlock));
    memset(&alias->emptyBlock, 0, sizeof(alias->emptyBlock));
    memset(&alias->dbfilename, 0, sizeof(alias->dbfilename));
    alias->cache = NULL;
    alias->filename = NULL;
    alias->host = NULL;
    alias->connect = 0;
    alias->tran_stat = 0;
    alias->breakOnError = breakOnError;
    alias->username = do_strdup(username);
    alias->password = do_strdup(password);
    alias->fdo_conv = (iconv_t) -1;
    alias->tdo_conv = (iconv_t) -1;
    if (!read_alias_config(alias))
    {
        do_alias_free(alias);
        return NULL;
    }
    to_do_init(alias, utf8);
    from_do_init(alias, utf8);
    alias->utf8 = utf8;

    return alias;
}
do_alias_t *do_alias_new_light(int breakOnError, const char *username, const char* password, int utf8)
{
    do_alias_t *alias;
    if ( (alias = (do_alias_t *)do_malloc0(sizeof(do_alias_t))) == NULL)
        return alias;
    memset(&alias->dbBlock, 0, sizeof(alias->dbBlock));
    memset(&alias->emptyBlock, 0, sizeof(alias->emptyBlock));
    memset(&alias->dbfilename, 0, sizeof(alias->dbfilename));
    alias->cache = NULL;
    alias->filename = NULL;
    alias->host = NULL;
    alias->connect = 0;
    alias->tran_stat = 0;
    alias->breakOnError = breakOnError;
    alias->username = do_strdup(username);
    alias->password = do_strdup(password);
    alias->fdo_conv = (iconv_t) -1;
    alias->tdo_conv = (iconv_t) -1;
    to_do_init(alias, utf8);
    from_do_init(alias, utf8);
    alias->utf8 = utf8;

    return alias;
}
DO_EXPORT int do_alias_utf8(do_alias_t *alias)
{
   return alias->utf8;
}

DO_EXPORT void do_alias_free(do_alias_t *alias)
{
    do_alias_close(alias);
    if (alias->dbConfDir)
        do_free(alias->dbConfDir);
    if (alias->aliasName)
        do_free(alias->aliasName);
    if (alias->filename)
        do_free(alias->filename);
    if ( alias->host )
        do_free(alias->host);
    int i;
    for (i = 0; i < DO_DB_END; i++)
        if (alias->dbfilename[i])
            do_free(alias->dbfilename[i]);
    for (i = 0; i < DO_DB_END; i++)
        if (alias->dbfilename_orig[i])
            do_free(alias->dbfilename_orig[i]);

    do_free(alias->username);
    do_free(alias->password);
    if (alias->fdo_conv != (iconv_t) -1)
        iconv_close(alias->fdo_conv);
    if (alias->tdo_conv != (iconv_t) -1)
        iconv_close(alias->tdo_conv);
    if (alias->cache)
        cache_free(alias->cache);

    do_free(alias);
}
DO_EXPORT int do_alias_change_dbname(do_alias_t *alias, const char *dbname)
{
    int i, ret;
    ret = FALSE;
    if ( alias->dbname )
        do_free(alias->dbname);
    alias->dbname = do_strdup(dbname);
    for (i = 0; i < DO_DB_END; i++) {
        if ( alias->dbfilename[i] ) {
            char *head, *tail, *value;
            if ( !alias->dbfilename_orig[i] )
                alias->dbfilename_orig[i] = do_strdup(alias->dbfilename[i]);
            value = do_strdup(alias->dbfilename_orig[i]);
            head = value;
            tail = strstr(head, alias->aliasName);
            if ( tail ) {
                 ret = TRUE;
                 do_free(alias->dbfilename[i]);
                 *tail = '\0';
                 tail += strlen(alias->aliasName);
                 alias->dbfilename[i] = do_strdup_printf("%s%s%s", head, dbname, tail);
            }
            do_free(value);
        }
    }
    return ret;
}

DO_EXPORT int do_alias_close(do_alias_t *alias)
{

    if ( alias->init_clientID )   {
        int i;
        for (i = 0; i < DO_DB_END; i++)
            if ( memcmp(&alias->dbBlock[i], &alias->emptyBlock, sizeof(alias->emptyBlock)) )
                closeTable(alias->dbBlock[i], &alias->clientID);
        freeConnect(&alias->clientID);
        alias->init_clientID = FALSE;
        alias->connect = 0;
    }
    return 1;
}
#define FILE_DB_NAME "file.ddf"
DO_EXPORT void do_alias_set_struct_location(do_alias_t *alias, const char *path)
{
    alias->dbfilename[DO_DB_STRUCT_FILE] = do_malloc(strlen(path)+2+strlen(FILE_DB_NAME));
    sprintf(alias->dbfilename[DO_DB_STRUCT_FILE], "%s/%s", path, FILE_DB_NAME);
}

DO_EXPORT int do_alias_open(do_alias_t *alias, int openAll, ...)
{
    alias->connect = 0;
    if ( !initClientThread(&alias->clientID) ) {
        alias->init_clientID = FALSE;
        return 0;
    }
    alias->init_clientID = TRUE;
    int i;
    if (openAll) {
        for (i = 0; i < DO_DB_END; i++)
        {
            //!!do_log(LOG_INFO, "open dbname(%s) %s", key[i], alias->dbfilename[i]);
            if ( alias->dbfilename[i][0] == '\0' ) {
                //!!do_log(LOG_WARNING, "undefine dbname for %s skip", key[i]);
                continue;
            }
            if ( !openTable(alias->dbBlock[i], alias->host, alias->dbfilename[i], &alias->clientID) )
                return FALSE;
        }
    }
    else  {
            int id_db;
            va_list arg;
            va_start(arg, openAll);
            while ((id_db = va_arg(arg, int)) < DO_DB_END)
            {
                if (!alias->dbfilename[id_db]) {
                    do_log(LOG_ERR, "undefine dbname for %s", key[id_db]);
                    return 0;
                }
                if (!openTable(alias->dbBlock[id_db], alias->host, alias->dbfilename[id_db], &alias->clientID))
                {
                    return 0;
                }
            }
            va_end(arg);
    };
    alias->connect = 1;
    do_log_debug(ZONE, "alias open");
    return TRUE;
}
DO_EXPORT int do_alias_open_manual(do_alias_t *alias,  ...)
{
    alias->connect = 0;
    if ( !initClientThread(&alias->clientID) ) {
        alias->init_clientID = FALSE;
        return 0;
    }
    alias->init_clientID = TRUE;
    int id_db;
    char *filename;
    va_list arg;
    va_start(arg, alias);
    while ( (id_db = va_arg(arg, int)) < DO_DB_END ) {
        filename = va_arg(arg, char*);
	if ( alias->dbfilename[id_db] )
	   do_free(alias->dbfilename[id_db]);
        alias->dbfilename[id_db] = do_strdup(filename);
        if ( !openTable(alias->dbBlock[id_db], alias->host, alias->dbfilename[id_db], &alias->clientID) )
            return 0;
    }
    va_end(arg);
    alias->connect = 1;
    do_log_debug(ZONE, "alias open");
    return TRUE;
}

DO_EXPORT int  do_alias_tran_begin(do_alias_t *alias)
{
    if (alias->tran_stat)
        alias->tran_stat++;
    else {
        int count;
        BTI_SINT status;
        for (count = 0; count < 60; count++) {
            status = BTRVID(B_BEGIN_TRAN + 1000, NULL, NULL, NULL, NULL, 0, (BTI_BUFFER_PTR)&alias->clientID);
            if (status == B_NO_ERROR) {
                alias->tran_stat++;
                return DO_OK;
            }
            if (status != B_TRANSACTION_IS_ACTIVE)
                return DO_ERROR;
            if (alias->no_wait_look)
                return DO_INUSE;
            DO_SEC_SLEEP(1);
        }
        return DO_ERROR;
    }
    return DO_OK;
}
DO_EXPORT int  do_alias_tran_end(do_alias_t *alias)
{
    if (!alias->tran_stat)
        return DO_OK;
    if (alias->tran_stat > 1) {
        alias->tran_stat--;
        return DO_OK;
    }
    alias->tran_stat = 0;
    if (BTRVID(B_END_TRAN, NULL, NULL, NULL, NULL, 0, (BTI_BUFFER_PTR)&alias->clientID) != B_NO_ERROR)
        return DO_ERROR;
    return DO_OK;
}
DO_EXPORT int  do_alias_tran_abort(do_alias_t *alias)
{
    if (!alias->tran_stat)
        return DO_OK;
    if (alias->tran_stat > 1) {
        alias->tran_stat--;
        return DO_OK;
    }
    alias->tran_stat = 0;
    if (BTRVID(B_ABORT_TRAN, NULL, NULL, NULL, NULL, 0, (BTI_BUFFER_PTR)&alias->clientID) != B_NO_ERROR)
        return DO_ERROR;
    return DO_OK;
}
DO_EXPORT void do_alias_set_wait_flag(do_alias_t *alias, int wait)
{
   alias->no_wait_look = !wait;
}
#ifdef DEBUG_BTI
static int debug_btr = 0;
DO_EXPORT void domino_set_debug_btr(int debug)
{
	debug_btr = debug;
}
#else
#define domino_set_debug_btr(a)
#endif
DO_EXPORT int do_alias_btr(do_alias_t *alias, BTI_WORD operation, file_db_t id, BTI_VOID_PTR dataBuffer, BTI_WORD_PTR dataLength, BTI_VOID_PTR keyBuffer, BTI_SINT keyNumber)
{
    BTI_SINT status;
    int count;
#ifdef DEBUG_BTI
    if ( debug_btr )
        do_log(LOG_INFO, "btr %s %s", key[id], bti_oper[operation]);
#endif
    status =  BTRVID(operation, alias->dbBlock[id], dataBuffer, dataLength, keyBuffer, keyNumber, (BTI_BUFFER_PTR)&alias->clientID);
#ifdef DEBUG_BTI
    if ( debug_btr ) {
    switch (status) {
        case B_NO_ERROR:
            do_log(LOG_INFO, "btr end %s %s ok", key[id], bti_oper[operation]);
            break;
        case B_REJECT_COUNT_REACHED:
            do_log(LOG_INFO, "btr end %s %s count reached", key[id], bti_oper[operation]);
            break;
        case B_OPTIMIZE_LIMIT_REACHED:
        case B_KEY_VALUE_NOT_FOUND:
        case B_END_OF_FILE:
            do_log(LOG_INFO, "btr end %s %s key not found %d", key[id], bti_oper[operation], status);
            break;
        case B_CONFLICT:
        case B_RECORD_INUSE:
        case B_FILE_INUSE:
            do_log(LOG_INFO, "btr end %s %s use %d", key[id], bti_oper[operation], status);
            break;
        case B_DUPLICATE_KEY_VALUE:
            do_log(LOG_INFO, "btr end %s %s dublicate key", key[id], bti_oper[operation]);
            break;
        default:
            do_log(LOG_INFO, "btr end %s %s status %d", key[id], bti_oper[operation], status);
    }
    }
#endif
    switch (status) {
        case B_NO_ERROR:
            return DO_OK;
        case B_REJECT_COUNT_REACHED:
            return DO_REJECT;
        case B_OPTIMIZE_LIMIT_REACHED:
        case B_KEY_VALUE_NOT_FOUND:
        case B_END_OF_FILE:
            return DO_KEY_NOT_FOUND;
            break;
        case B_CONFLICT:
        case B_RECORD_INUSE:
        case B_FILE_INUSE:
            if (alias->no_wait_look)
                return DO_INUSE;
            for (count = 0; count < 20 &&  (status == B_RECORD_INUSE ||
	                                    status == B_CONFLICT ||
	                                    status == B_FILE_INUSE); count++)
            {
                DO_SEC_SLEEP(1);
                status =  BTRVID(operation, alias->dbBlock[id], dataBuffer, dataLength, keyBuffer, keyNumber, (BTI_BUFFER_PTR)&alias->clientID);
            }
            if (status == B_NO_ERROR)
                return DO_OK;
        case B_DUPLICATE_KEY_VALUE:
            if (alias->dublicate)
                return DO_DUPLICATE_KEY_VALUE;
        default:
            do_log(LOG_ERR, "Btrieve operation %d status = %d file %s", operation, status, alias->dbfilename[id]);
            if (alias->breakOnError)
                do_alias_close(alias);
            return DO_ERROR;
    }
}
DO_EXPORT void do_alias_set_dublicate_allow(do_alias_t *alias, int allow)
{
   alias->dublicate = allow;
}
DO_EXPORT int do_alias_get_dublicate_allow(do_alias_t *alias)
{
   return alias->dublicate;
}

DO_EXPORT char *do_product_create_parcel(do_alias_t *alias, const char* base_code, const char *pref)
{
    product_rec_t product;
    product_key0_t product_key0, key;
    product_key0.base_parcel = 0;
    do_text_set(alias, product_key0.code, base_code);
    int stat = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
    int base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);
    int parcel_len = 3;//fix me!!!
    int full_pref_len;
    if (stat == DO_KEY_NOT_FOUND) {
        do_log(LOG_ERR, "base product \"%s\" not found", base_code);
        return NULL;
    }
    if (stat != DO_OK)
        return NULL;
    // get last parcel
    int indx;
    char *value;
    product_key0.base_parcel = 1;
    do_text_set_empty(product_key0.code);
    strncpy(product_key0.code, base_code, base_len);
    strncpy(product_key0.code + base_len, pref, strlen(pref));
    do_cpy(key.code, product_key0.code);
    full_pref_len = base_len + strlen(pref);
    memset(product_key0.code + full_pref_len, '9', sizeof(product_key0.code) - full_pref_len);
    if (do_product_key0(alias, &product_key0, DO_GET_LE) != DO_OK)
        return NULL;
    if ( strncmp(key.code, product_key0.code, full_pref_len) ) {
        indx = 0;
    }
    else {
        value = do_text(alias, product_key0.code);
        indx = atoi(value + full_pref_len);
        do_free(value);
    }
    indx++;
    value = do_malloc(100);
    char *format = (char*)do_malloc(21);
    sprintf(value, "%s.%s%0*d", base_code, pref, parcel_len, indx);
    do_free(format);
    do_text_set(alias, product.data.code, value);
    do_free(value);
    product.data.base_parcel = 1;
    if (do_product_insert(alias, &product) != DO_OK)
        return NULL;
    return do_text(alias, product.data.code);
}
DO_EXPORT char *do_product_get_name(do_alias_t *alias, const char* code)
{
    product_key4_t product_key4;
    product_rec_t product;
    do_text_set(alias, product_key4.code, code);
    if (do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) == DO_OK)
        return do_text(alias, product.data.name);
    else
        return do_strdup(code);
}

DO_EXPORT int do_document_get_last_line(do_alias_t *alias, document_key0_t *document_key0)
{
    document_order_key0_t document_order_key0;
    memcpy(&document_order_key0, document_key0, sizeof(document_key0_t));
    document_order_key0.line = 2147483646;
    if ( (do_document_order_key0(alias, &document_order_key0, DO_GET_LT) != DO_OK) ||
         memcmp(&document_order_key0, document_key0, sizeof(document_key0_t)) )
        return 0;
    return document_order_key0.line;
}


// make by perl

DO_EXPORT int do_struct_file_get0(do_alias_t *alias, struct_file_rec_t *rec, struct_file_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(struct_file_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_struct_file_get1(do_alias_t *alias, struct_file_rec_t *rec, struct_file_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(struct_file_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_STRUCT_FILE, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_struct_file_key0(do_alias_t *alias, struct_file_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_struct_file_key1(do_alias_t *alias, struct_file_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_STRUCT_FILE, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_struct_file_insert(do_alias_t *alias, struct_file_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_STRUCT_FILE, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_struct_file_update(do_alias_t *alias, struct_file_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_STRUCT_FILE, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_struct_file_delete(do_alias_t *alias, struct_file_rec_t *rec)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_STRUCT_FILE, NULL, NULL, NULL, -1);
}
DO_EXPORT int do_document_get0(do_alias_t *alias, document_rec_t *rec, document_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_key0(do_alias_t *alias, document_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_get1(do_alias_t *alias, document_rec_t *rec, document_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_key1(do_alias_t *alias, document_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_get2(do_alias_t *alias, document_rec_t *rec, document_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_key2(do_alias_t *alias, document_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_get3(do_alias_t *alias, document_rec_t *rec, document_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_key3(do_alias_t *alias, document_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 3);
    }
    return DO_ERROR;
}
#ifdef DOMINO78
DO_EXPORT int do_document_get4(do_alias_t *alias, document_rec_t *rec, document_key4_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT, &rec->data, &rec->size, key, 4);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_key4(do_alias_t *alias, document_key4_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT, NULL, 0, key, 4);
    }
    return DO_ERROR;
}
#endif
DO_EXPORT int do_document_order_get0(do_alias_t *alias, document_order_rec_t *rec, document_order_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_order_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_key0(do_alias_t *alias, document_order_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_get1(do_alias_t *alias, document_order_rec_t *rec, document_order_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_order_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_key1(do_alias_t *alias, document_order_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_get2(do_alias_t *alias, document_order_rec_t *rec, document_order_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_order_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_key2(do_alias_t *alias, document_order_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_get3(do_alias_t *alias, document_order_rec_t *rec, document_order_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_order_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_key3(do_alias_t *alias, document_order_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_get4(do_alias_t *alias, document_order_rec_t *rec, document_order_key4_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_order_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, key, 4);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_order_key4(do_alias_t *alias, document_order_key4_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_ORDER, NULL, 0, key, 4);
    }
    return DO_ERROR;
}

DO_EXPORT int do_document_link_get0(do_alias_t *alias, document_link_rec_t *rec, document_link_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_link_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_link_key0(do_alias_t *alias, document_link_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_link_get1(do_alias_t *alias, document_link_rec_t *rec, document_link_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_link_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_link_key1(do_alias_t *alias, document_link_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_LINK, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_enum_get0(do_alias_t *alias, enum_rec_t *rec, enum_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(enum_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_ENUM, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_enum_key0(do_alias_t *alias, enum_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_ENUM, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_document_view_get0(do_alias_t *alias, document_view_rec_t *rec, document_view_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_view_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_view_key0(do_alias_t *alias, document_view_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_view_get1(do_alias_t *alias, document_view_rec_t *rec, document_view_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_view_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_view_key1(do_alias_t *alias, document_view_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_VIEW, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_document_data_get0(do_alias_t *alias, document_data_rec_t *rec, document_data_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_data_key0(do_alias_t *alias, document_data_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
#ifdef DOMINO78
DO_EXPORT int do_document_data_get1(do_alias_t *alias, document_data_rec_t *rec, document_data_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_data_key1(do_alias_t *alias, document_data_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_data_get2(do_alias_t *alias, document_data_rec_t *rec, document_data_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_data_key2(do_alias_t *alias, document_data_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_DATA, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
#endif
DO_EXPORT int do_product_get0(do_alias_t *alias, product_rec_t *rec, product_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_key0(do_alias_t *alias, product_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_get1(do_alias_t *alias, product_rec_t *rec, product_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_key1(do_alias_t *alias, product_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_get2(do_alias_t *alias, product_rec_t *rec, product_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_key2(do_alias_t *alias, product_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_get3(do_alias_t *alias, product_rec_t *rec, product_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_key3(do_alias_t *alias, product_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_get4(do_alias_t *alias, product_rec_t *rec, product_key4_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT, &rec->data, &rec->size, key, 4);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_key4(do_alias_t *alias, product_key4_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT, NULL, 0, key, 4);
    }
    return DO_ERROR;
}

DO_EXPORT int do_product_link_get0(do_alias_t *alias, product_link_rec_t *rec, product_link_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_link_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_link_key0(do_alias_t *alias, product_link_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_link_get1(do_alias_t *alias, product_link_rec_t *rec, product_link_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_link_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_link_key1(do_alias_t *alias, product_link_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT_LINK, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_product_view_get0(do_alias_t *alias, product_view_rec_t *rec, product_view_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_view_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_view_key0(do_alias_t *alias, product_view_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_view_get1(do_alias_t *alias, product_view_rec_t *rec, product_view_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_view_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_view_key1(do_alias_t *alias, product_view_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT_VIEW, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_product_data_get0(do_alias_t *alias, product_data_rec_t *rec, product_data_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_data_key0(do_alias_t *alias, product_data_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_data_get1(do_alias_t *alias, product_data_rec_t *rec, product_data_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_data_key1(do_alias_t *alias, product_data_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_data_get2(do_alias_t *alias, product_data_rec_t *rec, product_data_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(product_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_product_data_key2(do_alias_t *alias, product_data_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PRODUCT_DATA, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_barcode_get0(do_alias_t *alias, barcode_rec_t *rec, barcode_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(barcode_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_BARCODE, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_barcode_key0(do_alias_t *alias, barcode_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_barcode_get1(do_alias_t *alias, barcode_rec_t *rec, barcode_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(barcode_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_BARCODE, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_barcode_key1(do_alias_t *alias, barcode_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_BARCODE, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_account_get0(do_alias_t *alias, account_rec_t *rec, account_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(account_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_ACCOUNT, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_account_key0(do_alias_t *alias, account_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_ACCOUNT, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_subaccount_get0(do_alias_t *alias, subaccount_rec_t *rec, subaccount_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(subaccount_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SUBACCOUNT, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_subaccount_key0(do_alias_t *alias, subaccount_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SUBACCOUNT, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_class_get0(do_alias_t *alias, class_rec_t *rec, class_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(class_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_CLASS, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_class_key0(do_alias_t *alias, class_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_CLASS, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_group_get0(do_alias_t *alias, group_rec_t *rec, group_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(group_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_GROUP, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_group_key0(do_alias_t *alias, group_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_group_get1(do_alias_t *alias, group_rec_t *rec, group_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(group_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_GROUP, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_group_key1(do_alias_t *alias, group_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_GROUP, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_subgroup_get0(do_alias_t *alias, subgroup_rec_t *rec, subgroup_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(subgroup_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_subgroup_key0(do_alias_t *alias, subgroup_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_subgroup_get1(do_alias_t *alias, subgroup_rec_t *rec, subgroup_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(subgroup_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SUBGROUP, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_subgroup_key1(do_alias_t *alias, subgroup_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SUBGROUP, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_region_get0(do_alias_t *alias, region_rec_t *rec, region_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(region_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_REGION, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_REGION, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_REGION, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_REGION, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_REGION, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_REGION, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_REGION, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_REGION, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_REGION, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_region_key0(do_alias_t *alias, region_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_region_get1(do_alias_t *alias, region_rec_t *rec, region_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(region_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_REGION, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_REGION, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_REGION, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_REGION, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_REGION, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_REGION, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_REGION, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_REGION, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_REGION, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_region_key1(do_alias_t *alias, region_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_REGION, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_otdel_get0(do_alias_t *alias, otdel_rec_t *rec, otdel_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(otdel_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_OTDEL, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_otdel_key0(do_alias_t *alias, otdel_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_OTDEL, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_partner_get0(do_alias_t *alias, partner_rec_t *rec, partner_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_key0(do_alias_t *alias, partner_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_get1(do_alias_t *alias, partner_rec_t *rec, partner_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_key1(do_alias_t *alias, partner_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_get2(do_alias_t *alias, partner_rec_t *rec, partner_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_key2(do_alias_t *alias, partner_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_get3(do_alias_t *alias, partner_rec_t *rec, partner_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_key3(do_alias_t *alias, partner_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER, NULL, 0, key, 3);
    }
    return DO_ERROR;
}

#ifdef USE_DB_PARNER_VIEW
DO_EXPORT int do_partner_view_get0(do_alias_t *alias, partner_view_rec_t *rec, partner_view_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_view_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_view_key0(do_alias_t *alias, partner_view_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_view_get1(do_alias_t *alias, partner_view_rec_t *rec, partner_view_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_view_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_view_key1(do_alias_t *alias, partner_view_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER_VIEW, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
#endif
DO_EXPORT int do_partner_data_get0(do_alias_t *alias, partner_data_rec_t *rec, partner_data_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_data_key0(do_alias_t *alias, partner_data_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_data_get1(do_alias_t *alias, partner_data_rec_t *rec, partner_data_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_data_key1(do_alias_t *alias, partner_data_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
#ifdef DOMINO78
DO_EXPORT int do_partner_data_get2(do_alias_t *alias, partner_data_rec_t *rec, partner_data_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(partner_data_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PARTNER_DATA, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_partner_data_key2(do_alias_t *alias, partner_data_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PARTNER_DATA, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
#endif

DO_EXPORT int do_sklad_get0(do_alias_t *alias, sklad_rec_t *rec, sklad_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(sklad_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SKLAD, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_sklad_key0(do_alias_t *alias, sklad_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SKLAD, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_document_prow_get0(do_alias_t *alias, document_prow_rec_t *rec, document_prow_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_prow_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_prow_key0(do_alias_t *alias, document_prow_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_PROW, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_document_type_get0(do_alias_t *alias, document_type_rec_t *rec, document_type_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(document_type_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_document_type_key0(do_alias_t *alias, document_type_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DOCUMENT_TYPE, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_stock_get0(do_alias_t *alias, stock_rec_t *rec, stock_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(stock_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_STOCK, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_stock_key0(do_alias_t *alias, stock_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_stock_get1(do_alias_t *alias, stock_rec_t *rec, stock_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(stock_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_STOCK, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_stock_key1(do_alias_t *alias, stock_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
#ifdef DOMINO78
DO_EXPORT int do_stock_get2(do_alias_t *alias, stock_rec_t *rec, stock_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(stock_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_STOCK, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_stock_key2(do_alias_t *alias, stock_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_stock_get3(do_alias_t *alias, stock_rec_t *rec, stock_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(stock_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_STOCK, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_stock_key3(do_alias_t *alias, stock_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_STOCK, NULL, 0, key, 3);
    }
    return DO_ERROR;
}
#endif
DO_EXPORT int do_prowod_get0(do_alias_t *alias, prowod_rec_t *rec, prowod_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_key0(do_alias_t *alias, prowod_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_get1(do_alias_t *alias, prowod_rec_t *rec, prowod_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_key1(do_alias_t *alias, prowod_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_get2(do_alias_t *alias, prowod_rec_t *rec, prowod_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_key2(do_alias_t *alias, prowod_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_get3(do_alias_t *alias, prowod_rec_t *rec, prowod_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_key3(do_alias_t *alias, prowod_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_get4(do_alias_t *alias, prowod_rec_t *rec, prowod_key4_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD, &rec->data, &rec->size, key, 4);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod_key4(do_alias_t *alias, prowod_key4_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD, NULL, 0, key, 4);
    }
    return DO_ERROR;
}

DO_EXPORT int do_prowod2_get0(do_alias_t *alias, prowod2_rec_t *rec, prowod2_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod2_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD2, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod2_key0(do_alias_t *alias, prowod2_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod2_get1(do_alias_t *alias, prowod2_rec_t *rec, prowod2_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod2_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD2, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod2_key1(do_alias_t *alias, prowod2_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod2_get2(do_alias_t *alias, prowod2_rec_t *rec, prowod2_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod2_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD2, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod2_key2(do_alias_t *alias, prowod2_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod2_get3(do_alias_t *alias, prowod2_rec_t *rec, prowod2_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(prowod2_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROWOD2, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_prowod2_key3(do_alias_t *alias, prowod2_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROWOD2, NULL, 0, key, 3);
    }
    return DO_ERROR;
}

DO_EXPORT int do_saldo_get0(do_alias_t *alias, saldo_rec_t *rec, saldo_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(saldo_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SALDO, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_saldo_key0(do_alias_t *alias, saldo_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SALDO, NULL, 0, key, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_protocol_step(do_alias_t *alias, protocol_rec_t *rec,  do_alias_step_t operation)
{
    rec->size = sizeof(protocol_struct_t);
    switch (operation) {
        case DO_STEP_NEXT:
            return do_alias_btr(alias, B_STEP_NEXT, DO_DB_PROTOCOL, &rec->data, &rec->size, NULL, 0);
        case DO_STEP_PREVIOUS:
            return do_alias_btr(alias, B_STEP_PREVIOUS, DO_DB_PROTOCOL, &rec->data, &rec->size, NULL, 0);
        case DO_STEP_FIRST:
            return do_alias_btr(alias, B_STEP_FIRST, DO_DB_PROTOCOL, &rec->data, &rec->size, NULL, 0);
        case DO_STEP_LAST:
            return do_alias_btr(alias, B_STEP_LAST, DO_DB_PROTOCOL, &rec->data, &rec->size, NULL, 0);
    }
    return DO_ERROR;
}

DO_EXPORT int do_protocol_get0(do_alias_t *alias, protocol_rec_t *rec, protocol_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(protocol_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_protocol_key0(do_alias_t *alias, protocol_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_protocol_get1(do_alias_t *alias, protocol_rec_t *rec, protocol_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(protocol_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_protocol_key1(do_alias_t *alias, protocol_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_protocol_get2(do_alias_t *alias, protocol_rec_t *rec, protocol_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(protocol_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_PROTOCOL, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_protocol_key2(do_alias_t *alias, protocol_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_PROTOCOL, NULL, 0, key, 2);
    }
    return DO_ERROR;
}

DO_EXPORT int do_user_get0(do_alias_t *alias, user_rec_t *rec, user_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(user_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_USER, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_USER, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_USER, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_USER, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_USER, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_USER, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_USER, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_USER, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_USER, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_user_key0(do_alias_t *alias, user_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_USER, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
#ifndef DOMINO78
DO_EXPORT int do_shift_get0(do_alias_t *alias, shift_rec_t *rec, shift_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(shift_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SHIFT, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_shift_key0(do_alias_t *alias, shift_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_shift_get1(do_alias_t *alias, shift_rec_t *rec, shift_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(shift_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SHIFT, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_shift_key1(do_alias_t *alias, shift_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
#ifndef USE_OLD_SHIFT
DO_EXPORT int do_shift_get2(do_alias_t *alias, shift_rec_t *rec, shift_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(shift_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SHIFT, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_shift_key2(do_alias_t *alias, shift_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_shift_get3(do_alias_t *alias, shift_rec_t *rec, shift_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(shift_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_SHIFT, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_shift_key3(do_alias_t *alias, shift_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_SHIFT, NULL, 0, key, 3);
    }
    return DO_ERROR;
}
#endif
DO_EXPORT int do_checksum_get0(do_alias_t *alias, checksum_rec_t *rec, checksum_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(checksum_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_checksum_key0(do_alias_t *alias, checksum_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_checksum_get1(do_alias_t *alias, checksum_rec_t *rec, checksum_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(checksum_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_CHECKSUM, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_checksum_key1(do_alias_t *alias, checksum_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_CHECKSUM, NULL, 0, key, 1);
    }
    return DO_ERROR;
}

DO_EXPORT int do_check_get0(do_alias_t *alias, check_rec_t *rec, check_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(check_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_CHECK, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_check_key0(do_alias_t *alias, check_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_check_get1(do_alias_t *alias, check_rec_t *rec, check_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(check_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_CHECK, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_check_key1(do_alias_t *alias, check_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_check_get2(do_alias_t *alias, check_rec_t *rec, check_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(check_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_CHECK, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_check_key2(do_alias_t *alias, check_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_CHECK, NULL, 0, key, 2);
    }
    return DO_ERROR;
}

DO_EXPORT int do_realization_get0(do_alias_t *alias, realization_rec_t *rec, realization_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(realization_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_key0(do_alias_t *alias, realization_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_get1(do_alias_t *alias, realization_rec_t *rec, realization_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(realization_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_key1(do_alias_t *alias, realization_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_get2(do_alias_t *alias, realization_rec_t *rec, realization_key2_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(realization_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_key2(do_alias_t *alias, realization_key2_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 2);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_get3(do_alias_t *alias, realization_rec_t *rec, realization_key3_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(realization_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_key3(do_alias_t *alias, realization_key3_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 3);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_get4(do_alias_t *alias, realization_rec_t *rec, realization_key4_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(realization_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_REALIZATION, &rec->data, &rec->size, key, 4);
    }
    return DO_ERROR;
}
DO_EXPORT int do_realization_key4(do_alias_t *alias, realization_key4_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_REALIZATION, NULL, 0, key, 4);
    }
    return DO_ERROR;
}

DO_EXPORT int do_discount_get0(do_alias_t *alias, discount_rec_t *rec, discount_key0_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(discount_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_discount_key0(do_alias_t *alias, discount_key0_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 0);
    }
    return DO_ERROR;
}
DO_EXPORT int do_discount_get1(do_alias_t *alias, discount_rec_t *rec, discount_key1_t *key, do_alias_oper_t operation)
{
    rec->size = sizeof(discount_struct_t);
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST, DO_DB_DISCOUNT, &rec->data, &rec->size, key, 1);
    }
    return DO_ERROR;
}
DO_EXPORT int do_discount_key1(do_alias_t *alias, discount_key1_t *key, do_alias_oper_t operation)
{
    switch (operation) {
        case DO_GET_EQUAL:
            return do_alias_btr(alias, B_GET_EQUAL + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
        case DO_GET_NEXT:
            return do_alias_btr(alias, B_GET_NEXT + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
        case DO_GET_PREVIOUS:
            return do_alias_btr(alias, B_GET_PREVIOUS + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
        case DO_GET_GT:
            return do_alias_btr(alias, B_GET_GT + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
        case DO_GET_GE:
            return do_alias_btr(alias, B_GET_GE + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
        case DO_GET_LT:
            return do_alias_btr(alias, B_GET_LT + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
        case DO_GET_LE:
            return do_alias_btr(alias, B_GET_LE + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
        case DO_GET_FIRST:
            return do_alias_btr(alias, B_GET_FIRST + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
        case DO_GET_LAST:
            return do_alias_btr(alias, B_GET_LAST + B_GET_KEY_ONLY, DO_DB_DISCOUNT, NULL, 0, key, 1);
    }
    return DO_ERROR;
}
#endif

DO_EXPORT int do_document_insert(do_alias_t *alias, document_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_update(do_alias_t *alias, document_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_DOCUMENT, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_document_order_insert(do_alias_t *alias, document_order_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_order_update(do_alias_t *alias, document_order_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_ORDER, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_order_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_DOCUMENT_ORDER, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_document_link_insert(do_alias_t *alias, document_link_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_link_update(do_alias_t *alias, document_link_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_LINK, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_link_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_DOCUMENT_LINK, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_enum_insert(do_alias_t *alias, enum_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_ENUM, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_enum_update(do_alias_t *alias, enum_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_ENUM, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_enum_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_ENUM, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_document_view_insert(do_alias_t *alias, document_view_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_view_update(do_alias_t *alias, document_view_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_VIEW, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_view_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_DOCUMENT_VIEW, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_document_data_insert(do_alias_t *alias, document_data_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_data_update(do_alias_t *alias, document_data_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_DATA, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_data_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_DOCUMENT_DATA, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_product_insert(do_alias_t *alias, product_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PRODUCT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_product_update(do_alias_t *alias, product_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PRODUCT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_product_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PRODUCT, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_product_link_insert(do_alias_t *alias, product_link_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_product_link_update(do_alias_t *alias, product_link_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PRODUCT_LINK, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_product_link_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PRODUCT_LINK, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_product_view_insert(do_alias_t *alias, product_view_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_product_view_update(do_alias_t *alias, product_view_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PRODUCT_VIEW, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_product_view_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PRODUCT_VIEW, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_product_data_insert(do_alias_t *alias, product_data_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_product_data_update(do_alias_t *alias, product_data_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PRODUCT_DATA, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_product_data_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PRODUCT_DATA, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_barcode_insert(do_alias_t *alias, barcode_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_BARCODE, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_barcode_update(do_alias_t *alias, barcode_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_BARCODE, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_barcode_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_BARCODE, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_account_insert(do_alias_t *alias, account_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_ACCOUNT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_account_update(do_alias_t *alias, account_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_ACCOUNT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_account_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_ACCOUNT, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_subaccount_insert(do_alias_t *alias, subaccount_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_SUBACCOUNT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_subaccount_update(do_alias_t *alias, subaccount_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_SUBACCOUNT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_subaccount_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_SUBACCOUNT, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_class_insert(do_alias_t *alias, class_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_CLASS, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_class_update(do_alias_t *alias, class_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_CLASS, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_class_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_CLASS, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_group_insert(do_alias_t *alias, group_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_GROUP, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_group_update(do_alias_t *alias, group_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_GROUP, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_group_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_GROUP, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_subgroup_insert(do_alias_t *alias, subgroup_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_SUBGROUP, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_subgroup_update(do_alias_t *alias, subgroup_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_SUBGROUP, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_subgroup_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_SUBGROUP, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_region_insert(do_alias_t *alias, region_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_REGION, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_region_update(do_alias_t *alias, region_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_REGION, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_region_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_REGION, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_otdel_insert(do_alias_t *alias, otdel_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_OTDEL, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_otdel_update(do_alias_t *alias, otdel_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_OTDEL, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_otdel_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_OTDEL, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_partner_insert(do_alias_t *alias, partner_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PARTNER, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_partner_update(do_alias_t *alias, partner_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PARTNER, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_partner_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PARTNER, NULL, NULL, NULL, -1);
}
#ifdef USE_DB_PARNER_VIEW
DO_EXPORT int do_partner_view_insert(do_alias_t *alias, partner_view_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_partner_view_update(do_alias_t *alias, partner_view_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PARTNER_VIEW, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_partner_view_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PARTNER_VIEW, NULL, NULL, NULL, -1);
}
#endif
DO_EXPORT int do_partner_data_insert(do_alias_t *alias, partner_data_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PARTNER_DATA, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_partner_data_update(do_alias_t *alias, partner_data_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PARTNER_DATA, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_partner_data_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PARTNER_DATA, NULL, NULL, NULL, -1);
}


DO_EXPORT int do_sklad_insert(do_alias_t *alias, sklad_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_SKLAD, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_sklad_update(do_alias_t *alias, sklad_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_SKLAD, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_sklad_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_SKLAD, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_document_prow_insert(do_alias_t *alias, document_prow_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_prow_update(do_alias_t *alias, document_prow_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_PROW, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_prow_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_DOCUMENT_PROW, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_document_type_insert(do_alias_t *alias, document_type_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_type_update(do_alias_t *alias, document_type_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_DOCUMENT_TYPE, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_document_type_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_DOCUMENT_TYPE, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_stock_insert(do_alias_t *alias, stock_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_STOCK, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_stock_update(do_alias_t *alias, stock_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_STOCK, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_stock_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_STOCK, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_prowod_insert(do_alias_t *alias, prowod_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PROWOD, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_prowod_update(do_alias_t *alias, prowod_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PROWOD, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_prowod_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PROWOD, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_prowod2_insert(do_alias_t *alias, prowod2_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PROWOD2, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_prowod2_update(do_alias_t *alias, prowod2_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PROWOD2, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_prowod2_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PROWOD2, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_saldo_insert(do_alias_t *alias, saldo_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_SALDO, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_saldo_update(do_alias_t *alias, saldo_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_SALDO, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_saldo_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_SALDO, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_protocol_insert(do_alias_t *alias, protocol_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_PROTOCOL, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_protocol_update(do_alias_t *alias, protocol_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_PROTOCOL, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_protocol_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_PROTOCOL, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_user_insert(do_alias_t *alias, user_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_USER, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_user_update(do_alias_t *alias, user_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_USER, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_user_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_USER, NULL, NULL, NULL, -1);
}
#ifndef DOMINO78
DO_EXPORT int do_shift_insert(do_alias_t *alias, shift_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_SHIFT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_shift_update(do_alias_t *alias, shift_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_SHIFT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_shift_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_SHIFT, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_checksum_insert(do_alias_t *alias, checksum_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_CHECKSUM, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_checksum_update(do_alias_t *alias, checksum_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_CHECKSUM, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_checksum_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_CHECKSUM, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_check_insert(do_alias_t *alias, check_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_CHECK, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_check_update(do_alias_t *alias, check_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_CHECK, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_check_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_CHECK, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_realization_insert(do_alias_t *alias, realization_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_REALIZATION, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_realization_update(do_alias_t *alias, realization_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_REALIZATION, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_realization_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_REALIZATION, NULL, NULL, NULL, -1);
}

DO_EXPORT int do_discount_insert(do_alias_t *alias, discount_rec_t *rec)
{
    return do_alias_btr(alias, B_INSERT, DO_DB_DISCOUNT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_discount_update(do_alias_t *alias, discount_rec_t *rec)
{
    return do_alias_btr(alias, B_UPDATE, DO_DB_DISCOUNT, &rec->data, &rec->size, NULL, -1);
}
DO_EXPORT int do_discount_delete(do_alias_t *alias)
{
    return do_alias_btr(alias, B_DELETE, DO_DB_DISCOUNT, NULL, NULL, NULL, -1);
}



DO_EXPORT int do_alias_discount_create(do_alias_t *alias)
{
    BTI_WORD dataLen = 144;
    BTI_SINT status;
    FILE_CREATE_BUF fileCreateBuf;
    memset(&fileCreateBuf, 0, sizeof(fileCreateBuf));

    fileCreateBuf.fileSpecs.recLength = 34;
    fileCreateBuf.fileSpecs.pageSize = 1024;
    fileCreateBuf.fileSpecs.indexCount = 2;
    fileCreateBuf.fileSpecs.flags = 512;

    fileCreateBuf.keySpecs[0].position = 5;
    fileCreateBuf.keySpecs[0].length = 2;
    fileCreateBuf.keySpecs[0].flags = 403;
    fileCreateBuf.keySpecs[0].type = 1;

    fileCreateBuf.keySpecs[1].position = 7;
    fileCreateBuf.keySpecs[1].length = 2;
    fileCreateBuf.keySpecs[1].flags = 403;
    fileCreateBuf.keySpecs[1].type = 1;

    fileCreateBuf.keySpecs[2].position = 11;
    fileCreateBuf.keySpecs[2].length = 4;
    fileCreateBuf.keySpecs[2].flags = 403;
    fileCreateBuf.keySpecs[2].type = 3;

    fileCreateBuf.keySpecs[3].position = 9;
    fileCreateBuf.keySpecs[3].length = 2;
    fileCreateBuf.keySpecs[3].flags = 403;
    fileCreateBuf.keySpecs[3].type = 1;

    fileCreateBuf.keySpecs[4].position = 15;
    fileCreateBuf.keySpecs[4].length = 4;
    fileCreateBuf.keySpecs[4].flags = 387;
    fileCreateBuf.keySpecs[4].type = 4;


    fileCreateBuf.keySpecs[5].position = 1;
    fileCreateBuf.keySpecs[5].length = 4;
    fileCreateBuf.keySpecs[5].flags = 403;
    fileCreateBuf.keySpecs[5].type = 1;
    fileCreateBuf.keySpecs[5].manualKeyNumber = 1;

    fileCreateBuf.keySpecs[6].position = 11;
    fileCreateBuf.keySpecs[6].length = 4;
    fileCreateBuf.keySpecs[6].flags = 403;
    fileCreateBuf.keySpecs[6].type = 3;
    fileCreateBuf.keySpecs[6].manualKeyNumber = 1;

    fileCreateBuf.keySpecs[7].position = 15;
    fileCreateBuf.keySpecs[7].length = 4;
    fileCreateBuf.keySpecs[7].flags = 387;
    fileCreateBuf.keySpecs[7].type = 4;
    fileCreateBuf.keySpecs[7].manualKeyNumber = 1;

    BTI_BYTE dataBuf[256];
    status = BTRVID(B_CREATE, dataBuf, &fileCreateBuf, &dataLen, alias->dbfilename[DO_DB_DISCOUNT], 0, (BTI_BUFFER_PTR)&alias->clientID);
    if (status != B_NO_ERROR) {
       do_log(LOG_ERR, "Btrieve B_CREATE %s status = %d", alias->dbfilename[DO_DB_DISCOUNT], status);
       return 0;
    }
    return 1;
}
#endif
static void from_do_init(do_alias_t *alias, int utf) {

    const char *str;
    if ( utf )
        str = UTF_CHARSET;
    else
        str = get_locale_lang();
    alias->fdo_conv = iconv_open(str, DOMINO_CHARSET);
    if (alias->fdo_conv == (iconv_t)-1) {
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", DOMINO_CHARSET, str);
        return;
    }
#ifdef PSEUDO_NUMBER_SIGN
    char buf1[1] = {NUMBER_SIGN};
    size_t len1 = 1, len2 = 5;
    char buf2[5];
    char *pbuf1 = buf1, *pbuf2 = buf2;
    number_sign_exist = iconv(res, &pbuf1, &len1, &pbuf2, &len2) != -1;
    //do_log_debug(__FILE__, __LINE__, "number sign exist %d", number_sign_exist);
#endif
}

static void to_do_init(do_alias_t *alias, int utf) {
    const char* str;
    if ( utf ) {
        str = UTF_CHARSET;
    }
    else
        str = get_locale_lang();
    alias->tdo_conv = iconv_open(DOMINO_CHARSET, str);
    if (alias->tdo_conv == (iconv_t)-1)
        do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", str, DOMINO_CHARSET);
}

static iconv_t fdo_conv = (iconv_t)-1;

static char* domino_text_(void* src, size_t size, int utf)
{
    char* buf = (char*)do_malloc(size + 1);
    buf[size] = '\0';
    strncpy(buf, (char*)src, size);
    char* cp = buf + size - 1;
    //while (cp >= buf) { if (*cp == '\0')  *cp = ' ';  --cp; };
    //cp = buf + size - 1;
    while ((cp >= buf) && (*cp == 32)) --cp;
    *(cp+1) = '\0';
    if ( fdo_conv == (iconv_t)-1 ) {
      const char *str;
      if ( utf )
        str = UTF_CHARSET;
      else
        str = get_locale_lang();
      fdo_conv = iconv_open(str, DOMINO_CHARSET);
      if (fdo_conv == (iconv_t)-1) {
          do_log(LOG_ERR, "Error iconv_open (\"%s\" to \"%s\") ", DOMINO_CHARSET, str);
          return NULL;
      }
    }
    char* result = iconv_(fdo_conv, buf);
    do_free(buf);
    return result;
}

DO_EXPORT char* do_alias_text_(do_alias_t *alias, void* src, size_t size)
{
    char* buf = (char*)do_malloc(size + 1);
    char* cp;
    buf[size] = '\0';
    memcpy(buf, src, size);

    cp = buf + size - 1;
    while (cp >= buf) { if (*cp == '\0') {*cp = ' ';} --cp; };
    cp = buf + size - 1;
    while ((cp >= buf) && (*cp == 32)) --cp;
    *(cp+1) = '\0';
    char* result = iconv_(alias->fdo_conv, buf);
    do_free(buf);
    return result;
}
static void* strcpy32 (void* dst, const char* src, size_t size)
{
    if (strlen(src) >= size)
        return memcpy(dst, src, size);
    else
    {
        memset( dst, 32, size);
        return memcpy(dst, src, strlen(src));
    }
};
/*
static char* fromstr32 (void* src, size_t size)
{
    char* buf = (char*)do_malloc(size + 2);
    buf[size + 1] = '\0';
    strncpy(buf, (char*)src, size);
    char* cp = buf + size - 1;
    while ((cp > buf) && (*cp == 32)) --cp;
    *(cp+1) = '\0';
    return buf;
};
*/
DO_EXPORT void* do_alias_text_set_(do_alias_t *alias, void* dst, const char* src, size_t size)
{
    char* buf = iconv_(alias->tdo_conv, src);
    strcpy32(dst, buf, size);
    do_free(buf);
    return dst;
}
DO_EXPORT int do_alias_text_check_set(do_alias_t *alias, const char* src)
{
    char* buf = iconv_with_error(alias->tdo_conv, src);
    if (!buf)
        return 0;
    char* buf1 = iconv_with_error(alias->fdo_conv, buf);
    if (!buf1) {
        do_free(buf);
        return 0;
    }
    int retval;
    retval = !strcmp(buf1, src);
    if (!retval)
        do_free(buf);
    do_free(buf1);
    return retval;
}


#define DO_PARAM_SUM_MAP  "SUM_MAP"
#define DO_PARAM_SUM  "SUM"

// local function
static int tooem32param(do_alias_t *alias, const char* dst, const char* name, const char *value, size_t size, size_t capacity);

DO_EXPORT double do_product_price(do_alias_t *alias, product_rec_t *product, const int number)
{
    int i;
    for (i = 0; i < product->data.total_price; i++)
        if (product->data.nprice[i] == number)
            return product->data.price[i];
    return 0;
}
DO_EXPORT void do_product_price_clear(do_alias_t *alias, product_rec_t *product)
{
    product->data.price_filled = 1;
    product->data.total_price = 0;
}

DO_EXPORT int do_product_price_set(do_alias_t *alias, product_rec_t *product, const int number, double value)
{
    int i;
    product->data.price_filled = 1;
    for (i = 0; i < product->data.total_price; i++)
        if (product->data.nprice[i] == number) {
            product->data.price[i] = value;
            return 1;
        }
    if (product->data.total_price < 15) {
        product->data.nprice[product->data.total_price] = number;
        product->data.price[product->data.total_price++] = value;
        return 1;
    }
    do_log(LOG_ERR, "\"%s\" product prices is full", do_text(alias, product->data.code));
    return 0;
}

DO_EXPORT double do_document_order_price(do_alias_t *alias, document_order_rec_t *document_order, const int number)
{
    int i;
    for (i = 0; i < 8; i++)
        if (document_order->data.nprice[i] == number)
            return document_order->data.price[i];
    return 0;
}
DO_EXPORT void do_document_order_price_clear(do_alias_t *alias, document_order_rec_t *rec)
{
    memset(&rec->data.nprice, 0, sizeof(rec->data.nprice));
}

DO_EXPORT int do_document_order_price_set(do_alias_t *alias, document_order_rec_t *document_order, const int number, double value)
{
    int i;
    for (i = 0; i < 8; i++) {
        if (document_order->data.nprice[i] == number) {
            document_order->data.price[i] = value;
            return 1;
        }
        if (!document_order->data.nprice[i]) {
            document_order->data.nprice[i] = number;
            document_order->data.price[i] = value;
            return 1;
        }
    }
    do_log(LOG_ERR, "\"%s.%s\\%s.%d\" document_order prices is full", do_text(alias, document_order->data.dtype),
                do_text(alias, document_order->data.document),
                do_text(alias, document_order->data.sklad),
                document_order->data.line);
    return 0;
}

DO_EXPORT double do_prowod_sum(do_alias_t *alias, prowod_rec_t *rec, int number)
{
    return do_pdecimal(rec->data.sums[number], 8, 2);
}

DO_EXPORT void do_prowod_sum_set(do_alias_t *alias, prowod_rec_t *rec, int number, double value)
{
    do_pdecimal_set(rec->data.sums[number], 8, 2, value);
}
DO_EXPORT void   do_prowod_sum_clear(do_alias_t *alias, prowod_rec_t *rec)
{
    memset(rec->data.sums, 0, sizeof(rec->data.sums));
}

DO_EXPORT double do_document_order_quant(do_alias_t *alias, document_order_rec_t *document_order, const int number)
{
    int i;
    for (i = 0; i < 4; i++)
        if (document_order->data.nquant[i] == number)
            return do_pdecimal(document_order->data.quant[i], 7, 3);
    return 0;
}
DO_EXPORT void   do_document_order_quant_clear(do_alias_t *alias, document_order_rec_t *rec)
{
    memset(rec->data.quant, 0, sizeof(rec->data.quant) + sizeof(rec->data.nquant));
}

DO_EXPORT int do_document_order_quant_set(do_alias_t *alias, document_order_rec_t *document_order, const int number, double value)
{
    int i;
    for (i = 0; i < 4; i++) {
        if (document_order->data.nquant[i] == number) {
            do_pdecimal_set(document_order->data.quant[i], 7, 3, value);
            return 1;
        }
        if (!document_order->data.nquant[i]) {
            document_order->data.nquant[i] = number;
            do_pdecimal_set(document_order->data.quant[i], 7, 3, value);
            return 1;
        }
    }
    do_log(LOG_ERR, "\"%s.%s\\%s.%d\" document_order quant is full", do_text(alias, document_order->data.dtype),
                do_text(alias, document_order->data.document),
                do_text(alias, document_order->data.sklad),
                document_order->data.line);
    return 0;
}

DO_EXPORT double do_stock_quant(do_alias_t *alias, stock_rec_t *stock, const int number)
{
    int i;
    for (i = 0; i < 6; i++)
        if (stock->data.nquant[i] == number)
            return do_pdecimal(stock->data.quant[i], 8, 3);
    return 0;
}

DO_EXPORT double do_stock_struct_quant(do_alias_t *alias, stock_struct_t *stock, const int number)
{
    int i;
    for (i = 0; i < 6; i++)
        if (stock->nquant[i] == number)
            return do_pdecimal(stock->quant[i], 8, 3);
    return 0;
}

DO_EXPORT int do_stock_quant_set(do_alias_t *alias, stock_rec_t *stock, const int number, double value)
{
    int i;
    for (i = 0; i < 6; i++) {
        if (stock->data.nquant[i] == number) {
            do_pdecimal_set(stock->data.quant[i], 8, 3, value);
            return 1;
        }
        if (!stock->data.nquant[i]) {
            stock->data.nquant[i] = number;
            do_pdecimal_set(stock->data.quant[i], 8, 3, value);
            return 1;
        }
    }
    do_log(LOG_ERR, "\"%s\\%s\" stock quant is full", do_text(alias, stock->data.code),
                do_text(alias, stock->data.sklad));
    return 0;
}
DO_EXPORT int do_stock_struct_quant_set(do_alias_t *alias, stock_struct_t *rec, const int number, double value)
{
    int i;
    for (i = 0; i < 6; i++) {
        if (rec->nquant[i] == number) {
            do_pdecimal_set(rec->quant[i], 8, 3, value);
            return 1;
        }
        if (!rec->nquant[i]) {
            rec->nquant[i] = number;
            do_pdecimal_set(rec->quant[i], 8, 3, value);
            return 1;
        }
    }
    do_log(LOG_ERR, "\"%s\\%s\" stock quant is full", do_text(alias, rec->code),
                do_text(alias, rec->sklad));
    return 0;
}

DO_EXPORT void do_stock_quant_clear(do_alias_t *alias, stock_rec_t *stock)
{
    memset(stock->data.nquant, 0, sizeof(stock->data.nquant));
}

DO_EXPORT double do_product_coef(do_alias_t *alias, product_rec_t *rec)
{
    return do_pdecimal((unsigned char*)&rec->data.tech_unit_rate, 4, 3);
}
DO_EXPORT double do_barcode_sk(do_alias_t *alias, barcode_rec_t *rec)
{
    return do_pdecimal((unsigned char*)&rec->data.sk, 3, 2);
}

DO_EXPORT int do_product_life_time(do_alias_t *alias, product_rec_t *rec, struct tm *tm)
{
    if (rec->data.life_time)
        return do_date(rec->data.life_time, tm);
    char *life_time = do_product_param(alias, rec, do_param(DO_PARAM_PRODUCT_PARAMETR_LIFE_TIME));
    int retval = 0;
    if (*life_time != '\0') {
        retval = do_atodate(life_time, tm);
    }
    do_free(life_time);
    return retval;
}



DO_EXPORT double do_sum(const char *params, const int number, int utf8)
{
    char *sum = do_fromparam(params, DO_PARAM_SUM_MAP, utf8);
    if (!strlen(sum)) {
        do_free(sum);
        return 0;
    }
    char *cp = sum, *head;
    int i, indx = 0;
    for (i = 0; *cp != '\0'; i++) {
        head = cp;
        while (*cp != '\0' && *cp != ',') cp++;
        if (head < cp + 1 && atoi(head) == number) {
            indx = i + 1;
            break;
        }
        if (*cp != '\0') cp++;
    }
    do_free(sum);
    if (!indx)
        return 0;
    sum = do_fromparam(params, DO_PARAM_SUM, utf8);
    if (!strlen(sum)) {
        do_free(sum);
        return 0;
    }
    cp = sum;
    double result;
    while (*cp != '\0' && indx) {
        head = cp;
        while (*cp != '\0' && *cp != ',') cp++;
        indx--;
        if (!indx) {
            if (head == cp)
                return 0;
            *cp = '\0';
            result = do_atof(head);
            do_free(sum);
            return result;
        }
        if (*cp != '\0') cp++;
    }
    return 0;

}
DO_EXPORT double do_document_sum(do_alias_t *alias, document_rec_t *rec, const int number)
{
    char *params;
    double sum;
    params =  do_text(alias, rec->data.params);
    sum = do_sum(params, number, alias->utf8);
    do_free(params);
    return sum;

}
DO_EXPORT void do_document_sum_clear(do_alias_t *alias, document_rec_t *rec)
{
    do_document_param_set(alias, rec, DO_PARAM_SUM_MAP, "");
    do_document_param_set(alias, rec, DO_PARAM_SUM, "");
}
DO_EXPORT char* do_ftoasum(const double value)
{
    char *res = do_malloc(100);
    sprintf(res, "%.2f", value);
    struct lconv *s = localeconv();
    char decimal_point = s->decimal_point[0];
    if (decimal_point !=  '.')
    {
        char* cp = res;
        for (; *cp; cp++)
            if (*cp == decimal_point)
            {
                *cp = '.';
                break;
            };
    }
    return res;
};

DO_EXPORT void do_document_sum_set(do_alias_t *alias, document_rec_t *rec, const int number, double value)
{
    char* sum_map = do_document_param(alias, rec, DO_PARAM_SUM_MAP);
    char* sum = do_document_param(alias, rec, DO_PARAM_SUM);
    char *cp = sum_map, *head, *val,
          *valuestr = do_ftoasum(value);
    int i, indx = 0;
    for (i = 0; *cp != '\0'; i++) {
        head = cp;
        while (*cp != '\0' && *cp != ',') cp++;
        if (head < cp + 1 && atoi(head) == number) {
            indx = i + 1;
            break;
        }
        if (*cp != '\0') cp++;
    }
    if (!indx) {
        indx = i + 1;
        val = (char*) do_malloc(strlen(sum_map) + 1 + 20 + 1);
        if (strlen(sum_map))
            sprintf(val, "%s,%d", sum_map, number);
        else
            sprintf(val, "%d", number);
        do_document_param_set(alias, rec, DO_PARAM_SUM_MAP, val);
        do_free(val);
    }
    cp = sum;
    val = NULL;
    while (*cp != '\0' && indx) {
        head = cp;
        while (*cp != '\0' && *cp != ',') cp++;
        indx--;
        if (!indx) {
            val = (char*) do_malloc((head - sum) + strlen(valuestr) + (strlen(sum) - (cp - sum)) + 10);
            strncpy(val, sum, head - sum);
            strcpy(val + (head - sum), valuestr);//, strlen(valuestr));
            strcpy(val + (head - sum) + strlen(valuestr), cp);
            break;
        }
        if (*cp != '\0') cp++;
    }
    if (!val) {
        if (strlen(sum)) {
            val = (char*) do_malloc(strlen(sum) + 1 + strlen(valuestr) + 1);
            sprintf(val, "%s,%s", sum, valuestr);
        }
        else
            val = do_strdup(valuestr);
    }

    do_document_param_set(alias, rec, DO_PARAM_SUM, val);
    do_free(val);
    do_free(valuestr);
    do_free(sum);
    do_free(sum_map);
}


// Perl

DO_EXPORT char *do_document_param(do_alias_t *alias, document_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_document_param_int(do_alias_t *alias, document_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_document_param_float(do_alias_t *alias, document_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_document_param_set(do_alias_t *alias, document_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_param_int_set(do_alias_t *alias, document_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_param_float_set(do_alias_t *alias, document_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_params_clear(do_alias_t *alias, document_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}
DO_EXPORT void do_document_key0_set(document_key0_t *document_key0, document_rec_t *rec)
{
    memcpy(&document_key0->dtype, &rec->data.dtype, sizeof(document_key0->dtype));
    memcpy(&document_key0->sklad, &rec->data.sklad, sizeof(document_key0->sklad));
    memcpy(&document_key0->document, &rec->data.document, sizeof(document_key0->document));
}

DO_EXPORT char *do_document_order_param(do_alias_t *alias, document_order_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_document_order_param_int(do_alias_t *alias, document_order_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_document_order_param_float(do_alias_t *alias, document_order_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_document_order_param_set(do_alias_t *alias, document_order_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_order_param_int_set(do_alias_t *alias, document_order_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_order_param_float_set(do_alias_t *alias, document_order_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_order_params_clear(do_alias_t *alias, document_order_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}
DO_EXPORT void do_document_order_key0_set(document_order_key0_t *document_order_key0, document_order_rec_t *rec)
{
    memcpy(&document_order_key0->dtype, &rec->data.dtype, sizeof(document_order_key0->dtype));
    memcpy(&document_order_key0->sklad, &rec->data.sklad, sizeof(document_order_key0->sklad));
    memcpy(&document_order_key0->document, &rec->data.document, sizeof(document_order_key0->document));
    document_order_key0->line = rec->data.line;
}
DO_EXPORT void do_document_order_key0_set_from_document(document_order_key0_t *document_order_key0, document_rec_t *rec)
{
    memcpy(&document_order_key0->dtype, &rec->data.dtype, sizeof(document_order_key0->dtype));
    memcpy(&document_order_key0->sklad, &rec->data.sklad, sizeof(document_order_key0->sklad));
    memcpy(&document_order_key0->document, &rec->data.document, sizeof(document_order_key0->document));
    document_order_key0->line = 0;
}
DO_EXPORT void do_document_order_key2_set_from_document(document_order_key2_t *document_order_key2, document_rec_t *rec)
{
    memcpy(&document_order_key2->dtype, &rec->data.dtype, sizeof(document_order_key2->dtype));
    memcpy(&document_order_key2->sklad, &rec->data.sklad, sizeof(document_order_key2->sklad));
    memcpy(&document_order_key2->document, &rec->data.document, sizeof(document_order_key2->document));
    memset(&document_order_key2->code, ' ', sizeof(document_order_key2->code));
}

DO_EXPORT char *do_document_data_param(do_alias_t *alias, document_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_document_data_param_int(do_alias_t *alias, document_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_document_data_param_float(do_alias_t *alias, document_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_document_data_param_set(do_alias_t *alias, document_data_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_data_param_int_set(do_alias_t *alias, document_data_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_data_param_float_set(do_alias_t *alias, document_data_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_data_params_clear(do_alias_t *alias, document_data_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_product_param(do_alias_t *alias, product_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_product_param_int(do_alias_t *alias, product_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_product_param_float(do_alias_t *alias, product_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_product_param_set(do_alias_t *alias, product_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_param_int_set(do_alias_t *alias, product_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_param_float_set(do_alias_t *alias, product_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_params_clear(do_alias_t *alias, product_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_product_link_param(do_alias_t *alias, product_link_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_product_link_param_int(do_alias_t *alias, product_link_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_product_link_param_float(do_alias_t *alias, product_link_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_product_link_param_set(do_alias_t *alias, product_link_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_link_param_int_set(do_alias_t *alias, product_link_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_link_param_float_set(do_alias_t *alias, product_link_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_link_params_clear(do_alias_t *alias, product_link_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_product_data_param(do_alias_t *alias, product_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_product_data_param_int(do_alias_t *alias, product_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_product_data_param_float(do_alias_t *alias, product_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_product_data_param_set(do_alias_t *alias, product_data_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_data_param_int_set(do_alias_t *alias, product_data_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_data_param_float_set(do_alias_t *alias, product_data_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_product_data_params_clear(do_alias_t *alias, product_data_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_subgroup_param(do_alias_t *alias, subgroup_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_subgroup_param_int(do_alias_t *alias, subgroup_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_subgroup_param_float(do_alias_t *alias, subgroup_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_subgroup_param_set(do_alias_t *alias, subgroup_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_subgroup_param_int_set(do_alias_t *alias, subgroup_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_subgroup_param_float_set(do_alias_t *alias, subgroup_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_subgroup_params_clear(do_alias_t *alias, subgroup_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_region_param(do_alias_t *alias, region_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_region_param_int(do_alias_t *alias, region_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_region_param_float(do_alias_t *alias, region_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_region_param_set(do_alias_t *alias, region_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_region_param_int_set(do_alias_t *alias, region_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_region_param_float_set(do_alias_t *alias, region_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_region_params_clear(do_alias_t *alias, region_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_otdel_param(do_alias_t *alias, otdel_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_otdel_param_int(do_alias_t *alias, otdel_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_otdel_param_float(do_alias_t *alias, otdel_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_otdel_param_set(do_alias_t *alias, otdel_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_otdel_param_int_set(do_alias_t *alias, otdel_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_otdel_param_float_set(do_alias_t *alias, otdel_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_otdel_params_clear(do_alias_t *alias, otdel_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_partner_param(do_alias_t *alias, partner_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_partner_param_int(do_alias_t *alias, partner_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_partner_param_float(do_alias_t *alias, partner_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_partner_param_set(do_alias_t *alias, partner_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_partner_param_int_set(do_alias_t *alias, partner_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_partner_param_float_set(do_alias_t *alias, partner_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_partner_params_clear(do_alias_t *alias, partner_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_sklad_param(do_alias_t *alias, sklad_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_sklad_param_int(do_alias_t *alias, sklad_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_sklad_param_float(do_alias_t *alias, sklad_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_sklad_param_set(do_alias_t *alias, sklad_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_sklad_param_int_set(do_alias_t *alias, sklad_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_sklad_param_float_set(do_alias_t *alias, sklad_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_sklad_params_clear(do_alias_t *alias, sklad_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_document_prow_param(do_alias_t *alias, document_prow_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_document_prow_param_int(do_alias_t *alias, document_prow_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_document_prow_param_float(do_alias_t *alias, document_prow_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_document_prow_param_set(do_alias_t *alias, document_prow_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_prow_param_int_set(do_alias_t *alias, document_prow_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_prow_param_float_set(do_alias_t *alias, document_prow_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_prow_params_clear(do_alias_t *alias, document_prow_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_document_type_param(do_alias_t *alias, document_type_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_document_type_param_int(do_alias_t *alias, document_type_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_document_type_param_float(do_alias_t *alias, document_type_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_document_type_param_set(do_alias_t *alias, document_type_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_type_param_int_set(do_alias_t *alias, document_type_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_type_param_float_set(do_alias_t *alias, document_type_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_document_type_params_clear(do_alias_t *alias, document_type_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_prowod_param(do_alias_t *alias, prowod_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_prowod_param_int(do_alias_t *alias, prowod_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_prowod_param_float(do_alias_t *alias, prowod_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_prowod_param_set(do_alias_t *alias, prowod_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_prowod_param_int_set(do_alias_t *alias, prowod_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_prowod_param_float_set(do_alias_t *alias, prowod_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_prowod_params_clear(do_alias_t *alias, prowod_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_prowod2_param(do_alias_t *alias, prowod2_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_prowod2_param_int(do_alias_t *alias, prowod2_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_prowod2_param_float(do_alias_t *alias, prowod2_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_prowod2_param_set(do_alias_t *alias, prowod2_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_prowod2_param_int_set(do_alias_t *alias, prowod2_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_prowod2_param_float_set(do_alias_t *alias, prowod2_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_prowod2_params_clear(do_alias_t *alias, prowod2_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}

DO_EXPORT char *do_user_param(do_alias_t *alias, user_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_user_param_int(do_alias_t *alias, user_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_user_param_float(do_alias_t *alias, user_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_user_param_set(do_alias_t *alias, user_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_user_param_int_set(do_alias_t *alias, user_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_user_param_float_set(do_alias_t *alias, user_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_user_params_clear(do_alias_t *alias, user_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}


DO_EXPORT char* do_fromparam(const char* src, const char* name, int utf8)
{
    if (!name)
        return do_strdup("");
    char *value, *cp = (char*)src, *head;
    //int len = strlen(name);
    char buf[1024];
    char *Uname,*buf1;
    if ( utf8 )
        Uname = utf8_upper(name);
    else
        Uname = do_strtowupper(name);
    while (*cp != '\0') {
        head = cp;
        while (*cp != '\0' && *cp != '\n' && *cp !='\r' && *cp != '=') cp++;
        if (*cp != '=' ) {
            while (*cp == '\n' || *cp =='\r') cp++;
            continue;
        }
        strncpy(buf, head, cp - head);
        buf[cp-head]='\0';
        if ( utf8 )
            buf1=utf8_upper(buf);
        else
            buf1=do_strtowupper(buf);
        if (!strcmp(Uname, buf1)) {
            do_free(buf1);
            head = ++cp;
            while (*cp != '\0' && *cp != '\n' && *cp !='\r') cp++;
            value = (char*)do_malloc(cp - head + 1);
            strncpy(value, head, cp - head);
            value[cp - head] = '\0';
            do_free(Uname);
            return value;
        }
        do_free(buf1);
        while (*cp != '\0' && *cp != '\n' && *cp !='\r') cp++;
        while (*cp == '\n' || *cp =='\r') cp++;
    }
    do_free(Uname);
    return do_strdup("");
}

static int tooem32param(do_alias_t *alias, const char* dst, const char* name, const char *value, size_t size, size_t capacity)
{
#ifdef DEBUG
    *((char*)(dst + size)) = '\0';
#endif

	char *Uname;
	if ( alias->utf8 )
        Uname = utf8_upper(name);
    else
        Uname = do_strtowupper(name);
    char *n = iconv_(alias->tdo_conv, (char*)Uname);
    char *v = iconv_(alias->tdo_conv, (char*)value);
    char *cp = (char*)dst, *np, *head = NULL, *tail = NULL;
    char *buf_, *buf1;
    //int len = strlen(n);
    char buf[1024];
    while (cp - dst < size) {
        np = cp;
        while ((cp - dst < size) && *cp != '\n' && *cp !='\r' && *cp != '=') cp++;
        if (!(cp - dst < size)) break;
        if (*cp != '=' ) {
            while (*cp == '\n' || *cp =='\r') cp++;
            continue;
        }
        strncpy(buf, np, cp - np);
        buf[cp-np]='\0';
        buf_ = iconv_(alias->fdo_conv, buf);
        if ( alias->utf8 )
            buf1 = utf8_upper(buf_);
        else
            buf1 = do_strtowupper(buf_);
        if ( !strcmp(buf1, Uname) ) {
        //if (!strncmp(n, np, cp - np) && (len == (cp - np))) {
            head = np;
            while ((cp - dst < size) && *cp != '\n' && *cp !='\r') cp++;
            tail = (char*) do_malloc(size - (cp - dst) + 1);
            strncpy(tail, cp , size - (cp - dst));
            tail[size - (cp - dst)]  = '\0';
            do_free(buf_);do_free(buf1);
            break;
        }
        do_free(buf_);do_free(buf1);
        while ((cp - dst < size) && *cp != '\n' && *cp !='\r') cp++;
        while ((cp - dst < size) && (*cp == '\n' || *cp =='\r')) cp++;
    }
    do_free(Uname);
    if (!head) {
        head = (char*)dst + size;
        if (size && (*(head - 1)!='\n')) {
            *head = '\n';
             head++;
        }
    }
    np = head;
    if ( capacity < (np - dst) + strlen(n) + strlen(v) + 1 ) {
        do_log(LOG_ERR, "parameter overflow, capacity: %d, crnt: %d, namelen: %d, valuelen: %d", capacity, (np - dst), strlen(n), strlen(v));
        do_free(n);do_free(v);if(tail) do_free(tail);
        return capacity < (np - dst) ? capacity : (np - dst);
    }
    strncpy(np, n, (capacity > (np - dst) + strlen(n)) ? strlen(n) : capacity - (np - dst));
    np += (capacity > (np - dst) + strlen(n)) ? strlen(n) : capacity - (np - dst);
    if (np - dst >= capacity) {
        do_free(n);do_free(v);if(tail) do_free(tail);
        return capacity;
    };
    *np = '='; np++;
    strncpy(np, v, (capacity > (np - dst) + strlen(v)) ? strlen(v) : capacity - (np - dst));
    np += (capacity > (np - dst) + strlen(v)) ? strlen(v) : capacity - (np - dst);
    if (np - dst >= capacity) {
        do_free(n);do_free(v);if(tail) do_free(tail);
        return capacity;
    };
    if (tail) {
        strncpy(np, tail, (capacity > (np - dst) + strlen(tail)) ? strlen(tail) : capacity - (np - dst));
        np += (capacity > (np - dst) + strlen(tail)) ? strlen(tail) : capacity - (np - dst);
    }
    else {
        *np = '\n';
        np++;
    }
    do_free(n);do_free(v);if(tail) do_free(tail);
    return np - dst;
}

static char *read_user_db_config(const char* dir, const char *name)
{
    FILE *fp;
    char *filename = (char*)do_malloc(strlen(dir) + strlen(name) + 2);
#ifdef _WIN32
    sprintf(filename, "%s\\%s", dir, name);
#elif defined(_LINUX)
    sprintf(filename, "%s/%s", dir, name);
#endif
    if ((fp = fopen(filename, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s", filename, strerror(errno));
        do_free(filename);
        return NULL;
    }

    size_t len = 1024;
    char* line = (char *)do_malloc(len + 1);
    char *file = NULL;
    char *host = NULL;
    char **val;
    char *retval = NULL;

    int error = 0;
    while (!feof(fp))  {
        char* cp, *name, *value, *tail;
        if (fgets(line, len + 1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = do_realloc(line, len * 2 + 1);
            if (!fgets(line + len, len + 1, fp)) {
                line = '\0';
                error = 1;
                break;
            }
            len *= 2;
        }
        if (error)
            break;
        cp = line;
        while (cp[0] == ' ') ++cp; // delspace
        /* A leading '#' or ';' signals a comment line.  */
        if ( cp[0] == '#' || cp[0] == ';' || cp[0] == '\0' || cp[0] == '\n' || cp[0] == '\r' )
            continue;
        name = cp;
        while (cp[0] != '\0' && cp[0] != '\n' && cp[0] != ' ') ++cp;
        if (name == cp)
            continue;

        if ( strlen(USER_DB_NAME) == (cp - name) && !strncmp(USER_DB_NAME, name, cp - name) )
            val = &file;
        else
        if ( strlen(DB_HOST) == (cp - name) && !strncmp(DB_HOST, name, cp - name) )
            val = &host;
        else
            continue;

        while (cp[0] == ' ') ++cp; // delspace
        value = cp;
        while (cp[0] != '\0' && cp[0] != '\n') ++cp;
        tail = cp - 1;
        while ( tail >= value && (tail[0] == ' ' || tail[0] == '\r') ) tail--;
        if (value > tail)
            continue;

        *val = (char*) do_malloc( tail - value + 2 );
        strncpy(*val, value, tail - value + 1);
        (*val)[tail - value + 1] = '\0';
    }
    do_free(filename);
    do_free(line);
    fclose(fp);
    if ( !host || host[0] == '\0' )  {
        retval = file;
        if ( host )
            do_free(host);
    }
    else {
        retval=do_strdup_printf("//%s%s", host, file);
        do_free(host);
        do_free(file);
    }
    return retval;
}

DO_EXPORT char *decoding_password(const char *password, size_t size)
{
    int i;
    char *retval = do_malloc(size + 1);
    for (i = 0; i < size - 1; i++)
        retval[i] = password[i + 1] ^ ((!i) ? 0x5C : password[i]);
    char *ch = retval + size - 2;
    while ((ch > retval) && (*ch == ' ')) ch--;
    *(ch + 1) = '\0';
    return retval;
}

DO_EXPORT const char *do_alias_get_name(do_alias_t *alias)
{
    return alias->aliasName;
}
const char *empty_str = "";
DO_EXPORT const char *do_alias_get_host(do_alias_t *alias)
{
    if ( alias->host )
        return alias->host;
    else
        return empty_str;
}
void do_alias_set_host(do_alias_t *alias, const char *host)
{
    if ( alias->host )
        do_free(alias->host);
    alias->host=do_strdup(host);
}
DO_EXPORT int do_alias_auth(const char* dbConfDir, const char* aliasName, const char *name, const char *password, char **groups, char **rights, int *super,int utf)
{
    char *dir;
    const char *alias;

    if (dbConfDir)
        dir = do_strdup(dbConfDir);
    else
        dir = DOMINO_CONFIG(DOMINO_ALIAS_RELATIVE_PATH);
    if (aliasName)
        alias = aliasName;
    else
        alias = DOMINO_LOCAL_ALIAS;

    char *filename;
    if ((filename = read_user_db_config(dir, alias)) == NULL) {
        do_free(dir);
        return -1;
    }
    do_free(dir);

    CLIENT_ID clientID;

    if ( !initClientThread(&clientID) )
        return -1;

    BTI_BYTE dbBlock[BTI_BLOCK_SIZE];

    int retval = -1;
    if (openTable(dbBlock, NULL, filename, &clientID)) {

        int status;
        BTI_SINT keyNumber = 0;
        user_rec_t user;
        user_key0_t user_key0;
        strcpy32(&user_key0.type, USER_G_USER, sizeof(user_key0.type));
        strcpy32(&user_key0.name, name, sizeof(user_key0.name));
        user.size = sizeof(user.data);


        status =  BTRVID(B_GET_EQUAL, dbBlock, &user.data, &user.size, &user_key0, keyNumber, (BTI_BUFFER_PTR)&clientID);
        if (status == B_NO_ERROR) {
            char *pas = decoding_password(user.data.pwd, sizeof(user.data.pwd));
            retval = !strcmp(pas, password);
            if (retval) {
                 *groups = domino_text_(user.data.groups, sizeof(user.data.groups), utf);
                *super = user.data.supervisore;
            }
            do_free(pas);
        }
        else
            retval = 0;
        closeTable(dbBlock, &clientID);
    }
    return retval;
}
DO_EXPORT int do_product_get_code_len(char *code, int code_len)
{
    char *ch;
    for ( ch = code + code_len - 1; ch > code && *ch == ' '; ch-- );
    return ch - code + 1;
}
DO_EXPORT double do_get_rest(do_alias_t *alias, const char *code, const char *sklad)
{
    stock_key0_t stock_key0, stock_key;
    stock_key1_t stock_key1;
    stock_rec_t  stock;
    int status;
    int len;
    double retval = 0;
    do_text_set(alias, stock_key1.code, code);
    len = do_product_get_code_len(stock_key1.code, sizeof(stock_key1.code));

    do_cpy(stock_key.code, stock_key1.code);
    if ( sklad ) {
        do_cpy(stock_key0.code, stock_key1.code);
        do_text_set(alias, stock_key0.sklad, sklad);
        do_cpy(stock_key.sklad, stock_key0.sklad);
    }

    if ( sklad )
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
    else
        status = do_stock_get1(alias, &stock, &stock_key1, DO_GET_GE);
    while (status == DO_OK) {
        if ( sklad && do_cmp(stock_key.sklad, stock_key0.sklad) ) break;

        if ( strncmp(stock.data.code, stock_key.code, len) &&
             ( len >= sizeof(stock.data.code) ||
              (stock.data.code[len] != '.' &&
               stock.data.code[len] != ' ') ) ) break;

        retval += do_stock_quant(alias, &stock, DO_CONST_QUANT_REST) +
                  do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        if ( sklad )
            status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
        else
            status = do_stock_get1(alias, &stock, &stock_key1, DO_GET_NEXT);
    }
    return retval;
}

/*
int do_get_rest_and_price(do_alias_t *alias, const char *base_code, sklad_rec_t *sklad, do_list_t *list)
{
    stock_key0_t stock_key0, stock_key;
    stock_rec_t  stock;
    barcode_key1_t barcode_key1;
    product_key0_t product_key0;
    product_rec_t product;
    int status = DO_OK;
    double rest = 0, sale = 0;
    do_rest_price_t rec = {0, 0, 0};
    do_rest_price_t *rec_p, *base_rec;
    char *sklad_code;
    int local_sklad;
    double coef;
    int i, new_rec, parcel;
    int retail_n = do_sklad_param_int(alias, sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PRICE_RETAIL));

    sklad_code = do_text(alias, sklad->data.code);
    local_sklad = !strcmp(sklad_code, domino_unit());
    do_free(sklad_code);

    do_text_set(alias, stock_key0.code, base_code);
    memcpy(&stock_key0.sklad, &sklad->data.code, sizeof(stock_key0.sklad));
    memcpy(&stock_key, &stock_key0, sizeof(stock_key));

    memcpy(&product_key0.code, &stock_key0.code, sizeof(stock_key0.code));
    product_key0.base_parcel = 0;
    status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
    if (status == DO_OK) {
        coef = do_product_coef(alias, &product);
        if (local_sklad)
            rec.price = coef*do_product_price(alias, &product, retail_n);
    }
    if (status == DO_OK) {
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if (status == DO_OK && !local_sklad)
            rec.price = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
    }
    if (status == DO_OK)
        base_rec = do_list_add_alloc(list, &rec, sizeof(rec));
    if (status != DO_ERROR)
        status = DO_OK;
    while (status == DO_OK) {
        if (memcmp(&stock_key0.sklad, &stock_key.sklad, sizeof(stock_key.sklad))) break;

        if (memcmp(&stock_key0.code, &stock_key.code, sizeof(stock_key.code)) &&
            !do_product_is_base_owner(stock_key.code, stock_key0.code, sizeof(stock_key.code)))
            break;

        rest = do_stock_quant(alias, &stock, DO_CONST_QUANT_REST) +
                do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        sale = do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        parcel = 0;
        if ((rest > 0.0009 || rest < - 0.0009) &&
            do_product_is_parcel(stock.data.code, sizeof(stock.data.code))) {
            rec.price = 0;
            rec.rest = rest/coef;
            rec.sale = sale/coef;
            if (local_sklad) {
                memcpy(&barcode_key1.code, &stock.data.code, sizeof(stock.data.code));
                status = do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL);
                if (status == DO_OK) {
                    parcel = 1;
                    memcpy(&product_key0.code, &stock.data.code, sizeof(stock.data.code));
                    product_key0.base_parcel = 1;
                    status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                    if (status == DO_OK)
                        rec.price = coef*do_product_price(alias, &product, retail_n);
                }
            }
            else {
                if (do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL) > 0.00000001)
                    rec.price = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
            }
            if (rec.price) {
                new_rec = 1;
                for (i = 0; i < list->count; i++) {
                    rec_p = list->list[i];
                    if (rec_p->price == rec.price) {
                        rec_p->rest += rec.rest;
                        rec_p->sale += rec.sale;
                        new_rec = 0;
                        break;
                    }
                }
                if (new_rec)
                    do_list_add_alloc(list, &rec, sizeof(rec));
            }

        }
        if (status == DO_ERROR)
            break;
        if (!parcel) {
            base_rec->rest += rest/coef;
            base_rec->sale += sale/coef;
        }
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
    }
    return status != DO_ERROR;
}
*/
DO_EXPORT int do_get_rest_and_price(do_alias_t *alias, const char *base_code, sklad_rec_t *sklad, do_list_t *list, int life_time)
{
    stock_key0_t stock_key0, stock_key;
    stock_rec_t  stock;
    barcode_key1_t barcode_key1;
    product_key0_t product_key0;
    product_rec_t product;
    int status = DO_OK;
    double rest = 0, sale = 0;
    do_rest_price_life_t rec = {0, 0, 0, };
    do_rest_price_life_t *rec_p, *base_rec;
    char *sklad_code;
    int local_sklad;
    double coef;
    int i, new_rec, parcel;
    int retail_n = do_sklad_param_int(alias, sklad, do_param(DO_PARAM_SKLAD_PARAMETR_PRICE_RETAIL));

    sklad_code = do_text(alias, sklad->data.code);
    local_sklad = !strcmp(sklad_code, domino_unit());
    do_free(sklad_code);

    do_text_set(alias, stock_key0.code, base_code);
    memcpy(&stock_key0.sklad, &sklad->data.code, sizeof(stock_key0.sklad));
    memcpy(&stock_key, &stock_key0, sizeof(stock_key));

    memcpy(&product_key0.code, &stock_key0.code, sizeof(stock_key0.code));
    product_key0.base_parcel = 0;
    status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
    if (status == DO_OK) {
        coef = do_product_coef(alias, &product);
        if ( local_sklad )
            rec.price = coef*do_product_price(alias, &product, retail_n);
    }
    if (status == DO_OK) {
        if ( local_sklad )
            status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
        else {
            status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
            if ( status == DO_OK )
                rec.price = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
        }
    }
    if (status == DO_OK) {
        //flag = do_product_param_int(alias, &product,do_param(DO_PARAM_PRODUCT_PARAMETR_CLASS)) != 1;
        //if ( flag )
        base_rec = do_list_add_alloc(list, &rec, sizeof(rec));
    }
    while (status == DO_OK) {
        if (memcmp(&stock_key0.sklad, &stock_key.sklad, sizeof(stock_key.sklad))) break;

        if (memcmp(&stock_key0.code, &stock_key.code, sizeof(stock_key.code)) &&
            !do_product_is_base_owner(stock_key.code, stock_key0.code, sizeof(stock_key.code)))
            break;

        rest = do_stock_quant(alias, &stock, DO_CONST_QUANT_REST) +
                do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        sale = do_stock_quant(alias, &stock, DO_CONST_QUANT_CRNTSALE);
        parcel = 0;
        if (do_stock_cmp(rest,0) &&
            do_product_is_parcel(stock.data.code, sizeof(stock.data.code))) {
            rec.price = 0;
            rec.rest = rest/coef;
            rec.sale = sale/coef;
            rec.life_time.tm_mday = 0;
            if (local_sklad) {
                if ( life_time ) {
                    struct tm tm;
                    do_cpy(product_key0.code, stock.data.code);
                    product_key0.base_parcel = 1;
                    do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                    if (do_product_life_time(alias, &product, &tm))
                        rec.life_time = tm;
                }
                else
                    rec.life_time.tm_year = 0;

                memcpy(&barcode_key1.code, &stock.data.code, sizeof(stock.data.code));
                status = do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL);
                if ( status == DO_OK ) {
                    parcel = 1;
                    if ( !life_time ) {
                        do_cpy(product_key0.code, stock.data.code);
                        product_key0.base_parcel = 1;
                        do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                    }
                    rec.price = coef*do_product_price(alias, &product, retail_n);
                }
            }
            else {
                if (do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL) > 0.00000001) {
                    rec.price = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
                    parcel = (rec.price > 0.0009);
                }
            }
            if (rec.price) {
                new_rec = 1;
                for (i = 0; i < list->count; i++) {
                    rec_p = list->list[i];
                    if (rec_p->price == rec.price) {
                        if (base_rec != rec_p || parcel) {
                            rec_p->rest += rec.rest;
                            rec_p->sale += rec.sale;
                        }
                        rec_p->life_time = rec.life_time;
                        new_rec = 0;
                        break;
                    }
                }
                if (new_rec)
                    do_list_add_alloc(list, &rec, sizeof(rec));
            }
            else
                if (rec.life_time.tm_mday && list->count) {
                    rec_p = list->list[0];
                    rec_p->life_time = rec.life_time;
                }
        }
        if (status == DO_ERROR)
            break;
        if ( !parcel ) {
            base_rec->rest += rest/coef;
            base_rec->sale += sale/coef;
        }
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
    }
    return status != DO_ERROR;
}



DO_EXPORT double do_get_retail_price_full(do_alias_t *alias, const char *code, const char *sklad)
{
    double retval = 0;
    int status;

    if (!strcmp(sklad, domino_unit())) {
        product_key4_t product_key4;
        product_rec_t product;
        do_text_set(alias, product_key4.code, code);
        status = do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL);
        if (status == DO_OK) {
            sklad_key0_t sklad_key0;
            sklad_rec_t sklad_r;
            do_text_set(alias, sklad_key0.code, sklad);
            status = do_sklad_get0(alias, &sklad_r, &sklad_key0, DO_GET_EQUAL);
            if (status == DO_OK)
               retval = do_product_price(alias, &product,
                                          do_sklad_param_int(alias, &sklad_r,
                                                             do_param(DO_PARAM_SKLAD_PARAMETR_PRICE_RETAIL))) *
                         do_product_coef(alias, &product);
        }
    }
    else {
        stock_key0_t stock_key0;
        stock_rec_t  stock;
        do_text_set(alias, stock_key0.code, code);
        do_text_set(alias, stock_key0.sklad, sklad);
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if (status == DO_OK)
            retval = do_stock_quant(alias, &stock, DO_CONST_QUANT_PRICE_FULL);
    }
    return retval;
}
DO_EXPORT char *do_get_partner_name(do_alias_t *alias, const char *g_code, int code)
{
    partner_key0_t partner_key0;
    partner_rec_t partner;
    int status;
    do_text_set(alias, partner_key0.g_code, g_code);
    partner_key0.code = code;

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);

    if (status == DO_OK)
        return do_text(alias, partner.data.name);
    else {
        char  *retval = do_malloc(strlen(g_code) + 10);
        sprintf(retval, "%s.%d", g_code, code);
        return retval;
    }
}
DO_EXPORT char *do_get_partner_name_no_conv(do_alias_t *alias, const char *g_code, int code)
{
    partner_key0_t partner_key0;
    partner_rec_t partner;
    int status;
    memcpy(&partner_key0.g_code, g_code, sizeof(partner_key0.g_code));
    partner_key0.code = code;

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);

    if (status == DO_OK)
        return do_text(alias, partner.data.name);
    else {
        char  *retval = do_malloc(strlen(g_code) + 10);
        char  *region = do_text(alias, partner_key0.g_code);
        sprintf(retval, "%s.%d", region, code);
        do_free(region);
        return retval;
    }
}
DO_EXPORT void  do_alias_cache_init(do_alias_t *alias)
{
    if (alias->cache)
        cache_free(alias->cache);
    alias->cache = cache_new();
}

static void cache_free(do_alias_cache_t *cache)
{
    int i;
    for (i = 0; i < CACHE_NO; i++)
        if (cache->list[i])
            do_sort_list_free(cache->list[i]);
}
static int account_cmp(account_rec_t *s1, account_rec_t *s2, void *user_data)
{
    int retval = memcmp(&s1->data.account, &s2->data.account, sizeof(s1->data.account));
    if (!retval)
        retval = s1->data.itog - s2->data.itog;
    return retval;
}
static int subaccount_cmp(subaccount_rec_t *s1, subaccount_rec_t *s2, void *user_data)
{
    int retval = memcmp(&s1->data.account, &s2->data.account, sizeof(s1->data.account));
    if (!retval)
        retval = memcmp(&s1->data.subaccount, &s2->data.subaccount, sizeof(s1->data.subaccount));
    if (!retval)
        retval = s1->data.itog - s2->data.itog;
    return retval;
}

static do_alias_cache_t *cache_new()
{
    do_alias_cache_t *cache = (do_alias_cache_t*) do_malloc(sizeof(do_alias_cache_t));
    int i;

    for (i = 0; i < CACHE_NO; i++)
        switch (i) {
            case CACHE_ACCOUNT:
                cache->list[i] = do_sort_list_new(1, 1, (do_list_cmp_func)account_cmp, NULL);
                break;
            case CACHE_SUBACCOUNT:
                cache->list[i] = do_sort_list_new(1, 1, (do_list_cmp_func)subaccount_cmp, NULL);
                break;
        };
    return cache;
}
DO_EXPORT int  do_alias_cache_account(do_alias_t *alias, account_rec_t *account)
{
    int indx;
    if (alias->cache && do_sort_list_find(alias->cache->list[CACHE_ACCOUNT], &indx, account)) {
        account->size = ((account_rec_t*)alias->cache->list[CACHE_ACCOUNT]->list[indx])->size;
        memcpy(&account->data, &((account_rec_t*)alias->cache->list[CACHE_ACCOUNT]->list[indx])->data,
                account->size);
        return DO_OK;
    }
    account_key0_t account_key0;
    memcpy(&account_key0.account, &account->data.account, sizeof(account_key0.account));
    account_key0.itog = account->data.itog;
    int status = do_account_get0(alias, account, &account_key0, DO_GET_EQUAL);
    if (status == DO_OK && alias->cache)
        do_sort_list_add_alloc(alias->cache->list[CACHE_ACCOUNT], account,
                             sizeof(*account) - sizeof(account->data) + account->size);
    return status;

}
DO_EXPORT int  do_alias_cache_subaccount(do_alias_t *alias, subaccount_rec_t *subaccount)
{
    int indx;
    if (alias->cache && do_sort_list_find(alias->cache->list[CACHE_SUBACCOUNT], &indx, subaccount)) {
        subaccount->size = ((subaccount_rec_t*)alias->cache->list[CACHE_SUBACCOUNT]->list[indx])->size;
        memcpy(&subaccount->data, &((subaccount_rec_t*)alias->cache->list[CACHE_SUBACCOUNT]->list[indx])->data,
                subaccount->size);
        return DO_OK;
    }
    subaccount_key0_t subaccount_key0;
    memcpy(&subaccount_key0.account, &subaccount->data.account, sizeof(subaccount_key0.account));
    memcpy(&subaccount_key0.subaccount, &subaccount->data.subaccount, sizeof(subaccount_key0.subaccount));
    subaccount_key0.itog = subaccount->data.itog;
    int status = do_subaccount_get0(alias, subaccount, &subaccount_key0, DO_GET_EQUAL);
    if (status == DO_OK && alias->cache)
        do_sort_list_add_alloc(alias->cache->list[CACHE_SUBACCOUNT], subaccount,
                             sizeof(*subaccount) - sizeof(subaccount->data) + subaccount->size);

    return status;

}
DO_EXPORT int do_text_is_empty_(char* src, size_t size)
{
    char* cp = src + size - 1;
    while ((cp >= src) && (*cp == 32)) --cp;
    return (cp < src);
}
DO_EXPORT void  *do_text_set_empty_(void* src, size_t size)
{
    return memset(src, 32, size);
}
# define ISLEAP(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
static const unsigned short int __mon_yday[2][13] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

DO_EXPORT time_t do_date(BTI_LONG date, struct tm *tm)
{
    int days = date - 4; //

    memset(tm, 0, sizeof(*tm));

    tm->tm_isdst = 0; // no daylight
    // January 1, 1801 was a Thursday.
    tm->tm_wday = (4 + days) % 7; // :) 18010101 = 4
    if (tm->tm_wday < 0)
        tm->tm_wday += 7;
    int y = 1801;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

    while (days < 0 || days >= (ISLEAP(y) ? 366 : 365)) {
      // Guess a corrected year, assuming 365 days per year.
      long int yg = y + days / 365 - (days % 365 < 0);
      /* Adjust DAYS and Y to match the guessed year.  */
      days -= ((yg - y) * 365
	       + LEAPS_THRU_END_OF (yg - 1)
	       - LEAPS_THRU_END_OF (y - 1));
      y = yg;
    }
    tm->tm_year = y - 1900;
    tm->tm_yday = days;
    const unsigned short int *ip;
    ip = __mon_yday[ISLEAP(y)];
    for (y = 11; days < (long int) ip[y]; --y);
    days -= ip[y];
    tm->tm_mon = y;
    tm->tm_mday = days + 1;
    return mktime(tm);
}
#define	SECS_PER_HOUR (60 * 60)
DO_EXPORT time_t do_time(BTI_LONG time, struct tm *tm)
{
    long rem = time / 100;
    tm->tm_hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    tm->tm_min = rem / 60;
    tm->tm_sec = rem % 60;
    return mktime(tm);
}
DO_EXPORT int do_date_set(BTI_LONG *date, const struct tm tm)
{
    int y = 1801;
    *date = tm.tm_yday + 4;
    for (;y < tm.tm_year + 1900; y++) {
        *date += ISLEAP(y) ? 366 : 365;
    }
    return *date;
}

DO_EXPORT int do_time_set(BTI_LONG *time, const struct tm tm)
{
    *time = (tm.tm_hour*SECS_PER_HOUR + tm.tm_min*60 + tm.tm_sec)*100;
    return *time;
}


DO_EXPORT double do_atof(const char *str)
{
    char decimal_point = localeconv()->decimal_point[0];
    if (decimal_point !=  '.')
    {
        char* str1 = do_strdup(str);
        char* cp = str1;
        for (; *cp; cp++)
            if (*cp == '.')
            {
                *cp = decimal_point;
                break;
            };
        double result = atof(str1);
        do_free(str1);
        return result;
    }
    else
        return atof(str);
}

DO_EXPORT char *do_ftoa(const double value, char *buf)
{
    sprintf(buf, "%f", value);
    char decimal_point = localeconv()->decimal_point[0];
    if (decimal_point !=  '.')
    {
        char* cp = buf;
        for (; *cp; cp++)
            if (*cp == decimal_point)
            {
                *cp = '.';
                break;
            };
    }
    return buf;
}

DO_EXPORT double do_pdecimal(unsigned char *value, size_t size, int fracplaces)
{
    double result;
    int i;
    result = 0;
    double dr = 1;
    for (i = 1; i < size*2; i++) {
        if (i < size*2 - fracplaces)
            result *=10;
        else
            dr *=10;
        if (i & 1)
            result += (value[i / 2] & 0x0F) / dr;
        else
            result += ((value[i / 2] & 0xF0) >> 4) / dr;
    }
    return (value[0] & 0xF0) ? -result : result;
}
DO_EXPORT int do_pdecimal_set(unsigned char *dst, size_t size, int fracplaces, double value)
{
    *dst = (value >= 0) ? 0 : 0xF0;
    memset(dst + 1, 0, size - 1);
    int i;
    double intvalue = (value >= 0) ? value : -value;
    long number;
    long ivalue;

    unsigned char *cr = dst + size - 1;
    for (i = 0; i < fracplaces; i++,intvalue*=10);
    ivalue = intvalue;
    if (intvalue - ivalue > 0.5)
        ivalue++;
    else
    if (intvalue - ivalue < -0.5)
        ivalue--;
    for (i = 0; i < size*2; i++, ivalue /= 10) {
        number = (long)ivalue % 10;
        if (i % 2) number <<= 4;
        *cr |= number;
        if (i % 2) cr--;
    }
    return ivalue < 1;
}

# define ISLEAP(year)	((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#define	SECS_PER_HOUR (60 * 60)
static void day_of_the_week (struct tm *tm)
{
  int corr_year = 1900 + tm->tm_year - (tm->tm_mon < 2);
  int wday = (-473
	      + (365 * (tm->tm_year - 70))
	      + (corr_year / 4)
	      - ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0)
	      + (((corr_year / 4) / 25) / 4)
	      + __mon_yday[0][tm->tm_mon]
	      + tm->tm_mday - 1);
  tm->tm_wday = ((wday % 7) + 7) % 7;
}

static void day_of_the_year (struct tm *tm)
{
  tm->tm_yday = (__mon_yday[ISLEAP(1900 + tm->tm_year)][tm->tm_mon]
		 + (tm->tm_mday - 1));
}

DO_EXPORT int do_atodate(const char *str, struct tm *tm)
{
    tm->tm_isdst = 0; // no daylight
    memset(tm, 0, sizeof(*tm));

    char *cp = (char*)str;
    tm->tm_mday = atoi(cp) ;
    while (*cp && *cp != '/' && *cp != '.')
      ++cp;
    if (*cp) ++cp;
    tm->tm_mon = atoi(cp) - 1;
    while (*cp && *cp != '/' && *cp != '.')
      ++cp;
    if (*cp) ++cp;
    tm->tm_year = atoi(cp);
    if (tm->tm_year > 1000)
        tm->tm_year -= 1900;
    else
        tm->tm_year += 100;

    day_of_the_week(tm);
    day_of_the_year(tm);
    return (tm->tm_year > -70 && tm->tm_year < 10000) && (tm->tm_mon >=0 && tm->tm_mon <12) && (tm->tm_mday > 0 && tm->tm_mday < 32);
}
DO_EXPORT time_t do_date_set_ymd(struct tm *tm, int year, int month, int day)
{
    memset(tm, 0, sizeof(*tm));
    tm->tm_mday = day;
    tm->tm_mon = month - 1;
    tm->tm_year = year - 1900;
    day_of_the_week(tm);
    day_of_the_year(tm);
    return mktime(tm);
}

DO_EXPORT int do_atotime(const char *str, struct tm *tm)
{
    char *cp = (char*)str;
    tm->tm_hour = atoi(cp);
    while (*cp && *cp != ':')
      ++cp;
    if (*cp) ++cp;
    tm->tm_min = atoi(cp);
    while (*cp && *cp != ':')
      ++cp;
    if (*cp) ++cp;
    tm->tm_sec = atoi(cp);
    return (tm->tm_hour <=23) && (tm->tm_min <=59) && (tm->tm_sec <=59);
}

DO_EXPORT int do_datetosql(const struct tm tm)
{
    return ((tm.tm_year + 1900) << 16) + ((tm.tm_mon + 1) << 8) + tm.tm_mday;
}

DO_EXPORT int do_timetosql(const struct tm tm)
{
    return (tm.tm_hour << 24) + (tm.tm_min << 16) + (tm.tm_sec << 8);
}
static char date_str[11];
DO_EXPORT char* do_datetoa(const struct tm tm)
{
    sprintf(date_str, "%2.2d/%2.2d/%4.4d", tm.tm_mday , tm.tm_mon + 1, tm.tm_year + 1900);
    return strdup(date_str);
}
DO_EXPORT char* do_datetoa_c(const struct tm *tm)
{
    sprintf(date_str, "%2.2d/%2.2d/%4.4d", tm->tm_mday , tm->tm_mon + 1, tm->tm_year + 1900);
    return date_str;
}
DO_EXPORT char* do_timetoa(const struct tm tm)
{
    char *res = (char *) do_malloc(9);
    sprintf(res, "%2.2d:%2.2d:%2.2d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return res;
}

DO_EXPORT time_t do_sqltodate(int date, struct tm *tm)
{
    memset(tm, 0, sizeof(*tm));
    tm->tm_isdst = 0; // no daylight
    tm->tm_mday =  (date & 0xFF);
    tm->tm_mon = ((date & 0xFF00) >> 8) - 1;
    tm->tm_year = ((date & 0xFFFF0000) >> 16) - 1900;
    day_of_the_week(tm);
    day_of_the_year(tm);
    return mktime(tm);
}


DO_EXPORT int  do_recover_print(FILE *file, void *data, int size)
{
    char buf[100];
    sprintf(buf, "%d,", size);
    if (fwrite(buf, strlen(buf), 1, file) != 1 ) {
        do_log(LOG_ERR, "Error writing file : %s", strerror(errno));
        return FALSE;
    }
    if (fwrite(data, size, 1, file) != 1 ) {
        do_log(LOG_ERR, "Error writing file : %s", strerror(errno));
        return FALSE;
    }
    sprintf(buf, "\r\n");
    if (fwrite(buf, strlen(buf), 1, file) != 1 ) {
        do_log(LOG_ERR, "Error writing file : %s", strerror(errno));
        return FALSE;
    }
    return TRUE;
}
DO_EXPORT char *do_ean13(const char *code)
{
    int i;
    int sum = 0;
    int digit;
    char *retval = (char*)do_malloc(14);
    for (i=0; i < 12; i++) {
        if (strlen(code) > i && code[i] >= '0' && code[i] <= '9')
            retval[i] = code[i];
        else
            retval[i] = '0';
        digit = retval[i] - '0';
    	if (i & 0x01)
            sum += digit*3;
        else
            sum += digit;
    }
    retval[12] = '0' + ((sum % 10) ? (10 - sum % 10) : 0);
    retval[13] = '\0';
    return retval;

};

DO_EXPORT char *do_partner_data_param(do_alias_t *alias, partner_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *res = do_fromparam(params, name, alias->utf8);
    do_free(params);
    return res;
}
DO_EXPORT int do_partner_data_param_int(do_alias_t *alias, partner_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    int  res = atoi(val);
    do_free(val);
    return res;
}
DO_EXPORT double do_partner_data_param_float(do_alias_t *alias, partner_data_rec_t *rec, const char *name)
{
    char *params = do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));
    char *val = do_fromparam(params, name, alias->utf8);
    do_free(params);
    double res = do_atof(val);
    do_free(val);
    return res;
}
DO_EXPORT void do_partner_data_param_set(do_alias_t *alias, partner_data_rec_t *rec, const char *name, const char *value)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , value,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_partner_data_param_int_set(do_alias_t *alias, partner_data_rec_t *rec, const char *name, int value)
{
    char val[20];
    sprintf(val, "%d", value);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_partner_data_param_float_set(do_alias_t *alias, partner_data_rec_t *rec, const char *name, double value)
{
    char val[100];  do_ftoa(value, val);
    rec->size = sizeof(rec->data) - sizeof(rec->data.params) +
                tooem32param(alias, rec->data.params, name , val,
                              rec->size - (sizeof(rec->data) - sizeof(rec->data.params))
                                 , sizeof(rec->data.params));
}
DO_EXPORT void do_partner_data_params_clear(do_alias_t *alias, partner_data_rec_t *rec)
{
    rec->size = sizeof(rec->data) - sizeof(rec->data.params);
}
DO_EXPORT int do_date_cmp(struct tm *tm1, struct tm *tm2)
{
    int i;
    i = tm1->tm_year - tm2->tm_year;
    if ( i )
        return i;
    i = tm1->tm_mon - tm2->tm_mon;
    if ( i )
        return i;
    i = tm1->tm_mday - tm2->tm_mday;
    return i;
}
DO_EXPORT int do_time_cmp(struct tm *tm1, struct tm *tm2)
{
    int i;
    i = tm1->tm_hour - tm2->tm_hour;
    if ( i )
        return i;
    i = tm1->tm_min - tm2->tm_min;
    if ( i )
        return i;
    i = tm1->tm_sec - tm2->tm_sec;
    return i;
}
DO_EXPORT int do_datetime_cmp(struct tm *tm1, struct tm *tm2)
{
    time_t t1,t2;
    t1 = mktime(tm1);
    t2 = mktime(tm2);
    return t1 - t2;
}
DO_EXPORT double do_prowod2_sum(do_alias_t *alias, prowod2_rec_t *prowod2, const int number)
{
    int i;
    for (i = 0; i < 5; i++)
        if (prowod2->data.nsum[i] == number)
            return do_pdecimal(prowod2->data.sums[i], 8, 2);
    return 0;
}
DO_EXPORT void   do_prowod2_sum_clear(do_alias_t *alias, prowod2_rec_t *rec)
{
    memset(rec->data.nsum, 0, sizeof(rec->data.sums) + sizeof(rec->data.nsum));
}

DO_EXPORT int do_prowod2_sum_set(do_alias_t *alias, prowod2_rec_t *prowod2, const int number, double value)
{
    int i;
    for (i = 0; i < 5; i++) {
        if (prowod2->data.nsum[i] == number) {
            do_pdecimal_set(prowod2->data.sums[i], 8, 2, value);
            return 1;
        }
        if (!prowod2->data.nsum[i]) {
            prowod2->data.nsum[i] = number;
            do_pdecimal_set(prowod2->data.sums[i], 8, 2, value);
            return 1;
        }
    }
    do_log(LOG_ERR, "prowod2 sum is full", do_text(alias, prowod2->data.dtype),
                do_text(alias, prowod2->data.document),
                do_text(alias, prowod2->data.sklad),
                prowod2->data.number);
    return 0;
}
