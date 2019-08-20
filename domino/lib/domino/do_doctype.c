#include <domino.h>
#include <dolib.h>
#include "../../misc/define.h"

#define DO_PARAM_DTYPE_SHORT_NAME "\x91\x8E\x8A\x90"
#define DO_PARAM_DTYPE_P_DOC_NAME "\x91\x8E\x8F\x5F\x84\x8E\x8A"
#define DO_PARAM_DTYPE_PARNTER_DEFAULT "\x8F\x80\x90_DEF"
#define DO_PARAM_DTYPE_DIRECTION "DT"
#define DO_PARAM_DTYPE_REST_ACTION "QTY"
#define DO_PARAM_DTYPE_PARAM "PARAM"
#define DO_PARAM_DTYPE_SUMS "SUM"
#define DO_PARAM_DTYPE_PRICE_SELECT_COPY "PR"
#define DO_PARAM_DTYPE_PRICE_ACCEPT_COPY "A_PR"

DO_EXPORT do_doctype_t *do_doctype_new(do_alias_t *alias, const char* dtype, const char* dtype1)
{
    document_type_rec_t document_type_;
    document_type_key0_t document_type_key0;
    do_text_set(alias, document_type_key0.dtype, dtype);
    do_text_set(alias, document_type_key0.dtype1, dtype1);
    switch (do_document_type_get0(alias, &document_type_, &document_type_key0, DO_GET_EQUAL)) {
        case DO_KEY_NOT_FOUND: do_log(LOG_ERR, "Document description \"%s.%s\" not found", dtype, dtype1);
            return NULL;
        case DO_OK:
            return do_doctype_new_from_dtype(alias, &document_type_);
        default:
            return NULL;
    }
}
DO_EXPORT do_doctype_t *do_doctype_new_from_document_order(do_alias_t *alias, document_order_rec_t *document_order)
{
    document_type_rec_t document_type_;
    document_type_key0_t document_type_key0;
    char *dtype, *dtype1;
    memcpy(&document_type_key0.dtype, &document_order->data.dtype, sizeof(document_type_key0.dtype));
    memcpy(&document_type_key0.dtype1, &document_order->data.dtype1, sizeof(document_type_key0.dtype1));
    int stat = do_document_type_get0(alias, &document_type_, &document_type_key0, DO_GET_EQUAL);
    switch (stat) {
        case DO_KEY_NOT_FOUND:
            dtype = do_text(alias, document_type_key0.dtype);
            dtype1 = do_text(alias, document_type_key0.dtype1);
            do_log(LOG_ERR, "Document description \"%s.%s\" not found", dtype, dtype1);
            do_free (dtype);do_free(dtype1);
            return NULL;
        case DO_OK:
            return do_doctype_new_from_dtype(alias, &document_type_);
        default:
            return NULL;
    }
}
DO_EXPORT do_doctype_t *do_doctype_new_from_document(do_alias_t *alias, document_rec_t *document)
{
    document_type_rec_t document_type_;
    document_type_key0_t document_type_key0;
    char *dtype, *dtype1;
    memcpy(&document_type_key0.dtype, &document->data.dtype, sizeof(document_type_key0.dtype));
    memcpy(&document_type_key0.dtype1, &document->data.dtype1, sizeof(document_type_key0.dtype1));
    switch (do_document_type_get0(alias, &document_type_, &document_type_key0, DO_GET_EQUAL)) {
        case DO_KEY_NOT_FOUND:
            dtype = do_text(alias, document_type_key0.dtype);
            dtype1 = do_text(alias, document_type_key0.dtype1);
            do_log(LOG_ERR, "Document description \"%s.%s\" not found", dtype, dtype1);
            do_free (dtype);do_free(dtype1);
            return NULL;
        case DO_OK:
            return do_doctype_new_from_dtype(alias, &document_type_);
        default:
            return NULL;
    }
}

