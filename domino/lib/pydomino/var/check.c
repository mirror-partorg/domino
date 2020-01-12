
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "check.h"

static void Check_dealloc(PyObject* self)
{
    do_free(((Check*)self)->priv);
    Py_DECREF(((Check*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Check free\n");
}
static PyObject *Check_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Check new\n");
    Check *self;
    self = PyObject_NEW(Check, type);
    //self = (Check *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(check_rec_t));
    return (PyObject *)self;
}
static int Check_init(Check *self, PyObject *args, PyObject *kwds)
{
    //LOG("Check init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.product_code);

    do_text_set_empty(self->priv->data.bar_code);

    do_text_set_empty(self->priv->data.operation);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Check_clone(Check *self)
{
    Check *result;
    result = (Check*)PyObject_CallFunction((PyObject*)getCheckType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Check_cash(Check* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.cash);

    return result;
}

static PyObject *Check_set_cash(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.cash");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.cash = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.cash = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.cash");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.cash);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_shift(Check* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.shift);

    return result;
}

static PyObject *Check_set_shift(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.shift");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.shift = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.shift = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.shift");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.shift);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_check(Check* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.check);

    return result;
}

static PyObject *Check_set_check(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.check");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.check = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.check = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.check");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.check);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_position(Check* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.position);

    return result;
}

