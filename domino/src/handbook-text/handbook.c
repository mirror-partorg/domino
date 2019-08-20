#include "handbook.h"
#include "c_util.h"
#include <domino.h>
#include <domino_action.h> 
#include <unistd.h>
#include <stdio.h>
#include <glib.h>

#define MAX_READ 22

#define PRODUCT_VIEW_TYPE_NAME_ 1
#define PRODUCT_VIEW_TYPE_MNN_ 2
#define CACHE_LIFETIME 20

typedef struct {
    char code[6];
    char name[201];
    time_t time;
} cache_t;


struct _handbook_t {
    do_list_t *keys;
    //artix_t *artix;
    do_alias_t *alias;
    int retail_price_n;
    int unfinished;
    char *last_name;
    int mode;
    char *marked_name[DOMINO_MARKED_AND_REJECT+1];
    do_sort_list_t *cache_list;
    do_list_t *stack;

};
static char *get_product_inn(do_alias_t *alias, const char *code);
static do_string_list_t *handbook_get_recomment_list(handbook_t *h, const char *master);

static int cache_cmp_func(cache_t *s1, cache_t *s2)
{
    return strncmp(s1->code,s2->code,5);
}

static int handbook_connect_alias(handbook_t *h)
{
    if (!do_alias_conected(h->alias))
        if (!do_alias_open(h->alias, 0, DO_DB_PRODUCT, DO_DB_DOCUMENT, DO_DB_BARCODE, DO_DB_STOCK, DO_DB_SKLAD, DO_DB_PRODUCT_VIEW, DO_DB_DOCUMENT_ORDER, DO_DB_DOCUMENT, DO_DB_PROTOCOL, DO_DB_PARTNER, DO_DB_END))
            return 0;
    if (h->retail_price_n == -1)
        h->retail_price_n = domino_nretail_price(h->alias, domino_unit());
    return 1;
}

void handbook_set_marked_name(handbook_t *h, domino_marked_t marked, const char *name)
{
    if ( marked <= DOMINO_MARKED_AND_REJECT )
         h->marked_name[marked]=do_strdup(name);
}

