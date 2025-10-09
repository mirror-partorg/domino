//!!#ifndef USE_MINIMAL

#include "sync_server.h"
#include "sync_info.h"
#include "replic.h"
#include "../config/config.h"
#include <domino.h>
#include "../misc/csv_util.h"
#include "../misc/send_error.h"
#include <errno.h>
#include <limits.h>
#include <locale.h>

#define LIMITS_STOCKS 512


#define RUNNING (do_mproc_state_get_state() == DO_STATE_RUN && do_alias_conected(sync->alias))
#define RUNNING_ ((!inproc || do_mproc_state_get_state() == DO_STATE_RUN) && do_alias_conected(alias))

//!!static int send_stock(sync_server_t *sync, stock_key0_t *stock_key0);
static int send_stocks(sync_server_t *sync, do_list_t *list);


void sync_server_check_config(option_t *opt)
{
    do_log(LOG_DEBUG, "Check config");
}

sync_server_t *sync_server_new(option_t *opt, sync_info_t *sync_info, do_alias_t* alias)
{
    sync_server_t *sync = (sync_server_t *) do_malloc(sizeof(sync_server_t));
    sync->opt = opt;
    sync->sync_info = sync_info;
    sync->alias = alias;
    return sync;
}

void sync_server_free(sync_server_t *sync)
{
    do_free(sync);
}
static int break_func(void *user_data)
{
    return do_mproc_state_get_state() != DO_STATE_RUN;
}

static int send_product(sync_server_t *sync, product_key4_t *product_key4)
{
    if (product_key4->code[do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE)] ==
        do_param(DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR)[0])
        return TRUE;


    int err = 0;
    char *code;
    char *param;
    do_data_t *out;
    do_data_t *in;

    code = do_text(sync->alias, product_key4->code);
    out = do_data_new();
#ifdef ZLIB
    do_data_set_send_flags(out, DO_EXCH_FLAG_ZIP);
