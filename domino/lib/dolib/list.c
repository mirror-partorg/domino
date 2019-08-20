#include <dolib.h>
#include "../../misc/define.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    int size;
}capacity_t;

typedef struct {
    int freenode;
}do_list_priv_t;

typedef struct  {
    int freenode;
    int uniq;
    void *user_data;
    do_list_cmp_func proccmp;
    do_list_cmp_func proccmpn;
}do_sort_list_priv_t;

#define get_capacity(a) ((capacity_t*)(((do_list_t*)a) + 1))

#define get_do_list_priv(a) ((do_list_priv_t*)(get_capacity(a) + 1))
#define get_do_sort_list_priv(a) ((do_sort_list_priv_t*)(get_capacity(a) + 1))

static void setCapacity(do_list_t *list, int newCapacity)
{
    capacity_t *capacity;
    list->list=(void **) do_realloc (list->list, newCapacity * sizeof(void*));
    capacity = get_capacity(list);
    capacity->size = newCapacity;
}

static void grow(do_list_t *list)
{
    capacity_t *capacity = get_capacity(list);
    if (capacity->size > 64)
        setCapacity(list, capacity->size + (capacity->size >> 2));
    else
        if (capacity->size > 8) setCapacity(list, capacity->size + 16);
        else setCapacity(list, capacity->size + 4);
}

DO_EXPORT do_list_t *do_list_new(int freenode)
{
    do_list_priv_t *priv;
    do_list_t *result = (do_list_t *) do_malloc0(sizeof(do_list_t) + sizeof(capacity_t) + sizeof(do_list_priv_t));
    priv = get_do_list_priv(result);
    priv->freenode = freenode;
    return result;
}
DO_EXPORT void do_list_clear(do_list_t *list)
{
    do_list_priv_t *priv;
    priv = get_do_list_priv(list);
    if (priv->freenode) {
        int i;
        for (i = 0; i < list->count; i++)
            if ( list->list[i] )
                do_free(list->list[i]);
    }
    setCapacity(list, 0);
    list->count = 0;
}

DO_EXPORT void do_list_free(do_list_t *list)
{
    do_list_clear(list);
    do_free(list);
}

DO_EXPORT void *do_list_add(do_list_t *list, void *node)
{
    if (!list)
        return NULL;
    capacity_t *capacity = get_capacity(list);
    if (list->count == capacity->size)
        grow(list);
    list->list[list->count] = node;
    list->count++;
    return list->list[list->count - 1];
}

DO_EXPORT void do_list_foreach(do_list_t *list, do_list_foreach_func func, void *data)
{
    int i;
    for (i = 0; i < list->count; i++)
        if (!func(list->list[i], data))
            break;
}

DO_EXPORT void *do_list_add_alloc(do_list_t *list, void *node, int size)
{
    capacity_t *capacity = get_capacity(list);
    if (list->count == capacity->size)
        grow(list);
    list->list[list->count] = do_malloc(size);
    memcpy(list->list[list->count], node, size);
    list->count++;
    return list->list[list->count - 1];
}


DO_EXPORT void  *do_list_insert(do_list_t *list, int indx, void *node)
{
    capacity_t *capacity = get_capacity(list);
    if ( list->count <= indx ) {
        if ( list->count == indx ) {
            return do_list_add(list, node);
        }
        capacity->size = indx + 1;
        setCapacity(list, capacity->size);
        memset(&list->list[list->count], 0, (indx - list->count) * sizeof(void*));
        list->count = indx + 1;
    }
    else
    {
        if (list->count == capacity->size)
            grow(list);
        memmove(&list->list[indx + 1], &list->list[indx], (list->count - indx) * sizeof(void*));
        list->count++;
    }
    list->list[indx] = node;
    return list->list[indx];
}