static PyObject *Check_set_position(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.position");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.position = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.position = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.position");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.position);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_date(Check* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Check_date_raw(Check* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *Check_set_date(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->data.date = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_time(Check* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Check_time_raw(Check* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.time);

    return result;
}

static PyObject *Check_set_time(Check* self, PyObject *args, PyObject *kwds)
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
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time");
            return NULL;
        }
        if ( PyTime_Check(value) ) {
            tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else {
            tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        self->priv->data.time = do_timetosql(tm);
    }
    /*do_sqltotime(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_sale_price(Check* self, void *unused)
{
    PyObject *result;

    result = PyFloat_FromDouble(self->priv->data.sale_price);

    return result;
}

static PyObject *Check_set_sale_price(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sale_price");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.sale_price = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.sale_price = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sale_price");
            return NULL;
        }
    }
    //result = PyFloat_FromDouble(self->priv->data.sale_price);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_quantity(Check* self, void *unused)
{
    PyObject *result;

    result = PyFloat_FromDouble(self->priv->data.quantity);

    return result;
}

static PyObject *Check_set_quantity(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.quantity");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.quantity = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.quantity = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.quantity");
            return NULL;
        }
    }
    //result = PyFloat_FromDouble(self->priv->data.quantity);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_code(Check* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.product_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Check_set_code(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.product_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.product_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.product_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_barcode(Check* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.bar_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Check_set_barcode(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.bar_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.bar_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.bar_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_cashier(Check* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.cashier);

    return result;
}

static PyObject *Check_set_cashier(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.cashier");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.cashier = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.cashier = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.cashier");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.cashier);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_valuta(Check* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.valuta);

    return result;
}

static PyObject *Check_set_valuta(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.valuta");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.valuta = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.valuta = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.valuta");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.valuta);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_operation(Check* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.operation);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Check_set_operation(Check* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.operation");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.operation, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.operation);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Check_prev(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_gt(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_next(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_ge(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_equal(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_last(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_lt(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_le(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_first(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckKey0Type() )
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getCheckKey1Type() )
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getCheckKey2Type() )
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_iter_gt(Check* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckKey0Type() ) {
        key_cmp = (check_key0_t*)do_malloc(sizeof(check_key0_t));
        memcpy(key_cmp, ((CheckKey0*)key)->priv, sizeof(check_key0_t));
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getCheckKey1Type() ) {
        key_cmp = (check_key1_t*)do_malloc(sizeof(check_key1_t));
        memcpy(key_cmp, ((CheckKey1*)key)->priv, sizeof(check_key1_t));
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getCheckKey2Type() ) {
        key_cmp = (check_key2_t*)do_malloc(sizeof(check_key2_t));
        memcpy(key_cmp, ((CheckKey2*)key)->priv, sizeof(check_key2_t));
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->cash, 
                    ((CheckKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->shift, 
                    ((CheckKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->check, 
                    ((CheckKey0*)key)->priv->check))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->position, 
                    ((CheckKey0*)key)->priv->position))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->date, 
                    ((CheckKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->time, 
                    ((CheckKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->date, 
                    ((CheckKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->time, 
                    ((CheckKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->product_code, 
                    ((CheckKey2*)key)->priv->product_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->date, 
                    ((CheckKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->time, 
                    ((CheckKey2*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->cash, 
                    ((CheckKey2*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->shift, 
                    ((CheckKey2*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->check, 
                    ((CheckKey2*)key)->priv->check))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Check_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
            status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
            status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
            status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *Check_iter_ge(Check* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckKey0Type() ) {
        key_cmp = (check_key0_t*)do_malloc(sizeof(check_key0_t));
        memcpy(key_cmp, ((CheckKey0*)key)->priv, sizeof(check_key0_t));
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getCheckKey1Type() ) {
        key_cmp = (check_key1_t*)do_malloc(sizeof(check_key1_t));
        memcpy(key_cmp, ((CheckKey1*)key)->priv, sizeof(check_key1_t));
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getCheckKey2Type() ) {
        key_cmp = (check_key2_t*)do_malloc(sizeof(check_key2_t));
        memcpy(key_cmp, ((CheckKey2*)key)->priv, sizeof(check_key2_t));
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->cash, 
                    ((CheckKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->shift, 
                    ((CheckKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->check, 
                    ((CheckKey0*)key)->priv->check))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->position, 
                    ((CheckKey0*)key)->priv->position))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->date, 
                    ((CheckKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->time, 
                    ((CheckKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->date, 
                    ((CheckKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->time, 
                    ((CheckKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->product_code, 
                    ((CheckKey2*)key)->priv->product_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->date, 
                    ((CheckKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->time, 
                    ((CheckKey2*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->cash, 
                    ((CheckKey2*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->shift, 
                    ((CheckKey2*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->check, 
                    ((CheckKey2*)key)->priv->check))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Check_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
            status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
            status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
            status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *Check_iter_equal(Check* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckKey0Type() ) {
        key_cmp = (check_key0_t*)do_malloc(sizeof(check_key0_t));
        memcpy(key_cmp, ((CheckKey0*)key)->priv, sizeof(check_key0_t));
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getCheckKey1Type() ) {
        key_cmp = (check_key1_t*)do_malloc(sizeof(check_key1_t));
        memcpy(key_cmp, ((CheckKey1*)key)->priv, sizeof(check_key1_t));
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getCheckKey2Type() ) {
        key_cmp = (check_key2_t*)do_malloc(sizeof(check_key2_t));
        memcpy(key_cmp, ((CheckKey2*)key)->priv, sizeof(check_key2_t));
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->cash, 
                    ((CheckKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->shift, 
                    ((CheckKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->check, 
                    ((CheckKey0*)key)->priv->check))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->position, 
                    ((CheckKey0*)key)->priv->position))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->date, 
                    ((CheckKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->time, 
                    ((CheckKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->date, 
                    ((CheckKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->time, 
                    ((CheckKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->product_code, 
                    ((CheckKey2*)key)->priv->product_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->date, 
                    ((CheckKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->time, 
                    ((CheckKey2*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->cash, 
                    ((CheckKey2*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->shift, 
                    ((CheckKey2*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->check, 
                    ((CheckKey2*)key)->priv->check))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Check_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
            status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
            status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
            status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *Check_iter_last(Check* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckKey0Type() ) {
        key_cmp = (check_key0_t*)do_malloc(sizeof(check_key0_t));
        memcpy(key_cmp, ((CheckKey0*)key)->priv, sizeof(check_key0_t));
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getCheckKey1Type() ) {
        key_cmp = (check_key1_t*)do_malloc(sizeof(check_key1_t));
        memcpy(key_cmp, ((CheckKey1*)key)->priv, sizeof(check_key1_t));
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getCheckKey2Type() ) {
        key_cmp = (check_key2_t*)do_malloc(sizeof(check_key2_t));
        memcpy(key_cmp, ((CheckKey2*)key)->priv, sizeof(check_key2_t));
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->cash, 
                    ((CheckKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->shift, 
                    ((CheckKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->check, 
                    ((CheckKey0*)key)->priv->check))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->position, 
                    ((CheckKey0*)key)->priv->position))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->date, 
                    ((CheckKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->time, 
                    ((CheckKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->date, 
                    ((CheckKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->time, 
                    ((CheckKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->product_code, 
                    ((CheckKey2*)key)->priv->product_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->date, 
                    ((CheckKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->time, 
                    ((CheckKey2*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->cash, 
                    ((CheckKey2*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->shift, 
                    ((CheckKey2*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->check, 
                    ((CheckKey2*)key)->priv->check))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Check_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
            status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
            status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
            status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Check_iter_lt(Check* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckKey0Type() ) {
        key_cmp = (check_key0_t*)do_malloc(sizeof(check_key0_t));
        memcpy(key_cmp, ((CheckKey0*)key)->priv, sizeof(check_key0_t));
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getCheckKey1Type() ) {
        key_cmp = (check_key1_t*)do_malloc(sizeof(check_key1_t));
        memcpy(key_cmp, ((CheckKey1*)key)->priv, sizeof(check_key1_t));
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getCheckKey2Type() ) {
        key_cmp = (check_key2_t*)do_malloc(sizeof(check_key2_t));
        memcpy(key_cmp, ((CheckKey2*)key)->priv, sizeof(check_key2_t));
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->cash, 
                    ((CheckKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->shift, 
                    ((CheckKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->check, 
                    ((CheckKey0*)key)->priv->check))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->position, 
                    ((CheckKey0*)key)->priv->position))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->date, 
                    ((CheckKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->time, 
                    ((CheckKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->date, 
                    ((CheckKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->time, 
                    ((CheckKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->product_code, 
                    ((CheckKey2*)key)->priv->product_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->date, 
                    ((CheckKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->time, 
                    ((CheckKey2*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->cash, 
                    ((CheckKey2*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->shift, 
                    ((CheckKey2*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->check, 
                    ((CheckKey2*)key)->priv->check))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Check_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
            status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
            status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
            status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Check_iter_le(Check* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckKey0Type() ) {
        key_cmp = (check_key0_t*)do_malloc(sizeof(check_key0_t));
        memcpy(key_cmp, ((CheckKey0*)key)->priv, sizeof(check_key0_t));
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getCheckKey1Type() ) {
        key_cmp = (check_key1_t*)do_malloc(sizeof(check_key1_t));
        memcpy(key_cmp, ((CheckKey1*)key)->priv, sizeof(check_key1_t));
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getCheckKey2Type() ) {
        key_cmp = (check_key2_t*)do_malloc(sizeof(check_key2_t));
        memcpy(key_cmp, ((CheckKey2*)key)->priv, sizeof(check_key2_t));
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->cash, 
                    ((CheckKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->shift, 
                    ((CheckKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->check, 
                    ((CheckKey0*)key)->priv->check))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->position, 
                    ((CheckKey0*)key)->priv->position))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->date, 
                    ((CheckKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->time, 
                    ((CheckKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->date, 
                    ((CheckKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->time, 
                    ((CheckKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->product_code, 
                    ((CheckKey2*)key)->priv->product_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->date, 
                    ((CheckKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->time, 
                    ((CheckKey2*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->cash, 
                    ((CheckKey2*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->shift, 
                    ((CheckKey2*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->check, 
                    ((CheckKey2*)key)->priv->check))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Check_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
            status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
            status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
            status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Check_iter_first(Check* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckKey0Type() ) {
        key_cmp = (check_key0_t*)do_malloc(sizeof(check_key0_t));
        memcpy(key_cmp, ((CheckKey0*)key)->priv, sizeof(check_key0_t));
        status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getCheckKey1Type() ) {
        key_cmp = (check_key1_t*)do_malloc(sizeof(check_key1_t));
        memcpy(key_cmp, ((CheckKey1*)key)->priv, sizeof(check_key1_t));
        status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getCheckKey2Type() ) {
        key_cmp = (check_key2_t*)do_malloc(sizeof(check_key2_t));
        memcpy(key_cmp, ((CheckKey2*)key)->priv, sizeof(check_key2_t));
        status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->cash, 
                    ((CheckKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->shift, 
                    ((CheckKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->check, 
                    ((CheckKey0*)key)->priv->check))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->position, 
                    ((CheckKey0*)key)->priv->position))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->date, 
                    ((CheckKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key0_t*)key_cmp)->time, 
                    ((CheckKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->date, 
                    ((CheckKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key1_t*)key_cmp)->time, 
                    ((CheckKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->product_code, 
                    ((CheckKey2*)key)->priv->product_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->date, 
                    ((CheckKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->time, 
                    ((CheckKey2*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->cash, 
                    ((CheckKey2*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->shift, 
                    ((CheckKey2*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((check_key2_t*)key_cmp)->check, 
                    ((CheckKey2*)key)->priv->check))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Check_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckKey0Type() ) {
            status = do_check_get0(self->alias->alias, self->priv, ((CheckKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckKey1Type() ) {
            status = do_check_get1(self->alias->alias, self->priv, ((CheckKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckKey2Type() ) {
            status = do_check_get2(self->alias->alias, self->priv, ((CheckKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *Check_update(Check* self)
{
    int status;
    status = do_check_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_insert(Check* self)
{
    int status;
    status = do_check_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_delete(Check* self)
{
    int status;
    status = do_check_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Check_set_alias(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Check*)self)->alias);
    Py_INCREF(obj);
    ((Check*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Check_set(Check* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getCheckType() ) {
        memcpy(self->priv, ((Check*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getCheckKey0Type() ) {

        do_cpy(self->priv->data.cash, ((CheckKey0*)obj)->priv->cash);

        do_cpy(self->priv->data.shift, ((CheckKey0*)obj)->priv->shift);

        do_cpy(self->priv->data.check, ((CheckKey0*)obj)->priv->check);

        do_cpy(self->priv->data.position, ((CheckKey0*)obj)->priv->position);

        do_cpy(self->priv->data.date, ((CheckKey0*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((CheckKey0*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getCheckKey1Type() ) {

        do_cpy(self->priv->data.date, ((CheckKey1*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((CheckKey1*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getCheckKey2Type() ) {

        do_cpy(self->priv->data.cash, ((CheckKey2*)obj)->priv->cash);

        do_cpy(self->priv->data.shift, ((CheckKey2*)obj)->priv->shift);

        do_cpy(self->priv->data.check, ((CheckKey2*)obj)->priv->check);

        do_cpy(self->priv->data.date, ((CheckKey2*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((CheckKey2*)obj)->priv->time);

        do_cpy(self->priv->data.product_code, ((CheckKey2*)obj)->priv->product_code);

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
            if ( !strcmp("cash", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_cash", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("shift", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_shift", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("check", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_check", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("position", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_position", "O", value) )
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
            if ( !strcmp("sale_price", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sale_price", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("quantity", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_quantity", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("barcode", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_barcode", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("cashier", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_cashier", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("valuta", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_valuta", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("operation", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_operation", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Check\"does not contain \"%s\"", field_name);
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

static PyObject *Check_fields(Check* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Check");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Check_cash(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("cash", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_shift(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("shift", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_check(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("check", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_position(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("position", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_date(self,NULL);
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

    value =  Check_time(self,NULL);
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

    value =  Check_sale_price(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("sale_price", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_quantity(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("quantity", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_code(self,NULL);
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

    value =  Check_barcode(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("barcode", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_cashier(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("cashier", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_valuta(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("valuta", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Check_operation(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("operation", value);
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

static PyGetSetDef Check_getset[] = {

    {"cash",(getter)Check_cash},

    {"shift",(getter)Check_shift},

    {"check",(getter)Check_check},

    {"position",(getter)Check_position},

    {"date",(getter)Check_date},

    {"date_raw",(getter)Check_date_raw},

    {"time",(getter)Check_time},

    {"time_raw",(getter)Check_time_raw},

    {"sale_price",(getter)Check_sale_price},

    {"quantity",(getter)Check_quantity},

    {"code",(getter)Check_code},

    {"barcode",(getter)Check_barcode},

    {"cashier",(getter)Check_cashier},

    {"valuta",(getter)Check_valuta},

    {"operation",(getter)Check_operation},

    {NULL}
};

static PyMethodDef Check_methods[] = {
    {"set", (PyCFunction)Check_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Check_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Check_clone, METH_NOARGS, "clone"},

    {"set_cash", (PyCFunction)Check_set_cash, METH_VARARGS|METH_KEYWORDS, "Check_set_cash"},

    {"set_shift", (PyCFunction)Check_set_shift, METH_VARARGS|METH_KEYWORDS, "Check_set_shift"},

    {"set_check", (PyCFunction)Check_set_check, METH_VARARGS|METH_KEYWORDS, "Check_set_check"},

    {"set_position", (PyCFunction)Check_set_position, METH_VARARGS|METH_KEYWORDS, "Check_set_position"},

    {"set_date", (PyCFunction)Check_set_date, METH_VARARGS|METH_KEYWORDS, "Check_set_date"},

    {"set_time", (PyCFunction)Check_set_time, METH_VARARGS|METH_KEYWORDS, "Check_set_time"},

    {"set_sale_price", (PyCFunction)Check_set_sale_price, METH_VARARGS|METH_KEYWORDS, "Check_set_sale_price"},

    {"set_quantity", (PyCFunction)Check_set_quantity, METH_VARARGS|METH_KEYWORDS, "Check_set_quantity"},

    {"set_code", (PyCFunction)Check_set_code, METH_VARARGS|METH_KEYWORDS, "Check_set_code"},

    {"set_barcode", (PyCFunction)Check_set_barcode, METH_VARARGS|METH_KEYWORDS, "Check_set_barcode"},

    {"set_cashier", (PyCFunction)Check_set_cashier, METH_VARARGS|METH_KEYWORDS, "Check_set_cashier"},

    {"set_valuta", (PyCFunction)Check_set_valuta, METH_VARARGS|METH_KEYWORDS, "Check_set_valuta"},

    {"set_operation", (PyCFunction)Check_set_operation, METH_VARARGS|METH_KEYWORDS, "Check_set_operation"},

    {"get_prev", (PyCFunction)Check_prev, METH_VARARGS|METH_KEYWORDS, "Check_prev"},

    {"get_gt", (PyCFunction)Check_gt, METH_VARARGS|METH_KEYWORDS, "Check_gt"},

    {"get_next", (PyCFunction)Check_next, METH_VARARGS|METH_KEYWORDS, "Check_next"},

    {"get_ge", (PyCFunction)Check_ge, METH_VARARGS|METH_KEYWORDS, "Check_ge"},

    {"get_equal", (PyCFunction)Check_equal, METH_VARARGS|METH_KEYWORDS, "Check_equal"},

    {"get_last", (PyCFunction)Check_last, METH_VARARGS|METH_KEYWORDS, "Check_last"},

    {"get_lt", (PyCFunction)Check_lt, METH_VARARGS|METH_KEYWORDS, "Check_lt"},

    {"get_le", (PyCFunction)Check_le, METH_VARARGS|METH_KEYWORDS, "Check_le"},

    {"get_first", (PyCFunction)Check_first, METH_VARARGS|METH_KEYWORDS, "Check_first"},

    {"gets_gt", (PyCFunction)Check_iter_gt, METH_VARARGS|METH_KEYWORDS, "Check_iter_gt"},

    {"gets_ge", (PyCFunction)Check_iter_ge, METH_VARARGS|METH_KEYWORDS, "Check_iter_ge"},

    {"gets_equal", (PyCFunction)Check_iter_equal, METH_VARARGS|METH_KEYWORDS, "Check_iter_equal"},

    {"gets_last", (PyCFunction)Check_iter_last, METH_VARARGS|METH_KEYWORDS, "Check_iter_last"},

    {"gets_lt", (PyCFunction)Check_iter_lt, METH_VARARGS|METH_KEYWORDS, "Check_iter_lt"},

    {"gets_le", (PyCFunction)Check_iter_le, METH_VARARGS|METH_KEYWORDS, "Check_iter_le"},

    {"gets_first", (PyCFunction)Check_iter_first, METH_VARARGS|METH_KEYWORDS, "Check_iter_first"},

    {"update", (PyCFunction)Check_update, METH_VARARGS|METH_KEYWORDS, "Check_update"},

    {"insert", (PyCFunction)Check_insert, METH_VARARGS|METH_KEYWORDS, "Check_insert"},

    {"delete", (PyCFunction)Check_delete, METH_VARARGS|METH_KEYWORDS, "Check_delete"},

    {NULL}
};

static PyTypeObject CheckType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Check",             /*tp_name*/
    sizeof(Check),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Check_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Check_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Check objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Check_methods,             /* tp_methods */
    0,
    Check_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Check_init,      /* tp_init */
    0,                         /* tp_alloc */
    Check_new,                 /* tp_new */
};
PyTypeObject *getCheckType()
{
    return &CheckType_;
}

static void CheckKey0_dealloc(PyObject* self)
{
    do_free(((CheckKey0*)self)->priv);
    Py_DECREF(((CheckKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("CheckKey0 free\n");
}
static PyObject *CheckKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("CheckKey0 new\n");
    CheckKey0 *self;
    self = PyObject_NEW(CheckKey0, type);
    //self = (CheckKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(check_key0_t));
    return (PyObject *)self;
}
static int CheckKey0_init(CheckKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("CheckKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    PyDateTime_IMPORT;
    return 0;
}

static PyObject *CheckKey0_clone(CheckKey0 *self)
{
    CheckKey0 *result;
    result = (CheckKey0*)PyObject_CallFunction((PyObject*)getCheckKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *CheckKey0_cash(CheckKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->cash);

    return result;
}

static PyObject *CheckKey0_set_cash(CheckKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set cash");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->cash = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->cash = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set cash");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->cash);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey0_shift(CheckKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->shift);

    return result;
}

static PyObject *CheckKey0_set_shift(CheckKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set shift");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->shift = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->shift = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set shift");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->shift);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey0_check(CheckKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->check);

    return result;
}

static PyObject *CheckKey0_set_check(CheckKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set check");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->check = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->check = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set check");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->check);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey0_position(CheckKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->position);

    return result;
}

