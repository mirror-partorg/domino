#include <domino.h>
#include <dolib.h>
#include <stdarg.h>
#include "btr_struct.h"
#include "../../misc/define.h"


#define RET_BUFFER_SIZE 65535
#define POS_UNDEFINE -1


typedef struct PACK_STRUCT
{
    BTI_CHAR    fieldType;
    BTI_SINT    fieldLen;
    BTI_SINT    fieldOffset;
    BTI_CHAR    comparisonCode;
    BTI_CHAR    connector;
} _do_term_t;
int do_alias_btr(do_alias_t *alias, BTI_WORD operation, file_db_t id, BTI_VOID_PTR dataBuffer, BTI_WORD_PTR dataLength, BTI_VOID_PTR keyBuffer, BTI_SINT keyNumber);
struct _do_extended_t {
    do_alias_t *alias;
    struct {
        do_extended_break_func func;
        void  *user_data;
    } break_;
    struct {
        do_extended_reject_func func;
        void  *user_data;
    } reject_;
    file_db_t file_db;

    int filter_count;
    int filter_dont_reject;
    size_t *filter_field_len;
    void *filter_term;

    int key_num;
    void *key_ptr;
    BTI_LONG key_pos;

    int fields_count;
    struct {
        size_t size;
        int offset;
    } *fields;
};

typedef struct PACK_STRUCT {
    BTI_SINT      recLen;
    BTI_LONG      recPos;
    char          data;
} do_extended_ret_t;

struct _do_ret_list_t {
    do_extended_ret_t **list;
    int count;
};

typedef struct {
    int size;
} capacity_t;

typedef struct  {
    do_list_t *memory;
} do_ret_list_priv_t;

static const size_t file_record_size[DO_DB_END] = {
    sizeof(document_struct_t),
    sizeof(document_order_struct_t),
    sizeof(document_link_struct_t),
    sizeof(document_view_struct_t),
    sizeof(document_data_struct_t),
    sizeof(document_prow_struct_t),
    sizeof(document_type_struct_t),
    sizeof(enum_struct_t),
    sizeof(product_struct_t),
    sizeof(product_link_struct_t),
    sizeof(product_view_struct_t),
    sizeof(product_data_struct_t),
    sizeof(barcode_struct_t),
    sizeof(account_struct_t),
    sizeof(subaccount_struct_t),
    sizeof(class_struct_t),
    sizeof(group_struct_t),
    sizeof(subgroup_struct_t),
    sizeof(region_struct_t),
    sizeof(otdel_struct_t),
    sizeof(partner_struct_t),
#ifdef USE_DB_PARNER_VIEW
    sizeof(partner_view_struct_t),
#endif
    sizeof(partner_data_struct_t),
    sizeof(sklad_struct_t),
    sizeof(stock_struct_t),
    sizeof(prowod_struct_t),
    sizeof(prowod2_struct_t),
    sizeof(saldo_struct_t),
    sizeof(protocol_struct_t),
    sizeof(user_struct_t),
    sizeof(shift_struct_t),
    sizeof(checksum_struct_t),
    sizeof(check_struct_t),
    sizeof(realization_struct_t),
    sizeof(discount_struct_t)
};

#define get_capacity(a) ((capacity_t*)(((do_ret_list_t*)a) + 1))
#define get_do_list_priv(a) ((do_ret_list_priv_t*)(get_capacity(a) + 1))

static void setCapacity(do_ret_list_t *list, int newCapacity)
{
    capacity_t *capacity;
    list->list=(do_extended_ret_t **) do_realloc (list->list, newCapacity * sizeof(void*));
    capacity = get_capacity(list);
    capacity->size = newCapacity;
}

static void grow(do_ret_list_t *list)
{
    capacity_t *capacity = get_capacity(list);
    if (capacity->size > 64)
        setCapacity(list, capacity->size + (capacity->size >> 2));
    else
        if (capacity->size > 8) setCapacity(list, capacity->size + 16);
        else setCapacity(list, capacity->size + 4);
}

