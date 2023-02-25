#include <domino.h>
#include <dolib.h>
#include "../misc/define.h"
#include <time.h>
#include <limits.h>

static do_param_t action_param[DO_PROTOCOL_ACTION_NONE] = {
    DO_PARAM_PROTOCOL_ACTION_INSERT,
    DO_PARAM_PROTOCOL_ACTION_INSERT_LINE,
    DO_PARAM_PROTOCOL_ACTION_ACCEPT,
    DO_PARAM_PROTOCOL_ACTION_DEACCEPT,
    DO_PARAM_PROTOCOL_ACTION_CHANGE,
    DO_PARAM_PROTOCOL_ACTION_DELETE,
    DO_PARAM_PROTOCOL_ACTION_DELETE_LINE,
    DO_PARAM_PROTOCOL_ACTION_CHANGE_KEY,
    DO_PARAM_PROTOCOL_ACTION_CHANGE_LINE,
    DO_PARAM_PROTOCOL_ACTION_CHANGE_LOCAL,
    DO_PARAM_PROTOCOL_ACTION_LOGIN,
    DO_PARAM_PROTOCOL_ACTION_LOGOUT,
    DO_PARAM_PROTOCOL_ACTION_REPLIC
};

static do_param_t object_param[DO_PROTOCOL_OBJECT_NONE] = {
    DO_PARAM_PROTOCOL_OBJECT_PRODUCT,
    DO_PARAM_PROTOCOL_OBJECT_SKLAD,
    DO_PARAM_PROTOCOL_OBJECT_REGION,
    DO_PARAM_PROTOCOL_OBJECT_PARTNER,
    DO_PARAM_PROTOCOL_OBJECT_CLASS,
    DO_PARAM_PROTOCOL_OBJECT_GROUP,
    DO_PARAM_PROTOCOL_OBJECT_SUBGROUP,
    DO_PARAM_PROTOCOL_OBJECT_DOCUMENT,
    DO_PARAM_PROTOCOL_OBJECT_CHECK,
    DO_PARAM_PROTOCOL_OBJECT_SYSTEM,
    DO_PARAM_PROTOCOL_OBJECT_REPLIC,
    DO_PARAM_PROTOCOL_OBJECT_DOCUMENT_LINK,
    DO_PARAM_PROTOCOL_OBJECT_PARTNER_DATA,
    DO_PARAM_PROTOCOL_OBJECT_PRODUCT_DATA,
    DO_PARAM_PROTOCOL_OBJECT_DOCUMENT_DATA,
    DO_PARAM_PROTOCOL_OBJECT_DOCUMENT_ORDER,
    DO_PARAM_PROTOCOL_OBJECT_OTHER,
};
const char *do_alias_username(do_alias_t *alias);
static int do_protocol_document_add_custom(do_alias_t *alias, const char *dtype, const char* unit, const char *document, const char *action_str);

