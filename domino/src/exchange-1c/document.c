#include "document.h"
#include "exchange.h"
#include "do_message.h"
#include "exchange-const.h"
#include "do_progress_info.h"
#include "misc.h"
#include <domino.h>
#include <libxml/tree.h>

typedef struct {
    do_alias_t *alias_1;
    do_alias_t *alias_2;
    ExchangeObject *unit;
    sklad_key0_t sklad_key0;
} alias_unit_t;

typedef struct {
    do_alias_t *alias;
    do_alias_t *alias_2;
    document_rec_t *document;
    xmlNode *root;
    gint npp;
    GSList *objs;
    GSList *operations;
    const gchar *doc_name;
    GSList *sale_doc;
    ExchangeObject *valuta;
    ExchangeObject *user;
} priv_t;

static gboolean get_document_product_kind_in(priv_t *priv);
static gboolean get_document_product_kind_move(priv_t *priv);
static gboolean get_document_product_kind_surplus(priv_t *priv);
static gboolean get_document_product_kind_resort(priv_t *priv);
static gboolean get_document_product_kind_back(priv_t *priv);
static gboolean get_document_product_kind_del(priv_t *priv);
static gboolean get_document_product_kind_sale(priv_t *priv);
static gboolean get_document_product_kind_other_outlay(priv_t *priv);
static gboolean get_document_product_kind_back_confirm(priv_t *priv);
typedef struct _sale_doc_t sale_doc_t;



static struct {
    DoExchangeDocumentProductKind kind;
    gchar *name;
    gboolean (*func) (priv_t *priv);
    gboolean no_changed;
} document_product_kind[DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_NO]
=
{
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_IN, "ПоступлениеТоваровУслуг", get_document_product_kind_in, FALSE},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_MOVE, "ПеремещениеТоваров", get_document_product_kind_move, FALSE},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SURPLUS, "ОприходованиеТоваров", get_document_product_kind_surplus, FALSE},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_RE_SORT, "ОперацияБух", get_document_product_kind_resort, FALSE},

    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_BACK, "ВозвратТоваровПоставщику", get_document_product_kind_back, FALSE},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_DEL, "СписаниеТоваров", get_document_product_kind_del, FALSE},

    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SALE, "ОтчетОРозничныхПродажах", get_document_product_kind_sale, TRUE},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_OTHER_OUTLAY, "ОперацияБух", get_document_product_kind_other_outlay, TRUE},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_BACK_CONFIRM, "ОперацияБух", get_document_product_kind_back_confirm, TRUE},
};
static struct {
    DoExchangeDocumentProductKind kind;
    gchar *dtype;
    gchar *dtype1;
} document_dtype[] =
{
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_IN, "ПТ", "01"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_MOVE, "ПТ", "06"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SURPLUS, "ПТ", "11"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_RE_SORT, "ПТ", "20"},

    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_BACK, "РТ", "10"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_DEL, "РТ", "11"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_DEL, "РТ", "14"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_DEL, "РТ", "15"},

    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SALE, "РТ", "01"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SALE, "РТ", "02"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SALE, "РТ", "03"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SALE, "РТ", "04"},

    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_OTHER_OUTLAY, "РТ", "55"},
    { DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_BACK_CONFIRM, "РТ", "70"},

};
static gchar *document_product_dtypes[] =
{
    "ПТ", "РТ",
};
typedef enum {
    VAT_0,
    VAT_10,
    VAT_20,
    VAT_NO
} vat_t;
typedef struct {
    const gchar *name;
    int sum_r;
    int sum_f;
    int sum_n;
    int sum_vat_n;
    int proc;
    const gchar *acc_name;
} vat_const_t;
const vat_const_t vat_const18[VAT_NO]
=
{
    { "БезНДС", 11000, 9000, 2900,    0, 0, ""},
    { "НДС10", 11010, 9010, 2910, 15010, 10 },
    { "НДС18", 11020, 9020, 2920, 15020, 18 },
};
const vat_const_t vat_const19[VAT_NO]
=
{
    { "БезНДС", 11000, 9000, 2900,    0, 0, "" },
    { "НДС10", 11010, 9010, 2910, 15010, 10 },
    { "НДС20", 11020, 9020, 2920, 15020, 20 },
};

static GSList *get_unit_alias(const gchar *aliases, GSList *objs, GSList *units, const gchar *stores);
static gboolean get_document_get_total(priv_t *priv, sklad_key0_t *sklad_key0, struct tm *tm1, struct tm *tm2, gboolean protocol, document_type_key0_t *dtype, gint *total, GSList **docs);
static gboolean get_document(priv_t *priv, sklad_key0_t *sklad_key0, struct tm *tm1, struct tm *tm2, gboolean protocol, gint *crnt, gint total, GSList *protocols);
static const vat_const_t *get_vat_const(BTI_LONG date);
static ExchangeObject *get_1c_product_by_vat(GSList *objs, vat_const_t vat_const);

static gboolean close_alias(alias_unit_t *s)
{
    if ( s->alias_1 )
        do_alias_close(s->alias_1);
    if ( s->alias_2 )
        do_alias_close(s->alias_2);
    return TRUE;
}
static void make_root_element(xmlNode *root, struct tm *tm1, struct tm *tm2, const char *comment);
static void make_rules_element(xmlNode *rules);
static gboolean make_rules_types(xmlNode *convert);
static gboolean get_document_product_kind_sale_(priv_t *priv, sale_doc_t *sale_doc);
static gboolean get_document_backsale_cost_(priv_t *priv, sale_doc_t *sale_doc);
static ExchangeObject *get_object_partner(do_alias_t *alias, partner_key0_t *partner_key0, GSList *objs, DoExchangeHandbook handbook, gboolean empty, document_rec_t *document, gchar *comment);
static gboolean get_other_mismatch_sums(do_alias_t *alias, document_rec_t *document, double *sums, double *sum);
//static GSList *get_protocol_docs(priv_t *priv, struct tm *tm1, struct tm *tm2, gint *crnt, gint total);
static const gchar *get_dtype(const gchar *value)
{
    unsigned char t[2][9] = {{208,160,194,160,208,160,209,158,0},
                             {208,160,209,159,208,160,209,158,0}};
    const gchar *dt[2] = {"РТ","ПТ"};
    int i,j;
    gboolean flag;
    for ( j = 0; j <  2; j++ ) {
        flag = TRUE;
        for ( i = 0; i < 9; i ++ )
            if ( t[j][i] != (unsigned char)value[i]) {
                flag = FALSE;
                break;
            }
        if ( flag )
            return dt[j];
    }
    return value;
}
gboolean make_goods_documents(const gchar *aliases, GSList *objs, struct tm *tm1, struct tm *tm2, GSList *units, const gchar *stores, const gchar *operations, gboolean protocol, const gchar *filename, ExchangeObject *user)
{
    GSList *unit_list, *l, *list, *docs = NULL, *oper;
    xmlNode *root, *rules, *convert;
    xmlDoc *doc;
    int i;
    priv_t priv;
    priv.npp = 0;
    priv.sale_doc = NULL;
    priv.objs = objs;
    priv.user = user;
    priv.operations = NULL;

    unit_list = get_unit_alias(aliases, objs, units, stores);
    if ( !unit_list )
        return FALSE;
    gint total =0, crnt= 0;
    gboolean res;

    total = g_slist_length(unit_list);

    doc = xmlNewDoc(BAD_CAST "1.0");
    root = xmlNewNode(NULL, BAD_CAST ELEM_1C_ROOT);
    xmlDocSetRootElement(doc, root);
    make_root_element(root, tm1, tm2, "");
    rules = xmlNewChild(root, NULL,  BAD_CAST ELEM_1C_RULES, BAD_CAST "");
    make_rules_element(rules);
    convert = xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_RULES_CONVERT, BAD_CAST "");

    list = find_exchange_objects_by_code(objs, EXCHANGE_TYPE_HANDBOOK_VALUTA, CONST_1C_DEFAULT_VALUTA);
    if ( !list ) {
        MY_ERROR("Не найдена валюта с кодом \"%s\"", CONST_1C_DEFAULT_VALUTA);
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для валюты с кодом \"%s\" более одной записи", CONST_1C_DEFAULT_VALUTA);
        return FALSE;
    }
    priv.valuta = list->data;

    if ( !make_rules_types(convert) )
        return FALSE;

    res = TRUE;

    PROGRESS_STATUS("Просмотр документов");
    PROGRESS_PROGRESS(0, 1);

    if ( operations ) {
        do_alias_t *alias;
        alias = ((alias_unit_t*)unit_list->data)->alias_1;
        document_type_key0_t document_type_key0;
        do_string_list_t *tmp;
        tmp = do_string_list_new_from_csv(operations);
        for ( i = 0; i < tmp->count; i++ ) {
            char *ch, *value;
            value = tmp->list[i];
            if ( (ch = strchr(value, '.')) != NULL ) {
                *ch = '\0';
                ch++;
                do_text_set(alias, document_type_key0.dtype, get_dtype(value));
                do_text_set(alias, document_type_key0.dtype1, ch);
                priv.operations = g_slist_append(priv.operations, g_memdup(&document_type_key0, sizeof(document_type_key0)));
            }
        }
        do_string_list_free(tmp);
    }
    total = 0;
    for ( l = unit_list; res && l; l=l->next ) {
        alias_unit_t *aunit;
        aunit = l->data;
        priv.alias = aunit->alias_1;
        priv.alias_2 = aunit->alias_2;
        priv.root = root;
        if ( priv.operations ) {
            for ( oper = priv.operations; res && oper; oper=oper->next)
                res = get_document_get_total(&priv, &aunit->sklad_key0, tm1, tm2, protocol, oper->data, &total, &docs);
        }
        else
            res = get_document_get_total(&priv, &aunit->sklad_key0, tm1, tm2, protocol, NULL, &total, &docs);
    }
    if ( total > 15000 ) {
        MY_ERROR("Не возможно выгрузить более 15000 документов, уменьшите период или установите организацию");
        res = FALSE;
    }
   /*else {
        PROGRESS_STATUS("Выгрузка документов");
        PROGRESS_PROGRESS(crnt, total);
        if ( protocol ) {
            docs = get_protocol_docs(&priv, tm1, tm2, &crnt, total);
        }
    }*/

    for ( l = unit_list; res && l; l=l->next ) {
        alias_unit_t *aunit;
        aunit = l->data;
        priv.alias = aunit->alias_1;
        priv.alias_2 = aunit->alias_2;
        priv.root = root;
        res = get_document(&priv, &aunit->sklad_key0, tm1, tm2, protocol, &crnt, total, docs);
    }
    for ( l = priv.sale_doc; res && l; l = l->next ) {
        res = get_document_product_kind_sale_(&priv, l->data);
        if ( res )
            res = get_document_backsale_cost_(&priv, l->data);
    }
    //PROGRESS_PROGRESS(110, 100);

    g_slist_foreach(unit_list, (GFunc)close_alias, NULL);
    g_slist_foreach(unit_list, (GFunc)g_free, NULL);
    g_slist_free(unit_list);

    if ( res ) {
        int ret;
        ret = xmlSaveFormatFileEnc(filename, doc, "UTF-8", -1);
        if ( ret < 0 ) {
            MY_ERROR("Невозможно записать файл \"%s\"", filename);
            res = FALSE;
        }
    }
    xmlFreeDoc(doc);

    return res;
}
static gboolean get_partners_by_guid(do_alias_t *alias, partner_key0_t *partner_key0, const gchar *guid, DoExchangeHandbook handbook)
{
    partner_data_key0_t partner_data_key0, key;
    partner_data_key1_t partner_data_key1;
    partner_data_rec_t partner_data;
    int status;

    if ( handbook == DO_EXCHANGE_HANDBOOK_NO ) {
        partner_data_key1.type = PARTNER_DATA_DOMINO_TYPE;
        do_text_set_empty(partner_data_key1.g_code);
        partner_data_key1.code = 0;
    }
    else {
        switch ( handbook ) {
            case DO_EXCHANGE_HANDBOOK_PARTNER:
                do_text_set(alias, key.g_code, "03");
                break;
/*!!            case DO_EXCHANGE_HANDBOOK_CASHIER:
                do_text_set(alias, key.g_code, "CAS");
                break;*/
            case DO_EXCHANGE_HANDBOOK_FIRM:
                do_text_set(alias, key.g_code, "00");
                break;
            case DO_EXCHANGE_HANDBOOK_SKLAD:
                do_text_set(alias, key.g_code, "01");
                break;
            case DO_EXCHANGE_HANDBOOK_NO:
                MY_ERROR("Неправильно указан тип справочника");
                return FALSE;
        }
        do_cpy(partner_data_key0.g_code, key.g_code);
        partner_data_key0.code = 0;
        partner_data_key0.type = 0;
        partner_data_key0.number = 0;
    }
    if ( handbook == DO_EXCHANGE_HANDBOOK_NO )
        status = do_partner_data_get1(alias, &partner_data, &partner_data_key1, DO_GET_GE);
    else
        status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_GE);

    while ( status == DO_OK ) {
        if ( CANCELLED )
            return FALSE;
        if ( handbook == DO_EXCHANGE_HANDBOOK_NO ) {
            if ( partner_data_key1.type != PARTNER_DATA_DOMINO_TYPE ) break;
        }
        else {
            if ( do_cmp(key.g_code, partner_data_key0.g_code) ) break;
        }

        if ( partner_data.data.type == PARTNER_DATA_DOMINO_TYPE && partner_data.data.number == firm_guid_number ) {
            gchar *code;
            code = do_partner_data_param(alias, &partner_data, "GUID");
            if ( !g_strcmp0(code, guid) ) {
                do_cpy(partner_key0->g_code, partner_data.data.g_code);
                partner_key0->code = partner_data.data.code;
                g_free(code);
                return TRUE;
            }
            g_free(code);
        }

        if ( handbook == DO_EXCHANGE_HANDBOOK_NO )
            status = do_partner_data_get1(alias, &partner_data, &partner_data_key1, DO_GET_NEXT);
        else
            status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_NEXT);
    }
    if ( status != DO_ERROR )
        MY_ERROR("Для \"%s\" \"%s\" не назначено соотвествия", ELEM_1C_GUID, guid);

    return FALSE;
}
static gboolean get_sklad(do_alias_t *alias, sklad_key0_t *sklad_key0, const gchar *guid, gboolean *error)
{
    partner_key0_t partner_key0;
    if ( !get_partners_by_guid(alias, &partner_key0, guid, DO_EXCHANGE_HANDBOOK_SKLAD) ) {
        *error = TRUE;
        return FALSE;
    }
    partner_rec_t partner;
    int status;

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR ) {
        *error = TRUE;
        return FALSE;
    }

    if ( status == DO_KEY_NOT_FOUND ) {
        gchar *str;
        str = do_util_partner_key0_to_str(alias, &partner_key0);
        MY_ERROR("Партнер \"%s\" не найден!", str);
        do_free(str);
        *error = TRUE;
        return FALSE;
    }
    do_cpy(sklad_key0->code, partner.data.sklad);

    status = do_sklad_key0(alias, sklad_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR ) {
        *error = TRUE;
        return FALSE;
    }
    if ( status == DO_KEY_NOT_FOUND )
        return FALSE;

    return TRUE;
}
static gboolean make_sklad(do_alias_t *alias, GSList *objs)
{
    int status;
    partner_key0_t partner_key0, key, *pkey;
    GSList *partner_keys = NULL;

    do_text_set(alias, partner_key0.g_code, "01");
    do_cpy(key.g_code, partner_key0.g_code);
    partner_key0.code = 0;
    status = do_partner_key0(alias, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( CANCELLED )
            return FALSE;
        if ( do_cmp(key.g_code, partner_key0.g_code) ) break;


        partner_keys = g_slist_append(partner_keys, g_memdup(&partner_key0, sizeof(partner_key0)));

        status = do_partner_key0(alias, &partner_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;
    GSList *p;


    for ( p = partner_keys; p; p=p->next ) {
        if ( CANCELLED )
            return FALSE;
        pkey = p->data;
        if ( !get_object_partner(alias, pkey, objs, DO_EXCHANGE_HANDBOOK_SKLAD, TRUE, NULL, NULL) ) {
            g_slist_foreach(partner_keys, (GFunc)g_free, NULL);
            g_slist_free(partner_keys);
            return FALSE;
        }
    }
    g_slist_foreach(partner_keys, (GFunc)g_free, NULL);
    g_slist_free(partner_keys);
    return TRUE;

}
static GSList *get_unit_alias(const gchar *aliases, GSList *objs, GSList *units, const gchar *stores)
{
    do_string_list_t *s, *st;
    GSList *result = NULL;
    gboolean res;
    int i, j;
    s = do_string_list_new_from_csv(aliases);
    res = TRUE;

    for ( i = 0; res && i < s->count; i++ ) {
        alias_unit_t node;
        sklad_rec_t sklad;
        int status;
        do_alias_t *alias;
        alias = node.alias_1 = domino_alias_new((char*)s->list[i]);
        if ( !alias || !do_alias_open(alias, TRUE) ) {
            MY_ERROR("Невозможно открыть алиас \"%s\"", (char*)s->list[i]);
            res = FALSE;
            break;
        }
        node.alias_2 = domino_alias_new((char*)s->list[i]);
        if ( !node.alias_2 || !do_alias_open(node.alias_2, TRUE) ) {
            MY_ERROR("Невозможно открыть алиас \"%s\"", (char*)s->list[i]);
            res = FALSE;
            break;
        }
        if ( stores ) {
            st = do_string_list_new_from_csv(stores);
            for ( j = 0; j < st->count; j++ ) {
                do_text_set(alias, node.sklad_key0.code, st->list[j]);
                if ( do_sklad_get0(alias, &sklad, &node.sklad_key0, DO_GET_GE) != DO_OK ) {
                    MY_ERROR("Невозможно найти склад \"%s\"", (char*)st->list[j]);
                    res = FALSE;
                }
                result = g_slist_append(result, g_memdup(&node, sizeof(node)));
            }
        }
        else
        if ( units ) {
            res = make_sklad(alias, objs);

            GSList *l;
            for ( l = units; res && l; l=l->next ) {
                const gchar *guid;
                guid = l->data;
                node.unit = find_exchange_object_by_guid(objs, guid);
                if ( !node.unit ) {
                    MY_ERROR("Не найден объект с %s %s", ELEM_1C_GUID, guid);
                    res = FALSE;
                }
                if ( res && node.unit->type != EXCHANGE_TYPE_HANDBOOK_UNIT ) {
                    OBJ_MY_ERROR(node.unit, "Неправильный тип подразделения");
                    res = FALSE;
                }
                if ( res ) {
                    gboolean error = FALSE;

                    if ( get_sklad(alias, &node.sklad_key0, guid, &error) )
                        result = g_slist_append(result, g_memdup(&node, sizeof(node)));
                    if ( error )
                        res = FALSE;
                }
/*
                do_text_set(alias, node.sklad_key0.code, l->data);
                status = do_sklad_get0(alias, &sklad, &node.sklad_key0, DO_GET_EQUAL);
                if ( status == DO_KEY_NOT_FOUND )
                    MY_ERROR("Не найден склад \"%s\"", (char*)l->data);
                if ( status != DO_OK  )
                    return FALSE;

                if ( do_sklad_param_int(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KIND)) != 2 ) {
                    MY_ERROR("Склад \"%s\" не является торговым", (char*)l->data);
                    return FALSE;
                }
                result = g_slist_append(result, g_memdup(&node, sizeof(node)));
*/
            }

        }
        else {
            do_text_set_empty(node.sklad_key0.code);
            status = do_sklad_get0(alias, &sklad, &node.sklad_key0, DO_GET_GE);
            while ( status == DO_OK ) {
                if ( !strcmp(do_text(alias, sklad.data.type_),"ТО") && do_sklad_param_int(alias, &sklad, do_param(DO_PARAM_SKLAD_PARAMETR_KIND)) == 2 ) {
                    result = g_slist_append(result, g_memdup(&node, sizeof(node)));
                }
                status = do_sklad_get0(alias, &sklad, &node.sklad_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR )
                res = FALSE;

        }
    }
    do_string_list_free(s);
    if ( !res ) {
        g_slist_foreach(result, (GFunc)close_alias, NULL);
        g_slist_foreach(result, (GFunc)g_free, NULL);
        g_slist_free(result);
        return NULL;
    }
    return result;

}
static void make_root_element(xmlNode *root, struct tm *tm1, struct tm *tm2, const char *comment)
{
    time_t now;
    struct tm tm;
    char buf[256];
    now = time(NULL);
    tm = *localtime(&now);

    xmlNewProp(root, BAD_CAST ELEM_1C_FORMAT_VERSION, BAD_CAST CONST_1C_FORMAT_VERSION);
    strftime(buf, 255, DATE_FORMAT_1C, &tm);
    xmlNewProp(root, BAD_CAST ELEM_1C_DATE_UPLOAD, BAD_CAST buf);
    strftime(buf, 255, DATE_FORMAT_1C, tm1);
    xmlNewProp(root, BAD_CAST ELEM_1C_DATE_START, BAD_CAST buf);
    strftime(buf, 255, DATE_FORMAT_1C, tm2);
    xmlNewProp(root, BAD_CAST ELEM_1C_DATE_END, BAD_CAST buf);
    xmlNewProp(root, BAD_CAST ELEM_1C_NAME_SOURCE, BAD_CAST CONST_1C_NAME_BOOK);
    xmlNewProp(root, BAD_CAST ELEM_1C_NAME_DEST, BAD_CAST CONST_1C_NAME_BOOK);
    //xmlNewProp(root, BAD_CAST ELEM_1C_GUID_RULES, BAD_CAST CONST_1C_GUID_RULES);//!!

    xmlNewProp(root, BAD_CAST ELEM_1C_COMMENT, BAD_CAST comment);

}
static void make_rules_element(xmlNode *rules)
{
    char buf[256];
    xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_FORMAT_VERSION, BAD_CAST CONST_1C_FORMAT_VERSION);
    //xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_ID, BAD_CAST CONST_1C_GUID_RULES);//!!
    sprintf(buf, "%s --&gt; %s", CONST_1C_NAME_BOOK, CONST_1C_NAME_BOOK);
    xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_NAME, BAD_CAST buf);
    xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_DATE_CREATED, BAD_CAST CONST_1C_DATE_CREATE);
    xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_SOURCE, BAD_CAST CONST_1C_NAME_BOOK);
    xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_DEST, BAD_CAST CONST_1C_NAME_BOOK);
    xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_PARAMS, BAD_CAST "");
    xmlNewChild(rules, NULL, BAD_CAST ELEM_1C_RUNTIME, BAD_CAST "");

    //xmlNewProp(rules, BAD_CAST ELEM_1C_, BAD_CAST CONST_1C_FORMAT_VERSION);

}