DO_EXPORT do_doctype_t *do_doctype_new_from_dtype(do_alias_t *alias, document_type_rec_t *dtype)
{
    char *cp, *head;
    int sum;
    do_doctype_t *result = (do_doctype_t*)do_malloc(sizeof(do_doctype_t));
    result->sums = do_list_new(TRUE);
    result->price_accept_copy = do_list_new(TRUE);
    result->price_select_copy = do_list_new(TRUE);
    result->dtype = do_text(alias, dtype->data.dtype);
    result->dtype1 = do_text(alias, dtype->data.dtype1);
    result->name = do_text(alias, dtype->data.name);
    result->short_name = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_SHORT_NAME);
    result->p_doc_name = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_P_DOC_NAME);
    result->partner_default.g_code = NULL;
    result->partner_default.code = 0;
    char *value = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_PARNTER_DEFAULT);
    if (strlen(value)) {
        cp = value;
        while (*cp != '\0' && *cp !='.') cp++;
        if (*cp == '.') {
            result->partner_default.g_code = (char*)do_malloc(cp - value + 1);
            strncpy(result->partner_default.g_code, value, cp - value);
            result->partner_default.g_code[cp - value] = '\0';
            result->partner_default.code = atoi(++cp);
        }
    }
    do_free(value);
    value = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_DIRECTION);
    result->direction = atoi(value);
    do_free(value);
    memset(result->rest_action, 0, sizeof(result->rest_action));
    value = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_SUMS);
    head = value;
    while ( (cp = strchr(head, ',')) != NULL) {
        *cp = '\0';
        sum = atoi(head);
        if ( sum )
            do_list_add_alloc(result->sums, &sum , sizeof(sum));
        head = cp + 1;
    }
    sum = atoi(head);
    if ( sum )
        do_list_add_alloc(result->sums, &sum , sizeof(sum));

    do_free(value);
    value = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_PRICE_ACCEPT_COPY);
    head = value;
    while ( (cp = strchr(head, ',')) != NULL) {
        *cp = '\0';
        sum = atoi(head);
        if ( sum )
            do_list_add_alloc(result->price_accept_copy, &sum , sizeof(sum));
        head = cp + 1;
    }
    sum = atoi(head);
    if ( sum )
        do_list_add_alloc(result->price_accept_copy, &sum , sizeof(sum));

    do_free(value);
    value = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_PRICE_SELECT_COPY);
    head = value;
    while ( (cp = strchr(head, ',')) != NULL) {
        *cp = '\0';
        sum = atoi(head);
        if ( sum )
            do_list_add_alloc(result->price_select_copy, &sum , sizeof(sum));
        head = cp + 1;
    }
    sum = atoi(head);
    if ( sum )
        do_list_add_alloc(result->price_select_copy, &sum , sizeof(sum));

    do_free(value);
    value = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_REST_ACTION);
    int i, k;
    cp = value;
    for (i = 0; i < 4 && *cp != '\0'; i++) {
        result->rest_action[i].number = atoi(cp);
        if (!result->rest_action[i].number) break;
        while (*cp != '\0' && *cp !=',') cp++;
        if (*cp !=',') {
            result->rest_action[i].number = 0;
            break;
        }
        cp++;
        k = atoi(cp);
        switch (k) {
            case 1: result->rest_action[i].action = DO_REST_ACTION_NONE;
                break;
            case 2: result->rest_action[i].action = DO_REST_ACTION_IN;
                break;
            case 3: result->rest_action[i].action = DO_REST_ACTION_OUT;
                break;
            default:
                result->rest_action[i].number = 0;
        }
        if (!result->rest_action[i].number) break;
        while (*cp != '\0' && *cp !=',') cp++;
    }
    do_free(value);
    value = do_document_type_param(alias, dtype, DO_PARAM_DTYPE_PARAM);
    result->auto_parcel = (strlen(value) > 8) && (value[8] == '1');
    result->accept_by_line = (strlen(value) > 9) && (value[9] == '1');
    do_free(value);
    return result;
}
DO_EXPORT void do_doctype_free(do_doctype_t *doctype)
{
    do_free(doctype->dtype);
    do_free(doctype->dtype1);
    do_free(doctype->name);
    do_free(doctype->short_name);
    do_free(doctype->p_doc_name);
    do_list_free(doctype->sums);
    do_list_free(doctype->price_accept_copy);
    do_list_free(doctype->price_select_copy);
    if (doctype->partner_default.g_code) do_free(doctype->partner_default.g_code);
    do_free(doctype);
};
DO_EXPORT int do_doctype_rest_effect(do_doctype_t *doctype)
{
    int i;
    for (i = 0; i < 4; i++) {
	if (!doctype->rest_action[i].number)
	    break;
	if (doctype->rest_action[i].action != DO_REST_ACTION_NONE)
	    return 1;
    }
    return 0;
}
