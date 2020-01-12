

#include "traffic.h"
#include <datetime.h>
#include "../var/db.h"
#include "../var/util.h"
#include "misc.h"
#include <domino.h>
#include <domino_action.h>

#define DAYS_WRITE_OFF 60

struct _TrafficPrivate
{
	Alias *obj_alias;
	do_alias_t *alias;
};

static void Traffic_dealloc(PyObject* self)
{
    LOG("Traffic free\n");
    if ( ((Traffic*)self)->priv->obj_alias )
	    Py_DECREF(((Traffic*)self)->priv->obj_alias);
	else
        if ( ((Traffic*)self)->priv->alias )
            do_alias_free(((Traffic*)self)->priv->alias);
    do_free(((Traffic*)self)->priv);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *Traffic_stocks(Traffic *self, PyObject *args, PyObject *kwds);
static PyObject *Traffic_order(Traffic *self, PyObject *args, PyObject *kwds);
static PyObject *Traffic_limit(Traffic *self, PyObject *args, PyObject *kwds);
static PyObject *Traffic_rebuild_stocks(Traffic *self, PyObject *args, PyObject *kwds);
static PyObject *Traffic_info(Traffic *self, PyObject *args, PyObject *kwds);
static PyObject *get_product_info(do_alias_t *alias, const char *code, const char *store);
static PyObject *Traffic_set_marked(Traffic *self, PyObject *args, PyObject *kwds);
static PyObject *Traffic_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Traffic new\n");
    Traffic *self;
    self = PyObject_NEW(Traffic, type);
    //self = (Traffic *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(TrafficPrivate));
    return (PyObject *)self;
}
static int Traffic_init(Traffic *self, PyObject *args, PyObject *kwds)
{
    //LOG("Traffic init\n");
    static char *kwlist[] = {"alias", "alias_name", NULL};
    PyObject *obj_alias = NULL;
    char *alias_name = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|Os", kwlist, &obj_alias, &alias_name) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }
    if ( obj_alias ) {
        if ( Py_TYPE(obj_alias) != getAliasType() ) {
            do_log(LOG_ERR, "no param \"alias\"");
            return -1;
        }
        Py_INCREF(obj_alias);
        self->priv->obj_alias = (Alias*)obj_alias;
        self->priv->alias = ((Alias*)obj_alias)->alias;
    }
    else {
        self->priv->alias = domino_alias_new(alias_name);
        if ( !self->priv->alias )
            return -1;
        do_alias_open(self->priv->alias, TRUE);
    }
    PyDateTime_IMPORT;
    return 0;
}

typedef struct {
    domino_marked_t marked;
    struct tm tm;
    struct tm last_tm;
    double quant;
    char *user;
} order_item_t;

static int get_product_key(do_alias_t *alias, PyObject *product, product_key0_t *key);
static double get_product_rest(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm, int number, int price, double *sum, int only);
static double get_product_sale(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1, struct tm *tm2, double *sum);
static double get_product_purchase(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1, struct tm *tm2, double *sum);
static do_list_t *get_product_order(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1, struct tm *tm2);
static void order_item_free(order_item_t *item)
{
    do_free(item->user);
}

static PyObject *Traffic_quarantine(Traffic *self, PyObject *args, PyObject *kwds);

static PyObject *Traffic_rest(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"product","stores","date", "number", "price", "only",  NULL};
    PyObject *product = NULL;
    PyObject *date = NULL;
    PyObject *stores = NULL;
    PyObject *only = NULL;
    int only_ = 0;
    int number = 1;
    int price = 2;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|OOiiO", kwlist, &product, &stores, &date, &number, &price, &only) ) {
        do_log(LOG_ERR, "no param \"product1\"");
        return NULL;
    }
    product_key0_t product_key0;
    do_string_list_t *stores_list;
    struct tm tm = {0,};
    int i;
    double quant;
    double sum;

    if ( only ) only_= PyObject_IsTrue(only);

    if ( !get_product_key(self->priv->alias, product, &product_key0) )
        return NULL;

    if ( !stores  )
        stores_list = do_string_list_new_from_csv(domino_unit());
    else {
        stores_list = get_store_code_list(stores);
        if ( !stores_list ) {
            do_log(LOG_ERR, "Invalid argument \"stores\": wrong type");
            return NULL;
        }
    }
    if ( date ) {
        if ( !PyDateTime_Check(date) && !PyDate_Check(date) ) {
            do_log(LOG_ERR, "Invalid argument \"date\": wrong type");
            do_string_list_free(stores_list);
            return NULL;
        }
        int year, month, day;
        year = PyDateTime_GET_YEAR(date);
        month = PyDateTime_GET_MONTH(date);
        day = PyDateTime_GET_DAY(date);
        do_date_set_ymd(&tm, year, month, day);
    }
    quant = 0;
    sum = 0;
    for ( i = 0; i < stores_list->count; i++ ) {
        double s;
        quant+=get_product_rest(self->priv->alias, &product_key0, stores_list->list[i], &tm, number, price, &s, only_);
        sum+=s;
    }
    do_string_list_free(stores_list);

    PyObject *result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, PyFloat_FromDouble(quant));
    PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(sum));

    return result;
}
static PyObject *Traffic_sale(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"product","stores","date1", "date2", NULL};
    PyObject *product = NULL;
    PyObject *date1 = NULL;
    PyObject *date2= NULL;
    PyObject *stores = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "OOOO", kwlist, &product, &stores, &date1, &date2) ) {
        do_log(LOG_ERR, "no params \"product\",\"stores\",\"date1\",\"date2\"");
        return NULL;
    }
    product_key0_t product_key0;
    do_string_list_t *stores_list;
    struct tm tm1 = {0,};
    struct tm tm2 = {0,};
    int i;
    double quant,sum;

    if ( !get_product_key(self->priv->alias, product, &product_key0) )
        return NULL;

    stores_list = get_store_code_list(stores);
    if ( !stores_list ) {
         do_log(LOG_ERR, "Invalid argument \"stores\": wrong type");
        return NULL;
    }
    if ( !PyDateTime_Check(date1) && !PyDate_Check(date1) ) {
         do_log(LOG_ERR, "Invalid argument \"date1\": wrong type");
         do_string_list_free(stores_list);
         return NULL;
    }
    int year, month, day;
    year = PyDateTime_GET_YEAR(date1);
    month = PyDateTime_GET_MONTH(date1);
    day = PyDateTime_GET_DAY(date1);
    do_date_set_ymd(&tm1, year, month, day);
    if ( !PyDateTime_Check(date2) && !PyDate_Check(date2) ) {
         do_log(LOG_ERR, "Invalid argument \"date2\": wrong type");
         do_string_list_free(stores_list);
         return NULL;
    }
    year = PyDateTime_GET_YEAR(date2);
    month = PyDateTime_GET_MONTH(date2);
    day = PyDateTime_GET_DAY(date2);
    do_date_set_ymd(&tm2, year, month, day);

    quant = 0;
    sum = 0;
    for ( i = 0; i < stores_list->count; i++ ) {
        double s;
        quant+=get_product_sale(self->priv->alias, &product_key0, stores_list->list[i], &tm1, &tm2, &s);
        sum+=s;
    }
    do_string_list_free(stores_list);

    PyObject *result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, PyFloat_FromDouble(quant));
    PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(sum));

    return result;
}
typedef struct {
    struct tm tm;
    double quant;
} realiz_t;
typedef struct {
    struct tm tm;
    char name[150];
    double quant;
    double price;
} purchase_t;

static int get_realiz(do_alias_t *alias, product_rec_t *product, const char *unit, realiz_t *month, realiz_t *season)
{
	realization_rec_t realization;
	realization_key4_t realization_key4, key;

	int status, count = 0;
	double coef;
    struct tm tm;
    time_t now;
    int i;
    now = time(NULL);
    tm = *localtime(&now);

    for( i = 0; i < 4; i++ ) {
        if ( month )
            month[i].quant = 0;
        if ( season )
            season[i].quant = 0;
    }

	do_text_set(alias, realization_key4.sklad, unit);
	do_cpy(realization_key4.code, product->data.code);
	do_cpy(key.sklad, realization_key4.sklad);
	do_cpy(key.code, realization_key4.code);
	coef = do_product_coef(alias, product);


	realization_key4.month = do_datetosql(tm);
	tm.tm_year--;
	tm.tm_mday=0;
	key.month = do_datetosql(tm);
	//fprintf(stderr,"%s\n", do_text(alias, product->data.code));

	status = do_realization_get4(alias, &realization, &realization_key4, DO_GET_LE);
	while ( status == DO_OK ) {
        if ( do_cmp(key.sklad, realization_key4.sklad) ||
             do_cmp(key.code, realization_key4.code)
           ) break;

        if ( count >= 4 && key.month > realization_key4.month )
            break;

        if ( realization.data.total > 0.001 ) {

            do_sqltodate(realization.data.month, &tm);
            if ( count <= 3 && month ) {
                month[count].tm = tm;
                month[count].quant=realization.data.total/coef;
            }
            if ( season ) {
                switch (tm.tm_mon) {
                    case 0:
                    case 1:
                    case 11:
                        season[0].quant += realization.data.total/coef;
                        break;
                    case 2:
                    case 3:
                    case 4:
                        season[1].quant += realization.data.total/coef;
                        break;
                    case 5:
                    case 6:
                    case 7:
                        season[2].quant += realization.data.total/coef;
                        break;
                    case 8:
                    case 9:
                    case 10:
                        season[3].quant += realization.data.total/coef;
                        break;

                }
            }
            count++;

        }
        status = do_realization_get4(alias, &realization, &realization_key4, DO_GET_PREVIOUS);
	}

    if ( status == DO_ERROR )
        return FALSE;
    return TRUE;
}
const char *season_name(int season)
{
    switch (season) {
        case 0:
            return "Зима";
        case 1:
            return "Весна";
        case 2:
            return "Лето";
        case 3:
            return "Осень";
        default:
            return "";
    }
}