static gboolean make_rules_types(xmlNode *convert)
{
    char buf[256];
    xmlNode *rule;
    int i;
    for ( i = 0; i < DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_NO; i++ ) {
        gchar *str;
        rule = xmlNewChild(convert, NULL, BAD_CAST ELEM_1C_RULE, BAD_CAST "");
        xmlNewChild(rule, NULL, BAD_CAST ELEM_1C_CODE, BAD_CAST document_product_kind[i].name);
        //xmlNewChild(rule, NULL, BAD_CAST ELEM_1C_FIND_FIELD_ORDER, BAD_CAST CONST_1C_FIND_FIELD_ORDER_PRODUCT);
        str = g_strdup_printf(
                "СсылкаНаОбъект=Документы.%s.НайтиПоНомеру(СвойстваПоиска[\"Номер\"], СвойстваПоиска[\"Дата\"]); "
                "ПрекратитьПоиск = Истина; "
                , document_product_kind[i].name);

        xmlNewChild(rule, NULL, BAD_CAST ELEM_1C_FIND_FIELD_ORDER, BAD_CAST str);
        xmlNewChild(rule, NULL, BAD_CAST ELEM_1C_SYNC_FOR_ID, BAD_CAST CONST_1C_TRUE);
        sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, document_product_kind[i].name);
        xmlNewChild(rule, NULL, BAD_CAST ELEM_1C_SOURCE, BAD_CAST buf);
        xmlNewChild(rule, NULL, BAD_CAST ELEM_1C_DEST, BAD_CAST buf);
    }
    return TRUE;
}
static DoExchangeDocumentProductKind get_doc_kind(do_alias_t *alias, document_rec_t *document)
{
    gchar *dtype, *dtype1;
    dtype = do_text(alias, document->data.dtype);
    dtype1 = do_text(alias, document->data.dtype1);
    int j;


    for ( j = 0; j < G_N_ELEMENTS(document_dtype); j++ ) {
        if ( !g_strcmp0(dtype, document_dtype[j].dtype) &&
             !g_strcmp0(dtype1, document_dtype[j].dtype1) ) {
            return document_dtype[j].kind;
        }
    }
    return DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_NO;
}
static gboolean pulse(gpointer data)
{
    PROGRESS_PULSE();
    return TRUE;
}
/*
static GSList *get_protocol_docs(priv_t *priv, struct tm *tm1, struct tm *tm2, gint *crnt, gint total)
{
    GSList *docs = NULL, *l;
    do_alias_t *alias = priv->alias;
    protocol_rec_t protocol;
    protocol_key1_t protocol_key1, key;
    gint source;
    int status;

    do_date_set(&protocol_key1.date, *tm1);
    protocol_key1.time = 0;
    do_date_set(&key.date, *tm2);

    source = g_timeout_add(100, pulse, NULL);

    status = do_protocol_get1(alias, &protocol, &protocol_key1, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( protocol_key1.date > key.date ) break;
        if ( CANCELLED ) {
            g_slist_foreach(docs, (GFunc)g_free, NULL);
            g_slist_free(docs);
            return NULL;
        }

        do_protocol_context_t context;
        if ( do_protocol_parse(alias, &protocol, &context) &&
             context.object == DO_PROTOCOL_OBJECT_DOCUMENT // && context.action == DO_PROTOCOL_ACTION_ACCEPT
              )   {
            gboolean flag;
            flag = FALSE;
            for ( l=docs; l && !flag; l=l->next ) {
                document_key0_t *d;
                d = l->data;
                flag = !do_cmp(d->dtype, context.document_key0.dtype) &&
                       !do_cmp(d->sklad, context.document_key0.sklad) &&
                       !do_cmp(d->document, context.document_key0.document);
            }
            if ( !flag ) {
                docs = g_slist_append(docs, g_memdup(&context.document_key0, sizeof(context.document_key0)));
            }
        }
        status = do_protocol_get1(alias, &protocol, &protocol_key1, DO_GET_NEXT);
    }
    g_source_remove(source);
    if ( status == DO_ERROR ) {
        g_slist_foreach(docs, (GFunc)g_free, NULL);
        g_slist_free(docs);
        return NULL;
    }

    return docs;
}*/
static gboolean get_document(priv_t *priv, sklad_key0_t *sklad_key0, struct tm *tm1, struct tm *tm2, gboolean protocol, gint *crnt, gint total, GSList *protocols)
{
    document_key3_t document_key3, key;
    document_rec_t document;
    gboolean res = TRUE;
    do_alias_t *alias = priv->alias;
    DoExchangeDocumentProductKind kind;
    GSList *l, *docs=NULL, *k, *oper;
    int status;
    int i;
    for ( i = 0; res && i < G_N_ELEMENTS(document_product_dtypes); i++ ) {
        gboolean flag;
        if ( CANCELLED )
            return FALSE;
        do_text_set(alias, document_key3.dtype, document_product_dtypes[i]);
        if ( priv->operations ) {
            flag = FALSE;
            for ( oper = priv->operations; oper; oper=oper->next )
                if ( !do_cmp(document_key3.dtype,((document_type_key0_t*)oper->data)->dtype) ) {
                    flag = TRUE;
                    break;
                }
            if ( !flag )
                continue;
        }
        do_cpy(document_key3.sklad, sklad_key0->code);
        do_date_set(&document_key3.date, *tm1);
        document_key3.time = 0;
        do_cpy(key.dtype, document_key3.dtype);
        do_cpy(key.sklad, document_key3.sklad);
        do_date_set(&key.date, *tm2);

        status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( do_cmp(key.dtype, document_key3.dtype) ||
                 do_cmp(key.sklad, document_key3.sklad) ||
                 document_key3.date > key.date ) break;

            if ( CANCELLED )
                return FALSE;

            //printf("%d %d\n",*crnt, total);

            if ( document.data.accepted ) {
                if ( priv->operations ) {
                    flag = FALSE;
                    for ( oper = priv->operations; oper; oper=oper->next )
                        if ( !do_cmp(document.data.dtype,((document_type_key0_t*)oper->data)->dtype) &&
                             !do_cmp(document.data.dtype1,((document_type_key0_t*)oper->data)->dtype1) ) {
                            flag = TRUE;
                            break;
                        }
                }
                else
                    flag = TRUE;

                if ( flag ) {
                    kind = get_doc_kind(alias, &document);

                    if ( kind != DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_NO )  {
                        document_key0_t document_key0;
                        do_cpy(document_key0.dtype, document.data.dtype);
                        do_cpy(document_key0.sklad, document.data.sklad);
                        do_cpy(document_key0.document, document.data.document);

                        priv->doc_name = document_product_kind[kind].name;
                        priv->document = &document;
                        docs = g_slist_append(docs, g_memdup(&document_key0, sizeof(document_key0)));

                        (*crnt) = (*crnt) + 1;
                        PROGRESS_PROGRESS(*crnt, total);

                        res = document_product_kind[kind].func(priv);
                    }
                    if ( !res )
                        break;
                }
            }
            status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
            return FALSE;
    }
    if ( res && protocol && protocols ) {

        for ( k=protocols; k && res; k=k->next ) {
            document_key0_t *p;
            document_key3_t key;
            do_date_set(&key.date, *tm1);
            p = k->data;
            if ( !do_cmp(p->sklad, sklad_key0->code) ) {
                int  flag;
                flag = FALSE;
                for ( l=docs; l && !flag; l=l->next ) {
                    document_key0_t *d;
                    d = l->data;
                    flag = !do_cmp(d->dtype, p->dtype) &&
                           !do_cmp(d->sklad, p->sklad) &&
                           !do_cmp(d->document, p->document);
                }
                if ( !flag ) {
                    status = do_document_get0(alias, &document, p, DO_GET_EQUAL);
                    if ( status == DO_ERROR ) {
                        res = FALSE;
                        break;
                    }
                    if ( status == DO_OK ) {
                        kind = get_doc_kind(alias, &document);

                        if ( document.data.accepted &&
                             kind != DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_NO &&
                             !document_product_kind[kind].no_changed
                            )  {
                            priv->doc_name = document_product_kind[kind].name;
                            priv->document = &document;
                            docs = g_slist_append(docs, g_memdup(p, sizeof(*p)));

                            res = document_product_kind[kind].func(priv);
                            (*crnt) = (*crnt) + 1;
                            PROGRESS_PROGRESS(*crnt, total);
                        }
                        if ( !res )
                            break;
                    }
                }

            }
        }
    }
    g_slist_foreach(docs, (GFunc)g_free, NULL);
    g_slist_free(docs);
    return res;
}
static int break_func(void *data)
{
    return CANCELLED;
}
static gboolean get_document_get_total(priv_t *priv, sklad_key0_t *sklad_key0, struct tm *tm1, struct tm *tm2, gboolean protocol, document_type_key0_t *dtype, gint *total, GSList **docs)
{
    document_key3_t document_key3, key;
    document_key0_t document_key0;
    gboolean res = TRUE;
    do_alias_t *alias = priv->alias;
    int i,j;
    gint source;
    char *value, *store;
    store = do_text(alias, sklad_key0->code);
    do_string_list_t *names = NULL;
    if ( protocol )
        names = do_string_list_new();

    source = g_timeout_add(100, pulse, NULL);

    for ( i = 0; res && i < G_N_ELEMENTS(document_product_dtypes); i++ ) {
        if ( CANCELLED ) {
            res = FALSE;
            break;
        }
        do_text_set(alias, document_key3.dtype, document_product_dtypes[i]);
        if ( dtype && do_cmp(document_key3.dtype, dtype->dtype) )
            continue;
        do_cpy(document_key3.sklad, sklad_key0->code);
        do_date_set(&document_key3.date, *tm1);
        document_key3.time = 0;

        do_cpy(key.dtype, document_key3.dtype);
        do_cpy(key.sklad, document_key3.sklad);
        do_date_set(&key.date, *tm2);
        do_extended_t *extended;
        extended = do_extended_new(alias);
        do_extended_set_break_func(extended, break_func, NULL);
        do_extended_set_file_db(extended, DO_DB_DOCUMENT);
        do_extended_set_key(extended, 3, &document_key3);
        if ( dtype != NULL )
            do_extended_set_filter(extended, TRUE, 4, sizeof(key.dtype), sizeof(key.sklad), sizeof(key.date), sizeof(dtype->dtype1));
        else
            do_extended_set_filter(extended, TRUE, 3, sizeof(key.dtype), sizeof(key.sklad), sizeof(key.date));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &key.dtype);
        do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &key.sklad);
        do_extended_set_filter_term(extended, 2,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(document_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                dtype != NULL ? DO_EXTENDED_CONNECTION_AND : DO_EXTENDED_CONNECTION_NO,
                                &key.date);
        if ( dtype )
            do_extended_set_filter_term(extended, 3,
                                    DO_EXTENDED_TYPE_STRING,
                                    offsetof(document_struct_t, dtype1),
                                    DO_EXTENDED_COMP_EQUAL,
                                    DO_EXTENDED_CONNECTION_NO,
                                    &dtype->dtype1);

        if ( protocol )
            do_extended_set_fields(extended, 1, do_size_and_offset(document_struct_t, document));
        else
            do_extended_set_fields(extended, 1, 1, 0);

        do_ret_list_t *list = NULL;

        list = do_extended_get(extended, -1);
        if ( !list )
            res = FALSE;
        else {
            if ( protocol ) {
                struct {
                    BTI_CHAR document[10];
                } *ret;
                for ( j = 0; j < do_ret_list_count(list); j++ ) {
                    ret = do_ret_list_item_data(list, j);
                    value = do_text(alias, ret->document);
                    do_string_list_add(names, do_strdup_printf("ДОК.%s.%s.%s", document_product_dtypes[i], store, value));
                    do_free(value);
                    (*total) = (*total) + 1;
                }
                do_ret_list_free(list);
            }
            else {
                (*total) = (*total) + do_ret_list_count(list);
                do_ret_list_free(list);
            }
        }
        do_extended_free(extended);
    }
    if ( res && protocol ) {
        protocol_key1_t protocol_key1, key;

        do_date_set(&protocol_key1.date, *tm1);
        protocol_key1.time = 0;
        do_date_set(&key.date, *tm2);

        do_extended_t *extended;
        extended = do_extended_new(alias);
        do_extended_set_break_func(extended, break_func, NULL);
        do_extended_set_file_db(extended, DO_DB_PROTOCOL);
        do_extended_set_key(extended, 1, &protocol_key1);

        do_extended_set_filter(extended, TRUE, 1, sizeof(key.date));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(protocol_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &key.date);
        do_extended_set_fields(extended, 1, do_size_and_offset(protocol_struct_t, objectName));
        do_ret_list_t *list = NULL;

        list = do_extended_get(extended, -1);
        if ( !list )
            res = FALSE;
        else {

            do_string_list_t *pref;
            int indx;
            pref = do_string_list_new();
            for ( i = 0; res && i < G_N_ELEMENTS(document_product_dtypes); i++ ) {
                do_string_list_add(pref, do_strdup_printf("ДОК.%s.%s.", document_product_dtypes[i], store));
            }
            for ( j = 0; j < do_ret_list_count(list); j++ ) {
                protocol_key0_t *key0;
                key0 = do_ret_list_item_data(list, j);
                value = do_text(alias, key0->objectName);
                if ( !do_string_list_find(names, &indx, value)) {
                    for ( i = 0; i < pref->count; i++ )
                        if ( !strncmp(value, pref->list[i], strlen(pref->list[i])) ) {
                            (*total) = (*total) + 1;
                            char *ch;
                            do_string_list_add_alloc(names, value);
                            do_text_set(alias, document_key0.dtype, document_product_dtypes[i]);
                            do_text_set(alias, document_key0.sklad, store);
                            ch = value + strlen(value);
                            while ( ch > value && *ch != '.' )
                                ch--;
                            ch++;
                            do_text_set(alias, document_key0.document, ch);
                            *docs = g_slist_append(*docs, g_memdup(&document_key0, sizeof(document_key0)));

                            break;
                        }
                }
                do_free(value);
            }
            do_string_list_free(pref);
            do_ret_list_free(list);
        }

        do_extended_free(extended);
    }
    if ( names )
        do_string_list_free(names);
    do_free(store);
    g_source_remove(source);
    return res;
}
static gchar *document_number_to_1c(do_alias_t *alias, document_rec_t *document, ExchangeObject *firm)
{
    //gchar *dtype;
    gchar *sklad_str;
    gchar *number;
    const gchar *pref;
    int sklad;
    //dtype = do_text(alias, document->data.dtype);
    sklad_str = do_text(alias, document->data.sklad);
    sklad = atoi(sklad_str);


    number = do_text(alias, document->data.document);
    ExchangeProperty *prop;

    prop = exchange_object_get_property(firm, "Префикс");
    if ( !prop || prop->empty )
        pref = "";
    else
        pref = g_value_get_string(&prop->value);
    gint len;
    len = text_length(pref)+2/*text_length(sklad)*/+text_length(number);
    if ( len > CONST_1C_DOCUMENT_NUMBER_LEN ) {
        MY_ERROR("Номер документа \"%s%2.2d%s\" более %d символов", pref,sklad,number, CONST_1C_DOCUMENT_NUMBER_LEN);
        return NULL;
    }
    len = CONST_1C_DOCUMENT_NUMBER_LEN - len;
    gchar *res;

    res = g_strdup_printf("%s%2.2d%*.*d%s", pref, sklad, len, len, 0, number);
    //g_free(dtype);
    g_free(sklad_str);g_free(number);
    if ( text_length(res) > 11 ) {
        MY_ERROR("Номер документа \"%s\" более 11 символов", res);
        return NULL;
    }
    return res;
}
static gchar *document_number_to_1c_(do_alias_t *alias, document_rec_t *document, ExchangeObject *firm)
{
    //gchar *dtype;
    gchar *sklad_str;
    gchar *number;
    const gchar *pref;
    int sklad;
    //dtype = do_text(alias, document->data.dtype);
    sklad_str = do_text(alias, document->data.sklad);
    sklad = atoi(sklad_str);


    number = do_text(alias, document->data.document);
    ExchangeProperty *prop;

    prop = exchange_object_get_property(firm, "Префикс");
    if ( !prop || prop->empty )
        pref = "";
    else
        pref = g_value_get_string(&prop->value);
    gint len;
    len = text_length(pref)+3/*text_length(sklad)*/+text_length(number);
    if ( len > CONST_1C_DOCUMENT_NUMBER_LEN ) {
        MY_ERROR("Номер документа \"%s%2.2d%s\" более %d символов", pref,sklad,number, CONST_1C_DOCUMENT_NUMBER_LEN);
        return NULL;
    }
    len = CONST_1C_DOCUMENT_NUMBER_LEN - len;
    gchar *res;

    res = g_strdup_printf("%sB%2.2d%*.*d%s", pref, sklad, len, len, 0, number);
    //g_free(dtype);
    g_free(sklad_str);g_free(number);
    if ( text_length(res) > 11 ) {
        MY_ERROR("Номер документа \"%s\" более 11 символов", res);
        return NULL;
    }
    return res;
}

typedef struct {
    gboolean event;
    gboolean res;
    gboolean no_close;
    gboolean empty;

} dialog_close_t;

static gboolean check_(GtkWidget *widget)
{
    GtkWidget *d;
    gint res;
    d = gtk_message_dialog_new(GTK_WINDOW(widget), GTK_DIALOG_DESTROY_WITH_PARENT,
                               GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO,
                               "%s", "Прервать выполнение?");
    gtk_window_set_icon_name(GTK_WINDOW(d), "domino");
    res = gtk_dialog_run(GTK_DIALOG(d));
    gtk_widget_destroy(d);
    if ( res != GTK_RESPONSE_YES )
        return FALSE;
    return TRUE;
}
static gboolean partner_link_dialog_delete_event(GtkWidget *widget, GdkEvent *event, dialog_close_t *close)
{
    return close->no_close;
}
static void partner_link_dialog_response(GtkWidget *widget,
                                                        gint       response_id,
                                                        dialog_close_t *close)
{
    if ( response_id == GTK_RESPONSE_CANCEL ||
         response_id == GTK_RESPONSE_DELETE_EVENT ) {
        if ( !check_(widget) ) {
            if ( response_id == GTK_RESPONSE_DELETE_EVENT )
                close->no_close = TRUE;
            //gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_NONE);
            return;
        }
    }
    if ( response_id == GTK_RESPONSE_NO ||
         response_id == GTK_RESPONSE_OK ||
         (response_id == GTK_RESPONSE_CANCEL && close->empty ) ) {
        if ( !do_link_dialog_save(DO_LINK_DIALOG(widget), response_id == GTK_RESPONSE_NO) ) {
            SHOW_MESSAGE();
            return;
        }
    }
    g_object_get( widget, "saved", &close->res, NULL);
    gtk_widget_destroy(widget);
    close->event = FALSE;
}
static gboolean sad(GtkWidget *dialog)
{
    if ( get_progress() )
        gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(do_progress_info_get_window(get_progress())));
    gtk_widget_show(dialog);
    return FALSE;
}
static ExchangeObject *get_object_partner(do_alias_t *alias, partner_key0_t *partner_key0, GSList *objs, DoExchangeHandbook handbook, gboolean empty, document_rec_t *document, gchar *comment)
{
    partner_data_key0_t partner_data_key0;
    partner_data_rec_t partner_data;
    gchar *guid;
    int status;
    ExchangeObject *obj = NULL;

    do_cpy(partner_data_key0.g_code, partner_key0->g_code);
    partner_data_key0.code = partner_key0->code;
    partner_data_key0.type = PARTNER_DATA_DOMINO_TYPE;
    partner_data_key0.number = firm_guid_number;

    while ( TRUE ) {
        status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_EQUAL);
        if ( status == DO_ERROR )
            return NULL;
        if ( CANCELLED )
            return NULL;
        if ( status == DO_OK ) {
            guid = do_partner_data_param(alias, &partner_data, "GUID");
            if ( strcmp(guid, CONST_EMPTY_GUID) ) {
                obj = find_exchange_object_by_guid(objs, guid);
                if ( !obj ) {
                    MY_ERROR("Не найден объект с %s %s", ELEM_1C_GUID, guid);
                    SHOW_MESSAGE();
                }
                else
                    return obj;
            }
            else {
                if ( empty ) {
                    return (ExchangeObject *)-1;
                }
            }
        }
        if (  !obj ) {

            dialog_close_t close = {TRUE, FALSE, FALSE};

            SHOW_MESSAGE();
            close.empty = empty;

            gdk_threads_enter();
            GtkWidget *dialog;
            dialog = do_link_dialog_new(alias, objs, handbook, (void*)partner_key0);
            g_object_set(dialog, "empty", empty, NULL);
            if ( document && comment ) {
               gchar *str_;
               str_=g_strdup_printf("%s %s.%s.%s",comment,do_text(alias, document->data.dtype),do_text(alias, document->data.sklad),do_text(alias, document->data.document));
               g_object_set(dialog, "comment", str_, NULL);
	    }
            g_signal_connect(dialog, "response", G_CALLBACK(partner_link_dialog_response), &close);
            g_signal_connect(dialog, "delete-event", G_CALLBACK(partner_link_dialog_delete_event), &close);
            gdk_threads_leave();
            gdk_threads_add_idle((GSourceFunc)sad, dialog);
            wait_for(&close.event);

            if ( !close.res )
                return NULL;
        }
    }
    return NULL;
}