static do_ret_list_t *do_ret_list_new()
{
    do_ret_list_priv_t *priv;
    do_ret_list_t *result = (do_ret_list_t *) do_malloc0(sizeof(do_ret_list_t) + sizeof(capacity_t) + sizeof(do_ret_list_priv_t));
    priv = get_do_list_priv(result);
    priv->memory = do_list_new(FALSE);
    return result;
}
static void *do_ret_list_memory_add(do_ret_list_t *list, void *ptr)
{
    do_ret_list_priv_t *priv;
    priv = get_do_list_priv(list);
    return do_list_add(priv->memory, ptr);
}
DO_EXPORT void do_ret_list_delete(do_ret_list_t *list, int indx)
{
    memmove(&list->list[indx], &list->list[indx + 1], (list->count - indx - 1) * sizeof(void*));
    list->count--;
}
static void *do_ret_list_add(do_ret_list_t *list, void *node)
{

    capacity_t *capacity = get_capacity(list);
    if (list->count == capacity->size)
        grow(list);
    list->list[list->count] = node;
    list->count++;

    return list->list[list->count - 1];
}
DO_EXPORT void  do_ret_list_check(do_ret_list_t *list)
{
    if (!get_do_list_priv(list)->memory)
        get_do_list_priv(list)->memory = NULL;
}

DO_EXPORT void do_ret_list_free(do_ret_list_t *list)
{
    do_ret_list_priv_t *priv;
    priv = get_do_list_priv(list);
    int i;
//    if (!priv->memory)
  //      priv->memory = 2;
    //else
    for ( i = 0; i < priv->memory->count; i++ )
        do_free(priv->memory->list[i]);

    do_list_free(priv->memory);
    do_free(list->list);
    do_free(list);

}
DO_EXPORT int do_ret_list_count(do_ret_list_t *list)
{
    return list->count;
}
DO_EXPORT void *do_ret_list_item_data(do_ret_list_t *list, int indx)
{
    return &list->list[indx]->data;
}
DO_EXPORT int do_ret_list_item_len(do_ret_list_t *list, int indx)
{
    return list->list[indx]->recLen;
}
DO_EXPORT int do_ret_list_item_pos(do_ret_list_t *list, int indx)
{
    return list->list[indx]->recPos;
}
DO_EXPORT  do_ret_list_t  *do_ret_list_static_new()
{

    return do_ret_list_new();
}

DO_EXPORT do_ret_list_t  *do_ret_list_static_add(do_ret_list_t *list, void *data, size_t size)
{
    if (!list)
        list = do_ret_list_new();
    if (data && size > 0) {
        do_extended_ret_t *ret;
        ret = (do_extended_ret_t*) do_malloc(sizeof(do_extended_ret_t) - sizeof(ret->data) + size);
        do_ret_list_memory_add(list, ret);
        ret->recLen = size;
        ret->recPos = POS_UNDEFINE;
        memcpy(&ret->data, data, size);
        do_ret_list_add(list, ret);
    }
    return list;
}

DO_EXPORT do_ret_list_t  *do_ret_list_add_list_and_free(do_ret_list_t *list, do_ret_list_t *add)
{
    if (!list)
        return add;

    int i;
    do_ret_list_priv_t *priv, *priv_add;
    priv = get_do_list_priv(list);
    priv_add = get_do_list_priv(list);

    for ( i = 0; i < priv_add->memory->count; i++ )
        do_list_add(priv->memory, priv_add->memory->list[i]);
    for ( i = 0; i < add->count; i++ )
        do_ret_list_add(list, add->list[i]);
    do_list_free(priv_add->memory);
    do_free(add->list);
    do_free(add);
    return list;
}

