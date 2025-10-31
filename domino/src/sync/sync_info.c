//!!#ifndef USE_MINIMAL

#include "sync_info.h"
#include <string.h>
#include <stddef.h>

#include "option.h"
#include "replic.h"
#include "util.h"
#include "../misc/mproc_state.h"
#include <domino.h>
#include <limits.h>

#define RUNNING (!break_func(NULL))

typedef  enum {
    DC_CMD_CONFIG = 0,

//Get
    DC_CMD_GET_PRODUCT,

    DC_CMD_GET_PRODUCTS,
    DC_CMD_GET_PRODUCTS_VIEW,
    DC_CMD_GET_PRODUCTS_DATA,

    DC_CMD_GET_BARCODES,

    DC_CMD_GET_REGIONS,

    DC_CMD_GET_PARTNER,

    DC_CMD_GET_PARTNERS,
    DC_CMD_GET_PARTNERS_VIEW,
    DC_CMD_GET_PARTNERS_DATA,

    DC_CMD_GET_CLASS,
    DC_CMD_GET_GROUPS,
    DC_CMD_GET_SUBGROUPS,

    DC_CMD_GET_OPERATIONS,

    DC_CMD_GET_SKLADS,
    DC_CMD_GET_OTDELS,

    DC_CMD_GET_USERS,

    DC_CMD_GET_DOCS,
    DC_CMD_GET_DOCS_IN_PERIOD,
    DC_CMD_GET_DOCS_FROM_PROTOCOL,

    DC_CMD_GET_STOCKS,

    DC_CMD_GET_UPDATED_STOCKS,
#ifndef DOMINO78
    DC_CMD_GET_CHECKS,

    DC_CMD_GET_REALIZATION,
#endif
    DC_CMD_GET_PROTOCOL,


// Put

    DC_CMD_PUT_PRODUCT,
    DC_CMD_PUT_PARTNER,
    DC_CMD_PUT_DOCUMENT,
    DC_CMD_PUT_DOCUMENT_ORDER,
    DC_CMD_PUT_STOCK,
    DC_CMD_PUT_STOCKS,

// Util

#ifndef DOMINO78
    DC_CMD_MAKE_REALIZATION,
#endif
    DC_CMD_GET_BALANCE_41,
    DC_CMD_GET_GOODS,
    DC_CMD_GET_GOODS_VIEW,
    DC_CMD_GET_GOODS_DATA,
    DC_CMD_GET_GOODS_BARCODES,

    DC_CMD_UNDEFINE
} sync_command_t;


static const do_rpc_command_t dc_commands[DC_CMD_UNDEFINE] = {
    {"config", "show config", "show config", 0, 0},

// Get
    {"get_product",       "get product info", "get product info\n\r\tparam: \"<code>\"", 0, 0},

    {"get_products",      "get products", "get products\n\rparam: {<base>|<parcel> \"<sklad>\"} [<code1> <code2>]", 0, 0},
    {"get_products_view", "get product's views", "get product's views\n\rparam: {<base>|<parcel> \"<sklad>\"} [<code1> <code2>]", 0, 0},
    {"get_products_data", "get product's daties", "get product's daties\n\rparam: {<base>|<parcel> \"<sklad>\"} [<code1> <code2>]", 0, 0},

    {"get_barcodes",       "get base barcodes", "get base barcodes\n\rparam: {<base>|<parcel> \"<sklad>\"}", 0, 0},

    {"get_regions", "get regions", "get regions", 0, 0},

    {"get_partner", "get partner info", "get partner info\n\r\tparam: \"<region>.<code>\"", 0, 0},

    {"get_partners", "get partners", "get partners\n\r\tparam: [\"<region>\"]", 0, 0},
    {"get_partners_view", "get partner's view", "get partners\n\r\tparam: [\"<region>\"]", 0, 0},
    {"get_partners_data", "get partner's daties", "get partners daties\n\r\tparam: [\"<region>\"]", 0, 0},

    {"get_class", "get class", "get class", 0, 0},
    {"get_groups", "get groups", "get groups", 0, 0},
    {"get_subgroups", "get subgroups", "get subgroups", 0, 0},

    {"get_operations", "get operations", "get operations", 0, 0},

    {"get_sklads", "get sklads", "get sklads", 0, 0},
    {"get_otdels", "get otdels", "get otdels", 0, 0},

    {"get_users", "get users", "get users", 0, 0},

    {"get_document", "get documents", "get documents\n\r\tparam: "
        "\"<dtype>.<sklad>.<number>\" [\"<dtype>.<sklad>.<number>\" ...]", 0, 0},
    {"get_document_in_period", "get documents in a given period", "get documents\n\r\tparam: "
        "\"<dtype>.<sklad>\" {all | <date start> <date end>}", 0, 0},
    {"get_document_from_protocol", "get modified documents", "get documents\n\r\tparam: "
        "\"<dtype>.<sklad>\" [,\"<dtype>.<sklad>\" ...] <date start> <date end>", 0, 0},
    {"get_stocks", "get stocks and price", "get stocks and price\n\r\tparam: \"<sklad>\"", 0, 0},
    {"get_updated_stocks", "get updated stocks and price", "get updated stocks and price\n\r\tparam: <index> [\"<skip_stores>\"]", 0, 0},
#ifndef DOMINO78
    {"get_checks", "get checks", "get checks\n\r\tparam: \"<sklad>\" <date start> <date end>", 0, 0},
    {"get_realization", "get realization", "get realization\n\r\tparam: \"<sklad>\" <date start> <date end>", 0, 0},
#endif
    {"get_protocol", "get protocol", "get protocol\n\r\tparam: <date start> <date end>", 0, 0},

// Put
    {"put_product", "put product for update", "put product for update\n\r\tparam: \"<code>\"", 0, 1},
    {"put_partner", "put partner for update", "put partner for update\n\r\tparam: \"<region>.<code>\"", 0, 1},
    {"put_document", "put document for update", "put document for update\n\r\tparam: \"<dtype>.<sklad>.<document>\"", 0, 1},
    {"put_document_order", "put document order for update", "put document order for update\n\r\tparam: \"<dtype>.<sklad>.<document>.<line>\"", 0, 1},
    {"put_stock", "put stock", "put stock for update", 0, 1},
    {"put_stocks", "put stocks", "put stocks for update", 0, 1},
// Utilddd
#ifndef DOMINO78
    {"make_realization", "make realization", "make realization\n\r\tparam: \"<sklad>\" <date start> <date end>", 0, 0},
#endif
    {"get_balance_41", "get balance 41", "get balance\n\r\tparam: \"<sklad>\" <date end>", 0, 0},
    {"get_goods",      "get goods", "get goods (products + parcels)\n\rparam: <code1> <code2>", 0, 0},
    {"get_goods_view", "get goods's views", "get goods's views\n\rparam: <code1> <code2>", 0, 0},
    {"get_goods_data", "get goods's daties", "get goods's daties\n\rparam: <code1> <code2>", 0, 0},
    {"get_goods_barcodes",  "get goods's barcodes", "get base barcodes\n\rparam:  <code1> <code2>", 0, 0},

};
typedef enum {
    ERROR_OK = 0,
    ERROR_CANT_CREATE_ALIAS,
    ERROR_CANT_OPEN_DB,
    ERROR_CANT_READ_DB,
    ERROR_BREAK,
    ERROR_SYNTAX,
    ERROR_NO
} error_code;

static const char *error_msg[ERROR_NO] = {"", "cant create alias db", "cant open db", "cant read db", "process break", "Syntax error. Use ? for help"};

#define RUNNING_ ((do_mproc_state_get_state() == DO_STATE_RUN) && do_alias_conected(alias))
#ifdef ZLIB
#define ZIP_IT do_data_set_send_flags(out, DO_EXCH_FLAG_ZIP)
#else
#define ZIP_IT
#endif

static void show_config(sync_info_t *sync_info, do_data_t *out);

static void get_product(sync_info_t *sync_info, do_list_t *param, do_data_t *out);