static ExchangeObject *get_object_partner_from_document(do_alias_t *alias, document_rec_t *document, GSList *objs)
{
    partner_key0_t partner_key0;
    do_cpy(partner_key0.g_code, document->data.p_g_code);
    partner_key0.code = document->data.p_code;
    return get_object_partner(alias, &partner_key0, objs, DO_EXCHANGE_HANDBOOK_PARTNER, FALSE, document, "Для партнера документа");

}
static gboolean make_property_const(xmlNode *root, const gchar *name, ExchangeType type, GValue *v)
{
    xmlNode *property;
    //xmlNode *value;
    char buf[256];

    property = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
    xmlNewProp(property, BAD_CAST ELEM_1C_NAME_S, BAD_CAST name);
    xmlNewProp(property, BAD_CAST ELEM_1C_TYPE, BAD_CAST exchange_type_get_name(type));
    switch ( type ) {
        case EXCHANGE_TYPE_CONST_BOOLEAN:
            sprintf(buf, "%s", g_value_get_boolean(v) ? CONST_1C_TRUE : CONST_1C_FALSE);
            //value =
            xmlNewChild(property, NULL, BAD_CAST ELEM_1C_VALUE, BAD_CAST buf);
            break;
        case EXCHANGE_TYPE_CONST_DATE: {
            struct tm *tm;
            tm = g_value_get_pointer(v);
            strftime(buf, 255, DATE_FORMAT_1C, tm);
            //value =
            xmlNewChild(property, NULL, BAD_CAST ELEM_1C_VALUE, BAD_CAST buf);
            break;
        }
        case EXCHANGE_TYPE_CONST_STRING:
            //value =
            xmlNewChild(property, NULL, BAD_CAST ELEM_1C_VALUE, BAD_CAST g_value_get_string(v));
            break;
        case EXCHANGE_TYPE_CONST_NUMBER:
            do_ftoa(g_value_get_double(v), buf);
            //value =
            xmlNewChild(property, NULL, BAD_CAST ELEM_1C_VALUE, BAD_CAST buf);
            break;
        default:
            MY_ERROR("Неправильный тип константы %d", type);
            return FALSE;
    }
    return TRUE;
}
static gboolean make_property_const_string(xmlNode *root, const gchar *name, const gchar *str)
{
    GValue value = {0,};
    g_value_init(&value, G_TYPE_STRING);
    g_value_set_string(&value, str);
    return make_property_const(root, name, EXCHANGE_TYPE_CONST_STRING, &value);
}
static gboolean make_property_list(xmlNode *root, const gchar *name, ExchangeType type, const gchar *str, GSList *objs)
{
    if ( exchange_type_get_kind(type) != EXCHANGE_TYPE_KIND_LIST ) {
        MY_ERROR("Неправильно указан тип перечесления \"%s\"", exchange_type_get_name(type));
        return FALSE;
    }
    /*if ( !find_exchange_list_by_value(objs, type, str) ) {
        MY_ERROR("Неправильное значение \"%s\" для перечесления \"%s\"", str, exchange_type_get_name(type));
        return FALSE;

    }*/
    xmlNode *property;
    //xmlNode *value;

    property = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
    xmlNewProp(property, BAD_CAST ELEM_1C_NAME_S, BAD_CAST name);
    xmlNewProp(property, BAD_CAST ELEM_1C_TYPE, BAD_CAST exchange_type_get_name(type));

    //value =
    xmlNewChild(property, NULL, BAD_CAST ELEM_1C_VALUE, BAD_CAST str);
    return TRUE;
}
static gboolean make_property_const_boolean(xmlNode *root, const gchar *name, gboolean v)
{
    GValue value = {0,};
    g_value_init(&value, G_TYPE_BOOLEAN);
    g_value_set_boolean(&value, v);
    return make_property_const(root, name, EXCHANGE_TYPE_CONST_BOOLEAN, &value);
}

static gboolean make_property_const_number(xmlNode *root, const gchar *name, gdouble v)
{
    GValue value = {0,};
    g_value_init(&value, G_TYPE_DOUBLE);
    g_value_set_double(&value, v);
    return make_property_const(root, name, EXCHANGE_TYPE_CONST_NUMBER, &value);
}
static gboolean make_property_const_date(xmlNode *root, const gchar *name, struct tm *tm)
{
    GValue value = {0,};
    gboolean res;
    g_value_init(&value, G_TYPE_POINTER);
    g_value_set_pointer(&value, tm);
    res = make_property_const(root, name, EXCHANGE_TYPE_CONST_DATE, &value);
    return res;
}
static gboolean make_property_empty(xmlNode *root, const gchar *name, ExchangeType type);
static gboolean make_property_object(xmlNode *root, const gchar *name, ExchangeObject *obj);
/*
static gboolean make_property_property(xmlNode *root, const gchar *name, GSList *objs, ExchangeType type, ExchangeProperty *prop, gboolean no_empty)
{
    if ( prop->empty && no_empty ) {
        PROP_MY_ERROR(prop, "Значение не должно быть пустым");
        return FALSE;
    }
    if ( prop->type != type ) {
        PROP_MY_ERROR(prop, "Неправильный тип \"%s\" ожидался \"%s\"", exchange_type_get_name(prop->type), exchange_type_get_name(type))
        return FALSE;
    }
    if ( prop->empty )
        return make_property_empty(root, name, type);

    ExchangeTypeKind kind;

    kind = exchange_type_get_kind(type);
    switch ( kind ) {
        case EXCHANGE_TYPE_KIND_CONST:
            return make_property_const(root, name, type, &prop->value);
        case EXCHANGE_TYPE_KIND_LIST:
            return make_property_list(root, name, type, g_value_get_string(&prop->value), objs);
        case EXCHANGE_TYPE_KIND_ATTRIBUTE:
        case EXCHANGE_TYPE_KIND_ACCOUNT:
        case EXCHANGE_TYPE_KIND_HANDBOOK: {
            ExchangeObject *obj;
            ExchangeKey *key;
            key = g_value_get_pointer(&prop->value);
            obj = find_exchange_object_by_key(objs, key);
            if ( !obj ) {
                PROP_MY_ERROR(prop, "Не найден объект с %s %s", ELEM_1C_GUID, key->guid);
                return FALSE;
            }
            return make_property_object(root, name, obj);
        }
        case EXCHANGE_TYPE_KIND_NO:
            MY_ERROR("Неправильный вид");
            return FALSE;
    }
    return FALSE;
}
*/
static gboolean make_property_object(xmlNode *root, const gchar *name, ExchangeObject *obj)
{
    xmlNode *property;


    property = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
    xmlNewProp(property, BAD_CAST ELEM_1C_NAME_S, BAD_CAST name);
    xmlNewProp(property, BAD_CAST ELEM_1C_TYPE, BAD_CAST exchange_type_get_name(obj->type));

    property = xmlNewChild(property, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");

    if ( !make_property_const_string(property, ELEM_1C_GUID, obj->key->guid) )
        return FALSE;

    switch ( obj->kind ) {
        case EXCHANGE_TYPE_KIND_HANDBOOK: {
            if ( !make_property_const_string(property, ELEM_1C_CODE, obj->key->code) )
                return FALSE;
            if ( !make_property_const_boolean(property, ELEM_1C_GROUP, obj->key->group) )
                return FALSE;
            break;
        }
        case EXCHANGE_TYPE_KIND_ATTRIBUTE:
        case EXCHANGE_TYPE_KIND_ACCOUNT: {
            if ( obj->key->name )
                if ( !make_property_const_string(property, ELEM_1C_CONST_NAME, obj->key->name) )
                    return FALSE;
            break;
        }
        case EXCHANGE_TYPE_KIND_CONST:
        case EXCHANGE_TYPE_KIND_LIST:
        case EXCHANGE_TYPE_KIND_NO:
            MY_ERROR("Неправильно указан вид");
            return FALSE;
    }


    return TRUE;
}
/*
static void make_property_object_string(xmlNode *root, const gchar *name, const gchar *type, const gchar *guid)
{
    xmlNode *property;


    property = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
    xmlNewProp(property, BAD_CAST ELEM_1C_NAME_S, BAD_CAST name);
    xmlNewProp(property, BAD_CAST ELEM_1C_TYPE, BAD_CAST type);

    property = xmlNewChild(property, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");

    make_property_const_string(property, ELEM_1C_GUID, guid);
}
*/
static gboolean make_property_empty(xmlNode *root, const gchar *name, ExchangeType type)
{
    xmlNode *property;

    property = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
    xmlNewProp(property, BAD_CAST ELEM_1C_NAME_S, BAD_CAST name);
    xmlNewProp(property, BAD_CAST ELEM_1C_TYPE, BAD_CAST exchange_type_get_name(type));

    property = xmlNewChild(property, NULL, BAD_CAST ELEM_1C_EMPTY, BAD_CAST "");

    return TRUE;
}
static gboolean make_property_empty_(xmlNode *root, const gchar *name, const gchar *type_name)
{
    xmlNode *property;

    property = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
    xmlNewProp(property, BAD_CAST ELEM_1C_NAME_S, BAD_CAST name);
    xmlNewProp(property, BAD_CAST ELEM_1C_TYPE, BAD_CAST type_name);

    property = xmlNewChild(property, NULL, BAD_CAST ELEM_1C_EMPTY, BAD_CAST "");

    return TRUE;
}
static ExchangeObject *get_object_firm_from_document(do_alias_t *alias, document_rec_t *document, GSList *objs)
{
    otdel_key0_t otdel_key0;
    otdel_rec_t otdel;
    partner_key0_t partner_key0;
    int status;

    do_text_set(alias, otdel_key0.code, domino_division());
    status = do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        MY_ERROR("Отдел не найден");
        return NULL;
    }
    char *ch;
    char *value = do_otdel_param(alias, &otdel, do_param(DO_PARAM_OTDEL_PARAMETR_FIRM));
    if ( (ch = strchr(value, '.')) == NULL ) {
        do_free(value);
        MY_ERROR("Фирма не определена");
        return NULL;
    }
    *ch = '\0';
    do_text_set(alias, partner_key0.g_code, value);
    partner_key0.code = atoi(ch + 1);
    do_free(value);

    return get_object_partner(alias, &partner_key0, objs, DO_EXCHANGE_HANDBOOK_FIRM, FALSE, document, "Для фирмы ");

}
static ExchangeObject *get_object_unit_from_document(do_alias_t *alias, document_rec_t *document, GSList *objs)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    partner_key0_t partner_key0;
    int status;

    do_cpy(sklad_key0.code, document->data.sklad);
    status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        MY_ERROR("Склад не найден");
        return NULL;
    }
    do_cpy(partner_key0.g_code, sklad.data.p_g_code);
    partner_key0.code = sklad.data.p_code;
    return get_object_partner(alias, &partner_key0, objs, DO_EXCHANGE_HANDBOOK_SKLAD, FALSE, document, g_strdup_printf("Для склада %s",do_alias_get_name(alias)));

}
static ExchangeObject *get_object_unit_from_partner(do_alias_t *alias, document_rec_t *document, GSList *objs)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    partner_key0_t partner_key0;
    partner_rec_t partner;
    int status;
    do_cpy(partner_key0.g_code, document->data.p_g_code);
    partner_key0.code = document->data.p_code;
    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        MY_ERROR("Партнер не найден");
        return NULL;
    }
    do_cpy(sklad_key0.code, partner.data.sklad);
    status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        MY_ERROR("Склад не найден");
        return NULL;
    }
    do_cpy(partner_key0.g_code, sklad.data.p_g_code);
    partner_key0.code = sklad.data.p_code;

    return get_object_partner(alias, &partner_key0, objs, DO_EXCHANGE_HANDBOOK_SKLAD, FALSE, document, "Для склада из документа ");

}

#define PROPERTY_DATE(root, name, value) {if ( !make_property_const_date(root, name, value) ) return FALSE;}
#define PROPERTY_STRING(root, name, value) {if ( !make_property_const_string(root, name, value) ) return FALSE;}
#define PROPERTY_NUMBER(root, name, value) {if ( !make_property_const_number(root, name, value) ) return FALSE;}
#define PROPERTY_BOOL(root, name, value) {if ( !make_property_const_boolean(root, name, value) ) return FALSE;}
#define PROPERTY_OBJECT(root, name, value) {if ( !make_property_object(root, name, value) ) return FALSE;}
#define PROPERTY_OBJECT_STR(root, name, type, guid) {make_property_object_string(root, name, type, guid);}
#define PROPERTY_LIST(root, name, type, value, objs) {if ( !make_property_list(root, name, type, value, objs) ) return FALSE;}
#define PROPERTY_EMPTY(root, name, type) {if ( !make_property_empty(root, name, type) ) return FALSE;}
#define PROPERTY_EMPTY_(root, name, type_name) {if ( !make_property_empty_(root, name, type_name) ) return FALSE;}
#define PROPERTY_PROPERTY(root, name, objs, type, prop, no_empty) {if ( !make_property_property(root, name, objs, type, prop, no_empty) ) return FALSE;}

static xmlNode *add_table(xmlNode *root, const gchar *name)
{
    xmlNode *res;
    res = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_TABLE, BAD_CAST "");
    xmlNewProp(res, BAD_CAST ELEM_1C_NAME_S, BAD_CAST name);
    return res;
}
static xmlNode *add_set(xmlNode *root, const gchar *name)
{
    xmlNode *res;
    res = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_SET, BAD_CAST "");
    xmlNewProp(res, BAD_CAST ELEM_1C_NAME_S, BAD_CAST name);
    return res;
}
static xmlNode *add_record(xmlNode *root)
{
    xmlNode *res;
    res = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_RECORD, BAD_CAST "");
    return res;
}
#define MONEY_IQUAL(a,b,c)  ((a > b - c) && (a < b + c) )