DO_EXPORT do_extended_t *do_extended_new(do_alias_t *alias)
{
    do_extended_t *extended;

    extended = (do_extended_t*)do_malloc(sizeof(do_extended_t));

    extended->alias = alias;
    extended->break_.func = NULL;
    extended->break_.user_data = NULL;
    extended->reject_.func = NULL;
    extended->reject_.user_data = NULL;
    extended->file_db = DO_DB_END;

    extended->filter_count = 0;
    extended->filter_dont_reject = FALSE;
    extended->filter_field_len = NULL;
    extended->filter_term = NULL;

    extended->key_num = -1;
    extended->key_ptr = NULL;
    extended->key_pos = POS_UNDEFINE;

    extended->fields_count = 0;
    extended->fields = NULL;

    return extended;
};
DO_EXPORT void do_extended_set_break_func(do_extended_t *extended, do_extended_break_func break_func, void *user_data)
{
    extended->break_.func = break_func;
    extended->break_.user_data = user_data;
}
DO_EXPORT void do_extended_set_reject_func(do_extended_t *extended, do_extended_reject_func reject_func, void *user_data)
{
    extended->reject_.func = reject_func;
    extended->reject_.user_data = user_data;
}

DO_EXPORT void do_extended_set_file_db(do_extended_t *extended, file_db_t file_db)
{
    extended->file_db = file_db;
}
DO_EXPORT void  do_extended_set_key(do_extended_t *extended, int key_num, void *key_ptr)
{
    extended->key_num = key_num;
    extended->key_ptr = key_ptr;
    extended->key_pos = POS_UNDEFINE;
}
DO_EXPORT void do_extended_set_filter(do_extended_t *extended, int reject, int count, ...)
{
    va_list arg;
    int i;
    size_t size;

    extended->filter_dont_reject = !reject;
    if (extended->filter_field_len)
        do_free(extended->filter_field_len);
    if (extended->filter_term)
        do_free(extended->filter_term);
    extended->filter_count = count;
    if (!extended->filter_count) {
        extended->filter_field_len = NULL;
        extended->filter_term = NULL;
    }
    else {
        size = 0;
        extended->filter_field_len = (size_t*)do_malloc(sizeof(size_t)*count);
        va_start(arg, count);
        for (i = 0; i < extended->filter_count; i++) {
            extended->filter_field_len[i] = va_arg(arg, size_t);
            size += extended->filter_field_len[i];
        }
        va_end(arg);
        extended->filter_term = do_malloc(sizeof(_do_term_t)*extended->filter_count + size);
    }
}
DO_EXPORT void do_extended_set_filter_v(do_extended_t *extended, int reject, do_list_t *sizes)
{
    int i;
    size_t size;

    extended->filter_dont_reject = !reject;
    if (extended->filter_field_len)
        do_free(extended->filter_field_len);
    if (extended->filter_term)
        do_free(extended->filter_term);
    extended->filter_count = sizes->count;
    if (!extended->filter_count) {
        extended->filter_field_len = NULL;
        extended->filter_term = NULL;
    }
    else {
        size = 0;
        extended->filter_field_len = (size_t*)do_malloc(sizeof(size_t)*sizes->count);
        for (i = 0; i < extended->filter_count; i++) {
            extended->filter_field_len[i] = (size_t)sizes->list[i];
            size += extended->filter_field_len[i];
        }
        extended->filter_term = do_malloc(sizeof(_do_term_t)*extended->filter_count + size);
    }
}
DO_EXPORT void do_extended_set_filter_term(do_extended_t *extended,
                                                  int number,
                                                  int field_type,
                                                  int field_offset,
                                                  int comparison_code,
                                                  int connector,
                                                  void *value)
{
    _do_term_t *term;
    int i;
    term = extended->filter_term;
    for (i = 0; i < number; i++) {
        term ++;
        term = (_do_term_t*)((char*)term + extended->filter_field_len[i]);
    }
    term->fieldType = field_type;
    term->fieldLen = extended->filter_field_len[number];
    term->fieldOffset = field_offset;
    term->comparisonCode = comparison_code;
    term->connector = connector;
    if (value)
        memcpy(term + 1, value, extended->filter_field_len[number]);
}
DO_EXPORT void  do_extended_set_filter_term_set_value(do_extended_t *extended,
                                                  int number,
                                                  void *value)
{
    _do_term_t *term;
    int i;
    term = extended->filter_term;
    for (i = 0; i < number; i++) {
        term ++;
        term = (_do_term_t*)((char*)term + extended->filter_field_len[i]);
    }
    memcpy(term + 1, value, extended->filter_field_len[number]);
}
DO_EXPORT void  do_extended_set_fields_full_record(do_extended_t *extended, size_t record_size)
{
    do_extended_set_fields(extended, 1, record_size, 0);
}