static void get_products(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_products_view(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_products_data(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_goods(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_goods_view(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_goods_data(sync_info_t *sync_info, do_list_t *param, do_data_t *out);

static void get_barcodes(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_goods_barcodes(sync_info_t *sync_info, do_list_t *param, do_data_t *out);

static void get_regions(sync_info_t *sync_info, do_list_t *param, do_data_t *out);

static void get_partner(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_partners(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_partners_view(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_partners_data(sync_info_t *sync_info, do_list_t *param, do_data_t *out);

static void get_class(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_groups(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_subgroups(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_sklads(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_otdels(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_users(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_operations(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_stocks(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_updated_stocks(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_docs(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_docs_in_period(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_docs_from_protocol(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
#ifndef DOMINO78
static void get_checks(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
static void get_realization(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
#endif
static void get_protocol(sync_info_t *sync_info, do_list_t *param, do_data_t *out);

static void put_product(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out);
static void put_partner(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out);
static void put_document(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out);
static void put_document_order(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out);
static void put_stock(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out);
static void put_stocks(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out);

#ifndef DOMINO78
static void make_realization(sync_info_t *sync_info, do_list_t *param, do_data_t *out);
#endif
static void get_balance41(sync_info_t *sync_info, do_list_t *param, do_data_t *out);

static int break_func(void *user_data)
{
    return do_mproc_state_get_state() != DO_STATE_RUN;
}


void sync_exec_command(int client_id, void *owner, int command, do_list_t *param, do_data_t *data, do_data_t *out)
{
    sync_info_t *sync_info = owner;
    switch (command) {
        case DC_CMD_CONFIG:
            show_config(sync_info, out);
            break;
        case DC_CMD_GET_PRODUCT:
            get_product(sync_info, param, out);
            break;
        case DC_CMD_GET_PRODUCTS:
            get_products(sync_info, param, out);
            break;
        case DC_CMD_GET_PRODUCTS_VIEW:
            get_products_view(sync_info, param, out);
            break;
        case DC_CMD_GET_PRODUCTS_DATA:
            get_products_data(sync_info, param, out);
            break;
        case DC_CMD_GET_GOODS:
            get_goods(sync_info, param, out);
            break;
        case DC_CMD_GET_GOODS_VIEW:
            get_goods_view(sync_info, param, out);
            break;
        case DC_CMD_GET_GOODS_DATA:
            get_goods_data(sync_info, param, out);
            break;

        case DC_CMD_GET_GOODS_BARCODES:
            get_goods_barcodes(sync_info, param, out);
            break;

        case DC_CMD_GET_BARCODES:
            get_barcodes(sync_info, param, out);
            break;

        case DC_CMD_GET_REGIONS:
            get_regions(sync_info, param, out);
            break;

        case DC_CMD_GET_PARTNER:
            get_partner(sync_info, param, out);
            break;

        case DC_CMD_GET_PARTNERS:
            get_partners(sync_info, param, out);
            break;
        case DC_CMD_GET_PARTNERS_VIEW:
            get_partners_view(sync_info, param, out);
            break;
        case DC_CMD_GET_PARTNERS_DATA:
            get_partners_data(sync_info, param, out);
            break;

        case DC_CMD_GET_CLASS:
            get_class(sync_info, param, out);
            break;
        case DC_CMD_GET_GROUPS:
            get_groups(sync_info, param, out);
            break;
        case DC_CMD_GET_SUBGROUPS:
            get_subgroups(sync_info, param, out);
            break;
        case DC_CMD_GET_OPERATIONS:
            get_operations(sync_info, param, out);
            break;
        case DC_CMD_GET_SKLADS:
            get_sklads(sync_info, param, out);
            break;
        case DC_CMD_GET_OTDELS:
            get_otdels(sync_info, param, out);
            break;
        case DC_CMD_GET_USERS:
            get_users(sync_info, param, out);
            break;
        case DC_CMD_GET_DOCS:
            get_docs(sync_info, param, out);
            break;
        case DC_CMD_GET_DOCS_IN_PERIOD:
            get_docs_in_period(sync_info, param, out);
            break;
        case DC_CMD_GET_DOCS_FROM_PROTOCOL:
            get_docs_from_protocol(sync_info, param, out);
            break;
        case DC_CMD_GET_STOCKS:
            get_stocks(sync_info, param, out);
            break;
        case DC_CMD_GET_UPDATED_STOCKS:
            get_updated_stocks(sync_info, param, out);
            break;
#ifndef DOMINO78
        case DC_CMD_GET_CHECKS:
            get_checks(sync_info, param, out);
            break;
        case DC_CMD_GET_REALIZATION:
            get_realization(sync_info, param, out);
            break;
#endif // DOMINO78
        case DC_CMD_GET_PROTOCOL:
            get_protocol(sync_info, param, out);
            break;


        case DC_CMD_PUT_PRODUCT:
            put_product(sync_info, param, data, out);
            break;
        case DC_CMD_PUT_PARTNER:
            put_partner(sync_info, param, data, out);
            break;
        case DC_CMD_PUT_DOCUMENT:
            put_document(sync_info, param, data, out);
            break;
        case DC_CMD_PUT_DOCUMENT_ORDER:
            put_document_order(sync_info, param, data, out);
            break;

        case DC_CMD_PUT_STOCK:
            put_stock(sync_info, param, data, out);
            break;

        case DC_CMD_PUT_STOCKS:
            put_stocks(sync_info, param, data, out);
            break;
#ifndef DOMINO78
        case DC_CMD_MAKE_REALIZATION:
            make_realization(sync_info, param, out);
            break;
#endif
        case DC_CMD_GET_BALANCE_41:
            get_balance41(sync_info, param, out);
            break;

        /*default:
            do_data_set_errf(out, "undefine command %d", command);*/
    }
}

do_rpc_command_t *sync_info_get_cmd()
{
    return (do_rpc_command_t*)&dc_commands;
}

int sync_info_get_cmd_count()
{
    return DC_CMD_UNDEFINE;
}

sync_info_t  *sync_info_new(option_t *opt)
{
    sync_info_t *result = (sync_info_t*) do_malloc (sizeof(sync_info_t));
    result->opt = opt;
    return result;
}

void sync_info_free(sync_info_t *sync_info)
{
    do_free(sync_info);
}

static void show_config(sync_info_t *sync_info, do_data_t *out)
{
    char *text;
    text = do_strdup("");//option_show_config(sync_info->opt);
    if (text) {
        do_data_set(out, text, strlen(text));
        do_free(text);
    }
}
static do_alias_t *create_alias(const char *alias_name, file_db_t file_id, do_data_t *out)
{
    do_alias_t *alias = domino_alias_new(alias_name);
    if (!alias || !do_alias_open(alias, 0, file_id, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return NULL;
    }
    return alias;
}
static void do_list_to_data(do_data_t *out, do_ret_list_t *list)
{
    int i;
    size_t size = 0, old_size = do_data_size(out);
    void *crnt;
    do_size_t *psize;

    do_ret_list_check(list);//!!

    for (i = 0; i < do_ret_list_count(list); i++)
        size += do_ret_list_item_len(list, i);

    size += do_ret_list_count(list)*sizeof(*psize) + sizeof(*psize);
    size += old_size;

    if (!do_data_realloc(out, size))
        do_data_set_errf(out, "Could not allocate memory %d byte", size);
    else {
        crnt = do_data_get(out) + old_size;
        psize = crnt;
        crnt += sizeof(*psize);
        *psize = do_ret_list_count(list);
        do_ret_list_check(list);//!!
        for (i = 0; i < do_ret_list_count(list); i++) {
            psize = crnt;
            crnt += sizeof(*psize);
            *psize = do_ret_list_item_len(list, i);
            memcpy(crnt, do_ret_list_item_data(list, i), do_ret_list_item_len(list, i));
            crnt += do_ret_list_item_len(list, i);
            do_ret_list_check(list);//!!
        }
    }
}

static void get_data(do_extended_t *extended,
                     do_data_t *out)
{
    do_ret_list_t *list = NULL;

    list = do_extended_get(extended, -1);

    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_ret_list_check(list);//!!
        do_list_to_data(out, list);
        do_ret_list_check(list);//!!
        do_ret_list_free(list);
    }
    ZIP_IT;
}

#define param_type(a,i)  ((do_rpc_command_param_t*)a->list[i])->type
#define param_value(a,i) ((do_rpc_command_param_t*)a->list[i])->value
#define param_count(a)   a->count

static void get_product(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if (param_count(param) != 1 ||
        param_type(param, 0) != do_lx_cstring) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_VIEW, DO_DB_PRODUCT_DATA, DO_DB_BARCODE, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }

    do_data_clear(out);
    if (!replic_get_product(alias, param_value(param, 0).string, out, break_func))
        do_data_set_err(out, "cant read db");
    ZIP_IT;
}

static void get_products(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) < 1 ||
         param_type(param, 0) != do_lx_ident  ||
         (strcmp(param_value(param, 0).string, "base") &&
          strcmp(param_value(param, 0).string, "parcel")) )  {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    int base_parcel = !strcmp(param_value(param, 0).string, "parcel");
    const char *sklad = NULL;
    product_key4_t key1,key2;
    int n = 0;
    do_text_set_empty(key1.code);
    do_text_set_empty(key2.code);
    n += 1;

    if (base_parcel) {
        if ( param_count(param) < n+1 ||
             param_type(param, n) != do_lx_cstring ) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
        sklad = param_value(param, n).string;
        n += 1;
    }
    if ((param_count(param) != n) && (param_count(param) != n + 2)) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    if (param_count(param) == n + 2) {
        if ((param_type(param, n) != do_lx_cstring) || (param_type(param, n+1) != do_lx_cstring )) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
    }

    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_DATA, DO_DB_SKLAD, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    if (param_count(param) == n + 2) {
        do_text_set(alias, key1.code, param_value(param, n).string);
        do_text_set(alias, key2.code, param_value(param, n+1).string);
        n += 2;
    }

    do_ret_list_t *list;

    list = replic_select_product_base_parcel(alias, sklad, base_parcel, &key1, &key2,  break_func);
    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}

static void get_goods(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) != 2 )  {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    product_key4_t key1,key2;
    do_text_set_empty(key1.code);
    do_text_set_empty(key2.code);


    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_DATA, DO_DB_SKLAD, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    do_text_set(alias, key1.code, param_value(param, 0).string);
    do_text_set(alias, key2.code, param_value(param, 1).string);
    do_ret_list_t *list;

    list = replic_select_goods(alias, &key1, &key2,  break_func);
    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}

static void get_products_view(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) < 1 ||
         param_type(param, 0) != do_lx_ident  ||
         (strcmp(param_value(param, 0).string, "base") &&
          strcmp(param_value(param, 0).string, "parcel")) )  {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    int base_parcel = !strcmp(param_value(param, 0).string, "parcel");
    const char *sklad = NULL;
    product_key4_t key1,key2;
    int n = 0;
    do_text_set_empty(key1.code);
    do_text_set_empty(key2.code);
    n += 1;

    if (base_parcel) {
        if ( param_count(param) < n+1 ||
             param_type(param, n) != do_lx_cstring ) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
        sklad = param_value(param, n).string;
        n += 1;
    }
    if ((param_count(param) != n) && (param_count(param) != n + 2)) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    if (param_count(param) == n + 2) {
        if ((param_type(param, n) != do_lx_cstring) || (param_type(param, n+1) != do_lx_cstring )) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
    }

    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_VIEW, DO_DB_SKLAD, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    if (param_count(param) == n + 2) {
        do_text_set(alias, key1.code, param_value(param, n).string);
        do_text_set(alias, key2.code, param_value(param, n+1).string);
    }

    do_ret_list_t *list;

    list = replic_select_product_view_base_parcel(alias, sklad, base_parcel, &key1, &key2, break_func);
    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}

static void get_goods_view(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) != 2 )  {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    product_key4_t key1,key2;
    do_text_set_empty(key1.code);
    do_text_set_empty(key2.code);

    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_VIEW, DO_DB_SKLAD, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    do_text_set(alias, key1.code, param_value(param, 0).string);
    do_text_set(alias, key2.code, param_value(param, 1).string);

    do_ret_list_t *list;

    list = replic_select_goods_view(alias, &key1, &key2, break_func);

    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}

static void get_products_data(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) < 1 ||
         param_type(param, 0) != do_lx_ident  ||
         (strcmp(param_value(param, 0).string, "base") &&
          strcmp(param_value(param, 0).string, "parcel")) )  {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    int base_parcel = !strcmp(param_value(param, 0).string, "parcel");
    const char *sklad = NULL;
    product_key4_t key1,key2;
    int n = 0;
    do_text_set_empty(key1.code);
    do_text_set_empty(key2.code);
    n += 1;

    if (base_parcel) {
        if ( param_count(param) < n+1 ||
             param_type(param, n) != do_lx_cstring ) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
        sklad = param_value(param, n).string;
        n += 1;
    }
    if ((param_count(param) != n) && (param_count(param) != n + 2)) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    if (param_count(param) == n + 2) {
        if ((param_type(param, n) != do_lx_cstring) || (param_type(param, n+1) != do_lx_cstring )) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
    }

    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_DATA, DO_DB_SKLAD, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    if (param_count(param) == n + 2) {
        do_text_set(alias, key1.code, param_value(param, n).string);
        do_text_set(alias, key2.code, param_value(param, n+1).string);
    }
    do_ret_list_t *list;

    list = replic_select_product_data_base_parcel(alias, sklad, base_parcel, &key1, &key2, break_func);
    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}

static void get_goods_data(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) != 2 )  {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    product_key4_t key1,key2;
    do_text_set_empty(key1.code);
    do_text_set_empty(key2.code);

    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_DATA, DO_DB_SKLAD, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    do_text_set(alias, key1.code, param_value(param, 0).string);
    do_text_set(alias, key2.code, param_value(param, 1).string);
    do_ret_list_t *list;

    list = replic_select_goods_data(alias, &key1, &key2, break_func);
    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}


static void get_barcodes(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) < 1 ||
         param_type(param, 0) != do_lx_ident  ||
         (strcmp(param_value(param, 0).string, "base") &&
          strcmp(param_value(param, 0).string, "parcel")) )  {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    int base_parcel = !strcmp(param_value(param, 0).string, "parcel");
    const char *sklad = NULL;
    product_key4_t key1,key2;
    int n = 0;
    do_text_set_empty(key1.code);
    do_text_set_empty(key2.code);
    if ( base_parcel ) {
        if ( ((param_count(param) != 2) && (param_count(param) != 4) ) ||
             param_type(param, 1) != do_lx_cstring ) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
        sklad = param_value(param, 1).string;
    }
    else {
        if ( (param_count(param) != 1) &&  (param_count(param) != 3) ) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
    }
    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_BARCODE, DO_DB_SKLAD, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    n = 1;
    if (param_count(param) == n + 2) {
        do_text_set(alias, key1.code, param_value(param, n).string);
        do_text_set(alias, key2.code, param_value(param, n+1).string);
    }


    do_ret_list_t *list;
    list = replic_select_barcode_base_parcel(alias, sklad, base_parcel, &key1, &key2, break_func);

    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}
static void get_goods_barcodes(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) != 2 )  {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    product_key4_t key1,key2;
    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_BARCODE, DO_DB_SKLAD, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    do_text_set(alias, key1.code, param_value(param, 0).string);
    do_text_set(alias, key2.code, param_value(param, 1).string);


    do_ret_list_t *list;
    list = replic_select_goods_barcode(alias, &key1, &key2, break_func);

    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}

static void get_regions(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    do_alias_t *alias;
    region_key0_t region_key0;
    do_extended_t   *extended;
    const file_db_t file_id = DO_DB_REGION;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    do_text_set_empty(region_key0.code);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, file_id);
    do_extended_set_key(extended, 0, &region_key0);
    do_extended_set_fields_full_record(extended, sizeof(region_struct_t));

    do_data_clear(out);
    get_data(extended, out);

    do_extended_free(extended);
    do_alias_free(alias);
}

static void get_partner(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    if ( param_count(param) != 1 ||
         param_type(param, 0) != do_lx_cstring ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    char *ch;
    int code;
    char *g_code;
    g_code = param_value(param, 0).string;
    ch = strchr(param_value(param, 0).string, '.');
    if (!ch) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    *ch = '\0';
    ch++;
    code = atoi(ch);

    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PARTNER,
#ifdef USE_DB_PARNER_VIEW
     DO_DB_PARTNER_VIEW,
#endif
     DO_DB_PARTNER_DATA, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }

    do_data_clear(out);

    if (!replic_get_partner(alias, g_code, code, out, break_func))
        do_data_set_err(out, "cant read db");
    do_alias_free(alias);
    ZIP_IT;
}

static void get_partners(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{

    const char *g_code = NULL;
    do_extended_t *extended;

    if ( param_count(param) > 1 ||
         ( param_count(param) == 1 &&
           param_type(param, 0) != do_lx_cstring ) ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    if (param_count(param) == 1)
        g_code = param_value(param, 0).string;

    do_alias_t *alias;
    const file_db_t file_id = DO_DB_PARTNER;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    partner_key0_t partner_key0;

    if (g_code)
        do_text_set(alias, partner_key0.g_code, g_code);
    else
        do_text_set(alias, partner_key0.g_code, "");
    partner_key0.code = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, file_id);
    do_extended_set_key(extended, 0, &partner_key0);

    if (g_code) {
        do_extended_set_filter(extended, TRUE, 1, sizeof(partner_key0.g_code));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_key0.g_code);
    }
    do_extended_set_fields_full_record(extended, sizeof(partner_struct_t));

    do_data_clear(out);
    get_data(extended, out);

    do_extended_free(extended);
    do_alias_free(alias);
}


static void get_partners_view(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{

#ifdef USE_DB_PARNER_VIEW
    const char *g_code = NULL;
    do_extended_t *extended;

    if ( param_count(param) > 1 ||
         ( param_count(param) == 1 &&
           param_type(param, 0) != do_lx_cstring ) ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    if (param_count(param) == 1)
        g_code = param_value(param, 0).string;

    do_alias_t *alias;
    const file_db_t file_id = DO_DB_PARTNER_VIEW;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    partner_view_key1_t partner_view_key1;
    if (g_code)
        do_text_set(alias, partner_view_key1.g_code, g_code);
    else
        do_text_set(alias, partner_view_key1.g_code, "");
    partner_view_key1.code = 0;
    partner_view_key1.type = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PARTNER_VIEW);
    do_extended_set_key(extended, 1, &partner_view_key1);

    if (g_code) {
        do_extended_set_filter(extended, TRUE, 1, sizeof(partner_view_key1.g_code));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_view_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_view_key1.g_code);
    }
    do_extended_set_fields_full_record(extended, sizeof(partner_view_struct_t));

    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);

    do_alias_free(alias);
#else
    do_size_t size_null = 0;
    do_data_set(out, &size_null, sizeof(size_null));
#endif
}

static void get_partners_data(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    const char *g_code = NULL;
    do_extended_t *extended;

    if ( param_count(param) > 1 ||
         ( param_count(param) == 1 &&
           param_type(param, 0) != do_lx_cstring ) ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    if (param_count(param) == 1)
        g_code = param_value(param, 0).string;

    do_alias_t *alias;
    const file_db_t file_id = DO_DB_PARTNER_DATA;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    partner_data_key0_t partner_data_key0;
    if (g_code)
        do_text_set(alias, partner_data_key0.g_code, g_code);
    else
        do_text_set(alias, partner_data_key0.g_code, "");
    partner_data_key0.code = 0;
    partner_data_key0.type = 0;
    partner_data_key0.number = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_PARTNER_DATA);
    do_extended_set_key(extended, 0, &partner_data_key0);

    if (g_code) {
        do_extended_set_filter(extended, TRUE, 1, sizeof(partner_data_key0.g_code));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(partner_data_struct_t, g_code),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &partner_data_key0.g_code);
    }
    do_extended_set_fields_full_record(extended, sizeof(partner_data_struct_t));

    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);
    do_alias_free(alias);
}

static void get_class(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    do_alias_t *alias;
    const file_db_t file_id = DO_DB_CLASS;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    class_key0_t class_key0;
    do_extended_t *extended;
    do_text_set_empty(class_key0.class_id);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_CLASS);
    do_extended_set_key(extended, 0, &class_key0);
    do_extended_set_fields_full_record(extended, sizeof(class_struct_t));
    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);
    do_alias_free(alias);
}
static void get_groups(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    do_alias_t *alias;
    const file_db_t file_id = DO_DB_GROUP;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    group_key0_t group_key0;
    do_extended_t *extended;
    do_text_set_empty(group_key0.class_id);
    do_text_set_empty(group_key0.group_id);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_GROUP);
    do_extended_set_key(extended, 0, &group_key0);
    do_extended_set_fields_full_record(extended, sizeof(group_struct_t));
    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);
    do_alias_free(alias);
}
static void get_subgroups(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    do_alias_t *alias;
    const file_db_t file_id = DO_DB_SUBGROUP;
    do_extended_t *extended;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    subgroup_key0_t subgroup_key0;
    do_text_set_empty(subgroup_key0.class_id);
    do_text_set_empty(subgroup_key0.group_id);
    do_text_set_empty(subgroup_key0.subgroup_id);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_SUBGROUP);
    do_extended_set_key(extended, 0, &subgroup_key0);
    do_extended_set_fields_full_record(extended, sizeof(subgroup_struct_t));
    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);
    do_alias_free(alias);
}
static void get_sklads(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    do_alias_t *alias;
    const file_db_t file_id = DO_DB_SKLAD;
    do_extended_t *extended;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    sklad_key0_t sklad_key0;
    do_text_set_empty(sklad_key0.code);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_SKLAD);
    do_extended_set_key(extended, 0, &sklad_key0);
    do_extended_set_fields_full_record(extended, sizeof(sklad_struct_t));
    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);
    do_alias_free(alias);
}
static void get_otdels(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    do_alias_t *alias;
    const file_db_t file_id = DO_DB_OTDEL;
    do_extended_t *extended;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    otdel_key0_t otdel_key0;
    do_text_set_empty(otdel_key0.code);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_OTDEL);
    do_extended_set_key(extended, 0, &otdel_key0);
    do_extended_set_fields_full_record(extended, sizeof(otdel_struct_t));
    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);
    do_alias_free(alias);
}
static void get_users(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    do_alias_t *alias;
    const file_db_t file_id = DO_DB_USER;
    do_extended_t *extended;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    user_key0_t user_key0;
    user_key0.type = ' ';
    do_text_set_empty(user_key0.name);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_USER);
    do_extended_set_key(extended, 0, &user_key0);
    do_extended_set_fields_full_record(extended, sizeof(user_struct_t));
    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);
    do_alias_free(alias);
}

