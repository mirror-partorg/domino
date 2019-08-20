
#include "traffic.h"
#include <datetime.h>
#include "../var/db.h"
#include "../var/util.h"
#include "misc.h"

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

static int get_product_key(do_alias_t *alias, PyObject *product, product_key0_t *key);
static double get_product_rest(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm, int number, int price, double *sum);
static double get_product_sale(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1, struct tm *tm2, double *sum);
static double get_product_purchase(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm1, struct tm *tm2, double *sum);

static PyObject *Traffic_rest(Traffic *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"product","stores","date", "number", "price", NULL};
    PyObject *product = NULL;
    PyObject *date = NULL;
    PyObject *stores = NULL;
    int number = 1;
    int price = 2;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|OOii", kwlist, &product, &stores, &date, &number, &price) ) {
        do_log(LOG_ERR, "no param \"product\"");
        return NULL;
    }
    product_key0_t product_key0;
    do_string_list_t *stores_list;
    struct tm tm = {0,};
    int i;
    double quant;
    double sum;

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
        quant+=get_product_rest(self->priv->alias, &product_key0, stores_list->list[i], &tm, number, price, &s);
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


static PyMethodDef Traffic_methods[] = {
    {"rest", (PyCFunction)Traffic_rest, METH_VARARGS|METH_KEYWORDS, "rest"},
    {"sale", (PyCFunction)Traffic_sale, METH_VARARGS|METH_KEYWORDS, "sale"},
    {"purchase", (PyCFunction)Traffic_purchase, METH_VARARGS|METH_KEYWORDS, "purchase"},

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

static double get_product_rest(do_alias_t *alias, product_key0_t *key, const char *store, struct tm *tm, int number, int price, double *sum)
{
    double retval, s;
    retval = get_product_parcel_rest(alias, key, store, tm, number, price, &s);
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