#endif

    err = !replic_get_product(sync->alias, code, out, break_func);

    if (!err) {
        do_rpc_client_t *client;
        client = do_rpc_client_new(sync->opt->client_host,
                                 sync->opt->client_tcp_port,
                                 SYNC_USERNAME,
                                 SYNC_PASSWORD,
                                 FALSE);
        if (client &&
#ifdef GCRYPT
            do_rpc_client_crypt_init_key(client, sync->opt->client_key_file) &&
#endif
            do_rpc_client_connect(client)) {
            param = (char*)do_malloc(strlen(code) + 3);
            sprintf(param, "\"%s\"", code);
            in = do_data_new();
            err = !do_rpc_client_send_command(client, "put_product", param, out, in);
            if (err)
                do_log(LOG_ERR, "Error rpc put product order");
            do_free(param);
            if (!err) {
                param = do_data_str(in);
                err = strcmp(param, DO_EXCH_OK);
                do_free(param);
            }
            do_data_free(in);
            do_rpc_client_disconnect(client);
        }
        else
            err = 1;
        if (client)
            do_rpc_client_free(client);
    }
    do_data_free(out);
    if (!err)
	do_log(LOG_INFO, "send product \"%s\"", code);
    do_free(code);
    return !err;
}
static int send_partner(sync_server_t *sync, partner_key0_t *partner_key0)
{
    int err = 0;
    char *g_code;
    char *param;
    do_data_t *out;
    do_data_t *in;

    g_code = do_text(sync->alias, partner_key0->g_code);
    out = do_data_new();
#ifdef ZLIB
    do_data_set_send_flags(out, DO_EXCH_FLAG_ZIP);
#endif

    err = !replic_get_partner(sync->alias, g_code, partner_key0->code, out, break_func);

    if (!err && do_data_size(out) > 0) {
        do_rpc_client_t *client;
        client = do_rpc_client_new(sync->opt->client_host,
                                 sync->opt->client_tcp_port,
                                 SYNC_USERNAME,
                                 SYNC_PASSWORD,
                                 FALSE);
        if (client &&
#ifdef GCRYPT
            do_rpc_client_crypt_init_key(client, sync->opt->client_key_file) &&
#endif
            do_rpc_client_connect(client)) {
            param = (char*)do_malloc(strlen(g_code) + 100);
            sprintf(param, "\"%s.%d\"", g_code, (int)partner_key0->code);
            in = do_data_new();
            err = !do_rpc_client_send_command(client, "put_partner", param, out, in);
            if (err)
                do_log(LOG_ERR, "Error rpc put partner order");
            do_free(param);
            if (!err) {
                param = do_data_str(in);
                err = strcmp(param, DO_EXCH_OK);
                do_free(param);
            }
            do_data_free(in);
            do_rpc_client_disconnect(client);
        }
        else
            err = 1;
        if (client)
            do_rpc_client_free(client);
    }
    do_data_free(out);
    if (!err)
	do_log(LOG_INFO, "send parnter \"%s.%d\"", g_code, partner_key0->code);
    do_free(g_code);
    return !err;
}
static int send_handbook_document_order(sync_server_t *sync, document_order_key0_t *document_order_key0)
{

    int err = 0;
    char *dtype;
    char *sklad;
    char *document;
    char *order;
    int line;

    dtype = do_text(sync->alias, document_order_key0->dtype);
    document = do_text(sync->alias, document_order_key0->document);
    sklad = do_text(sync->alias, document_order_key0->sklad);
    order = (char*)do_malloc(strlen(dtype)+strlen(sklad)+strlen(document)+3+21);
    line = document_order_key0->line;
    sprintf(order, "%s.%s.%s.%d", dtype, sklad, document, line);

    char *param;
    do_data_t *out;
    do_data_t *in;

    out = do_data_new();
#ifdef ZLIB
    do_data_set_send_flags(out, DO_EXCH_FLAG_ZIP);
#endif

    err = !replic_get_document_order(sync->alias, dtype, sklad, document, document_order_key0->line, out, break_func);

    if (!err) {
        do_rpc_client_t *client;
        client = do_rpc_client_new(sync->opt->client_host,
                                 sync->opt->client_tcp_port,
                                 SYNC_USERNAME,
                                 SYNC_PASSWORD,
                                 FALSE);
        if (client &&
#ifdef GCRYPT
            do_rpc_client_crypt_init_key(client, sync->opt->client_key_file) &&
#endif
            do_rpc_client_connect(client)) {
            param = (char*)do_malloc(strlen(order) + 3);
            sprintf(param, "\"%s\"", order);
            in = do_data_new();
            err = !do_rpc_client_send_command(client, "put_document_order", param, out, in);
            if (err)
                do_log(LOG_ERR, "Error rpc put document order");
            do_free(param);
            if (!err) {
                param = do_data_str(in);
                err = strcmp(param, DO_EXCH_OK);
                do_free(param);
            }
            do_data_free(in);
            do_rpc_client_disconnect(client);
        }
        else
            err = 1;
        if (client)
            do_rpc_client_free(client);
    }
    do_data_free(out);
    if (!err)
	do_log(LOG_INFO, "send document order \"%s.%s.%s.%d\"", dtype, sklad, document, document_order_key0->line);
    do_free(order);
    do_free(dtype);
    do_free(sklad);
    do_free(document);
    return !err;
}
static int send_document(sync_server_t *sync, document_key0_t *document_key0)
{

    int err = 0;
    char *dtype;
    char *sklad;
    char *document;
    char *doc_name;

    dtype = do_text(sync->alias, document_key0->dtype);
    document = do_text(sync->alias, document_key0->document);
    sklad = do_text(sync->alias, document_key0->sklad);
    doc_name = (char*)do_malloc(strlen(dtype)+strlen(sklad)+strlen(document)+3);
    sprintf(doc_name, "%s.%s.%s", dtype, sklad, document);

    do_free(dtype);
    do_free(sklad);
    do_free(document);

    char *param;
    do_data_t *out;
    do_data_t *in;

    out = do_data_new();
#ifdef ZLIB
    do_data_set_send_flags(out, DO_EXCH_FLAG_ZIP);
#endif
    sync_get_document(sync->alias, document_key0, out);

    if (do_data_is_error(out)) {
        document = do_data_error(out);
        do_log(LOG_ERR, "Cant read document %s : %s", doc_name, document);
        do_free(document);
        do_free(doc_name);
        do_data_free(out);
        return 0;
    }

    do_rpc_client_t *client;
    client = do_rpc_client_new(sync->opt->client_host,
                             sync->opt->client_tcp_port,
                             SYNC_USERNAME,
                             SYNC_PASSWORD,
                             FALSE);
    if (client &&
#ifdef GCRYPT
        do_rpc_client_crypt_init_key(client, sync->opt->client_key_file) &&
#endif
        do_rpc_client_connect(client)) {
        param = (char*)do_malloc(strlen(doc_name) + 3);
        sprintf(param, "\"%s\"", doc_name);
        in = do_data_new();
        do_log(LOG_INFO, "send document \"%s\"", doc_name);
        err = !do_rpc_client_send_command(client, "put_document", param, out, in);
        if (err)
            do_log(LOG_ERR, "Error rpc put document");

        do_free(param);
        if (!err) {
            param = do_data_str(in);
            err = strcmp(param, DO_EXCH_OK);
            do_free(param);
        }
        do_data_free(in);
        do_rpc_client_disconnect(client);
    }
    else
        err = 1;

    if (client)
        do_rpc_client_free(client);
    do_data_free(out);
    if (!err)
	do_log(LOG_INFO, "send document order \"%s\"", doc_name);
    do_free(doc_name);
    return !err;
}