static void get_operations(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_extended_t *extended;
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_TYPE, DO_DB_DOCUMENT_PROW, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }

    document_type_key0_t document_type_key0;
    do_text_set_empty(document_type_key0.dtype);
    do_text_set_empty(document_type_key0.dtype1);
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_TYPE);
    do_extended_set_key(extended, 0, &document_type_key0);
    do_extended_set_fields_full_record(extended, sizeof(document_type_struct_t));
    do_data_clear(out);
    get_data(extended, out);
    do_extended_free(extended);

    document_prow_key0_t document_prow_key0;
    do_text_set_empty(document_prow_key0.dtype);
    do_text_set_empty(document_prow_key0.dtype1);
    document_prow_key0.number = 0;
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_PROW);
    do_extended_set_key(extended, 0, &document_prow_key0);
    do_extended_set_fields_full_record(extended, sizeof(document_prow_struct_t));
    get_data(extended, out);
    do_extended_free(extended);

    do_alias_free(alias);
}
int document_key_cmp(void *a, void *b, void *user_data)
{
    return memcmp(a, b, sizeof(document_key0_t));
}

static int add_documents_context(do_alias_t *alias, do_ret_list_t *doc_list, do_data_t *out)
{
    do_ret_list_t **l_content;
    document_key0_t    document_key0;
    document_struct_t *document_st;
    void *crnt;
    size_t size;
    do_size_t *psize;
    int i, j, k;
    int err;

    err = FALSE;
    l_content = (do_ret_list_t**)do_malloc0(do_ret_list_count(doc_list)*DOCUMENT_CONTEXT_COUNT*sizeof(*l_content));
    for (i = 0; i < do_ret_list_count(doc_list); i++) {
        if (do_ret_list_item_len(doc_list,i) == sizeof(document_key0_t))
            memcpy(&document_key0, do_ret_list_item_data(doc_list, i), sizeof(document_key0_t));
        else {
            document_st = do_ret_list_item_data(doc_list, i);
            do_cpy(document_key0.dtype, document_st->dtype);
            do_cpy(document_key0.sklad, document_st->sklad);
            do_cpy(document_key0.document, document_st->document);
        }
        for (j =0; !err && j < DOCUMENT_CONTEXT_COUNT; j++) {
            switch (j) {
                case 0:
                    l_content[i*DOCUMENT_CONTEXT_COUNT + j] = replic_select_document_view(alias, &document_key0, break_func);
                    break;
                case 1:
                    l_content[i*DOCUMENT_CONTEXT_COUNT + j] = replic_select_document_data(alias, &document_key0, break_func);
                    break;
                case 2:
                    l_content[i*DOCUMENT_CONTEXT_COUNT + j] = replic_select_document_order(alias, &document_key0, break_func);
                    break;
                case 3:
                    l_content[i*DOCUMENT_CONTEXT_COUNT + j] = replic_select_document_link(alias, &document_key0, break_func);
                    break;
                case 4:
                    l_content[i*DOCUMENT_CONTEXT_COUNT + j] = replic_select_document_prowod(alias, &document_key0, break_func);
                    break;
                case 5:
                    l_content[i*DOCUMENT_CONTEXT_COUNT + j] = replic_select_document_prowod2(alias, &document_key0, break_func);
                    break;
                case 6:
                    l_content[i*DOCUMENT_CONTEXT_COUNT + j] = replic_select_document_parcel(alias, &document_key0, break_func);
                    break;
                case 7:
                    l_content[i*DOCUMENT_CONTEXT_COUNT + j] = replic_select_document_protocol(alias, &document_key0, break_func);
                    break;

            }

            if (!l_content[i*DOCUMENT_CONTEXT_COUNT + j]) {
                err = TRUE;
                break;
            }
        }
    }
    if (!err) {
        size = sizeof(*psize);
        for (i = 0; i < do_ret_list_count(doc_list); i++)
            size += sizeof(*psize) + do_ret_list_item_len(doc_list, i);
        for (j = 0; j < do_ret_list_count(doc_list)*DOCUMENT_CONTEXT_COUNT; j++) {
            size += sizeof(*psize);
            for (i = 0; i < do_ret_list_count(l_content[j]); i++)
                size += sizeof(*psize) + do_ret_list_item_len(l_content[j], i);
        }
        if (!do_data_alloc(out, size)) {
            do_data_set_errf(out, "Cant alloc %d byte", size);
            err = TRUE;
        }
        else {
            crnt = do_data_get(out);
            psize = crnt;
            crnt += sizeof(*psize);
            *psize = do_ret_list_count(doc_list);
            for (i = 0; i < do_ret_list_count(doc_list); i++) {
                psize = crnt;
                crnt += sizeof(*psize);
                *psize = do_ret_list_item_len(doc_list, i);
                memcpy(crnt, do_ret_list_item_data(doc_list, i), do_ret_list_item_len(doc_list, i));
                crnt +=do_ret_list_item_len(doc_list, i);
                for (j = 0; j < DOCUMENT_CONTEXT_COUNT; j++) {
                    psize = crnt;
                    crnt += sizeof(*psize);
                    *psize = do_ret_list_count(l_content[i*DOCUMENT_CONTEXT_COUNT + j]);

                    for (k = 0; k < do_ret_list_count(l_content[i*DOCUMENT_CONTEXT_COUNT + j]); k++) {
                        psize = crnt;
                        crnt += sizeof(*psize);
                        *psize = do_ret_list_item_len(l_content[i*DOCUMENT_CONTEXT_COUNT + j], k);
                        memcpy(crnt, do_ret_list_item_data(l_content[i*DOCUMENT_CONTEXT_COUNT + j], k),
                               do_ret_list_item_len(l_content[i*DOCUMENT_CONTEXT_COUNT + j], k));
                        crnt +=do_ret_list_item_len(l_content[i*DOCUMENT_CONTEXT_COUNT + j], k);
                    }
                }
            }
        }
    }
    for (j = 0; j < do_ret_list_count(doc_list)*DOCUMENT_CONTEXT_COUNT; j++)
        if (l_content[j])
            do_ret_list_free(l_content[j]);
    do_free(l_content);
    return !err;
}