static int get_purchase(do_alias_t *alias, product_rec_t *product, const char *unit, purchase_t *last)
{
	prowod_key2_t prowod_key2, prowod_key;
	prowod_rec_t prowod;
	int status, i;
	char  *value;
	double coef;
	char *code;

	for ( i = 0; i < 4; i++ )
        last[i].quant=0;

    code = do_text(alias, product->data.code);
    coef = do_product_coef(alias, product);

	do_text_set(alias, prowod_key2.division, domino_division());
    do_text_set(alias, prowod_key2.debit_account, do_param(DO_PARAM_ACCOUNT_PRODUCT_IN));
    do_text_set_empty(prowod_key2.debit_subaccount);
    do_text_set_empty(prowod_key2.debit_p_g_code);
    prowod_key2.debit_p_code = 0;
    value = do_strdup_printf("%s.%s", unit, code);
    do_text_set(alias, prowod_key2.debit_other, value);
    do_free(value);
    prowod_key2.date = INT_MAX;
    memcpy(&prowod_key, &prowod_key2, sizeof(prowod_key2) - sizeof(prowod_key2.date));

    status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_LT);

    for (i = 0; i < 4 && (status == DO_OK); i++) {
        if (memcmp(&prowod_key, &prowod_key2, sizeof(prowod_key2) - sizeof(prowod_key2.date)))
            break;

        //gchar *quantity, *price;
        //gchar *price;
        //gchar *supplier;
        //struct tm tm;

        do_date(prowod.data.date, &last[i].tm);
        last[i].quant=do_prowod_sum(alias, &prowod, DO_CONST_PROWOD_SUM_QUANT)/coef;
        last[i].price=do_prowod_sum(alias, &prowod, DO_CONST_PROWOD_SUM_PRICE)*coef;
        char *buf;
        buf = do_prowod_param(alias, &prowod, "КОММ");
        strncpy(last[i].name, buf, 149);
        if ( strlen(buf) < 149 )
            last[i].name[strlen(buf)] = '\0';
        else
            last[i].name[149] = '\0';
        do_free(buf);
        //supplier = do_prowod_param(alias, &prowod, do_param(DO_PARAM_PROWOD_PARAMETR_COMMENT));

        status = do_prowod_get2(alias, &prowod, &prowod_key2, DO_GET_PREVIOUS);
	}
	do_free(code);
	if ( status == DO_ERROR )
        return FALSE;

	return TRUE;
}
static PyObject *Traffic_purchase(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"product","stores","date1", "date2", NULL};
    PyObject *product = NULL;
    PyObject *date1 = NULL;
    PyObject *date2= NULL;
    PyObject *stores = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "OOOO", kwlist, &product, &stores, &date1, &date2) ) {
        do_log(LOG_ERR, "no params \"product\",\"stores\",\"date1\",\"date2\"");
        return NULL;
    }
    product_key0_t product_key0;
    do_string_list_t *stores_list;
    struct tm tm1 = {0,};
    struct tm tm2 = {0,};
    int i;
    double quant,sum;

    if ( !get_product_key(self->priv->alias, product, &product_key0) )
        return NULL;

    stores_list = get_store_code_list(stores);
    if ( !stores_list ) {
         do_log(LOG_ERR, "Invalid argument \"stores\": wrong type");
        return NULL;
    }
    if ( !PyDateTime_Check(date1) && !PyDate_Check(date1) ) {
         do_log(LOG_ERR, "Invalid argument \"date1\": wrong type");
         do_string_list_free(stores_list);
         return NULL;
    }
    int year, month, day;
    year = PyDateTime_GET_YEAR(date1);
    month = PyDateTime_GET_MONTH(date1);
    day = PyDateTime_GET_DAY(date1);
    do_date_set_ymd(&tm1, year, month, day);
    if ( !PyDateTime_Check(date2) && !PyDate_Check(date2) ) {
         do_log(LOG_ERR, "Invalid argument \"date2\": wrong type");
         do_string_list_free(stores_list);
         return NULL;
    }
    year = PyDateTime_GET_YEAR(date2);
    month = PyDateTime_GET_MONTH(date2);
    day = PyDateTime_GET_DAY(date2);
    do_date_set_ymd(&tm2, year, month, day);

    quant = 0;
    sum = 0;
    for ( i = 0; i < stores_list->count; i++ ) {
        double s;
        quant+=get_product_purchase(self->priv->alias, &product_key0, stores_list->list[i], &tm1, &tm2, &s);
        sum+=s;
    }
    do_string_list_free(stores_list);

    PyObject *result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, PyFloat_FromDouble(quant));
    PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(sum));

    return result;
}

static PyObject *Traffic_balance(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"store","date", NULL};
    PyObject *date = NULL;
    char *store = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &store, &date) ) {
        do_log(LOG_ERR, "no params \"store\",\"date\"");
        return NULL;
    }
    struct tm tm1 = {0,};
    double sum;

    if ( !store ) {
         do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
        return NULL;
    }
    if ( !PyDateTime_Check(date) && !PyDate_Check(date) ) {
         do_log(LOG_ERR, "Invalid argument \"date\": wrong type");
         return NULL;
    }
    int year, month, day;
    year = PyDateTime_GET_YEAR(date);
    month = PyDateTime_GET_MONTH(date);
    day = PyDateTime_GET_DAY(date);
    do_date_set_ymd(&tm1, year, month, day);
    sum = 0;

    document_key3_t document_key3, key;
    do_alias_t *alias = self->priv->alias;
    int i;
    struct PACK_STRUCT {
        BTI_CHAR params[1024];
    } *ret_;

    for ( i = 0; i < 2; i++ ) {
        if ( i )
            do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
        else
            do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));

        do_text_set(alias, document_key3.sklad, store);
        document_key3.date = 0;
        document_key3.time = 0;

        do_cpy(key.dtype, document_key3.dtype);
        do_cpy(key.sklad, document_key3.sklad);
        do_date_set(&key.date, tm1);
        do_extended_t *extended;
        extended = do_extended_new(alias);

        do_extended_set_file_db(extended, DO_DB_DOCUMENT);
        do_extended_set_key(extended, 3, &document_key3);


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
                                DO_EXTENDED_COMP_LT,
                                DO_EXTENDED_CONNECTION_NO,
                                &key.date);
        do_extended_set_fields(extended, 1,
                                        do_size_and_offset(document_struct_t, params));
        do_ret_list_t *list = NULL;
        while ( ((list = do_extended_get(extended, 52)) != NULL) &&
            do_ret_list_count(list) ) {
            int indx;
            char *params;
            for (indx = 0; indx < do_ret_list_count(list); indx++) {
                 ret_ = do_ret_list_item_data(list, indx);
                 params = do_alias_text_(alias, ret_->params,
                              do_ret_list_item_len(list, indx));
                 if ( i )
                      sum += do_sum(params, 11, TRUE);
                 else
                      sum -= do_sum(params, 11, TRUE);
                 do_free(params);
             }
        }
        do_ret_list_free(list);
        do_extended_free(extended);
    }
    return PyFloat_FromDouble(sum);
}
static PyMethodDef Traffic_methods[] = {
    {"rest", (PyCFunction)Traffic_rest, METH_VARARGS|METH_KEYWORDS, "rest"},
    {"sale", (PyCFunction)Traffic_sale, METH_VARARGS|METH_KEYWORDS, "sale"},
    {"purchase", (PyCFunction)Traffic_purchase, METH_VARARGS|METH_KEYWORDS, "purchase"},
    {"balance", (PyCFunction)Traffic_balance, METH_VARARGS|METH_KEYWORDS, "balance"},
    {"stocks", (PyCFunction)Traffic_stocks, METH_VARARGS|METH_KEYWORDS, "stocks"},
    {"rebuild_stocks", (PyCFunction)Traffic_rebuild_stocks, METH_VARARGS|METH_KEYWORDS, "rebuild-stocks"},
    {"quarantine", (PyCFunction)Traffic_quarantine, METH_VARARGS|METH_KEYWORDS, "quarantine"},
    {"order", (PyCFunction)Traffic_order, METH_VARARGS|METH_KEYWORDS, "order"},
    {"limit", (PyCFunction)Traffic_limit, METH_VARARGS|METH_KEYWORDS, "limit"},
    {"info", (PyCFunction)Traffic_info, METH_VARARGS|METH_KEYWORDS, "info"},
    {"set_marked", (PyCFunction)Traffic_set_marked, METH_VARARGS|METH_KEYWORDS, "set_marked"},


    {NULL}
};

static PyTypeObject TrafficType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Traffic",             /*tp_name*/
    sizeof(Traffic),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Traffic_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,0/*(reprfunc)Traffic_fields*/,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Traffic objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Traffic_methods,             /* tp_methods */
    0,0,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Traffic_init,      /* tp_init */
    0,                         /* tp_alloc */
    Traffic_new,                 /* tp_new */
};
PyTypeObject *getTrafficType()
{
    return &TrafficType_;
}

static int get_product_key(do_alias_t *alias, PyObject *product, product_key0_t *key)
{
    if ( MyString_Check(product) ) {
        char *code = NULL;
        code = MyString_AsString(product);
        do_text_set(alias, key->code, code);
        key->base_parcel = key->code[5] == '.';
        do_free(code);
        return TRUE;
    }
    if ( Py_TYPE(product) == getProductType() ) {
        do_cpy(key->code,((Product*)product)->priv->data.code);
        key->base_parcel = ((Product*)product)->priv->data.base_parcel;
    }
    else
    if ( Py_TYPE(product) == getProductKey0Type() ) {
        do_cpy(key->code,((ProductKey0*)product)->priv->code);
        key->base_parcel = ((ProductKey0*)product)->priv->base_parcel;
    }
    else {
        do_log(LOG_ERR, "Invalid argument \"product\": wrong type");
        return FALSE;
    }
    return TRUE;
}
static double get_product_parcel_rest(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm, int number, int price, double *sum);