DO_EXPORT int do_protocol_document_order_add(do_alias_t *alias,  const char *dtype, const char* unit, const char *document, int line, do_protocol_action_t action, const char *action_str)
{
    protocol_rec_t protocol;
    char buff[100];
    sprintf(buff, "%s.%s.%s.%s", do_param(DO_PARAM_PROTOCOL_OBJECT_DOCUMENT_ORDER), dtype, unit, document);
    do_text_set(alias, protocol.data.objectName, buff);
    do_text_set(alias, protocol.data.userName, do_alias_username(alias));
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    BTI_LONG date_,time_;
    protocol.data.date = do_date_set(&date_, tm);
    protocol.data.time = do_time_set(&time_, tm);
    protocol.size = sizeof(protocol.data);
    switch (action) {
        case DO_PROTOCOL_ACTION_NONE:
            do_text_set(alias, protocol.data.action, (action_str) ? action_str  : "");
            break;
        default: {
            sprintf(buff, "%s %d", do_param(action_param[action]), line);
            do_text_set(alias, protocol.data.action, buff);
        }
    }
    if (do_protocol_insert(alias, &protocol) != DO_OK)
        return 0;
    if (action == DO_PROTOCOL_ACTION_INSERT ||
        action == DO_PROTOCOL_ACTION_CHANGE ||
        action == DO_PROTOCOL_ACTION_DELETE ) {
        if ( !do_protocol_document_add(alias, dtype, unit, document, DO_PROTOCOL_ACTION_CHANGE_LINE))
            return 0;
    }
    if ( action == DO_PROTOCOL_ACTION_NONE )
        if ( !do_protocol_document_add_custom(alias, dtype, unit, document, action_str) )
            return 0;
    return 1;
}
DO_EXPORT int do_protocol_document_order_add_key(do_alias_t *alias, document_order_key0_t *key, do_protocol_action_t action, const char *action_str)
{
    char *dtype, *unit, *document;
    dtype = do_text(alias, key->dtype);
    unit = do_text(alias, key->sklad);
    document = do_text(alias, key->document);
    int result = do_protocol_document_order_add(alias, dtype, unit, document, (int)key->line, action, action_str);
    do_free(dtype);do_free(unit);do_free(document);
    return result;
}
DO_EXPORT int do_protocol_document_order_add_rec(do_alias_t *alias, document_order_rec_t *rec, do_protocol_action_t action, const char *action_str)
{
    char *dtype, *unit, *document;
    dtype = do_text(alias, rec->data.dtype);
    unit = do_text(alias, rec->data.sklad);
    document = do_text(alias, rec->data.document);
    int result = do_protocol_document_order_add(alias, dtype, unit, document, (int)rec->data.line, action, action_str);
    do_free(dtype);do_free(unit);do_free(document);
    return result;
}

DO_EXPORT int do_protocol_document_add(do_alias_t *alias, const char *dtype, const char* unit, const char *document, do_protocol_action_t action)
{
    protocol_rec_t protocol;
    char buff[100];
    sprintf(buff, "%s.%s.%s.%s", do_param(DO_PARAM_PROTOCOL_OBJECT_DOCUMENT), dtype, unit, document);
    do_text_set(alias, protocol.data.objectName, buff);
    do_text_set(alias, protocol.data.userName, do_alias_username(alias));
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    BTI_LONG date_,time_;
    protocol.data.date = do_date_set(&date_, tm);
    protocol.data.time = do_time_set(&time_, tm);
    protocol.size = sizeof(protocol.data);
    switch (action) {
        case DO_PROTOCOL_ACTION_NONE:
            do_text_set(alias, protocol.data.action, "");
            break;
        default:
            do_text_set(alias, protocol.data.action, do_param(action_param[action]));
    }
    if (do_protocol_insert(alias, &protocol) != DO_OK)
        return 0;
    return 1;
}
static int do_protocol_document_add_custom(do_alias_t *alias, const char *dtype, const char* unit, const char *document, const char *action_str)
{
    if ( !action_str )
        return TRUE;

    protocol_rec_t protocol;
    char buff[100];
    sprintf(buff, "%s.%s.%s.%s", do_param(DO_PARAM_PROTOCOL_OBJECT_DOCUMENT), dtype, unit, document);
    do_text_set(alias, protocol.data.objectName, buff);
    do_text_set(alias, protocol.data.userName, do_alias_username(alias));
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
        BTI_LONG date_,time_;
    protocol.data.date = do_date_set(&date_, tm);
    protocol.data.time = do_time_set(&time_, tm);
    protocol.size = sizeof(protocol.data);
    do_text_set(alias, protocol.data.action, action_str);
    if (do_protocol_insert(alias, &protocol) != DO_OK)
        return 0;
    return 1;
}