static ExchangeObject *get_partner_contract(GSList *objs, ExchangeObject *partner, const gchar *type_contract, ExchangeObject *firm)
{
    //ExchangeProperty *prop;
    ExchangeObject *contract = NULL;

    /*prop = exchange_object_get_property(partner, ELEM_1C_DEFAULT_CONTRACT);
    if ( !prop ) {
        OBJ_MY_ERROR(partner, "У контрагента \"%s\" не определен договор", exchange_object_get_name(partner));
        return NULL;
    }
    if ( !prop->empty ) {
        if ( prop->type != EXCHANGE_TYPE_HANDBOOK_CONTRACT ) {
            PROP_MY_ERROR(prop, "Не правильный тип договора. Логическая ошибка");
            return NULL;
        }
        ExchangeProperty *type;
        ExchangeObject *contact;
        ExchangeKey *key;
        key = g_value_get_pointer(&prop->value);
        contact = find_exchange_object_by_key(objs, key);
        if ( !contact ) {
            PROP_MY_ERROR(prop, "Не найден объект с %s %s", ELEM_1C_GUID, key->guid);
            return NULL;
        }
        type = exchange_object_get_property(contact, ELEM_1C_KIND_CONTRACT);
        if ( type && !type->empty &&
            !g_strcmp0(g_value_get_string(&type->value), type_contract) )
            return contact;
    }*/
    GSList *contracts, *l;
    //gboolean choose;
    contracts = find_exchange_objects_by_owner(objs, EXCHANGE_TYPE_HANDBOOK_CONTRACT, partner);
    //choose = g_slist_length(contracts) > 1;
    for ( l = contracts; l; l=l->next ) {
        ExchangeObject *obj;
        obj = l->data;
        //if ( choose ) {
            ExchangeProperty *prop;
            prop = exchange_object_get_property(obj, ELEM_1C_KIND_CONTRACT);
            if ( prop && !prop->empty &&
                !g_strcmp0(g_value_get_string(&prop->value), type_contract) ) {
                ExchangeProperty *firm_prop;
                firm_prop = exchange_object_get_property(obj, ELEM_1C_FIRM);

                if ( firm_prop && !firm_prop->empty ) {
                    ExchangeKey *key;
                    key = g_value_get_pointer(&firm_prop->value);
                    if ( !g_strcmp0(key->guid, firm->key->guid) ) {
                        contract = obj;
                        break;
                    }
                }
            }
        //}
        //else
            //contract = obj;

    }
    if ( !contract ) {
        //if ( choose )
            OBJ_MY_ERROR(partner, "У контрагента \"%s\" в списке договоров нет договора \"%s\" по организации \"%s\". Создайте такой договор",
                         exchange_object_get_name(partner),
                         type_contract,
                         exchange_object_get_name(firm) )
        //else
            //OBJ_MY_ERROR(partner, "У контрагента \"%s\" нет ни одного договора", exchange_object_get_name(partner));
        return NULL;
    }
    return contract;
}
static gchar *get_first_product_name(do_alias_t *alias, document_rec_t *document)
{
    document_order_key0_t document_order_key0;
    document_order_rec_t document_order;
    product_key4_t product_key4;
    product_rec_t product;
    int status;

    do_cpy(document_order_key0.dtype, document->data.dtype);
    do_cpy(document_order_key0.sklad, document->data.sklad);
    do_cpy(document_order_key0.document, document->data.document);
    document_order_key0.line = 0;

    status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
    if ( status != DO_OK ||
         do_cmp(document_order_key0.dtype, document->data.dtype) ||
         do_cmp(document_order_key0.sklad, document->data.sklad) ||
         do_cmp(document_order_key0.document, document->data.document)
        )
        return g_strdup("");
    do_cpy(product_key4.code, document_order.data.code);
    if ( do_product_get4(alias, &product, &product_key4, DO_GET_EQUAL) != DO_OK )
        return g_strdup("");
    return do_text(alias, product.data.name);
}
static gboolean get_document_product_kind_in(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    char buf[255];
    gchar *str;
    struct tm tm;
    ExchangeObject *partner;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit;
    double sum, sum_vat, sum_11;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;


    sum = do_document_sum(alias, document, 2);
    sum_11 = do_document_sum(alias, document, 11);
    sum_vat = do_document_param_float(alias, document, "30");

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;

    unit = get_object_unit_from_document(alias, document, objs);
    if ( !unit )
        return FALSE;

    partner = get_object_partner_from_document(alias, document, objs);
    if ( !partner )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);
    str = document_number_to_1c(alias, document, firm);
    if ( !str )
        return FALSE;

    PROPERTY_STRING(link, ELEM_1C_NUMBER, str);
    PROPERTY_DATE(link , ELEM_1C_DATE, &tm);
    PROPERTY_DATE(node, ELEM_1C_DOC_SOURCE_DATE, &tm);
    g_free(str);
    str = do_text(alias, document->data.p_doc);
    PROPERTY_STRING(node, ELEM_1C_DOC_SOURCE_NUMBER, str);
    g_free(str);


    PROPERTY_OBJECT(node, ELEM_1C_DOC_VALUTA, priv->valuta);

    PROPERTY_LIST(node, ELEM_1C_KIND_OPER, EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_IN, "ПокупкаКомиссия", objs);

    PROPERTY_EMPTY(node, ELEM_1C_CONSIGNER, EXCHANGE_TYPE_HANDBOOK_PARTNER);
    PROPERTY_EMPTY(node, ELEM_1C_CONSIGNEE, EXCHANGE_TYPE_HANDBOOK_PARTNER);
    PROPERTY_DATE(node, ELEM_1C_DATE, &tm);

    str=do_document_param(alias, document, "42");
    do_atodate(str, &tm);
    do_free(str);
    str=do_document_param(alias, document, "41");
    PROPERTY_STRING(node, ELEM_1C_INPUT_INVOICE_NUMBER, str);
    PROPERTY_DATE(node, ELEM_1C_INPUT_INVOICE_DATE, &tm);
    do_free(str);

    ExchangeObject *contract;
    contract = get_partner_contract(objs, partner, "СПоставщиком", firm);
    if ( !contract )
        return FALSE;
    PROPERTY_OBJECT(node, ELEM_1C_PARTNER_CONTRACT, contract);

    PROPERTY_STRING(node, ELEM_1C_KIND_CODE_OPER, CONST_1C_KIND_CODE_OPER_PRODUCT_IN);
    PROPERTY_NUMBER(node, ELEM_1C_KIND_CODE_IN, CONST_1C_KIND_CODE_IN);
    PROPERTY_STRING(node, ELEM_1C_COMMENT, get_first_product_name(alias, document));
    PROPERTY_OBJECT(node, ELEM_1C_PARTNER, partner);
    PROPERTY_NUMBER(node, "КратностьВзаиморасчетов", 1);
    PROPERTY_NUMBER(node, "КурсВзаиморасчетов", 1);
    PROPERTY_BOOL(node, "НДСВключенВСтоимость", TRUE);//!!
    PROPERTY_BOOL(node, "НДСПредъявленКВычету", FALSE);
    PROPERTY_OBJECT(node, ELEM_1C_FIRM, firm);
    if ( priv->user )
        PROPERTY_OBJECT(node, "Ответственный", priv->user)
    else
        PROPERTY_EMPTY(node, "Ответственный", EXCHANGE_TYPE_HANDBOOK_USER);
    PROPERTY_BOOL(node, "ПометкаУдаления", FALSE);
    PROPERTY_BOOL(node, "ПредъявленСчетФактура", TRUE);
    PROPERTY_BOOL(node, "Проведен", document->data.accepted);
    PROPERTY_BOOL(node, "РегистрироватьЦеныПоставщика", FALSE);
    PROPERTY_BOOL(node, "РучнаяКорректировка", FALSE);
    PROPERTY_OBJECT(node, ELEM_1C_UNIT, unit);
    PROPERTY_LIST(node, "СпособЗачетаАвансов", EXCHANGE_TYPE_LIST_METHOD_CLOSE_ADVANCE, "НеЗачитывать", objs);
    PROPERTY_BOOL(node, "СуммаВключаетНДС", TRUE);
    PROPERTY_NUMBER(node, ELEM_1C_DOC_SUM, sum);

    obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "РасчетыПоАвансамВыданным");
    if ( !obj ) {
        MY_ERROR("Не найден счет РасчетыПоАвансамВыданным в плане счетов");
        return FALSE;
    }
    PROPERTY_OBJECT(node, "СчетУчетаРасчетовПоАвансам", obj);
    PROPERTY_EMPTY(node, "СчетУчетаРасчетовПоТаре", EXCHANGE_TYPE_ACCOUNT_DEFAULT);
    obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "РасчетыСПоставщиками");
    if ( !obj ) {
        MY_ERROR("Не найден счет РасчетыСПоставщиками в плане счетов");
        return FALSE;
    }
    PROPERTY_OBJECT(node, "СчетУчетаРасчетовСКонтрагентом", obj);
    GSList *list = find_exchange_objects_by_code(objs, EXCHANGE_TYPE_HANDBOOK_PRICE_TYPE, CONST_1C_PRICE_PURCHASE);
    if ( !list ) {
        MY_ERROR("Не найден тип цен \"%s\"", CONST_1C_PRICE_PURCHASE);
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для типа цен с кодом \"%s\" более одной записи", CONST_1C_PRICE_PURCHASE);
        return FALSE;
    }
    obj = list->data;
    PROPERTY_OBJECT(node, "ТипЦен", obj);
    PROPERTY_BOOL(node, "УчитыватьНДС", FALSE);

    add_table(node, "ВозвратнаяТара");
    add_table(node, "ЗачетАвансов");
    add_table(node, "Оборудование");
    add_table(node, "ОбъектыСтроительства");
    table = add_table(node, "Товары");

    int i;
    double sum_=0, sum_vat_=0;
    struct {
        double price;
        double price_vat;
    } s[VAT_NO];
    vat_const_t const *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO; i++ ) {
        s[i].price = do_document_sum(alias, document, vat_const[i].sum_n);
        s[i].price_vat = do_document_sum(alias, document, vat_const[i].sum_vat_n);
        sum_+=s[i].price;
        sum_vat_+=s[i].price_vat;
    }
    if ( !MONEY_IQUAL(sum_, sum, 0.001) || //!! !MONEY_IQUAL(sum_vat, sum_vat_, 0.001) ||
         !MONEY_IQUAL(sum, sum_11, 0.001) ) {
        str = do_util_document_to_str(alias, document);
        MY_ERROR("Документ %s неправильный 2(%.5f,%.5f) 11(%.5f,%.5f) НДС(%.5f,%.5f)", str, sum_, sum, sum_11, sum, sum_vat,sum_vat_);
        g_free(str);
        return FALSE;
    }

    for ( i = 0; i < VAT_NO; i++ ) {
        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        //s[i].price +=100; //!!
        if ( s[i].price != 0 ) {
            record = add_record(table);
            PROPERTY_EMPTY(record,  "ЕдиницаИзмерения", EXCHANGE_TYPE_HANDBOOK_EI);
            PROPERTY_NUMBER(record, "Количество", 1);
            PROPERTY_EMPTY(record,  "КоличествоМест", EXCHANGE_TYPE_CONST_NUMBER);
            PROPERTY_NUMBER(record, "Коэффициент", 1);
            PROPERTY_OBJECT(record, "Номенклатура", obj);
            PROPERTY_EMPTY(record,  "НомерГТД", EXCHANGE_TYPE_HANDBOOK_DECLARATION);
            PROPERTY_EMPTY(record,  "ОтражениеВУСН", EXCHANGE_TYPE_LIST_SHOW_VUSN);
            PROPERTY_LIST(record,   "СтавкаНДС", EXCHANGE_TYPE_LIST_VAT_RATE, vat_const[i].name, objs);
            PROPERTY_EMPTY(record,  "СтранаПроисхождения", EXCHANGE_TYPE_HANDBOOK_COUNTRY);
            PROPERTY_NUMBER(record, "Сумма", s[i].price);
            PROPERTY_EMPTY(record,  "СуммаВРознице", EXCHANGE_TYPE_CONST_NUMBER);
            PROPERTY_NUMBER(record, "СуммаНДС", s[i].price_vat);
            obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
            if ( !obj ) {
                MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
                return FALSE;
            }
            PROPERTY_OBJECT(record, "СчетУчета", obj);
            obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "НДСпоПриобретеннымМПЗ");
            if ( !obj ) {
                MY_ERROR("Не найден счет НДСпоПриобретеннымМПЗ в плане счетов");
                return FALSE;
            }
            PROPERTY_OBJECT(record, "СчетУчетаНДС", obj);
            PROPERTY_NUMBER(record, "Цена", s[i].price);
            PROPERTY_EMPTY(record, "ЦенаВРознице", EXCHANGE_TYPE_CONST_NUMBER);
        }
    }
    add_table(node, "Услуги");
    return TRUE;
}
static gboolean get_document_product_kind_move(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    char buf[255];
    gchar *str;
    struct tm tm;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit_receiver;
    ExchangeObject *unit_sender;
    ExchangeObject *account;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;

    unit_receiver = get_object_unit_from_document(alias, document, objs);
    if ( !unit_receiver )
        return FALSE;

    unit_sender = get_object_unit_from_partner(alias, document, objs);
    if ( !unit_sender )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);

    str = document_number_to_1c(alias, document, firm);
    if ( !str )
        return FALSE;


    PROPERTY_STRING(link, ELEM_1C_NUMBER, str);
    PROPERTY_DATE(link, ELEM_1C_DATE, &tm);
    g_free(str);

    PROPERTY_LIST(node, ELEM_1C_KIND_OPER, EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_MOVE, "ТоварыПродукция", objs);

    PROPERTY_STRING(node, ELEM_1C_COMMENT, get_first_product_name(alias, document));
    PROPERTY_LIST(node, "НДСвСтоимостиТоваров", EXCHANGE_TYPE_LIST_METHOD_VAT_IN_PRICE, CONST_1C_METHOD_VAT_IN_PRICE, objs);

    PROPERTY_OBJECT(node, ELEM_1C_FIRM, firm);
    if ( priv->user )
        PROPERTY_OBJECT(node, "Ответственный", priv->user)
    else
        PROPERTY_EMPTY(node, "Ответственный", EXCHANGE_TYPE_HANDBOOK_USER);
    PROPERTY_BOOL(node, "ПометкаУдаления", FALSE);
    PROPERTY_BOOL(node, "Проведен", document->data.accepted);
    PROPERTY_OBJECT(node, "СкладОтправитель", unit_sender);
    PROPERTY_OBJECT(node, "СкладПолучатель", unit_receiver);
    PROPERTY_EMPTY(node, "СчетСписанияНДС", EXCHANGE_TYPE_ACCOUNT_DEFAULT);

    account = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
    if ( !account ) {
        MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
        return FALSE;
    }
    add_table(node, "ВозвратнаяТара");
    table = add_table(node, "Товары");

    int i;
    const vat_const_t *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO; i++ ) {
        double price;
        ExchangeProperty *prop;
        ExchangeObject *unit;
        ExchangeKey *key;
        price = do_document_sum(alias, document, vat_const[i].sum_r);//!!
        if ( !price )
            continue;

        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        prop = exchange_object_get_property(obj, "БазоваяЕдиницаИзмерения");
        if ( !prop || prop->empty ) {
            OBJ_MY_ERROR(obj, "У товара не задана базовая единица измерения");
            return FALSE;
        }
        key = g_value_get_pointer(&prop->value);
        unit = find_exchange_object_by_key(objs, key);
        if ( !unit ) {
            OBJ_MY_ERROR(obj, "Не найдена единица измерения \"%s\"", key->guid);
            return FALSE;
        }
        record = add_record(table);
        PROPERTY_OBJECT(record, "ЕдиницаИзмерения", unit);
        PROPERTY_NUMBER(record, "Количество", 1);
        PROPERTY_EMPTY(record,  "КоличествоМест", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "Коэффициент", 1);
        PROPERTY_OBJECT(record, "НовыйСчетУчета", account);
        PROPERTY_OBJECT(record, "Номенклатура", obj);
        PROPERTY_NUMBER(record, "Себестоимость", price);
        PROPERTY_EMPTY(record,  "СтавкаНДСВРознице", EXCHANGE_TYPE_LIST_VAT_RATE);
        PROPERTY_EMPTY(record,  "СуммаВРознице", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_OBJECT(record, "СчетУчета", account);
        PROPERTY_NUMBER(record, "Цена", price);
        PROPERTY_EMPTY(record,  "ЦенаВРознице", EXCHANGE_TYPE_CONST_NUMBER);
    }
    return TRUE;
}
static gboolean get_document_product_kind_surplus(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    char buf[255];
    gchar *str_number;
    struct tm tm;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit;
    ExchangeObject *account;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;
    unit = get_object_unit_from_document(alias, document, objs);
    if ( !unit )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    //xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);
    str_number = document_number_to_1c(alias, document, firm);
    if ( !str_number )
        return FALSE;

    PROPERTY_STRING(link, ELEM_1C_NUMBER, str_number);
    PROPERTY_DATE(link, ELEM_1C_DATE, &tm);

    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, "ИнвентаризацияТоваровНаСкладе");
    PROPERTY_STRING(node, "ИнвентаризацияТоваровНаСкладе", buf);
    PROPERTY_STRING(node, ELEM_1C_COMMENT, "Перенесено из Домино");
    PROPERTY_OBJECT(node, ELEM_1C_FIRM, firm);
    PROPERTY_EMPTY(node, "Основание", EXCHANGE_TYPE_CONST_STRING);
    if ( priv->user )
        PROPERTY_OBJECT(node, "Ответственный", priv->user)
    else
        PROPERTY_EMPTY(node, "Ответственный", EXCHANGE_TYPE_HANDBOOK_USER);
    PROPERTY_BOOL(node, "ПометкаУдаления", FALSE);
    PROPERTY_BOOL(node, "Проведен", document->data.accepted);
    PROPERTY_BOOL(node, "РучнаяКорректировка", TRUE);
    PROPERTY_OBJECT(node, "Склад", unit);
    GSList *list;
    list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_OTHER_PROFIT_OUTLAY, "Излишки");
    if ( !list ) {
        MY_ERROR("Не найдена статья прочих прибылей затрат \"Излишки\"");
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для статьи прочих прибылей и затрат \"Излишки\"  более одной записи");
        return FALSE;
    }
    obj = list->data;

    PROPERTY_OBJECT(node, "СтатьяПрочихДоходовРасходов", obj);
    double sum;
    sum = do_document_sum(alias, document, 11);
    PROPERTY_NUMBER(node, "СуммаДокумента", sum);
    PROPERTY_EMPTY(node, "СуммаДокументаРегл", EXCHANGE_TYPE_CONST_NUMBER);
    list = find_exchange_objects_by_code(objs, EXCHANGE_TYPE_HANDBOOK_PRICE_TYPE, CONST_1C_PRICE_PURCHASE);
    if ( !list ) {
        MY_ERROR("Не найден тип цен \"%s\"", CONST_1C_PRICE_PURCHASE);
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для типа цен с кодом \"%s\" более одной записи", CONST_1C_PRICE_PURCHASE);
        return FALSE;
    }
    obj = list->data;
    PROPERTY_OBJECT(node, "ТипЦен", obj);


    account = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
    if ( !account ) {
        MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
        return FALSE;
    }
    table = add_table(node, "Товары");
    int i;

    double other_sums[VAT_NO], other_sum;
    get_other_mismatch_sums(priv->alias_2, document, other_sums, &other_sum);//
    //    MY_ERROR("Невозможно найти документ списания недостачи для документа %s", do_util_document_to_str(alias, document));
    //    return FALSE;
    //}
    double all_sum = 0;
    const vat_const_t *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO && (sum > other_sum); i++ )
        all_sum += do_document_sum(alias, document, vat_const[i].sum_r);


    for ( i = 0; i < VAT_NO && (sum > other_sum); i++ ) {
        double price;
        ExchangeProperty *prop;
        ExchangeObject *unit;
        ExchangeKey *key;
        price = do_document_sum(alias, document, vat_const[i].sum_r);//!!
        if ( other_sum > all_sum )
            price = 0;
        else
            price -= other_sums[i];

        if ( price <= 0.001 && price >= -0.001 )
            continue;
        price = 1;

        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        prop = exchange_object_get_property(obj, "БазоваяЕдиницаИзмерения");
        if ( !prop || prop->empty ) {
            OBJ_MY_ERROR(obj, "У товара не задана базовая единица измерения");
            return FALSE;
        }
        key = g_value_get_pointer(&prop->value);
        unit = find_exchange_object_by_key(objs, key);
        if ( !unit ) {
            OBJ_MY_ERROR(obj, "Не найдена единица измерения \"%s\"", key->guid);
            return FALSE;
        }
        record = add_record(table);
        PROPERTY_OBJECT(record, "ЕдиницаИзмерения", unit);
        PROPERTY_NUMBER(record, "Количество", 1);
        PROPERTY_EMPTY(record,  "КоличествоМест", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "Коэффициент", 1);
        PROPERTY_OBJECT(record, "Номенклатура", obj);
        PROPERTY_EMPTY(record,  "НомерГТД", EXCHANGE_TYPE_HANDBOOK_DECLARATION);
        PROPERTY_EMPTY(record,  "СтавкаНДСВРознице", EXCHANGE_TYPE_LIST_VAT_RATE);
        PROPERTY_EMPTY(record,  "СтранаПроисхождения", EXCHANGE_TYPE_HANDBOOK_COUNTRY);
        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_EMPTY(record,  "СуммаВРознице", EXCHANGE_TYPE_LIST_VAT_RATE);
        PROPERTY_OBJECT(record, "СчетУчета", account);
        PROPERTY_NUMBER(record, "Цена", price);
        PROPERTY_EMPTY(record,  "ЦенаВРознице", EXCHANGE_TYPE_CONST_NUMBER);
    }

    table = add_set(node, "Хозрасчетный");

    ExchangeObject *account_dt, *account_dt_sub_key1, *account_dt_sub_key2;
    ExchangeObject *account_kt, *account_kt_sub_key, *account_kt_sub_value;

    account_dt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
    if ( !account_dt ) {
        MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
        return FALSE;
    }
    account_dt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Номенклатура");
    if ( !account_dt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто Номенклатура");
        return FALSE;
    }
    account_dt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Склады");
    if ( !account_dt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто Склады");
        return FALSE;
    }

    account_kt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ПрочиеДоходы");
    if ( !account_kt ) {
        MY_ERROR("Не найден счет ПрочиеДоходы в плане счетов");
        return FALSE;
    }
    account_kt_sub_key = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "ПрочиеДоходыИРасходы");
    if ( !account_kt_sub_key ) {
        MY_ERROR("Не найден вид субконто СтатьиЗатрат");
        return FALSE;
    }

    list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_OTHER_PROFIT_OUTLAY, "Излишки");
    if ( !list ) {
        MY_ERROR("Не найдена прочих доховом расходов \"Излишки\"");
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для прочих доходов расходов \"Излишки\" более одной записи");
        return FALSE;
    }
    account_kt_sub_value = list->data;


    for ( i = 0; i < VAT_NO; i++ ) {
        double price;
        xmlNode *sub;
        price = do_document_sum(alias, document, vat_const[i].sum_r);//!!
        if ( other_sum > all_sum )
            price = 0;
        else
            price -= other_sums[i];

        if ( price <= 0.001 && price >= -0.001 )
            continue;

        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        record = add_record(table);
        PROPERTY_BOOL(record, "Активность", TRUE);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "КоличествоКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "КоличествоДт", 1);
        PROPERTY_BOOL(record, "НеКорректироватьСтоимостьАвтоматически", TRUE);
        PROPERTY_OBJECT(record, "Организация", firm);
        PROPERTY_DATE(record, "Период", &tm);
        //Регистратор
        node = xmlNewChild(record, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
        xmlNewProp(node, BAD_CAST ELEM_1C_NAME_S, BAD_CAST "Регистратор");
        xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST "ДокументСсылка.СписаниеТоваров");
        link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
        sprintf(buf, "%d", priv->npp);
        xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
        PROPERTY_STRING(link, ELEM_1C_NUMBER, str_number);
        PROPERTY_DATE(link , ELEM_1C_DATE, &tm);
        // !Регистратор
        //PROPERTY_STRING(record, "Содержание", comment);

        PROPERTY_OBJECT(record, "СчетДт", account_dt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key1);
        PROPERTY_OBJECT(sub, "Значение", obj);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key2);
        PROPERTY_OBJECT(sub, "Значение", unit);


        PROPERTY_OBJECT(record, "СчетКт", account_kt);

        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key);
        PROPERTY_OBJECT(sub, "Значение", account_kt_sub_value);


        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_NUMBER(record, "СуммаНУДт", price);
        PROPERTY_NUMBER(record, "СуммаНУКт", price);
        PROPERTY_EMPTY(record, "СуммаВРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаВРКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРКт", EXCHANGE_TYPE_CONST_NUMBER);

    }
    g_free(str_number);

    return TRUE;
}
static gboolean get_document_product_kind_resort(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    xmlNode *sub;
    char buf[255];
    gchar *str;
    struct tm tm;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit;
    ExchangeObject *account_dt, *account_dt_sub_key, *account_dt_sub_value;
    ExchangeObject *account_kt, *account_kt_sub_key1,*account_kt_sub_key2;
    GSList *list;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;
    unit = get_object_unit_from_document(alias, document, objs);
    if ( !unit )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    //!!xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    //!!xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);
    str = document_number_to_1c(alias, document, firm);
    if ( !str )
        return FALSE;

    PROPERTY_STRING(link, ELEM_1C_NUMBER, str);
    PROPERTY_DATE(link, ELEM_1C_DATE, &tm);
    g_free(str);

    PROPERTY_STRING(node, ELEM_1C_COMMENT, "Перенесено из Домино");
    PROPERTY_OBJECT(node, ELEM_1C_FIRM, firm);
    PROPERTY_STRING(node ,"Содержание", "Пересорт товаров");
    PROPERTY_STRING(node ,"СпособЗаполнения", "Вручную");
    PROPERTY_NUMBER(node ,"СуммаОперации", do_document_sum(alias, document, 11));

    account_dt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "СебестоимостьПродажЕНВД");
    if ( !account_dt ) {
        MY_ERROR("Не найден счет СебестоимостьПродажЕНВД в плане счетов");
        return FALSE;
    }
    account_dt_sub_key = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "НоменклатурныеГруппы");
    if ( !account_dt_sub_key ) {
        MY_ERROR("Не найден вид субконто НоменклатурныеГруппы");
        return FALSE;
    }
    list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_PRODUCT_GROUP, "Розница");
    if ( !list ) {
        MY_ERROR("Не найдена номенклатурная группа \"Розница\"");
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для номерклатурной группы \"Розница\" более одной записи");
        return FALSE;
    }
    account_dt_sub_value = list->data;

    account_kt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
    if ( !account_kt ) {
        MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
        return FALSE;
    }
    account_kt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Номенклатура");
    if ( !account_kt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто Номенклатура");
        return FALSE;
    }
    account_kt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Склады");
    if ( !account_kt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто Склады");
        return FALSE;
    }

    table = add_set(node, "Хозрасчетный");
    int i;
    const vat_const_t *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO; i++ ) {
        double price;
        price = -do_document_sum(alias, document, vat_const[i].sum_r);//!!
        if ( !price )
            continue;

        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;

        record = add_record(table);
        PROPERTY_DATE(record, "Период", &tm);
        PROPERTY_OBJECT(record, "СчетДт", account_dt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key);
        PROPERTY_OBJECT(sub, "Значение", account_dt_sub_value);

        PROPERTY_OBJECT(record, "СчетКт", account_kt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key1);
        PROPERTY_OBJECT(sub, "Значение", obj);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key2);
        PROPERTY_OBJECT(sub, "Значение", unit);

        PROPERTY_OBJECT(record, "Организация", firm);
        PROPERTY_EMPTY(record, "ВалютаДт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_EMPTY(record, "ВалютаКт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаКт", EXCHANGE_TYPE_CONST_NUMBER);
        //PROPERTY_NUMBER(record, "КоличествоДт", 1);
        PROPERTY_NUMBER(record, "КоличествоКт", 1);

        PROPERTY_EMPTY(record, "Содержание", EXCHANGE_TYPE_CONST_STRING);
        PROPERTY_EMPTY(record, "СуммаНУДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРДт", price);
        PROPERTY_EMPTY(record, "СуммаНУКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРКт", price);
        PROPERTY_BOOL(record, "Активность", TRUE);
    }
    return TRUE;
}
static gboolean check_exist_debit_account_45(do_alias_t *alias, document_rec_t *document_rec)
{
    prowod_key4_t prowod_key4, prowod_key;
    prowod_rec_t prowod;
    char *prowodkey, *param_doc;
    char *dtype, *sklad, *document;
    char *account;
    gboolean retval = FALSE;
    int status;

    dtype = do_text(alias, document_rec->data.dtype);
    sklad = do_text(alias, document_rec->data.sklad);
    document = do_text(alias, document_rec->data.document);
    prowodkey = (char*)do_malloc(strlen(dtype) + strlen(sklad) + strlen(document) + 3 + 1);
    sprintf(prowodkey, "@%s.%s/%s", document, dtype, sklad);
    do_free(dtype);do_free(sklad);do_free(document);

    do_text_set(alias, prowod_key4.division, domino_division());
    do_text_set(alias, prowod_key4.document, prowodkey);
    prowod_key4.date = INT_MIN;

    memcpy(&prowod_key, &prowod_key4, sizeof(prowod_key4.division) + sizeof(prowod_key4.document));

	status = do_prowod_get4(alias, &prowod, &prowod_key4, DO_GET_GE);
	while (status == DO_OK) {
	    if (
            memcmp(&prowod_key, &prowod_key4, sizeof(prowod_key4.division) + sizeof(prowod_key4.document))
           )
           break;

        param_doc = do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_DOCUMENT));
        if ( !strcmp(param_doc, prowodkey) ) {
            account = do_text(alias, prowod.data.debit_account);
            if ( !strcmp(account, "45") )
                retval = TRUE;
        }
        do_free(param_doc);
        if ( retval )
            break;

        status = do_prowod_get4(alias, &prowod, &prowod_key4, DO_GET_NEXT);
	}
	return retval;
}
static gboolean get_document_product_kind_back(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    char buf[255];
    gchar *str;
    struct tm tm;
    ExchangeObject *partner;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit;
    GSList *list;
    double sum, sum_11;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;


    sum = do_document_sum(alias, document, 2);
    sum_11 = do_document_sum(alias, document, 11);

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;

    unit = get_object_unit_from_document(alias, document, objs);
    if ( !unit )
        return FALSE;

    partner = get_object_partner_from_document(alias, document, objs);
    if ( !partner )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);
    str = document_number_to_1c(alias, document, firm);
    if ( !str )
        return FALSE;

    PROPERTY_STRING(link, ELEM_1C_NUMBER, str);
    PROPERTY_DATE(link , ELEM_1C_DATE, &tm);
    g_free(str);


    PROPERTY_OBJECT(node, "ВалютаДокумента", priv->valuta);
    PROPERTY_LIST(node, ELEM_1C_KIND_OPER, EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_BACK, "ПокупкаКомиссия", objs);

    PROPERTY_EMPTY(node, "Грузоотправитель", EXCHANGE_TYPE_HANDBOOK_PARTNER);
    PROPERTY_EMPTY(node, "Грузополучатель", EXCHANGE_TYPE_HANDBOOK_PARTNER);

    ExchangeObject *contract;
    contract = get_partner_contract(objs, partner, "СПоставщиком", firm);
    if ( !contract )
        return FALSE;
    PROPERTY_OBJECT(node, "ДоговорКонтрагента", contract);
    str = do_document_param(alias, document, "37");

    PROPERTY_STRING(node, "Комментарий", str);
    do_free(str);
    PROPERTY_OBJECT(node, "Контрагент", partner);

    PROPERTY_NUMBER(node, "КратностьВзаиморасчетов", 1);
    PROPERTY_NUMBER(node, "КурсВзаиморасчетов", 1);
    PROPERTY_BOOL(node, "НДСВключенВСтоимость", TRUE);
    PROPERTY_OBJECT(node, "Организация", firm);
    if ( priv->user )
        PROPERTY_OBJECT(node, "Ответственный", priv->user)
    else
        PROPERTY_EMPTY(node, "Ответственный", EXCHANGE_TYPE_HANDBOOK_USER);
    PROPERTY_BOOL(node, "ПометкаУдаления", FALSE);
    PROPERTY_BOOL(node, "ПоставщикуВыставляетсяСчетФактураНаВозврат", TRUE);
    PROPERTY_BOOL(node, "Проведен", document->data.accepted);
    PROPERTY_BOOL(node, "РучнаяКорректировка", FALSE);