static double get_product_rest(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm, int number, int price, double *sum, int only)
{
    double retval, s;
    retval = get_product_parcel_rest(alias, key, store, tm, number, price, &s);
    *sum = s;
    if ( key->base_parcel || only )
        return retval;

    product_key0_t product_key0;
    product_key4_t product_key4;
    int status;

    do_cpy(product_key4.code, key->code);
    snprintf(product_key4.code+5,4,".%2.2d", atoi(store));
    do_cpy(product_key0.code, product_key4.code);
    product_key0.base_parcel = 1;

    status = do_product_key0(alias, &product_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( strncmp(product_key0.code, product_key4.code, 8) ) break;
        retval+=get_product_parcel_rest(alias, &product_key0, store, tm, number, price, &s);
        *sum += s;
        status = do_product_key0(alias, &product_key0, DO_GET_NEXT);
    }

    return retval;
}
static double get_product_parcel_rest(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm, int number, int price, double *sum)
{
    int status;
    double retval;
    if ( !tm->tm_year ) {
        stock_key0_t stock_key0;
        stock_rec_t stock;
        product_rec_t product;

        do_cpy(stock_key0.code, key->code);
        do_text_set(alias, stock_key0.sklad, store);
        status = do_stock_get0(alias, &stock, &stock_key0, DO_GET_EQUAL);
        if ( status == DO_OK ) {
            retval = do_stock_quant(alias, &stock, number);
            if ( do_product_get0(alias, &product, key, DO_GET_EQUAL) == DO_OK )
                 *sum = do_product_price(alias, &product, price)*retval;
            else
                 *sum = 0;
        }
        else {
            retval = 0;
            *sum = 0;
        }
    }
    else {
        document_order_key3_t document_order_key3;
        document_order_rec_t document_order;
        BTI_LONG date;
        sklad_key0_t sklad;
        BTI_CHAR in[sizeof(document_order.data.dtype)],out[sizeof(document_order.data.dtype)];

        do_text_set(alias, in, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
        do_text_set(alias, out, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));

        do_cpy(document_order_key3.code, key->code);
        document_order_key3.date = 0;
        document_order_key3.time = 0;
        do_date_set(&date, *tm);
        do_text_set(alias, sklad.code, store);

        status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_GE);
        retval = 0;
        *sum = 0;
        while ( status == DO_OK ) {
            if ( do_cmp(document_order_key3.code, key->code) ||
                 document_order_key3.date > date ) break;

            if ( !do_cmp(document_order.data.sklad, sklad.code) ) {
                if ( !do_cmp(document_order.data.dtype, in) ) {
                    retval += do_document_order_quant(alias, &document_order, number);
                    *sum += do_document_order_price(alias, &document_order, price)*do_document_order_quant(alias, &document_order, number);
                }
                else
                if ( !do_cmp(document_order.data.dtype, out) ) {
                    retval -= do_document_order_quant(alias, &document_order, number);
                    *sum -= do_document_order_price(alias, &document_order, price)*do_document_order_quant(alias, &document_order, number);
                }
            }
            status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_NEXT);
        }

   }
   return retval;

}
static double get_product_parcel_sale(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1,  struct tm *tm2, double *sum)
{
    int status;
    double retval;
    int number = 1;

    document_order_key3_t document_order_key3;
    document_order_rec_t document_order;
    BTI_LONG date;
    sklad_key0_t sklad;
    BTI_CHAR dtype[sizeof(document_order.data.dtype)];

    do_text_set(alias, dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));

    do_cpy(document_order_key3.code, key->code);
    do_date_set(&document_order_key3.date, *tm1);
    document_order_key3.time = 0;
    do_date_set(&date, *tm2);
    do_text_set(alias, sklad.code, store);

    status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_GE);
    retval = 0;
    *sum = 0;
    while ( status == DO_OK ) {
        if ( do_cmp(document_order_key3.code, key->code) ||
             document_order_key3.date > date ) break;

        if ( !do_cmp(document_order.data.sklad, sklad.code) ) {
            if ( !do_cmp(document_order.data.dtype, dtype) &&
                 (document_order.data.dtype1[0] == '0' &&
                  document_order.data.dtype1[1] >= '1' &&
                  document_order.data.dtype1[1] <= '4') ) {
                retval += do_document_order_quant(alias, &document_order, number);
                *sum += do_document_order_quant(alias, &document_order, number)*do_document_order_price(alias, &document_order, 9);
            }
        }
        status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_NEXT);
    }
    return retval;
}
static double get_product_parcel_purchase(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1,  struct tm *tm2, double *sum)
{
    int status;
    double retval;
    int number = 1;

    document_order_key3_t document_order_key3;
    document_order_rec_t document_order;
    BTI_LONG date;
    sklad_key0_t sklad;
    BTI_CHAR dtype[sizeof(document_order.data.dtype)];

    do_text_set(alias, dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));

    do_cpy(document_order_key3.code, key->code);
    do_date_set(&document_order_key3.date, *tm1);
    document_order_key3.time = 0;
    do_date_set(&date, *tm2);
    do_text_set(alias, sklad.code, store);

    status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_GE);
    retval = 0;
    *sum = 0;
    while ( status == DO_OK ) {
        if ( do_cmp(document_order_key3.code, key->code) ||
             document_order_key3.date > date ) break;

        if ( !do_cmp(document_order.data.sklad, sklad.code) ) {
            if ( !do_cmp(document_order.data.dtype, dtype) &&
                 (document_order.data.dtype1[0] == '0' &&
                  document_order.data.dtype1[1] == '1') ) {
                retval += do_document_order_quant(alias, &document_order, number);
                *sum += do_document_order_quant(alias, &document_order, number)*do_document_order_price(alias, &document_order, 2);
            }
        }
        status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_NEXT);
    }
    return retval;
}
static double get_product_sale(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1, struct tm *tm2, double *sum)
{
    double retval,s;
    retval = get_product_parcel_sale(alias, key, store, tm1, tm2, &s);
    *sum = s;
    if ( key->base_parcel )
        return retval;

    product_key0_t product_key0;
    product_key4_t product_key4;
    int status;

    do_cpy(product_key4.code, key->code);
    snprintf(product_key4.code+5,4,".%2.2d", atoi(store));
    do_cpy(product_key0.code, product_key4.code);
    product_key0.base_parcel = 1;

    status = do_product_key0(alias, &product_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( strncmp(product_key0.code, product_key4.code, 8) ) break;
        retval+=get_product_parcel_sale(alias, &product_key0, store, tm1, tm2, &s);
        *sum+=s;
        status = do_product_key0(alias, &product_key0, DO_GET_NEXT);
    }

    return retval;
}
static double get_product_purchase(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1, struct tm *tm2, double *sum)
{
    double retval,s;
    retval = get_product_parcel_purchase(alias, key, store, tm1, tm2, &s);
    *sum = s;
    if ( key->base_parcel )
        return retval;

    product_key0_t product_key0;
    product_key4_t product_key4;
    int status;

    do_cpy(product_key4.code, key->code);
    snprintf(product_key4.code+5,4,".%2.2d", atoi(store));
    do_cpy(product_key0.code, product_key4.code);
    product_key0.base_parcel = 1;

    status = do_product_key0(alias, &product_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( strncmp(product_key0.code, product_key4.code, 8) ) break;
        retval+=get_product_parcel_purchase(alias, &product_key0, store, tm1, tm2, &s);
        *sum+=s;
        status = do_product_key0(alias, &product_key0, DO_GET_NEXT);
    }
    return retval;
}
typedef struct {
    BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
    double quant;
    double quant2;
    double quant3;
} stock_t;
static int cmp1(stock_t *s1, stock_t *s2)
{
    return strncmp(s1->code, s2->code, DOMINO_FIELD_CODE_SIZE);

}
static PyObject *Traffic_stocks(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"store","date", NULL};
    PyObject *date = NULL;
    char *store = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &store, &date) ) {
        do_log(LOG_ERR, "no params \"store\",\"date\"");
        return NULL;
    }
    struct tm tm1 = {0,};

    if ( !store ) {
         do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
        return NULL;
    }
    if ( !PyDateTime_Check(date) && !PyDate_Check(date) ) {
         do_log(LOG_ERR, "Invalid argument \"date\": wrong type");
         return NULL;
    }
    int year, month, day;
    year = PyDateTime_GET_YEAR(date);
    month = PyDateTime_GET_MONTH(date);
    day = PyDateTime_GET_DAY(date);
    do_date_set_ymd(&tm1, year, month, day);

    document_order_key4_t document_order_key4, key;
    do_alias_t *alias = self->priv->alias;
    int i;

    struct PACK_STRUCT {
        BTI_CHAR dtype[2];
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        //BTI_REAL price[8];
        //BTI_SINT nprice[8];
        BTI_BYTE accepted;
    } *ret_;

    do_text_set(alias, document_order_key4.sklad, store);
    document_order_key4.date = 0;
    document_order_key4.time = 0;

    do_cpy(key.sklad, document_order_key4.sklad);
    do_date_set(&key.date, tm1);

    do_extended_t *extended;
    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
    do_extended_set_key(extended, 4, &document_order_key4);

    do_extended_set_filter(extended, TRUE, 2, sizeof(key.sklad), sizeof(key.date));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &key.sklad);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(document_order_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &key.date);
    do_extended_set_fields(extended, 5,
                do_size_and_offset(document_order_struct_t, dtype),
                do_size_and_offset(document_order_struct_t, code),
                do_size_and_offset(document_order_struct_t, quant),
                do_size_and_offset(document_order_struct_t, nquant),
                //do_size_and_offset(document_order_struct_t, price),
                //do_size_and_offset(document_order_struct_t, nprice),
                do_size_and_offset(document_order_struct_t, accepted));

    BTI_CHAR in[2],out[2];
    do_text_set(alias, in, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_text_set(alias, out, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_sort_list_t *stocks;
    stocks = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)cmp1, NULL);
    do_ret_list_t *list = NULL;
    stock_t stock;
    while ( ((list = do_extended_get(extended, 1000)) != NULL) &&
            do_ret_list_count(list) ) {
        int indx;
        for ( indx = 0; indx < do_ret_list_count(list); indx++ ) {
            ret_ = do_ret_list_item_data(list, indx);
            if ( ret_->accepted && ((ret_->dtype[0] == in[0] && ret_->dtype[1] == in[1]) || (ret_->dtype[0] == out[0] && ret_->dtype[1] == out[1])) ) {

                do_cpy(stock.code, ret_->code);
                stock.quant = 0;
               // stock.sum = 0;
                for ( i = 0; i < 4; i++ )
                    if ( ret_->nquant[i] == DO_CONST_QUANT_REST ) {
                        stock.quant = do_pdecimal(ret_->quant[i], 7, 3);
                        break;
                    }
                if ( ret_->dtype[0] == out[0] && ret_->dtype[1] == out[1] )
                    stock.quant=-stock.quant;
                do_cpy(stock.code, ret_->code);
                if ( !do_sort_list_find(stocks, &i, &stock) )
                    do_sort_list_insert_new(stocks, i, &stock, sizeof(stock));
                else
                    ((stock_t*)stocks->list[i])->quant += stock.quant;
             }
         }
    }
    do_ret_list_free(list);
    do_extended_free(extended);
    PyObject *res;
    res = PyList_New(0);
    for ( i = 0; i < stocks->count; i++) {
        stock_t *rec;
        char *code;
        PyObject *result;
        rec = stocks->list[i];
        code = do_text(alias, rec->code);

        result = PyTuple_New(2);
        PyTuple_SET_ITEM(result, 0, MyString_FromString(code));
        PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(rec->quant));
        do_free(code);
        PyList_Append(res, result);

    }

    do_sort_list_free(stocks);
    return res;
}
static PyObject *Traffic_quarantine(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"store","date", NULL};
    PyObject *date = NULL;
    char *store = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO", kwlist, &store, &date) ) {
        do_log(LOG_ERR, "no params \"store\",\"date\"");
        return NULL;
    }
    struct tm tm1 = {0,};

    if ( !store ) {
         do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
        return NULL;
    }
    if ( !PyDateTime_Check(date) && !PyDate_Check(date) ) {
         do_log(LOG_ERR, "Invalid argument \"date\": wrong type");
         return NULL;
    }
    int year, month, day;
    year = PyDateTime_GET_YEAR(date);
    month = PyDateTime_GET_MONTH(date);
    day = PyDateTime_GET_DAY(date);
    do_date_set_ymd(&tm1, year, month, day);

    document_order_key4_t document_order_key4, key;
    do_alias_t *alias = self->priv->alias;
    int i;

    struct PACK_STRUCT {
        BTI_CHAR dtype[2];
        BTI_CHAR dtype1[10];
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        //BTI_REAL price[8];
        //BTI_SINT nprice[8];
        BTI_BYTE accepted;
    } *ret_;

    do_text_set(alias, document_order_key4.sklad, store);
    document_order_key4.date = 0;
    document_order_key4.time = 0;

    do_cpy(key.sklad, document_order_key4.sklad);
    do_date_set(&key.date, tm1);

    do_extended_t *extended;
    extended = do_extended_new(alias);
    do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
    do_extended_set_key(extended, 4, &document_order_key4);

    do_extended_set_filter(extended, TRUE, 2, sizeof(key.sklad), sizeof(key.date));
    do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &key.sklad);
    do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_INTEGER,
                                offsetof(document_order_struct_t, date),
                                DO_EXTENDED_COMP_LE,
                                DO_EXTENDED_CONNECTION_NO,
                                &key.date);
    do_extended_set_fields(extended, 6,
                do_size_and_offset(document_order_struct_t, dtype),
                do_size_and_offset(document_order_struct_t, dtype1),
                do_size_and_offset(document_order_struct_t, code),
                do_size_and_offset(document_order_struct_t, quant),
                do_size_and_offset(document_order_struct_t, nquant),
                //do_size_and_offset(document_order_struct_t, price),
                //do_size_and_offset(document_order_struct_t, nprice),
                do_size_and_offset(document_order_struct_t, accepted));

    BTI_CHAR in[2],out[2];
    do_text_set(alias, in, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
    do_text_set(alias, out, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_sort_list_t *stocks;
    stocks = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)cmp1, NULL);
    do_ret_list_t *list = NULL;
    stock_t stock;
    while ( ((list = do_extended_get(extended, 1000)) != NULL) &&
            do_ret_list_count(list) ) {
        int indx;
        for ( indx = 0; indx < do_ret_list_count(list); indx++ ) {
            ret_ = do_ret_list_item_data(list, indx);
            if ( ret_->accepted && ((ret_->dtype[0] == in[0] && ret_->dtype[1] == in[1]) || (ret_->dtype[0] == out[0] && ret_->dtype[1] == out[1]))
                && ret_->dtype1[0] == '6' && ret_->dtype1[1] == '6' ) {

                do_cpy(stock.code, ret_->code);
                stock.quant = 0;
                //stock.sum = 0;
                for ( i = 0; i < 4; i++ )
                    if ( ret_->nquant[i] == 3 ) {
                        stock.quant = do_pdecimal(ret_->quant[i], 7, 3);
                        break;
                    }
                if ( ret_->dtype[0] == out[0] && ret_->dtype[1] == out[1] )
                    stock.quant=-stock.quant;
                do_cpy(stock.code, ret_->code);
                if ( !do_sort_list_find(stocks, &i, &stock) )
                    do_sort_list_insert_new(stocks, i, &stock, sizeof(stock));
                else
                    ((stock_t*)stocks->list[i])->quant += stock.quant;
             }
         }
    }
    do_ret_list_free(list);
    do_extended_free(extended);
    PyObject *res;
    res = PyList_New(0);
    for ( i = 0; i < stocks->count; i++) {
        stock_t *rec;
        char *code;
        PyObject *result;
        rec = stocks->list[i];
        code = do_text(alias, rec->code);

        result = PyTuple_New(2);
        PyTuple_SET_ITEM(result, 0, MyString_FromString(code));
        PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(rec->quant));
        do_free(code);
        PyList_Append(res, result);

    }

    do_sort_list_free(stocks);
    return res;
}
static int quant_equal(double s1, double s2)
{
    return s1 < s2 + 0.0001 && s1 > s2 - 0.0001;
}
static PyObject *Traffic_rebuild_stocks(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"store","messages", NULL};
    PyObject *message = NULL;

    char *store = NULL;
    int show_message = 0;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|O", kwlist, &store, &message) ) {
        do_log(LOG_ERR, "no params \"store\"");
        return NULL;
    }
    show_message = PyObject_IsTrue(message);

    if ( !store ) {
         do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
        return NULL;
    }


    document_order_key0_t document_order_key0;
    do_alias_t *alias = self->priv->alias;
    int i,j;
    int indx;

    struct PACK_STRUCT {
        BTI_CHAR dtype1[10];
        BTI_CHAR code[DOMINO_FIELD_CODE_SIZE];
        BTI_BYTE quant[4][7];
        BTI_SINT nquant[4];
        //BTI_REAL price[8];
        //BTI_SINT nprice[8];
        BTI_BYTE accepted;
    } *ret_;

    do_extended_t *extended;
    char *value;
    do_sort_list_t *stocks;
    stocks = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)cmp1, NULL);
    for ( i = 0; i < 3; i++ ) {
        switch (i) {
        case 0:
             do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_IN));
             break;
        case 1:
             do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
             break;
        default:
             do_text_set(alias, document_order_key0.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_CRNTSALE));
             break;
        };
        if ( show_message ) {
             value = do_text(alias, document_order_key0.dtype);
             do_log(LOG_INFO, "%s", value);
             do_free(value);
        }
        extended = do_extended_new(alias);
        do_extended_set_file_db(extended, DO_DB_DOCUMENT_ORDER);
        do_text_set(alias, document_order_key0.sklad, store);
        do_text_set_empty(document_order_key0.document);
        document_order_key0.line = 0;
        do_extended_set_key(extended, 0, &document_order_key0);

        do_extended_set_filter(extended, TRUE, 2, sizeof(document_order_key0.dtype), sizeof(document_order_key0.sklad));
        do_extended_set_filter_term(extended, 0,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, dtype),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_AND,
                                &document_order_key0.dtype);
        do_extended_set_filter_term(extended, 1,
                                DO_EXTENDED_TYPE_STRING,
                                offsetof(document_order_struct_t, sklad),
                                DO_EXTENDED_COMP_EQUAL,
                                DO_EXTENDED_CONNECTION_NO,
                                &document_order_key0.sklad);
        do_extended_set_fields(extended, 5,
                do_size_and_offset(document_order_struct_t, dtype1),
                do_size_and_offset(document_order_struct_t, code),
                do_size_and_offset(document_order_struct_t, quant),
                do_size_and_offset(document_order_struct_t, nquant),
                //do_size_and_offset(document_order_struct_t, price),
                //do_size_and_offset(document_order_struct_t, nprice),
                do_size_and_offset(document_order_struct_t, accepted));

        do_ret_list_t *list = NULL;
        stock_t stock;
        while ( ((list = do_extended_get(extended, 1000)) != NULL) &&
            do_ret_list_count(list) ) {
            for ( indx = 0; indx < do_ret_list_count(list); indx++ ) {
                ret_ = do_ret_list_item_data(list, indx);
                if ( ret_->accepted ) {

                    do_cpy(stock.code, ret_->code);
                    stock.quant = 0;
                    stock.quant2 = 0;
                    stock.quant3 = 0;
                    for ( j = 0; j < 4; j++ ) {
                        if ( ret_->nquant[j] == 1 )
                            stock.quant = do_pdecimal(ret_->quant[j], 7, 3);
                        if ( ret_->nquant[j] == 2 && i == 2 )
                            stock.quant2 = -do_pdecimal(ret_->quant[j], 7, 3);
                        if ( ret_->nquant[j] == 3 && ret_->dtype1[0] == '6' && ret_->dtype1[1] == '6' && i != 2 )
                            stock.quant3 = -do_pdecimal(ret_->quant[j], 7, 3);
                    }
                    if ( i == 1 ) {
                        stock.quant=-stock.quant;
                        stock.quant3=-stock.quant3;
                    }
                    do_cpy(stock.code, ret_->code);
                    if ( !do_sort_list_find(stocks, &j, &stock) )
                        do_sort_list_insert_new(stocks, j, &stock, sizeof(stock));
                    else {
                        ((stock_t*)stocks->list[j])->quant += stock.quant;
                        ((stock_t*)stocks->list[j])->quant2 += stock.quant2;
                        ((stock_t*)stocks->list[j])->quant3 += stock.quant3;
                    }
                }
            }
        }
        do_ret_list_free(list);
        do_extended_free(extended);
    }
    stock_rec_t stock;
    stock_t rec, *rec_;
    stock_key0_t stock_key0, key;
    do_text_set(alias, stock_key0.sklad, store);
    do_text_set_empty(stock_key0.code);
    do_cpy(key.sklad,stock_key0.sklad);
    int ret;
    ret = do_stock_get0(alias, &stock, &stock_key0, DO_GET_GE);
    while ( ret == DO_OK ) {
        if ( do_cmp(key.sklad, stock_key0.sklad) ) break;
        do_cpy(rec.code, stock.data.code);
        if ( !do_sort_list_find(stocks, &indx, &rec) ) {
            if ( do_stock_quant(alias, &stock, 1) || do_stock_quant(alias, &stock, 2) || do_stock_quant(alias, &stock, 3) ) {
                   if ( show_message ) {
                      value = do_text(alias, stock.data.code);
                      do_log(LOG_INFO, "stock %s %.3f %.3f %.3f", value, do_stock_quant(alias, &stock, 1),do_stock_quant(alias, &stock, 2),do_stock_quant(alias, &stock, 3));
                   }
                   do_stock_quant_set(alias, &stock, 1, 0);
                   do_stock_quant_set(alias, &stock, 2, 0);
                   do_stock_quant_set(alias, &stock, 3, 0);
                   do_stock_update(alias, &stock);
            }
        }
        else {
            rec_= stocks->list[indx];
            if ( !quant_equal(rec_->quant,do_stock_quant(alias, &stock, 1)) || !quant_equal(rec_->quant2, do_stock_quant(alias, &stock, 2)) || !quant_equal(rec_->quant3,do_stock_quant(alias, &stock, 3)) ) {
                   if ( show_message ) {
                      value = do_text(alias, stock.data.code);
                      do_log(LOG_INFO, "stock %s %.3f %.3f %.3f %.3f %.3f %.3f", value, do_stock_quant(alias, &stock, 1),rec_->quant,do_stock_quant(alias, &stock, 2),rec_->quant2, do_stock_quant(alias, &stock, 3), rec_->quant3);
                   }
                   do_stock_quant_set(alias, &stock, 1, rec_->quant);
                   do_stock_quant_set(alias, &stock, 2, rec_->quant2);
                   do_stock_quant_set(alias, &stock, 3, rec_->quant3);
                   do_stock_update(alias, &stock);
            }
        }
        ret = do_stock_get0(alias, &stock, &stock_key0, DO_GET_NEXT);
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static char *MonthShort[12] =
{"Янв","Фев","Мар","Апр","Май","Июн","Июл","Авг","Сен","Окт","Ноя","Дек"
};
static char *MonthFull[12] =
{"Январь","Февраль","Март","Апрель","Май","Июнь","Июль","Август","Сентябрь","Октябрь","Ноябрь","Декабрь"
};
static char *do_rest_format(double value)
{
    if ( value > -0.0001 && value < 0.0001)
        return strdup("");
    char buf[500], *ch;
    sprintf(buf, "%.3f", value);
    ch = buf + strlen(buf) - 1;
    while (ch > buf && (*ch == '0' || *ch == ',' || *ch == '.')) {
        if (*ch == ',' || *ch == '.') {
            *ch = '\0';
            break;
        }
        *ch = '\0', ch--;
    }
    return do_strdup(buf);
}
#define RU_MONEY "р"
static char *do_money_format(double value, int abr)
{
    if ( value < 0.004 && value > -0.004 )
        return strdup("");

    char *buf, *ch, *retval, *ch1;
    if (abr)
        buf = do_strdup_printf("%.2f" RU_MONEY, value);
    else
        buf = do_strdup_printf("%.2f", value);
    int len = strlen(buf);
    int sign = (value > 0) ? 0 : 1;
    int len_man = len - ((abr) ? strlen(RU_MONEY) : 0) - 3 - sign;
    retval = do_malloc0(len + 10 + (len_man % 3 + 1));
    ch = buf;
    ch1 = retval;
    while (*ch != '\0') {
        if (!((len_man - (ch - buf - sign)) % 3) &&
            (ch > buf + sign) &&
            (len_man - (ch - buf - sign) > 0))
            *ch1 = ' ',ch1++;
        *ch1 = *ch++, ch1++;
    }
    do_free(buf);
    return retval;
}

static char* get_ads(do_alias_t *alias, const char *code)
{
    product_rec_t product;
    product_key0_t product_key0;
    /*document_rec_t document;
    document_key0_t document_key0;*/
    //do_string_list_t *keys;
    char *value;
    //int i;

    product_key0.base_parcel = 0;
    do_text_set(alias, product_key0.code, code);

    if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK )
        return NULL;
    value = do_product_param(alias, &product, "MR");
    if ( !value[0] ) {
        do_free(value);
        return NULL;
    }
    return value;
}
static char *get_store_category(do_alias_t *alias, const char *store)
{
    sklad_key0_t sklad_key0;
    sklad_rec_t sklad;
    partner_key0_t partner_key0;
    partner_rec_t partner;

    do_text_set(alias, sklad_key0.code, store);
    if ( do_sklad_get0(alias, &sklad, &sklad_key0, DO_GET_EQUAL) != DO_OK )
        return do_strdup("");
    do_cpy(partner_key0.g_code, sklad.data.p_g_code);
    partner_key0.code = sklad.data.p_code;
    if ( do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL) != DO_OK )
        return do_strdup("");
    return do_partner_param(alias, &partner, "CATEGORY");
}
static char *get_contract(do_alias_t *alias, const char *code, const char *store, int *min_rest)
{
    document_key0_t document_key0;
    document_order_key2_t document_order_key2, key;
    document_order_rec_t document_order;
    document_rec_t document;
    partner_key0_t partner_key0;
    partner_rec_t partner;
    int status;
    char *val_string = do_strdup("");

    do_text_set(alias, document_key0.dtype, "LT");
    do_text_set(alias, document_key0.sklad, "ТБ");
    do_text_set_empty(document_key0.document);
    do_cpy(key.dtype, document_key0.dtype);
    do_cpy(key.sklad, document_key0.sklad);
    *min_rest = 0;

    status = do_document_key0(alias, &document_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( do_cmp(key.dtype, document_key0.dtype) ||
             do_cmp(key.sklad, document_key0.sklad) ) break;
        do_cpy(document_order_key2.dtype, document_key0.dtype);
        do_cpy(document_order_key2.sklad, document_key0.sklad);
        do_cpy(document_order_key2.document, document_key0.document);
        do_text_set(alias, document_order_key2.code, code);
        if ( do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL) == DO_OK ) {
            if ( do_document_get0(alias, &document, &document_key0, DO_GET_EQUAL) == DO_OK ) {
                char *val1, *val2;
                val1 = do_document_param(alias, &document, "452");
                if ( strcmp(val1, "1") ) {
                    do_free(val1);
                    int quant;

                    do_cpy(partner_key0.g_code, document.data.p_g_code);
                    partner_key0.code = document.data.p_code;
                    do_partner_get0(alias, &partner, &partner_key0, DO_GET_EQUAL);
                    val1 = do_text(alias, partner.data.name);
                    val2 = do_document_param(alias, &document, "37");
                    char *buf;
                    buf = val_string;
                    if ( buf[0] )
                        val_string = do_strdup_printf("%s\n%s %s", buf, val1, val2);
                    else
                        val_string = do_strdup_printf("%s %s", val1, val2);
                    do_free(buf);
                    do_free(val1);
                    do_free(val2);
                    val1 = do_strdup_printf("A%s", store);
                    quant = do_document_order_param_int(alias, &document_order, val1);
                    do_free(val1);
                    if ( quant <= 0 ) {
                        char *category;
                        category = get_store_category(alias, store);
                        val1 = do_strdup_printf("QUANT_%s", category);
                        quant = do_document_order_param_int(alias, &document_order, val1);
                        do_free(val1);
                    }
                    *min_rest = quant;
                    break;
                }
                else
                    do_free(val1);
            }
        }
        status = do_document_key0(alias, &document_key0, DO_GET_NEXT);
    }
    return val_string;
}
static PyObject *Traffic_order(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"store","date1","date2", "kind", NULL};
    PyObject *date1 = NULL;
    PyObject *date2 = NULL;
    char *store = NULL;
    int  kind = 0;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sOOi", kwlist, &store, &date1, &date2, &kind) ) {
        do_log(LOG_ERR, "no params \"store\",\"date1\",\"date2\",\"kind\"");
        return NULL;
    }
    if ( !store ) {
         do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
        return NULL;
    }
    if ( !PyDateTime_Check(date1) && !PyDate_Check(date1) ) {
         do_log(LOG_ERR, "Invalid argument \"date1\": wrong type");
         return NULL;
    }
    if ( !PyDateTime_Check(date2) && !PyDate_Check(date2) ) {
         do_log(LOG_ERR, "Invalid argument \"date2\": wrong type");
         return NULL;
    }
    if ( !kind ) {
         do_log(LOG_ERR, "Invalid argument \"kind\": wrong type");
        return NULL;
    }

    int day, month, year;
    struct tm tm1, tm_;
    struct tm tm2;
    do_alias_t *alias;
    do_list_t *list;

    alias = self->priv->alias;
    if ( !alias )
        return FALSE;

    year = PyDateTime_GET_YEAR(date1);
    month = PyDateTime_GET_MONTH(date1);
    day = PyDateTime_GET_DAY(date1);
    do_date_set_ymd(&tm1, year, month, day);

    year = PyDateTime_GET_YEAR(date2);
    month = PyDateTime_GET_MONTH(date2);
    day = PyDateTime_GET_DAY(date2);
    do_date_set_ymd(&tm2, year, month, day);

    list = domino_order_get_marked_list(alias, store, &tm1, &tm2, TRUE);

    if ( !list )
        return NULL;

    int retval = TRUE;
    int count;
    count = 0;
    int inx;

    PyObject *result = PyList_New(0);

    for ( inx = 0; inx < list->count; inx++ ) {

        domino_order_marked_list_item_t *item;


        item = list->list[inx];
        if ( item->marked == DOMINO_MARKED_UNMARK )
            continue;


        if ( kind == 1 ) {
            if ( item->marked != DOMINO_MARKED )
                continue;
        }
        else
        if ( kind == 2 ) {
            if ( item->marked != DOMINO_MARKED_AND_REJECT )
                continue;
        }
        else
        if ( kind == 3 ) {
            if ( (do_date_cmp(&item->last_tm, &tm_) < 0) ||
                 (do_date_cmp(&item->last_tm, &tm2) > 0) ) continue;

            if ( item->marked != DOMINO_MARKED_AND_ACCEPT)
                continue;
        }
        else
        if ( kind == 4 ) {
            if ( item->marked != DOMINO_MARKED || item->quant < 0.1 )
                continue;
        }
        if ( domino_order_after_traffic_in(alias, store, item->code, &item->tm) )
            continue;
        count++;

        PyObject *r =  PyTuple_New(2);
        PyList_Append(result, r);

        PyTuple_SET_ITEM(r, 0, MyString_FromString(item->code));
        PyTuple_SET_ITEM(r, 1, get_product_info(alias, item->code, store));

    }
    domino_order_marked_list_free(list);
    if ( !retval )
        return NULL;
    return result;
}
typedef struct {
   char code[6];
   double quant;
}
realization_t;