DO_EXPORT int do_protocol_document_add_key(do_alias_t *alias, document_key0_t *key, do_protocol_action_t action)
{
    char  *dtype, *unit, *document;
    dtype = do_text(alias, key->dtype);
    unit = do_text(alias, key->sklad);
    document = do_text(alias, key->document);
    int result = do_protocol_document_add(alias, dtype, unit, document, action);
    do_free(dtype);do_free(unit);do_free(document);
    return result;
}
DO_EXPORT int do_protocol_document_add_rec(do_alias_t *alias, document_rec_t *rec, do_protocol_action_t action)
{
    char  *dtype, *unit, *document;
    dtype = do_text(alias, rec->data.dtype);
    unit = do_text(alias, rec->data.sklad);
    document = do_text(alias, rec->data.document);
    int result = do_protocol_document_add(alias, dtype, unit, document, action);
    do_free(dtype);do_free(unit);do_free(document);
    return result;
}
static void context_clear(do_protocol_context_t *context)
{
    context->object = DO_PROTOCOL_OBJECT_NONE;
    context->action = DO_PROTOCOL_ACTION_NONE;
}
DO_EXPORT do_protocol_context_t *do_protocol_context_new(void)
{
    do_protocol_context_t *retval = do_malloc0(sizeof(do_protocol_context_t));
    context_clear(retval);
    return retval;
}

static int make_object_context(do_alias_t *alias, do_protocol_context_t *context, const char *s, do_protocol_object_t object)
{
    char *ch = (char*)s;
    int retval = 0;
    if (*ch == '\0')
        retval = 1;
    else
    if (*ch == '.') {
        ch++;
        switch (object) {
            case DO_PROTOCOL_OBJECT_PRODUCT_DATA:
            case DO_PROTOCOL_OBJECT_PRODUCT: {
                do_text_set(alias, context->product_key4.code, ch);
                retval = 1;
                break;
            }
            case DO_PROTOCOL_OBJECT_SKLAD: {
                do_text_set(alias, context->sklad_key0.code, ch);
                retval = 1;
                break;
            }
            case DO_PROTOCOL_OBJECT_REGION: {
                do_text_set(alias, context->region_key0.code, ch);
                retval = 1;
                break;
            }
            case DO_PROTOCOL_OBJECT_PARTNER: {
                char *value = do_strdup(ch);
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.') {
                    *ch = '\0';
                    ch++;
                    if (atoi(ch) != 0) {
                        do_text_set(alias, context->partner_key0.g_code, value);
                        context->partner_key0.code = atoi(ch);
                        retval = 1;
                    }
                }
                do_free(value);
                break;
            }
            case DO_PROTOCOL_OBJECT_PARTNER_DATA: {
                char *value = do_strdup(ch);
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.') {
                    *ch = '\0';
                    ch++;
                    if (atoi(ch) != 0) {
                        do_text_set(alias, context->partner_key0.g_code, value);
                        context->partner_key0.code = atoi(ch);
                        retval = 1;
                    }
                }
                do_free(value);
                break;
            }
            case DO_PROTOCOL_OBJECT_CLASS: {
                do_text_set(alias, context->class_key0.class_id, ch);
                retval = 1;
                break;
            }
            case DO_PROTOCOL_OBJECT_GROUP: {
                char *value = do_strdup(ch);
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.') {
                    *ch = '\0';
                    ch++;
                    do_text_set(alias, context->group_key0.class_id, value);
                    do_text_set(alias, context->group_key0.group_id, ch);
                    retval = 1;
                }
                do_free(value);
                break;
            }
            case DO_PROTOCOL_OBJECT_SUBGROUP: {
                char *value = do_strdup(ch);
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.') {
                    *ch = '\0';
                    ch++;
                    char *val1 = ch;
                    while (*ch != '\0' && *ch != '.') ch++;
                    if (*ch =='.') {
                        *ch = '\0';
                        ch++;
                        do_text_set(alias, context->subgroup_key0.class_id, value);
                        do_text_set(alias, context->subgroup_key0.group_id, val1);
                        do_text_set(alias, context->subgroup_key0.subgroup_id, ch);
                        retval = 1;
                    }
                }
                do_free(value);
                break;
            }
            case DO_PROTOCOL_OBJECT_DOCUMENT_DATA:{
                char *value = do_strdup(ch);
                char *doc;
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.') {
                    *ch = '\0';
                    ch++;
                    char *val1 = ch;
                    while (*ch != '\0' && *ch != '.') ch++;
                    if (*ch =='.') {
                        *ch = '\0';
                        ch++;
                        do_text_set(alias, context->document_key0.dtype, value);
                        do_text_set(alias, context->document_key0.sklad, val1);
                        doc = ch;
                        while (*ch != '\0' && *ch != '.') ch++;
                        *ch = '\0';
                        do_text_set(alias, context->document_key0.document, doc);
                        retval = 1;
                    }
                }
                do_free(value);
                break;
            }
            case DO_PROTOCOL_OBJECT_DOCUMENT: {
                char *value = do_strdup(ch);
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.') {
                    *ch = '\0';
                    ch++;
                    char *val1 = ch;
                    while (*ch != '\0' && *ch != '.') ch++;
                    if (*ch =='.') {
                        *ch = '\0';
                        ch++;
                        do_text_set(alias, context->document_key0.dtype, value);
                        do_text_set(alias, context->document_key0.sklad, val1);
                        do_text_set(alias, context->document_key0.document, ch);
                        retval = 1;
                    }
                }
                do_free(value);
                break;
            }
            case DO_PROTOCOL_OBJECT_DOCUMENT_ORDER: {
                char *value = do_strdup(ch);
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.') {
                    *ch = '\0';
                    ch++;
                    char *val1 = ch;
                    while (*ch != '\0' && *ch != '.') ch++;
                    if (*ch =='.') {
                        *ch = '\0';
                        ch++;
                        do_text_set(alias, context->document_order_key0.dtype, value);
                        do_text_set(alias, context->document_order_key0.sklad, val1);
                        do_text_set(alias, context->document_order_key0.document, ch);
                        retval = 1;
                    }
                }
                do_free(value);
                break;
            }
            case DO_PROTOCOL_OBJECT_DOCUMENT_LINK: {
                char *value = do_strdup(ch);
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.') {
                    *ch = '\0';
                    ch++;
                    char *val1 = ch;
                    while (*ch != '\0' && *ch != '.') ch++;
                    if (*ch =='.') {
                        *ch = '\0';
                        ch++;
                        char *val2 = ch;
                        while (*ch != '\0' && *ch != '>') ch++;
                        if (*ch =='>') {
                            do_text_set(alias, context->document_key0.dtype, value);
                            do_text_set(alias, context->document_key0.sklad, val1);
                            do_text_set(alias, context->document_key0.document, val2);
                            retval = 1;
                        }
                    }
                }
                do_free(value);
                break;
            }