DO_EXPORT void  do_extended_set_fields(do_extended_t *extended, int count, ...)
{
    if (extended->fields)
        do_free(extended->fields);
    if (!count)
        extended->fields = NULL;
    else {
        va_list arg;
        int i;
        extended->fields_count = count;
        extended->fields = do_malloc(sizeof(*extended->fields)*count);

        va_start(arg, count);
        for (i = 0; i < extended->fields_count; i++) {
            extended->fields[i].size = va_arg(arg, size_t);
            extended->fields[i].offset = va_arg(arg, int);
        }
        va_end(arg);

    }
}
#define RUNNING_ (!extended->break_.func || !extended->break_.func(extended->break_.user_data))
#define max_(a,b) (a > b) ? a : b
#define min_(a,b) (a > b) ? b : a

DO_EXPORT do_ret_list_t *do_extended_get(do_extended_t *extended, int  max_record)
{
    int status;
    int i;
    BTI_SINT rec_count;
    void *crnt_ret;
    BTI_WORD dataLen;
    BTI_SINT *numReturned;
    RETURNED_HEADER *record;//!!, *old;
    void *buffer, *crnt_buf;
    size_t header_size;
    size_t buffer_size;
    do_ret_list_t *retval;
    int record_size;
    do_alias_t *alias;
    size_t filter_size;
    int reject;
    int ret_size;

    if (extended->file_db == DO_DB_END) {
        do_log(LOG_ERR, "do_extended_get : undefine file_db");
        return NULL;
    }
    if (extended->key_num == -1 || extended->key_ptr == NULL) {
        do_log(LOG_ERR, "do_extended_get : undefine key");
        return NULL;
    }
    if (extended->fields_count == 0) {
        do_log(LOG_ERR, "do_extended_get : undefine fields");
        return NULL;
    }
    alias = extended->alias;
    retval = do_ret_list_new();
    if (extended->key_pos == POS_UNDEFINE) {
        dataLen = file_record_size[extended->file_db];
        buffer = do_malloc(dataLen);
        status = do_alias_btr(alias, B_GET_GE, extended->file_db, buffer, &dataLen, extended->key_ptr, extended->key_num);
        do_free(buffer);
        if (status == DO_KEY_NOT_FOUND)
            return retval;
        if (status == DO_ERROR) {
            do_ret_list_free(retval);
            return NULL;
        }
    }
    else
        status = DO_OK;

    filter_size = 0;
    for (i = 0; i < extended->filter_count; i++)
        filter_size += sizeof(_do_term_t) + extended->filter_field_len[i];

    header_size = sizeof(GNE_HEADER) + filter_size +
                  sizeof(RETRIEVAL_HEADER) + sizeof(FIELD_RETRIEVAL_HEADER)*
                                                    extended->fields_count;
    record_size = 0;
    for (i = 0; i < extended->fields_count; i++)
        record_size += extended->fields[i].size;

    if (max_record != -1) {
        ret_size =
        (sizeof(RETURNED_HEADER) + record_size + 1)*(max_record + 3) + sizeof(BTI_SINT);
        //min_((sizeof(RETURNED_HEADER) + record_size + 1)*(max_record + 2) + sizeof(BTI_SINT),
          //     RET_BUFFER_SIZE);
    }
    else
        ret_size = RET_BUFFER_SIZE;
    buffer_size = max_(header_size, ret_size);

    if (buffer_size > RET_BUFFER_SIZE)
        buffer_size = RET_BUFFER_SIZE;

    rec_count = (buffer_size - sizeof(BTI_SINT)) / (sizeof(RETURNED_HEADER) + record_size + 1) - 2;
    while (rec_count < 2) {
        buffer_size *=2;
        rec_count = (buffer_size - sizeof(BTI_SINT)) / (sizeof(RETURNED_HEADER) + record_size + 1) - 2;
    }

    reject = FALSE;
    while (status == DO_OK ||
           (extended->filter_dont_reject && status == DO_REJECT)) {
        if (!RUNNING_)
            break;

        if (max_record != -1 && max_record <= retval->count)
            break;

        buffer = do_malloc(buffer_size);
        if (!buffer) {
            do_log(LOG_ERR, "Could not allocate memory %d byte", buffer_size);
            status = DO_ERROR;
            break;
        }
        do_ret_list_memory_add(retval, buffer);

        dataLen = buffer_size;
        crnt_buf = buffer;
        memset(crnt_buf, 0, header_size);
        memcpy(&((GNE_HEADER*)crnt_buf)->currencyConst[0], "UC", 2);
        ((GNE_HEADER*)crnt_buf)->descriptionLen = header_size;
        ((GNE_HEADER*)crnt_buf)->rejectCount =(extended->filter_dont_reject) ? 65535 : 1;
        ((GNE_HEADER*)crnt_buf)->numberTerms = extended->filter_count;
        crnt_buf += sizeof(GNE_HEADER);

        if (filter_size) {
            memcpy(crnt_buf, extended->filter_term, filter_size);
            crnt_buf += filter_size;
        }
        if (max_record != -1)
            ((RETRIEVAL_HEADER*)crnt_buf)->maxRecsToRetrieve = min_(rec_count, max_record - retval->count + 1);
        else
            ((RETRIEVAL_HEADER*)crnt_buf)->maxRecsToRetrieve = rec_count;
        ((RETRIEVAL_HEADER*)crnt_buf)->noFieldsToRetrieve = extended->fields_count;
        crnt_buf += sizeof(RETRIEVAL_HEADER);

        for (i = 0; i < extended->fields_count; i++) {
            ((FIELD_RETRIEVAL_HEADER*)crnt_buf)->fieldLen = extended->fields[i].size;
            ((FIELD_RETRIEVAL_HEADER*)crnt_buf)->fieldOffset = extended->fields[i].offset;
            crnt_buf += sizeof(FIELD_RETRIEVAL_HEADER);
        }
        //ctrl_data_write_to_file(ctrl_data_new_from_src(buffer, header_size), "d:/4.txt");
        do_ret_list_check(retval); //!!
        status = do_alias_btr(alias, B_GET_NEXT_EXTENDED, extended->file_db, buffer, &dataLen, extended->key_ptr, extended->key_num);
        if ( status == DO_ERROR )
            break;

        crnt_ret = buffer + sizeof(BTI_SINT);
        numReturned = buffer;
        int len1 = 0;
        for (i = 0;/* status != DO_ERROR &&*/ i < *numReturned; i++) {
            if (!RUNNING_)
                break;
            record = crnt_ret;
            do_ret_list_check(retval); //!!
            len1 += (record->recLen + sizeof(RETURNED_HEADER));

            if (extended->reject_.func &&
                extended->reject_.func(record + 1, record->recLen, record->recPos,
                                       extended->reject_.user_data)) {
                reject = TRUE;
                break;
            }

            if (extended->key_pos != record->recPos) {
                if (max_record != -1 && max_record <= retval->count)
                    break;
                extended->key_pos = record->recPos;
                do_ret_list_add(retval, record);
                //!!old = record;
            }
            //!!else
                //reject = FALSE;
            crnt_ret += sizeof(record->recLen) + sizeof(record->recPos) +
                      record->recLen;
        }
        if (reject)
            break;
    }
    if (status == DO_ERROR || !RUNNING_ ) {
        do_ret_list_free(retval);
        retval = NULL;
    }
    return retval;
}