DO_EXPORT do_sort_list_t *do_sort_list_new(int freenode, int uniq, do_list_cmp_func cmp_func, void *user_data)
{
    do_sort_list_priv_t *priv;
    //capacity_t *capacity;
    do_sort_list_t *result = (do_sort_list_t *)
               do_malloc0(sizeof(do_sort_list_t) + sizeof(capacity_t) + sizeof(do_sort_list_priv_t));
    //capacity = get_capacity(result);
    priv = get_do_sort_list_priv(result);
    priv->freenode = freenode;
    priv->uniq = uniq;
    priv->proccmp = cmp_func;
    priv->user_data = user_data;
    return result;
}
DO_EXPORT void do_sort_list_clear(do_sort_list_t *list)
{
    do_sort_list_priv_t *priv = get_do_sort_list_priv(list);
    if (priv->freenode) {
        int i;
        for (i = 0; i < list->count; i++)
            if ( list->list[i] )
                do_free(list->list[i]);
    }
    setCapacity(list, 0);
    list->count = 0;
}
DO_EXPORT void do_list_delete(do_list_t *list, int indx)
{
    do_list_priv_t *priv = get_do_list_priv(list);
    if ( priv->freenode && list->list[indx] )
        do_free(list->list[indx]);
    memmove(&list->list[indx], &list->list[indx + 1], (list->count - indx - 1) * sizeof(void*));
    list->count--;
}

DO_EXPORT void do_sort_list_free(do_sort_list_t *list)
{
    do_sort_list_clear(list);
    do_free(list);
}