/*!!
*/
    PROPERTY_EMPTY_(node, "Сделка", "ДокументСсылка.ПоступлениеТоваровУслуг");
    PROPERTY_OBJECT(node, "Склад", unit);
    obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_HANDBOOK_OTHER_PROFIT_OUTLAY, "РазницыСтоимостиВозвратаИФактическойСтоимостиТоваров");
    if ( !obj ) {
        MY_ERROR("Не вид РазницыСтоимостиВозвратаИФактическойСтоимостиТоваров в прочих доходах расходах");
        return FALSE;
    }
    PROPERTY_OBJECT(node, "СтатьяДоходовИРасходов", obj);
    PROPERTY_BOOL(node, "СуммаВключаетНДС", TRUE);
    PROPERTY_NUMBER(node, "СуммаДокумента", sum);

    obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ПрочиеДоходы");
    if ( !obj ) {
        MY_ERROR("Не найден счет ПрочиеДоходы в плане счетов");
        return FALSE;
    }
    PROPERTY_OBJECT(node, "СчетУчетаДоходов", obj);
    obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ПрочиеРасходы");
    if ( !obj ) {
        MY_ERROR("Не найден счет ПрочиеРасходы в плане счетов");
        return FALSE;
    }
    PROPERTY_OBJECT(node, "СчетУчетаРасходов", obj);

    /*obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "РасчетыПоПретензиям");
    if ( !obj ) {
        MY_ERROR("Не найден счет РасчетыПоПретензиям в плане счетов");
        return FALSE;
    }
    PROPERTY_OBJECT(node, "СчетУчетаРасчетовПоПретензиям", obj);*/

    obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "РасчетыСПрочимиПоставщикамиИПодрядчиками");
    if ( !obj ) {
        MY_ERROR("Не найден счет РасчетыСПрочимиПоставщикамиИПодрядчиками в плане счетов");
        return FALSE;
    }
    PROPERTY_OBJECT(node, "СчетУчетаРасчетовПоТаре", obj);

    if ( check_exist_debit_account_45(alias, document) ) {
        obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ПрочиеТоварыОтгруженные");
        if ( !obj ) {
            MY_ERROR("Не найден счет ПрочиеТоварыОтгруженные в плане счетов");
            return FALSE;
        }
    }
    else {
        obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "РасчетыСПоставщиками");
        if ( !obj ) {
            MY_ERROR("Не найден счет РасчетыСПоставщиками в плане счетов");
            return FALSE;
        }
    }
    PROPERTY_OBJECT(node, "СчетУчетаРасчетовСКонтрагентом", obj);
    PROPERTY_OBJECT(node, "СчетУчетаРасчетовПоПретензиям", obj);

    list = find_exchange_objects_by_code(objs, EXCHANGE_TYPE_HANDBOOK_PRICE_TYPE, CONST_1C_PRICE_PURCHASE);
    if ( !list ) {
        MY_ERROR("Не найден тип цен \"%s\"", CONST_1C_PRICE_PURCHASE);
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для типа цен с кодом \"%s\" более одной записи", CONST_1C_PRICE_PURCHASE);
        return FALSE;
    }
    obj = list->data;
    PROPERTY_OBJECT(node, "ТипЦен", obj);
    PROPERTY_BOOL(node, "УчитыватьНДС", FALSE);

    add_table(node, "ВозвратнаяТара");
    table = add_table(node, "Товары");

    int i;
    double sum_=0, sum_vat_=0;
    struct {
        double price;
        double price_vat;
    } s[VAT_NO];
    const vat_const_t *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO; i++ ) {
        s[i].price = do_document_sum(alias, document, vat_const[i].sum_r);
        s[i].price_vat = (s[i].price/(100.+vat_const[i].proc))*vat_const[i].proc;
        sum_+=s[i].price;
        sum_vat_+=s[i].price_vat;
    }
    if ( !MONEY_IQUAL(sum_, sum, 0.001) ||
         !MONEY_IQUAL(sum, sum_11, 0.001) ) {
        str = do_util_document_to_str(alias, document);
        MY_ERROR("Документ %s неправильный 2(%.5f,%.5f) 11(%.5f,%.5f)", str, sum_, sum, sum_11, sum);
        g_free(str);
        return FALSE;
    }

    for ( i = 0; i < VAT_NO; i++ ) {
        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        //s[i].price +=100; //!!
        if ( s[i].price != 0 ) {
            record = add_record(table);
            PROPERTY_EMPTY(record,  "ЕдиницаИзмерения", EXCHANGE_TYPE_HANDBOOK_EI);
            PROPERTY_NUMBER(record, "Количество", 1);
            PROPERTY_EMPTY(record,  "КоличествоМест", EXCHANGE_TYPE_CONST_NUMBER);
            PROPERTY_NUMBER(record, "Коэффициент", 1);
            PROPERTY_OBJECT(record, "Номенклатура", obj);
            PROPERTY_EMPTY(record,  "НомерГТД", EXCHANGE_TYPE_HANDBOOK_DECLARATION);
            PROPERTY_NUMBER(record, "Себестоимость", s[i].price);
            PROPERTY_LIST(record,   "СтавкаНДС", EXCHANGE_TYPE_LIST_VAT_RATE, vat_const[i].name, objs);
            PROPERTY_EMPTY(record,  "СтавкаНДСВРознице", EXCHANGE_TYPE_LIST_VAT_RATE);
            PROPERTY_EMPTY(record,  "СтранаПроисхождения", EXCHANGE_TYPE_HANDBOOK_COUNTRY);
            PROPERTY_NUMBER(record, "Сумма", s[i].price);
            PROPERTY_EMPTY(record,  "СуммаВРознице", EXCHANGE_TYPE_CONST_NUMBER);
            PROPERTY_NUMBER(record, "СуммаНДС", s[i].price_vat);
            obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
            if ( !obj ) {
                MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
                return FALSE;
            }
            PROPERTY_OBJECT(record, "СчетУчета", obj);
            obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "НДСпоПриобретеннымМПЗ");
            if ( !obj ) {
                MY_ERROR("Не найден счет НДСпоПриобретеннымМПЗ в плане счетов");
                return FALSE;
            }
            PROPERTY_OBJECT(record, "СчетУчетаНДС", obj);
            PROPERTY_NUMBER(record, "Цена", s[i].price);
            PROPERTY_EMPTY(record, "ЦенаВРознице", EXCHANGE_TYPE_CONST_NUMBER);
        }
    }
    return TRUE;
}
static ExchangeObject *find_account_by_const_name(GSList *objs, const gchar *name)
{
    ExchangeObject *obj;
    obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, name);
    if ( !obj )
        MY_ERROR("Не найден счет \"%s\" в плане счетов", name);
    return obj;
}

static gboolean get_document_product_kind_del(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    xmlNode *sub;
    char buf[255];
    gchar *str_number;
    struct tm tm;
    ExchangeObject *partner;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit;
    double sum;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;


    sum = do_document_sum(alias, document, 11);

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;

    unit = get_object_unit_from_document(alias, document, objs);
    if ( !unit )
        return FALSE;

    partner = get_object_partner_from_document(alias, document, objs);
    if ( !partner )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    //xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);
    str_number = document_number_to_1c(alias, document, firm);
    if ( !str_number )
        return FALSE;

    PROPERTY_STRING(link, ELEM_1C_NUMBER, str_number);
    PROPERTY_DATE(link , ELEM_1C_DATE, &tm);

    PROPERTY_LIST(node, ELEM_1C_KIND_OPER, EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_DEL, "ТоварыПродукция", objs);
    PROPERTY_EMPTY_(node, "ИнвентаризацияТоваровНаСкладе", "ДокументСсылка.ИнвентаризацияТоваровНаСкладе");
    gchar *dtype1, *comment, *reason;
    dtype1 = do_text(alias, document->data.dtype1);
    if ( !g_strcmp0(dtype1, "11") ) {
        comment = g_strdup("Списание недостачи");
        reason = g_strdup("Инвентаризация");
    }
    if ( !g_strcmp0(dtype1, "14") ) {
        comment = g_strdup("Списание порча, брак, сроки годности");
        reason = g_strdup("");
    }
    if ( !g_strcmp0(dtype1, "15") ) {
        comment = g_strdup("Списание на прочие расходы");
        reason = g_strdup("");
    }
    PROPERTY_STRING(node, "Комментарий", comment);
    PROPERTY_OBJECT(node, "Организация", firm);
    PROPERTY_STRING(node, "Основание", reason);

    if ( priv->user )
        PROPERTY_OBJECT(node, "Ответственный", priv->user)
    else
        PROPERTY_EMPTY(node, "Ответственный", EXCHANGE_TYPE_HANDBOOK_USER);
    PROPERTY_BOOL(node, "ПометкаУдаления", FALSE);
    PROPERTY_BOOL(node, "Проведен", document->data.accepted);
    //PROPERTY_BOOL(node, "РучнаяКорректировка", FALSE);
    PROPERTY_BOOL(node, "РучнаяКорректировка", TRUE);
    PROPERTY_OBJECT(node, "Склад", unit);
    PROPERTY_NUMBER(node, "СуммаДокумента", sum);

    add_table(node, "ВозвратнаяТара");
    table = add_table(node, "Товары");

    int i;
    double sum_=0, sum_vat_=0;
    struct {
        double price;
        double price_vat;
    } s[VAT_NO];
    const vat_const_t *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO; i++ ) {
        s[i].price = do_document_sum(alias, document, vat_const[i].sum_r);
        s[i].price_vat = (s[i].price/(100.+vat_const[i].proc))*vat_const[i].proc;
        sum_+=s[i].price;
        sum_vat_+=s[i].price_vat;
    }
    for ( i = 0; i < VAT_NO; i++ ) {
        ExchangeProperty *prop;
        ExchangeKey *key;
        ExchangeObject *unit;
        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        prop = exchange_object_get_property(obj, "БазоваяЕдиницаИзмерения");
        if ( !prop || prop->empty ) {
            OBJ_MY_ERROR(obj, "У товара не задана базовая единица измерения");
            return FALSE;
        }
        key = g_value_get_pointer(&prop->value);
        unit = find_exchange_object_by_key(objs, key);
        if ( !unit ) {
            OBJ_MY_ERROR(obj, "Не найдена единица измерения \"%s\"", key->guid);
            return FALSE;
        }
        //s[i].price +=100; //!!
        if ( s[i].price != 0 ) {
            record = add_record(table);
            PROPERTY_OBJECT(record,  "ЕдиницаИзмерения", unit);

            PROPERTY_NUMBER(record, "Количество", 1);
            PROPERTY_EMPTY(record,  "КоличествоМест", EXCHANGE_TYPE_CONST_NUMBER);
            PROPERTY_NUMBER(record, "Коэффициент", 1);
            PROPERTY_OBJECT(record, "Номенклатура", obj);
            PROPERTY_EMPTY(record,  "НомерГТД", EXCHANGE_TYPE_HANDBOOK_DECLARATION);
            PROPERTY_EMPTY(record,  "ОтражениеВУСН", EXCHANGE_TYPE_LIST_SHOW_VUSN);
            PROPERTY_NUMBER(record, "Себестоимость", s[i].price);
            //PROPERTY_LIST(record,   "СтавкаНДС", EXCHANGE_TYPE_LIST_VAT_RATE, vat_const[i].name, objs);
            PROPERTY_EMPTY(record,  "СтавкаНДСВРознице", EXCHANGE_TYPE_LIST_VAT_RATE);
            PROPERTY_EMPTY(record,  "СтранаПроисхождения", EXCHANGE_TYPE_HANDBOOK_COUNTRY);
            PROPERTY_NUMBER(record, "Сумма", s[i].price);
            obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
            if ( !obj ) {
                MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
                return FALSE;
            }
            PROPERTY_OBJECT(record, "СчетУчета", obj);
            PROPERTY_NUMBER(record, "Цена", s[i].price);
        }
    }
    ExchangeObject *account_dt, *account_dt_sub_key, *account_dt_sub_value;
    ExchangeObject *account_kt, *account_kt_sub_key1,*account_kt_sub_key2;

    if ( !g_strcmp0(dtype1, "11") ) {
        double other_sums[VAT_NO], other_sum;


        get_other_mismatch_sums(priv->alias_2, document, other_sums, &other_sum);
        //    MY_ERROR("Невозможно найти документ оприходования излишек");
        //    return FALSE;
        //}

        for ( i = 0; i < VAT_NO; i++ ) {
            if ( other_sum > sum_ ) //other_sums[i] > s[i].price )
                s[i].price = 0;
            else
                s[i].price -= other_sums[i];
        }

        account_dt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ПрочиеРасходы");
        if ( !account_dt ) {
            MY_ERROR("Не найден счет ПрочиеРасходы в плане счетов");
            return FALSE;
        }
        account_dt_sub_key = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "ПрочиеДоходыИРасходы");
        if ( !account_dt_sub_key ) {
            MY_ERROR("Не найден вид субконто СтатьиЗатрат");
            return FALSE;
        }
        GSList *list;

        list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_OTHER_PROFIT_OUTLAY, "Недостача");
        if ( !list ) {
            MY_ERROR("Не найдена прочих доховом расходов \"Недостача\"");
            return FALSE;
        }
        if ( g_slist_length(list) > 1 ) {
            MY_ERROR("Для прочих доходов расходов \"Недостача\" более одной записи");
            return FALSE;
        }
        account_dt_sub_value = list->data;

    }
    else {

        account_dt = find_account_by_const_name(objs, "ИздержкиОбращения");
        if ( !account_dt )
            return FALSE;
        account_dt_sub_key = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "СтатьиЗатрат");
        if ( !account_dt_sub_key ) {
            MY_ERROR("Не найден вид субконто СтатьиЗатрат");
            return FALSE;
        }
        GSList *list;

        if ( !g_strcmp0(dtype1, "14") ) {
            list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_COST, "Срок годности,брак,бой");
            if ( !list ) {
                MY_ERROR("Не найдена статья затрат \"Срок годности,брак,бой\"");
                return FALSE;
            }
            if ( g_slist_length(list) > 1 ) {
                MY_ERROR("Для статьи затрат \"Срок годности,брак,бой\" более одной записи");
                return FALSE;
            }
            account_dt_sub_value = list->data;
        }
        if ( !g_strcmp0(dtype1, "15") ) {

            //list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_COST, "Материальные расходы");
            list = find_exchange_objects_by_code(objs, EXCHANGE_TYPE_HANDBOOK_COST, "000000048");
            if ( !list ) {
                MY_ERROR("Не найдена статья затрат \"000000048\"");
                return FALSE;
            }
            if ( g_slist_length(list) > 1 ) {
                MY_ERROR("Для статьи затрат \"000000048\" более одной записи");
                return FALSE;
            }
            account_dt_sub_value = list->data;
        }
    }

    account_kt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
    if ( !account_kt ) {
        MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
        return FALSE;
    }
    account_kt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Номенклатура");
    if ( !account_kt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто Номенклатура");
        return FALSE;
    }
    account_kt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Склады");
    if ( !account_kt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто Склады");
        return FALSE;
    }

    add_set(node, "ИПИныеМатериальныеРасходы");
    add_set(node, "ИПМПЗ");
    add_set(node, "ИПМПЗОбороты");
    add_set(node, "ИПРБП");
    add_set(node, "КнигаУчетаДоходовИРасходов");
    add_set(node, "НДСВключенныйВСтоимость");
    add_set(node, "НДСЗаписиКнигиПродаж");
    add_set(node, "НДСНачисленный");
    //add_set(node, "НДСпоОСиНМА");
    add_set(node, "НДСПоПриобретеннымЦенностям");
    add_set(node, "НДСПредъявленный");
    add_set(node, "РасходыПриУСН");
    add_set(node, "РасшифровкаКУДиР");
    add_set(node, "РеализованныеТоварыКомитентов");

    table = add_set(node, "Хозрасчетный");

    for ( i = 0; i < VAT_NO; i++ ) {
        double price;
        price = s[i].price;
        if ( !price )
            continue;

        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        record = add_record(table);
        PROPERTY_BOOL(record, "Активность", TRUE);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "КоличествоДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "КоличествоКт", 1);
        PROPERTY_BOOL(record, "НеКорректироватьСтоимостьАвтоматически", TRUE);
        PROPERTY_OBJECT(record, "Организация", firm);
        PROPERTY_DATE(record, "Период", &tm);
        //Регистратор
        node = xmlNewChild(record, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
        xmlNewProp(node, BAD_CAST ELEM_1C_NAME_S, BAD_CAST "Регистратор");
        xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST "ДокументСсылка.СписаниеТоваров");
        link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
        sprintf(buf, "%d", priv->npp);
        xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
        PROPERTY_STRING(link, ELEM_1C_NUMBER, str_number);
        PROPERTY_DATE(link , ELEM_1C_DATE, &tm);
        // !Регистратор
        PROPERTY_STRING(record, "Содержание", comment);

        PROPERTY_OBJECT(record, "СчетДт", account_dt);

        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key);
        PROPERTY_OBJECT(sub, "Значение", account_dt_sub_value);

        PROPERTY_OBJECT(record, "СчетКт", account_kt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key1);
        PROPERTY_OBJECT(sub, "Значение", obj);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key2);
        PROPERTY_OBJECT(sub, "Значение", unit);

        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_NUMBER(record, "СуммаНУДт", price);
        PROPERTY_NUMBER(record, "СуммаНУКт", price);
        PROPERTY_EMPTY(record, "СуммаВРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаВРКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРКт", EXCHANGE_TYPE_CONST_NUMBER);

    }
    g_free(str_number);
    g_free(dtype1);g_free(comment);g_free(reason);
    return TRUE;
}

