
#include <dolib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include "../../misc/define.h"

struct _do_data_t {
    void  *data;
    size_t size;
    int    type;
    int    send_flags;
};

DO_EXPORT int do_data_type(do_data_t *data)
{
    return data->type;
}
DO_EXPORT int do_data_send_flags(do_data_t *data)
{
    return data->send_flags;
}

DO_EXPORT int do_data_set_type(do_data_t *data, int type)
{
    return data->type = type;
}
DO_EXPORT void* do_data_get(do_data_t *data)
{
    return data->data;
}
DO_EXPORT size_t do_data_size(do_data_t *data)
{
    return data->size;
}


DO_EXPORT do_data_t *do_data_new()
{
    do_data_t *data = (do_data_t*) do_malloc(sizeof(do_data_t));
    data->data = NULL;
    data->size = 0;
    data->send_flags = 0;
    data->type = DO_EXCH_DATA_TYPE_RAW;
    return data;
}
DO_EXPORT do_data_t *do_data_new_from_src(void *src, size_t size)
{
    do_data_t *data = (do_data_t*) do_malloc(sizeof(do_data_t));
    data->data = NULL;
    data->size = 0;
    data->send_flags = 0;
    data->type = DO_EXCH_DATA_TYPE_RAW;
    if (size && !do_data_alloc(data, size)) {
        do_data_free(data);
        return NULL;
    }
    if (src && size)
        memcpy(data->data, src, size);
    return data;
}

DO_EXPORT void do_data_free(do_data_t *data)
{
    if (data) {
        if (data->data)
            do_free(data->data);
        do_free(data);
    }
}

DO_EXPORT void do_data_clear(do_data_t *data)
{
    do_free(data->data);
    data->size = 0;
    data->data = NULL;
}

DO_EXPORT int do_data_alloc(do_data_t *data, size_t size)
{
    if (data->data)
        do_free(data->data);
    if ((data->data = do_malloc(size)) == NULL && size) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", size);
        data->size = 0;
        return 0;
    }
    else
        data->size = size;
    return 1;
}
DO_EXPORT int do_data_realloc(do_data_t *data, size_t size)
{
    if ((data->data = do_realloc(data->data, size)) == NULL
          && size) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", size);
        data->size = 0;
        return 0;
    }
    else
        data->size = size;
    return 1;
}

DO_EXPORT int do_data_add_str(do_data_t *data, const char *str)
{
    if ((data->data = do_realloc(data->data, data->size + strlen(str))) == NULL  && (data->size + strlen(str))) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", data->size + strlen(str));
        data->size = 0;
        return 0;
    }
    memcpy(data->data + data->size, str, strlen(str));
    data->size += strlen(str);
    return 1;
}

DO_EXPORT int do_data_add_strf(do_data_t *data, const char *cformat, ...)
{
    va_list ap;
    int sz = 1024, n = -1;
    char *buf = NULL;
    while (n == -1) {
        buf = (char*) do_realloc(buf, sz + 1);
        if (!buf) return 0;
        va_start(ap, cformat);
        n = vsnprintf(buf, sz , cformat, ap);
        va_end(ap);
        sz *=2;
    }
    if ((data->data = do_realloc(data->data, data->size + strlen(buf))) == NULL  && (data->size + strlen(buf))) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", data->size + strlen(buf));
        data->size = 0;
        return 0;
    }
    memcpy(data->data + data->size, buf, strlen(buf));
    data->size += strlen(buf);
    do_free(buf);
    return 1;
}

DO_EXPORT int do_data_set_err(do_data_t *data, const char *str)
{
    if ((data->data = do_realloc(data->data, strlen(str))) == NULL && strlen(str)) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", strlen(str));
        data->size = 0;
        return 0;
    }
    memcpy(data->data, str, strlen(str));
    data->size = strlen(str);
    data->type = DO_EXCH_DATA_TYPE_ERROR;
    return 1;
}