DO_EXPORT int do_sort_list_add(do_sort_list_t *list, void *node)
{
    capacity_t *capacity = get_capacity(list);
    do_sort_list_priv_t *priv = get_do_sort_list_priv(list);
    if (list->count == capacity->size)
        grow(list);
    if (!priv->proccmp) {
        list->list[list->count] = node;
        list->count++;
        return list->count - 1;
    }
    int indx;
    if (do_sort_list_find(list, &indx, node) && priv->uniq)
        return indx;
    do_sort_list_insert(list, indx, node);
    return indx;
}
DO_EXPORT void do_sort_list_set_cmp_proc(do_sort_list_t *list, do_list_cmp_func proc)
{
    do_sort_list_priv_t *priv = get_do_sort_list_priv(list);
    priv->proccmp = proc;
}
DO_EXPORT void do_sort_list_set_cmpn_proc(do_sort_list_t *list, do_list_cmp_func proc)
{
    do_sort_list_priv_t *priv = get_do_sort_list_priv(list);
    priv->proccmpn = proc;
}
DO_EXPORT int do_sort_list_find(do_sort_list_t *list, int *indx, void *node)
{
    do_sort_list_priv_t *priv = get_do_sort_list_priv(list);
    int l = 0, h = list->count - 1, i, c;
    int result = 0;;
    while (l <= h) {
        i = (l + h) >> 1;
        if (i > list->count - 1)
           i = list->count;
        c = priv->proccmp(list->list[i], node, priv->user_data);
        if (c < 0) l = i + 1;
        else {
            h = i - 1;
            if (!c) {
                while (i > 0 && !priv->proccmp(list->list[i-1], node, priv->user_data)) i--;
                l = i;
                result = 1;
            }
        }
    }
    *indx = l;
    return result;
}
DO_EXPORT int do_sort_list_findn(do_sort_list_t *list, int *indx, void *node)
{
    do_sort_list_priv_t *priv = get_do_sort_list_priv(list);
    if (!priv->proccmpn)
        return do_sort_list_find(list, indx, node);

    int l = 0, h = list->count - 1, i, c;
    int result = 0;;
    while (l <= h) {
        i = (l + h) >> 1;
        c = priv->proccmpn(list->list[i], node, priv->user_data);
        if (c < 0) l = i + 1;
        else {
            h = i - 1;
            if (!c) {
                l = i;
                for (; i; i--)
                    if (!priv->proccmpn(list->list[i], node, priv->user_data))
                        l = i;
                    else
                        break;
                result = 1;
            }
        }
    }
    *indx = l;
    return result;
}
DO_EXPORT int do_sort_list_add_alloc(do_sort_list_t *list, void *node, size_t size)
{

    capacity_t *capacity = get_capacity(list);
    do_sort_list_priv_t *priv = get_do_sort_list_priv(list);
    if (list->count == capacity->size)
        grow(list);
    if (!priv->proccmp) {
        list->list[list->count] = node;
        list->count++;
        return list->count - 1;
    }
    int indx;
    if (do_sort_list_find(list, &indx, node) && priv->uniq)
        return indx;
    void *nn = do_malloc(size);
    memcpy(nn, node, size);
    do_sort_list_insert(list, indx, nn);
    return indx;
}
DO_EXPORT void  *do_sort_list_insert_new(do_sort_list_t *list, int indx, void *node, size_t size)
{
    void *nn = do_malloc(size);
    memcpy(nn, node, size);
    return do_sort_list_insert(list, indx, nn);
}
DO_EXPORT void  *do_sort_list_insert(do_sort_list_t *list, int indx, void *node)
{
    capacity_t *capacity = get_capacity(list);
    if (list->count == capacity->size)
        grow(list);
    memmove(&list->list[indx + 1], &list->list[indx], (list->count - indx) * sizeof(void*));
    list->count++;
    list->list[indx] = node;
    return list->list[indx];
}
DO_EXPORT void do_sort_list_delete(do_sort_list_t *list, int indx)
{
    do_sort_list_priv_t *priv = get_do_sort_list_priv(list);
    if ( priv->freenode && list->list[indx] )
        do_free(list->list[indx]);
    memmove(&list->list[indx], &list->list[indx + 1], (list->count - indx - 1) * sizeof(void*));
    list->count--;
}
DO_EXPORT void   do_sort_list_delete_node(do_sort_list_t *list, void *node)
{
    int indx;
    if (do_sort_list_find(list, &indx, node))
        do_sort_list_delete(list, indx);
}
DO_EXPORT do_string_list_t *do_string_list_new()
{
    return
      (do_string_list_t *) do_malloc0(sizeof(do_string_list_t) + sizeof(capacity_t));
}
DO_EXPORT do_string_list_t *do_string_list_dup(do_string_list_t *list)
{
    do_string_list_t *retval;
    int i;
    capacity_t *capacity;

    retval = do_string_list_new();
    capacity = get_capacity(list);
    setCapacity(retval, capacity->size);
    for (i = 0; i < list->count; i++)
        do_string_list_add_alloc(retval, list->list[i]);

    return retval;
}
DO_EXPORT do_string_list_t *do_string_list_new_from_csv(const char *csv)
{
    do_string_list_t *retval;
    retval = do_string_list_new();

    char *cp = (char*)csv;
    char *value;
    int len;

    while (cp[0] != '\0') {
        while (cp[0] == ' ' || cp[0] == '\t') ++cp;
        if (cp[0] == '"')
        {
            value = ++cp;
            while (cp[0] != '\0' && (cp[0] != '"' || cp[1] == '"'))
                if (cp[0] == '"')
                    cp +=2;
                else
                    cp++;
            len = cp - value;
            while (cp[0] != '\0' && cp[0] != ',') cp++;
        }
        else
        {
            value = cp;
            while (cp[0] != '\0' && cp[0] != ',') cp++;
            len = cp - value;
        }
        if (cp[0] == ',') cp++;
        do_string_list_addn_alloc(retval, value, len);

    }
    return retval;
}
DO_EXPORT void do_string_list_clear(do_string_list_t *list)
{
    int i;
    for (i = 0; i < list->count; i++)
        do_free(list->list[i]);
    setCapacity(list, 0);
    list->count = 0;
}
DO_EXPORT void do_string_list_delete(do_string_list_t *list, int indx)
{
    do_free(list->list[indx]);
    memmove(&list->list[indx], &list->list[indx + 1], (list->count - indx - 1) * sizeof(void*));
    list->count--;
}
DO_EXPORT void do_string_list_free(do_string_list_t *list)
{
    do_string_list_clear(list);
    do_free(list);
}
DO_EXPORT char *do_string_list_add_alloc(do_string_list_t *list, const char* str)
{
    capacity_t *capacity = get_capacity(list);
    if (list->count == capacity->size)
        grow(list);
    list->list[list->count] = do_strdup(str);
    list->count++;
    return list->list[list->count - 1];
}
DO_EXPORT char  *do_string_list_insert(do_string_list_t *list, int indx, char *str)
{
    capacity_t *capacity = get_capacity(list);
    if ( list->count <= indx ) {
        if ( list->count == indx ) {
            return do_string_list_add(list, str);
        }
        capacity->size = indx + 1;
        setCapacity(list, capacity->size);
        memset(&list->list[list->count], 0, (indx - list->count) * sizeof(void*));
        list->count = indx + 1;
    }
    else
    {
        if (list->count == capacity->size)
            grow(list);
        memmove(&list->list[indx + 1], &list->list[indx], (list->count - indx) * sizeof(void*));
        list->count++;
    }
    list->list[indx] = str;
    return list->list[indx];
}