handbook_t *handbook_new(const char *host, const char *cs, const char *user, const char *passwod)
{
    handbook_t *retval;
    retval = (handbook_t*)do_malloc0(sizeof(handbook_t));
    retval->keys = NULL;
    /*retval->artix = artix_new(host, cs, 0, user, passwod, FALSE);
    if (!retval->artix) {
        do_free(retval);
        return NULL;
    }*/
    int i;
    retval->cache_list = do_sort_list_new(TRUE,TRUE,(do_list_cmp_func)cache_cmp_func, NULL);
    for ( i = 0; i <= DOMINO_MARKED_AND_REJECT; i++ )
         retval->marked_name[i]=do_strdup("");
    retval->alias = domino_alias_new(NULL);
    if (!retval->alias) {
        //artix_free(retval->artix);
        do_free(retval);
        return NULL;

    }
    retval->retail_price_n = -1;
    handbook_connect_alias(retval);
    retval->stack = do_list_new(FALSE);
    return retval;
}
void handbook_free(handbook_t *h)
{
    if (h->keys)
        do_list_free(h->keys);
    //artix_free(h->artix);
    do_sort_list_free(h->cache_list);
    do_alias_free(h->alias);
    if ( h->last_name )
        do_free(h->last_name);
    do_free(h);
}
do_list_t *domino_get_products_by_name(handbook_t *h, const char *subs, int max_, product_view_key0_t *last_key)
{
    product_view_key0_t key0;
    do_alias_t *alias=h->alias;

    char sort[sizeof(key0.sort)];

    int status, len;
    do_list_t *retval;
    int type_;
    type_ = (h->mode == 1)  ? PRODUCT_VIEW_TYPE_MNN_: PRODUCT_VIEW_TYPE_NAME_;

    handbook_stack_clear(h);

    retval = do_list_new(TRUE);
    if ( !last_key ) {
        key0.type = type_;
        do_text_set_empty(key0.sklad);
        do_text_set_empty(key0.code);
        do_text_set(alias, key0.sort, subs);
        do_cpy(sort, key0.sort);
        status = do_product_view_key0(alias, &key0, DO_GET_GE);

    }
    else {
        do_cpy(key0, *last_key);
        do_cpy(sort, key0.sort);
        status = do_product_view_key0(alias, &key0, DO_GET_NEXT);
    }
    len = c_strlen(subs);

    h->unfinished = FALSE;


    while ( status == DO_OK ) {
        if ( (key0.type != type_) ||
             (strncmp(key0.sort, sort, len) )
             )

              break;

        do_list_add_alloc(retval, &key0, sizeof(key0));
        if ( h->mode == 1 )
            do_list_add_alloc(retval, &key0, sizeof(key0));

        if ( retval->count > max_ ) {
            h->unfinished = TRUE;
            break;
        }

        status = do_product_view_key0(alias, &key0, DO_GET_NEXT);

    }
    if ( status == DO_ERROR ) {
        do_list_free(retval);
        return NULL;
    }
    return retval;
}
do_list_t *domino_get_recoment_by_master(handbook_t *h, const char *master)
{
    product_view_key0_t key0;
    do_alias_t *alias = h->alias;
    document_key0_t document_key0, key;
    document_rec_t document;

    char *value, *desc;

    int status;
    do_list_t *retval;

    retval = do_list_new(TRUE);

    do_text_set(alias, document_key0.dtype, "MR");
    do_text_set(alias, document_key0.sklad, do_param(DO_PARAM_COMMON_SKLAD));
    do_text_set_empty(document_key0.document);

    do_cpy(key.dtype, document_key0.dtype);
    do_cpy(key.sklad, document_key0.sklad);
    h->unfinished = FALSE;


    status = do_document_get0(alias, &document, &document_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key0.dtype) ||
             do_cmp(key.sklad, document_key0.sklad)
             )

              break;
        value = do_document_param(alias, &document, "MASTER");
        if ( !strcmp(value, master) ) {
            if ( do_document_param_int(alias, &document, "END") == 1 ) {
                document_order_key0_t document_order_key0;
                document_order_rec_t document_order;

                do_cpy(document_order_key0.dtype, document_key0.dtype);
                do_cpy(document_order_key0.sklad, document_key0.sklad);
                do_cpy(document_order_key0.document, document_key0.document);
                document_order_key0.line = 0;
                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GT);
                while ( status == DO_OK ) {
                    if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                         do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                         do_cmp(document_order_key0.document, document_key0.document)
                       )
                       break;
                    do_cpy(key0.code, document_order.data.code);
                    do_text_set_empty(key0.sort);
                    key0.type=1;
                    do_list_add_alloc(retval, &key0, sizeof(key0));
                    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
                }
                if ( status == DO_ERROR ) {
                    do_list_free(retval);
                    return NULL;
                }
                do_free(value);
                return retval;
            }
            else {
                char *sad = do_text(alias,document.data.document);
                key0.type = 77;
                desc = do_document_param(alias ,&document, "37");
                do_text_set(alias, key0.sort, desc);
                do_text_set_empty(key0.code);
                do_text_set(alias, key0.code, sad);
                do_list_add_alloc(retval, &key0, sizeof(key0));
                do_free(desc);do_free(sad);
                if ( h->mode == 1 )
                    do_list_add_alloc(retval, &key0, sizeof(key0));
            }
        }
        do_free(value);
        status = do_document_get0(alias, &document, &document_key0, DO_GET_NEXT);

    }
    if ( status == DO_ERROR ) {
        do_list_free(retval);
        return NULL;
    }
    return retval;
}

int handbook_mode_changed(int mode, handbook_t *h)
{
    handbook_get_product_list(h,"");
    h->mode=mode;
    return TRUE;
}
do_string_list_t *handbook_get_product_list(handbook_t *h, const char *name)
{
    do_string_list_t *retval;
    //artix_product_by_name_t *product;
    product_view_key0_t *product;
    //char *buf;
    int i;

    if (h->keys) {
        do_list_free(h->keys);
        h->keys = NULL;
    }
    if ( h->last_name ) {
        do_free(h->last_name);
        h->last_name = NULL;
    }

    if ( strlen(name) == 0 ) {
	    do_sort_list_clear(h->cache_list);

        return handbook_get_recomment_list(h,"");
    }

    retval = do_string_list_new();

//    buf = (char*)do_malloc(strlen(name) + 5);
  //  sprintf(buf, "%s%c", name, '%');

    h->last_name = do_strdup(name);

    h->keys = domino_get_products_by_name(h, name, MAX_READ, NULL); //(artix_alias_get_products_by_name(h->artix->alias, buf);
//    do_free(buf);
    if (!h->keys)
        return retval;

    for (i = 0; i < h->keys->count; i++) {
        product = h->keys->list[i];
        do_string_list_add(retval, do_text(h->alias, product->sort));
        //buf = artix_text(h->artix->alias, product->name);
        //do_string_list_addf_alloc(retval, "%s %.2f", buf, product->price);
        //do_free(buf);
    }
    return retval;
}
do_string_list_t *handbook_get_recomment_list(handbook_t *h, const char *master)
{
    do_string_list_t *retval;
    product_view_key0_t *product;
    int i;

    if (h->keys) {
        do_list_free(h->keys);
        h->keys = NULL;
    }
    if ( h->last_name ) {
        do_free(h->last_name);
        h->last_name = NULL;
    }
    retval = do_string_list_new();
    h->keys = domino_get_recoment_by_master(h, master); //(artix_alias_get_products_by_name(h->artix->alias, buf);
    if (!h->keys)
        return retval;

    for (i = 0; i < h->keys->count; i++) {
        product = h->keys->list[i];
        do_string_list_add(retval, do_text(h->alias, product->sort));
    }
    return retval;
}