struct _sale_doc_t {
    gint cash;
    gint cash_nn;
    gint shift;
    gboolean cash_exist;
    ExchangeObject *firm;
    ExchangeObject *unit;
    GSList *line;
    GSList *money;
    char *cashier;
    const gchar *doc_name;
    gchar *doc_number_1c_sale;
    gchar *doc_number_1c_back;
    struct tm tm;
    gint npp;
};
typedef struct {
    ExchangeObject *product;
    const gchar *vat_name;
    double sum11;
    double sum9;
    int proc;
} sale_doc_line_t;

typedef struct {
    gchar *name;
    ExchangeObject *kind;
    ExchangeObject *contract;
    ExchangeObject *partner;
    ExchangeObject *account;
    double sum9;
} sale_doc_money_t;

static gboolean get_document_product_kind_sale_(priv_t *priv, sale_doc_t *sale_doc)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    char buf[255];
    //gchar *str;
    ExchangeObject *obj;
    double sum;
    GSList *l, *list;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;
    sale_doc->npp = priv->npp;

    sum = 0;

    for ( l = sale_doc->line; l; l=l->next ) {
        sale_doc_line_t *line;
        line = l->data;
        sum = sum + line->sum9;
    }

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, sale_doc->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST sale_doc->doc_name);
    xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");


    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    PROPERTY_STRING(link, ELEM_1C_NUMBER, sale_doc->doc_number_1c_sale);
    PROPERTY_DATE(link , ELEM_1C_DATE, &sale_doc->tm);

    PROPERTY_OBJECT(node, "ВалютаДокумента", priv->valuta);
    PROPERTY_LIST(node, ELEM_1C_KIND_OPER, EXCHANGE_TYPE_LIST_OPERATION_PRODUCT_SALE, "ОтчетККМОПродажах", objs);
    PROPERTY_EMPTY_(node, "ИнвентаризацияТоваровНаСкладе", "ДокументСсылка.ИнвентаризацияТоваровНаСкладе");
    gchar *comment;
    /*!!if ( !sale_doc->cash_exist ) {
        MY_ERROR("Не найден документ наличной продажи");
        return FALSE;
    }*/
    comment = g_strdup_printf("Касса: №%d смена: №%d кассир: %s", sale_doc->cash_nn, sale_doc->shift, sale_doc->cashier);
    PROPERTY_STRING(node, "Комментарий", comment);
    g_free(comment);
    PROPERTY_NUMBER(node, "КратностьДокумента", 1);
    PROPERTY_NUMBER(node, "КурсДокумента", 1);
    PROPERTY_OBJECT(node, "Организация", sale_doc->firm);
    if ( priv->user )
        PROPERTY_OBJECT(node, "Ответственный", priv->user)
    else
        PROPERTY_EMPTY(node, "Ответственный", EXCHANGE_TYPE_HANDBOOK_USER);
    PROPERTY_BOOL(node, "ПометкаУдаления", FALSE);
    PROPERTY_BOOL(node, "Проведен", TRUE);
    PROPERTY_BOOL(node, "РучнаяКорректировка", FALSE);
    //PROPERTY_BOOL(node, "РучнаяКорректировка", TRUE);
    PROPERTY_OBJECT(node, "Склад", sale_doc->unit);

    list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_KIND_MOVE_MONEY, "Выручка с аптек");
    if ( !list ) {
        MY_ERROR("Не найдено статьи ДДС \"Выручка с аптек\"");
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для статьи ДДС \"Выручка с аптек\" более одной записи");
        return FALSE;
    }
    obj = list->data;
    PROPERTY_OBJECT(node, "СтатьяДвиженияДенежныхСредств", obj);
    PROPERTY_BOOL(node, "СуммаВключаетНДС", TRUE);
    PROPERTY_NUMBER(node, "СуммаДокумента", sum);
    obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "КассаОрганизации");
    if ( !obj ) {
        MY_ERROR("Не найден счет КассаОрганизации в плане счетов");
        return FALSE;
    }
    PROPERTY_OBJECT(node, "СчетКасса", obj);
    list = find_exchange_objects_by_code(objs, EXCHANGE_TYPE_HANDBOOK_PRICE_TYPE, CONST_1C_PRICE_PURCHASE);
    if ( !list ) {
        MY_ERROR("Не найден тип цен \"%s\"", CONST_1C_PRICE_PURCHASE);
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для типа цен с кодом \"%s\" более одной записи", CONST_1C_PRICE_PURCHASE);
        return FALSE;
    }
    obj = list->data;
    PROPERTY_OBJECT(node, "ТипЦен", obj);
    PROPERTY_BOOL(node, "УчитыватьНДС", FALSE);

    add_table(node, "АгентскиеУслуги");

    table = add_table(node, "Оплата");
    for ( l = sale_doc->money; l; l = l->next ) {
        sale_doc_money_t *money;
        money = l->data;
        record = add_record(table);
        PROPERTY_OBJECT(record, "ВидОплаты", money->kind);
        PROPERTY_OBJECT(record, "ДоговорКонтрагента", money->contract);
        PROPERTY_OBJECT(record, "Контрагент", money->partner);
        PROPERTY_NUMBER(record, "СуммаОплаты", money->sum9);
        PROPERTY_OBJECT(record, "СчетУчетаРасчетов", money->account);
    }
    table = add_table(node, "Товары");
    for ( l = sale_doc->line; l; l = l->next ) {
        sale_doc_line_t *line;
        line = l->data;
	if ( line->sum9 > -0.001 && line->sum9 < 0.001 ) continue;
        record = add_record(table);
  	    PROPERTY_NUMBER(record, "Количество", 1)
        PROPERTY_OBJECT(record, "Номенклатура", line->product);
        PROPERTY_EMPTY(record,  "НомерГТД", EXCHANGE_TYPE_HANDBOOK_DECLARATION);
        if ( line->sum11 > 0 )
            PROPERTY_NUMBER(record, "Себестоимость", line->sum11)
        else
            PROPERTY_NUMBER(record, "Себестоимость", 1);

        PROPERTY_LIST(record,   "СтавкаНДС", EXCHANGE_TYPE_LIST_VAT_RATE, line->vat_name, objs);
        PROPERTY_EMPTY(record,  "СтранаПроисхождения", EXCHANGE_TYPE_HANDBOOK_COUNTRY);
        list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_PRODUCT_GROUP, "Розница");
        if ( !list ) {
            MY_ERROR("Не найдена номенклатурная группа \"Розница\"");
            return FALSE;
        }
        if ( g_slist_length(list) > 1 ) {
            MY_ERROR("Для номерклатурной группы \"Розница\" более одной записи");
            return FALSE;
        }
        obj = list->data;
        PROPERTY_OBJECT(record, "Субконто", obj);
        PROPERTY_NUMBER(record, "Сумма", line->sum9);
        PROPERTY_NUMBER(record, "СуммаНДС", (line->sum9/(100.+line->proc))*line->proc);

        obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ВыручкаЕНВД");
        if ( !obj ) {
            MY_ERROR("Не найден счет ВыручкаЕНВД в плане счетов");
            return FALSE;
        }
        PROPERTY_OBJECT(record, "СчетДоходов", obj);
        obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "СебестоимостьПродажЕНВД");
        if ( !obj ) {
            MY_ERROR("Не найден счет СебестоимостьПродажЕНВД в плане счетов");
            return FALSE;
        }
        PROPERTY_OBJECT(record, "СчетРасходов", obj);
        obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
        if ( !obj ) {
            MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
            return FALSE;
        }
        PROPERTY_OBJECT(record, "СчетУчета", obj);
        obj = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "Продажи_НДС");
        if ( !obj ) {
            MY_ERROR("Не найден счет Продажи_НДС в плане счетов");
            return FALSE;
        }
        PROPERTY_OBJECT(record, "СчетУчетаНДСПоРеализации", obj);
        PROPERTY_NUMBER(record, "Цена", line->sum9);
    }

    return TRUE;
}
GSList *get_params(do_alias_t *alias, document_rec_t *rec, const char *pref)
{
    char *params =
        do_alias_text_(alias, rec->data.params, rec->size - (sizeof(rec->data) - sizeof(rec->data.params)));

    GSList *res = NULL;
    char *value, *cp = (char*)params, *head;

    while (*cp != '\0') {
        head = cp;
        while (*cp != '\0' && *cp != '\n' && *cp !='\r' && *cp != '=') cp++;
        if (*cp != '=' ) {
            while (*cp == '\n' || *cp =='\r') cp++;
            continue;
        }
        if ( (strlen(pref) <= cp - head) &&
             !strncmp(pref, head, strlen(pref))
             ) {
                value = g_malloc0(cp - head + 1 - strlen(pref));
                strncpy(value, head + strlen(pref), cp - head - strlen(pref));
                res = g_slist_append(res, value);
        }
        while (*cp != '\0' && *cp != '\n' && *cp !='\r') cp++;
        while (*cp == '\n' || *cp =='\r') cp++;
    }
    g_free(params);
    return res;
}

static gboolean get_document_product_kind_sale(priv_t *priv)
{
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    GSList *objs = priv->objs;
    GSList *l;
    gint cash, cash_nn;
    gint shift;
    sale_doc_t *sale_doc;
    gint i,j;

    gchar *str, *ch;
    str = do_text(alias, document->data.p_doc);
    cash = document->data.p_code;
    ch = strchr(str, '/');
    cash_nn = atoi(str);
    //if ( cash != atoi(str) || !ch ) {
    if ( !cash_nn || !ch ) {
        MY_ERROR("Неправильный номер ККЛ в документе \"%s\"", do_util_document_to_str(alias, document));
        return FALSE;
    }
    ch++;
    shift = atoi(ch);
    if ( !shift ) {
        MY_ERROR("Неправильный номер ККЛ в документе \"%s\"", do_util_document_to_str(alias, document));
        return FALSE;
    }
    do_free(str);
    sale_doc = NULL;
    for ( l=priv->sale_doc; l; l=l->next ) {
        sale_doc_t *s;
        s = l->data;
        if ( (s->cash == cash) && (s->shift == shift) ) {
            sale_doc = s;
            break;
        }
    }
    if ( !sale_doc ) {
        sale_doc = g_new0(sale_doc_t, 1);
        sale_doc->cash = cash;
        sale_doc->cash_nn = cash_nn;
        sale_doc->shift = shift;
        sale_doc->firm = get_object_firm_from_document(alias, document, objs);
        if ( !sale_doc->firm )
            return FALSE;

        sale_doc->unit = get_object_unit_from_document(alias, document, objs);
        if ( !sale_doc->unit )
            return FALSE;

        sale_doc->doc_name = priv->doc_name;
        sale_doc->cashier = do_document_param(alias, document, "606");

        sale_doc->doc_number_1c_sale = document_number_to_1c(alias, document, sale_doc->firm);
        sale_doc->doc_number_1c_back = document_number_to_1c_(alias, document, sale_doc->firm);
        do_date(document->data.date, &sale_doc->tm);
        do_time(document->data.time, &sale_doc->tm);
        priv->sale_doc = g_slist_append(priv->sale_doc, sale_doc);
    }
    else {
        gchar *str;
        str = document_number_to_1c(alias, document, sale_doc->firm);
        if ( strcmp(str, sale_doc->doc_number_1c_sale) < 0 ) {
            g_free(sale_doc->doc_number_1c_sale);
            g_free(sale_doc->doc_number_1c_back);
            sale_doc->doc_number_1c_sale = str;
            sale_doc->doc_number_1c_back = document_number_to_1c_(alias, document, sale_doc->firm);
        }
        else
            g_free(str);
    }
    if ( document->data.dtype1[1] == '1' )
        sale_doc->cash_exist = TRUE;
    struct {
        double price;
        double price_vat;
        double price_11;
    } s[VAT_NO];
    double sum_check, sum_, sum_11, sum_check11;
    gint max = -1;
    sum_check = do_document_param_float(alias, document, "214");
    sum_check11 = do_document_sum(alias, document, 11);
    sum_=0;
    const vat_const_t *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO; i++ ) {
        s[i].price = do_document_sum(alias, document, vat_const[i].sum_f);
        sum_+=s[i].price;
        s[i].price_11 = do_document_sum(alias, document, vat_const[i].sum_r);
        sum_11+=s[i].price_11;
        if ( max == -1 || s[max].price < s[i].price )
            max = i;
    }
    if ( !MONEY_IQUAL(sum_, sum_check, 0.001) )
        s[max].price = s[max].price - (sum_ - sum_check);
    if ( !MONEY_IQUAL(sum_11, sum_check11, 0.001) )
        s[VAT_20].price_11 = s[VAT_20].price_11 - (sum_11 - sum_check11);

    for ( i = 0; i < VAT_NO; i++ ) {
        double price9, price11;
        price9 = s[i].price;
        price11 = s[i].price_11;
        if ( !price9 && price11 ) {
	    for ( j = 0; j < VAT_NO; j++ ) {
                if ( i != j ) {
                   if ( s[j].price ) {
                       s[j].price_11+=price11;
                       price11=0;
                       break;
                   }
                }
           }
           if ( price11 ) {
                MY_ERROR("Не распределилась сумма в документе по ставкам НДС \"%s\"", do_util_document_to_str(alias, document));
                return FALSE;
           }
        }
    }

    for ( i = 0; i < VAT_NO; i++ ) {
        double price9, price11;
        sale_doc_line_t *line;
        price9 = s[i].price; // do_document_sum(alias, document, vat_const[i].sum_f);
        price11 = s[i].price_11; // do_document_sum(alias, document, vat_const[i].sum_r);
        if ( !price9 )
            continue;
        line = NULL;
        for ( l=sale_doc->line; l; l=l->next ) {
            sale_doc_line_t *s;
            s = l->data;
            if ( s->proc == vat_const[i].proc ) {
                line = s;
                break;
            }
        }
        if ( !line ) {
            line = g_new0(sale_doc_line_t, 1);
            line->proc = vat_const[i].proc;
            line->product = get_1c_product_by_vat(objs, vat_const[i]);
            if ( !line->product )
                return FALSE;
            line->vat_name = vat_const[i].name;
            sale_doc->line = g_slist_append(sale_doc->line, line);
        }
        line->sum9+=price9;
        line->sum11+=price11;
    }
    char *dtype1;
    dtype1 = do_text(alias, document->data.dtype1);
    if ( !strcmp(dtype1, "03") ||
         !strcmp(dtype1, "04") ) {

        double sum, sum9;
        GSList *list, *m;
        sale_doc_money_t *money;
        list = get_params(alias, document, "VAL_");
        sum = 0;
#ifdef DEPRICATE_MONEY_INFO
        if ( !list ) { //!!
            GSList *olist;
            const gchar *name = "Неопределенный";
            money = g_new0(sale_doc_money_t, 1);
            money->name = g_strdup(name);
            ExchangeProperty *prop;
            ExchangeKey *key;

            olist = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_KIND_MONEY_PAY, name);
            if ( !olist ) {
                MY_ERROR("Не найден способ оплаты \"%s\"", name);
                return FALSE;
            }
            if ( g_slist_length(olist) > 1 ) {
                MY_ERROR("Для способа оплаты \"%s\" более одной записи", name);
                return FALSE;
            }
            money->kind = olist->data;
            prop = exchange_object_get_property(money->kind, "ДоговорКонтрагента");
            if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_HANDBOOK_CONTRACT ) {
                OBJ_MY_ERROR(money->kind, "У способа оплаты \"%s\" не задан договор", name);
                return FALSE;
            }
            key = g_value_get_pointer(&prop->value);
            money->contract = find_exchange_object_by_key(objs, key);
            if ( !money->contract ) {
                OBJ_MY_ERROR(money->kind, "Не найден договор \"%s\"", key->guid);
                return FALSE;
            }
            prop = exchange_object_get_property(money->kind, "Контрагент");
            if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_HANDBOOK_PARTNER ) {
                OBJ_MY_ERROR(money->kind, "У вида оплаты \"%s\" не контрагент", name);
                return FALSE;
            }
            key = g_value_get_pointer(&prop->value);
            money->partner = find_exchange_object_by_key(objs, key);
            if ( !money->partner ) {
                OBJ_MY_ERROR(money->kind, "Не найден партнер \"%s\"", key->guid);
                return FALSE;
            }
            prop = exchange_object_get_property(money->kind, "СчетУчетаРасчетов");
            if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_ACCOUNT_DEFAULT ) {
                OBJ_MY_ERROR(money->kind, "У вида оплаты \"%s\" не задан счет", name);
                return FALSE;
            }
            key = g_value_get_pointer(&prop->value);
            money->account = find_exchange_object_by_key(objs, key);
            if ( !money->account ) {
                OBJ_MY_ERROR(money->kind, "Не найден счет \"%s\"", key->guid);
                return FALSE;
            }
            sale_doc->money = g_slist_append(sale_doc->money, money);
            money->sum9 = do_document_param_float(alias, document, "214");
            //do_document_sum(alias, document, 9);
            sum = money->sum9;
        }
