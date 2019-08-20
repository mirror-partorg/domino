#include "grlspl.h"
#include <domino.h>
#include <glib.h>
#include "../misc/send_error.h"
#include "../misc/utf8util.h"
#include <time.h>

#define DEFAULT_CONFIG_NAME "grlspl.xml"

enum {
    ITEM_MNN = 0,
    ITEM_TRADE_NAME,
    ITEM_PACKING,
    ITEM_MANUFACTOR,
    ITEM_PAKING_QUANTITY,
    ITEM_PRICE,
    ITEM_PRIMARY_PACKING,
    ITEM_NUMBER_RU,
    ITEM_ORDER_DATE,
    ITEM_ORDER_NUMBER,
    ITEM_BARCODE,
    ITEM_NO,
};

typedef struct {
    int    sheet_number;
    int    first_row;
    int    title_row;
    int    title_column;
    char   *title_pattern;
    struct {
        int column;
        char *logical;
        char *pattern;
    } item[ITEM_NO];
} config_t;

static const char *item_name[ITEM_NO] =
{ "MNN", "Trade Name", "Packing", "Manufactor", "Packing quantity", "Price",
  "Primary packaging", "Number RU", "Order date", "Order number", "Barcode" };

static int read_config(config_t *cf, const char *configfilename);
static void clear_config(config_t *cf);
static void rz_clear(rz_info_t *rz);
static GMatchInfo *get_match_info(const char *str, const char *pattern);
static char *get_string(GIConv conv, const char *str, config_t *cf, int item_n);
static int get_integer(const char *str, config_t *cf, int item_n);
static double get_double(const char *str, config_t *cf, int item_n);
static int get_boolean(const char *str, config_t *cf, int item_n);
static time_t get_time(const char *str, config_t *cf, int item_n);
static char *get_title(const char *str, config_t *cf);
static int clear_grlspl(do_alias_t *alias);
static int rz_insert(do_alias_t *alias, rz_info_t *rz_info);
static int rz_insert_document(do_alias_t *alias, document_order_rec_t *document_order);