static inline int real_cmp(realization_t *r1, realization_t *r2)
{
    return strcmp(r1->code, r2->code);
}

static do_sort_list_t *get_real(do_alias_t *alias, struct tm *tm1, struct tm *tm2, const char *store)
{
    do_sort_list_t *realization;
    realization = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)real_cmp, NULL);
    realization_t rec;
    document_key3_t document_key3, key;
    document_rec_t document;
    document_order_rec_t document_order;
    document_order_key0_t document_order_key0;
    int status, index;

    do_text_set(alias, document_key3.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
    do_text_set(alias, document_key3.sklad, store);
    do_date_set(&document_key3.date, *tm1);
    document_key3.time = 0;
    do_cpy(key.dtype, document_key3.dtype);
    do_cpy(key.sklad, document_key3.sklad);
    do_date_set(&key.date, *tm2);

    rec.code[5] = '\0';

    status = do_document_get3(alias, &document, &document_key3, DO_GET_GE);
    while ( status ) {
        if ( key.date < document_key3.date ||
             do_cmp(key.sklad, document_key3.sklad) ||
             do_cmp(key.dtype, document_key3.dtype) ) break;

        if ( document.data.dtype1[0] == '0' && document.data.dtype1[1]>='1' && document.data.dtype1[1]<='4') {
            do_cpy(document_order_key0.dtype, document.data.dtype);
            do_cpy(document_order_key0.sklad, document.data.sklad);
            do_cpy(document_order_key0.document, document.data.document);
            document_order_key0.line = 0;

            status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_GE);
            while ( status ) {
                if (
                     do_cmp(document_order_key0.document, document.data.document) ||
                     do_cmp(document_order_key0.sklad, document.data.sklad) ||
                     do_cmp(document_order_key0.dtype, document.data.dtype)
                ) break;

                if ( document_order.data.accepted ) {
                    strncpy(rec.code, document_order.data.code, 5);
                    if ( do_sort_list_find(realization, &index, &rec) ) {
                        ((realization_t*)realization->list[index])->quant +=
                        do_document_order_quant(alias, &document_order, 1);
                    }
                    else {
                        rec.quant = do_document_order_quant(alias, &document_order, 1);
                        do_sort_list_insert_new(realization, index, &rec, sizeof(rec));
                    }

                }

                status = do_document_order_get0(alias, &document_order, &document_order_key0, DO_GET_NEXT);
            }
            if ( status == DO_ERROR )
                break;
        }
        status = do_document_get3(alias, &document, &document_key3, DO_GET_NEXT);
    }

    if ( status == DO_ERROR ) {
        do_sort_list_free(realization);
        return NULL;
    }
    return realization;

}
static do_sort_list_t *get_stock(do_alias_t *alias, struct tm *tm1, struct tm *tm2, const char *store)
{

    do_sort_list_t *stocks, *all;
    do_sort_list_t *retval;

	realization_rec_t realization;
	realization_key4_t realization_key4, key;

    stocks = do_report_get_rest(alias, store);
    if ( !stocks )
        return NULL;
    all = do_report_get_rest(alias, NULL);
    if ( !all )
        return NULL;

    retval = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)real_cmp, NULL);
    int i;
    for ( i = 0; i < stocks->count; i++ ) {
        do_rest_t *rest;
        rest = stocks->list[i];
        if ( rest->quant[0] < 0.001 ) {
            realization_t crnt;
            crnt.code[5] = '\0';
            int indx;
            if ( !do_sort_list_find(all, &indx, rest) ) {
                fprintf(stderr, "Error !!\n");
                break;
            }
            if ( ((do_rest_t*)all->list[indx])->quant[0] > 0 ) {

                struct tm tm;
                time_t now;
                now = time(NULL);
                tm = *localtime(&now);

                do_text_set(alias, realization_key4.sklad, store);
                do_text_set(alias, realization_key4.code, rest->code);
                do_cpy(key.sklad, realization_key4.sklad);
                do_cpy(key.code, realization_key4.code);

                realization_key4.month = do_datetosql(tm);
                tm.tm_year--;
                mktime(&tm);
                key.month = do_datetosql(tm);

                if ( ( do_realization_get4(alias, &realization, &realization_key4, DO_GET_LE) == DO_KEY_NOT_FOUND) ||
                     do_cmp(key.sklad, realization_key4.sklad) ||
                     do_cmp(key.code, realization_key4.code) ||
                     key.month > realization_key4.month
                    )  {

                    strncpy(crnt.code, rest->code, 5);
                    crnt.quant = rest->quant[0];
                    do_sort_list_add_alloc(retval, &crnt, sizeof(crnt));
                }
            }
        }
    }
    do_rest_list_free(stocks);
    return retval;
}