#endif
        for ( l = list; l; l=l->next ) {
            gchar *name, *param;
            gdouble price9;
            name = l->data;
            param = g_strdup_printf("VAL_%s", name);
            price9 = do_document_param_float(alias, document, param);
            sum = sum + price9;
            g_free(param);

            money = NULL;
            for ( m = sale_doc->money; m; m = m->next ) {
                sale_doc_money_t *s;
                s = m->data;
                if ( !strcmp(s->name, name) ) {
                    money = s;
                    break;
                }
            }
            if ( !money ) {
                GSList *olist;
                money = g_new0(sale_doc_money_t, 1);
                money->name = g_strdup(name);
                ExchangeProperty *prop;
                ExchangeKey *key;

                olist = find_exchange_objects_by_name_i(objs, EXCHANGE_TYPE_HANDBOOK_KIND_MONEY_PAY, name);
                if ( !olist ) {
                    if ( name[0] == '\0' ) {
                        olist = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_KIND_MONEY_PAY, "Неизвестно");
                    }
                    if ( !olist ) {
                        MY_ERROR("Не найден вид оплат организации \"%s\" и \"Неизвестно\" %s.%s.%s", name, do_text(alias, document->data.dtype),do_text(alias, document->data.sklad),do_text(alias, document->data.document));
                        return FALSE;
                    }
                }
                if ( g_slist_length(olist) > 1 ) {
                    MY_ERROR("Для способа оплаты \"%s\" более одной записи", name);
                    return FALSE;
                }
                money->kind = olist->data;
                prop = exchange_object_get_property(money->kind, "ДоговорКонтрагента");
                if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_HANDBOOK_CONTRACT ) {
                    OBJ_MY_ERROR(money->kind, "У способа оплаты \"%s\" не задан договор", name);
                    return FALSE;
                }
                key = g_value_get_pointer(&prop->value);
                money->contract = find_exchange_object_by_key(objs, key);
                if ( !money->contract ) {
                    OBJ_MY_ERROR(money->kind, "Не найден договор \"%s\"", key->guid);
                    return FALSE;
                }
                prop = exchange_object_get_property(money->kind, "Контрагент");
                if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_HANDBOOK_PARTNER ) {
                    OBJ_MY_ERROR(money->kind, "У способа оплаты \"%s\" не контрагент", name);
                    return FALSE;
                }
                key = g_value_get_pointer(&prop->value);
                money->partner = find_exchange_object_by_key(objs, key);
                if ( !money->partner ) {
                    OBJ_MY_ERROR(money->kind, "Не найден партнер \"%s\"", key->guid);
                    return FALSE;
                }
                prop = exchange_object_get_property(money->kind, "СчетУчетаРасчетов");
                if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_ACCOUNT_DEFAULT ) {
                    OBJ_MY_ERROR(money->kind, "У способа оплаты \"%s\" не задан счет", name);
                    return FALSE;
                }
                key = g_value_get_pointer(&prop->value);
                money->account = find_exchange_object_by_key(objs, key);
                if ( !money->account ) {
                    OBJ_MY_ERROR(money->kind, "Не найден счет \"%s\"", key->guid);
                    return FALSE;
                }
                sale_doc->money = g_slist_append(sale_doc->money, money);
            }
            money->sum9 = money->sum9 + price9;
        }
        sum9 = do_document_param_float(alias, document, "214");//do_document_sum(alias, document, 9);
        if ( (sum < sum9 - 0.005) || (sum > sum9 + 0.005) ) {

            GSList *olist;
            const gchar *name = "Неопределенный";
            money = g_new0(sale_doc_money_t, 1);
            money->name = g_strdup(name);
            ExchangeProperty *prop;
            ExchangeKey *key;

            olist = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_KIND_MONEY_PAY, name);
            if ( !olist ) {
                MY_ERROR("Не найден способ оплаты \"%s\"", name);
                return FALSE;
            }
            if ( g_slist_length(olist) > 1 ) {
                MY_ERROR("Для способа оплаты \"%s\" более одной записи", name);
                return FALSE;
            }
            money->kind = olist->data;
            prop = exchange_object_get_property(money->kind, "ДоговорКонтрагента");
            if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_HANDBOOK_CONTRACT ) {
                OBJ_MY_ERROR(money->kind, "У способа оплаты \"%s\" не задан договор", name);
                return FALSE;
            }
            key = g_value_get_pointer(&prop->value);
            money->contract = find_exchange_object_by_key(objs, key);
            if ( !money->contract ) {
                OBJ_MY_ERROR(money->kind, "Не найден договор \"%s\"", key->guid);
                return FALSE;
            }
            prop = exchange_object_get_property(money->kind, "Контрагент");
            if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_HANDBOOK_PARTNER ) {
                OBJ_MY_ERROR(money->kind, "У вида оплаты \"%s\" не контрагент", name);
                return FALSE;
            }
            key = g_value_get_pointer(&prop->value);
            money->partner = find_exchange_object_by_key(objs, key);
            if ( !money->partner ) {
                OBJ_MY_ERROR(money->kind, "Не найден партнер \"%s\"", key->guid);
                return FALSE;
            }
            prop = exchange_object_get_property(money->kind, "СчетУчетаРасчетов");
            if ( !prop || prop->empty || prop->type != EXCHANGE_TYPE_ACCOUNT_DEFAULT ) {
                OBJ_MY_ERROR(money->kind, "У вида оплаты \"%s\" не задан счет", name);
                return FALSE;
            }
            key = g_value_get_pointer(&prop->value);
            money->account = find_exchange_object_by_key(objs, key);
            if ( !money->account ) {
                OBJ_MY_ERROR(money->kind, "Не найден счет \"%s\"", key->guid);
                return FALSE;
            }
            sale_doc->money = g_slist_append(sale_doc->money, money);
	    money->sum9 = sum9 - sum;

            /*MY_ERROR("Сумма по документу \"%s\" расходиться с суммой по видам оплат. %f %f",
                   do_util_document_to_str(alias, document), sum9, sum);
            return FALSE;*/
        }

    }
    return TRUE;
}
static gboolean get_document_backsale_cost_(priv_t *priv, sale_doc_t *sale_doc)
{
    GSList *l;
    gdouble doc_sum=0;

    for ( l = sale_doc->line; l; l=l->next ) {
        sale_doc_line_t *line;
        line = l->data;
        if ( line->sum11 <= 0 )
            doc_sum += line->sum11 - 1;
    }
    if ( doc_sum >=0  )
        return TRUE;

    priv->npp++;
    xmlNode *link;
    //xmlNode *link_owner;
    //xmlNode *owner;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    xmlNode *sub;
    char buf[255];

    ExchangeObject *account_dt, *account_dt_sub_key, *account_dt_sub_value;
    ExchangeObject *account_kt, *account_kt_sub_key1,*account_kt_sub_key2;
    GSList *list;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, document_product_kind[DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_RE_SORT].name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST document_product_kind[DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_RE_SORT].name);

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    PROPERTY_STRING(link, ELEM_1C_NUMBER, sale_doc->doc_number_1c_back);
    PROPERTY_DATE(link , ELEM_1C_DATE, &sale_doc->tm);
/*!!
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, document_product_kind[DO_EXCHANGE_DOCUMENT_PRODUCT_KIND_SALE].name);

    owner = xmlNewChild(link, NULL, BAD_CAST ELEM_1C_PROPERTY, BAD_CAST "");
    xmlNewProp(owner, BAD_CAST ELEM_1C_NAME_S, BAD_CAST "ДокументОснование");
    xmlNewProp(owner, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);

    link_owner = xmlNewChild(owner, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", sale_doc->npp);
    xmlNewProp(link_owner, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
*/
    PROPERTY_STRING(node, ELEM_1C_COMMENT, "Перенесено из Домино");
    PROPERTY_OBJECT(node, ELEM_1C_FIRM, sale_doc->firm);
    PROPERTY_STRING(node ,"Содержание", "Списание себестоимости по возврату ");
    PROPERTY_STRING(node ,"СпособЗаполнения", "Вручную");
    PROPERTY_NUMBER(node ,"СуммаОперации", doc_sum);

    account_dt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "СебестоимостьПродажЕНВД");
    if ( !account_dt ) {
        MY_ERROR("Не найден счет СебестоимостьПродажЕНВД в плане счетов");
        return FALSE;
    }
    account_dt_sub_key = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "НоменклатурныеГруппы");
    if ( !account_dt_sub_key ) {
        MY_ERROR("Не найден вид субконто НоменклатурныеГруппы");
        return FALSE;
    }
    list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_PRODUCT_GROUP, "Розница");
    if ( !list ) {
        MY_ERROR("Не найдена номенклатурная группа \"Розница\"");
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для номерклатурной группы \"Розница\" более одной записи");
        return FALSE;
    }
    account_dt_sub_value = list->data;

    account_kt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
    if ( !account_kt ) {
        MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
        return FALSE;
    }
    account_kt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Номенклатура");
    if ( !account_kt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто Номенклатура");
        return FALSE;
    }
    account_kt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Склады");
    if ( !account_kt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто Склады");
        return FALSE;
    }

    table = add_set(node, "Хозрасчетный");
    for ( l = sale_doc->line; l; l = l->next ) {
        sale_doc_line_t *line;
        double price;
        line = l->data;
        if ( line->sum11 > 0 )
            continue;

        record = add_record(table);
        price = line->sum11 - 1;

        PROPERTY_DATE(record, "Период", &sale_doc->tm);
        PROPERTY_OBJECT(record, "СчетДт", account_dt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key);
        PROPERTY_OBJECT(sub, "Значение", account_dt_sub_value);

        PROPERTY_OBJECT(record, "СчетКт", account_kt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key1);
        PROPERTY_OBJECT(sub, "Значение", line->product);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key2);
        PROPERTY_OBJECT(sub, "Значение", sale_doc->unit);

        PROPERTY_OBJECT(record, "Организация", sale_doc->firm);
        PROPERTY_EMPTY(record, "ВалютаДт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_EMPTY(record, "ВалютаКт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаКт", EXCHANGE_TYPE_CONST_NUMBER);
        //PROPERTY_NUMBER(record, "КоличествоДт", 1);
        PROPERTY_NUMBER(record, "КоличествоКт", 1);

        PROPERTY_EMPTY(record, "Содержание", EXCHANGE_TYPE_CONST_STRING);
        PROPERTY_EMPTY(record, "СуммаНУДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРДт", price);
        PROPERTY_EMPTY(record, "СуммаНУКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРКт", price);
        PROPERTY_BOOL(record, "Активность", TRUE);
    }
    return TRUE;
}
#ifdef ALFA_
static gboolean get_document_product_kind_other_outlay(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    xmlNode *sub;
    char buf[255];
    gchar *str;
    struct tm tm;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit;
    ExchangeObject *account_dt, *account_dt_sub_key, *account_dt_sub_value;
    ExchangeObject *account_kt, *account_kt_sub_key1,*account_kt_sub_key2;
    GSList *list;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;
    unit = get_object_unit_from_document(alias, document, objs);
    if ( !unit )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    //!!xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    //!!xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);
    str = document_number_to_1c(alias, document, firm);
    if ( !str )
        return FALSE;

    PROPERTY_STRING(link, ELEM_1C_NUMBER, str);
    PROPERTY_DATE(link, ELEM_1C_DATE, &tm);
    g_free(str);

    PROPERTY_STRING(node, ELEM_1C_COMMENT, "Перенесено из Домино");
    PROPERTY_OBJECT(node, ELEM_1C_FIRM, firm);
    PROPERTY_STRING(node ,"Содержание", "Продажа Шатохиной");
    PROPERTY_STRING(node ,"СпособЗаполнения", "Вручную");
    PROPERTY_NUMBER(node ,"СуммаОперации", do_document_sum(alias, document, 9));

    account_dt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "СебестоимостьПродажЕНВД");
    if ( !account_dt ) {
        MY_ERROR("Не найден счет СебестоимостьПродажЕНВД в плане счетов");
        return FALSE;
    }
    /*account_dt_sub_key = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "ПрочиеДоходыИРасходы");
    if ( !account_dt_sub_key ) {
        MY_ERROR("Не найден вид субконто \"ПрочиеДоходыИРасходы\"");
        return FALSE;
    }
    list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_OTHER_PROFIT_OUTLAY, "Благотворительность");
    if ( !list ) {
        MY_ERROR("Не найден вид прочих расходов доходов \"Благотворительность\"");
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для вида прочих доходов расходов \"Благотворительность\" более одной записи");
        return FALSE;
    }
    account_dt_sub_value = list->data;*/

    account_kt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
    if ( !account_kt ) {
        MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
        return FALSE;
    }
    account_kt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Номенклатура");
    if ( !account_kt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто Номенклатура");
        return FALSE;
    }
    account_kt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Склады");
    if ( !account_kt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто Склады");
        return FALSE;
    }

    table = add_set(node, "Хозрасчетный");
    int i;
    for ( i = 0; i < VAT_NO; i++ ) {
        double price;
        price = do_document_sum(alias, document, vat_const[i].sum_r);//!!
        if ( !price )
            continue;

        if ( vat_const[i].obj )
            obj = vat_const[i].obj;
        else {
	    if ( document->data.date >= DOMINO_DATE_20190101 && i == VAT_20 )
               obj = find_exchange_object_by_value_string(objs, EXCHANGE_TYPE_HANDBOOK_PRODUCT, "СтавкаНДС", vat_const[VAT_NO].name);
	    else
               obj = find_exchange_object_by_value_string(objs, EXCHANGE_TYPE_HANDBOOK_PRODUCT, "СтавкаНДС", vat_const[i].name);
            vat_const[i].obj = obj;
        }
        if ( !obj ) {
            MY_ERROR("Товар со ставкой НДС \"%s\" не найден", vat_const[i].name);
            return FALSE;
        }

        record = add_record(table);
        PROPERTY_DATE(record, "Период", &tm);
        PROPERTY_OBJECT(record, "СчетДт", account_dt);
        /*sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key);
        PROPERTY_OBJECT(sub, "Значение", account_dt_sub_value);*/

        PROPERTY_OBJECT(record, "СчетКт", account_kt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key1);
        PROPERTY_OBJECT(sub, "Значение", obj);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key2);
        PROPERTY_OBJECT(sub, "Значение", unit);

        PROPERTY_OBJECT(record, "Организация", firm);
        PROPERTY_EMPTY(record, "ВалютаДт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_EMPTY(record, "ВалютаКт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаКт", EXCHANGE_TYPE_CONST_NUMBER);
        //PROPERTY_NUMBER(record, "КоличествоДт", 1);
        PROPERTY_NUMBER(record, "КоличествоКт", 1);

        PROPERTY_EMPTY(record, "Содержание", EXCHANGE_TYPE_CONST_STRING);
        PROPERTY_EMPTY(record, "СуммаНУДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРДт", price);
        PROPERTY_EMPTY(record, "СуммаНУКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРКт", price);
        PROPERTY_BOOL(record, "Активность", TRUE);
    }
    account_dt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "РасчетыСПокупателями");
    if ( !account_dt ) {
        MY_ERROR("Не найден счет РасчетыСПокупателями в плане счетов");
        return FALSE;
    }
    /*account_dt_sub_key = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "ПрочиеДоходыИРасходы");
    if ( !account_dt_sub_key ) {
        MY_ERROR("Не найден вид субконто \"ПрочиеДоходыИРасходы\"");
        return FALSE;
    }
    list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_OTHER_PROFIT_OUTLAY, "Благотворительность");
    if ( !list ) {
        MY_ERROR("Не найден вид прочих расходов доходов \"Благотворительность\"");
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для вида прочих доходов расходов \"Благотворительность\" более одной записи");
        return FALSE;
    }
    account_dt_sub_value = list->data;*/

    account_kt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ВыручкаЕНВД");
    if ( !account_kt ) {
        MY_ERROR("Не найден счет ВыручкаЕНВД в плане счетов");
        return FALSE;
    }
    /*account_kt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Номенклатура");
    if ( !account_kt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто Номенклатура");
        return FALSE;
    }
    account_kt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Склады");
    if ( !account_kt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто Склады");
        return FALSE;
    }
    */

    //table = add_set(node, "Хозрасчетный");
    //int i;
    for ( i = 0; i < VAT_NO; i++ ) {
        double price;
        price = do_document_sum(alias, document, vat_const[i].sum_f);//!!
        if ( !price )
            continue;

        if ( vat_const[i].obj )
            obj = vat_const[i].obj;
        else {
            obj = find_exchange_object_by_value_string(objs, EXCHANGE_TYPE_HANDBOOK_PRODUCT, "СтавкаНДС", vat_const[i].name);
            vat_const[i].obj = obj;
        }
        if ( !obj ) {
            MY_ERROR("Товар со ставкой НДС \"%s\" не найден", vat_const[i].name);
            return FALSE;
        }

        record = add_record(table);
        PROPERTY_DATE(record, "Период", &tm);
        PROPERTY_OBJECT(record, "СчетДт", account_dt);
        /*sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key);
        PROPERTY_OBJECT(sub, "Значение", account_dt_sub_value);*/

        PROPERTY_OBJECT(record, "СчетКт", account_kt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        //PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key1);
        //PROPERTY_OBJECT(sub, "Значение", obj);
        //sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        //PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key2);
        //PROPERTY_OBJECT(sub, "Значение", unit);

        PROPERTY_OBJECT(record, "Организация", firm);
        PROPERTY_EMPTY(record, "ВалютаДт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_EMPTY(record, "ВалютаКт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаКт", EXCHANGE_TYPE_CONST_NUMBER);
        //PROPERTY_NUMBER(record, "КоличествоДт", 1);
        PROPERTY_NUMBER(record, "КоличествоКт", 1);

        PROPERTY_EMPTY(record, "Содержание", EXCHANGE_TYPE_CONST_STRING);
        PROPERTY_EMPTY(record, "СуммаНУДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРДт", price);
        PROPERTY_EMPTY(record, "СуммаНУКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРКт", price);
        PROPERTY_BOOL(record, "Активность", TRUE);
    }
    return TRUE;
}
#else

static gboolean get_document_product_kind_other_outlay(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    xmlNode *sub;
    char buf[255];
    gchar *str;
    struct tm tm;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit;
    ExchangeObject *account_dt, *account_dt_sub_key, *account_dt_sub_value;
    ExchangeObject *account_kt, *account_kt_sub_key1,*account_kt_sub_key2;
    GSList *list;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;
    unit = get_object_unit_from_document(alias, document, objs);
    if ( !unit )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    //!!xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    //!!xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);
    str = document_number_to_1c(alias, document, firm);
    if ( !str )
        return FALSE;

    PROPERTY_STRING(link, ELEM_1C_NUMBER, str);
    PROPERTY_DATE(link, ELEM_1C_DATE, &tm);
    g_free(str);

    PROPERTY_STRING(node, ELEM_1C_COMMENT, "Перенесено из Домино");
    PROPERTY_OBJECT(node, ELEM_1C_FIRM, firm);
    PROPERTY_STRING(node ,"Содержание", "Благотворительность");
    PROPERTY_STRING(node ,"СпособЗаполнения", "Вручную");
    PROPERTY_NUMBER(node ,"СуммаОперации", do_document_sum(alias, document, 11));

    account_dt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ПрочиеРасходы");
    if ( !account_dt ) {
        MY_ERROR("Не найден счет ПрочиеРасходы в плане счетов");
        return FALSE;
    }
    account_dt_sub_key = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "ПрочиеДоходыИРасходы");
    if ( !account_dt_sub_key ) {
        MY_ERROR("Не найден вид субконто \"ПрочиеДоходыИРасходы\"");
        return FALSE;
    }
    list = find_exchange_objects_by_name(objs, EXCHANGE_TYPE_HANDBOOK_OTHER_PROFIT_OUTLAY, "Благотворительность");
    if ( !list ) {
        MY_ERROR("Не найден вид прочих расходов доходов \"Благотворительность\"");
        return FALSE;
    }
    if ( g_slist_length(list) > 1 ) {
        MY_ERROR("Для вида прочих доходов расходов \"Благотворительность\" более одной записи");
        return FALSE;
    }
    account_dt_sub_value = list->data;

    account_kt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ТоварыВРозничнойТорговле");
    if ( !account_kt ) {
        MY_ERROR("Не найден счет ТоварыВРозничнойТорговле в плане счетов");
        return FALSE;
    }
    account_kt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Номенклатура");
    if ( !account_kt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто Номенклатура");
        return FALSE;
    }
    account_kt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Склады");
    if ( !account_kt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто Склады");
        return FALSE;
    }

    table = add_set(node, "Хозрасчетный");
    int i;
    const vat_const_t *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO; i++ ) {
        double price;
        price = do_document_sum(alias, document, vat_const[i].sum_r);//!!
        if ( !price )
            continue;

        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        record = add_record(table);
        PROPERTY_DATE(record, "Период", &tm);
        PROPERTY_OBJECT(record, "СчетДт", account_dt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key);
        PROPERTY_OBJECT(sub, "Значение", account_dt_sub_value);

        PROPERTY_OBJECT(record, "СчетКт", account_kt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key1);
        PROPERTY_OBJECT(sub, "Значение", obj);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key2);
        PROPERTY_OBJECT(sub, "Значение", unit);

        PROPERTY_OBJECT(record, "Организация", firm);
        PROPERTY_EMPTY(record, "ВалютаДт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_EMPTY(record, "ВалютаКт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаКт", EXCHANGE_TYPE_CONST_NUMBER);
        //PROPERTY_NUMBER(record, "КоличествоДт", 1);
        PROPERTY_NUMBER(record, "КоличествоКт", 1);

        PROPERTY_EMPTY(record, "Содержание", EXCHANGE_TYPE_CONST_STRING);
        PROPERTY_EMPTY(record, "СуммаНУДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРДт", price);
        PROPERTY_EMPTY(record, "СуммаНУКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРКт", price);
        PROPERTY_BOOL(record, "Активность", TRUE);
    }
    return TRUE;
}
#endif // ALFA_
static gboolean get_document_product_kind_back_confirm(priv_t *priv)
{
    priv->npp++;
    xmlNode *link;
    xmlNode *node;
    xmlNode *table;
    xmlNode *record;
    xmlNode *sub;
    char buf[255];
    gchar *str, *value, *date_str;
    struct tm tm;
    struct tm tm1;
    ExchangeObject *obj;
    ExchangeObject *firm;
    ExchangeObject *unit;
    ExchangeObject *account_dt, *account_dt_sub_key1, *account_dt_sub_key2;
    ExchangeObject *account_kt, *account_kt_sub_key1,*account_kt_sub_key2;
    do_alias_t *alias = priv->alias;
    document_rec_t *document = priv->document;
    xmlNode *root = priv->root;
    GSList *objs = priv->objs;

    firm = get_object_firm_from_document(alias, document, objs);
    if ( !firm )
        return FALSE;
    unit = get_object_unit_from_document(alias, document, objs);
    if ( !unit )
        return FALSE;

    node = xmlNewChild(root, NULL, BAD_CAST ELEM_1C_OBJECT, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(node, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);
    sprintf(buf, "%s.%s", CONST_1C_DOCUMENT_LINK, priv->doc_name);
    xmlNewProp(node, BAD_CAST ELEM_1C_TYPE, BAD_CAST buf);
    xmlNewProp(node, BAD_CAST ELEM_1C_RULE_NAME, BAD_CAST priv->doc_name);
    //!!xmlNewProp(node, BAD_CAST "РежимЗаписи", BAD_CAST "Проведение");
    //!!xmlNewProp(node, BAD_CAST "РежимПроведение", BAD_CAST "Оперативный");

    link = xmlNewChild(node, NULL, BAD_CAST ELEM_1C_LINK, BAD_CAST "");
    sprintf(buf, "%d", priv->npp);
    xmlNewProp(link, BAD_CAST ELEM_1C_NPP, BAD_CAST buf);

    do_date(document->data.date, &tm);//!!
    do_time(document->data.time, &tm);
    str = document_number_to_1c(alias, document, firm);
    if ( !str )
        return FALSE;

    PROPERTY_STRING(link, ELEM_1C_NUMBER, str);
    PROPERTY_DATE(link, ELEM_1C_DATE, &tm);
    g_free(str);

    PROPERTY_STRING(node, ELEM_1C_COMMENT, "Перенесено из Домино");
    PROPERTY_OBJECT(node, ELEM_1C_FIRM, firm);
    str = do_text(alias, document->data.p_doc);
    do_date(document->data.p_date, &tm1);
    date_str = do_datetoa(tm1);
    value = do_strdup_printf("Закрытие возврата %s от %s", str, date_str);
    PROPERTY_STRING(node ,"Содержание", value);
    do_free(value);do_free(str);do_free(date_str);
    PROPERTY_STRING(node ,"СпособЗаполнения", "Вручную");
    PROPERTY_NUMBER(node ,"СуммаОперации", do_document_param_float(alias, document, "20"));

    account_dt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "РасчетыСПоставщиками");
    if ( !account_dt ) {
        MY_ERROR("Не найден счет РасчетыСПоставщиками в плане счетов");
        return FALSE;
    }
    account_dt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Контрагенты");
    if ( !account_dt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто \"Контрагенты\"");
        return FALSE;
    }
    account_dt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Договоры");
    if ( !account_dt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто \"Договоры\"");
        return FALSE;
    }
    ExchangeObject *partner;
    partner = get_object_partner_from_document(alias, document, objs);
    if ( !partner )
        return FALSE;

    ExchangeObject *contract;
    contract = get_partner_contract(objs, partner, "СПоставщиком", firm);
    if ( !contract )
        return FALSE;
    //PROPERTY_OBJECT(node, "ДоговорКонтрагента", contract);


    account_kt = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ACCOUNT_DEFAULT, "ПрочиеТоварыОтгруженные");
    if ( !account_kt ) {
        MY_ERROR("Не найден счет ПрочиеТоварыОтгруженные в плане счетов");
        return FALSE;
    }
    account_kt_sub_key1 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Контрагенты");
    if ( !account_kt_sub_key1 ) {
        MY_ERROR("Не найден вид субконто Контрагенты");
        return FALSE;
    }

    account_kt_sub_key2 = find_exchange_object_by_const_name(objs, EXCHANGE_TYPE_ATTRIBUTE_ACCOUNT_DEFAULT, "Номенклатура");
    if ( !account_kt_sub_key2 ) {
        MY_ERROR("Не найден вид субконто Номенклатура");
        return FALSE;
    }
    table = add_set(node, "Хозрасчетный");
    int i;
    double sum_ = 0;
    const vat_const_t *vat_const = get_vat_const(document->data.date);
    for ( i = 0; i < VAT_NO; i++ ) {
        char buf[20];
        sprintf(buf, "%d", vat_const[i].sum_r);
        sum_ += do_document_param_float(alias, document, buf);
    }
    if ( sum_ == 0 ) {
        do_document_param_float_set(alias, document, "11010", do_document_param_float(alias, document, "20"));
    }
    for ( i = 0; i < VAT_NO; i++ ) {
        double price;
        char buf[20];
        sprintf(buf, "%d", vat_const[i].sum_r);
        price = do_document_param_float(alias, document, buf);//!!
        if ( !price )
            continue;

        obj = get_1c_product_by_vat(objs, vat_const[i]);
        if ( !obj )
            return FALSE;
        record = add_record(table);
        PROPERTY_DATE(record, "Период", &tm);
        PROPERTY_OBJECT(record, "СчетДт", account_dt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key1);
        PROPERTY_OBJECT(sub, "Значение", partner);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоДт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_dt_sub_key2);
        PROPERTY_OBJECT(sub, "Значение", contract);

        PROPERTY_OBJECT(record, "СчетКт", account_kt);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key1);
        PROPERTY_OBJECT(sub, "Значение", partner);
        sub = xmlNewChild(record, NULL,  BAD_CAST "СубконтоКт", BAD_CAST "");
        PROPERTY_OBJECT(sub, "Ключ", account_kt_sub_key2);
        PROPERTY_OBJECT(sub, "Значение", obj);

        PROPERTY_OBJECT(record, "Организация", firm);
        PROPERTY_EMPTY(record, "ВалютаДт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_EMPTY(record, "ВалютаКт", EXCHANGE_TYPE_HANDBOOK_VALUTA);
        PROPERTY_NUMBER(record, "Сумма", price);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "ВалютнаяСуммаКт", EXCHANGE_TYPE_CONST_NUMBER);
        //PROPERTY_NUMBER(record, "КоличествоДт", 1);
        PROPERTY_NUMBER(record, "КоличествоКт", 1);

        PROPERTY_EMPTY(record, "Содержание", EXCHANGE_TYPE_CONST_STRING);
        PROPERTY_EMPTY(record, "СуммаНУДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРДт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРДт", price);
        PROPERTY_EMPTY(record, "СуммаНУКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_EMPTY(record, "СуммаПРКт", EXCHANGE_TYPE_CONST_NUMBER);
        PROPERTY_NUMBER(record, "СуммаВРКт", price);
        PROPERTY_BOOL(record, "Активность", TRUE);
    }
    return TRUE;
}