int grlspl_load_from_xls(const char *alias_name,
                         const char *filename,
                         const char *configfilename,
                         int clear,
                         int use_utf8,
                         int info,
                         int sendmessage)
{
    do_xls_t *xls = NULL;
    int retval;
    config_t cf;
    rz_info_t rz;
    do_alias_t *alias = NULL;
    int indx;
    int proc;
    int p;
    GIConv conv;

    retval = TRUE;
    conv = do_utf8_make_compatibility_cp866_init();

    if ( !read_config(&cf, configfilename) ) {
        retval = FALSE;
    }
    

    if ( retval ) {
        alias = domino_alias_new(alias_name);
        if ( !alias ) {
            retval = FALSE;
        }
    }
    if ( retval && !do_alias_open(alias, TRUE) ) {
        retval = FALSE;
    }

    /*do_log(LOG_INFO, "Clear price limit list");
    retval = clear_grlspl(alias);//!!
    return 0;*/

    do_log(LOG_INFO, "%s", filename);
    if ( retval && (xls = do_xls_new(filename, TRUE)) == NULL ) {
        do_log(LOG_ERR, "File \"%s\" not correct", filename);
        retval = FALSE;
    }
    if ( retval && do_xls_worksheet_count(xls) < cf.sheet_number ) {
        do_log(LOG_ERR, "Sheet \"%d\" not existed", cf.sheet_number);
        retval = FALSE;
    }
    if ( retval && do_xls_row_count(xls, cf.sheet_number-1) == 0 ) {
        do_log(LOG_ERR, "Sheet \"%d\" is empty", cf.sheet_number);
        retval = FALSE;
    }
    if ( retval )
        retval = do_alias_tran_begin(alias);
    if ( retval && clear ) {
        if ( info )
            do_log(LOG_INFO, "Clear price limit list");
        retval = clear_grlspl(alias);
        if ( info && retval ) {
            do_log(LOG_INFO, "Price limit clean");
        }
    }

#define GET_STR(a) get_string(conv, do_xls_cell_str(xls, cf.sheet_number-1, indx, cf.item[a].column-1), &cf, a)
#define CHECK_STR(a,b) { if ( a[0] == '\0' ) {do_log(LOG_ERR, "Error in string %d \"%s\" is empty", indx+1, item_name[b]); retval=FALSE; break;} }
#define GET_INT(a) get_integer(do_xls_cell_str(xls, cf.sheet_number-1, indx, cf.item[a].column-1), &cf, a)
#define GET_DOUBLE(a) get_double(do_xls_cell_str(xls, cf.sheet_number-1, indx, cf.item[a].column-1), &cf, a)
#define GET_BOOLEAN(a) get_boolean(do_xls_cell_str(xls, cf.sheet_number-1, indx, cf.item[a].column-1), &cf, a)
#define GET_TIME(a) get_time(do_xls_cell_str(xls, cf.sheet_number-1, indx, cf.item[a].column-1), &cf, a)
#define CHECK_NUMBER(a,b) { if ( a <= 0 ) {do_log(LOG_ERR, "Error in string %d \"%s\" is empty or negative", indx+1, item_name[b]); retval=FALSE;  break;} }
#define CHECK_TIME(a,b) { if ( a == -1  ) {do_log(LOG_ERR, "Error in string %d \"%s\" is empty or error", indx+1, item_name[b]); retval=FALSE;  break;} }

    memset(&rz, 0, sizeof(rz));
    proc = 0;
    if ( info && retval )
        do_log(LOG_INFO, "Start load price limit");

    for ( indx = cf.first_row - 1;
          retval && indx < do_xls_row_count(xls, cf.sheet_number-1);
          indx++ ) {
        rz_clear(&rz);

        rz.mnn = GET_STR(ITEM_MNN);
        rz.trade_name = GET_STR(ITEM_TRADE_NAME);
        rz.pack_form = GET_STR(ITEM_PACKING);
        if ( rz.pack_form[0] == '\0' ) {
            do_log(LOG_WARNING, "Warning in string %d \"%s\" is empty. skip", indx+1, item_name[ITEM_PACKING]); 
            continue;
        }
        CHECK_STR(rz.pack_form, ITEM_PACKING);
        rz.manufactor = GET_STR(ITEM_MANUFACTOR);
        if ( rz.manufactor[0] == '\0' ) {
            do_log(LOG_WARNING, "Warning in string %d \"%s\" is empty. skip", indx+1, item_name[ITEM_MANUFACTOR]); 
            continue;
        }
        CHECK_STR(rz.manufactor, ITEM_MANUFACTOR);
        rz.pack_quant = GET_INT(ITEM_PAKING_QUANTITY);
        if ( !rz.pack_quant )
           rz.pack_quant = 1;
        rz.price = GET_DOUBLE(ITEM_PRICE);
        if ( rz.price <= 0 ) {
            do_log(LOG_WARNING, "Error in string %d \"%s\" is empty or negative %f", indx+1, item_name[ITEM_PRICE], rz.price); 
            continue;
        }
        CHECK_NUMBER(rz.price, ITEM_PRICE);
        rz.valuta = 1;
        rz.unpack = GET_BOOLEAN(ITEM_PRIMARY_PACKING);
        rz.order.time = GET_TIME(ITEM_ORDER_DATE);
        CHECK_TIME(rz.order.time, ITEM_ORDER_DATE);
        rz.order.number = GET_STR(ITEM_ORDER_NUMBER);
        //!!CHECK_STR(rz.order.number, ITEM_ORDER_NUMBER);
        rz.barcode = GET_STR(ITEM_BARCODE);
        if ( rz.barcode[0] == '\0' ) {
            do_free(rz.barcode);
            rz.barcode = do_strdup("0");
        }
        if ( rz.trade_name[0] == '\0' || rz.mnn[0]=='\0' ) {
            if ( rz.mnn[0]=='\0' ) 
                 do_log(LOG_WARNING, "Warning in string %d \"%s\" is empty.", indx+1, item_name[ITEM_MNN]); 
            if ( rz.trade_name[0]=='\0' ) 
                 do_log(LOG_WARNING, "Warning in string %d \"%s\" is empty.", indx+1, item_name[ITEM_TRADE_NAME]);
            product_key0_t product_key0;
            product_rec_t product;
            barcode_key0_t barcode_key0;
            barcode_rec_t barcode;
            int status;
            do_text_set(alias, barcode_key0.barcode, rz.barcode);
            status = do_barcode_get0(alias, &barcode, &barcode_key0, DO_GET_EQUAL);
            if ( status != DO_OK ) {
                 do_log(LOG_WARNING, "Warning in string %d barcode \"%s\" not found. skip", indx+1, rz.barcode);
                 continue;
            }
            do_cpy(product_key0.code, barcode.data.code);
            product_key0.base_parcel = 0;
            status = do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL);
            if ( status != DO_OK ) {
                 do_log(LOG_WARNING, "Warning in string %d product \"%s\" not found. skip", indx+1, do_text(alias, barcode.data.code));
                 continue;
            }
            rz.mnn = do_text(alias, product.data.name);
            rz.trade_name = do_text(alias, product.data.name);
            do_log(LOG_INFO, "Warning in string %d MNN and TRADE_NAME set \"%s\"", indx+1, rz.trade_name);
        }
        do_log(LOG_INFO, "%s %s", rz.mnn, rz.barcode);
        rz.link.type = RZ_INFO_LINK_NO;
        if ( !rz_insert(alias, &rz) ) {
            retval = FALSE;
            break;
        }
        if ( info && retval ) {
            p = ((indx - cf.first_row) / ((double)do_xls_row_count(xls, cf.sheet_number-1))) * 100;
            if ( p > proc ) {
                proc = p;
                do_log(LOG_INFO, "%d%% complect", proc);
            }
        }
    }
    if ( info && retval ) {
        do_log(LOG_INFO, "%d%% complect", 100);
        do_log(LOG_INFO, "parced %d record", do_xls_row_count(xls, cf.sheet_number-1) - cf.first_row + 1);
        if ( cf.title_row && cf.title_column )
            do_log(LOG_INFO, "%s", do_xls_cell_str(xls, cf.sheet_number-1, cf.title_row-1, cf.title_column-1));
    }
    if ( sendmessage ) {
        gchar *message;
        if ( retval ) {
            gchar *title;
            if ( cf.title_row && cf.title_column )
                title = get_title(do_xls_cell_str(xls, cf.sheet_number-1, cf.title_row-1, cf.title_column-1), &cf);
            else
                title = do_strdup("");

            message = g_strdup_printf("Price limit load\n%s\n%d records",
                                     title,
                                     do_xls_row_count(xls, cf.sheet_number-1) - cf.first_row + 1);
            do_free(title);
        }
        else
            message = g_strdup_printf("ERROR: Price limit not load!!!");

        send_error_message(message);
        g_free(message);
    }
    rz_clear(&rz);
    clear_config(&cf);
    if ( xls ) do_xls_free(xls);
    if ( alias ) {
        if ( retval )
            retval = do_alias_tran_end(alias);
        else
            do_alias_tran_abort(alias);
    }
    if ( alias ) do_alias_free(alias);
    do_utf8_make_compatibility_cp866_close(conv);
    return retval;
}
static int read_config(config_t *cf, const char *configfilename)
{
    do_config_t *config;
    int i;

    memset(cf, 0, sizeof(config_t));

    config = do_config_new("UTF-8", "grlspl");
    if ( !do_config_read(config, (configfilename) ? configfilename : DEFAULT_CONFIG_NAME, FALSE) )
        return FALSE;
    cf->sheet_number = do_config_get_property_int(config, "Excel", "sheet number");
    if ( !cf->sheet_number ) {
        do_log(LOG_ERR, "Undefine sheet number");
        do_config_free(config);
        return FALSE;
    }
    cf->first_row = do_config_get_property_int(config, "Excel", "first row");
    if ( !cf->first_row ) {
        do_log(LOG_ERR, "Undefine first row");
        do_config_free(config);
        return FALSE;
    }
    cf->title_row = do_config_get_property_int(config, "Excel", "title row");
    cf->title_column = do_config_get_property_int(config, "Excel", "title column");
    cf->title_pattern = do_strdup(do_config_get_property(config, "Excel", "title pattern"));
    for ( i = 0; i < ITEM_NO; i++ ) {
        const char *value;
        gchar *path = g_strdup_printf("%s/%s", "Excel", item_name[i]);

        cf->item[i].column = do_config_get_property_int(config, path, "column");
        if ( !cf->item[i].column ) {
            do_log(LOG_ERR, "undefine column for item \"%s\"", item_name[i]);
            g_free(path);
            do_config_free(config);
            return FALSE;
        }
        value = do_config_get_property(config, path, "logical");
        if ( value && value[0] != '\0' )
            cf->item[i].logical = do_strdup(value);
        value = do_config_get_property(config, path, "pattern");
        if ( value && value[0] != '\0' )
            cf->item[i].pattern = do_strdup(value);
    }
    do_config_free(config);
    return TRUE;
}
static void clear_config(config_t *cf)
{
    int i;
    if ( cf->title_pattern ) do_free(cf->title_pattern);
    for ( i = 0; i < ITEM_NO; i++ ) {
        if ( cf->item[i].logical ) do_free(cf->item[i].logical);
        if ( cf->item[i].pattern ) do_free(cf->item[i].pattern);
    }
}