static void get_docs(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    document_key0_t document_key0, document_key;
    document_rec_t document;
    document_link_key0_t document_link_key0;
    document_link_rec_t document_link;
    do_ret_list_t *doc_list;
    do_alias_t *alias;
    int err, status, indx;

    alias = domino_alias_new(sync_info->opt->alias_name);
    if (!alias) {
        do_data_set_err(out, error_msg[ERROR_CANT_CREATE_ALIAS]);
        return;
    }
    if (!do_alias_open(alias, 0, DOCUMENT_DB)) {
        do_data_set_err(out, error_msg[ERROR_CANT_OPEN_DB]);
        do_alias_free(alias);
        return;
    }

    err = ERROR_OK;
    doc_list = NULL;

    for ( indx = 0; indx < param_count(param); indx++ )  {
        if (!RUNNING) {
            err = ERROR_BREAK;
            break;
        }
        if ( param_type(param, indx) != do_lx_cstring ) {
            err = ERROR_SYNTAX;
            do_data_set_err(out, "Syntax error. Use ? for help");
            break;
        }
        char *value, *ch;
        value = param_value(param, indx).string;
        ch = strchr (value, '.');
        if (!ch) {
            err = TRUE;
            break;
        }
        *ch = '\0';
        do_text_set(alias, document_key0.dtype, value);
        value = ch + 1;
        ch = strchr (value, '.');
        if (!ch) {
            err = ERROR_SYNTAX;
            break;
        }
        *ch = '\0';
        do_text_set(alias, document_key0.sklad, value);
        value = ch + 1;
        do_text_set(alias, document_key0.document, value);
        switch (do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL)) {
            case DO_OK:
                doc_list = do_ret_list_static_add(doc_list, &document.data, document.size);
                break;
            case DO_KEY_NOT_FOUND:
                doc_list = do_ret_list_static_add(doc_list, &document_key0, sizeof(document_key0));
                break;
            default :
                err = ERROR_CANT_READ_DB;
                break;
        }
        if (err)
            break;
        do_cpy(document_link_key0.m_dtype, document_key0.dtype);
        do_cpy(document_link_key0.m_sklad, document_key0.sklad);
        do_cpy(document_link_key0.m_document, document_key0.document);
        document_link_key0.type = 0;
        document_link_key0.sort = 0;
        status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_GE);
        while (status == DO_OK) {
            if (!RUNNING) {
                err = ERROR_BREAK;
                break;
            }
            if (
                do_cmp(document_link_key0.m_dtype, document_key0.dtype) ||
                do_cmp(document_link_key0.m_sklad, document_key0.sklad) ||
                do_cmp(document_link_key0.m_document, document_key0.document)
               ) break;


            do_cpy(document_key.dtype, document_link.data.s_dtype);
            do_cpy(document_key.sklad, document_link.data.s_sklad);
            do_cpy(document_key.document, document_link.data.s_document);

            switch (do_document_get0(alias, &document, &document_key, DO_GET_EQUAL)) {
                case DO_OK:
                    doc_list = do_ret_list_static_add(doc_list, &document.data, document.size);
                    break;
                case DO_KEY_NOT_FOUND:
                    doc_list = do_ret_list_static_add(doc_list, &document_key, sizeof(document_key));
                    break;
                default :
                    err = ERROR_CANT_READ_DB;
                    break;
            }
            if (status == DO_ERROR || err)
                break;
            status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_NEXT);
        }
        if (status == DO_ERROR) {
            err = ERROR_CANT_READ_DB;
            if (doc_list)
                do_ret_list_free(doc_list);
            do_alias_free(alias);
            break;
        }
    }
    if (!doc_list)
        err = ERROR_SYNTAX;

    if (err) {

        do_data_set_err(out, error_msg[err]);
        if (doc_list)
            do_ret_list_free(doc_list);
        do_alias_free(alias);
        return;
    };

    do_data_set_str(out, "");
    if (!add_documents_context(alias, doc_list, out))
        do_data_set_err(out, error_msg[ERROR_CANT_READ_DB]);
    else
        ZIP_IT;
    do_ret_list_free(doc_list);
    do_alias_free(alias);
}

void sync_get_document(do_alias_t *alias, document_key0_t *document_key0, do_data_t *out)
{

    int err = 0;
    int status;
    document_rec_t document;
    do_ret_list_t *doc_list = NULL;
    document_link_key0_t document_link_key0;
    document_link_rec_t document_link;
    document_key0_t document_key;

    switch (do_document_get0(alias, &document, document_key0, DO_GET_EQUAL)) {
        case DO_OK:
            doc_list = do_ret_list_static_add(doc_list, &document.data, document.size);
            break;
        case DO_KEY_NOT_FOUND:
            doc_list = do_ret_list_static_add(doc_list, document_key0, sizeof(document_key0_t));
            break;
        default :
            do_data_set_err(out, error_msg[ERROR_CANT_READ_DB]);
            return;
    }

    do_cpy(document_link_key0.m_dtype, document_key0->dtype);
    do_cpy(document_link_key0.m_sklad, document_key0->sklad);
    do_cpy(document_link_key0.m_document, document_key0->document);
    document_link_key0.type = 0;
    document_link_key0.sort = 0;
    status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_GE);
    while (status == DO_OK) {
        if (!RUNNING) {
            err = ERROR_BREAK;
            break;
        }
        if (
            do_cmp(document_link_key0.m_dtype, document_key0->dtype) ||
            do_cmp(document_link_key0.m_sklad, document_key0->sklad) ||
            do_cmp(document_link_key0.m_document, document_key0->document)
           ) break;

        do_cpy(document_key.dtype, document_link.data.s_dtype);
        do_cpy(document_key.sklad, document_link.data.s_sklad);
        do_cpy(document_key.document, document_link.data.s_document);

        switch (do_document_get0(alias, &document, &document_key, DO_GET_EQUAL)) {
            case DO_OK:
                doc_list = do_ret_list_static_add(doc_list, &document.data, document.size);
                break;
            case DO_KEY_NOT_FOUND:
                doc_list = do_ret_list_static_add(doc_list, &document_key, sizeof(document_key));
                break;
            default :
                err = ERROR_CANT_READ_DB;
                break;
        }
        if (status == DO_ERROR || err)
            break;
        status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_NEXT);
    }
    if (status == DO_ERROR)
        err = ERROR_CANT_READ_DB;

    if (err) {
        do_data_set_err(out, error_msg[err]);
        if (doc_list)
            do_ret_list_free(doc_list);
        return;
    }
    do_data_set_str(out, "");
    if (!add_documents_context(alias, doc_list, out))
        do_data_set_err(out, error_msg[ERROR_CANT_READ_DB]);
    else
        ZIP_IT;
    do_ret_list_free(doc_list);
}