static void send_error_from_host(const char *msg, const char *dst, int port, const char *name)
{
#ifdef SEND_ERROR_ONLY_WORK_TIME
    if ( !do_isworktime() )
        return;
#endif
    const char *pref = "Error from";
    char hostname[1024], *buf;
    if (gethostname(hostname, 1023))
        strcpy(hostname, "unknow");
    buf = (char*)do_malloc(strlen(pref)+strlen(hostname)+strlen(msg)+500);
    sprintf(buf, "%s %s: %s to %s:%d %s", pref, hostname, msg, dst, port, name ? name : "");
    send_error_message(buf);
    do_free(buf);
}
#define MAX_DOC_LIST 100
static char common_unit[do_sizeof(document_key0_t, sklad)] = {0,};
static char local_unit[do_sizeof(document_key0_t, sklad)] = {0,};
static int doc_count = -1;
static int doc_product_count = -1;
static int operation_count = -1;
static struct {
    BTI_CHAR dtype[do_sizeof(document_key0_t, dtype)];
} doc_list[MAX_DOC_LIST];
static struct {
    BTI_CHAR dtype[do_sizeof(document_key0_t, dtype)];
} doc_product_list[MAX_DOC_LIST];

static struct {
    BTI_CHAR dtype[do_sizeof(document_struct_t, dtype)];
    BTI_CHAR dtype1[do_sizeof(document_struct_t, dtype1)];
} operation_list[MAX_DOC_LIST];