DO_EXPORT int do_data_set_errf(do_data_t *data, const char *cformat, ...)
{
    va_list ap;
    int sz = 1024, n = -1;
    char *buf = NULL;
    while (n == -1) {
        buf = (char*) do_realloc(buf, sz + 1);
        if (!buf) return 0;
        va_start(ap, cformat);
        n = vsnprintf(buf, sz , cformat, ap);
        va_end(ap);
        sz *=2;
    }

    if ((data->data = do_realloc(data->data, strlen(buf))) == NULL && strlen(buf)) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", strlen(buf));
        data->size = 0;
        return 0;
    }
    memcpy(data->data, buf, strlen(buf));
    data->size = strlen(buf);
    data->type = DO_EXCH_DATA_TYPE_ERROR;
    do_free(buf);
    return 1;
}
DO_EXPORT int do_data_copy(do_data_t *dst, do_data_t *src)
{
    if ((dst->data = do_realloc(dst->data, src->size)) == NULL && src->size) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", src->size);
        dst->size = 0;
        return 0;
    }
    memcpy(dst->data, src->data, src->size);
    dst->size = src->size;
    dst->type = src->type;
    dst->send_flags = src->send_flags;
    return 1;
}
DO_EXPORT int do_data_set(do_data_t *data, void *src, size_t size)
{
    if (data->size)
        do_free(data->data);
    if (size) {
        if ((data->data = do_malloc(size)) == NULL) {
            do_log(LOG_ERR, "Could not allocate memory %d byte", size);
            data->size = 0;
            return 0;
        }
        memcpy(data->data, src, size);
    }
    data->size = size;
    return 1;
}
DO_EXPORT int do_data_set_data(do_data_t *data, void *src, size_t size)
{
    if (data->size)
        do_free(data->data);
    data->data = src;
    data->size = size;
    return 1;
}
DO_EXPORT void *do_data_add(do_data_t *data, void *src, size_t size)
{
    if ((data->data = do_realloc(data->data, data->size + size)) == NULL && size) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", size);
        data->size = 0;
        return NULL;
    }
    memcpy(data->data + data->size, src, size);
    data->size += size;
    return data->data + data->size - size;
}
DO_EXPORT int do_data_add_do_data(do_data_t *data, do_data_t *src)
{
    return do_data_add(data, src->data, src->size) != NULL;
}

DO_EXPORT void do_data_set_send_flags(do_data_t *data, int flags)
{
   data->send_flags = flags;
}


DO_EXPORT int do_data_set_str(do_data_t *data, const char *str)
{
    if ((data->data = do_realloc(data->data, strlen(str))) == NULL  && strlen(str)) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", strlen(str));
        data->size = 0;
        return 0;
    }
    memcpy(data->data, str, strlen(str));
    data->size = strlen(str);
    return 1;
}

DO_EXPORT int do_data_set_strf(do_data_t *data, const char *cformat, ...)
{
    va_list ap;
    int sz = 1024, n = -1;
    char *buf = NULL;
    while (n == -1) {
        buf = (char*) do_realloc(buf, sz + 1);
        if (!buf) return 0;
        va_start(ap, cformat);
        n = vsnprintf(buf, sz , cformat, ap);
        va_end(ap);
        sz *=2;
    }

    if ((data->data = do_realloc(data->data, strlen(buf))) == NULL  && strlen(buf)) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", strlen(buf));
        data->size = 0;
        return 0;
    }
    memcpy(data->data, buf, strlen(buf));
    data->size = strlen(buf);
    do_free(buf);
    return 1;
}
DO_EXPORT char *do_data_error(do_data_t *data)
{
    char *result;
    if (data->type == DO_EXCH_DATA_TYPE_ERROR) {
        result = (char*)do_malloc(data->size + 1);
        strncpy(result, data->data, data->size);
        result[data->size] = '\0';
        return result;
    };
    return do_strdup("");
}
DO_EXPORT int do_data_is_error(do_data_t *data)
{
    return data->type == DO_EXCH_DATA_TYPE_ERROR;
}
DO_EXPORT char *do_data_str(do_data_t *data)
{
    char *result;
    if (data->type != DO_EXCH_DATA_TYPE_ERROR) {
        result = (char*)do_malloc(data->size + 1);
        strncpy(result, data->data, data->size);
        result[data->size] = '\0';
        return result;
    }
    return do_strdup("");
}