#ifndef DOMINO78
            case DO_PROTOCOL_OBJECT_CHECK:  {
                char *value = do_strdup(ch);
                ch = value;
                while (*ch != '\0' && *ch != '.') ch++;
                if (*ch =='.' && atoi(value)) {
                    *ch = '\0';
                    ch++;
                    if (atoi(ch)) {
                        context->shift_key0.cash = atoi(value);
                        context->shift_key0.shift = atoi(ch);
                        retval = 1;
                    }
                }
                do_free(value);
                break;
            }
#endif
            case DO_PROTOCOL_OBJECT_SYSTEM:
            case DO_PROTOCOL_OBJECT_REPLIC:
                retval = 0;
                break;
            case DO_PROTOCOL_OBJECT_OTHER:
                strcpy(context->other, ch);
                retval = 1;
                break;
            case DO_PROTOCOL_OBJECT_NONE:
                retval = 0;
                break;
        }

    }
    if (retval)
        context->object = object;
    return retval;

}
static int make_action_context(do_protocol_context_t *context, const char *s, do_protocol_action_t action)
{
    char *ch = (char*)s;
    int retval = 0;
    if (*ch == '\0')
        retval = 1;
    else
    if (*ch == ' ') {
        while (*ch == ' ') ch++;
        if (*ch != '\0') {
            if (context->object == DO_PROTOCOL_OBJECT_DOCUMENT_ORDER) {
                switch (action) {
                    case DO_PROTOCOL_ACTION_DELETE_LINE:
                    case DO_PROTOCOL_ACTION_INSERT_LINE:
                    case DO_PROTOCOL_ACTION_CHANGE_LINE:
                        context->document_order_key0.line = atoi(ch);
                        retval = 1;
                        break;
                    default:
                        retval = 0;
                        break;
                }
            }
            else
                switch (action) {
                    case DO_PROTOCOL_ACTION_CHANGE_KEY:
                    case DO_PROTOCOL_ACTION_DELETE_LINE:
                    case DO_PROTOCOL_ACTION_LOGIN:
                    case DO_PROTOCOL_ACTION_LOGOUT:
                    case DO_PROTOCOL_ACTION_INSERT_LINE:
                        strcpy(context->action_context, ch);
                        retval = 1;
                        break;
                    case DO_PROTOCOL_ACTION_INSERT:
                    case DO_PROTOCOL_ACTION_ACCEPT:
                    case DO_PROTOCOL_ACTION_DEACCEPT:
                    case DO_PROTOCOL_ACTION_CHANGE:
                    case DO_PROTOCOL_ACTION_DELETE:
                    case DO_PROTOCOL_ACTION_CHANGE_LINE:
                    case DO_PROTOCOL_ACTION_CHANGE_LOCAL:
                    case DO_PROTOCOL_ACTION_REPLIC:
                        retval = 0;
                        break;
                    case DO_PROTOCOL_ACTION_NONE:
                        retval = 0;
                        break;
                }
        }
    }
    if (retval)
        context->action = action;
    return retval;

}