static int read_document_type(const char *filename, do_alias_t *alias)
{
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s\n", filename, strerror(errno));
        return 0;
    }
    doc_count = 0;
    doc_product_count = 0;
    operation_count = 0;

    do_text_set(alias, doc_product_list[doc_product_count++].dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_text_set(alias, doc_product_list[doc_product_count++].dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));

    size_t len = BUFSIZ;
    char* line = (char*) do_malloc(len + 1);
    char *type;
    char *operation;
    while (!feof(fp))  {
        char *cp,*elements[10],*str;
        int element_count;
        if (fgets(line, len+1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = (char *)do_realloc(line, len * 2 + 1);
            if (!fgets(line+len, len+1, fp)) {
                do_free(line);
                do_log(LOG_ERR, "Error reading configuration file (%s): %s\n", filename, strerror(errno));
                return 0;
            }
            len *= 2;
        }
        cp = line;
        CSV_DELSPACE(cp);
        if (CSV_COMMENT(cp))
            continue;
        element_count = 0;

        str = cp;
        while ( !CSV_EOS(cp) )
           ++cp;
        *cp = '\0';
        while ( CSV_SPACE(cp) )
           --cp;
        *cp = '\0';
        if ( str == cp ) continue;
        cp = str;
        while ( !CSV_EOS(cp)) {
            if ( element_count > 9 ) break;
            elements[element_count++] = cp;
            while ( !CSV_SPACE(cp) && !CSV_EOS(cp) ) cp++;
            if ( CSV_EOS(cp) ) break;
            *cp = '\0';
            cp++;
        }
        if ( !element_count ) continue;
        if ( !strcmp(elements[0], "FORCE_OPERATION") ) {
            if ( element_count >= 3 ) {
                type = (char*)do_param_by_name(elements[1]);
                operation = elements[2];
                if ( type ) {
                    operation_count++;
                    if ( operation_count < MAX_DOC_LIST ) {
                        do_text_set(alias, operation_list[operation_count-1].dtype, type);
                        do_text_set(alias, operation_list[operation_count-1].dtype1, operation);
                    }
                }
                else {
                    do_log(LOG_ERR, "Error undefine document type \"%s\"", elements[1]);
                    operation_count = 0;
                    return FALSE;
                }
            }

        }
        else {
            type = (char*)do_param_by_name(elements[0]);
            if ( type ) {
                doc_count++;
                if ( doc_count < MAX_DOC_LIST )
                    do_text_set(alias, doc_list[doc_count-1].dtype, type);
            }
            else {
                do_log(LOG_ERR, "Error undefine document type \"%s\"", elements[0]);
                doc_count = 0;
                return FALSE;
            }
        }
    }
    do_free(line);
    fclose(fp);
    return TRUE;
}
/*
static int read_document_type(const char *filename, do_alias_t *alias)
{
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL)  {
        do_log(LOG_ERR, "Error opening configuration file (%s): %s\n", filename, strerror(errno));
        return 0;
    }
    doc_count = 0;
    size_t len = BUFSIZ;
    char* line = (char*) do_malloc(len + 1);
    char *type;
    while (!feof(fp))  {
        char* cp,*element;
        if (fgets(line, len+1, fp) == NULL) break;
        while ((strlen(line) == len) && !feof(fp)) {
            line = (char *)do_realloc(line, len * 2 + 1);
            if (!fgets(line+len, len+1, fp)) {
        	do_free(line);
	        do_log(LOG_ERR, "Error reading configuration file (%s): %s\n", filename, strerror(errno));
	        return 0;
            }
            len *= 2;
        }
        cp = line;
        CSV_DELSPACE(cp);
        if (CSV_COMMENT(cp))
            continue;
        element = cp;
        while (!CSV_SPACE(cp) && !CSV_EOS(cp))
           ++cp;
        if (element == cp) continue;
        *cp = '\0';
        type = (char*)do_param_by_name(element);
        if ( type ) {
            doc_count++;
            do_text_set(alias, doc_list[doc_count-1].dtype, type);
        }
        else {
            do_log(LOG_ERR, "Error undefine document type \"%s\"", element);
            doc_count = 0;
            return 0;
        }
    }
    do_free(line);
    fclose(fp);
    return 1;
}*/
static int read_main_protocol(sync_server_t *sync);
static int need_send_document(sync_server_t *sync, document_key0_t *document_key0)
{
    int i;

    do_log(LOG_INFO, "! doc %s.%s.%s", do_text(sync->alias, document_key0->dtype),do_text(sync->alias, document_key0->sklad),do_text(sync->alias, document_key0->document));

    if ( do_cmp( document_key0->sklad, local_unit) )
        return FALSE;

    if ( sync->opt->document_replic_all ) {
        for ( i = 0; i < doc_count; i++ )
            if ( !do_cmp(document_key0->dtype, doc_list[i].dtype) )
                return TRUE;
    }
    else {
        int status;
        document_rec_t document;
        BTI_LONG today;
        time_t now = time(NULL);
        struct tm tm = *localtime(&now);
        do_date_set(&today, tm);

        status = do_document_get0(sync->alias, &document, document_key0, DO_GET_EQUAL);
        if ( status == DO_ERROR )
            return FALSE;

        if ( status == DO_OK ) {
            if ( sync->opt->document_replic_aging_days >= 0 &&
                today - sync->opt->document_replic_aging_days >= document.data.date ) {
                for ( i = 0; i < doc_product_count; i++ )
                     if ( !do_cmp(document.data.dtype, doc_product_list[i].dtype) )
                         return TRUE;
            }
            for ( i = 0; i < operation_count; i++ )
                if ( !do_cmp(document.data.dtype, operation_list[i].dtype) &&
                     !do_cmp(document.data.dtype1, operation_list[i].dtype1) )
                    return TRUE;
        }
    }
    return FALSE;
}