static PyObject *CheckKey0_set_position(CheckKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set position");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->position = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->position = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set position");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->position);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey0_date(CheckKey0* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *CheckKey0_date_raw(CheckKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *CheckKey0_set_date(CheckKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->date = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey0_time(CheckKey0* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *CheckKey0_time_raw(CheckKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *CheckKey0_set_time(CheckKey0* self, PyObject *args, PyObject *kwds)
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
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
            return NULL;
        }
        if ( PyTime_Check(value) ) {
            tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else {
            tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        self->priv->time = do_timetosql(tm);
    }
    /*do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey0_prev(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_gt(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_next(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_ge(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_equal(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_last(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_lt(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_le(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_first(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey0_iter_gt(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.position, 
                 self->priv->position))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey0_iter_ge(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.position, 
                 self->priv->position))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey0_iter_equal(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.position, 
                 self->priv->position))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey0_iter_last(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.position, 
                 self->priv->position))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey0_iter_lt(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.position, 
                 self->priv->position))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey0_iter_le(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.position, 
                 self->priv->position))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey0_iter_first(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.position, 
                 self->priv->position))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey0_cmp(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getCheckKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->cash - ((CheckKey0*)obj)->priv->cash;
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->shift - ((CheckKey0*)obj)->priv->shift;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->check - ((CheckKey0*)obj)->priv->check;
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->position - ((CheckKey0*)obj)->priv->position;
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->date - ((CheckKey0*)obj)->priv->date;
       }

       if ( !res && depth >= 6 ) {
           res = self->priv->time - ((CheckKey0*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *CheckKey0_set_alias(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((CheckKey0*)self)->alias);
    Py_INCREF(obj);
    ((CheckKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *CheckKey0_set(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getCheckKey0Type() ) {
        memcpy(self->priv, ((CheckKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getCheckType() ) {

        do_cpy(self->priv->cash, ((Check*)obj)->priv->data.cash);

        do_cpy(self->priv->shift, ((Check*)obj)->priv->data.shift);

        do_cpy(self->priv->check, ((Check*)obj)->priv->data.check);

        do_cpy(self->priv->position, ((Check*)obj)->priv->data.position);

        do_cpy(self->priv->date, ((Check*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Check*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_cash", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_shift", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_check", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_position", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 5:
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
            if ( !strcmp("cash", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_cash", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("shift", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_shift", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("check", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_check", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("position", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_position", "O", value) )
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
                do_log(LOG_ERR, "Class \"CheckKey0\"does not contain \"%s\"", field_name);
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

static PyObject *CheckKey0_fields(CheckKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class CheckKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  CheckKey0_cash(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("cash", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  CheckKey0_shift(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("shift", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  CheckKey0_check(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("check", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  CheckKey0_position(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("position", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  CheckKey0_date(self,NULL);
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

    value =  CheckKey0_time(self,NULL);
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

static PyGetSetDef CheckKey0_getset[] = {

    {"cash",(getter)CheckKey0_cash},

    {"shift",(getter)CheckKey0_shift},

    {"check",(getter)CheckKey0_check},

    {"position",(getter)CheckKey0_position},

    {"date",(getter)CheckKey0_date},

    {"date_raw",(getter)CheckKey0_date_raw},

    {"time",(getter)CheckKey0_time},

    {"time_raw",(getter)CheckKey0_time_raw},

    {NULL}
};

static PyMethodDef CheckKey0_methods[] = {
    {"set", (PyCFunction)CheckKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)CheckKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)CheckKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)CheckKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_cash", (PyCFunction)CheckKey0_set_cash, METH_VARARGS|METH_KEYWORDS, "CheckKey0_set_cash"},

    {"set_shift", (PyCFunction)CheckKey0_set_shift, METH_VARARGS|METH_KEYWORDS, "CheckKey0_set_shift"},

    {"set_check", (PyCFunction)CheckKey0_set_check, METH_VARARGS|METH_KEYWORDS, "CheckKey0_set_check"},

    {"set_position", (PyCFunction)CheckKey0_set_position, METH_VARARGS|METH_KEYWORDS, "CheckKey0_set_position"},

    {"set_date", (PyCFunction)CheckKey0_set_date, METH_VARARGS|METH_KEYWORDS, "CheckKey0_set_date"},

    {"set_time", (PyCFunction)CheckKey0_set_time, METH_VARARGS|METH_KEYWORDS, "CheckKey0_set_time"},

    {"get_prev", (PyCFunction)CheckKey0_prev, METH_VARARGS|METH_KEYWORDS, "CheckKey0_prev"},

    {"get_gt", (PyCFunction)CheckKey0_gt, METH_VARARGS|METH_KEYWORDS, "CheckKey0_gt"},

    {"get_next", (PyCFunction)CheckKey0_next, METH_VARARGS|METH_KEYWORDS, "CheckKey0_next"},

    {"get_ge", (PyCFunction)CheckKey0_ge, METH_VARARGS|METH_KEYWORDS, "CheckKey0_ge"},

    {"get_equal", (PyCFunction)CheckKey0_equal, METH_VARARGS|METH_KEYWORDS, "CheckKey0_equal"},

    {"get_last", (PyCFunction)CheckKey0_last, METH_VARARGS|METH_KEYWORDS, "CheckKey0_last"},

    {"get_lt", (PyCFunction)CheckKey0_lt, METH_VARARGS|METH_KEYWORDS, "CheckKey0_lt"},

    {"get_le", (PyCFunction)CheckKey0_le, METH_VARARGS|METH_KEYWORDS, "CheckKey0_le"},

    {"get_first", (PyCFunction)CheckKey0_first, METH_VARARGS|METH_KEYWORDS, "CheckKey0_first"},

    {"gets_gt", (PyCFunction)CheckKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "CheckKey0_iter_gt"},

    {"gets_ge", (PyCFunction)CheckKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "CheckKey0_iter_ge"},

    {"gets_equal", (PyCFunction)CheckKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "CheckKey0_iter_equal"},

    {"gets_last", (PyCFunction)CheckKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "CheckKey0_iter_last"},

    {"gets_lt", (PyCFunction)CheckKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "CheckKey0_iter_lt"},

    {"gets_le", (PyCFunction)CheckKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "CheckKey0_iter_le"},

    {"gets_first", (PyCFunction)CheckKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "CheckKey0_iter_first"},

    {NULL}
};

static PyTypeObject CheckKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.CheckKey0",             /*tp_name*/
    sizeof(CheckKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)CheckKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckKey0_methods,             /* tp_methods */
    0,
    CheckKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckKey0_new,                 /* tp_new */
};
PyTypeObject *getCheckKey0Type()
{
    return &CheckKey0Type_;
}

static void CheckKey1_dealloc(PyObject* self)
{
    do_free(((CheckKey1*)self)->priv);
    Py_DECREF(((CheckKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("CheckKey1 free\n");
}
static PyObject *CheckKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("CheckKey1 new\n");
    CheckKey1 *self;
    self = PyObject_NEW(CheckKey1, type);
    //self = (CheckKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(check_key1_t));
    return (PyObject *)self;
}
static int CheckKey1_init(CheckKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("CheckKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    PyDateTime_IMPORT;
    return 0;
}

static PyObject *CheckKey1_clone(CheckKey1 *self)
{
    CheckKey1 *result;
    result = (CheckKey1*)PyObject_CallFunction((PyObject*)getCheckKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *CheckKey1_date(CheckKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *CheckKey1_date_raw(CheckKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *CheckKey1_set_date(CheckKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->date = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey1_time(CheckKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *CheckKey1_time_raw(CheckKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *CheckKey1_set_time(CheckKey1* self, PyObject *args, PyObject *kwds)
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
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
            return NULL;
        }
        if ( PyTime_Check(value) ) {
            tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else {
            tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        self->priv->time = do_timetosql(tm);
    }
    /*do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey1_prev(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_gt(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_next(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_ge(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_equal(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_last(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_lt(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_le(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_first(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey1_iter_gt(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey1_iter_ge(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey1_iter_equal(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey1_iter_last(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey1_iter_lt(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey1_iter_le(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey1_iter_first(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = CheckKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey1_cmp(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getCheckKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->date - ((CheckKey1*)obj)->priv->date;
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->time - ((CheckKey1*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *CheckKey1_set_alias(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((CheckKey1*)self)->alias);
    Py_INCREF(obj);
    ((CheckKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *CheckKey1_set(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getCheckKey1Type() ) {
        memcpy(self->priv, ((CheckKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getCheckType() ) {

        do_cpy(self->priv->date, ((Check*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Check*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 1:
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
                do_log(LOG_ERR, "Class \"CheckKey1\"does not contain \"%s\"", field_name);
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

static PyObject *CheckKey1_fields(CheckKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class CheckKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  CheckKey1_date(self,NULL);
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

    value =  CheckKey1_time(self,NULL);
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

static PyGetSetDef CheckKey1_getset[] = {

    {"date",(getter)CheckKey1_date},

    {"date_raw",(getter)CheckKey1_date_raw},

    {"time",(getter)CheckKey1_time},

    {"time_raw",(getter)CheckKey1_time_raw},

    {NULL}
};

static PyMethodDef CheckKey1_methods[] = {
    {"set", (PyCFunction)CheckKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)CheckKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)CheckKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)CheckKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_date", (PyCFunction)CheckKey1_set_date, METH_VARARGS|METH_KEYWORDS, "CheckKey1_set_date"},

    {"set_time", (PyCFunction)CheckKey1_set_time, METH_VARARGS|METH_KEYWORDS, "CheckKey1_set_time"},

    {"get_prev", (PyCFunction)CheckKey1_prev, METH_VARARGS|METH_KEYWORDS, "CheckKey1_prev"},

    {"get_gt", (PyCFunction)CheckKey1_gt, METH_VARARGS|METH_KEYWORDS, "CheckKey1_gt"},

    {"get_next", (PyCFunction)CheckKey1_next, METH_VARARGS|METH_KEYWORDS, "CheckKey1_next"},

    {"get_ge", (PyCFunction)CheckKey1_ge, METH_VARARGS|METH_KEYWORDS, "CheckKey1_ge"},

    {"get_equal", (PyCFunction)CheckKey1_equal, METH_VARARGS|METH_KEYWORDS, "CheckKey1_equal"},

    {"get_last", (PyCFunction)CheckKey1_last, METH_VARARGS|METH_KEYWORDS, "CheckKey1_last"},

    {"get_lt", (PyCFunction)CheckKey1_lt, METH_VARARGS|METH_KEYWORDS, "CheckKey1_lt"},

    {"get_le", (PyCFunction)CheckKey1_le, METH_VARARGS|METH_KEYWORDS, "CheckKey1_le"},

    {"get_first", (PyCFunction)CheckKey1_first, METH_VARARGS|METH_KEYWORDS, "CheckKey1_first"},

    {"gets_gt", (PyCFunction)CheckKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "CheckKey1_iter_gt"},

    {"gets_ge", (PyCFunction)CheckKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "CheckKey1_iter_ge"},

    {"gets_equal", (PyCFunction)CheckKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "CheckKey1_iter_equal"},

    {"gets_last", (PyCFunction)CheckKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "CheckKey1_iter_last"},

    {"gets_lt", (PyCFunction)CheckKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "CheckKey1_iter_lt"},

    {"gets_le", (PyCFunction)CheckKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "CheckKey1_iter_le"},

    {"gets_first", (PyCFunction)CheckKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "CheckKey1_iter_first"},

    {NULL}
};

static PyTypeObject CheckKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.CheckKey1",             /*tp_name*/
    sizeof(CheckKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)CheckKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckKey1_methods,             /* tp_methods */
    0,
    CheckKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckKey1_new,                 /* tp_new */
};
PyTypeObject *getCheckKey1Type()
{
    return &CheckKey1Type_;
}

static void CheckKey2_dealloc(PyObject* self)
{
    do_free(((CheckKey2*)self)->priv);
    Py_DECREF(((CheckKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("CheckKey2 free\n");
}
static PyObject *CheckKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("CheckKey2 new\n");
    CheckKey2 *self;
    self = PyObject_NEW(CheckKey2, type);
    //self = (CheckKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(check_key2_t));
    return (PyObject *)self;
}
static int CheckKey2_init(CheckKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("CheckKey2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->product_code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *CheckKey2_clone(CheckKey2 *self)
{
    CheckKey2 *result;
    result = (CheckKey2*)PyObject_CallFunction((PyObject*)getCheckKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *CheckKey2_code(CheckKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->product_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *CheckKey2_set_code(CheckKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set product_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->product_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->product_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey2_date(CheckKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *CheckKey2_date_raw(CheckKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *CheckKey2_set_date(CheckKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->date = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey2_time(CheckKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *CheckKey2_time_raw(CheckKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *CheckKey2_set_time(CheckKey2* self, PyObject *args, PyObject *kwds)
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
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
            return NULL;
        }
        if ( PyTime_Check(value) ) {
            tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else {
            tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        self->priv->time = do_timetosql(tm);
    }
    /*do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey2_cash(CheckKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->cash);

    return result;
}

static PyObject *CheckKey2_set_cash(CheckKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set cash");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->cash = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->cash = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set cash");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->cash);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey2_shift(CheckKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->shift);

    return result;
}

static PyObject *CheckKey2_set_shift(CheckKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set shift");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->shift = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->shift = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set shift");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->shift);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey2_check(CheckKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->check);

    return result;
}

static PyObject *CheckKey2_set_check(CheckKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set check");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->check = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->check = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set check");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->check);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckKey2_prev(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_gt(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_next(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_ge(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_equal(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_last(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_lt(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_le(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_first(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_check_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckKey2_iter_gt(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.product_code, 
                 self->priv->product_code))
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

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey2_iter_ge(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.product_code, 
                 self->priv->product_code))
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

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey2_iter_equal(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.product_code, 
                 self->priv->product_code))
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

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey2_iter_last(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.product_code, 
                 self->priv->product_code))
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

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey2_iter_lt(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.product_code, 
                 self->priv->product_code))
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

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey2_iter_le(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.product_code, 
                 self->priv->product_code))
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

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey2_iter_first(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    check_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_check_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.product_code, 
                 self->priv->product_code))
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

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.shift, 
                 self->priv->shift))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_check_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckKey2_cmp(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getCheckKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->product_code, ((CheckKey2*)obj)->priv->product_code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((CheckKey2*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->time - ((CheckKey2*)obj)->priv->time;
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->cash - ((CheckKey2*)obj)->priv->cash;
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->shift - ((CheckKey2*)obj)->priv->shift;
       }

       if ( !res && depth >= 6 ) {
           res = self->priv->check - ((CheckKey2*)obj)->priv->check;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *CheckKey2_set_alias(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((CheckKey2*)self)->alias);
    Py_INCREF(obj);
    ((CheckKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *CheckKey2_set(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getCheckKey2Type() ) {
        memcpy(self->priv, ((CheckKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getCheckType() ) {

        do_cpy(self->priv->product_code, ((Check*)obj)->priv->data.product_code);

        do_cpy(self->priv->date, ((Check*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Check*)obj)->priv->data.time);

        do_cpy(self->priv->cash, ((Check*)obj)->priv->data.cash);

        do_cpy(self->priv->shift, ((Check*)obj)->priv->data.shift);

        do_cpy(self->priv->check, ((Check*)obj)->priv->data.check);

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

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_cash", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_shift", "O", value) )
                        return NULL;
                    break;

                case 5:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_check", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
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

            else
            if ( !strcmp("cash", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_cash", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("shift", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_shift", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("check", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_check", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"CheckKey2\"does not contain \"%s\"", field_name);
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

static PyObject *CheckKey2_fields(CheckKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class CheckKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  CheckKey2_code(self,NULL);
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

    value =  CheckKey2_date(self,NULL);
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

    value =  CheckKey2_time(self,NULL);
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

    value =  CheckKey2_cash(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("cash", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  CheckKey2_shift(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("shift", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  CheckKey2_check(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("check", value);
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

static PyGetSetDef CheckKey2_getset[] = {

    {"code",(getter)CheckKey2_code},

    {"date",(getter)CheckKey2_date},

    {"date_raw",(getter)CheckKey2_date_raw},

    {"time",(getter)CheckKey2_time},

    {"time_raw",(getter)CheckKey2_time_raw},

    {"cash",(getter)CheckKey2_cash},

    {"shift",(getter)CheckKey2_shift},

    {"check",(getter)CheckKey2_check},

    {NULL}
};

static PyMethodDef CheckKey2_methods[] = {
    {"set", (PyCFunction)CheckKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)CheckKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)CheckKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)CheckKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)CheckKey2_set_code, METH_VARARGS|METH_KEYWORDS, "CheckKey2_set_code"},

    {"set_date", (PyCFunction)CheckKey2_set_date, METH_VARARGS|METH_KEYWORDS, "CheckKey2_set_date"},

    {"set_time", (PyCFunction)CheckKey2_set_time, METH_VARARGS|METH_KEYWORDS, "CheckKey2_set_time"},

    {"set_cash", (PyCFunction)CheckKey2_set_cash, METH_VARARGS|METH_KEYWORDS, "CheckKey2_set_cash"},

    {"set_shift", (PyCFunction)CheckKey2_set_shift, METH_VARARGS|METH_KEYWORDS, "CheckKey2_set_shift"},

    {"set_check", (PyCFunction)CheckKey2_set_check, METH_VARARGS|METH_KEYWORDS, "CheckKey2_set_check"},

    {"get_prev", (PyCFunction)CheckKey2_prev, METH_VARARGS|METH_KEYWORDS, "CheckKey2_prev"},

    {"get_gt", (PyCFunction)CheckKey2_gt, METH_VARARGS|METH_KEYWORDS, "CheckKey2_gt"},

    {"get_next", (PyCFunction)CheckKey2_next, METH_VARARGS|METH_KEYWORDS, "CheckKey2_next"},

    {"get_ge", (PyCFunction)CheckKey2_ge, METH_VARARGS|METH_KEYWORDS, "CheckKey2_ge"},

    {"get_equal", (PyCFunction)CheckKey2_equal, METH_VARARGS|METH_KEYWORDS, "CheckKey2_equal"},

    {"get_last", (PyCFunction)CheckKey2_last, METH_VARARGS|METH_KEYWORDS, "CheckKey2_last"},

    {"get_lt", (PyCFunction)CheckKey2_lt, METH_VARARGS|METH_KEYWORDS, "CheckKey2_lt"},

    {"get_le", (PyCFunction)CheckKey2_le, METH_VARARGS|METH_KEYWORDS, "CheckKey2_le"},

    {"get_first", (PyCFunction)CheckKey2_first, METH_VARARGS|METH_KEYWORDS, "CheckKey2_first"},

    {"gets_gt", (PyCFunction)CheckKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "CheckKey2_iter_gt"},

    {"gets_ge", (PyCFunction)CheckKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "CheckKey2_iter_ge"},

    {"gets_equal", (PyCFunction)CheckKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "CheckKey2_iter_equal"},

    {"gets_last", (PyCFunction)CheckKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "CheckKey2_iter_last"},

    {"gets_lt", (PyCFunction)CheckKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "CheckKey2_iter_lt"},

    {"gets_le", (PyCFunction)CheckKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "CheckKey2_iter_le"},

    {"gets_first", (PyCFunction)CheckKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "CheckKey2_iter_first"},

    {NULL}
};

static PyTypeObject CheckKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.CheckKey2",             /*tp_name*/
    sizeof(CheckKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)CheckKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckKey2_methods,             /* tp_methods */
    0,
    CheckKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckKey2_new,                 /* tp_new */
};
PyTypeObject *getCheckKey2Type()
{
    return &CheckKey2Type_;
}