static gboolean get_document_bank(do_alias_t *alias, struct tm *tm1, struct tm *tm2, const gchar *user_name, xmlNode *root, xmlDoc *doc);
gboolean make_bank_documents(const gchar *aliases, struct tm *tm1, struct tm *tm2, const gchar *filename, const gchar *user_name)
{
    do_string_list_t *s;
    int i;
    xmlNode *root;
    xmlDoc *doc;
    gboolean res;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root = xmlNewNode(NULL, BAD_CAST ELEM_1C_ROOT);
    xmlDocSetRootElement(doc, root);
    res = TRUE;

    s = do_string_list_new_from_csv(aliases);
    for ( i = 0; res && i < s->count; i++ ) {
        if ( CANCELLED )
            return FALSE;
        do_alias_t *alias;
        alias = domino_alias_new(s->list[i]);
        if ( !alias || !do_alias_open(alias, TRUE) ) {
            MY_ERROR("Невозможно открыть алиас \"%s\"", (char*)s->list[i]);
            res = FALSE;
            break;
        }
        res = get_document_bank(alias, tm1, tm2, user_name, root, doc);

        do_alias_free(alias);
    }
    do_string_list_free(s);
    if ( res ) {
        int ret;
        ret = xmlSaveFormatFileEnc(filename, doc, "UTF-8", -1);
        if ( ret < 0 ) {
            MY_ERROR("Невозможно записать файл \"%s\"", filename);
            res = FALSE;
        }
    }
    xmlFreeDoc(doc);
    return res;
}
static GSList *get_bank_units(do_alias_t *alias);
static gboolean get_document_bank_payment(do_alias_t *alias, document_rec_t *document, xmlNode *root, xmlDoc *doc);
//static gboolean get_document_close_debts(do_alias_t *alias, document_rec_t *document, xmlNode *root);
static gboolean get_document_bank(do_alias_t *alias, struct tm *tm1, struct tm *tm2, const gchar *user_name, xmlNode *root, xmlDoc *doc)
{
    GSList *l, *units;
    gboolean res;

    units = get_bank_units(alias);
    if ( !units )
        return FALSE;
    res = TRUE;
    for ( l=units; l && res; l=l->next ) {
        document_key3_t document_key3, key;
        document_rec_t document;
        int status;
        do_text_set(alias, document_key3.dtype, "51");
        do_cpy(document_key3.sklad, ((sklad_key0_t*)l->data)->code);
        do_date_set(&document_key3.date, *tm1);
        document_key3.time = 0;
        do_cpy(key.dtype, document_key3.dtype);
        do_cpy(key.sklad, document_key3.sklad);
        do_date_set(&key.date, *tm2);
        status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
        while ( status == DO_OK ) {
            if ( CANCELLED )
                return FALSE;
            if ( do_cmp(key.dtype, document_key3.dtype) ||
                 do_cmp(key.sklad, document_key3.sklad) ||
                (document_key3.date > key.date) ) break;
            if ( document.data.accepted ) {
                gchar *dtype1;
                dtype1 = do_text(alias, document.data.dtype1);
                if ( !strcmp(dtype1, "021") )
                    res = get_document_bank_payment(alias, &document, root, doc);
                else
                if ( !strcmp(dtype1, "104") )
                    res = res; //get_document_close_debts(alias, &document, root);
                else
                    MY_ERROR("Неизвестный тип п/п %s", dtype1)
                g_free(dtype1);
            }
            if ( !res )
                break;
            status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
        }
        if ( status == DO_ERROR )
            res = FALSE;
    }
    return res;

}
static GSList *get_bank_units(do_alias_t *alias)
{
    otdel_key0_t otdel_key0;
    otdel_rec_t otdel;
    partner_key0_t partner_key0;
    int status;

    do_text_set(alias, otdel_key0.code, domino_division());
    status = do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        MY_ERROR("Отдел не найден");
        return NULL;
    }
    char *ch;
    char *value = do_otdel_param(alias, &otdel, "01");
    if ( (ch = strchr(value, '.')) == NULL ) {
        do_free(value);
        MY_ERROR("Расчетный счет не определен");
        return NULL;
    }
    *ch = '\0';
    do_text_set(alias, partner_key0.g_code, value);
    partner_key0.code = atoi(ch + 1);
    do_free(value);
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    do_text_set_empty(sklad_key0.code);
    GSList *result = NULL;

    status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        gchar *type;
        type = do_text(alias, sklad.data.type_);
        if ( !strcmp(type, "РС") ) {
           /*!! if ( !do_cmp(sklad.data.p_g_code, partner_key0.g_code) &&
                  sklad.data.p_code == partner_key0.code ) {*/
                result = g_slist_append(result, g_memdup(&sklad_key0, sizeof(sklad_key0)));
            //}
        }
        g_free(type);
        status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return NULL;
    if ( !result )
        MY_ERROR("Не найденно ни одного банковского подразделения");

    return result;
}
static gchar *get_partner_guid(do_alias_t *alias, partner_key0_t *partner_key0)
{
    partner_data_key0_t partner_data_key0;
    partner_data_rec_t partner_data;
    gchar *guid;
    int status;

    do_cpy(partner_data_key0.g_code, partner_key0->g_code);
    partner_data_key0.code = partner_key0->code;
    partner_data_key0.type = PARTNER_DATA_DOMINO_TYPE;
    partner_data_key0.number = firm_guid_number;

    status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( CANCELLED )
        return NULL;
    if ( status == DO_OK ) {
        guid = do_partner_data_param(alias, &partner_data, "GUID");
        if ( strcmp(guid, CONST_EMPTY_GUID) )
            return guid;
    }
    return NULL;
}
static gchar  *get_firm_guid_from_document(do_alias_t *alias, document_rec_t *document)
{
    otdel_key0_t otdel_key0;
    otdel_rec_t otdel;
    partner_key0_t partner_key0;
    int status;

    do_text_set(alias, otdel_key0.code, domino_division());
    status = do_otdel_get0(alias, &otdel, &otdel_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        MY_ERROR("Отдел не найден");
        return NULL;
    }
    char *ch;
    char *value = do_otdel_param(alias, &otdel, do_param(DO_PARAM_OTDEL_PARAMETR_FIRM));
    if ( (ch = strchr(value, '.')) == NULL ) {
        do_free(value);
        MY_ERROR("Фирма не определена");
        return NULL;
    }
    *ch = '\0';
    do_text_set(alias, partner_key0.g_code, value);
    partner_key0.code = atoi(ch + 1);
    do_free(value);

    return get_partner_guid(alias, &partner_key0);

}
static gchar  *get_bank_accont_from_sklad(do_alias_t *alias, gchar *store)
{
    sklad_key0_t sklad_key0;
    partner_data_key0_t partner_data_key0;
    sklad_rec_t sklad;
    partner_data_rec_t partner_data;
    int status;

    do_text_set(alias, sklad_key0.code, store);
    status = do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL);
    if ( status == DO_ERROR )
        return NULL;
    if ( status == DO_KEY_NOT_FOUND ) {
        MY_ERROR("Склад не найден");
        return NULL;
    }
    do_cpy(partner_data_key0.g_code, sklad.data.p_g_code);
    partner_data_key0.code = sklad.data.p_code;
    partner_data_key0.type = 6;
    partner_data_key0.number = 0;
    status = do_partner_data_get0(alias, &partner_data, &partner_data_key0, DO_GET_GE);
    if ( status != DO_OK || do_cmp(partner_data_key0.g_code, sklad.data.p_g_code) ||
         partner_data_key0.code != sklad.data.p_code || partner_data_key0.type != 6 )
        return do_strdup("");
    return do_partner_data_param(alias, &partner_data, "3");
}
static gchar  *get_partner_guid_from_document(do_alias_t *alias, document_rec_t *document)
{
    partner_key0_t partner_key0;
    do_cpy(partner_key0.g_code, document->data.p_g_code);
    partner_key0.code = document->data.p_code;

    return get_partner_guid(alias, &partner_key0);

}
#define NODE_STRING(root, name, value) {xmlNode *node1; node1 = xmlNewChild(root, NULL, BAD_CAST name, NULL); xmlNodeAddContent(node1, BAD_CAST value);}
#define NODE_DATE(root, name, date) {char buf[256];  strftime(buf, 255, DATE_FORMAT_1C, date); xmlNewChild(root, NULL, BAD_CAST name, BAD_CAST buf);}
#define NODE_NUMBER(root, name, number) {char buf[256]; do_ftoa(number, buf); xmlNewChild(root, NULL, BAD_CAST name, BAD_CAST buf);}
#define NODE_BOOL(root, name, value) {char buf[256]; sprintf(buf, "%s", value ? CONST_1C_TRUE : CONST_1C_FALSE);; xmlNewChild(root, NULL, BAD_CAST name, BAD_CAST buf);}

static gboolean get_document_bank_payment(do_alias_t *alias, document_rec_t *document, xmlNode *root, xmlDoc *doc)
{
    xmlNode *node;
    node = xmlNewChild(root, NULL, BAD_CAST "Документ", BAD_CAST "");
    xmlNewProp(node, BAD_CAST "тип", BAD_CAST "ПлатежноеПоручение");
    gchar *value, *str;

    value = get_firm_guid_from_document(alias, document);
    if ( !value ) {
        MY_ERROR("Фирма не определена");
        return FALSE;
    }
    NODE_STRING(node, "Организация", value);
    g_free(value);
    gchar *store;
    store = do_text(alias, document->data.sklad);
    value = get_bank_accont_from_sklad(alias, store);
    do_free(store);
    if ( !value ) {
        MY_ERROR("Расчетный счет не найден");
        return FALSE;
    }
    NODE_STRING(node, "ОрганизацияНомерСчета", value);
    g_free(value);
    value = get_partner_guid_from_document(alias, document);
    if ( !value ) {
        MY_ERROR("Партнер не определен. Документ \"%s.%s.%s\"", do_text(alias, document->data.dtype), do_text(alias, document->data.sklad),do_text(alias, document->data.document));
        return FALSE;
    }
    NODE_STRING(node, "Контрагент", value);
    g_free(value);

    struct tm tm;
    do_date(document->data.date, &tm);
    do_time(document->data.time, &tm);
    NODE_DATE(node, "Дата", &tm);
    NODE_NUMBER(node, "Сумма", do_document_param_float(alias, document, "20"));
    NODE_NUMBER(node, "СуммаНДС", do_document_param_float(alias, document, "19"));
    value = do_document_param(alias, document, "BUH15");
    gchar *ch, *tail;
    ch = value;
    if ( *ch != '\0' ) {
        for ( ; *ch != '\0' && *ch == '"'; ch++ );
        tail = value + strlen(value) - 1;
        for ( ; tail > ch && *tail == '"'; tail-- )
            *tail = '\0';

    }
    NODE_STRING(node, "Назначение", ch);
    g_free(value);
    value = do_document_param(alias, document, "57");
    NODE_STRING(node, "НомерСчета", value);
    g_free(value);
    str = do_util_document_to_str(alias, document);
    value = g_strdup_printf("Перенесено из Домино %s", str);
    NODE_STRING(node, "Комментарий", value);
    g_free(value);g_free(str);
    return TRUE;
}
/*
static gboolean get_document_close_debts(do_alias_t *alias, document_rec_t *document, xmlNode *root)
{
    xmlNode *node;
    node = xmlNewChild(root, NULL, BAD_CAST "Документ", BAD_CAST "");
    xmlNewProp(node, BAD_CAST "тип", BAD_CAST "ЗакрытиеОплаты");
    gchar *value, *str;

    value = get_firm_guid_from_document(alias, document);
    if ( !value ) {
        MY_ERROR("Фирма не определена");
        return FALSE;
    }
    NODE_STRING(node, "Организация", value);
    g_free(value);
    value = get_partner_guid_from_document(alias, document);
    if ( !value ) {
        MY_ERROR("Партнер не определен");
        return FALSE;
    }
    NODE_STRING(node, "Контрагент", value);
    g_free(value);
    struct tm tm;
    do_date(document->data.date, &tm);
    do_time(document->data.time, &tm);
    NODE_DATE(node, "Дата", &tm);
    NODE_NUMBER(node, "Сумма", do_document_param_float(alias, document, "20"));
    value = do_document_param(alias, document, "57");
    NODE_STRING(node, "НомерСчета", value);
    g_free(value);
    str = do_util_document_to_str(alias, document);
    value = g_strdup_printf("Перенесено из Домино %s", str);
    NODE_STRING(node, "Комментарий", value);
    g_free(value);g_free(str);
    return TRUE;
}
*/
static gboolean get_other_mismatch_sums(do_alias_t *alias, document_rec_t *doc, double *sums, double *sum)
{
    document_link_rec_t document_link;
    document_link_key1_t document_link_key1;
    document_link_key0_t document_link_key0, key;
    int status;
    int i;
    document_type_key0_t document_type_key0;

    for ( i = 0; i < VAT_NO; i++ )
        sums[i] = 0;
    *sum = 0;

    gchar *dtype;
    dtype = do_text(alias, doc->data.dtype);
    if ( !strcmp(dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN)) )
        do_text_set(alias, document_type_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    else
        do_text_set(alias, document_type_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));

    do_cpy(document_link_key1.s_dtype, doc->data.dtype);
	do_cpy(document_link_key1.s_sklad, doc->data.sklad);
	do_cpy(document_link_key1.s_document, doc->data.document);
	document_link_key1.type = 110;
	document_link_key1.sort = 0;

	status = do_document_link_get1(alias, &document_link, &document_link_key1, DO_GET_GE);
	if ( status != DO_OK )
        return FALSE;
    if ( do_cmp(document_link_key1.s_dtype, doc->data.dtype) ||
         do_cmp(document_link_key1.s_sklad, doc->data.sklad) ||
	     do_cmp(document_link_key1.s_document, doc->data.document) ||
	     document_link_key1.type != 110
        )
        return FALSE;

    do_cpy(document_link_key0.m_dtype, document_link.data.m_dtype);
	do_cpy(document_link_key0.m_sklad, document_link.data.m_sklad);
	do_cpy(document_link_key0.m_document, document_link.data.m_document);
	document_link_key0.type = 110;
	document_link_key0.sort = 0;
	do_cpy(key, document_link_key0);

    const vat_const_t *vat_const = get_vat_const(doc->data.date);

    status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(document_link_key0.m_dtype, key.m_dtype) ||
           	 do_cmp(document_link_key0.m_sklad, key.m_sklad) ||
	         do_cmp(document_link_key0.m_document, key.m_document) ||
	         document_link_key0.type != 110
            ) break;
        if ( !do_cmp(document_type_key0.dtype, document_link.data.s_dtype) ) {
            document_key0_t document_key0;
            document_rec_t document;

            do_cpy(document_key0.dtype, document_link.data.s_dtype);
            do_cpy(document_key0.sklad, document_link.data.s_sklad);
            do_cpy(document_key0.document, document_link.data.s_document);

            if ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) != DO_OK )
                return FALSE;
            *sum = do_document_sum(alias, &document, 11);
            for ( i = 0; i < VAT_NO; i++ )
                sums[i] = do_document_sum(alias, &document, vat_const[i].sum_r);
            break;
        }
        status = do_document_link_get0(alias, &document_link, &document_link_key0, DO_GET_NEXT);
    }
    if ( status == DO_ERROR )
        return FALSE;

    return TRUE;
}

static ExchangeObject *get_1c_product_by_vat(GSList *objs, vat_const_t vat_const)
{
    ExchangeObject *obj;
    obj = find_exchange_object_by_value_string(objs, EXCHANGE_TYPE_HANDBOOK_PRODUCT, "СтавкаНДС", vat_const.name);
    if ( !obj ) {
        MY_ERROR("Товар со ставкой НДС \"%s\" не найден", vat_const.name);
        return NULL;
    }
    return obj;
}
inline static const vat_const_t *get_vat_const(BTI_LONG date)
{
   return ( date >= DOMINO_DATE_20190101 ) ? vat_const19 : vat_const18;
}