static int key_cmp(document_key0_t *k1, document_key0_t *k2)
{
    return memcmp(k1, k2, sizeof(document_key0_t));
}
void sync_server_run(sync_server_t *sync)
{
    do_protocol_context_t context;
    protocol_rec_t protocol;

    int status;//, indx;

// from main protocol
#ifdef DEBUG_READ_ENUM
    domino_set_debug_btr(TRUE);
#endif
    if ( ( sync->opt->handbook_replic1 ||  sync->opt->document_replic1 || sync->opt->stock_replic1 ) && sync->opt->client_host ) {
        if ( !read_main_protocol(sync) )
        {
            DO_SEC_SLEEP(10);
#ifdef DEBUG_READ_ENUM
            domino_set_debug_btr(FALSE);
#endif
            return;
        }
    }

#ifdef DEBUG_READ_ENUM
    domino_set_debug_btr(FALSE);
#endif
// from protocol

    if (common_unit[0] == '\0')
        do_text_set(sync->alias, common_unit, do_param(DO_PARAM_COMMON_SKLAD));
    if (local_unit[0] == '\0')
        do_text_set(sync->alias, local_unit, domino_unit());
    if (doc_count == -1)
        read_document_type(sync->opt->document_replic_config_file, sync->alias);

    do_sort_list_t *documents;
    if ( sync->opt->document_replic1 )
        documents = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)key_cmp, NULL);


    status = do_protocol_step(sync->alias, &protocol, DO_STEP_NEXT);
    while (status == DO_OK) {
        if (!RUNNING)
            break;
        if ( do_protocol_parse(sync->alias, &protocol, &context) ) {
            if (sync->opt->handbook_replic1) {
                switch (context.object) {
                    case DO_PROTOCOL_OBJECT_PRODUCT:
                    case DO_PROTOCOL_OBJECT_PRODUCT_DATA:
                        if (
                            context.action == DO_PROTOCOL_ACTION_CHANGE ||
                            context.action == DO_PROTOCOL_ACTION_DELETE ||
                            context.action == DO_PROTOCOL_ACTION_INSERT
                           )
                            if (!send_product(sync, &context.product_key4))
                                send_error_from_host("send product", sync->opt->client_host, sync->opt->client_tcp_port, sync->opt->client_name);
                        break;
                    case DO_PROTOCOL_OBJECT_PARTNER:
                    case DO_PROTOCOL_OBJECT_PARTNER_DATA:
                        if (
                            context.action == DO_PROTOCOL_ACTION_CHANGE ||
                            context.action == DO_PROTOCOL_ACTION_DELETE ||
                            context.action == DO_PROTOCOL_ACTION_INSERT
                           )
                            if (!send_partner(sync, &context.partner_key0))
                                send_error_from_host("send partner", sync->opt->client_host, sync->opt->client_tcp_port, sync->opt->client_name);
                        break;
                    case DO_PROTOCOL_OBJECT_DOCUMENT_ORDER:
                        if (
                            context.action == DO_PROTOCOL_ACTION_CHANGE_LINE ||
                            context.action == DO_PROTOCOL_ACTION_DELETE_LINE ||
                            context.action == DO_PROTOCOL_ACTION_INSERT_LINE
                           )                        {
                            if (do_cmp(context.document_order_key0.sklad, common_unit))
                                break;
                            if (!send_handbook_document_order(sync, &context.document_order_key0))
                                send_error_from_host("send document order", sync->opt->client_host, sync->opt->client_tcp_port, sync->opt->client_name);
                            }
                    default:
                        break;
                }
            }
            /*
            if ( sync->opt->document_replic1 ) {
                switch (context.object) {
                    case DO_PROTOCOL_OBJECT_DOCUMENT:
                        if (
                            context.action == DO_PROTOCOL_ACTION_ACCEPT ||
                            context.action == DO_PROTOCOL_ACTION_DEACCEPT ||
                            context.action == DO_PROTOCOL_ACTION_DELETE
                           ) {
                               if ( !do_sort_list_find(documents, &indx, &context.document_key0) )
                                do_sort_list_add_alloc(documents, &context.document_key0, sizeof(context.document_key0));
                           }

                    default:
                        break;
                }
            }*/
        }
        status = do_protocol_step(sync->alias, &protocol, DO_STEP_NEXT);
    }
    if ( sync->opt->document_replic1 ) {
        if ( status != DO_ERROR && documents->count > 0 ) {
            int i;
            for ( i = 0; i < documents->count; i++ ) {
                document_key0_t *document_key0;
                document_key0 = documents->list[i];
                if ( need_send_document(sync, document_key0) ) {
                    if ( !send_document (sync, document_key0) )
                       send_error_from_host("send document", sync->opt->client_host, sync->opt->client_tcp_port, sync->opt->client_name);
                }
            }
        }
    }
    if ( sync->opt->document_replic1 )
        do_sort_list_free(documents);

}
static int get_last_value(do_alias_t *alias, const char *name);
static int set_last_value(do_alias_t *alias, const char *name, int last);
static int read_main_protocol(sync_server_t *sync)
{
    product_data_key0_t product_data_key0, key;
    product_data_rec_t product_data;
    do_alias_t *alias = sync->alias;

    int last,status,new_last,i;


    last = get_last_value(alias, sync->opt->client_name);
    if ( last < 0 )
        return FALSE;
    do_log(LOG_INFO,"last %d",last);//!!

    //do_log(LOG_INFO, "last0 %d",last);
    do_text_set(alias, product_data_key0.code, "00001");
    do_cpy(key.code, product_data_key0.code);
    product_data_key0.type = 77;

    if ( !last )
    {
        product_data_key0.number = INT_MAX;
        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_LT);
        if ( status == DO_ERROR )
            return FALSE;
        if ( status == DO_KEY_NOT_FOUND ||
             do_cmp(key.code, product_data_key0.code) ||
             product_data_key0.type != 77 )
        {
            do_log(LOG_WARNING, "product data 77 for '00001' empty");
            return TRUE;
        }
        return set_last_value(alias, sync->opt->client_name, product_data.data.number);
    }

	product_data_key0.number = last;
    new_last = last;
    do_list_t *stocks;
    do_string_list_t *products;
    do_string_list_t *documents;

    stocks = do_list_new(TRUE);
    products = do_string_list_new();
    documents = do_string_list_new();

    //do_log(LOG_INFO, "new last %d",new_last);