DO_EXPORT void do_extended_free(do_extended_t *extended)
{
    if (extended->filter_field_len)
        do_free(extended->filter_field_len);

    if (extended->filter_term)
        do_free(extended->filter_term);

    if (extended->fields)
        do_free(extended->fields);

    do_free(extended);
}
/*
do_ret_list_t *do_extended_get(do_extended_t *extended, int  max_record)
{
    BTI_SINT status;
    int i;
    BTI_SINT rec_count;
    void *crnt_ret;
    BTI_WORD dataLen;
    BTI_SINT *numReturned;
    RETURNED_HEADER *record;
    void *buffer, *crnt_buf;
    size_t header_size;
    size_t buffer_size;
    do_ret_list_t *retval;
    int record_size;
    do_alias_t *alias;
    size_t filter_size;
    int reject;
#ifndef PSQL_IS_CORRECT
    int correct = 0;
#endif

    if (extended->file_db == DO_DB_END) {
        do_log(LOG_ERR, "do_extended_get : undefine file_db");
        return NULL;
    }
    if (extended->key_num == -1 || extended->key_ptr == NULL) {
        do_log(LOG_ERR, "do_extended_get : undefine key");
        return NULL;
    }
    if (extended->fields_count == 0) {
        do_log(LOG_ERR, "do_extended_get : undefine fields");
        return NULL;
    }
    alias = extended->alias;

    retval = do_ret_list_new();

    if (extended->key_pos == POS_UNDEFINE) {
        int status;
        dataLen = CORRECT_BUFFER_SIZE;
        buffer = do_malloc(dataLen);
        if (!extended->filter_count)
            status = do_alias_btr(alias, B_GET_FIRST, extended->file_db, buffer, &dataLen, extended->key_ptr, extended->key_num);
        else
            status = do_alias_btr(alias, B_GET_GE, extended->file_db, buffer, &dataLen, extended->key_ptr, extended->key_num);
        if (status == DO_KEY_NOT_FOUND)
            return retval;
        if (status == DO_ERROR) {
            do_ret_list_free(retval);
            return NULL;
        }
    }
    else
        status = B_NO_ERROR;

    filter_size = 0;
    for (i = 0; i < extended->filter_count; i++)
        filter_size += sizeof(_do_term_t) + extended->filter_field_len[i];

    header_size = sizeof(GNE_HEADER) + filter_size +
                  sizeof(RETRIEVAL_HEADER) + sizeof(FIELD_RETRIEVAL_HEADER)*
                                                    extended->fields_count;
    buffer_size = max_(header_size, RET_BUFFER_SIZE);
    record_size = 0;
    for (i = 0; i < extended->fields_count; i++)
        record_size += extended->fields[i].size;
    rec_count = (buffer_size - sizeof(BTI_SINT)) / (sizeof(RETURNED_HEADER) + record_size + 1) - 1;

    while (rec_count < 2) {
        buffer_size *=2;
        rec_count = (buffer_size - sizeof(BTI_SINT)) / (sizeof(RETURNED_HEADER) + record_size + 1) - 1;
    }
    reject = FALSE;
    while (status == B_NO_ERROR ||
           (extended->filter_dont_reject && status == B_REJECT_COUNT_REACHED)) {
        if (!RUNNING_)
            break;
#ifndef PSQL_IS_CORRECT
        if (correct) {
            record_size++;
            header_size += sizeof(FIELD_RETRIEVAL_HEADER);
            rec_count = (buffer_size - sizeof(BTI_SINT)) / (sizeof(RETURNED_HEADER) + record_size + 1) - 1;
        }
#endif

        if (max_record != -1 && max_record <= retval->count)
            break;

        buffer = do_malloc(buffer_size);
        if (!buffer) {
            do_log(LOG_ERR, "Could not allocate memory %d byte", buffer_size);
            status = DO_ERROR;
            break;
        }
        do_ret_list_memory_add(retval, buffer);

        dataLen = buffer_size;
        crnt_buf = buffer;
        memset(crnt_buf, 0, header_size);
        memcpy(&((GNE_HEADER*)crnt_buf)->currencyConst[0], "UC", 2);
        ((GNE_HEADER*)crnt_buf)->descriptionLen = header_size;
        ((GNE_HEADER*)crnt_buf)->rejectCount =(extended->filter_dont_reject) ? 65535 : 1;
        ((GNE_HEADER*)crnt_buf)->numberTerms = extended->filter_count;
        crnt_buf += sizeof(GNE_HEADER);

        if (filter_size) {
            memcpy(crnt_buf, extended->filter_term, filter_size);
            crnt_buf += filter_size;
        }
        if (max_record != -1)
            ((RETRIEVAL_HEADER*)crnt_buf)->maxRecsToRetrieve = min_(rec_count, max_record - retval->count + 1);
        else
            ((RETRIEVAL_HEADER*)crnt_buf)->maxRecsToRetrieve = rec_count;
        ((RETRIEVAL_HEADER*)crnt_buf)->noFieldsToRetrieve = extended->fields_count;
#ifndef PSQL_IS_CORRECT
        if (correct)
            ((RETRIEVAL_HEADER*)crnt_buf)->noFieldsToRetrieve = extended->fields_count + 1;
#endif
        crnt_buf += sizeof(RETRIEVAL_HEADER);

        for (i = 0; i < extended->fields_count; i++) {
            ((FIELD_RETRIEVAL_HEADER*)crnt_buf)->fieldLen = extended->fields[i].size;
            ((FIELD_RETRIEVAL_HEADER*)crnt_buf)->fieldOffset = extended->fields[i].offset;
            crnt_buf += sizeof(FIELD_RETRIEVAL_HEADER);
        }
#ifndef PSQL_IS_CORRECT
        if (correct) {
            ((FIELD_RETRIEVAL_HEADER*)crnt_buf)->fieldLen = 1;
            ((FIELD_RETRIEVAL_HEADER*)crnt_buf)->fieldOffset = MAX_POS;
            crnt_buf += sizeof(FIELD_RETRIEVAL_HEADER);
        }
#endif
        //ctrl_data_write_to_file(ctrl_data_new_from_src(buffer, header_size), "d:/4.txt");

        status =  BTRVID(B_GET_NEXT_EXTENDED,
                        alias->dbBlock[extended->file_db],
                        buffer, &dataLen,
                        extended->key_ptr, extended->key_num,
                        (BTI_BUFFER_PTR)&alias->clientID);

        switch (status) {
            case B_NO_ERROR:
            case B_REJECT_COUNT_REACHED:
                break;
#ifndef PSQL_IS_CORRECT
            case B_DATALENGTH_ERROR:
                if (!correct) {
                    correct = 1;
                    status = B_NO_ERROR;
                    break;
                }
#endif
            default:
                do_log(LOG_ERR, "Btrieve operation %d status = %d file %s", B_GET_NEXT_EXTENDED, status, alias->dbfilename[extended->file_db]);
                if (alias->breakOnError)
                    do_alias_close(alias);
                do_ret_list_free(retval);
                return NULL;
        }

        crnt_ret = buffer + sizeof(BTI_SINT);
        numReturned = buffer;
        for (i = 0; i < *numReturned; i++) {
            if (!RUNNING_)
                break;
            record = crnt_ret;

            if (extended->reject_.func &&
                extended->reject_.func(record + 1, record->recLen, record->recPos,
                                       extended->reject_.user_data)) {
                reject = TRUE;
                break;
            }


            if (extended->key_pos != record->recPos) {
                extended->key_pos = record->recPos;
                do_ret_list_add(retval, record);
            }
            crnt_ret += sizeof(record->recLen) + sizeof(record->recPos) +
                      record->recLen;
        }
        if (reject)
            break;
    }
    if (!RUNNING_ ) {
        do_ret_list_free(retval);
        retval = NULL;
    }
    return retval;
}
*/