static do_sort_list_t *get_stock1(do_alias_t *alias, struct tm *tm1, struct tm *tm2, const char *store)
{

    do_sort_list_t *stocks;
    do_sort_list_t *retval;

	realization_rec_t realization;
	realization_key4_t realization_key4, key;

    stocks = do_report_get_rest(alias, store);
    if ( !stocks )
        return NULL;

    retval = do_sort_list_new(TRUE, TRUE, (do_list_cmp_func)real_cmp, NULL);
    int i;
    for ( i = 0; i < stocks->count; i++ ) {
        do_rest_t *rest;
        rest = stocks->list[i];
        if ( rest->quant[0] < 0.001 ) {

            struct tm tm;
            time_t now;
            now = time(NULL);
            tm = *localtime(&now);

            do_text_set(alias, realization_key4.sklad, store);
            do_text_set(alias, realization_key4.code, rest->code);
            do_cpy(key.sklad, realization_key4.sklad);
            do_cpy(key.code, realization_key4.code);

            realization_key4.month = do_datetosql(tm);
            tm.tm_year--;
            mktime(&tm);
            key.month = do_datetosql(tm);

            if ( do_realization_get4(alias, &realization, &realization_key4, DO_GET_LE) == DO_OK )
                continue;
            if ( do_cmp(key.sklad, realization_key4.sklad) ||
                 do_cmp(key.code, realization_key4.code) ||
                 key.month > realization_key4.month
                )  continue;

            realization_t crnt;
            crnt.code[5] = '\0';
            strncpy(crnt.code, rest->code, 5);
            crnt.quant = 0;
            do_sort_list_add_alloc(retval, &crnt, sizeof(crnt));
        }
    }
    do_rest_list_free(stocks);
    return retval;
}
static PyObject *Traffic_limit(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"store","date1","date2", "kind", NULL};
    PyObject *date1 = NULL;
    PyObject *date2 = NULL;
    char *store = NULL;
    int  kind = 0;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sOOi", kwlist, &store, &date1, &date2, &kind) ) {
        do_log(LOG_ERR, "no params \"store\",\"date1\",\"date2\",\"kind\"");
        return NULL;
    }
    if ( !store ) {
         do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
        return NULL;
    }
    if ( !PyDateTime_Check(date1) && !PyDate_Check(date1) ) {
         do_log(LOG_ERR, "Invalid argument \"date1\": wrong type");
         return NULL;
    }
    if ( !PyDateTime_Check(date2) && !PyDate_Check(date2) ) {
         do_log(LOG_ERR, "Invalid argument \"date2\": wrong type");
         return NULL;
    }
    if ( !kind ) {
         do_log(LOG_ERR, "Invalid argument \"kind\": wrong type");
        return NULL;
    }

    int day, month, year;
    struct tm tm1;
    struct tm tm2;
    do_alias_t *alias;

    alias = self->priv->alias;
    if ( !alias )
        return FALSE;

    year = PyDateTime_GET_YEAR(date1);
    month = PyDateTime_GET_MONTH(date1);
    day = PyDateTime_GET_DAY(date1);
    do_date_set_ymd(&tm1, year, month, day);

    year = PyDateTime_GET_YEAR(date2);
    month = PyDateTime_GET_MONTH(date2);
    day = PyDateTime_GET_DAY(date2);
    do_date_set_ymd(&tm2, year, month, day);


    int skip_ordered;
    //gboolean skip_def;
    int retval = TRUE;

    do_sort_list_t *realization;
    int count =0;

    skip_ordered = TRUE;

    switch ( kind ) {
        case 1:
            realization = get_real(alias, &tm1, &tm2, store);
            break;
        case 2:
            realization = get_stock1(alias, &tm1, &tm2, store);
            break;
        case 3:
            realization = get_stock(alias, &tm1, &tm2, store);
            break;
        default:
            do_log(LOG_ERR, "Invalid argument \"kind\"");
            return NULL;
    }

    if ( !realization )
        return NULL;
    struct tm tm1_;
    struct tm tm2_;
    struct tm tm_mark;
    time_t now;
    char *str;

    now = time(NULL);
    tm1_ = *localtime(&now);
    tm2_ = tm1_;
    tm1_.tm_mday-= DOMINO_MARKED_INTERVAL;
    mktime(&tm1_);


    int inx;

    PyObject *result = PyList_New(0);



    for ( inx = 0; inx < realization->count; inx++ ) {
        realization_t *rec;
        rec = realization->list[inx];
        product_key0_t product_key0;
        product_rec_t product;
        domino_marked_t marked;
        double rest;

        //if ( strcmp(rec->code, "19179" ) ) continue;//


        if ( !domino_order_get_marked(alias, store, rec->code, &tm1_, &tm2_, &marked, &tm_mark) ) {
            retval = FALSE;
            break;
        }


        if ( skip_ordered &&
            ( marked == DOMINO_MARKED_AND_ACCEPT || marked == DOMINO_MARKED_AND_REJECT) ) {
            continue;
        }

        /*if ( skip_def && marked == DOMINO_MARKED_AND_REJECT )
            continue;*/

        if ( kind == 1 ) {

            rest = do_get_rest(alias, rec->code, store);

            if ( rest >= rec->quant ) {
                continue;
            }
        }
        else
            rest = rec->quant;

        count++;

        PyObject *r;
        PyObject *info;

        info = get_product_info(alias, rec->code, store);
        r =  PyTuple_New(2);
        PyList_Append(result, r);

        PyTuple_SET_ITEM(r, 0, MyString_FromString(rec->code));

        PyTuple_SET_ITEM(r, 1, info);

        product_key0.base_parcel = 0;
        do_text_set(alias, product_key0.code, rec->code);
        if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
            do_log(LOG_ERR, "Can't get product \"%s\"", rec->code);
            retval = FALSE;
            break;
        }
        r =  PyTuple_New(2);
        PyList_Insert(info, 0, r);
        PyTuple_SET_ITEM(r, 0, MyString_FromString("Продано"));
        str = do_rest_format(rec->quant/do_product_coef(alias, &product));
        PyTuple_SET_ITEM(r, 1, MyString_FromString(str));
        do_free(str);
    }
    if ( !retval )
        return NULL;
    return result;
}
static do_list_t *get_last_product_parcel_order(do_alias_t *alias, product_key0_t *key, const char *dtype, const char **operations, int operations_len, const char *store, struct tm *tm1, struct tm *tm2)
{
    int status,i;
    do_list_t *ret;
    //int number = 1;

    sklad_key0_t sklad_key0;
    document_order_key3_t document_order_key3;
    document_order_rec_t document_order;
    BTI_LONG date;

    ret = do_list_new(FALSE);
    do_cpy(document_order_key3.code, key->code);
    do_date_set(&document_order_key3.date, *tm2);
    document_order_key3.date += 1;
    document_order_key3.time = 0;
    do_date_set(&date, *tm1);
    do_text_set(alias, sklad_key0.code, store);

    status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_LT);
    while ( status == DO_OK ) {
        if ( do_cmp(document_order_key3.code, key->code) ||
             document_order_key3.date < date ) break;
        if ( !do_cmp(document_order.data.sklad, sklad_key0.code) ) {
            if ( !strncmp(document_order.data.dtype, dtype, 2) ) {
                for ( i = 0; i < operations_len; i++ ) {
                    if ( document_order.data.dtype1[0] == operations[i][0] &&
                        document_order.data.dtype1[1] == operations[i][1] ) {

                            do_list_add_alloc(ret, &document_order, sizeof(document_order)-(sizeof(document_order.data)-document_order.size)+10);
                            break;
                        }
                }
            }
        }
        status = do_document_order_get3(alias, &document_order, &document_order_key3, DO_GET_PREVIOUS);
    }
    return ret;
}