static void get_docs_in_period(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    char *value, *ch;
    BTI_LONG date_start;
    BTI_LONG date_end;
    int all;
    do_ret_list_t *doc_list;
    do_alias_t *alias;
    document_key3_t document_key3;
    do_extended_t *extended;
    int err;
    int indx;

    alias = domino_alias_new(sync_info->opt->alias_name);
    if (!alias) {
        do_data_set_err(out, "cant create alias db");
        return;
    }
    if (!do_alias_open(alias, 0, DOCUMENT_DB)) {
        do_data_set_err(out, "cant open db");
        do_alias_free(alias);
        return;
    }

    doc_list = NULL;

    if ( param_count(param) < 2 ||
         param_type(param, 0) != do_lx_cstring ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        do_alias_free(alias);
        return;
    }
    value = param_value(param, 0).string;
    ch = strchr (value, '.');
    if (!ch) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        do_alias_free(alias);
        return;
    }
    *ch = '\0';
    do_text_set(alias, document_key3.dtype, value);
    value = ch + 1;
    do_text_set(alias, document_key3.sklad, value);
    all = FALSE;
    err = FALSE;
    indx = 1;

    switch (param_type(param, indx)) {
        case do_lx_ident:
            if (!strcmp(param_value(param, 1).string, "all")) {
                all = TRUE;
                break;
            }
            err = TRUE;
        case do_lx_cdate:

            do_date_set(&date_start, param_value(param, indx).date);
            indx++;
            if (param_count(param) < indx + 1 ||
                param_type(param, indx) != do_lx_cdate) {
                err = TRUE;
                break;
            }
            do_date_set(&date_end, param_value(param, indx).date);
            break;
        default :
            err = TRUE;
    }
    if (err || param_count(param) > indx + 1) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        do_alias_free(alias);
        return;
    }

    document_key3.date = date_end;
    document_key3.time = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);

    do_extended_set_file_db(extended, DO_DB_DOCUMENT);
    do_extended_set_key(extended, 3, &document_key3);

    if (all)
        do_extended_set_filter(extended, TRUE, 2,
                                        do_sizeof(document_struct_t, dtype),
                                        do_sizeof(document_struct_t, sklad)
                                        );
    else
        do_extended_set_filter(extended, TRUE, 3,
                                        do_sizeof(document_struct_t, dtype),
                                        do_sizeof(document_struct_t, sklad),
                                        do_sizeof(document_struct_t, date)
                                        );
    do_extended_set_filter_term(extended,
                                0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_key3.dtype);

    do_extended_set_filter_term(extended,
                                1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                (!all) ? DO_EXTENDED_CONNECTION_AND : DO_EXTENDED_CONNECTION_NO,
                                &document_key3.sklad);
    if (!all)
        do_extended_set_filter_term(extended,
                                2,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(document_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &document_key3.date);
    if (all)
        document_key3.date = INT_MIN;
    else
        document_key3.date = date_start;
    do_extended_set_fields(extended, 1, sizeof(document_struct_t), 0);

    doc_list = do_extended_get(extended, -1);
    do_extended_free(extended);

    if (!doc_list || !add_documents_context(alias, doc_list, out))
        do_data_set_err(out, "Cant read db");
    else
        ZIP_IT;

    if (doc_list)
        do_ret_list_free(doc_list);
    do_alias_free(alias);
}
static int doc_in_list(do_list_t *key_list, document_key0_t *key)
{
    int i;
    for (i = 0; i < key_list->count; i++)
        if (!memcmp(key_list->list[i], key, sizeof(key->dtype) + sizeof(key->sklad)))
            return TRUE;

    return FALSE;
}
static do_ret_list_t *get_documents_do_list_from_protocol(do_alias_t *alias, do_list_t *key_list,
                                                       BTI_LONG date_start,
                                                       BTI_LONG date_end)
{
    do_ret_list_t *retval;
    document_key0_t     document_key0;
    document_rec_t      document;
    do_extended_t      *extended;
    int status;
    int err;

    protocol_key1_t protocol_key1;
    char *objectName;
    char *action;
    do_ret_list_t *temp;
    do_sort_list_t *keys;
    int i;
    do_protocol_context_t context;

    retval = NULL;

    protocol_key1.date = date_end;
    protocol_key1.time = 0;
    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);

    do_extended_set_file_db(extended, DO_DB_PROTOCOL);
    do_extended_set_key(extended, 1, &protocol_key1);
    do_extended_set_filter(extended, TRUE, 1, sizeof(protocol_key1.date));
    do_extended_set_filter_term(extended,
                                0,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(protocol_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &protocol_key1.date);
    protocol_key1.date = date_start;
    do_extended_set_fields(extended, 2,
                            do_size_and_offset(protocol_struct_t, objectName),
                            do_size_and_offset(protocol_struct_t, action)
                                        );

    temp = do_extended_get(extended, -1);

    do_extended_free(extended);

    if (!temp)
        return NULL;
    err = 0;
    keys = do_sort_list_new(1, 1, document_key_cmp, NULL);
    for (i = 0; i < do_ret_list_count(temp); i++) {
        if (!RUNNING_) {
            err = 1;
            break;
        }
        objectName = do_alias_text_(alias, do_ret_list_item_data(temp, i),
                                           do_sizeof(protocol_struct_t, objectName));
        action = do_alias_text_(alias, do_ret_list_item_data(temp, i) + do_sizeof(protocol_struct_t, objectName),
                                           do_sizeof(protocol_struct_t, action));
        if (do_protocol_parse_string(alias, objectName, action, &context)) {
            switch (context.object) {
                case DO_PROTOCOL_OBJECT_DOCUMENT: {
                    switch (context.action) {
                        case DO_PROTOCOL_ACTION_CHANGE_KEY:
                            if (doc_in_list(key_list, &context.document_key0)) {
                                do_cpy(document_key0, context.document_key0);
                                do_text_set(alias, document_key0.document, context.action_context);
                                do_sort_list_add_alloc(keys, &document_key0, sizeof(document_key0_t));
                            }
                        case DO_PROTOCOL_ACTION_INSERT:
                        case DO_PROTOCOL_ACTION_INSERT_LINE:
                        case DO_PROTOCOL_ACTION_ACCEPT:
                        case DO_PROTOCOL_ACTION_DEACCEPT:
                        case DO_PROTOCOL_ACTION_CHANGE:
                        case DO_PROTOCOL_ACTION_DELETE:
                        case DO_PROTOCOL_ACTION_DELETE_LINE:
                        case DO_PROTOCOL_ACTION_CHANGE_LINE:
                            if (doc_in_list(key_list, &context.document_key0))
                                do_sort_list_add_alloc(keys, &context.document_key0, sizeof(document_key0_t));
                            break;
                        default:
                            break;
                    };
                    break;
                }
                case DO_PROTOCOL_OBJECT_DOCUMENT_LINK:
                case DO_PROTOCOL_OBJECT_DOCUMENT_DATA:
                    if (doc_in_list(key_list, &context.document_key0))
                        do_sort_list_add_alloc(keys, &context.document_key0, sizeof(document_key0_t));
                    break;
                default:
                    break;
            }
        }
        do_free(objectName);
        do_free(action);
    }
    do_ret_list_free(temp);
    if (!err)
        retval = do_ret_list_static_new();
    for (i = 0; i < keys->count && !err; i++) {
        if (!RUNNING_) {
            err = 1;
            break;
        }
        memcpy(&document_key0, keys->list[i], sizeof(document_key0));
        status = do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL);
        switch (status) {
            case DO_OK:
                do_ret_list_static_add(retval, &document.data, document.size);
                break;
            case DO_KEY_NOT_FOUND:
                do_ret_list_static_add(retval, &document_key0, sizeof(document_key0));
                break;
            default:
                err = 1;
        }
    }
    if (err) {
        if (retval)
            do_ret_list_free(retval);
        retval = NULL;
    }
    do_sort_list_free(keys);
    return retval;
}
static void get_docs_from_protocol(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    char *value, *ch;
    BTI_LONG date_start;
    BTI_LONG date_end;
    do_ret_list_t *doc_list;
    do_list_t *key_list;
    do_alias_t *alias;
    document_key0_t document_key0;
    int err;
    int indx;

    alias = domino_alias_new(sync_info->opt->alias_name);
    if (!alias) {
        do_data_set_err(out, "cant create alias db");
        return;
    }
    if (!do_alias_open(alias, 0, DOCUMENT_DB)) {
        do_data_set_err(out, "cant open db");
        do_alias_free(alias);
        return;
    }

    key_list = do_list_new(TRUE);
    err = FALSE;
    indx = 0;
    for (; indx < param_count(param); indx++) {
        if (param_type(param, indx) == do_lx_cdate)
            break;
        if (param_type(param, indx) != do_lx_cstring) {
            err = TRUE;
            break;
        }
        value = param_value(param, indx).string;
        ch = strchr (value, '.');
        if (!ch) {
            err = TRUE;
            break;
        }
        *ch = '\0';
        do_text_set(alias, document_key0.dtype, value);
        value = ch + 1;
        do_text_set(alias, document_key0.sklad, value);
        do_list_add_alloc(key_list, &document_key0, sizeof(document_key0.dtype) + sizeof(document_key0.sklad));
        if (param_count(param) < indx + 1) {
            err = TRUE;
            break;
        }
    }
    if (!err && key_list->count) {
        if (param_count(param)  != indx + 2 ||
            param_type(param, indx) != do_lx_cdate ||
            param_type(param, indx + 1) != do_lx_cdate)
            err = TRUE;
        else {

            do_date_set(&date_start, param_value(param, indx).date);
            do_date_set(&date_end, param_value(param, indx + 1).date);
        }
    }
    if (err || !key_list->count) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        do_list_free(key_list);
        do_alias_free(alias);
        return;
    }

    doc_list = get_documents_do_list_from_protocol(alias, key_list, date_start, date_end);
    do_list_free(key_list);

    if (!doc_list || !add_documents_context(alias, doc_list, out))
        do_data_set_err(out, "Cant read db");
    else
        ZIP_IT;

    if (doc_list)
        do_ret_list_free(doc_list);
    do_alias_free(alias);
}

