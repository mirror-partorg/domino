
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "document_order.h"

static void DocumentOrder_dealloc(PyObject* self)
{
    do_free(((DocumentOrder*)self)->priv);
    Py_DECREF(((DocumentOrder*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentOrder free\n");
}
static PyObject *DocumentOrder_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrder new\n");
    DocumentOrder *self;
    self = PyObject_NEW(DocumentOrder, type);
    //self = (DocumentOrder *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_order_rec_t));
    return (PyObject *)self;
}
static int DocumentOrder_init(DocumentOrder *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentOrder init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.dtype);

    do_text_set_empty(self->priv->data.dtype1);

    do_text_set_empty(self->priv->data.sklad);

    do_text_set_empty(self->priv->data.document);

    do_text_set_empty(self->priv->data.code);

    do_document_order_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentOrder_clone(DocumentOrder *self)
{
    DocumentOrder *result;
    result = (DocumentOrder*)PyObject_CallFunction((PyObject*)getDocumentOrderType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *DocumentOrder_document_type(DocumentOrder* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrder_set_document_type(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_document_operation(DocumentOrder* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype1);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrder_set_document_operation(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.dtype1");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.dtype1, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.dtype1);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_store(DocumentOrder* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrder_set_store(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_document(DocumentOrder* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrder_set_document(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_line(DocumentOrder* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.line);

    return result;
}

static PyObject *DocumentOrder_set_line(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.line");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.line = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.line = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.line");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.line);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_sort(DocumentOrder* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.sort);

    return result;
}

static PyObject *DocumentOrder_set_sort(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sort");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.sort = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.sort = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sort");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.sort);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_code(DocumentOrder* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrder_set_code(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_date(DocumentOrder* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *DocumentOrder_date_raw(DocumentOrder* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *DocumentOrder_set_date(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            do_date_set(&self->priv->data.date, tm);
        }
    }
    /*do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_time(DocumentOrder* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *DocumentOrder_time_raw(DocumentOrder* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.time);

    return result;
}

static PyObject *DocumentOrder_set_time(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.time = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             do_time_set(&self->priv->data.time, tm);
        }
    }
    /*do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_get_quant(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get quant");
        return NULL;
    }
    double quant;
    quant = do_document_order_quant(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(quant);

    return result;
}

static PyObject *DocumentOrder_set_quant(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value=NULL;
    int number;
    static char *kwlist[] = {"number", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set quant");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_document_order_quant_set(self->alias->alias,self->priv, number, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_document_order_quant_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set quant");
            return NULL;
        }    
    }
    /*double quant;
    quant = do_document_order_quant(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(quant);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_get_price(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get price");
        return NULL;
    }
    double price;
    price = do_document_order_price(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(price);

    return result;
}

static PyObject *DocumentOrder_set_price(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set price");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_document_order_price_set(self->alias->alias,self->priv, number, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_document_order_price_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set price");
            return NULL;
        }
    }
    /*double price;
    price = do_document_order_price(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(price);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_accepted(DocumentOrder* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.accepted);

    return result;
}

static PyObject *DocumentOrder_set_accepted(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.accepted");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.accepted = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.accepted = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.accepted");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.accepted);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrder_set_param(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_document_order_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_document_order_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_document_order_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *DocumentOrder_set_params(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_order_params_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             PyObject *tmp_name;
             tmp_name = NULL;
             char *name;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyString_Check(pKey) )
                  name = MyString_AsString(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
                return NULL;
             }
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyString_Check(pValue) ) {
                 char *val;
                 val = MyString_AsString(pValue);
                 do_document_order_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_document_order_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_order_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \"value iter\": wrong type");
                 return NULL;
             }
             do_free(name);
             if ( tmp_name )
                 Py_DECREF(tmp_name);
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *DocumentOrder_get_param(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_document_order_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *DocumentOrder_params_clear(DocumentOrder* self)
{
    do_document_order_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *DocumentOrder_get_params(DocumentOrder* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}


static PyObject *DocumentOrder_prices_clear(DocumentOrder* self)
{
    do_document_order_price_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *DocumentOrder_get_prices(DocumentOrder* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double price;

    for ( i = 0; i < 8; i++ ) {
        number = self->priv->data.nprice[i];
        if ( !number ) break;
        price = do_document_order_price(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(price);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *DocumentOrder_set_prices(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_order_price_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             int number;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyLong_Check(pKey) )
                  number = MyLong_AsLong(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
                 return NULL;
             }
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_document_order_price_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_order_price_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \"value iter\": wrong type");
                 return NULL;
             }
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}


static PyObject *DocumentOrder_quants_clear(DocumentOrder* self)
{
    do_document_order_quant_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *DocumentOrder_get_quants(DocumentOrder* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 4; i++ ) {
        number = self->priv->data.nquant[i];
        if ( !number ) break;
        quant = do_document_order_quant(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *DocumentOrder_set_quants(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_order_quant_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             int number;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyLong_Check(pKey) )
                  number = MyLong_AsLong(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
                 return NULL;
             }
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_document_order_quant_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_order_quant_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \"value iter\": wrong type");
                 return NULL;
             }
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrder_prev(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_gt(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_next(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_ge(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_equal(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_last(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_lt(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_le(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_first(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() )
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() )
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() )
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() )
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() )
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_iter_gt(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", "depth", NULL};
    int status;
    int depth;
    void *key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &key, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
        key_cmp = (document_order_key0_t*)do_malloc(sizeof(document_order_key0_t));
        memcpy(key_cmp, ((DocumentOrderKey0*)key)->priv, sizeof(document_order_key0_t));
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
        key_cmp = (document_order_key1_t*)do_malloc(sizeof(document_order_key1_t));
        memcpy(key_cmp, ((DocumentOrderKey1*)key)->priv, sizeof(document_order_key1_t));
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
        key_cmp = (document_order_key2_t*)do_malloc(sizeof(document_order_key2_t));
        memcpy(key_cmp, ((DocumentOrderKey2*)key)->priv, sizeof(document_order_key2_t));
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
        key_cmp = (document_order_key3_t*)do_malloc(sizeof(document_order_key3_t));
        memcpy(key_cmp, ((DocumentOrderKey3*)key)->priv, sizeof(document_order_key3_t));
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
        key_cmp = (document_order_key4_t*)do_malloc(sizeof(document_order_key4_t));
        memcpy(key_cmp, ((DocumentOrderKey4*)key)->priv, sizeof(document_order_key4_t));
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->document, 
                    ((DocumentOrderKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->line, 
                    ((DocumentOrderKey0*)key)->priv->line))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->document, 
                    ((DocumentOrderKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sort, 
                    ((DocumentOrderKey1*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey2*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey2*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->document, 
                    ((DocumentOrderKey2*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->code, 
                    ((DocumentOrderKey2*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->code, 
                    ((DocumentOrderKey3*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->date, 
                    ((DocumentOrderKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->time, 
                    ((DocumentOrderKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->date, 
                    ((DocumentOrderKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->time, 
                    ((DocumentOrderKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentOrder_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
            status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
            status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
            status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
            status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
            status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_NEXT);
        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

    }
    if ( status == DO_ERROR ) {
        do_free(key_cmp);
        Py_DECREF(retval);
        return NULL;
    }
    do_free(key_cmp);
    //Py_INCREF(retval);
    return retval;
}

static PyObject *DocumentOrder_iter_ge(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", "depth", NULL};
    int status;
    int depth;
    void *key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &key, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
        key_cmp = (document_order_key0_t*)do_malloc(sizeof(document_order_key0_t));
        memcpy(key_cmp, ((DocumentOrderKey0*)key)->priv, sizeof(document_order_key0_t));
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
        key_cmp = (document_order_key1_t*)do_malloc(sizeof(document_order_key1_t));
        memcpy(key_cmp, ((DocumentOrderKey1*)key)->priv, sizeof(document_order_key1_t));
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
        key_cmp = (document_order_key2_t*)do_malloc(sizeof(document_order_key2_t));
        memcpy(key_cmp, ((DocumentOrderKey2*)key)->priv, sizeof(document_order_key2_t));
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
        key_cmp = (document_order_key3_t*)do_malloc(sizeof(document_order_key3_t));
        memcpy(key_cmp, ((DocumentOrderKey3*)key)->priv, sizeof(document_order_key3_t));
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
        key_cmp = (document_order_key4_t*)do_malloc(sizeof(document_order_key4_t));
        memcpy(key_cmp, ((DocumentOrderKey4*)key)->priv, sizeof(document_order_key4_t));
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->document, 
                    ((DocumentOrderKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->line, 
                    ((DocumentOrderKey0*)key)->priv->line))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->document, 
                    ((DocumentOrderKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sort, 
                    ((DocumentOrderKey1*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey2*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey2*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->document, 
                    ((DocumentOrderKey2*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->code, 
                    ((DocumentOrderKey2*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->code, 
                    ((DocumentOrderKey3*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->date, 
                    ((DocumentOrderKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->time, 
                    ((DocumentOrderKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->date, 
                    ((DocumentOrderKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->time, 
                    ((DocumentOrderKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentOrder_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
            status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
            status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
            status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
            status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
            status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_NEXT);
        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

    }
    if ( status == DO_ERROR ) {
        do_free(key_cmp);
        Py_DECREF(retval);
        return NULL;
    }
    do_free(key_cmp);
    //Py_INCREF(retval);
    return retval;
}

static PyObject *DocumentOrder_iter_equal(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", "depth", NULL};
    int status;
    int depth;
    void *key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &key, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
        key_cmp = (document_order_key0_t*)do_malloc(sizeof(document_order_key0_t));
        memcpy(key_cmp, ((DocumentOrderKey0*)key)->priv, sizeof(document_order_key0_t));
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
        key_cmp = (document_order_key1_t*)do_malloc(sizeof(document_order_key1_t));
        memcpy(key_cmp, ((DocumentOrderKey1*)key)->priv, sizeof(document_order_key1_t));
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
        key_cmp = (document_order_key2_t*)do_malloc(sizeof(document_order_key2_t));
        memcpy(key_cmp, ((DocumentOrderKey2*)key)->priv, sizeof(document_order_key2_t));
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
        key_cmp = (document_order_key3_t*)do_malloc(sizeof(document_order_key3_t));
        memcpy(key_cmp, ((DocumentOrderKey3*)key)->priv, sizeof(document_order_key3_t));
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
        key_cmp = (document_order_key4_t*)do_malloc(sizeof(document_order_key4_t));
        memcpy(key_cmp, ((DocumentOrderKey4*)key)->priv, sizeof(document_order_key4_t));
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->document, 
                    ((DocumentOrderKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->line, 
                    ((DocumentOrderKey0*)key)->priv->line))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->document, 
                    ((DocumentOrderKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sort, 
                    ((DocumentOrderKey1*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey2*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey2*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->document, 
                    ((DocumentOrderKey2*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->code, 
                    ((DocumentOrderKey2*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->code, 
                    ((DocumentOrderKey3*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->date, 
                    ((DocumentOrderKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->time, 
                    ((DocumentOrderKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->date, 
                    ((DocumentOrderKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->time, 
                    ((DocumentOrderKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentOrder_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
            status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
            status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
            status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
            status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
            status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_NEXT);
        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

    }
    if ( status == DO_ERROR ) {
        do_free(key_cmp);
        Py_DECREF(retval);
        return NULL;
    }
    do_free(key_cmp);
    //Py_INCREF(retval);
    return retval;
}

static PyObject *DocumentOrder_iter_last(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", "depth", NULL};
    int status;
    int depth;
    void *key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &key, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
        key_cmp = (document_order_key0_t*)do_malloc(sizeof(document_order_key0_t));
        memcpy(key_cmp, ((DocumentOrderKey0*)key)->priv, sizeof(document_order_key0_t));
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
        key_cmp = (document_order_key1_t*)do_malloc(sizeof(document_order_key1_t));
        memcpy(key_cmp, ((DocumentOrderKey1*)key)->priv, sizeof(document_order_key1_t));
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
        key_cmp = (document_order_key2_t*)do_malloc(sizeof(document_order_key2_t));
        memcpy(key_cmp, ((DocumentOrderKey2*)key)->priv, sizeof(document_order_key2_t));
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
        key_cmp = (document_order_key3_t*)do_malloc(sizeof(document_order_key3_t));
        memcpy(key_cmp, ((DocumentOrderKey3*)key)->priv, sizeof(document_order_key3_t));
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
        key_cmp = (document_order_key4_t*)do_malloc(sizeof(document_order_key4_t));
        memcpy(key_cmp, ((DocumentOrderKey4*)key)->priv, sizeof(document_order_key4_t));
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->document, 
                    ((DocumentOrderKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->line, 
                    ((DocumentOrderKey0*)key)->priv->line))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->document, 
                    ((DocumentOrderKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sort, 
                    ((DocumentOrderKey1*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey2*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey2*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->document, 
                    ((DocumentOrderKey2*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->code, 
                    ((DocumentOrderKey2*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->code, 
                    ((DocumentOrderKey3*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->date, 
                    ((DocumentOrderKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->time, 
                    ((DocumentOrderKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->date, 
                    ((DocumentOrderKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->time, 
                    ((DocumentOrderKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentOrder_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
            status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
            status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
            status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
            status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
            status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_PREVIOUS);
        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

    }
    if ( status == DO_ERROR ) {
        do_free(key_cmp);
        Py_DECREF(retval);
        return NULL;
    }
    do_free(key_cmp);
    //Py_INCREF(retval);
    return retval;
}

static PyObject *DocumentOrder_iter_lt(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", "depth", NULL};
    int status;
    int depth;
    void *key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &key, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
        key_cmp = (document_order_key0_t*)do_malloc(sizeof(document_order_key0_t));
        memcpy(key_cmp, ((DocumentOrderKey0*)key)->priv, sizeof(document_order_key0_t));
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
        key_cmp = (document_order_key1_t*)do_malloc(sizeof(document_order_key1_t));
        memcpy(key_cmp, ((DocumentOrderKey1*)key)->priv, sizeof(document_order_key1_t));
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
        key_cmp = (document_order_key2_t*)do_malloc(sizeof(document_order_key2_t));
        memcpy(key_cmp, ((DocumentOrderKey2*)key)->priv, sizeof(document_order_key2_t));
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
        key_cmp = (document_order_key3_t*)do_malloc(sizeof(document_order_key3_t));
        memcpy(key_cmp, ((DocumentOrderKey3*)key)->priv, sizeof(document_order_key3_t));
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
        key_cmp = (document_order_key4_t*)do_malloc(sizeof(document_order_key4_t));
        memcpy(key_cmp, ((DocumentOrderKey4*)key)->priv, sizeof(document_order_key4_t));
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->document, 
                    ((DocumentOrderKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->line, 
                    ((DocumentOrderKey0*)key)->priv->line))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->document, 
                    ((DocumentOrderKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sort, 
                    ((DocumentOrderKey1*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey2*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey2*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->document, 
                    ((DocumentOrderKey2*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->code, 
                    ((DocumentOrderKey2*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->code, 
                    ((DocumentOrderKey3*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->date, 
                    ((DocumentOrderKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->time, 
                    ((DocumentOrderKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->date, 
                    ((DocumentOrderKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->time, 
                    ((DocumentOrderKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentOrder_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
            status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
            status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
            status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
            status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
            status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_PREVIOUS);
        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

    }
    if ( status == DO_ERROR ) {
        do_free(key_cmp);
        Py_DECREF(retval);
        return NULL;
    }
    do_free(key_cmp);
    //Py_INCREF(retval);
    return retval;
}

static PyObject *DocumentOrder_iter_le(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", "depth", NULL};
    int status;
    int depth;
    void *key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &key, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
        key_cmp = (document_order_key0_t*)do_malloc(sizeof(document_order_key0_t));
        memcpy(key_cmp, ((DocumentOrderKey0*)key)->priv, sizeof(document_order_key0_t));
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
        key_cmp = (document_order_key1_t*)do_malloc(sizeof(document_order_key1_t));
        memcpy(key_cmp, ((DocumentOrderKey1*)key)->priv, sizeof(document_order_key1_t));
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
        key_cmp = (document_order_key2_t*)do_malloc(sizeof(document_order_key2_t));
        memcpy(key_cmp, ((DocumentOrderKey2*)key)->priv, sizeof(document_order_key2_t));
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
        key_cmp = (document_order_key3_t*)do_malloc(sizeof(document_order_key3_t));
        memcpy(key_cmp, ((DocumentOrderKey3*)key)->priv, sizeof(document_order_key3_t));
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
        key_cmp = (document_order_key4_t*)do_malloc(sizeof(document_order_key4_t));
        memcpy(key_cmp, ((DocumentOrderKey4*)key)->priv, sizeof(document_order_key4_t));
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->document, 
                    ((DocumentOrderKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->line, 
                    ((DocumentOrderKey0*)key)->priv->line))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->document, 
                    ((DocumentOrderKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sort, 
                    ((DocumentOrderKey1*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey2*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey2*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->document, 
                    ((DocumentOrderKey2*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->code, 
                    ((DocumentOrderKey2*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->code, 
                    ((DocumentOrderKey3*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->date, 
                    ((DocumentOrderKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->time, 
                    ((DocumentOrderKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->date, 
                    ((DocumentOrderKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->time, 
                    ((DocumentOrderKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentOrder_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
            status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
            status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
            status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
            status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
            status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_PREVIOUS);
        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

    }
    if ( status == DO_ERROR ) {
        do_free(key_cmp);
        Py_DECREF(retval);
        return NULL;
    }
    do_free(key_cmp);
    //Py_INCREF(retval);
    return retval;
}

static PyObject *DocumentOrder_iter_first(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", "depth", NULL};
    int status;
    int depth;
    void *key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &key, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }

    if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
        key_cmp = (document_order_key0_t*)do_malloc(sizeof(document_order_key0_t));
        memcpy(key_cmp, ((DocumentOrderKey0*)key)->priv, sizeof(document_order_key0_t));
        status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
        key_cmp = (document_order_key1_t*)do_malloc(sizeof(document_order_key1_t));
        memcpy(key_cmp, ((DocumentOrderKey1*)key)->priv, sizeof(document_order_key1_t));
        status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
        key_cmp = (document_order_key2_t*)do_malloc(sizeof(document_order_key2_t));
        memcpy(key_cmp, ((DocumentOrderKey2*)key)->priv, sizeof(document_order_key2_t));
        status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
        key_cmp = (document_order_key3_t*)do_malloc(sizeof(document_order_key3_t));
        memcpy(key_cmp, ((DocumentOrderKey3*)key)->priv, sizeof(document_order_key3_t));
        status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
        key_cmp = (document_order_key4_t*)do_malloc(sizeof(document_order_key4_t));
        memcpy(key_cmp, ((DocumentOrderKey4*)key)->priv, sizeof(document_order_key4_t));
        status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->document, 
                    ((DocumentOrderKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key0_t*)key_cmp)->line, 
                    ((DocumentOrderKey0*)key)->priv->line))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->document, 
                    ((DocumentOrderKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key1_t*)key_cmp)->sort, 
                    ((DocumentOrderKey1*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->dtype, 
                    ((DocumentOrderKey2*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey2*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->document, 
                    ((DocumentOrderKey2*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_order_key2_t*)key_cmp)->code, 
                    ((DocumentOrderKey2*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->code, 
                    ((DocumentOrderKey3*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->date, 
                    ((DocumentOrderKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key3_t*)key_cmp)->time, 
                    ((DocumentOrderKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->sklad, 
                    ((DocumentOrderKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->date, 
                    ((DocumentOrderKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_order_key4_t*)key_cmp)->time, 
                    ((DocumentOrderKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentOrder_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentOrderKey0Type() ) {
            status = do_document_order_get0(self->alias->alias, self->priv, ((DocumentOrderKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey1Type() ) {
            status = do_document_order_get1(self->alias->alias, self->priv, ((DocumentOrderKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey2Type() ) {
            status = do_document_order_get2(self->alias->alias, self->priv, ((DocumentOrderKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey3Type() ) {
            status = do_document_order_get3(self->alias->alias, self->priv, ((DocumentOrderKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentOrderKey4Type() ) {
            status = do_document_order_get4(self->alias->alias, self->priv, ((DocumentOrderKey4*)key)->priv, DO_GET_NEXT);
        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

    }
    if ( status == DO_ERROR ) {
        do_free(key_cmp);
        Py_DECREF(retval);
        return NULL;
    }
    do_free(key_cmp);
    //Py_INCREF(retval);
    return retval;
}

static PyObject *DocumentOrder_update(DocumentOrder* self)
{
    int status;
    status = do_document_order_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_insert(DocumentOrder* self)
{
    int status;
    status = do_document_order_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_delete(DocumentOrder* self)
{
    int status;
    status = do_document_order_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrder_set_alias(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentOrder*)self)->alias);
    Py_INCREF(obj);
    ((DocumentOrder*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrder_set(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {
        memcpy(self->priv, ((DocumentOrder*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentOrderKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentOrderKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentOrderKey0*)obj)->priv->document);

        do_cpy(self->priv->data.line, ((DocumentOrderKey0*)obj)->priv->line);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentOrderKey1*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentOrderKey1*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentOrderKey1*)obj)->priv->document);

        do_cpy(self->priv->data.sort, ((DocumentOrderKey1*)obj)->priv->sort);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentOrderKey2*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentOrderKey2*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentOrderKey2*)obj)->priv->document);

        do_cpy(self->priv->data.code, ((DocumentOrderKey2*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {

        do_cpy(self->priv->data.code, ((DocumentOrderKey3*)obj)->priv->code);

        do_cpy(self->priv->data.date, ((DocumentOrderKey3*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((DocumentOrderKey3*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {

        do_cpy(self->priv->data.sklad, ((DocumentOrderKey4*)obj)->priv->sklad);

        do_cpy(self->priv->data.date, ((DocumentOrderKey4*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((DocumentOrderKey4*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->data.dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->data.dtype1, ((Document*)obj)->priv->data.dtype1);

        do_cpy(self->priv->data.sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->data.document, ((Document*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey3*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentKey3*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey4*)obj)->priv->dtype);

    }

    else 
    if ( PyDict_Check(obj) ) {
        PyObject *fields;
        int i;
        
        fields = PyDict_Keys(obj);
        for ( i = 0; i < PyList_Size(fields); i++ ) {
            PyObject *name;
            PyObject *value;
            char *field_name;
            
            name = PyList_GetItem(fields, i);
            value = PyDict_GetItem(obj, name); 
            field_name = MyString_AsString(name);
            if ( field_name[0] == '\0' ) {
                do_log(LOG_ERR, "Dictionary key is not a string");
                return NULL;
            } 


            else
            if ( !strcmp("document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document_operation", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_operation", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("line", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_line", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("quant", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_quant", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("quants", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_quants", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("price", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_price", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("prices", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_prices", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("accepted", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_accepted", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("param", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_param", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("params", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_params", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentOrder\"does not contain \"%s\"", field_name);
                return NULL;
            }
            do_free(field_name);
        }
    }
    
    else
    {
        do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrder_fields(DocumentOrder* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentOrder");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentOrder_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_document_operation(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_operation", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_line(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("line", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_sort(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("sort", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = DocumentOrder_get_quants(self, NULL);
    buf = get_quants_desc(value, "quants");
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = DocumentOrder_get_prices(self, NULL);
    buf=get_prices_desc(value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrder_accepted(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("accepted", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = DocumentOrder_get_params(self, NULL);
    buf = get_params_desc(value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef DocumentOrder_getset[] = {

    {"document_type",(getter)DocumentOrder_document_type},

    {"document_operation",(getter)DocumentOrder_document_operation},

    {"store",(getter)DocumentOrder_store},

    {"document",(getter)DocumentOrder_document},

    {"line",(getter)DocumentOrder_line},

    {"sort",(getter)DocumentOrder_sort},

    {"code",(getter)DocumentOrder_code},

    {"date",(getter)DocumentOrder_date},

    {"date_raw",(getter)DocumentOrder_date_raw},

    {"time",(getter)DocumentOrder_time},

    {"time_raw",(getter)DocumentOrder_time_raw},

    {"quants",(getter)DocumentOrder_get_quants},

    {"prices",(getter)DocumentOrder_get_prices},

    {"accepted",(getter)DocumentOrder_accepted},

    {"params",(getter)DocumentOrder_get_params},

    {NULL}
};

static PyMethodDef DocumentOrder_methods[] = {
    {"set", (PyCFunction)DocumentOrder_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentOrder_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentOrder_clone, METH_NOARGS, "clone"},

    {"clear_prices", (PyCFunction)DocumentOrder_prices_clear, METH_NOARGS, "do_DocumentOrder_prices_clear"},
    {"price", (PyCFunction)DocumentOrder_get_price, METH_VARARGS|METH_KEYWORDS, "do_DocumentOrder_price"},
    {"set_prices", (PyCFunction)DocumentOrder_set_prices, METH_VARARGS|METH_KEYWORDS, "do_DocumentOrder_set_prices"},

    {"clear_quants", (PyCFunction)DocumentOrder_quants_clear, METH_NOARGS, "do_DocumentOrder_quants_clear"},
    {"quant", (PyCFunction)DocumentOrder_get_quant, METH_VARARGS|METH_KEYWORDS, "do_DocumentOrder_quant"},
    {"set_quants", (PyCFunction)DocumentOrder_set_quants, METH_VARARGS|METH_KEYWORDS, "do_DocumentOrder_set_quants"},

    {"set_document_type", (PyCFunction)DocumentOrder_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_document_type"},

    {"set_document_operation", (PyCFunction)DocumentOrder_set_document_operation, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_document_operation"},

    {"set_store", (PyCFunction)DocumentOrder_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_store"},

    {"set_document", (PyCFunction)DocumentOrder_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_document"},

    {"set_line", (PyCFunction)DocumentOrder_set_line, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_line"},

    {"set_sort", (PyCFunction)DocumentOrder_set_sort, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_sort"},

    {"set_code", (PyCFunction)DocumentOrder_set_code, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_code"},

    {"set_date", (PyCFunction)DocumentOrder_set_date, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_date"},

    {"set_time", (PyCFunction)DocumentOrder_set_time, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_time"},

    {"set_quant", (PyCFunction)DocumentOrder_set_quant, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_quant"},

    {"set_price", (PyCFunction)DocumentOrder_set_price, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_price"},

    {"set_accepted", (PyCFunction)DocumentOrder_set_accepted, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_set_accepted"},

    {"set_param", (PyCFunction)DocumentOrder_set_param, METH_VARARGS|METH_KEYWORDS, "do_DocumentOrder_param_set"},
    {"param", (PyCFunction)DocumentOrder_get_param, METH_VARARGS|METH_KEYWORDS, "do_DocumentOrder_param"},
    {"clear_params", (PyCFunction)DocumentOrder_params_clear, METH_NOARGS, "do_DocumentOrder_param_clear"},
    {"set_params", (PyCFunction)DocumentOrder_set_params, METH_VARARGS|METH_KEYWORDS, "do_DocumentOrder_set_params"},

    {"get_prev", (PyCFunction)DocumentOrder_prev, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_prev"},

    {"get_gt", (PyCFunction)DocumentOrder_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_gt"},

    {"get_next", (PyCFunction)DocumentOrder_next, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_next"},

    {"get_ge", (PyCFunction)DocumentOrder_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_ge"},

    {"get_equal", (PyCFunction)DocumentOrder_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_equal"},

    {"get_last", (PyCFunction)DocumentOrder_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_last"},

    {"get_lt", (PyCFunction)DocumentOrder_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_lt"},

    {"get_le", (PyCFunction)DocumentOrder_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_le"},

    {"get_first", (PyCFunction)DocumentOrder_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_first"},

    {"gets_gt", (PyCFunction)DocumentOrder_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentOrder_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentOrder_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_iter_equal"},

    {"gets_last", (PyCFunction)DocumentOrder_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_iter_last"},

    {"gets_lt", (PyCFunction)DocumentOrder_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_iter_lt"},

    {"gets_le", (PyCFunction)DocumentOrder_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_iter_le"},

    {"gets_first", (PyCFunction)DocumentOrder_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_iter_first"},

    {"update", (PyCFunction)DocumentOrder_update, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_update"},

    {"insert", (PyCFunction)DocumentOrder_insert, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_insert"},

    {"delete", (PyCFunction)DocumentOrder_delete, METH_VARARGS|METH_KEYWORDS, "DocumentOrder_delete"},

    {NULL}
};

static PyTypeObject DocumentOrderType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentOrder",             /*tp_name*/
    sizeof(DocumentOrder),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrder_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentOrder_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrder objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrder_methods,             /* tp_methods */
    0,
    DocumentOrder_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrder_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrder_new,                 /* tp_new */
};
PyTypeObject *getDocumentOrderType()
{
    return &DocumentOrderType_;
}

static void DocumentOrderKey0_dealloc(PyObject* self)
{
    do_free(((DocumentOrderKey0*)self)->priv);
    Py_DECREF(((DocumentOrderKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentOrderKey0 free\n");
}
static PyObject *DocumentOrderKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey0 new\n");
    DocumentOrderKey0 *self;
    self = PyObject_NEW(DocumentOrderKey0, type);
    //self = (DocumentOrderKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_order_key0_t));
    return (PyObject *)self;
}
static int DocumentOrderKey0_init(DocumentOrderKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentOrderKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->dtype);

    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentOrderKey0_clone(DocumentOrderKey0 *self)
{
    DocumentOrderKey0 *result;
    result = (DocumentOrderKey0*)PyObject_CallFunction((PyObject*)getDocumentOrderKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentOrderKey0_document_type(DocumentOrderKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey0_set_document_type(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey0_store(DocumentOrderKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey0_set_store(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey0_document(DocumentOrderKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey0_set_document(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey0_line(DocumentOrderKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->line);

    return result;
}

static PyObject *DocumentOrderKey0_set_line(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set line");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->line = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->line = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set line");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->line);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey0_prev(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_gt(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_next(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_ge(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_equal(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_last(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_lt(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_le(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_first(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey0_iter_gt(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.line, 
                 self->priv->line))
               break;
       }

 
        item = DocumentOrderKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey0_iter_ge(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.line, 
                 self->priv->line))
               break;
       }

 
        item = DocumentOrderKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey0_iter_equal(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.line, 
                 self->priv->line))
               break;
       }

 
        item = DocumentOrderKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey0_iter_last(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.line, 
                 self->priv->line))
               break;
       }

 
        item = DocumentOrderKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey0_iter_lt(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.line, 
                 self->priv->line))
               break;
       }

 
        item = DocumentOrderKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey0_iter_le(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.line, 
                 self->priv->line))
               break;
       }

 
        item = DocumentOrderKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey0_iter_first(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.line, 
                 self->priv->line))
               break;
       }

 
        item = DocumentOrderKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey0_cmp(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentOrderKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentOrderKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentOrderKey0*)obj)->priv->document);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->line - ((DocumentOrderKey0*)obj)->priv->line;
       }

       return MyLong_FromLong(res);
    }
        
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentKey0*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentOrderKey0_set_alias(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentOrderKey0*)self)->alias);
    Py_INCREF(obj);
    ((DocumentOrderKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey0_set(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {
        memcpy(self->priv, ((DocumentOrderKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->dtype, ((DocumentOrder*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentOrder*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentOrder*)obj)->priv->data.document);

        do_cpy(self->priv->line, ((DocumentOrder*)obj)->priv->data.line);

    }

    else 
    if ( PyList_Check(obj) ||  PyTuple_Check(obj) ) {
        int i;
        int size;
        size = PyList_Check(obj) ? PyList_Size(obj) : PyTuple_Size(obj);
        for ( i = 0; i < size; i++ ) {
            PyObject *value;
            value =  PyList_Check(obj) ? PyList_GetItem(obj, i) : PyTuple_GetItem(obj, i);
            switch (i) {

                case 0:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_line", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((Document*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey3*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey3*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey4*)obj)->priv->dtype);

    }

    else 
    if ( PyDict_Check(obj) ) {
        PyObject *fields;
        int i;
        
        fields = PyDict_Keys(obj);
        for ( i = 0; i < PyList_Size(fields); i++ ) {
            PyObject *name;
            PyObject *value;
            char *field_name;
            
            name = PyList_GetItem(fields, i);
            value = PyDict_GetItem(obj, name); 
            field_name = MyString_AsString(name);
            if ( field_name[0] == '\0' ) {
                do_log(LOG_ERR, "Dictionary key is not a string");
                return NULL;
            } 


            else
            if ( !strcmp("document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("line", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_line", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentOrderKey0\"does not contain \"%s\"", field_name);
                return NULL;
            }
            do_free(field_name);
        }
    }
    
    else
    {
        do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey0_fields(DocumentOrderKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentOrderKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentOrderKey0_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey0_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey0_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey0_line(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("line", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef DocumentOrderKey0_getset[] = {

    {"document_type",(getter)DocumentOrderKey0_document_type},

    {"store",(getter)DocumentOrderKey0_store},

    {"document",(getter)DocumentOrderKey0_document},

    {"line",(getter)DocumentOrderKey0_line},

    {NULL}
};

static PyMethodDef DocumentOrderKey0_methods[] = {
    {"set", (PyCFunction)DocumentOrderKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentOrderKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentOrderKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentOrderKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentOrderKey0_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_set_document_type"},

    {"set_store", (PyCFunction)DocumentOrderKey0_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_set_store"},

    {"set_document", (PyCFunction)DocumentOrderKey0_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_set_document"},

    {"set_line", (PyCFunction)DocumentOrderKey0_set_line, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_set_line"},

    {"get_prev", (PyCFunction)DocumentOrderKey0_prev, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_prev"},

    {"get_gt", (PyCFunction)DocumentOrderKey0_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_gt"},

    {"get_next", (PyCFunction)DocumentOrderKey0_next, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_next"},

    {"get_ge", (PyCFunction)DocumentOrderKey0_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_ge"},

    {"get_equal", (PyCFunction)DocumentOrderKey0_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_equal"},

    {"get_last", (PyCFunction)DocumentOrderKey0_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_last"},

    {"get_lt", (PyCFunction)DocumentOrderKey0_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_lt"},

    {"get_le", (PyCFunction)DocumentOrderKey0_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_le"},

    {"get_first", (PyCFunction)DocumentOrderKey0_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_first"},

    {"gets_gt", (PyCFunction)DocumentOrderKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentOrderKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentOrderKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_iter_equal"},

    {"gets_last", (PyCFunction)DocumentOrderKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_iter_last"},

    {"gets_lt", (PyCFunction)DocumentOrderKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_iter_lt"},

    {"gets_le", (PyCFunction)DocumentOrderKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_iter_le"},

    {"gets_first", (PyCFunction)DocumentOrderKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey0_iter_first"},

    {NULL}
};

static PyTypeObject DocumentOrderKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentOrderKey0",             /*tp_name*/
    sizeof(DocumentOrderKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentOrderKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey0_methods,             /* tp_methods */
    0,
    DocumentOrderKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey0_new,                 /* tp_new */
};
PyTypeObject *getDocumentOrderKey0Type()
{
    return &DocumentOrderKey0Type_;
}

static void DocumentOrderKey1_dealloc(PyObject* self)
{
    do_free(((DocumentOrderKey1*)self)->priv);
    Py_DECREF(((DocumentOrderKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentOrderKey1 free\n");
}
static PyObject *DocumentOrderKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey1 new\n");
    DocumentOrderKey1 *self;
    self = PyObject_NEW(DocumentOrderKey1, type);
    //self = (DocumentOrderKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_order_key1_t));
    return (PyObject *)self;
}
static int DocumentOrderKey1_init(DocumentOrderKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentOrderKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->dtype);

    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentOrderKey1_clone(DocumentOrderKey1 *self)
{
    DocumentOrderKey1 *result;
    result = (DocumentOrderKey1*)PyObject_CallFunction((PyObject*)getDocumentOrderKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentOrderKey1_document_type(DocumentOrderKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey1_set_document_type(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey1_store(DocumentOrderKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey1_set_store(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey1_document(DocumentOrderKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey1_set_document(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey1_sort(DocumentOrderKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->sort);

    return result;
}

static PyObject *DocumentOrderKey1_set_sort(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sort");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->sort = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->sort = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sort");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->sort);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey1_prev(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_gt(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_next(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_ge(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_equal(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_last(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_lt(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_le(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_first(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey1_iter_gt(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentOrderKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey1_iter_ge(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentOrderKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey1_iter_equal(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentOrderKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey1_iter_last(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentOrderKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey1_iter_lt(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentOrderKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey1_iter_le(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentOrderKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey1_iter_first(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentOrderKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey1_cmp(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentOrderKey1*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentOrderKey1*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentOrderKey1*)obj)->priv->document);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->sort - ((DocumentOrderKey1*)obj)->priv->sort;
       }

       return MyLong_FromLong(res);
    }
        
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentKey0*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentOrderKey1_set_alias(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentOrderKey1*)self)->alias);
    Py_INCREF(obj);
    ((DocumentOrderKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey1_set(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {
        memcpy(self->priv, ((DocumentOrderKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->dtype, ((DocumentOrder*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentOrder*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentOrder*)obj)->priv->data.document);

        do_cpy(self->priv->sort, ((DocumentOrder*)obj)->priv->data.sort);

    }

    else 
    if ( PyList_Check(obj) ||  PyTuple_Check(obj) ) {
        int i;
        int size;
        size = PyList_Check(obj) ? PyList_Size(obj) : PyTuple_Size(obj);
        for ( i = 0; i < size; i++ ) {
            PyObject *value;
            value =  PyList_Check(obj) ? PyList_GetItem(obj, i) : PyTuple_GetItem(obj, i);
            switch (i) {

                case 0:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((Document*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey3*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey3*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey4*)obj)->priv->dtype);

    }

    else 
    if ( PyDict_Check(obj) ) {
        PyObject *fields;
        int i;
        
        fields = PyDict_Keys(obj);
        for ( i = 0; i < PyList_Size(fields); i++ ) {
            PyObject *name;
            PyObject *value;
            char *field_name;
            
            name = PyList_GetItem(fields, i);
            value = PyDict_GetItem(obj, name); 
            field_name = MyString_AsString(name);
            if ( field_name[0] == '\0' ) {
                do_log(LOG_ERR, "Dictionary key is not a string");
                return NULL;
            } 


            else
            if ( !strcmp("document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentOrderKey1\"does not contain \"%s\"", field_name);
                return NULL;
            }
            do_free(field_name);
        }
    }
    
    else
    {
        do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey1_fields(DocumentOrderKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentOrderKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentOrderKey1_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey1_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey1_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey1_sort(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("sort", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef DocumentOrderKey1_getset[] = {

    {"document_type",(getter)DocumentOrderKey1_document_type},

    {"store",(getter)DocumentOrderKey1_store},

    {"document",(getter)DocumentOrderKey1_document},

    {"sort",(getter)DocumentOrderKey1_sort},

    {NULL}
};

static PyMethodDef DocumentOrderKey1_methods[] = {
    {"set", (PyCFunction)DocumentOrderKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentOrderKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentOrderKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentOrderKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentOrderKey1_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_set_document_type"},

    {"set_store", (PyCFunction)DocumentOrderKey1_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_set_store"},

    {"set_document", (PyCFunction)DocumentOrderKey1_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_set_document"},

    {"set_sort", (PyCFunction)DocumentOrderKey1_set_sort, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_set_sort"},

    {"get_prev", (PyCFunction)DocumentOrderKey1_prev, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_prev"},

    {"get_gt", (PyCFunction)DocumentOrderKey1_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_gt"},

    {"get_next", (PyCFunction)DocumentOrderKey1_next, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_next"},

    {"get_ge", (PyCFunction)DocumentOrderKey1_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_ge"},

    {"get_equal", (PyCFunction)DocumentOrderKey1_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_equal"},

    {"get_last", (PyCFunction)DocumentOrderKey1_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_last"},

    {"get_lt", (PyCFunction)DocumentOrderKey1_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_lt"},

    {"get_le", (PyCFunction)DocumentOrderKey1_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_le"},

    {"get_first", (PyCFunction)DocumentOrderKey1_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_first"},

    {"gets_gt", (PyCFunction)DocumentOrderKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentOrderKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentOrderKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_iter_equal"},

    {"gets_last", (PyCFunction)DocumentOrderKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_iter_last"},

    {"gets_lt", (PyCFunction)DocumentOrderKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_iter_lt"},

    {"gets_le", (PyCFunction)DocumentOrderKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_iter_le"},

    {"gets_first", (PyCFunction)DocumentOrderKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey1_iter_first"},

    {NULL}
};

static PyTypeObject DocumentOrderKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentOrderKey1",             /*tp_name*/
    sizeof(DocumentOrderKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentOrderKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey1_methods,             /* tp_methods */
    0,
    DocumentOrderKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey1_new,                 /* tp_new */
};
PyTypeObject *getDocumentOrderKey1Type()
{
    return &DocumentOrderKey1Type_;
}

static void DocumentOrderKey2_dealloc(PyObject* self)
{
    do_free(((DocumentOrderKey2*)self)->priv);
    Py_DECREF(((DocumentOrderKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentOrderKey2 free\n");
}
static PyObject *DocumentOrderKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey2 new\n");
    DocumentOrderKey2 *self;
    self = PyObject_NEW(DocumentOrderKey2, type);
    //self = (DocumentOrderKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_order_key2_t));
    return (PyObject *)self;
}
static int DocumentOrderKey2_init(DocumentOrderKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentOrderKey2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->dtype);

    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->document);

    do_text_set_empty(self->priv->code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentOrderKey2_clone(DocumentOrderKey2 *self)
{
    DocumentOrderKey2 *result;
    result = (DocumentOrderKey2*)PyObject_CallFunction((PyObject*)getDocumentOrderKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentOrderKey2_document_type(DocumentOrderKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey2_set_document_type(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey2_store(DocumentOrderKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey2_set_store(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey2_document(DocumentOrderKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey2_set_document(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey2_code(DocumentOrderKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey2_set_code(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey2_prev(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_gt(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_next(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_ge(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_equal(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_last(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_lt(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_le(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_first(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey2_iter_gt(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DocumentOrderKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey2_iter_ge(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DocumentOrderKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey2_iter_equal(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DocumentOrderKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey2_iter_last(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DocumentOrderKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey2_iter_lt(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DocumentOrderKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey2_iter_le(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DocumentOrderKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey2_iter_first(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DocumentOrderKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey2_cmp(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentOrderKey2*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentOrderKey2*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentOrderKey2*)obj)->priv->document);
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->code, ((DocumentOrderKey2*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }
        
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentKey0*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentOrderKey2_set_alias(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentOrderKey2*)self)->alias);
    Py_INCREF(obj);
    ((DocumentOrderKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey2_set(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {
        memcpy(self->priv, ((DocumentOrderKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->dtype, ((DocumentOrder*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentOrder*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentOrder*)obj)->priv->data.document);

        do_cpy(self->priv->code, ((DocumentOrder*)obj)->priv->data.code);

    }

    else 
    if ( PyList_Check(obj) ||  PyTuple_Check(obj) ) {
        int i;
        int size;
        size = PyList_Check(obj) ? PyList_Size(obj) : PyTuple_Size(obj);
        for ( i = 0; i < size; i++ ) {
            PyObject *value;
            value =  PyList_Check(obj) ? PyList_GetItem(obj, i) : PyTuple_GetItem(obj, i);
            switch (i) {

                case 0:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((Document*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey3*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey3*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey4*)obj)->priv->dtype);

    }

    else 
    if ( PyDict_Check(obj) ) {
        PyObject *fields;
        int i;
        
        fields = PyDict_Keys(obj);
        for ( i = 0; i < PyList_Size(fields); i++ ) {
            PyObject *name;
            PyObject *value;
            char *field_name;
            
            name = PyList_GetItem(fields, i);
            value = PyDict_GetItem(obj, name); 
            field_name = MyString_AsString(name);
            if ( field_name[0] == '\0' ) {
                do_log(LOG_ERR, "Dictionary key is not a string");
                return NULL;
            } 


            else
            if ( !strcmp("document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentOrderKey2\"does not contain \"%s\"", field_name);
                return NULL;
            }
            do_free(field_name);
        }
    }
    
    else
    {
        do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey2_fields(DocumentOrderKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentOrderKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentOrderKey2_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey2_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey2_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey2_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef DocumentOrderKey2_getset[] = {

    {"document_type",(getter)DocumentOrderKey2_document_type},

    {"store",(getter)DocumentOrderKey2_store},

    {"document",(getter)DocumentOrderKey2_document},

    {"code",(getter)DocumentOrderKey2_code},

    {NULL}
};

static PyMethodDef DocumentOrderKey2_methods[] = {
    {"set", (PyCFunction)DocumentOrderKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentOrderKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentOrderKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentOrderKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentOrderKey2_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_set_document_type"},

    {"set_store", (PyCFunction)DocumentOrderKey2_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_set_store"},

    {"set_document", (PyCFunction)DocumentOrderKey2_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_set_document"},

    {"set_code", (PyCFunction)DocumentOrderKey2_set_code, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_set_code"},

    {"get_prev", (PyCFunction)DocumentOrderKey2_prev, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_prev"},

    {"get_gt", (PyCFunction)DocumentOrderKey2_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_gt"},

    {"get_next", (PyCFunction)DocumentOrderKey2_next, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_next"},

    {"get_ge", (PyCFunction)DocumentOrderKey2_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_ge"},

    {"get_equal", (PyCFunction)DocumentOrderKey2_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_equal"},

    {"get_last", (PyCFunction)DocumentOrderKey2_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_last"},

    {"get_lt", (PyCFunction)DocumentOrderKey2_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_lt"},

    {"get_le", (PyCFunction)DocumentOrderKey2_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_le"},

    {"get_first", (PyCFunction)DocumentOrderKey2_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_first"},

    {"gets_gt", (PyCFunction)DocumentOrderKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentOrderKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentOrderKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_iter_equal"},

    {"gets_last", (PyCFunction)DocumentOrderKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_iter_last"},

    {"gets_lt", (PyCFunction)DocumentOrderKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_iter_lt"},

    {"gets_le", (PyCFunction)DocumentOrderKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_iter_le"},

    {"gets_first", (PyCFunction)DocumentOrderKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey2_iter_first"},

    {NULL}
};

static PyTypeObject DocumentOrderKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentOrderKey2",             /*tp_name*/
    sizeof(DocumentOrderKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentOrderKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey2_methods,             /* tp_methods */
    0,
    DocumentOrderKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey2_new,                 /* tp_new */
};
PyTypeObject *getDocumentOrderKey2Type()
{
    return &DocumentOrderKey2Type_;
}

static void DocumentOrderKey3_dealloc(PyObject* self)
{
    do_free(((DocumentOrderKey3*)self)->priv);
    Py_DECREF(((DocumentOrderKey3*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentOrderKey3 free\n");
}
static PyObject *DocumentOrderKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey3 new\n");
    DocumentOrderKey3 *self;
    self = PyObject_NEW(DocumentOrderKey3, type);
    //self = (DocumentOrderKey3 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_order_key3_t));
    return (PyObject *)self;
}
static int DocumentOrderKey3_init(DocumentOrderKey3 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentOrderKey3 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentOrderKey3_clone(DocumentOrderKey3 *self)
{
    DocumentOrderKey3 *result;
    result = (DocumentOrderKey3*)PyObject_CallFunction((PyObject*)getDocumentOrderKey3Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentOrderKey3_code(DocumentOrderKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey3_set_code(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey3_date(DocumentOrderKey3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *DocumentOrderKey3_date_raw(DocumentOrderKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *DocumentOrderKey3_set_date(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            do_date_set(&self->priv->date, tm);
        }
    }
    /*do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey3_time(DocumentOrderKey3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *DocumentOrderKey3_time_raw(DocumentOrderKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *DocumentOrderKey3_set_time(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->time = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             do_time_set(&self->priv->time, tm);
        }
    }
    /*do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey3_prev(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_gt(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_next(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_ge(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_equal(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_last(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_lt(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_le(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_first(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey3_iter_gt(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey3_iter_ge(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey3_iter_equal(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey3_iter_last(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey3_iter_lt(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey3_iter_le(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey3_iter_first(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey3_cmp(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((DocumentOrderKey3*)obj)->priv->code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((DocumentOrderKey3*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->time - ((DocumentOrderKey3*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentOrderKey3_set_alias(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentOrderKey3*)self)->alias);
    Py_INCREF(obj);
    ((DocumentOrderKey3*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey3_set(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {
        memcpy(self->priv, ((DocumentOrderKey3*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->code, ((DocumentOrder*)obj)->priv->data.code);

        do_cpy(self->priv->date, ((DocumentOrder*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((DocumentOrder*)obj)->priv->data.time);

    }

    else 
    if ( PyList_Check(obj) ||  PyTuple_Check(obj) ) {
        int i;
        int size;
        size = PyList_Check(obj) ? PyList_Size(obj) : PyTuple_Size(obj);
        for ( i = 0; i < size; i++ ) {
            PyObject *value;
            value =  PyList_Check(obj) ? PyList_GetItem(obj, i) : PyTuple_GetItem(obj, i);
            switch (i) {

                case 0:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

    }

    else 
    if ( PyDict_Check(obj) ) {
        PyObject *fields;
        int i;
        
        fields = PyDict_Keys(obj);
        for ( i = 0; i < PyList_Size(fields); i++ ) {
            PyObject *name;
            PyObject *value;
            char *field_name;
            
            name = PyList_GetItem(fields, i);
            value = PyDict_GetItem(obj, name); 
            field_name = MyString_AsString(name);
            if ( field_name[0] == '\0' ) {
                do_log(LOG_ERR, "Dictionary key is not a string");
                return NULL;
            } 


            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentOrderKey3\"does not contain \"%s\"", field_name);
                return NULL;
            }
            do_free(field_name);
        }
    }
    
    else
    {
        do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey3_fields(DocumentOrderKey3* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentOrderKey3");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentOrderKey3_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey3_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey3_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef DocumentOrderKey3_getset[] = {

    {"code",(getter)DocumentOrderKey3_code},

    {"date",(getter)DocumentOrderKey3_date},

    {"date_raw",(getter)DocumentOrderKey3_date_raw},

    {"time",(getter)DocumentOrderKey3_time},

    {"time_raw",(getter)DocumentOrderKey3_time_raw},

    {NULL}
};

static PyMethodDef DocumentOrderKey3_methods[] = {
    {"set", (PyCFunction)DocumentOrderKey3_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentOrderKey3_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentOrderKey3_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentOrderKey3_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)DocumentOrderKey3_set_code, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_set_code"},

    {"set_date", (PyCFunction)DocumentOrderKey3_set_date, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_set_date"},

    {"set_time", (PyCFunction)DocumentOrderKey3_set_time, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_set_time"},

    {"get_prev", (PyCFunction)DocumentOrderKey3_prev, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_prev"},

    {"get_gt", (PyCFunction)DocumentOrderKey3_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_gt"},

    {"get_next", (PyCFunction)DocumentOrderKey3_next, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_next"},

    {"get_ge", (PyCFunction)DocumentOrderKey3_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_ge"},

    {"get_equal", (PyCFunction)DocumentOrderKey3_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_equal"},

    {"get_last", (PyCFunction)DocumentOrderKey3_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_last"},

    {"get_lt", (PyCFunction)DocumentOrderKey3_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_lt"},

    {"get_le", (PyCFunction)DocumentOrderKey3_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_le"},

    {"get_first", (PyCFunction)DocumentOrderKey3_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_first"},

    {"gets_gt", (PyCFunction)DocumentOrderKey3_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentOrderKey3_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentOrderKey3_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_iter_equal"},

    {"gets_last", (PyCFunction)DocumentOrderKey3_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_iter_last"},

    {"gets_lt", (PyCFunction)DocumentOrderKey3_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_iter_lt"},

    {"gets_le", (PyCFunction)DocumentOrderKey3_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_iter_le"},

    {"gets_first", (PyCFunction)DocumentOrderKey3_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey3_iter_first"},

    {NULL}
};

static PyTypeObject DocumentOrderKey3Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentOrderKey3",             /*tp_name*/
    sizeof(DocumentOrderKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentOrderKey3_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey3_methods,             /* tp_methods */
    0,
    DocumentOrderKey3_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey3_new,                 /* tp_new */
};
PyTypeObject *getDocumentOrderKey3Type()
{
    return &DocumentOrderKey3Type_;
}

static void DocumentOrderKey4_dealloc(PyObject* self)
{
    do_free(((DocumentOrderKey4*)self)->priv);
    Py_DECREF(((DocumentOrderKey4*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentOrderKey4 free\n");
}
static PyObject *DocumentOrderKey4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentOrderKey4 new\n");
    DocumentOrderKey4 *self;
    self = PyObject_NEW(DocumentOrderKey4, type);
    //self = (DocumentOrderKey4 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_order_key4_t));
    return (PyObject *)self;
}
static int DocumentOrderKey4_init(DocumentOrderKey4 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentOrderKey4 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->sklad);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentOrderKey4_clone(DocumentOrderKey4 *self)
{
    DocumentOrderKey4 *result;
    result = (DocumentOrderKey4*)PyObject_CallFunction((PyObject*)getDocumentOrderKey4Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentOrderKey4_store(DocumentOrderKey4* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentOrderKey4_set_store(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey4_date(DocumentOrderKey4* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *DocumentOrderKey4_date_raw(DocumentOrderKey4* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *DocumentOrderKey4_set_date(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            do_date_set(&self->priv->date, tm);
        }
    }
    /*do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey4_time(DocumentOrderKey4* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *DocumentOrderKey4_time_raw(DocumentOrderKey4* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *DocumentOrderKey4_set_time(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->time = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             do_time_set(&self->priv->time, tm);
        }
    }
    /*do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentOrderKey4_prev(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_gt(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_next(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_ge(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_equal(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_last(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_lt(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_le(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_first(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentOrderKey4_iter_gt(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey4_iter_ge(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey4_iter_equal(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey4_iter_last(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey4_iter_lt(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey4_iter_le(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey4_iter_first(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_order_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentOrderKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_order_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentOrderKey4_cmp(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->sklad, ((DocumentOrderKey4*)obj)->priv->sklad);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((DocumentOrderKey4*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->time - ((DocumentOrderKey4*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentOrderKey4_set_alias(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentOrderKey4*)self)->alias);
    Py_INCREF(obj);
    ((DocumentOrderKey4*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey4_set(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {
        memcpy(self->priv, ((DocumentOrderKey4*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->sklad, ((DocumentOrder*)obj)->priv->data.sklad);

        do_cpy(self->priv->date, ((DocumentOrder*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((DocumentOrder*)obj)->priv->data.time);

    }

    else 
    if ( PyList_Check(obj) ||  PyTuple_Check(obj) ) {
        int i;
        int size;
        size = PyList_Check(obj) ? PyList_Size(obj) : PyTuple_Size(obj);
        for ( i = 0; i < size; i++ ) {
            PyObject *value;
            value =  PyList_Check(obj) ? PyList_GetItem(obj, i) : PyTuple_GetItem(obj, i);
            switch (i) {

                case 0:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->sklad, ((Document*)obj)->priv->data.sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

        do_cpy(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->sklad, ((DocumentKey3*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

    }

    else 
    if ( PyDict_Check(obj) ) {
        PyObject *fields;
        int i;
        
        fields = PyDict_Keys(obj);
        for ( i = 0; i < PyList_Size(fields); i++ ) {
            PyObject *name;
            PyObject *value;
            char *field_name;
            
            name = PyList_GetItem(fields, i);
            value = PyDict_GetItem(obj, name); 
            field_name = MyString_AsString(name);
            if ( field_name[0] == '\0' ) {
                do_log(LOG_ERR, "Dictionary key is not a string");
                return NULL;
            } 


            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentOrderKey4\"does not contain \"%s\"", field_name);
                return NULL;
            }
            do_free(field_name);
        }
    }
    
    else
    {
        do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentOrderKey4_fields(DocumentOrderKey4* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentOrderKey4");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentOrderKey4_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey4_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentOrderKey4_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef DocumentOrderKey4_getset[] = {

    {"store",(getter)DocumentOrderKey4_store},

    {"date",(getter)DocumentOrderKey4_date},

    {"date_raw",(getter)DocumentOrderKey4_date_raw},

    {"time",(getter)DocumentOrderKey4_time},

    {"time_raw",(getter)DocumentOrderKey4_time_raw},

    {NULL}
};

static PyMethodDef DocumentOrderKey4_methods[] = {
    {"set", (PyCFunction)DocumentOrderKey4_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentOrderKey4_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentOrderKey4_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentOrderKey4_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_store", (PyCFunction)DocumentOrderKey4_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_set_store"},

    {"set_date", (PyCFunction)DocumentOrderKey4_set_date, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_set_date"},

    {"set_time", (PyCFunction)DocumentOrderKey4_set_time, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_set_time"},

    {"get_prev", (PyCFunction)DocumentOrderKey4_prev, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_prev"},

    {"get_gt", (PyCFunction)DocumentOrderKey4_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_gt"},

    {"get_next", (PyCFunction)DocumentOrderKey4_next, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_next"},

    {"get_ge", (PyCFunction)DocumentOrderKey4_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_ge"},

    {"get_equal", (PyCFunction)DocumentOrderKey4_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_equal"},

    {"get_last", (PyCFunction)DocumentOrderKey4_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_last"},

    {"get_lt", (PyCFunction)DocumentOrderKey4_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_lt"},

    {"get_le", (PyCFunction)DocumentOrderKey4_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_le"},

    {"get_first", (PyCFunction)DocumentOrderKey4_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_first"},

    {"gets_gt", (PyCFunction)DocumentOrderKey4_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentOrderKey4_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentOrderKey4_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_iter_equal"},

    {"gets_last", (PyCFunction)DocumentOrderKey4_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_iter_last"},

    {"gets_lt", (PyCFunction)DocumentOrderKey4_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_iter_lt"},

    {"gets_le", (PyCFunction)DocumentOrderKey4_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_iter_le"},

    {"gets_first", (PyCFunction)DocumentOrderKey4_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentOrderKey4_iter_first"},

    {NULL}
};

static PyTypeObject DocumentOrderKey4Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentOrderKey4",             /*tp_name*/
    sizeof(DocumentOrderKey4),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentOrderKey4_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentOrderKey4_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentOrderKey4 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentOrderKey4_methods,             /* tp_methods */
    0,
    DocumentOrderKey4_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentOrderKey4_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentOrderKey4_new,                 /* tp_new */
};
PyTypeObject *getDocumentOrderKey4Type()
{
    return &DocumentOrderKey4Type_;
}