DO_EXPORT int do_protocol_parse(do_alias_t *alias, protocol_rec_t *protocol, do_protocol_context_t *context)
{
    return do_protocol_parse_st(alias, &protocol->data, context);
}
DO_EXPORT int do_protocol_parse_st(do_alias_t *alias, protocol_struct_t *protocol_st, do_protocol_context_t *context)
{
    char *objectName;
    char *action;
    int retval;
    objectName = do_text(alias, protocol_st->objectName);
    action = do_text(alias, protocol_st->action);
    retval = do_protocol_parse_string(alias, objectName, action, context);
    do_free(objectName);
    do_free(action);
    return retval;
}
DO_EXPORT int do_protocol_parse_string(do_alias_t *alias, const char *objectName, const char *action, do_protocol_context_t *context)
{
    int i, j;

    context_clear(context);

    for (i = 0; i < DO_PROTOCOL_OBJECT_NONE; i++) {
        const char *obj = do_param(object_param[i]);
        if (!strncmp(objectName, obj, strlen(obj)) &&
            make_object_context(alias, context, objectName + strlen(obj), i))
            break;
    }
    if (context->object == DO_PROTOCOL_OBJECT_NONE)
        return 0;

    for (i = 0; i < DO_PROTOCOL_ACTION_NONE; i++) {
        const char *obj = do_param(action_param[i]);
        if (!strncmp(action, obj, strlen(obj)) &&
            make_action_context(context, action + strlen(obj), i ))
            break;
    }
    if (context->action == DO_PROTOCOL_ACTION_NONE) {
        for (i = 0; i < DO_PROTOCOL_ACTION_NONE; i++) {
            for (j = 0; j < do_param_alias_count(action_param[i]); j++) {
                const char *obj = do_param_alias(action_param[i], j);
                if (!strncmp(action, obj, strlen(obj)) &&
                    make_action_context(context, action + strlen(obj), i))
                    break;
            }
            if (context->action != DO_PROTOCOL_ACTION_NONE)
                break;
        }

    }
    return (context->action != DO_PROTOCOL_ACTION_NONE);
}