DO_EXPORT char *do_string_list_addn_alloc(do_string_list_t *list, const char* str, int n)
{
    capacity_t *capacity = get_capacity(list);
    if (list->count == capacity->size)
        grow(list);
    list->list[list->count] = (char*)do_malloc(n + 1);
    strncpy(list->list[list->count], str, n);
    ((char*)list->list[list->count])[n] = '\0';
    list->count++;
    return list->list[list->count - 1];
}
DO_EXPORT char *do_string_list_add(do_string_list_t *list, char* str)
{
    capacity_t *capacity = get_capacity(list);
    if (list->count == capacity->size)
        grow(list);
    list->list[list->count] = str;
    list->count++;
    return list->list[list->count - 1];
}
DO_EXPORT char *do_string_list_addf_alloc(do_string_list_t *list, const char* cformat, ...)
{
    va_list ap;
    int sz = 1024, n = -1;
    char *buf = NULL;
    while (n == -1) {
        buf = (char*) do_realloc(buf, sz + 1);
        if (!buf) return NULL;
        va_start(ap, cformat);
        n = vsnprintf(buf, sz , cformat, ap);
        va_end(ap);
        sz *=2;
    }
    char *res = do_string_list_add_alloc(list, buf);
    do_free(buf);
    return res;
}
DO_EXPORT int do_string_list_find(do_string_list_t *list, int* indx, const char *str)
{
    int i;
    *indx = 0;
    for (i = 0; i < list->count; i++)
        if  (!strcmp(list->list[i], str)) {
            *indx = i;
            return 1;
        }
    return 0;
}
DO_EXPORT char *do_string_list_to_string(do_string_list_t *list)
{
    char *retval, *ch;
    size_t len;
    int i;
    len = 0;
    for ( i = 0; i < list->count; i++ )
        len += strlen(list->list[i]);
    len++;
    retval = do_malloc(len);
    ch = retval;
    for ( i = 0; i < list->count; i++ ) {
        strcpy(ch, list->list[i]);
        ch += strlen(list->list[i]);
    }

    return retval;
}

DO_EXPORT char *do_string_list_to_csv(do_string_list_t *list)
{
    char *retval, *ch;
    size_t len;
    int i;
    len = 0;
    for ( i = 0; i < list->count; i++ )
        len += strlen(list->list[i]) + 1;
    if ( !len )
        return do_strdup("");
    len++;
    retval = do_malloc(len);
    ch = retval;
    for ( i = 0; i < list->count; i++ ) {
        strcpy(ch, list->list[i]);
        ch[strlen(list->list[i])] = ',';
        ch += strlen(list->list[i]) + 1;
    }
    ch--;
    *ch = '\0';
    return retval;
}