do_string_list_t *handbook_get_continue(handbook_t *h)
{


    if ( !h->unfinished || !h->keys || h->keys->count < 1 || !h->last_name)
        return NULL;
    do_list_t *keys;
    do_string_list_t *ret;
    ret = do_string_list_new();

    keys = domino_get_products_by_name(h, h->last_name, MAX_READ, h->keys->list[h->keys->count-1]);
    if ( keys ) {
        int i;

        for ( i = 0; i < keys->count; i++ ) {
            do_list_add_alloc(h->keys, keys->list[i], sizeof(product_view_key0_t));
            do_string_list_add(ret, do_text(h->alias, ((product_view_key0_t*)keys->list[i])->sort));
        }
        do_list_free(keys);
    }
    return ret;
}
static do_string_list_t *str_to_list(const char *str, int width)
{
   char *end, *space;
   char *p;
   int i;
   do_string_list_t *retval;

   retval = do_string_list_new();
   p = (char *)str;
   while (*p) {
       end = p;
       space = NULL;
       for (i = 0; i < width && *end; i++) {
           end = c_next_ch(end);
           if (*end == ' ')
               space = end;
       }
       if (*end) {
          if (!space)
             space = end;
          do_string_list_addn_alloc(retval, p, space - p);
          p = space;
          if (*p == ' ')
            p++;
       }
       else {
          do_string_list_add_alloc(retval, p);
          break;
       }
   }
   return retval;
}
typedef struct {
    int code_int;
    char *code;
    char *name;
    sklad_rec_t *sklad;
} h_sklad_t;

static int list_cmp(h_sklad_t *a, h_sklad_t *b)
{
    return a->code_int - b->code_int;
}
static int list_free_(h_sklad_t *a)
{
    do_free(a->code);
    do_free(a->name);
    do_free(a->sklad);
    do_free(a);
    return 1;
}