static void get_stocks(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{

    const char *sklad;
    do_extended_t *extended;


    if ( param_count(param) != 1 ||
         param_type(param, 0) != do_lx_cstring ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    sklad = param_value(param, 0).string;

    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_DATA, DO_DB_SKLAD, DO_DB_BARCODE, DO_DB_STOCK, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    do_ret_list_t *do_list_stock;
    do_ret_list_t *do_list_price;
    //do_ret_list_t *do_list_barcode;
    int indx_stock, indx_price;
    do_list_t *list;

    typedef struct PACK_STRUCT {
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_REAL price[15];
        BTI_BYTE tech_unit_rate[4];
        BTI_BYTE total_price;
        BTI_BYTE nprice[15];
    } ret_struct_t;
    ret_struct_t *price;
    stock_struct_t stock_st;
    int nretail_price;
    int nreg_price=11;
    int i;
    int local_unit;

    nretail_price = domino_nretail_price(alias, sklad);

    product_key0_t product_key0;
    do_text_set_empty(product_key0.code);
    local_unit = !strcmp(sklad, domino_unit());

    if (local_unit) {
        product_key0.base_parcel = 0;
        extended = do_extended_new(alias);
        do_extended_set_break_func(extended, break_func, NULL);
        do_extended_set_file_db(extended, DO_DB_PRODUCT);
        do_extended_set_key(extended, 0, &product_key0);
        do_extended_set_filter(extended, TRUE, 1, sizeof(product_key0.base_parcel));
        do_extended_set_filter_term(extended,
                                    0,
                                    DO_EXTENDED_TYPE_INTEGER,
                                    offsetof(product_struct_t, base_parcel),
                                    DO_EXTENDED_COMP_EQUAL,
                                    DO_EXTENDED_CONNECTION_NO,
                                    &product_key0.base_parcel);
        do_extended_set_fields(extended, 5,
                                do_size_and_offset(product_struct_t, code),
                                do_size_and_offset(product_struct_t, price),
                                do_size_and_offset(product_struct_t, tech_unit_rate),
                                do_size_and_offset(product_struct_t, total_price),
                                do_size_and_offset(product_struct_t, nprice)
                                            );
        do_list_price = do_extended_get(extended, -1);
        do_extended_free(extended);

        if (!do_list_price) {
            do_data_set_err(out, "Cant read db");
            do_alias_free(alias);
            return;
        }
    }
    else {
        do_list_price = do_ret_list_static_new();
        //do_list_barcode = do_ret_list_static_new();
    }
    do_list_stock = replic_select_stocks(alias, sklad, break_func);
    if (!do_list_stock) {
        do_data_set_err(out, "Cant read db");
        do_ret_list_free(do_list_price);
        do_alias_free(alias);
        return;
    }
    if (sizeof(stock_struct_t) > sizeof(*price)) {
        do_log(LOG_ERR, "Logical error sizeof(stock_struct_t) > sizeof(*price) %d, %d", sizeof(stock_struct_t), sizeof(*price));
        do_data_set_err(out, "Logical error");
        do_ret_list_free(do_list_price);
        do_ret_list_free(do_list_stock);
        do_alias_free(alias);
        return;
    }
    const char separator = do_param(DO_PARAM_PRODUCT_BASE_PARCEL_SEPARATOR)[0];
    int base_len = do_param_int(DO_PARAM_PRODUCT_BASE_CODE_LENGTH_DEPRECATE);
    int status;

    stock_struct_t *stock;
    barcode_key1_t barcode_key1;
    product_rec_t product;
    int err = 0;
    double coef,retail_price,reg_price;
    //int j;

    status = DO_OK;
    // sad but true, quarantine off
    /*for (indx_stock = 0; indx_stock < do_ret_list_count(do_list_stock); indx_stock++) {
        if (!RUNNING_) {
            err = 1;
            break;
        }
        stock = do_ret_list_item_data(do_list_stock, indx_stock);
        for ( i = 0; i < 6; i++ )
           if ( stock->nquant[i] == 3 || stock->nquant[i] == 4 )
               do_pdecimal_set(stock->quant[i], 8, 3, 0);
    }*/

    for (indx_stock = 0; indx_stock < do_ret_list_count(do_list_stock); indx_stock++) {
        if (!RUNNING_) {
            err = 1;
            break;
        }
        stock = do_ret_list_item_data(do_list_stock, indx_stock);

        if ( local_unit &&
              (do_stock_struct_quant(alias, stock, DO_CONST_QUANT_REST) +
                do_stock_struct_quant(alias, stock, DO_CONST_QUANT_CRNTSALE)
              > 0 ) &&
              (stock->code[base_len] == separator) ) {
            do_cpy(barcode_key1.code, stock->code);
            status = do_barcode_key1(alias, &barcode_key1, DO_GET_EQUAL);
            if (status == DO_ERROR) {
                err = 1;
                break;
            }
            if (status == DO_OK) {
                do_cpy(product_key0.code, barcode_key1.code);
                product_key0.base_parcel = 1;
                status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
                if (status == DO_ERROR) {
                    err = 1;
                    break;
                }
                if (status == DO_KEY_NOT_FOUND) {
                    char *code = do_text(alias, barcode_key1.code);
                    do_log(LOG_ERR, "product with code \"%s\" not found", code);
                    do_free(code);
                    err = 1;
                    break;
                }
                coef = do_product_coef(alias, &product);
                do_stock_struct_quant_set(alias, stock,
                                    DO_CONST_QUANT_PRICE, do_product_price(alias, &product, nretail_price));
                do_stock_struct_quant_set(alias, stock,
                                    DO_CONST_QUANT_PRICE_FULL, do_product_price(alias, &product, nretail_price)*coef);

            }

        }
    }
    if (err) {
        if (status == DO_ERROR)
            do_data_set_err(out, "Read DB");
        else {
            do_log(LOG_ERR, "Break");
            do_data_set_err(out, "Break");
        }
        do_ret_list_free(do_list_price);
        do_ret_list_free(do_list_stock);
        do_alias_free(alias);
        return;
    }


    indx_stock = 0;
    list = do_list_new(0);
    for (indx_price = 0; indx_price < do_ret_list_count(do_list_price); indx_price++) {
        price = do_ret_list_item_data(do_list_price, indx_price);
        coef = do_pdecimal((unsigned char*)&price->tech_unit_rate, 4, 3);
        if (coef < 1)
            coef = 1;
        retail_price = 0; reg_price = 0;
        for (i = 0; i < price->total_price; i++) {
            if ( price->nprice[i] == nretail_price ) {
                retail_price = price->price[i];
            }
            if ( price->nprice[i] == nreg_price ) {
                reg_price = price->price[i];
            }
        };

        if (retail_price > 0 || reg_price > 0) {
            for (; indx_stock < do_ret_list_count(do_list_stock) &&
                     strncmp(price->code, ((stock_struct_t*)do_ret_list_item_data(do_list_stock, indx_stock))->code,
                   DOMINO_FIELD_CODE_SIZE) > 0
                 ; indx_stock++)
                do_list_add(list, do_ret_list_item_data(do_list_stock, indx_stock));

            if ( indx_stock < do_ret_list_count(do_list_stock) &&
                !strncmp(price->code, ((stock_struct_t*)do_ret_list_item_data(do_list_stock, indx_stock))->code,
                           DOMINO_FIELD_CODE_SIZE) ) {
                        do_stock_struct_quant_set(alias, do_ret_list_item_data(do_list_stock, indx_stock),
                                            DO_CONST_QUANT_PRICE, retail_price);
                        do_stock_struct_quant_set(alias, do_ret_list_item_data(do_list_stock, indx_stock),
                                            DO_CONST_QUANT_PRICE_FULL, retail_price*coef);
                        do_stock_struct_quant_set(alias, do_ret_list_item_data(do_list_stock, indx_stock),
                                            5, reg_price);
                        do_list_add(list, do_ret_list_item_data(do_list_stock, indx_stock));
                        indx_stock++;
            }
            else {
                        do_text_set(alias, stock_st.sklad, sklad);
                        do_cpy(stock_st.code, price->code);
                        memset(stock_st.nquant, 0, sizeof(stock_st.nquant));
                        memset(stock_st.quant, 0, sizeof(stock_st.quant));
                        do_stock_struct_quant_set(alias, &stock_st,
                                            DO_CONST_QUANT_PRICE, retail_price);
                        do_stock_struct_quant_set(alias, &stock_st,
                                            DO_CONST_QUANT_PRICE_FULL, retail_price*coef);
                        do_stock_struct_quant_set(alias, &stock_st,
                                            5, reg_price);
                        memcpy(price, &stock_st, sizeof(stock_st));
                        do_list_add(list, price);
            }
        }
        else {
            for (; indx_stock < do_ret_list_count(do_list_stock) &&
                     strncmp(price->code, ((stock_struct_t*)do_ret_list_item_data(do_list_stock, indx_stock))->code,
                   DOMINO_FIELD_CODE_SIZE) > 0
                 ; indx_stock++)
                do_list_add(list, do_ret_list_item_data(do_list_stock, indx_stock));
            if ( indx_stock < do_ret_list_count(do_list_stock) &&
                !strncmp(price->code, ((stock_struct_t*)do_ret_list_item_data(do_list_stock, indx_stock))->code,
                           DOMINO_FIELD_CODE_SIZE) ) {
                        do_stock_struct_quant_set(alias, do_ret_list_item_data(do_list_stock, indx_stock),
                                            DO_CONST_QUANT_PRICE, 0);
                        do_stock_struct_quant_set(alias, do_ret_list_item_data(do_list_stock, indx_stock),
                                            DO_CONST_QUANT_PRICE_FULL, 0);
                        do_stock_struct_quant_set(alias, do_ret_list_item_data(do_list_stock, indx_stock),
                                            5, 0);
                        do_list_add(list, do_ret_list_item_data(do_list_stock, indx_stock));
                        indx_stock++;
            }
        }
    }
    for (; indx_stock < do_ret_list_count(do_list_stock); indx_stock++ )
        do_list_add(list, do_ret_list_item_data(do_list_stock, indx_stock));

    do_data_clear(out);
    size_t size;
    void *crnt;
    do_size_t *psize;

    size = list->count*(sizeof(*psize) + sizeof(stock_struct_t)) + sizeof(*psize);
    if (!do_data_realloc(out, size))
        do_data_set_errf(out, "Could not allocate memory %d byte", size);
    else {
        crnt = do_data_get(out);
        psize = crnt;
        crnt += sizeof(*psize);
        *psize = list->count;
        for (i = 0; i < list->count; i++) {
            psize = crnt;
            crnt += sizeof(*psize);
            *psize = sizeof(stock_struct_t);
            memcpy(crnt, list->list[i], sizeof(stock_struct_t));
            crnt += sizeof(stock_struct_t);
        }
    }

    do_list_free(list);
    do_ret_list_free(do_list_price);
    do_ret_list_free(do_list_stock);
    do_alias_free(alias);
    ZIP_IT;
}
#ifndef DOMINO78
static void get_checks(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    int date_start, date_end;
    char sklad[1024];
    int i, j, k;

    if ( param_count(param) != 3 ||
         param_type(param, 0) != do_lx_cstring ||
         param_type(param, 1) != do_lx_cdate ||
         param_type(param, 2) != do_lx_cdate )     {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    strcpy(sklad, param_value(param, 0).string);

    date_start = do_datetosql(param_value(param, 1).date);
    date_end = do_datetosql(param_value(param, 2).date);

    do_alias_t *alias;

    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_SHIFT, DO_DB_CHECKSUM, DO_DB_CHECK,
                                           DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }


    shift_struct_t *shift_st;
    int err = FALSE;
    void *crnt;
    size_t size;
    do_size_t *psize;
    do_ret_list_t *do_list_shift;
    do_ret_list_t **l_content;

    do_list_shift = replic_select_shifts(alias, sklad, date_start, date_end, break_func);
    if (!do_list_shift) {
        do_data_set_err(out, "Cant read db");
        err = TRUE;
    }

    l_content = (do_ret_list_t **)do_malloc0(sizeof(do_ret_list_t *)*2*do_ret_list_count(do_list_shift));
    for (i = 0; !err && i < do_ret_list_count(do_list_shift); i++) {
        shift_st = do_ret_list_item_data(do_list_shift, i);
        l_content[i*2] = replic_select_shift_checksum(alias, shift_st, break_func);
        if (!l_content[i*2]) {
             do_data_set_err(out, "Cant read db");
             err = TRUE;
             break;
        }
        l_content[i*2 + 1] = replic_select_shift_check(alias, shift_st, break_func);
        if (!l_content[i*2 + 1]) {
             do_data_set_err(out, "Cant read db");
             err = TRUE;
             break;
        }
    }
    if (!err) {
        size = sizeof(*psize);
        for (i = 0; i < do_ret_list_count(do_list_shift); i++)
            size += sizeof(*psize) + do_ret_list_item_len(do_list_shift, i);
        for (j = 0; j < do_ret_list_count(do_list_shift)*2; j++) {
            size += sizeof(*psize);
            for (i = 0; i < do_ret_list_count(l_content[j]); i++)
                size += sizeof(*psize) + do_ret_list_item_len(l_content[j], i);
        }
        if (!do_data_alloc(out, size))
            do_data_set_errf(out, "Cant alloc %d byte", size);
        else {
            crnt = do_data_get(out);
            psize = crnt;
            crnt += sizeof(*psize);
            *psize = do_ret_list_count(do_list_shift);
            for (i = 0; i < do_ret_list_count(do_list_shift); i++) {
                psize = crnt;
                crnt += sizeof(*psize);
                *psize = do_ret_list_item_len(do_list_shift, i);
                memcpy(crnt, do_ret_list_item_data(do_list_shift, i), do_ret_list_item_len(do_list_shift, i));
                crnt +=do_ret_list_item_len(do_list_shift, i);
                for (j = 0; j < 2; j++) {
                    psize = crnt;
                    crnt += sizeof(*psize);
                    *psize = do_ret_list_count(l_content[i*2 + j]);
                    for (k = 0; k < do_ret_list_count(l_content[i*2 + j]); k++) {
                        psize = crnt;
                        crnt += sizeof(*psize);
                        *psize = do_ret_list_item_len(l_content[i*2 + j], k);
                        memcpy(crnt, do_ret_list_item_data(l_content[i*2 + j], k), do_ret_list_item_len(l_content[i*2 + j], k));
                        crnt +=do_ret_list_item_len(l_content[i*2 + j], k);
                    }
                }
            }
        }
    }
    for (i = 0; i < do_ret_list_count(do_list_shift)*2; i++)
        if (l_content[i])
            do_ret_list_free(l_content[i]);
    do_free(l_content);
    if (do_list_shift)
        do_ret_list_free(do_list_shift);
    do_alias_free(alias);
    ZIP_IT;
}
static void get_realization(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    int date_start, date_end;
    char sklad[1024];

    if ( param_count(param) != 3 ||
         param_type(param, 0) != do_lx_cstring ||
         param_type(param, 1) != do_lx_cdate ||
         param_type(param, 2) != do_lx_cdate )     {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    strcpy(sklad, param_value(param, 0).string);

    date_start = do_datetosql(param_value(param, 1).date);
    date_end = do_datetosql(param_value(param, 2).date);

    do_alias_t *alias;

    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, FALSE, DO_DB_REALIZATION,
                                           DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    do_ret_list_t *list;

    list = replic_select_realization(alias, sklad, date_start, date_end, break_func);
    do_data_clear(out);
    if (!list)
        do_data_set_err(out, "Cant read db");
    else {
        do_list_to_data(out, list);
        do_ret_list_free(list);
    }
    do_alias_free(alias);
    ZIP_IT;
}
#endif
static void get_protocol(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{

    BTI_LONG date_start, date_end;
    //void *sad;

    if ( param_count(param) != 2 ||
         param_type(param, 0) != do_lx_cdate ||
         param_type(param, 1) != do_lx_cdate ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    do_date_set(&date_start, param_value(param, 0).date);
    do_date_set(&date_end, param_value(param, 0).date);
    do_alias_t *alias;

    const file_db_t file_id = DO_DB_PROTOCOL;

    if ((alias = create_alias(sync_info->opt->alias_name, file_id, out)) == NULL)
        return;

    //sad = &alias;
    protocol_key1_t protocol_key1;
    do_extended_t *extended;
    protocol_key1.time = 0;

    extended = do_extended_new(alias);
    do_extended_set_break_func(extended, break_func, NULL);
    do_extended_set_file_db(extended, file_id);
    do_extended_set_key(extended, 1, &protocol_key1);

    do_extended_set_filter(extended, TRUE, 1, sizeof(protocol_key1.date));
    protocol_key1.date = date_start;
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(protocol_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &protocol_key1.date);
    protocol_key1.date = date_end;
    do_extended_set_fields_full_record(extended, sizeof(partner_struct_t));

    do_data_clear(out);
    get_data(extended, out);

    do_extended_free(extended);
    do_alias_free(alias);
}

static void put_product(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out)
{

    if ( param_count(param) != 1 ||
         param_type(param, 0) != do_lx_cstring ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PROTOCOL, DO_DB_PRODUCT, DO_DB_PRODUCT_VIEW, DO_DB_PRODUCT_DATA, DO_DB_BARCODE, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }

    do_data_clear(out);
    if (!replic_product(alias, param_value(param, 0).string, do_data_get(in), do_data_size(in), break_func))
        do_data_set_err(out, "Error replic product");
    else
        do_data_set_str(out, DO_EXCH_OK);
    do_alias_free(alias);
}
static void put_partner(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out)
{

    if ( param_count(param) != 1 ||
         param_type(param, 0) != do_lx_cstring ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    char *ch;
    int code;
    char *g_code;
    g_code = param_value(param, 0).string;
    ch = strchr(param_value(param, 0).string, '.');
    if (!ch) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    *ch = '\0';
    ch++;
    code = atoi(ch);

    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_PROTOCOL, DO_DB_PARTNER,
#ifdef USE_DB_PARNER_VIEW
     DO_DB_PARTNER_VIEW,
#endif
     DO_DB_PARTNER_DATA, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }

    do_data_clear(out);
    if (!replic_partner(alias, g_code, code, do_data_get(in), do_data_size(in), break_func))
        do_data_set_err(out, "Error replic partner");
    else
        do_data_set_str(out, DO_EXCH_OK);
    do_alias_free(alias);
}
static void put_document_order(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out)
{
    if ( param_count(param) != 1 ||
         param_type(param, 0) != do_lx_cstring ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    char *ch;
    int line;
    char *dtype;
    char *sklad;
    char *document;

    dtype = param_value(param, 0).string;
    ch = strchr(param_value(param, 0).string, '.');
    if (!ch) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    *ch = '\0';
    ch++;
    sklad = ch;
    ch = strchr(ch, '.');
    if (!ch) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    *ch = '\0';
    ch++;
    document = ch;
    ch = strchr(ch, '.');
    if (!ch) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    *ch = '\0';
    ch++;
    line = atoi(ch);

    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_DOCUMENT_ORDER, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }

    do_data_clear(out);
    int changed;
    if (!replic_document_order(alias, dtype, sklad, document, line, do_data_get(in), do_data_size(in), &changed, break_func))
        do_data_set_err(out, "Error replic document order");
    else
        do_data_set_str(out, DO_EXCH_OK);
    do_alias_free(alias);
}
static void put_document(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out)
{
    if ( param_count(param) != 1 ||
         param_type(param, 0) != do_lx_cstring ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }

    char *ch;
    //char *dtype;
    //char *sklad;
    //char *document;

    //dtype = param_value(param, 0).string;
    ch = strchr(param_value(param, 0).string, '.');
    if (!ch) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    *ch = '\0';
    ch++;
    //sklad = ch;
    ch = strchr(ch, '.');
    if (!ch) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    *ch = '\0';
    ch++;
    //document = ch;

    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");

    alias = domino_alias_new(sync_info->opt->alias_name);
    if (!alias) {
        do_data_set_err(out, error_msg[ERROR_CANT_CREATE_ALIAS]);
        return;
    }
    if (!do_alias_open(alias, 0, DOCUMENT_DB)) {
        do_data_set_err(out, error_msg[ERROR_CANT_OPEN_DB]);
        do_alias_free(alias);
        return;
    }
    do_data_clear(out);
    if (!replic_document(alias, do_data_get(in), do_data_size(in), break_func))
        do_data_set_err(out, "Error replic document");
    else
        do_data_set_str(out, DO_EXCH_OK);
    do_alias_free(alias);
}
#ifndef DOMINO78
static void make_realization(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    int date_start, date_end;
    char sklad[1024];

    if ( param_count(param) != 3 ||
         param_type(param, 0) != do_lx_cstring ||
         param_type(param, 1) != do_lx_cdate ||
         param_type(param, 2) != do_lx_cdate )     {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    strcpy(sklad, param_value(param, 0).string);

    date_start = do_datetosql(param_value(param, 1).date);
    date_end = do_datetosql(param_value(param, 2).date);

    do_alias_t *alias;

    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, FALSE, DO_DB_REALIZATION,
                                           DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    if ( !util_make_realization(alias, sklad, date_start, date_end, break_func) )
        do_data_set_err(out, "cant make realization");
    else
        do_data_set_str(out, "Ok");
    do_alias_free(alias);
}
#endif
static int get_last_protocol_last_index(do_alias_t *alias)
{
    product_data_key0_t product_data_key0, key;
    do_text_set(alias, product_data_key0.code, "00001");
    product_data_key0.type = 77;
    product_data_key0.number = INT_MAX;

    do_cpy(key.code,product_data_key0.code)

    if ( do_product_data_key0(alias, &product_data_key0, DO_GET_LT) != DO_OK ||
         do_cmp(product_data_key0.code,key.code) ||
         product_data_key0.type != 77 )
        return 0;
    else
        return product_data_key0.number;
}

static void get_updated_stocks(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{

    do_string_list_t *skip_stores=NULL;
    BTI_LONG last_index;


    if ( param_count(param) < 1 ||
         param_type(param, 0) != do_lx_cinteger ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    last_index = param_value(param, 0).integer;
    if ( param_count(param) == 2 ) {
        if ( param_type(param, 1) != do_lx_cstring ) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
        skip_stores = do_string_list_new_from_csv(param_value(param, 1).string);
    }

    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_DATA, DO_DB_SKLAD, DO_DB_BARCODE, DO_DB_STOCK, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        if ( skip_stores )
            do_string_list_free(skip_stores);
        return;
    }
    if ( !last_index ) {
        last_index = get_last_protocol_last_index(alias);
        do_data_clear(out);
        size_t size;
        void *crnt;
        do_size_t *psize;
        BTI_LONG *index;

        size = sizeof(last_index) + sizeof(*psize);
        if (!do_data_realloc(out, size))
            do_data_set_errf(out, "Could not allocate memory %d byte", size);
        else {
            crnt = do_data_get(out);
            index = crnt;
            *index = last_index;
            crnt += sizeof(*psize);
            psize = crnt;
            *psize = 0;
        }
        do_alias_free(alias);
        if ( skip_stores )
            do_string_list_free(skip_stores);
        return;
    }

    product_data_rec_t product_data;
    product_data_key0_t product_data_key0, key;
    stock_key0_t stock_key0;
    stock_rec_t stock;
    int status;
    do_list_t *list;

    list = do_list_new(TRUE);

    do_text_set(alias, product_data_key0.code, "00001");
    product_data_key0.type = 77;
    product_data_key0.number = last_index;
    do_cpy(key.code,product_data_key0.code);

    status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_LT);
    while ( status == DO_OK ) {
        if ( do_cmp(product_data_key0.code,key.code) ||
            product_data_key0.type != 77 ) break;
        if (!RUNNING) {
            status = DO_ERROR;
            break;
        }
        last_index = product_data_key0.number;

        if ( do_product_data_param_int(alias, &product_data, "1") == 5 ) {
            char *store;
            int indx;

            store = do_product_data_param(alias, &product_data, "5");
            if ( !skip_stores || !do_string_list_find(skip_stores, &indx, store) ) {
                char *code;

                code = do_product_data_param(alias, &product_data, "2");
                do_log(LOG_INFO, "update stock %s %s", store, code);//!!
                do_text_set(alias, stock_key0.code, code);
                do_text_set(alias, stock_key0.sklad, store);

                status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
                if ( status == DO_KEY_NOT_FOUND ) {
                    do_cpy(stock.data.sklad,stock_key0.sklad);
                    do_cpy(stock.data.code,stock_key0.code);
                    do_stock_quant_clear(alias, &stock);
                }
                do_list_add_alloc(list, &stock.data, sizeof(stock_struct_t));
                do_free(code);
            }
            do_free(store);
        }
        if ( status == DO_ERROR )
            break;

        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        do_data_set_err(out, "cant read db");
        do_alias_free(alias);
        if ( skip_stores )
            do_string_list_free(skip_stores);
        do_list_free(list);
        return;
    }

    do_data_clear(out);
    size_t size;
    void *crnt;
    do_size_t *psize;

    size = list->count*(sizeof(*psize) + sizeof(stock_struct_t)) + sizeof(*psize) + sizeof(last_index);
    if (!do_data_realloc(out, size))
        do_data_set_errf(out, "Could not allocate memory %d byte", size);
    else {
        BTI_LONG *index;
        int i;
        crnt = do_data_get(out);
        index = crnt;
        crnt += sizeof(*index);
        *index = last_index;
        psize = crnt;
        crnt += sizeof(*psize);
        *psize = list->count;
        for (i = 0; i < list->count; i++) {
            psize = crnt;
            crnt += sizeof(*psize);
            *psize = sizeof(stock_struct_t);
            memcpy(crnt, list->list[i], sizeof(stock_struct_t));
            crnt += sizeof(stock_struct_t);
        }
    }

    do_list_free(list);
    do_alias_free(alias);
    ZIP_IT;
}
/*!!
static void get_updated_stocks(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{

    do_string_list_t *skip_stores=NULL;
    BTI_LONG last_index;


    if ( param_count(param) < 1 ||
         param_type(param, 0) != do_lx_cinteger ) {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    last_index = param_value(param, 0).integer;
    if ( param_count(param) == 2 ) {
        if ( param_type(param, 1) != do_lx_cstring ) {
            do_data_set_err(out, "Syntax error. Use ? for help");
            return;
        }
        skip_stores = do_string_list_new_from_csv(param_value(param, 1).string);
    }

    do_alias_t *alias;
    alias = domino_alias_new(sync_info->opt->alias_name);
    if (!alias || !do_alias_open(alias, 0, DO_DB_PRODUCT, DO_DB_PRODUCT_DATA, DO_DB_SKLAD, DO_DB_BARCODE, DO_DB_STOCK, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        if ( skip_stores )
            do_string_list_free(skip_stores);
        return;
    }
    if ( !last_index ) {
        last_index = get_last_protocol_last_index(alias);
        do_data_clear(out);
        size_t size;
        void *crnt;
        do_size_t *psize;
        BTI_LONG *index;

        size = sizeof(last_index) + sizeof(*psize);
        if (!do_data_realloc(out, size))
            do_data_set_errf(out, "Could not allocate memory %d byte", size);
        else {
            crnt = do_data_get(out);
            index = crnt;
            *index = last_index;
            crnt += sizeof(*psize);
            psize = crnt;
            *psize = 0;
        }
        do_alias_free(alias);
        if ( skip_stores )
            do_string_list_free(skip_stores);
        return;
    }

    product_data_rec_t product_data;
    product_data_key0_t product_data_key0, key;
    stock_key0_t stock_key0;
    int status;
    do_list_t *list;

    list = do_list_new(TRUE);

    do_text_set(alias, product_data_key0.code, "00001");
    product_data_key0.type = 77;
    product_data_key0.number = last_index;
    do_cpy(key.code,product_data_key0.code);

    status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_LT);
    while ( status == DO_OK ) {
        if ( do_cmp(product_data_key0.code,key.code) ||
            product_data_key0.type != 77 ) break;
        if (!RUNNING) {
            status = DO_ERROR;
            break;
        }
        last_index = product_data_key0.number;

        if ( do_product_data_param_int(alias, &product_data, "1") == 5 ) {
            char *store;
            int indx;

            store = do_product_data_param(alias, &product_data, "5");
            if ( !skip_stores || !do_string_list_find(skip_stores, &indx, store) ) {
                char *code;

                code = do_product_data_param(alias, &product_data, "2");
                do_log(LOG_INFO, "update stock %s %s", store, code);//!!
                do_text_set(alias, stock_key0.code, code);
                do_text_set(alias, stock_key0.sklad, store);
                d
                if ( !replic_select_stock(alias, &stock_key0, list, NULL) )
                    status = DO_ERROR;
                do_free(code);
            }
            do_free(store);
        }
        if ( status == DO_ERROR )
            break;

        status = do_product_data_get0(alias, &product_data, &product_data_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        do_data_set_err(out, "cant read db");
        do_alias_free(alias);
        if ( skip_stores )
            do_string_list_free(skip_stores);
        do_list_free(list);
        return;
    }

    do_data_clear(out);
    size_t size;
    void *crnt;
    do_size_t *psize;

    size = list->count*(sizeof(*psize) + sizeof(stock_struct_t)) + sizeof(*psize) + sizeof(last_index);
    if (!do_data_realloc(out, size))
        do_data_set_errf(out, "Could not allocate memory %d byte", size);
    else {
        BTI_LONG *index;
        int i;
        crnt = do_data_get(out);
        index = crnt;
        crnt += sizeof(*index);
        *index = last_index;
        psize = crnt;
        crnt += sizeof(*psize);
        *psize = list->count;
        for (i = 0; i < list->count; i++) {
            psize = crnt;
            crnt += sizeof(*psize);
            *psize = sizeof(stock_struct_t);
            memcpy(crnt, list->list[i], sizeof(stock_struct_t));
            crnt += sizeof(stock_struct_t);
        }
    }

    do_list_free(list);
    do_alias_free(alias);
    ZIP_IT;
}
*/
static void put_stock(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out)
{

    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_STOCK, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }

    do_data_clear(out);
    if (!replic_stock(alias, do_data_get(in), do_data_size(in), break_func))
        do_data_set_err(out, "Error replic stock");
    else
        do_data_set_str(out, DO_EXCH_OK);
    do_alias_free(alias);
}

static void put_stocks(sync_info_t *sync_info, do_list_t *param, do_data_t *in, do_data_t *out)
{

    do_alias_t *alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, 0, DO_DB_STOCK, DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }

    do_data_clear(out);
    if (!replic_stock_list(alias, do_data_get(in), do_data_size(in), break_func))
        do_data_set_err(out, "Error replic stocks");
    else
        do_data_set_str(out, DO_EXCH_OK);
    do_alias_free(alias);
}

static void get_balance41(sync_info_t *sync_info, do_list_t *param, do_data_t *out)
{
    struct tm date_end;
    char sklad[1024];

    if ( param_count(param) != 2 ||
         param_type(param, 0) != do_lx_cstring ||
         param_type(param, 1) != do_lx_cdate )     {
        do_data_set_err(out, "Syntax error. Use ? for help");
        return;
    }
    strcpy(sklad, param_value(param, 0).string);

    date_end = param_value(param, 1).date;

    do_alias_t *alias;

    alias = domino_alias_new(sync_info->opt->alias_name);
    do_data_set_str(out, "");
    if (!alias || !do_alias_open(alias, FALSE, DO_DB_DOCUMENT,
                                           DO_DB_END)) {
        do_data_set_err(out, "cant open db");
        if (alias)
            do_alias_free(alias);
        return;
    }
    double balance;
    int count;
    do_data_clear(out);
    if ( !get_balance_41(alias, sklad, date_end, &balance, &count, break_func) )
        do_data_set_err(out, "cant read balance");
    else {
        do_data_add(out, &balance, sizeof(balance));
        do_data_add(out, &count, sizeof(count));
    }
    do_alias_free(alias);
}

//#endif