#ifdef HAVY_DEBUG
	do_log(LOG_INFO, "get protocol %d", product_data_key0.number);
#endif

    status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_GT);
    if ( status == DO_OK && !do_cmp(key.code, product_data_key0.code) && product_data_key0.type == 77) {
        struct tm tm;
        BTI_LONG d,t;
        char *buff;
        d = do_product_data_param_int(alias, &product_data, "3");
        t = do_product_data_param_int(alias, &product_data, "4");
        do_date(d, &tm);
        do_time(t, &tm);
        buff = do_strdup_printf("%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d", tm.tm_mday , tm.tm_mon + 1, tm.tm_year + 1900,
                                tm.tm_hour, tm.tm_min, tm.tm_sec);
        do_log(LOG_INFO, "get protocol from %s ", buff);
        do_free(buff);
    }
    while ( status == DO_OK )
    {
        char *value, *object;
        int indx;

        if ( do_cmp(key.code, product_data_key0.code) || product_data_key0.type != 77 ) break;

        new_last = product_data.data.number;
        //value = do_text(alias, product_data.data.code);
        //do_log(LOG_INFO, "data %s.%d.%d ", value, product_data.data.type, product_data.data.number);
        //do_free(value);
        value = do_product_data_param(alias, &product_data, "1");
        if ( !strcmp(value,"2") ) { // document
            if ( sync->opt->document_replic1 ) {
                object = do_product_data_param(alias, &product_data, "2");
                if ( !do_string_list_find(documents, &indx, object) )
                    do_string_list_add(documents, object);
                else
                    do_free(object);
            }
        }
        else
        if ( !strcmp(value,"4") ) { // main product
            if ( sync->opt->handbook_replic1 ) {
                object = do_product_data_param(alias, &product_data, "2");
                if ( !do_string_list_find(products, &indx, object) )
                    do_string_list_add(products, object);
                else
                    do_free(object);
            }
        }
        else
        if ( !strcmp(value,"5") ) // stock
        {
            if ( sync->opt->stock_replic1 ) {
                char *store;
                stock_key0_t stock_key0;

                do_free(value);
                value = do_product_data_param(alias, &product_data, "2");
                store = do_product_data_param(alias, &product_data, "5");
                do_text_set(alias,stock_key0.sklad, store);
                do_text_set(alias,stock_key0.code, value);

                do_list_add_alloc(stocks, &stock_key0, sizeof(stock_key0));

                do_free(store);
            }
        }
        do_free(value);
        if ( status != DO_OK )
            break;
        /*if ( stocks->count > 99 )
            break;*/
        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_NEXT);
    }
    if ( status != DO_ERROR && products->count > 0 ) {
        //int i;
        for ( i = 0; i < products->count; i++ ) {
            product_key4_t product_key4;
            if ( !strcmp(products->list[i], "00001") ) {
                do_log(LOG_INFO, "skip main product '%s'", products->list[i]);
                continue;
            }
            do_text_set(alias, product_key4.code, products->list[i]);
            do_log(LOG_INFO, "send main product '%s'", products->list[i]);
            if ( !send_product(sync, &product_key4) )
            {
                send_error_from_host("send product", sync->opt->client_host, sync->opt->client_tcp_port, sync->opt->client_name);
                status = DO_ERROR;
            }
        }
    }
    if ( status != DO_ERROR && documents->count > 0 ) {
        //int i;
        for ( i = 0; i < documents->count; i++ ) {
            document_key0_t document_key0;
            char *ch, *str;
            str = documents->list[i];
            ch = strchr(str, '.');
            if ( ch ) {
                *ch = '\0';
                do_text_set(sync->alias, document_key0.dtype, str);
                str = ch + 1;
                ch = strchr(str, '.');
                if ( ch ) {
                    *ch = '\0';
                    do_text_set(sync->alias, document_key0.sklad, str);
                    str = ch + 1;
                    do_text_set(sync->alias, document_key0.document, str);
                   if ( need_send_document(sync, &document_key0) ) {
                       if ( !send_document (sync, &document_key0) )
                           send_error_from_host("send document", sync->opt->client_host, sync->opt->client_tcp_port, sync->opt->client_name);
                   }
                }
            }
        }
    }

    if ( status != DO_ERROR && stocks->count > 0 ) {
        do_list_t *crnt;
        crnt = do_list_new(FALSE);
        int indx;
        indx = 0;
        while ( indx < stocks->count ) {
               for ( i = 0; i < LIMITS_STOCKS && indx < stocks->count; i++,indx++ ) {
                    do_list_add(crnt, stocks->list[indx]);
#ifdef HAVY_DEBUG
                    char *v1,*v2;
                    v1 = do_text(alias, ((stock_key0_t*)stocks->list[indx])->code);
                    v2 = do_text(alias, ((stock_key0_t*)stocks->list[indx])->sklad);
                    do_log(LOG_INFO, "send stocks %s %s", v2, v1);
                    do_free(v1);do_free(v2);
#endif
               }
               do_log(LOG_INFO, "send stocks %d шт", crnt->count);
               if ( !send_stocks(sync, crnt) ) {
                   send_error_from_host("send stocks", sync->opt->client_host, sync->opt->client_tcp_port, sync->opt->client_name);
                   status = DO_ERROR;
                   break;
               }
#ifdef HAVY_DEBUG
               do_log(LOG_INFO, "sent stocks %d шт", crnt->count);
#endif
               do_list_clear(crnt);
         }
         do_list_free(crnt);
    }
    if ( status != DO_ERROR && new_last != last && !set_last_value(alias, sync->opt->client_name, new_last) ) {
         status = DO_ERROR;
    }
    do_list_free(stocks);
    do_string_list_free(products);
    do_string_list_free(documents);
    return status != DO_ERROR;
}
static int get_last_value(do_alias_t *alias, const char *name)
{
    int status;
    enum_key0_t key;
    enum_key0_t enum_key0;
    enum_rec_t  enum_;
    char *name_;

    name_ = do_strdup_printf("SYNC.%s", name);
    do_text_set(alias, enum_key0.name, name_);
    do_cpy(key.name, enum_key0.name);

    //do_log(LOG_INFO,"read enum");//!!
    status = do_enum_get0(alias, &enum_, &enum_key0, DO_GET_EQUAL);
    //do_log(LOG_INFO,"readee enum");//!!

    if ( status == DO_ERROR )
        return -1;
    if ( status == DO_KEY_NOT_FOUND ||
         do_cmp(key.name, enum_key0.name) )
        return 0;
    do_free(name_);
    return enum_.data.lastvalue;
}
static int set_last_value(do_alias_t *alias, const char *name, int last)
{
    int status;
    enum_key0_t key;
    enum_key0_t enum_key0;
    enum_rec_t  enum_;
    char *name_;

    do_log(LOG_INFO, "set last %d", last);//!!

    name_ = do_strdup_printf("SYNC.%s", name);
    do_text_set(alias, enum_key0.name, name_);
    do_cpy(key.name, enum_key0.name);

    status = do_enum_get0(alias, &enum_, &enum_key0, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return FALSE;
    if ( status == DO_KEY_NOT_FOUND ||
         do_cmp(key.name, enum_key0.name) )
    {
        do_text_set(alias, enum_.data.name, name_);
        enum_.data.lastvalue = last;
        enum_.data.step = 1;
        enum_.data.LowBound = 0;
        enum_.data.hiBound = INT_MAX;
        enum_.data.cycleFl = FALSE;
        do_text_set(alias, enum_.data.format, "N11");
        status = do_enum_insert(alias, &enum_);
    }
    else
    {
        enum_.data.lastvalue = last;
        status = do_enum_update(alias, &enum_);
    }
    return status == DO_OK;
}
/*!!
static int send_stock(sync_server_t *sync, stock_key0_t *stock_key0)
{

    int err = 0;
    //char *code;
    do_data_t *out;
    do_data_t *in;
    char *code,*store;

    store = do_text(sync->alias, stock_key0->sklad);
    code = do_text(sync->alias, stock_key0->code);

    out = do_data_new();
#ifdef ZLIB
    do_data_set_send_flags(out, DO_EXCH_FLAG_ZIP);
#endif

    err = !replic_get_stock(sync->alias, store, code, out, break_func);

    if (!err) {
        do_rpc_client_t *client;
        client = do_rpc_client_new(sync->opt->client_host,
                                 sync->opt->client_tcp_port,
                                 SYNC_USERNAME,
                                 SYNC_PASSWORD,
                                 FALSE);
        if (client &&
#ifdef GCRYPT
            do_rpc_client_crypt_init_key(client, sync->opt->client_key_file) &&
#endif
            do_rpc_client_connect(client)) {
            in = do_data_new();
            err = !do_rpc_client_send_command(client, "put_stock", "", out, in);
            if (err)
                do_log(LOG_ERR, "Error rpc put product order");
            if (!err) {
                char *value;
                value = do_data_str(in);
                err = strcmp(value, DO_EXCH_OK);
                do_free(value);
            }
            do_data_free(in);
            do_rpc_client_disconnect(client);
        }
        else
            err = 1;
        if (client)
            do_rpc_client_free(client);
    }
    do_data_free(out);
    if (!err)
        do_log(LOG_INFO, "send stock \"%s\" \"%s\"", store, code);
    do_free(code);
    do_free(store);
    return !err;
}
*/
static int send_stocks(sync_server_t *sync, do_list_t *list)
{

    int err = 0;
    //char *code;
    do_data_t *out;
    do_data_t *in;

    out = do_data_new();
#ifdef ZLIB
    do_data_set_send_flags(out, DO_EXCH_FLAG_ZIP);
#endif

    err = !replic_get_stocks(sync->alias, list, out, break_func);

    if (!err) {
        do_rpc_client_t *client;
        client = do_rpc_client_new(sync->opt->client_host,
                                 sync->opt->client_tcp_port,
                                 SYNC_USERNAME,
                                 SYNC_PASSWORD,
                                 FALSE);
        if (client &&
#ifdef GCRYPT
            do_rpc_client_crypt_init_key(client, sync->opt->client_key_file) &&
#endif
            do_rpc_client_connect(client)) {
            in = do_data_new();
            err = !do_rpc_client_send_command(client, "put_stocks", "", out, in);
            if (err)
                do_log(LOG_ERR, "Error rpc put stocks");
            if (!err) {
                char *value;
                value = do_data_str(in);
                err = strcmp(value, DO_EXCH_OK);
                do_free(value);
            }
            do_data_free(in);
            do_rpc_client_disconnect(client);
        }
        else
            err = 1;
        if (client)
            do_rpc_client_free(client);
    }
    do_data_free(out);
    if (!err)
        do_log(LOG_INFO, "send stocks ok");
    return !err;
}

//#endif