/*
int do_data_safe_cp1251(do_data_t *data, const char *fname)
{
    FILE *fp;
    if ((fp = fopen(fname, "wb")) == NULL)  {
        do_log(LOG_ERR, "Error opening for write file (%s): %s", fname, strerror(errno));
        return 0;
    }
    size_t n;
    char *out = data->data;
    size_t size = data->size;
#ifdef _LINUX
    out = (char*)tocp1251(data->data, data->size);
    size = strlen(out);
#endif
    if ((n = fwrite(out, 1, size, fp)) != size ) {
        do_log(LOG_ERR, "Error writing file (%s): %s", fname, strerror(errno));
        fclose(fp);
        return 0;
    }
#ifdef _LINUX
    do_free(out);
#endif
    fclose(fp);
    return 1;
}
*/
DO_EXPORT int do_data_read_from_file(do_data_t *data, const char* filename)
{
    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL)  {
        do_log(LOG_ERR, "Error opening file (%s): %s", filename, strerror(errno));
        return 0;
    }
    fseek (fp, 0L, SEEK_END);
    long len = ftell (fp), l;
    if ((data->data = do_realloc(data->data, len)) == NULL  && len) {
        do_log(LOG_ERR, "Could not allocate memory %d byte", len);
        data->size = 0;
        return 0;
    }
    l = fseek (fp, 0L, SEEK_SET);
    data->size = len;
    if (((l = fread(data->data, 1, len, fp)) != len ) && (errno)) {
        do_log(LOG_ERR, "Error read file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}
DO_EXPORT int do_data_write_to_file(do_data_t *data, const char* filename)
{
    FILE *fp;
    if ((fp = fopen(filename, "wb")) == NULL)  {
        do_log(LOG_ERR, "Error opening for write file (%s): %s", filename, strerror(errno));
        return 0;
    }
    size_t n;
    char *out = data->data;
    size_t size = data->size;
    if ((n = fwrite(out, 1, size, fp)) != size ) {
        do_log(LOG_ERR, "Error writing file (%s): %s", filename, strerror(errno));
        fclose(fp);
        return 0;
    }
    fclose(fp);
    return 1;
}
DO_EXPORT int do_data_list_count(do_data_t *data)
{
    do_size_t  *sz;
    int retval;
    void *crnt;
    crnt = data->data;
    if (data->type != DO_EXCH_DATA_TYPE_RAW)
	return 0;
    sz = crnt;
    retval = *sz;
    return retval;
}
DO_EXPORT int do_data_list_item_size(do_data_t *data, int indx)
{
    do_size_t  *sz;
    int retval,count,i;
    void *crnt;
    crnt = data->data;
    sz = crnt;
    crnt += sizeof(*sz);
    count = *sz;
    for (i = 0; i < count; i++) {
        sz = crnt;
        crnt += sizeof(*sz);
        crnt += *sz;
        if ( i == indx ) {
    	    retval = *sz;
    	    return retval;
    	}
    }
    return 0;
}
DO_EXPORT void *do_data_list_item(do_data_t *data, int indx)
{
    do_size_t  *sz;
    int count,i;
    void *crnt;
    crnt = data->data;
    sz = crnt;
    crnt += sizeof(*sz);
    count = *sz;
    for (i = 0; i < count; i++) {
        sz = crnt;
        crnt += sizeof(*sz);
        if ( i == indx ) {
    	    //retval = *sz;
    	    return crnt;
    	}
        crnt += *sz;
    }
    return NULL;
}