static PyObject *get_write_off(do_alias_t *alias, const char *code, const char *store, int days)
{
    struct tm tm1, tm;
    struct tm tm2;
    time_t now;
    const char *operations_off[] = {"14","15"};
    const char *operations_price[] = {"03"};
    const char *operations_move[] = {"06"};
    //char *str;
    int i;
    double coef;
    document_order_rec_t *document_order;
    now = time(NULL);
    tm2 = *localtime(&now);
    now -= days * 24 * 60 * 60;
    tm1 = *localtime(&now);

    product_key0_t product_key0;
    product_key4_t product_key4;
    product_rec_t product;
    char date[50];
    char dtype[3];
    int status;
    //do_sort_list_t *list;
    do_list_t *orders;
    PyObject *retval,*r;

    retval = PyList_New(0);

    do_text_set(alias, product_key0.code, code);
    product_key0.base_parcel = 0;

    if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
        do_log(LOG_ERR, "product \"%s\" not found", do_text(alias, product_key0.code));
        return retval;

    }

    coef = do_product_coef(alias, &product);

    do_text_set(alias, product_key4.code, code);
    snprintf(product_key4.code+5,4,".%2.2d", atoi(store));
    do_cpy(product_key0.code, product_key4.code);
    product_key0.base_parcel = 1;

    status = do_product_key0(alias, &product_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( strncmp(product_key0.code, product_key4.code, 8) ) break;

        do_text_set(alias, dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRODUCT_OUT));
        orders = get_last_product_parcel_order(alias, &product_key0, dtype, operations_off, 2, store, &tm1, &tm2);

        for ( i = 0; i < orders->count; i++ ) {
            document_order = orders->list[i];
            r = PyList_New(0);
            PyList_Append(retval, r);
            PyList_Append(r, MyString_FromString("Списание"));
            PyList_Append(r, PyFloat_FromDouble(do_document_order_quant(alias, document_order, 1)/coef));
            do_date(document_order->data.date, &tm);
            strftime(date, sizeof(date)-1, "%d/%m/%y", &tm);
            PyList_Append(r, MyString_FromString(do_strdup(date)));
        }
        orders = get_last_product_parcel_order(alias, &product_key0, dtype, operations_move, 1, store, &tm1, &tm2);

        for ( i = 0; i < orders->count; i++ ) {
            document_order = orders->list[i];
            r = PyList_New(0);
            PyList_Append(retval, r);
            PyList_Append(r, MyString_FromString("Перемещение"));
            PyList_Append(r, PyFloat_FromDouble(do_document_order_quant(alias, document_order, 1)/coef));
            do_date(document_order->data.date, &tm);
            strftime(date, sizeof(date)-1, "%d/%m/%y", &tm);
            PyList_Append(r, MyString_FromString(do_strdup(date)));
        }
        do_text_set(alias, dtype, do_param(DO_PARAM_DOCUMENT_TYPE_PRICE));
        orders = get_last_product_parcel_order(alias, &product_key0, dtype, operations_price, 1, store, &tm1, &tm2);

        for ( i = 0; i < orders->count; i++ ) {
            document_order = orders->list[i];
            r = PyList_New(0);
            PyList_Append(retval, r);
            PyList_Append(r, MyString_FromString("Уценка"));
            PyList_Append(r, PyFloat_FromDouble(do_document_order_price(alias, document_order, 4)*coef));
            do_date(document_order->data.date, &tm);
            strftime(date, sizeof(date)-1, "%d/%m/%y", &tm);
            PyList_Append(r, MyString_FromString(do_strdup(date)));
        }
        status = do_product_key0(alias, &product_key0, DO_GET_NEXT);
    }

    return retval;
}
static PyObject *get_product_info(do_alias_t *alias, const char *code, const char *store)
{

    product_key0_t product_key0;
    product_rec_t product;

    //PyObject *ret;
    PyObject *info;

    char *str;
    PyObject *purchase_;
    PyObject *real_;
    PyObject *season_;
    PyObject *values;
    PyObject *r;
    PyObject *items;
    PyObject *stock_items;
    int firm;
    int i;
    do_string_list_t *units;

    double rest;


    purchase_ = PyList_New(0);
    real_ = PyList_New(0);
    season_ = PyList_New(0);

    //ret =  PyTuple_New(2);

    //PyTuple_SET_ITEM(ret, 0, MyString_FromString(code));
    info = PyList_New(0);
    //PyTuple_SET_ITEM(ret, 1, info);
    product_key0.base_parcel = 0;
    do_text_set(alias, product_key0.code, code);
    if ( do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK ) {
        do_log(LOG_ERR, "Can't get product \"%s\"", code);
        return NULL;
    }

    if ( do_product_param_int(alias, &product, "DELETED") != 0 ) {

        r =  PyTuple_New(2);
        PyList_Append(info, r);
        PyTuple_SET_ITEM(r, 0, MyString_FromString("!"));
        PyTuple_SET_ITEM(r, 1, MyString_FromString("Не заказывать!"));
        return info;
    }

    purchase_t last[4];

    get_purchase(alias, &product, store, last);

    firm = do_firm_get_firm_by_unit(alias, store);
    units = do_firm_get_units(alias, firm);


    rest = do_get_rest(alias, code, store)/do_product_coef(alias, &product);



    realiz_t month[4], season[4];
    get_realiz(alias, &product, store, month, season);

    time_t now;
    int year;
    struct tm tm;
    now = time(NULL);
    tm = *localtime(&now);
    year = tm.tm_year;

    for ( i = 0; i < 4; i++ ) {
        if ( month[i].quant > 0 ) {
            values = PyList_New(0);
            PyList_Append(real_, values);
            if ( year == month[i].tm.tm_year )
                PyList_Append(values, MyString_FromString(MonthFull[month[i].tm.tm_mon]));
            else {
                str  = do_strdup_printf("%s %d",MonthShort[month[i].tm.tm_mon], month[i].tm.tm_year-100);
                PyList_Append(values, MyString_FromString(str));
                do_free(str);
            }
            if ( month[i].quant < 2 ) {
                str = do_rest_format(month[i].quant);
                PyList_Append(values, MyString_FromString(str));
                do_free(str);
            }
            else
                PyList_Append(values, PyLong_FromLong((int)month[i].quant));
        }
        if ( season[i].quant > 0 ) {

            values = PyList_New(0);
            PyList_Append(season_, values);
            PyList_Append(values, MyString_FromString(season_name(i)));
            if ( season[i].quant < 2 ) {
                str = do_rest_format(season[i].quant);
                PyList_Append(values, MyString_FromString(str));
                do_free(str);
            }
            else
                PyList_Append(values, PyLong_FromLong((int)season[i].quant));
        }
        if ( last[i].quant > 0 ) {
            char date[50];
            values = PyList_New(0);
            PyList_Append(purchase_, values);
            strftime(date, sizeof(date)-1, "%d/%m/%y", &last[i].tm);
            PyList_Append(values, MyString_FromString(do_strdup(date)));
            PyList_Append(values, PyLong_FromLong((int)last[i].quant));
            PyList_Append(values, MyString_FromString(last[i].name));
            str = do_money_format(last[i].price, TRUE);
            PyList_Append(values, MyString_FromString(str));
            do_free(str);
        }
    }

    int min_rest = 0, indx;
    char *ads_long, *ch;
    char *ads;
    char *contract;


    ads = do_product_param(alias, &product, "480");
    contract = get_contract(alias, code, store, &min_rest);

    ads_long = NULL;
    ch = strchr(ads, '-');
    if ( ch ) {
        *ch = '\0';
        ads_long = ch+1;
    }
    ch = strchr(ads, ',');
    while ( ch ) {
        *ch = '\n';
        ch = strchr(ads, ',');
    }
    r =  PyTuple_New(2);
    PyList_Append(info, r);
    PyTuple_SET_ITEM(r, 0, MyString_FromString("Остаток"));
    str = rest ? do_rest_format(rest) : do_strdup("0");
    stock_items = items = PyList_New(0);
    values = PyList_New(0);
    PyTuple_SET_ITEM(r, 1, items);
    PyList_Append(items, values);

    PyList_Append(values, MyString_FromString("текущий"));
    PyList_Append(values, MyString_FromString(str));
    do_free(str);

    if ( min_rest > 0 ) {
        values = PyList_New(0);
        PyList_Append(items, values);
        PyList_Append(values, MyString_FromString("минимальный"));
        PyList_Append(values, PyLong_FromLong(min_rest));
    }
    if ( min_rest > 0 && rest < min_rest ) {
        values = PyList_New(0);
        PyList_Append(items, values);
        str = do_rest_format(min_rest-((rest > 0) ? rest : 0));
        PyList_Append(values, MyString_FromString("нехватка"));
        PyList_Append(values, MyString_FromString(str));
        do_free(str);
    }
    values = get_write_off(alias, code, store, DAYS_WRITE_OFF);
    if ( values && PyList_GET_SIZE(values) ) {
        r =  PyTuple_New(2);
        PyList_Append(info, r);
        PyTuple_SET_ITEM(r, 0, MyString_FromString("Движение"));
        PyTuple_SET_ITEM(r, 1, values);
    }
    if ( ads && ads[0] ) {
        r =  PyTuple_New(2);
        PyList_Append(info, r);
        PyTuple_SET_ITEM(r, 0, MyString_FromString("Указания"));
        PyTuple_SET_ITEM(r, 1, MyString_FromString(ads));

    }

    r =  PyTuple_New(2);
    PyList_Append(info, r);
    PyTuple_SET_ITEM(r, 0, MyString_FromString("Закуп"));
    PyTuple_SET_ITEM(r, 1, purchase_);

    r =  PyTuple_New(2);
    PyList_Append(info, r);
    PyTuple_SET_ITEM(r, 0, MyString_FromString("Реализация"));
    PyTuple_SET_ITEM(r, 1, real_);

    r =  PyTuple_New(2);
    PyList_Append(info, r);
    PyTuple_SET_ITEM(r, 0, MyString_FromString("Сезоны"));
    PyTuple_SET_ITEM(r, 1, season_);

    if ( ads_long && ads_long[0] ) {
        r =  PyTuple_New(2);
        PyList_Append(info, r);
        PyTuple_SET_ITEM(r, 0, MyString_FromString("Указания"));
        PyTuple_SET_ITEM(r, 1, MyString_FromString(ads_long));

    }
    if ( ads ) do_free(ads);
    ads = get_ads(alias, code);
    if ( ads && ads[0] ) {
        r =  PyTuple_New(2);
        PyList_Append(info, r);
        PyTuple_SET_ITEM(r, 0, MyString_FromString("Реклама"));
        PyTuple_SET_ITEM(r, 1, MyString_FromString(ads));

    }
    if ( ads ) do_free(ads);

    if ( contract && contract[0] ) {
        r =  PyTuple_New(2);
        PyList_Append(info, r);
        PyTuple_SET_ITEM(r, 0, MyString_FromString("Контракт"));
        PyTuple_SET_ITEM(r, 1, MyString_FromString(contract));

    }
    if ( contract ) do_free(contract);

    char *shorts[1000];

    partner_key0_t partner_key0;
    partner_rec_t partner;
    int status;

    do_text_set(alias, partner_key0.g_code, "01");
    partner_key0.code = 0;

    for ( i = 0; i < 1000; i++ )
        shorts[i]=do_strdup_printf("А%d", i+1);

    status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_GE);
    while ( status == DO_OK ) {
        if ( partner_key0.g_code[0] != '0' || partner_key0.g_code[1] != '1' || partner_key0.g_code[2] != ' ') break;
        char *store;

        store = do_text(alias, partner.data.sklad);
        i = atoi(store);

        if ( ( i > 0 ) && ( i < 1000 ) ) {
            do_free(shorts[i-1]);
            shorts[i-1] = do_partner_param(alias, &partner, "SNAME");
        }
        status = do_partner_get0(alias, &partner, &partner_key0, DO_GET_NEXT);
    }

    r =  PyTuple_New(2);
    values = PyList_New(0);
    PyList_Append(info, r);
    PyTuple_SET_ITEM(r, 0, MyString_FromString("В организации"));
    PyTuple_SET_ITEM(r, 1, values);
    for ( i = 0; i < units->count; i++ )
        if ( strcmp(units->list[i], store) ) {
            double quant;
            char *val;
            quant = do_get_rest(alias, code, units->list[i])/do_product_coef(alias, &product);
            if ( quant > 0 ) {
                indx = atoi(units->list[i]);
                r =  PyTuple_New(2);
                PyList_Append(values, r);
                if ( indx > 0 && indx < 1000 )
                    val = do_strdup(shorts[indx-1]);
                else
                    val = do_strdup_printf("A%s", units->list[i]);
                PyTuple_SET_ITEM(r, 0, MyString_FromString(val));
                do_free(val);
                val = do_rest_format(quant);
                PyTuple_SET_ITEM(r, 1, MyString_FromString(val));
                do_free(val);
            }
        }
    for ( i = 0; i < 1000; i++ ) {
        do_free(shorts[i]);
    }

    do_list_t *order;
    struct tm tm1;
    struct tm tm2;
    BTI_LONG date1;
    now = time(NULL);
    tm2 = *localtime(&now);

    do_date_set(&date1, tm2);
    date1-=60;
    do_date(date1, &tm1);

    order = get_product_order(alias, &product_key0, store, &tm1, &tm2);

    if ( order->count > 0 ) {
        order_item_t *o;
        char *old="";
        r =  PyTuple_New(2);
        items = PyList_New(0);
        PyList_Append(info, r);
        PyTuple_SET_ITEM(r, 0, MyString_FromString("Заявки"));
        PyTuple_SET_ITEM(r, 1, items);
        for ( i = 0; i < order->count; i++ ) {
            o = order->list[i];
            //char date_[20];
            char *v;
            v = strchr(o->user, '-');
            if ( v ) {
                v[0] = '\0';
                v++;
            //strftime(date_, sizeof(date_)-1, "%d/%m/%y", &o->tm);
            }
            else {
                v = "";
            }
            if ( !i ) { 
                if ( last[0].quant > 0 && do_date_cmp(&last[0].tm,&o->tm) < 0 && o->marked == DOMINO_MARKED_AND_ACCEPT) {
                    values = PyList_New(0);
                    PyList_Append(stock_items, values);
                    PyList_Append(values, MyString_FromString("на заявке"));
                    PyList_Append(values, PyLong_FromLong((int)o->quant));
                    PyList_Append(values, MyString_FromString(o->user));
               }
            }
            values = PyList_New(0);
            PyList_Append(items, values);
            if ( !strcmp(o->user, old) ) {
                PyList_Append(values, MyString_FromString(""));
            }
            else {
                PyList_Append(values, MyString_FromString(o->user));
                old = o->user;
            }
            PyList_Append(values, o->quant ? PyLong_FromLong((int)o->quant) : MyString_FromString(""));
            PyList_Append(values, MyString_FromString(v));

            switch ( o->marked ) {
                case DOMINO_MARKED_UNMARK:
                    PyList_Append(values, MyString_FromString(""));
                    break;
                case DOMINO_MARKED:
                    PyList_Append(values, MyString_FromString("заявка"));
                    break;
                case DOMINO_MARKED_AND_ACCEPT:
                    PyList_Append(values, MyString_FromString("заказ"));
                    break;
                case DOMINO_MARKED_AND_REJECT:
                    PyList_Append(values, MyString_FromString("отказ"));
                    break;
            }
        }
        for ( i = 0; i < order->count; i++ ) {
            o = order->list[i];
            order_item_free(o);
        }
        do_list_free(order);
    }
    return info;
}