static do_sort_list_t *get_sklad(do_alias_t *alias)
{
    do_sort_list_t *retval;
    sklad_rec_t sklad;
    sklad_key0_t sklad_key0;
    h_sklad_t *rec;

    retval = do_sort_list_new(0, 1, (do_list_cmp_func)list_cmp, NULL);
    do_text_set(alias, sklad_key0.code, domino_unit());
    do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL);

	do_string_list_t *units;
	int i;
    //units = do_string_list_new_from_csv(do_sklad_param(alias, &sklad, "LOCAL"));
    int city;
    city = do_city_get_city_by_unit(alias, domino_unit());
    units = do_city_get_units(alias, city);
    for ( i = 0; i < units->count; i++ ) {
        int status;
        do_text_set(alias, sklad_key0.code, units->list[i]);
        status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL);
        if ( status != DO_OK ) {
            do_log(LOG_ERR, "Error read sklad \"%s\"", units->list[i]);
            return NULL;
        }
        rec = do_malloc(sizeof(h_sklad_t));
        rec->code = do_text(alias, sklad.data.code);
        rec->name = do_text(alias, sklad.data.name);
        rec->sklad = do_malloc(sizeof(sklad_rec_t));
        memcpy(rec->sklad, &sklad, sizeof(sklad_rec_t));
        if (!strcmp(rec->code, domino_unit()))
            rec->code_int = 0;
        else
            rec->code_int = atoi(sklad.data.code);
        do_sort_list_add(retval, rec);
	}
	do_string_list_free(units);

    return retval;

}
static char *get_address(const char *str)
{
    char *p;
    int i;
    p = (char*)str;
    i = 0;
    while (*p) {
        if (*p == ' ')
            i++;
        if (i == 2)
            return p + 1;
        p = c_next_ch(p);

    }
    return (char*)str;
}
int handbook_stack_push(handbook_t *h)
{
    if ( !h->keys )
        return FALSE;
    do_list_insert(h->stack, 0, h->keys);
    h->keys = NULL;
    return TRUE;
}
int handbook_stack_pop(handbook_t *h)
{
    if ( !h->stack->count )
        return FALSE;

    if (h->keys) {
        do_list_free(h->keys);
        h->keys = NULL;
    }
    if ( h->last_name ) {
        do_free(h->last_name);
        h->last_name = NULL;
    }
    h->keys = h->stack->list[0];
    do_list_delete(h->stack, 0);
    return TRUE;
}
int handbook_stack_clear(handbook_t *h)
{
    int i;
    for ( i = 0; i < h->stack->count; i++ ) {
        do_list_t *keys;
        keys = h->stack->list[i];
        do_list_free(keys);
    }
    do_list_clear(h->stack);
    return TRUE;
}
int handbook_recom(handbook_t *h, const char *product_code)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    document_key0_t document_key0;
    document_rec_t document;
    BTI_LONG date_, time_;
    do_alias_t *alias = h->alias;


    char document_number[40];
    char buf[1024];
    char hostname[1024];

    struct tm tm_now;
    time_t now;
    now = time(NULL);
    tm_now = *localtime(&now);
    if (gethostname(hostname, 1023))
        strcpy(hostname, "unknow");

    do_text_set(alias, document_key0.dtype, "MM");
    do_text_set(alias, document_key0.sklad, domino_unit());
    sprintf(document_number, "%4.4d%2.2d%2.2d", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday);
    do_text_set(alias, document_key0.document, document_number);

 
    switch ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL)) {
        case DO_ERROR:
            return FALSE;
        case DO_OK:
            break;
        case DO_KEY_NOT_FOUND: {
            document_rec_t document;
            memset(&document.data, 0, sizeof(document.data));
            document.size = sizeof(document.data) - sizeof(document.data.params);
            do_cpy(document.data.dtype, document_key0.dtype);
            do_text_set(alias, document.data.dtype1, "1");
            do_cpy(document.data.sklad, document_key0.sklad);
            do_cpy(document.data.document, document_key0.document);
            do_text_set_empty(document.data.name);
            do_date_set(&document.data.date, tm_now);
            do_time_set(&document.data.time, tm_now);
            do_text_set(alias, document.data.p_g_code, "00");
            document.data.p_code = 1;
            do_text_set_empty(document.data.p_doc);
            do_text_set_empty(document.data.party);
            if (do_document_insert(alias, &document) == DO_ERROR)
                return FALSE;
            if ( !do_protocol_document_add_rec(alias, &document, DO_PROTOCOL_ACTION_INSERT) )
                return FALSE;
            break;
        }
    }

    do_cpy(document_order_key0.dtype, document_key0.dtype);
    do_cpy(document_order_key0.sklad, document_key0.sklad);
    do_cpy(document_order_key0.document, document_key0.document);
    document_order_key0.line = 2147483646;

    switch ( do_document_order_key0(alias, &document_order_key0, DO_GET_LT) ) {
        case DO_ERROR:
            return FALSE;
        case DO_KEY_NOT_FOUND:
            document_order_key0.line = 0;
            break;
        case DO_OK:
            if ( do_cmp(document_order_key0.dtype, document_key0.dtype) ||
                 do_cmp(document_order_key0.sklad, document_key0.sklad) ||
                 do_cmp(document_order_key0.document, document_key0.document)
                )
                document_order_key0.line = 0;
            break;
    }
    do_cpy(document_order.data.dtype, document_key0.dtype);
    do_text_set(alias, document_order.data.dtype1, "1");
    do_cpy(document_order.data.sklad, document_key0.sklad);
    do_cpy(document_order.data.document, document_key0.document);
    document_order.data.line = document_order_key0.line + 1;
    document_order.data.sort = 0;
    do_text_set(alias, document_order.data.code, product_code);
    do_date_set(&document_order.data.date, tm_now);
    do_time_set(&document_order.data.time, tm_now);
    document_order.data.accepted = 0;
    do_set_empty(document_order.data.nquant);
    do_set_empty(document_order.data.nprice);
    do_document_order_params_clear(alias, &document_order);
    do_document_order_param_set(alias, &document_order, "host", hostname);
    if ( do_document_order_insert(alias, &document_order) != DO_OK )
        return FALSE;
    return TRUE;
}
do_string_list_t *handbook_get_product_info(handbook_t *h, int index, int width, int *index_top, int *clickable, char **footer)
{
    int i, j;
    char *code;
    char *name;
    char *ei;
    char buf[2048];
    do_string_list_t *info;
    do_string_list_t *n;
    do_sort_list_t *sklad;
    product_rec_t product;
    product_key0_t product_key0;
    product_view_key1_t product_view_key1;
    h_sklad_t *rec;
    do_list_t *stock;
    do_rest_price_life_t *rest;
    double coef;
    int fl;
    char *mnn;
    int type_;

    if (h->keys->count <= index)
        return NULL;


    info = do_string_list_new();
    code = do_text(h->alias, ((product_view_key0_t*)h->keys->list[index])->code);
    type_ = ((product_view_key0_t*)h->keys->list[index])->type;
    *footer = NULL;
    if ( type_ == 77 ) {
        *clickable = TRUE;
        handbook_stack_push(h);
        info = handbook_get_recomment_list(h, code);
        return info;
    }
    else {
	if ( h->stack->count > 0 ) {
	    handbook_recom(h, code);
	}
    }
    *clickable = FALSE;
    //artix_text(h->artix->alias, ((artix_product_by_name_t*)h->product->list[index])->code);
    if (!handbook_connect_alias(h))
        return NULL;

    do_text_set(h->alias, product_key0.code, code);
    product_key0.base_parcel = 0;
    if (do_product_get0(h->alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK) {
        do_free(code);
        return NULL;
    }
    //width = 20;//!!
    name = do_text(h->alias, product.data.name);
    coef = do_product_coef(h->alias, &product);
    ei = do_text(h->alias, product.data.unit);
    do_text_set_empty(product_view_key1.sklad);
    do_cpy(product_view_key1.code, product.data.code);
    mnn = get_product_inn(h->alias, ((product_view_key0_t*)h->keys->list[index])->code);
    sprintf(buf, "%s %s", code, name);
    if (coef > 1)
        sprintf(buf+strlen(buf), " (1*%.0f %s)", coef, ei);
    if ( mnn && mnn[0] != '\0' ) {
        sprintf(buf+strlen(buf), " [%s]", mnn);
    }
    do_free(mnn);
    do_free(ei);
    char *order = NULL;

    struct tm tm1;
    struct tm tm2;
    struct tm tm;
    time_t now;
    domino_marked_t marked;

    now = time(NULL);
    tm1 = *localtime(&now);
    tm2 = tm1;
    tm1.tm_mday-=DOMINO_MARKED_INTERVAL;
    mktime(&tm1);

    if ( domino_order_get_marked(h->alias, domino_unit(), code, &tm1, &tm2, &marked ,&tm) ) {
        if ( marked != DOMINO_MARKED_UNMARK &&
             domino_order_after_traffic_in(h->alias, domino_unit(), code, &tm) )
             marked = DOMINO_MARKED_UNMARK;
        if ( marked != DOMINO_MARKED_UNMARK ) {
            order = (char*)malloc(200);
            strcpy(order, h->marked_name[marked]);
            order[strlen(h->marked_name[marked])] = ' ';
            strftime(order + strlen(h->marked_name[marked]) + 1, 200-3, "%d/%m %H:%M", &tm);

        }
    }
    if ( order ) {
        sprintf(buf+strlen(buf), " (%s)", order);
        do_free(order);
    }



    n = str_to_list(buf, width);
    for (i = 0; i < n->count; i++)
        do_string_list_add_alloc(info, n->list[i]);
    do_string_list_free(n);
    sklad = get_sklad(h->alias);
    stock = do_list_new(1);
    *index_top = 0;
    for (i = 0; i < sklad->count; i++) {
        rec = sklad->list[i];
        if (!i) {
            do_string_list_addf_alloc(info, "%-2s %s", rec->code, get_address(rec->name));
            fl = 1;
            if (!*index_top)
                 *index_top = info->count - 1;
        }
        else
            fl = 0;
        if (do_get_rest_and_price(h->alias, code, rec->sklad, stock, 1)) {
            for (j = 0; j < stock->count; j++) {
                char life[20];

                rest = stock->list[j];
                if (rest->rest != 0) {
                    if (!fl) {
                        do_string_list_addf_alloc(info, "%-2s %s", rec->code, get_address(rec->name)
                        );
                        fl = 1;
                    }
                    if (rest->life_time.tm_year)
                        sprintf(life, "%2.2d/%4.4d", rest->life_time.tm_mon + 1, rest->life_time.tm_year + 1900);
                    else
                        life[0] = '\0';

                    if (coef > 1)
                        do_string_list_addf_alloc(info, "  %.2fp %.3f %.2fp %.0f %s", rest->price, rest->rest, rest->price/coef, rest->rest*coef, life);
                    else
                        do_string_list_addf_alloc(info, "  %.2fp %.3f %s", rest->price, rest->rest, life);
                }
            }
        }
        do_list_clear(stock);
    }
    do_list_foreach(sklad, (do_list_foreach_func)list_free_, NULL);
    do_sort_list_free(sklad);
    do_free(name);
    do_free(code);
    return info;
}
static gchar *do_product_name_format(do_alias_t *alias, product_rec_t *product)
{
    char *name;
    name = do_text(alias, product->data.name);
#ifdef USE_UTF8
    gchar *buf = g_malloc(sizeof (gunichar) * strlen (name) + 1);
    gunichar  out;
    int nlen = 0, first_alpha = 1;
    gchar *p;
    for (p = (gchar*)name; p && *p !='\0'; p = (gchar*)g_utf8_next_char(p)) {
        out = g_utf8_get_char(p);
        if (first_alpha && g_unichar_isalpha(out))
            first_alpha = 0;
        else
            out = g_unichar_tolower (out);
        nlen += g_unichar_to_utf8(out, buf + nlen);
    }
    buf[nlen] = '\0';
#else
    char *buf = do_strdup(name);
    int first_alpha = 1;
    char *p;
    for (p = (char *)buf; p && *p !='\0'; p++ ) {
        if (first_alpha && isalpha(*p))
            first_alpha = 0;
        else
            *p = tolower (*p);
    }
#endif
    do_free(name);
    return buf;
}

static domino_marked_t do_product_get_received(do_alias_t *alias, const char *code)
{
    struct tm tm1;
    struct tm tm2;
    struct tm tm;
    time_t now;
    domino_marked_t retval;

    now = time(NULL);
    tm1 = *localtime(&now);
    tm2 = tm1;
    tm1.tm_mday-=DOMINO_MARKED_INTERVAL;
    mktime(&tm1);

    if ( domino_order_get_marked(alias, domino_unit(), code, &tm1, &tm2, &retval, &tm) ) {
        if ( retval != DOMINO_MARKED_UNMARK &&
             domino_order_after_traffic_in(alias, domino_unit(), code, &tm) )
            return DOMINO_MARKED_UNMARK;
        return retval;
    }
    return DOMINO_MARKED_UNMARK;
}
/*
int do_product_get_received(do_alias_t *alias, const char *code)
{
    struct tm tm;
    time_t now;
    int i;
    document_order_key2_t document_order_key2;
    char *document_number;

    now = time(NULL);
    tm = *localtime(&now);

    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER));
    do_text_set(alias, document_order_key2.sklad, domino_unit());
    do_text_set(alias, document_order_key2.code, code);

    for (i = 0; i < 2; i++ ) {
        tm.tm_mday-=i;
        mktime(&tm);

        document_number = g_strdup_printf("%4.4d%2.2d%2.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        do_text_set(alias, document_order_key2.document, document_number);
        g_free(document_number);

        if ( do_document_order_key2(alias, &document_order_key2, DO_GET_EQUAL) == DO_OK )
            return (!i) ? 2 : 1;
    }

    return FALSE;
}
*/
static char *get_product_inn(do_alias_t *alias, const char *code)
{
    char *b, *ch;
    char *name;
    int status;
    product_view_key1_t product_view_key1;
    product_view_rec_t product_view;
    product_key0_t product_key0;

    do_text_set(alias, product_view_key1.code,code);
    do_cpy(product_key0.code, product_view_key1.code);
    do_text_set_empty(product_view_key1.sklad);
    status = do_product_view_get1(alias, &product_view, &product_view_key1, DO_GET_EQUAL);
    name = NULL;
    while ( status == DO_OK ) {
        if ( do_cmp(product_key0.code, product_view_key1.code) )
            break;
        if ( product_view.data.type == PRODUCT_VIEW_TYPE_MNN_) {
            b = do_text(alias, product_view.data.sort);
            if ( !name )
                name = b;
            else {
                ch = g_strdup_printf("%s+%s", name, b);
                g_free(b);
                g_free(name);
                name = ch;
            }
        }
        status = do_product_view_get1(alias, &product_view, &product_view_key1, DO_GET_NEXT);
    }
    return name ? name : do_strdup("");

}
//static int indexo=0;
char *handbook_get_product_row(handbook_t *h, int index, const char *text, int max_text_len)
{
    char *code;
    char *name;
    product_rec_t product;
    product_key0_t product_key0;
    double price;
    char *retval;
    char buf[50];
    char *ch;


    if ( h->mode == 1 && (index % 2) ) {
        name = get_product_inn(h->alias, ((product_view_key0_t*)h->keys->list[index])->code);
        ch = g_strdup_printf(" [%s]", name);
        do_free(name);
        return ch;
    }

    if ( h->keys->count <= index && text[0] != '\0' )
        return do_strdup(text);

    cache_t cache, *item_cache;
    int index_cache,type_;

//    return do_strdup("dsfddsafdsafdsafdafdsa");

//    code = artix_text(h->artix->alias, ((artix_product_by_name_t*)h->product->list[index])->code);
    strncpy(cache.code, ((product_view_key0_t*)h->keys->list[index])->code, 5);
    if ( do_sort_list_find(h->cache_list, &index_cache, &cache) ) {
	item_cache = h->cache_list->list[index_cache];
	if ( time(NULL) - item_cache->time < CACHE_LIFETIME ) {
	//    fprin
	    return do_strdup(item_cache->name);
	}
    }
    else {
        item_cache = NULL;
    }
    code = do_text(h->alias, ((product_view_key0_t*)h->keys->list[index])->code);
    type_ = ((product_view_key0_t*)h->keys->list[index])->type;
    if ( type_ == 77 )
        return do_strdup(text);


    if (!do_alias_conected(h->alias))
        if (!do_alias_open(h->alias, 0, DO_DB_PRODUCT, DO_DB_BARCODE, DO_DB_STOCK, DO_DB_SKLAD, DO_DB_END))
            return do_strdup(text);

    do_text_set(h->alias, product_key0.code, code);
    product_key0.base_parcel = 0;
    if (do_product_get0(h->alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK) {
        do_free(code);
        return do_strdup(text);
    }
    name = do_product_name_format(h->alias, &product);

    price = do_product_price(h->alias, &product, h->retail_price_n)*do_product_coef(h->alias, &product);

    retval = (char*)do_malloc(max_text_len*4 + 1);
    memset(retval, ' ', max_text_len*4);
    int shift = 0;
    domino_marked_t marked;
    marked = do_product_get_received(h->alias, code);
    if ( marked != DOMINO_MARKED_UNMARK ) {
        shift = 3;
        switch (marked)  {
                case DOMINO_MARKED:
                    retval[0] = '*';
                    break;
                case DOMINO_MARKED_AND_ACCEPT:
                    retval[0] = '+';
                    break;
                case DOMINO_MARKED_AND_REJECT:
                    retval[0] = '-';
                    break;
                case DOMINO_MARKED_UNMARK:
                    retval[0] = ' ';
            }
    }

    if (c_strlen(name) < max_text_len-shift)
        c_strncpy(retval+shift, name, strlen(name));
    else
        c_strncpy(retval+shift, name, max_text_len);


    if ( (price > 0) ) { // && (do_get_rest(h->alias, code, domino_unit()) > 0) ) {

        sprintf(buf, "%0.2f", price);
        if (c_strlen(buf) > max_text_len - 1) {
            ch = c_str_at(retval, max_text_len);
            *ch = '\0';
        }
        else {
            ch = c_str_at(retval, max_text_len - 1 - c_strlen(buf));
            *ch = ' ';
            strcpy(ch + 1, buf);
        }
    }

    do_free(name);
    do_free(code);
    if ( !item_cache )
	item_cache = do_sort_list_insert_new(h->cache_list, index_cache, &cache, sizeof(cache));


    if ( strlen(retval) > 200 ) {
        strncpy(item_cache->name, retval, 200);
	item_cache->name[200]='\0';
    }
    else
        strcpy(item_cache->name, retval);
    item_cache->time = time(NULL);

    return retval;
}
int handbook_row_seleted(int index, int unselected, handbook_t *h)
{
    if ( index < 0 || index >= h->keys->count )
        return FALSE;

    char *code;
    do_alias_t *alias = h->alias;
    product_key0_t product_key0;
    int retval;
    struct tm tm1;
    struct tm tm2;
    time_t now;
    int type_;

    now = time(NULL);
    tm1 = *localtime(&now);
    tm2 = tm1;
    tm1.tm_mday-=DOMINO_MARKED_INTERVAL;
    mktime(&tm1);
    
    type_= ((product_view_key0_t*)h->keys->list[index])->type;
    if ( type_ == 77 )
        return FALSE;
    code = do_text(h->alias, ((product_view_key0_t*)h->keys->list[index])->code);
    

    domino_marked_t marked;
    marked = do_product_get_received(h->alias, code);

    if ( unselected && (marked != DOMINO_MARKED) )
    	return FALSE;
    if ( !unselected && (marked != DOMINO_MARKED_UNMARK) )
    	return FALSE;
    retval = domino_order_set_marked(alias, domino_unit(), code, &tm1, &tm2, unselected ? DOMINO_MARKED_UNMARK : DOMINO_MARKED);
    do_sort_list_clear(h->cache_list);
    do_free(code);
    return retval;
}
/*
int handbook_row_seleted(int index, int unselected, handbook_t *h)
{
    if ( index < 0 || index >= h->keys->count )
        return FALSE;

    char *code;
    int retval,status;
    struct tm tm;
    do_alias_t *alias = h->alias;
    time_t now;
    char *document_number;
    document_order_key2_t document_order_key2;
    document_order_rec_t document_order;

    code = do_text(h->alias, ((product_view_key0_t*)h->keys->list[index])->code);
    retval = TRUE;

    if ( do_product_get_received(alias, code) )
        return TRUE;

    return domino_action_order_insert_product(alias, code);

    now = time(NULL);
    tm = *localtime(&now);
    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER));
    do_text_set(alias, document_order_key2.sklad, domino_unit());
    document_number = g_strdup_printf("%4.4d%2.2d%2.2d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    do_text_set(alias, document_order_key2.document, document_number);
    g_free(document_number);
    do_text_set(alias, document_order_key2.code, code);
    status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL);

    if ( unselected ) {

        switch ( status ) {
            case DO_ERROR:
                retval = FALSE;
                break;
            case DO_KEY_NOT_FOUND:
                break;
            case DO_OK:
                if (do_document_order_delete(alias) != DO_OK)
                    retval = FALSE;
                break;
        }
    }
    else {
        int insert = FALSE;
        switch ( status ) {
            case DO_ERROR:
                retval = FALSE;
                break;
            case DO_OK:
                insert = FALSE;
                break;
            case DO_KEY_NOT_FOUND:
                insert = TRUE;
                break;
        }
        if ( retval && insert ) {
            document_key0_t document_key0;
            document_order_key0_t document_order_key0;
            document_order_rec_t document_order;

            do_cpy(document_key0.dtype, document_order_key2.dtype);
            do_cpy(document_key0.sklad, document_order_key2.sklad);
            do_cpy(document_key0.document, document_order_key2.document);

            memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
            document_order_key0.line = 2147483646;

            if (do_document_order_key0(alias, &document_order_key0, DO_GET_LT) == DO_ERROR) {
                retval = FALSE;
            }
            if ( retval && memcmp(&document_order_key0, &document_key0, sizeof(document_key0))) {
                memcpy(&document_order_key0, &document_key0, sizeof(document_key0));
                document_order_key0.line = 0;
            }
            if ( retval ) {
                do_cpy(document_order.data.dtype, document_order_key0.dtype);
                do_text_set(alias, document_order.data.dtype1, "1");
                do_cpy(document_order.data.sklad, document_order_key0.sklad);
                do_cpy(document_order.data.document, document_order_key0.document);
                document_order.data.line = document_order_key0.line + 1;
                document_order.data.sort = 0;
                do_text_set(alias, document_order.data.code, code);
                do_date_set(&document_order.data.date, tm);
                do_time_set(&document_order.data.time, tm);
                document_order.data.accepted = 0;
                do_set_empty(document_order.data.nquant);
                do_set_empty(document_order.data.nprice);
                document_order.size = sizeof(document_order.data) - sizeof(document_order.data.params);
                if (do_document_order_insert(alias, &document_order) != DO_OK) {
                    retval = FALSE;
                }
            }
        }
    }
    do_free(code);
    return retval;
}
*/