static void rz_clear(rz_info_t *rz)
{
    if (rz->barcode) do_free(rz->barcode);
    if (rz->mnn) do_free(rz->mnn);
    if (rz->trade_name) do_free(rz->trade_name);
    if (rz->pack_form) do_free(rz->pack_form);
    if (rz->manufactor) do_free(rz->manufactor);
    if (rz->key.dtype) do_free(rz->key.dtype);
    if (rz->key.sklad) do_free(rz->key.sklad);
    if (rz->key.document) do_free(rz->key.document);
    if (rz->link.barcode) do_free(rz->link.barcode);
    if (rz->order.number) do_free(rz->order.number);
    memset(rz, 0, sizeof(rz_info_t));
}
static GMatchInfo *get_match_info(const char *str, const char *pattern)
{
    GRegex     *re = NULL;
    GMatchInfo *match_info = NULL;
    GError     *error = NULL;
    int res;
    res = FALSE;

    re = g_regex_new(pattern,
                     G_REGEX_MULTILINE | G_REGEX_NEWLINE_CR, 0, &error);
    if ( !re ) {
        g_error_free(error);
        do_log(LOG_ERR, "Format \"%s\" error :", pattern, error->message);
        return NULL;
    }
    g_regex_match (re, str, 0, &match_info);
    if ( !g_match_info_matches (match_info) ) {
        do_log(LOG_ERR, "Unable to parse string: \"%s\" \"%s\"", str, pattern);
        g_match_info_free(match_info);
        g_regex_unref(re);
        return NULL;
    }
    g_regex_unref(re);
    return match_info;
}
static char *get_string(GIConv conv, const char *str, config_t *cf, int item_n)
{
    GMatchInfo *match_info;
    char *value;
    char *retval;
    if ( !cf->item[item_n].pattern )
        return do_utf8_make_compatibility_cp866(conv, str);

    match_info = get_match_info(str, cf->item[item_n].pattern);
    if ( !match_info )
        return do_strdup("");
    value = g_match_info_fetch_named (match_info, "Value");
    if ( !value )
        value = do_strdup("");
    g_match_info_free(match_info);
    retval = do_utf8_make_compatibility_cp866(conv, value);
    do_free(value);

    return retval;
}
static char *get_title(const char *str, config_t *cf)
{
    GMatchInfo *match_info;
    char *value;
    if ( !cf->title_pattern || cf->title_pattern[0] == '\0' )
        return do_strdup(str);

    match_info = get_match_info(str, cf->title_pattern);
    if ( !match_info )
        return do_strdup("");
    value = g_match_info_fetch_named (match_info, "Value");
    if ( !value )
        value = do_strdup("");
    g_match_info_free(match_info);
    return value;
}
static int get_integer(const char *str, config_t *cf, int item_n)
{
    GMatchInfo *match_info;
    char *value;
    int retval;
    if ( !cf->item[item_n].pattern )
        return atoi(str);

    match_info = get_match_info(str, cf->item[item_n].pattern);
    if ( !match_info )
        return 0;
    value = g_match_info_fetch_named (match_info, "Value");
    if ( !value )
        retval = 0;
    else {
        retval = atoi(value);
        do_free(value);
    }
    g_match_info_free(match_info);
    return retval;
}
static double get_double(const char *str, config_t *cf, int item_n)
{
    GMatchInfo *match_info;
    char *value;
    int retval;
    if ( !cf->item[item_n].pattern )
        return atof(str);

    match_info = get_match_info(str, cf->item[item_n].pattern);
    if ( !match_info )
        return 0;
    value = g_match_info_fetch_named (match_info, "Value");
    if ( !value )
        retval = 0;
    else {
        retval = atof(value);
        do_free(value);
    }
    g_match_info_free(match_info);
    return retval;
}
static int get_boolean(const char *str, config_t *cf, int item_n)
{
    char *value;
    int retval;
    value = get_string((GIConv)-1, str, cf, item_n);
    if ( !cf->item[item_n].logical )
        retval = atoi(value) == 1;
    else
        retval = g_regex_match_simple (cf->item[item_n].logical, value, 0, 0);
    do_free(value);
    return retval;
}
static time_t get_time(const char *str, config_t *cf, int item_n)
{
    GMatchInfo *match_info;
    char *value;
    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    if ( !cf->item[item_n].pattern )
        return (time_t)-1;

    match_info = get_match_info(str, cf->item[item_n].pattern);
    if ( !match_info )
        return (time_t)-1;

    value = g_match_info_fetch_named (match_info, "ValueDay");
    if ( !value ) {
        g_match_info_free(match_info);
        return (time_t)-1;
    }
    tm.tm_mday = atoi(value);
    g_free(value);
    value = g_match_info_fetch_named (match_info, "ValueMonth");
    if ( !value ) {
        g_match_info_free(match_info);
        return (time_t)-1;
    }
    tm.tm_mon = atoi(value) - 1;
    g_free(value);
    value = g_match_info_fetch_named (match_info, "ValueYear");
    if ( !value ) {
        g_match_info_free(match_info);
        return (time_t)-1;
    }
    tm.tm_year = atoi(value) - 1900;
    if ( tm.tm_year < 0 )
        tm.tm_year += 2000;

    g_free(value);
    g_match_info_free(match_info);
    return mktime(&tm);
}
static int clear_grlspl(do_alias_t *alias)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    document_rec_t document;
    document_key0_t document_key0, key;
    int status;

    do_text_set(alias, document_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_RZN));
    do_text_set(alias, document_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set_empty(document_key0.document);
    do_cpy(key.dtype, document_key0.dtype);
    do_cpy(key.sklad, document_key0.sklad);

    do_cpy(document_order_key0.dtype, key.dtype);
    do_cpy(document_order_key0.sklad, key.sklad);
    do_text_set_empty(document_order_key0.document);
    document_order_key0.line = 0;

    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    while (status == DO_OK) {
        if ( do_cmp(document_order_key0.dtype, key.dtype) ||
             do_cmp(document_order_key0.sklad, key.sklad) )
            break;

        if (document_order.data.accepted == RZ_INFO_LINK_NO) {
            if ( do_document_order_delete(alias) != DO_OK ) {
        	status = DO_ERROR;
                break;
            }
        }
        else {
            document_order.data.accepted = RZ_INFO_LINK_FAIL;
            if ( do_document_order_update(alias, &document_order) != DO_OK ) {
        	status = DO_ERROR;
                break;
            }
        }
        status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
    }

    if ( status == DO_ERROR )
        return FALSE;

    status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key0.dtype) ||
             do_cmp(key.sklad, document_key0.sklad) )
            break;

	do_cpy(document_order_key0.dtype, document_key0.dtype);
	do_cpy(document_order_key0.sklad, document_key0.sklad);
	do_cpy(document_order_key0.document, document_key0.document);
	do_text_set_empty(document_order_key0.document);
	document_order_key0.line = 0;
	status = do_document_order_key0(alias, &document_order_key0, DO_GET_GE);
	if ( status == DO_ERROR )
	    break;

	if ( status == DO_KEY_NOT_FOUND ||
	     do_cmp(document_order_key0.dtype, document_key0.dtype) ||
	     do_cmp(document_order_key0.sklad, document_key0.sklad) ||
	     do_cmp(document_order_key0.document, document_key0.document)
	  ) {
	    if ( do_document_delete(alias) != DO_OK ) {
        	status = DO_ERROR;
        	break;
    	    }
    	}
        status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);
    }
    return status != DO_ERROR;
}
static int rz_insert(do_alias_t *alias, rz_info_t *rz_info)
{
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    gboolean retval = FALSE;
    int indx;

    for (indx = 1; indx < 100 && !retval; indx++) {
        do_rz_info_barcode_to_document_order_key0(rz_info, alias, &document_order_key0);
        document_order_key0.line += indx;
        switch (do_document_order_key0(alias, &document_order_key0, DO_GET_EQUAL)) {
            case DO_KEY_NOT_FOUND: {
                rz_info->key.dtype = do_text(alias, document_order_key0.dtype);
                rz_info->key.sklad = do_text(alias, document_order_key0.sklad);
                rz_info->key.document = do_text(alias, document_order_key0.document);
                rz_info->key.line = document_order_key0.line;

                do_rz_info_key_to_document_order(rz_info, alias, &document_order);
                if (do_document_order_insert(alias, &document_order) == DO_OK )
                    retval = TRUE;
                else
                    return FALSE;
                break;
            }
            case DO_ERROR:
                return FALSE;
            default:
                break;
        }
    }
    if (!retval)
        return FALSE;

    return rz_insert_document(alias, &document_order);
}
static int rz_insert_document(do_alias_t *alias, document_order_rec_t *document_order)
{
    document_rec_t document;
    document_key0_t document_key0;

    do_cpy(document_key0.dtype, document_order->data.dtype);
    do_cpy(document_key0.sklad, document_order->data.sklad);
    do_cpy(document_key0.document, document_order->data.document);

    switch (do_document_key0(alias, &document_key0, DO_GET_EQUAL)) {
        case DO_OK:
            return TRUE;
        case DO_KEY_NOT_FOUND: {
            memset(&document.data, 0, sizeof(document.data));
            document.size = sizeof(document.data) - sizeof(document.data.params);
            do_cpy(document.data.dtype, document_key0.dtype);
            do_text_set(alias, document.data.dtype1, "1");
            do_cpy(document.data.sklad, document_key0.sklad);
            do_cpy(document.data.document, document_key0.document);
            do_text_set_empty(document.data.name);
            time_t now = time(NULL);
            struct tm tm = *localtime(&now);
            do_date_set(&document.data.date, tm);
            do_time_set(&document.data.time, tm);
            do_text_set(alias, document.data.p_g_code, "00");
            document.data.p_code = 1;
            do_text_set_empty(document.data.p_doc);
            do_text_set_empty(document.data.party);
            if (do_document_insert(alias, &document) == DO_ERROR)
                return FALSE;
            return TRUE;
        }
        default :
            return FALSE;
    }
}