static do_list_t *get_product_order(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1, struct tm *tm2)
{

    do_list_t *ret;
    ret = do_list_new(TRUE);
    product_rec_t product;
    product_key0_t product_key0;
    double coef;
    BTI_LONG d1,d2;
    int status, i, count;

    do_cpy(product_key0, *key);

	if (do_product_get0(alias, &product, &product_key0, DO_GET_EQUAL) != DO_OK) {
        return ret;
	}
    document_order_key2_t document_order_key2, document_order_key;
    document_order_rec_t document_order;

    do_text_set(alias, document_order_key2.dtype, do_param(DO_PARAM_DOCUMENT_TYPE_ORDER1));
    do_text_set(alias, document_order_key2.sklad, store);

    memcpy(&document_order_key, &document_order_key2, sizeof(document_order_key2.dtype)+ sizeof(document_order_key2.sklad));

    coef = do_product_coef(alias, &product);

    do_date_set(&d1, *tm1);
    do_date_set(&d2, *tm2);
    struct tm tm;

    while ( d1 <= d2 ) {
        char *value;
        memcpy(&document_order_key2, &document_order_key, sizeof(document_order_key2.dtype)+ sizeof(document_order_key2.sklad));
        do_date(d2, &tm);
        value = do_strdup_printf("%4.4d%2.2d%2.2d",  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

        tm.tm_mday += 1;
        do_text_set(alias, document_order_key2.document, value);
        do_free(value);
        do_cpy(document_order_key2.code, product_key0.code);
        do_cpy(document_order_key, document_order_key2);

        status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_EQUAL);
        while (status == DO_OK) {
            order_item_t item;
            if (do_cmp(document_order_key, document_order_key2))
                break;
            //struct tm tm_mark;

            item.marked = document_order.data.accepted;
            /*switch ( marked ) {
                case DOMINO_MARKED_AND_ACCEPT:
                case DOMINO_MARKED_AND_REJECT:
                    do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), &tm_mark);
                    do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), &tm_mark);
                    break;
                default:
                    do_date(document_order.data.date, &tm_mark);
                    do_time(document_order.data.time, &tm_mark);
            }*/
            if ( do_document_order_param_int(alias, &document_order, "DATE_ORDER") > 0 ) {
                do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), &item.last_tm);
                do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), &item.last_tm);
                do_date(do_document_order_param_int(alias, &document_order, "DATE_ORDER"), &item.tm);
                do_time(do_document_order_param_int(alias, &document_order, "TIME_ORDER"), &item.tm);
            } 
            else {
                do_date(document_order.data.date, &item.last_tm);
                do_time(document_order.data.time, &item.last_tm);
                do_date(document_order.data.date, &item.tm);
                do_time(document_order.data.time, &item.tm);
            }
            item.quant = do_document_order_quant(alias, &document_order, DO_CONST_QUANT_REST)/coef;
            count = 0;
            for ( i = 0; i < 1000; i++ ) {
                char *param, *value;
                int empty;
                param = do_strdup_printf("PRO%3.3d", i);
                value = do_document_order_param(alias, &document_order, param);
                empty = value[0] == '\0';
                if ( !empty ) {
                    item.user = do_strdup(value);
                    do_list_add_alloc(ret, &item, sizeof(item));
                    count++;
                }
                do_free(param);do_free(value);
                if ( empty ) break;
            }
            if ( !count ) {
                char *c;
                c = do_datetoa(item.tm);
                item.user = do_strdup_printf("%s - автозаказ",c);
                do_free(c);
                do_list_add_alloc(ret, &item, sizeof(item));
            }
            status = do_document_order_get2(alias, &document_order, &document_order_key2, DO_GET_NEXT);
        }
        d2--;
	}
	return ret;
}


