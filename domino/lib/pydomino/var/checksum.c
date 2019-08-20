
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "checksum.h"

static void CheckSum_dealloc(PyObject* self)
{
    do_free(((CheckSum*)self)->priv);
    Py_DECREF(((CheckSum*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("CheckSum free\n");
}
static PyObject *CheckSum_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("CheckSum new\n");
    CheckSum *self;
    self = PyObject_NEW(CheckSum, type);
    //self = (CheckSum *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(checksum_rec_t));
    return (PyObject *)self;
}
static int CheckSum_init(CheckSum *self, PyObject *args, PyObject *kwds)
{
    //LOG("CheckSum init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.operation);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *CheckSum_clone(CheckSum *self)
{
    CheckSum *result;
    result = (CheckSum*)PyObject_CallFunction((PyObject*)getCheckSumType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *CheckSum_cash(CheckSum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.cash);

    return result;
}

static PyObject *CheckSum_set_cash(CheckSum* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSum_shift(CheckSum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.shift);

    return result;
}

static PyObject *CheckSum_set_shift(CheckSum* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSum_check(CheckSum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.check);

    return result;
}

static PyObject *CheckSum_set_check(CheckSum* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSum_date(CheckSum* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *CheckSum_date_raw(CheckSum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *CheckSum_set_date(CheckSum* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSum_time(CheckSum* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *CheckSum_time_raw(CheckSum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.time);

    return result;
}

static PyObject *CheckSum_set_time(CheckSum* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSum_cashier(CheckSum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.cashier);

    return result;
}

static PyObject *CheckSum_set_cashier(CheckSum* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSum_valuta(CheckSum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.valuta);

    return result;
}

static PyObject *CheckSum_set_valuta(CheckSum* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSum_operation(CheckSum* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.operation);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *CheckSum_set_operation(CheckSum* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSum_sale_sum(CheckSum* self, void *unused)
{
    PyObject *result;

    result = PyFloat_FromDouble(self->priv->data.sale_sum);

    return result;
}

static PyObject *CheckSum_set_sale_sum(CheckSum* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sale_sum");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.sale_sum = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.sale_sum = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sale_sum");
            return NULL;
        }
    }
    //result = PyFloat_FromDouble(self->priv->data.sale_sum);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckSum_total(CheckSum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.total);

    return result;
}

static PyObject *CheckSum_set_total(CheckSum* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.total");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.total = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.total = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.total");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.total);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *CheckSum_gt(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_last(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_next(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_le(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_lt(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_equal(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_ge(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_prev(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_first(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getCheckSumKey0Type() )
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() )
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_iter_gt(CheckSum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
        key_cmp = (checksum_key0_t*)do_malloc(sizeof(checksum_key0_t));
        memcpy(key_cmp, ((CheckSumKey0*)key)->priv, sizeof(checksum_key0_t));
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
        key_cmp = (checksum_key1_t*)do_malloc(sizeof(checksum_key1_t));
        memcpy(key_cmp, ((CheckSumKey1*)key)->priv, sizeof(checksum_key1_t));
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->cash, 
                    ((CheckSumKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->shift, 
                    ((CheckSumKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->date, 
                    ((CheckSumKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->time, 
                    ((CheckSumKey0*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->check, 
                    ((CheckSumKey0*)key)->priv->check))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->date, 
                    ((CheckSumKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->time, 
                    ((CheckSumKey1*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->cash, 
                    ((CheckSumKey1*)key)->priv->cash))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = CheckSum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
            status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
            status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *CheckSum_iter_last(CheckSum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
        key_cmp = (checksum_key0_t*)do_malloc(sizeof(checksum_key0_t));
        memcpy(key_cmp, ((CheckSumKey0*)key)->priv, sizeof(checksum_key0_t));
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
        key_cmp = (checksum_key1_t*)do_malloc(sizeof(checksum_key1_t));
        memcpy(key_cmp, ((CheckSumKey1*)key)->priv, sizeof(checksum_key1_t));
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->cash, 
                    ((CheckSumKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->shift, 
                    ((CheckSumKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->date, 
                    ((CheckSumKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->time, 
                    ((CheckSumKey0*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->check, 
                    ((CheckSumKey0*)key)->priv->check))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->date, 
                    ((CheckSumKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->time, 
                    ((CheckSumKey1*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->cash, 
                    ((CheckSumKey1*)key)->priv->cash))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = CheckSum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
            status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
            status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *CheckSum_iter_le(CheckSum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
        key_cmp = (checksum_key0_t*)do_malloc(sizeof(checksum_key0_t));
        memcpy(key_cmp, ((CheckSumKey0*)key)->priv, sizeof(checksum_key0_t));
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
        key_cmp = (checksum_key1_t*)do_malloc(sizeof(checksum_key1_t));
        memcpy(key_cmp, ((CheckSumKey1*)key)->priv, sizeof(checksum_key1_t));
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->cash, 
                    ((CheckSumKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->shift, 
                    ((CheckSumKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->date, 
                    ((CheckSumKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->time, 
                    ((CheckSumKey0*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->check, 
                    ((CheckSumKey0*)key)->priv->check))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->date, 
                    ((CheckSumKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->time, 
                    ((CheckSumKey1*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->cash, 
                    ((CheckSumKey1*)key)->priv->cash))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = CheckSum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
            status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
            status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *CheckSum_iter_ge(CheckSum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
        key_cmp = (checksum_key0_t*)do_malloc(sizeof(checksum_key0_t));
        memcpy(key_cmp, ((CheckSumKey0*)key)->priv, sizeof(checksum_key0_t));
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
        key_cmp = (checksum_key1_t*)do_malloc(sizeof(checksum_key1_t));
        memcpy(key_cmp, ((CheckSumKey1*)key)->priv, sizeof(checksum_key1_t));
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->cash, 
                    ((CheckSumKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->shift, 
                    ((CheckSumKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->date, 
                    ((CheckSumKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->time, 
                    ((CheckSumKey0*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->check, 
                    ((CheckSumKey0*)key)->priv->check))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->date, 
                    ((CheckSumKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->time, 
                    ((CheckSumKey1*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->cash, 
                    ((CheckSumKey1*)key)->priv->cash))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = CheckSum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
            status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
            status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *CheckSum_iter_equal(CheckSum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
        key_cmp = (checksum_key0_t*)do_malloc(sizeof(checksum_key0_t));
        memcpy(key_cmp, ((CheckSumKey0*)key)->priv, sizeof(checksum_key0_t));
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
        key_cmp = (checksum_key1_t*)do_malloc(sizeof(checksum_key1_t));
        memcpy(key_cmp, ((CheckSumKey1*)key)->priv, sizeof(checksum_key1_t));
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->cash, 
                    ((CheckSumKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->shift, 
                    ((CheckSumKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->date, 
                    ((CheckSumKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->time, 
                    ((CheckSumKey0*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->check, 
                    ((CheckSumKey0*)key)->priv->check))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->date, 
                    ((CheckSumKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->time, 
                    ((CheckSumKey1*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->cash, 
                    ((CheckSumKey1*)key)->priv->cash))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = CheckSum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
            status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
            status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *CheckSum_iter_lt(CheckSum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
        key_cmp = (checksum_key0_t*)do_malloc(sizeof(checksum_key0_t));
        memcpy(key_cmp, ((CheckSumKey0*)key)->priv, sizeof(checksum_key0_t));
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
        key_cmp = (checksum_key1_t*)do_malloc(sizeof(checksum_key1_t));
        memcpy(key_cmp, ((CheckSumKey1*)key)->priv, sizeof(checksum_key1_t));
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->cash, 
                    ((CheckSumKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->shift, 
                    ((CheckSumKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->date, 
                    ((CheckSumKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->time, 
                    ((CheckSumKey0*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->check, 
                    ((CheckSumKey0*)key)->priv->check))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->date, 
                    ((CheckSumKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->time, 
                    ((CheckSumKey1*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->cash, 
                    ((CheckSumKey1*)key)->priv->cash))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = CheckSum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
            status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
            status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *CheckSum_iter_first(CheckSum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
        key_cmp = (checksum_key0_t*)do_malloc(sizeof(checksum_key0_t));
        memcpy(key_cmp, ((CheckSumKey0*)key)->priv, sizeof(checksum_key0_t));
        status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
        key_cmp = (checksum_key1_t*)do_malloc(sizeof(checksum_key1_t));
        memcpy(key_cmp, ((CheckSumKey1*)key)->priv, sizeof(checksum_key1_t));
        status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->cash, 
                    ((CheckSumKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->shift, 
                    ((CheckSumKey0*)key)->priv->shift))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->date, 
                    ((CheckSumKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->time, 
                    ((CheckSumKey0*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((checksum_key0_t*)key_cmp)->check, 
                    ((CheckSumKey0*)key)->priv->check))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->date, 
                    ((CheckSumKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->time, 
                    ((CheckSumKey1*)key)->priv->time))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((checksum_key1_t*)key_cmp)->cash, 
                    ((CheckSumKey1*)key)->priv->cash))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = CheckSum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getCheckSumKey0Type() ) {
            status = do_checksum_get0(self->alias->alias, self->priv, ((CheckSumKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getCheckSumKey1Type() ) {
            status = do_checksum_get1(self->alias->alias, self->priv, ((CheckSumKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *CheckSum_insert(CheckSum* self)
{
    int status;
    status = do_checksum_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_update(CheckSum* self)
{
    int status;
    status = do_checksum_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_delete(CheckSum* self)
{
    int status;
    status = do_checksum_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSum_set_alias(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((CheckSum*)self)->alias);
    Py_INCREF(obj);
    ((CheckSum*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *CheckSum_set(CheckSum* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getCheckSumType() ) {
        memcpy(self->priv, ((CheckSum*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getCheckSumKey0Type() ) {

        do_cpy(self->priv->data.cash, ((CheckSumKey0*)obj)->priv->cash);

        do_cpy(self->priv->data.shift, ((CheckSumKey0*)obj)->priv->shift);

        do_cpy(self->priv->data.check, ((CheckSumKey0*)obj)->priv->check);

        do_cpy(self->priv->data.date, ((CheckSumKey0*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((CheckSumKey0*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getCheckSumKey1Type() ) {

        do_cpy(self->priv->data.cash, ((CheckSumKey1*)obj)->priv->cash);

        do_cpy(self->priv->data.date, ((CheckSumKey1*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((CheckSumKey1*)obj)->priv->time);

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

            else
            if ( !strcmp("sale_sum", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sale_sum", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_total", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"CheckSum\"does not contain \"%s\"", field_name);
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

static PyObject *CheckSum_fields(CheckSum* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class CheckSum");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  CheckSum_cash(self,NULL);
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

    value =  CheckSum_shift(self,NULL);
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

    value =  CheckSum_check(self,NULL);
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

    value =  CheckSum_date(self,NULL);
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

    value =  CheckSum_time(self,NULL);
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

    value =  CheckSum_cashier(self,NULL);
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

    value =  CheckSum_valuta(self,NULL);
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

    value =  CheckSum_operation(self,NULL);
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

    value =  CheckSum_sale_sum(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("sale_sum", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  CheckSum_total(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("total", value);
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

static PyGetSetDef CheckSum_getset[] = {

    {"cash",(getter)CheckSum_cash},

    {"shift",(getter)CheckSum_shift},

    {"check",(getter)CheckSum_check},

    {"date",(getter)CheckSum_date},

    {"date_raw",(getter)CheckSum_date_raw},

    {"time",(getter)CheckSum_time},

    {"time_raw",(getter)CheckSum_time_raw},

    {"cashier",(getter)CheckSum_cashier},

    {"valuta",(getter)CheckSum_valuta},

    {"operation",(getter)CheckSum_operation},

    {"sale_sum",(getter)CheckSum_sale_sum},

    {"total",(getter)CheckSum_total},

    {NULL}
};

static PyMethodDef CheckSum_methods[] = {
    {"set", (PyCFunction)CheckSum_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)CheckSum_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)CheckSum_clone, METH_NOARGS, "clone"},

    {"set_cash", (PyCFunction)CheckSum_set_cash, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_cash"},

    {"set_shift", (PyCFunction)CheckSum_set_shift, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_shift"},

    {"set_check", (PyCFunction)CheckSum_set_check, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_check"},

    {"set_date", (PyCFunction)CheckSum_set_date, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_date"},

    {"set_time", (PyCFunction)CheckSum_set_time, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_time"},

    {"set_cashier", (PyCFunction)CheckSum_set_cashier, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_cashier"},

    {"set_valuta", (PyCFunction)CheckSum_set_valuta, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_valuta"},

    {"set_operation", (PyCFunction)CheckSum_set_operation, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_operation"},

    {"set_sale_sum", (PyCFunction)CheckSum_set_sale_sum, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_sale_sum"},

    {"set_total", (PyCFunction)CheckSum_set_total, METH_VARARGS|METH_KEYWORDS, "CheckSum_set_total"},

    {"get_gt", (PyCFunction)CheckSum_gt, METH_VARARGS|METH_KEYWORDS, "CheckSum_gt"},

    {"get_last", (PyCFunction)CheckSum_last, METH_VARARGS|METH_KEYWORDS, "CheckSum_last"},

    {"get_next", (PyCFunction)CheckSum_next, METH_VARARGS|METH_KEYWORDS, "CheckSum_next"},

    {"get_le", (PyCFunction)CheckSum_le, METH_VARARGS|METH_KEYWORDS, "CheckSum_le"},

    {"get_lt", (PyCFunction)CheckSum_lt, METH_VARARGS|METH_KEYWORDS, "CheckSum_lt"},

    {"get_equal", (PyCFunction)CheckSum_equal, METH_VARARGS|METH_KEYWORDS, "CheckSum_equal"},

    {"get_ge", (PyCFunction)CheckSum_ge, METH_VARARGS|METH_KEYWORDS, "CheckSum_ge"},

    {"get_prev", (PyCFunction)CheckSum_prev, METH_VARARGS|METH_KEYWORDS, "CheckSum_prev"},

    {"get_first", (PyCFunction)CheckSum_first, METH_VARARGS|METH_KEYWORDS, "CheckSum_first"},

    {"gets_gt", (PyCFunction)CheckSum_iter_gt, METH_VARARGS|METH_KEYWORDS, "CheckSum_iter_gt"},

    {"gets_last", (PyCFunction)CheckSum_iter_last, METH_VARARGS|METH_KEYWORDS, "CheckSum_iter_last"},

    {"gets_le", (PyCFunction)CheckSum_iter_le, METH_VARARGS|METH_KEYWORDS, "CheckSum_iter_le"},

    {"gets_ge", (PyCFunction)CheckSum_iter_ge, METH_VARARGS|METH_KEYWORDS, "CheckSum_iter_ge"},

    {"gets_equal", (PyCFunction)CheckSum_iter_equal, METH_VARARGS|METH_KEYWORDS, "CheckSum_iter_equal"},

    {"gets_lt", (PyCFunction)CheckSum_iter_lt, METH_VARARGS|METH_KEYWORDS, "CheckSum_iter_lt"},

    {"gets_first", (PyCFunction)CheckSum_iter_first, METH_VARARGS|METH_KEYWORDS, "CheckSum_iter_first"},

    {"insert", (PyCFunction)CheckSum_insert, METH_VARARGS|METH_KEYWORDS, "CheckSum_insert"},

    {"update", (PyCFunction)CheckSum_update, METH_VARARGS|METH_KEYWORDS, "CheckSum_update"},

    {"delete", (PyCFunction)CheckSum_delete, METH_VARARGS|METH_KEYWORDS, "CheckSum_delete"},

    {NULL}
};

static PyTypeObject CheckSumType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.CheckSum",             /*tp_name*/
    sizeof(CheckSum),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckSum_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)CheckSum_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckSum objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckSum_methods,             /* tp_methods */
    0,
    CheckSum_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckSum_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckSum_new,                 /* tp_new */
};
PyTypeObject *getCheckSumType()
{
    return &CheckSumType_;
}

static void CheckSumKey0_dealloc(PyObject* self)
{
    do_free(((CheckSumKey0*)self)->priv);
    Py_DECREF(((CheckSumKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("CheckSumKey0 free\n");
}
static PyObject *CheckSumKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("CheckSumKey0 new\n");
    CheckSumKey0 *self;
    self = PyObject_NEW(CheckSumKey0, type);
    //self = (CheckSumKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(checksum_key0_t));
    return (PyObject *)self;
}
static int CheckSumKey0_init(CheckSumKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("CheckSumKey0 init\n");
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

static PyObject *CheckSumKey0_clone(CheckSumKey0 *self)
{
    CheckSumKey0 *result;
    result = (CheckSumKey0*)PyObject_CallFunction((PyObject*)getCheckSumKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *CheckSumKey0_cash(CheckSumKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->cash);

    return result;
}

static PyObject *CheckSumKey0_set_cash(CheckSumKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSumKey0_shift(CheckSumKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->shift);

    return result;
}

static PyObject *CheckSumKey0_set_shift(CheckSumKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSumKey0_date(CheckSumKey0* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *CheckSumKey0_date_raw(CheckSumKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *CheckSumKey0_set_date(CheckSumKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSumKey0_time(CheckSumKey0* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *CheckSumKey0_time_raw(CheckSumKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *CheckSumKey0_set_time(CheckSumKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSumKey0_check(CheckSumKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->check);

    return result;
}

static PyObject *CheckSumKey0_set_check(CheckSumKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSumKey0_gt(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_last(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_next(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_le(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_lt(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_equal(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_ge(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_prev(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_first(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey0_iter_gt(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_GT);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckSumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey0_iter_last(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_LAST);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckSumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey0_iter_le(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_LE);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckSumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey0_iter_ge(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_GE);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckSumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey0_iter_equal(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckSumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey0_iter_lt(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_LT);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckSumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey0_iter_first(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_FIRST);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.check, 
                 self->priv->check))
               break;
       }

 
        item = CheckSumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey0_cmp(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getCheckSumKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->cash - ((CheckSumKey0*)obj)->priv->cash;
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->shift - ((CheckSumKey0*)obj)->priv->shift;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->date - ((CheckSumKey0*)obj)->priv->date;
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->time - ((CheckSumKey0*)obj)->priv->time;
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->check - ((CheckSumKey0*)obj)->priv->check;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *CheckSumKey0_set_alias(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((CheckSumKey0*)self)->alias);
    Py_INCREF(obj);
    ((CheckSumKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *CheckSumKey0_set(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getCheckSumKey0Type() ) {
        memcpy(self->priv, ((CheckSumKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getCheckSumType() ) {

        do_cpy(self->priv->cash, ((CheckSum*)obj)->priv->data.cash);

        do_cpy(self->priv->shift, ((CheckSum*)obj)->priv->data.shift);

        do_cpy(self->priv->date, ((CheckSum*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((CheckSum*)obj)->priv->data.time);

        do_cpy(self->priv->check, ((CheckSum*)obj)->priv->data.check);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                        return NULL;
                    break;

                case 4:
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
            if ( !strcmp("check", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_check", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"CheckSumKey0\"does not contain \"%s\"", field_name);
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

static PyObject *CheckSumKey0_fields(CheckSumKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class CheckSumKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  CheckSumKey0_cash(self,NULL);
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

    value =  CheckSumKey0_shift(self,NULL);
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

    value =  CheckSumKey0_date(self,NULL);
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

    value =  CheckSumKey0_time(self,NULL);
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

    value =  CheckSumKey0_check(self,NULL);
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

static PyGetSetDef CheckSumKey0_getset[] = {

    {"cash",(getter)CheckSumKey0_cash},

    {"shift",(getter)CheckSumKey0_shift},

    {"date",(getter)CheckSumKey0_date},

    {"date_raw",(getter)CheckSumKey0_date_raw},

    {"time",(getter)CheckSumKey0_time},

    {"time_raw",(getter)CheckSumKey0_time_raw},

    {"check",(getter)CheckSumKey0_check},

    {NULL}
};

static PyMethodDef CheckSumKey0_methods[] = {
    {"set", (PyCFunction)CheckSumKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)CheckSumKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)CheckSumKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)CheckSumKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_cash", (PyCFunction)CheckSumKey0_set_cash, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_set_cash"},

    {"set_shift", (PyCFunction)CheckSumKey0_set_shift, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_set_shift"},

    {"set_date", (PyCFunction)CheckSumKey0_set_date, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_set_date"},

    {"set_time", (PyCFunction)CheckSumKey0_set_time, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_set_time"},

    {"set_check", (PyCFunction)CheckSumKey0_set_check, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_set_check"},

    {"get_gt", (PyCFunction)CheckSumKey0_gt, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_gt"},

    {"get_last", (PyCFunction)CheckSumKey0_last, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_last"},

    {"get_next", (PyCFunction)CheckSumKey0_next, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_next"},

    {"get_le", (PyCFunction)CheckSumKey0_le, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_le"},

    {"get_lt", (PyCFunction)CheckSumKey0_lt, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_lt"},

    {"get_equal", (PyCFunction)CheckSumKey0_equal, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_equal"},

    {"get_ge", (PyCFunction)CheckSumKey0_ge, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_ge"},

    {"get_prev", (PyCFunction)CheckSumKey0_prev, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_prev"},

    {"get_first", (PyCFunction)CheckSumKey0_first, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_first"},

    {"gets_gt", (PyCFunction)CheckSumKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_iter_gt"},

    {"gets_last", (PyCFunction)CheckSumKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_iter_last"},

    {"gets_le", (PyCFunction)CheckSumKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_iter_le"},

    {"gets_ge", (PyCFunction)CheckSumKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_iter_ge"},

    {"gets_equal", (PyCFunction)CheckSumKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_iter_equal"},

    {"gets_lt", (PyCFunction)CheckSumKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_iter_lt"},

    {"gets_first", (PyCFunction)CheckSumKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "CheckSumKey0_iter_first"},

    {NULL}
};

static PyTypeObject CheckSumKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.CheckSumKey0",             /*tp_name*/
    sizeof(CheckSumKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckSumKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)CheckSumKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckSumKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckSumKey0_methods,             /* tp_methods */
    0,
    CheckSumKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckSumKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckSumKey0_new,                 /* tp_new */
};
PyTypeObject *getCheckSumKey0Type()
{
    return &CheckSumKey0Type_;
}

static void CheckSumKey1_dealloc(PyObject* self)
{
    do_free(((CheckSumKey1*)self)->priv);
    Py_DECREF(((CheckSumKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("CheckSumKey1 free\n");
}
static PyObject *CheckSumKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("CheckSumKey1 new\n");
    CheckSumKey1 *self;
    self = PyObject_NEW(CheckSumKey1, type);
    //self = (CheckSumKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(checksum_key1_t));
    return (PyObject *)self;
}
static int CheckSumKey1_init(CheckSumKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("CheckSumKey1 init\n");
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

static PyObject *CheckSumKey1_clone(CheckSumKey1 *self)
{
    CheckSumKey1 *result;
    result = (CheckSumKey1*)PyObject_CallFunction((PyObject*)getCheckSumKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *CheckSumKey1_date(CheckSumKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *CheckSumKey1_date_raw(CheckSumKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *CheckSumKey1_set_date(CheckSumKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSumKey1_time(CheckSumKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *CheckSumKey1_time_raw(CheckSumKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *CheckSumKey1_set_time(CheckSumKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSumKey1_cash(CheckSumKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->cash);

    return result;
}

static PyObject *CheckSumKey1_set_cash(CheckSumKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *CheckSumKey1_gt(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_last(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_next(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_le(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_lt(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_equal(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_ge(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_prev(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_first(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *CheckSumKey1_iter_gt(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_GT);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

 
        item = CheckSumKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey1_iter_last(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_LAST);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

 
        item = CheckSumKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey1_iter_le(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_LE);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

 
        item = CheckSumKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey1_iter_ge(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_GE);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

 
        item = CheckSumKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey1_iter_equal(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

 
        item = CheckSumKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey1_iter_lt(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_LT);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

 
        item = CheckSumKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey1_iter_first(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    checksum_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_FIRST);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.cash, 
                 self->priv->cash))
               break;
       }

 
        item = CheckSumKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_checksum_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *CheckSumKey1_cmp(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getCheckSumKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->date - ((CheckSumKey1*)obj)->priv->date;
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->time - ((CheckSumKey1*)obj)->priv->time;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->cash - ((CheckSumKey1*)obj)->priv->cash;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *CheckSumKey1_set_alias(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((CheckSumKey1*)self)->alias);
    Py_INCREF(obj);
    ((CheckSumKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *CheckSumKey1_set(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getCheckSumKey1Type() ) {
        memcpy(self->priv, ((CheckSumKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getCheckSumType() ) {

        do_cpy(self->priv->date, ((CheckSum*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((CheckSum*)obj)->priv->data.time);

        do_cpy(self->priv->cash, ((CheckSum*)obj)->priv->data.cash);

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

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_cash", "O", value) )
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

            else
            if ( !strcmp("cash", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_cash", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"CheckSumKey1\"does not contain \"%s\"", field_name);
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

static PyObject *CheckSumKey1_fields(CheckSumKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class CheckSumKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  CheckSumKey1_date(self,NULL);
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

    value =  CheckSumKey1_time(self,NULL);
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

    value =  CheckSumKey1_cash(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef CheckSumKey1_getset[] = {

    {"date",(getter)CheckSumKey1_date},

    {"date_raw",(getter)CheckSumKey1_date_raw},

    {"time",(getter)CheckSumKey1_time},

    {"time_raw",(getter)CheckSumKey1_time_raw},

    {"cash",(getter)CheckSumKey1_cash},

    {NULL}
};

static PyMethodDef CheckSumKey1_methods[] = {
    {"set", (PyCFunction)CheckSumKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)CheckSumKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)CheckSumKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)CheckSumKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_date", (PyCFunction)CheckSumKey1_set_date, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_set_date"},

    {"set_time", (PyCFunction)CheckSumKey1_set_time, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_set_time"},

    {"set_cash", (PyCFunction)CheckSumKey1_set_cash, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_set_cash"},

    {"get_gt", (PyCFunction)CheckSumKey1_gt, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_gt"},

    {"get_last", (PyCFunction)CheckSumKey1_last, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_last"},

    {"get_next", (PyCFunction)CheckSumKey1_next, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_next"},

    {"get_le", (PyCFunction)CheckSumKey1_le, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_le"},

    {"get_lt", (PyCFunction)CheckSumKey1_lt, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_lt"},

    {"get_equal", (PyCFunction)CheckSumKey1_equal, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_equal"},

    {"get_ge", (PyCFunction)CheckSumKey1_ge, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_ge"},

    {"get_prev", (PyCFunction)CheckSumKey1_prev, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_prev"},

    {"get_first", (PyCFunction)CheckSumKey1_first, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_first"},

    {"gets_gt", (PyCFunction)CheckSumKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_iter_gt"},

    {"gets_last", (PyCFunction)CheckSumKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_iter_last"},

    {"gets_le", (PyCFunction)CheckSumKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_iter_le"},

    {"gets_ge", (PyCFunction)CheckSumKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_iter_ge"},

    {"gets_equal", (PyCFunction)CheckSumKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_iter_equal"},

    {"gets_lt", (PyCFunction)CheckSumKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_iter_lt"},

    {"gets_first", (PyCFunction)CheckSumKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "CheckSumKey1_iter_first"},

    {NULL}
};

static PyTypeObject CheckSumKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.CheckSumKey1",             /*tp_name*/
    sizeof(CheckSumKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)CheckSumKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)CheckSumKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "CheckSumKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    CheckSumKey1_methods,             /* tp_methods */
    0,
    CheckSumKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)CheckSumKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    CheckSumKey1_new,                 /* tp_new */
};
PyTypeObject *getCheckSumKey1Type()
{
    return &CheckSumKey1Type_;
}