DO_EXPORT void do_protocol_context_free(do_protocol_context_t *context)
{
    //if (context->other)
    //    do_free(context->other);
    //if (context->action_context)
    //    do_free(context->action_context);
    do_free(context);
}
DO_EXPORT int do_protocol_add(do_alias_t *alias, do_protocol_object_t obj, const char *obj_content,
                                       do_protocol_action_t action, const char *action_content)
{
    protocol_rec_t protocol;
    int res;
    char *buff;
    if (obj_content && *obj_content != '\0') {
        buff = (char*)do_malloc(strlen(do_param(object_param[obj])) + 1 + strlen(obj_content) + 2);
        sprintf(buff, "%s.%s", do_param(object_param[obj]), obj_content);
        do_text_set(alias, protocol.data.objectName, buff);
        do_free(buff);
    }
    else
        do_text_set(alias, protocol.data.objectName, do_param(object_param[obj]));

    do_text_set(alias, protocol.data.userName, do_alias_username(alias));
    time_t now = time(NULL);
    struct tm tm = *localtime(&now);
    BTI_LONG date_,time_;
    protocol.data.date = do_date_set(&date_, tm);
    protocol.data.time = do_time_set(&time_, tm);
    protocol.size = sizeof(protocol.data);
    if (action == DO_PROTOCOL_ACTION_NONE &&
        (!action_content || *action_content == '\0'))
        do_text_set_empty(protocol.data.action);
    else {
        buff = (char*)do_malloc(100);//
            //(action != DO_PROTOCOL_ACTION_NONE) ? strlen(do_param(action_param[action])):0 + 1 +
             //(action_content) ?  strlen(action_content):0 + 2);

        sprintf(buff, "%s%s%s",
                          (action != DO_PROTOCOL_ACTION_NONE) ? do_param(action_param[action]):"",
                          (action != DO_PROTOCOL_ACTION_NONE) ? " ":"",
                          (action_content) ?  action_content:"");
        do_text_set(alias, protocol.data.action, buff);
        do_free(buff);


    }
    res = do_protocol_insert(alias, &protocol) == DO_OK;
    if ( res ) {
        switch (obj) {
            case DO_PROTOCOL_OBJECT_PRODUCT: {
                 /*if ( do_alias_tran_begin(alias) != DO_OK ) {
                    do_log(LOG_ERR, "Error start transaction");
                    res = FALSE;
                    break;
                 }*/
                 int i, status;
                 status = DO_ERROR;
                 buff = (char*)do_malloc(64);//
                 product_data_rec_t product_data;
                 product_data_key0_t product_data_key0;
                 do_text_set(alias, product_data_key0.code, "00001");
                 product_data_key0.type = 77;
                 product_data_key0.number = INT_MAX;
                 int date_int;
                 do_cpy(product_data.data.code,product_data_key0.code);
                 product_data.data.type = product_data_key0.type;
                 if ( do_product_data_key0(alias, &product_data_key0, DO_GET_LT) != DO_OK ||
                      do_cmp(product_data.data.code,product_data_key0.code) ||
                      product_data.data.type != product_data_key0.type )
                      product_data.data.number = 1;
                 else
                      product_data.data.number = product_data_key0.number + 1;

                 do_product_data_params_clear(alias, &product_data);
                 do_product_data_param_set(alias, &product_data, "1", "1");
                 do_product_data_param_set(alias, &product_data, "2", obj_content);
                 date_int=protocol.data.date;
                 sprintf(buff,"%d",date_int);
                 do_product_data_param_set(alias, &product_data, "3", buff);
                 date_int=protocol.data.time;
                 sprintf(buff,"%d",date_int);
                 do_product_data_param_set(alias, &product_data, "4", buff);
                 //do_log(LOG_INFO, "write protocol %d",product_data.data.number);
                 do_alias_set_dublicate_allow(alias, TRUE);
                 do_free(buff);
                 for ( i = 0; i < 200; i ++ ) {
                      status = do_product_data_insert(alias, &product_data);
                      if ( status != DO_DUPLICATE_KEY_VALUE )
                          break;
                      product_data.data.number += 1;
                 }
                 do_alias_set_dublicate_allow(alias, FALSE);
                 res = status == DO_OK;
                 /*if ( res ) {
                    if (do_alias_tran_end(alias) != DO_OK) {
                         do_log(LOG_ERR, "Error end transaction");
                       res = FALSE;
                    }
                }
                else
                   do_alias_tran_abort(alias);*/
            }
            default:
                  break;
        }
    }
    return res;
}
DO_EXPORT int do_protocol_partner_add(do_alias_t *alias, const char *g_code, int code, do_protocol_action_t action)
{
    int retval;
    char *buff;

    buff = (char*)do_malloc(strlen(g_code) + 1 + 30);
    sprintf(buff, "%s.%d", g_code, code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PARTNER, buff, action, NULL);
    do_free(buff);
    return retval;
}
DO_EXPORT int do_protocol_partner_add_rec(do_alias_t *alias, partner_rec_t *rec, do_protocol_action_t action)
{
    int retval, code;
    char *buff, *g_code;
    g_code = do_text(alias, rec->data.g_code);
    buff = (char*)do_malloc(strlen(g_code) + 1 + 30);
    code = rec->data.code;
    sprintf(buff, "%s.%d", g_code, code);
    do_free(g_code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PARTNER, buff, action, NULL);
    do_free(buff);
    return retval;
}
DO_EXPORT int do_protocol_partner_add_key(do_alias_t *alias, partner_key0_t *key, do_protocol_action_t action)
{
    int retval, code;
    char *buff, *g_code;
    g_code = do_text(alias, key->g_code);
    buff = (char*)do_malloc(strlen(g_code) + 1 + 30);
    code = key->code;
    sprintf(buff, "%s.%d", g_code, code);
    do_free(g_code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PARTNER, buff, action, NULL);
    do_free(buff);
    return retval;
}
DO_EXPORT int do_protocol_partner_add_recd(do_alias_t *alias, partner_data_rec_t *rec, do_protocol_action_t action)
{
    int retval, code;
    char *buff, *g_code;
    g_code = do_text(alias, rec->data.g_code);
    buff = (char*)do_malloc(strlen(g_code) + 1 + 30);
    code = rec->data.code;
    sprintf(buff, "%s.%d", g_code, code);
    do_free(g_code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PARTNER, buff, action, NULL);
    do_free(buff);
    return retval;
}
DO_EXPORT int do_protocol_partner_add_keyd(do_alias_t *alias, partner_data_key0_t *key, do_protocol_action_t action)
{
    int retval, code;
    char *buff, *g_code;
    g_code = do_text(alias, key->g_code);
    buff = (char*)do_malloc(strlen(g_code) + 1 + 30);
    code = key->code;
    sprintf(buff, "%s.%d", g_code, code);
    do_free(g_code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PARTNER, buff, action, NULL);
    do_free(buff);
    return retval;
}
DO_EXPORT int do_protocol_partner_add_recv(do_alias_t *alias, partner_view_rec_t *rec, do_protocol_action_t action)
{
    int retval, code;
    char *buff, *g_code;
    g_code = do_text(alias, rec->data.g_code);
    buff = (char*)do_malloc(strlen(g_code) + 1 + 30);
    code = rec->data.code;
    sprintf(buff, "%s.%d", g_code, code);
    do_free(g_code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PARTNER, buff, action, NULL);
    do_free(buff);
    return retval;
}
DO_EXPORT int do_protocol_partner_add_keyv(do_alias_t *alias, partner_view_key0_t *key, do_protocol_action_t action)
{
    int retval, code;
    char *buff, *g_code;
    g_code = do_text(alias, key->g_code);
    buff = (char*)do_malloc(strlen(g_code) + 1 + 30);
    code = key->code;
    sprintf(buff, "%s.%d", g_code, code);
    do_free(g_code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PARTNER, buff, action, NULL);
    do_free(buff);
    return retval;
}

DO_EXPORT int do_protocol_product_add(do_alias_t *alias, const char *code, do_protocol_action_t action)
{
    return do_protocol_add(alias, DO_PROTOCOL_OBJECT_PRODUCT, code, action, NULL);
}
DO_EXPORT int do_protocol_product_add_rec(do_alias_t *alias, product_rec_t *rec, do_protocol_action_t action)
{
    int retval;
    char *code;
    code = do_text(alias, rec->data.code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PRODUCT, code, action, NULL);
    do_free(code);
    return retval;
}

DO_EXPORT int do_protocol_product_add_key(do_alias_t *alias, product_key0_t *key, do_protocol_action_t action)
{
    int retval;
    char *code;
    code = do_text(alias, key->code);
    retval = do_protocol_add(alias, DO_PROTOCOL_OBJECT_PRODUCT, code, action, NULL);
    do_free(code);
    return retval;
}