static PyObject *Traffic_info(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"code","store", NULL};
    char *code = NULL;
    char *store = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "ss", kwlist, &code, &store) ) {
        do_log(LOG_ERR, "no params \"code\", \"store\"");
        return NULL;
    }
    if ( !store ) {
         do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
        return NULL;
    }
    if ( !code ) {
         do_log(LOG_ERR, "Invalid argument \"code\": wrong type");
        return NULL;
    }

    return get_product_info(self->priv->alias, code, store);
}
#define ISLEAP(year) ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
static const unsigned short int __mon_yday[2][13] = {
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};
#define SECS_PER_HOUR (60 * 60)
static void day_of_the_week (struct tm *tm)
{
  int corr_year = 1900 + tm->tm_year - (tm->tm_mon < 2);
  int wday = (-473
      + (365 * (tm->tm_year - 70))
      + (corr_year / 4)
      - ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0)
      + (((corr_year / 4) / 25) / 4)
      + __mon_yday[0][tm->tm_mon]
       + tm->tm_mday - 1);
  tm->tm_wday = ((wday % 7) + 7) % 7;
}

static void day_of_the_year (struct tm *tm)
{
  tm->tm_yday = (__mon_yday[ISLEAP(1900 + tm->tm_year)][tm->tm_mon]
      + (tm->tm_mday - 1));
}


static PyObject *Traffic_set_marked(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"code","store", "datetime", "reject", "quant", "partner", "desc", "life_time", "manufactor", "price", "price_fact", NULL};
    char *code = NULL;
    char *store = NULL;
    PyObject *date = NULL;
    PyObject *reject = NULL;
    double quant = 0, price = 0, price_fact = 0;
    int partner = 0;
    char *desc = NULL;
    char *life_time = NULL;
    char *manufactor = NULL;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "ssOO|disssdd", kwlist, &code, &store, &date,
                                       &reject, &quant, &partner, &desc, &life_time, &manufactor,
                                       &price, &price_fact) ) {
        do_log(LOG_ERR, "no params \"code\", \"store\", \"datetime\", \"reject\"");
        return NULL;
    }
    if ( !store ) {
        do_log(LOG_ERR, "Invalid argument \"store\": wrong type");
        return NULL;
    }
    if ( !code ) {
        do_log(LOG_ERR, "Invalid argument \"code\": wrong type");
        return NULL;
    }
    if ( !date || !PyDateTime_Check(date) ) {
        do_log(LOG_ERR, "Invalid argument \"datetime\": wrong type");
        return NULL;
    }
    if ( !reject ) {
        do_log(LOG_ERR, "Invalid argument \"reject\": wrong type");
        return NULL;
    }
    int ret;
    struct tm tm1;
    struct tm tm2;

    int year, month, day, hour, minute, second;
    year = PyDateTime_GET_YEAR(date);
    month = PyDateTime_GET_MONTH(date);
    day = PyDateTime_GET_DAY(date);
    hour = PyDateTime_DATE_GET_HOUR(date);
    minute = PyDateTime_DATE_GET_MINUTE(date);
    second = PyDateTime_DATE_GET_SECOND(date);
    memset(&tm1, 0, sizeof(tm1));
    tm1.tm_mday = day;
    tm1.tm_mon = month - 1;
    tm1.tm_year = year - 1900;
    tm1.tm_hour = hour;
    tm1.tm_min = minute;
    tm1.tm_sec = second;
    day_of_the_week(&tm1);
    day_of_the_year(&tm1);

    tm2 = tm1;
    tm1.tm_mday-= DOMINO_MARKED_INTERVAL;
    mktime(&tm1);

    ret = domino_order_set_marked_full(self->priv->alias, store, code, &tm1, &tm2,
                                       PyObject_IsTrue(reject) ? DOMINO_MARKED_AND_REJECT : DOMINO_MARKED_AND_ACCEPT,
                                       quant, partner, desc, life_time, manufactor, price, price_fact);
    return PyBool_FromLong(ret);

}

//int domino_order_set_marked_full(do_alias_t *alias, const char *unit, const char *product_code, struct tm *tm1_, struct tm *tm2_, domino_marked_t marked, double quant, int partner, const char *desc, const char *life_time, const char *manufactor, double price, double price_fact)
