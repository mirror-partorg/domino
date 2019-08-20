
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "shift.h"

static void Shift_dealloc(PyObject* self)
{
    do_free(((Shift*)self)->priv);
    Py_DECREF(((Shift*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Shift free\n");
}
static PyObject *Shift_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Shift new\n");
    Shift *self;
    self = PyObject_NEW(Shift, type);
    //self = (Shift *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(shift_rec_t));
    return (PyObject *)self;
}
static int Shift_init(Shift *self, PyObject *args, PyObject *kwds)
{
    //LOG("Shift init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.sklad);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Shift_clone(Shift *self)
{
    Shift *result;
    result = (Shift*)PyObject_CallFunction((PyObject*)getShiftType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Shift_cash(Shift* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.cash);

    return result;
}

static PyObject *Shift_set_cash(Shift* self, PyObject *args, PyObject *kwds)
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

static PyObject *Shift_shift(Shift* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.shift);

    return result;
}

static PyObject *Shift_set_shift(Shift* self, PyObject *args, PyObject *kwds)
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

static PyObject *Shift_date(Shift* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Shift_date_raw(Shift* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *Shift_set_date(Shift* self, PyObject *args, PyObject *kwds)
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

static PyObject *Shift_time(Shift* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Shift_time_raw(Shift* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.time);

    return result;
}

static PyObject *Shift_set_time(Shift* self, PyObject *args, PyObject *kwds)
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

static PyObject *Shift_total_check(Shift* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.totalCheck);

    return result;
}

static PyObject *Shift_set_total_check(Shift* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.totalCheck");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.totalCheck = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.totalCheck = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.totalCheck");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.totalCheck);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Shift_sale_sum(Shift* self, void *unused)
{
    PyObject *result;

    result = PyFloat_FromDouble(self->priv->data.sale_sum);

    return result;
}

static PyObject *Shift_set_sale_sum(Shift* self, PyObject *args, PyObject *kwds)
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

static PyObject *Shift_date_start(Shift* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->data.date_b, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Shift_set_date_start(Shift* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date_b");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date_b");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->data.date_b = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->data.date_b, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Shift_time_start(Shift* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->data.time_b, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Shift_set_time_start(Shift* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time_b");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time_b");
            return NULL;
        }
        if ( PyTime_Check(value) ) {
            tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else {
            tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        self->priv->data.time_b = do_timetosql(tm);
    }
    /*do_sqltotime(self->priv->data.time_b, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Shift_store(Shift* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Shift_set_store(Shift* self, PyObject *args, PyObject *kwds)
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

static PyObject *Shift_gt(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_last(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_next(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_le(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_lt(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_equal(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_ge(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_prev(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_first(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getShiftKey0Type() )
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getShiftKey1Type() )
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getShiftKey2Type() )
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getShiftKey3Type() )
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_iter_gt(Shift* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getShiftKey0Type() ) {
        key_cmp = (shift_key0_t*)do_malloc(sizeof(shift_key0_t));
        memcpy(key_cmp, ((ShiftKey0*)key)->priv, sizeof(shift_key0_t));
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getShiftKey1Type() ) {
        key_cmp = (shift_key1_t*)do_malloc(sizeof(shift_key1_t));
        memcpy(key_cmp, ((ShiftKey1*)key)->priv, sizeof(shift_key1_t));
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getShiftKey2Type() ) {
        key_cmp = (shift_key2_t*)do_malloc(sizeof(shift_key2_t));
        memcpy(key_cmp, ((ShiftKey2*)key)->priv, sizeof(shift_key2_t));
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getShiftKey3Type() ) {
        key_cmp = (shift_key3_t*)do_malloc(sizeof(shift_key3_t));
        memcpy(key_cmp, ((ShiftKey3*)key)->priv, sizeof(shift_key3_t));
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->cash, 
                    ((ShiftKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->shift, 
                    ((ShiftKey0*)key)->priv->shift))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->date, 
                    ((ShiftKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->time, 
                    ((ShiftKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->date_b, 
                    ((ShiftKey2*)key)->priv->date_b))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->time_b, 
                    ((ShiftKey2*)key)->priv->time_b))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->sklad, 
                    ((ShiftKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->date, 
                    ((ShiftKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->time, 
                    ((ShiftKey3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Shift_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
            status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
            status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
            status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
            status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Shift_iter_last(Shift* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getShiftKey0Type() ) {
        key_cmp = (shift_key0_t*)do_malloc(sizeof(shift_key0_t));
        memcpy(key_cmp, ((ShiftKey0*)key)->priv, sizeof(shift_key0_t));
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getShiftKey1Type() ) {
        key_cmp = (shift_key1_t*)do_malloc(sizeof(shift_key1_t));
        memcpy(key_cmp, ((ShiftKey1*)key)->priv, sizeof(shift_key1_t));
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getShiftKey2Type() ) {
        key_cmp = (shift_key2_t*)do_malloc(sizeof(shift_key2_t));
        memcpy(key_cmp, ((ShiftKey2*)key)->priv, sizeof(shift_key2_t));
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getShiftKey3Type() ) {
        key_cmp = (shift_key3_t*)do_malloc(sizeof(shift_key3_t));
        memcpy(key_cmp, ((ShiftKey3*)key)->priv, sizeof(shift_key3_t));
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->cash, 
                    ((ShiftKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->shift, 
                    ((ShiftKey0*)key)->priv->shift))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->date, 
                    ((ShiftKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->time, 
                    ((ShiftKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->date_b, 
                    ((ShiftKey2*)key)->priv->date_b))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->time_b, 
                    ((ShiftKey2*)key)->priv->time_b))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->sklad, 
                    ((ShiftKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->date, 
                    ((ShiftKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->time, 
                    ((ShiftKey3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Shift_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
            status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
            status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
            status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
            status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Shift_iter_le(Shift* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getShiftKey0Type() ) {
        key_cmp = (shift_key0_t*)do_malloc(sizeof(shift_key0_t));
        memcpy(key_cmp, ((ShiftKey0*)key)->priv, sizeof(shift_key0_t));
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getShiftKey1Type() ) {
        key_cmp = (shift_key1_t*)do_malloc(sizeof(shift_key1_t));
        memcpy(key_cmp, ((ShiftKey1*)key)->priv, sizeof(shift_key1_t));
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getShiftKey2Type() ) {
        key_cmp = (shift_key2_t*)do_malloc(sizeof(shift_key2_t));
        memcpy(key_cmp, ((ShiftKey2*)key)->priv, sizeof(shift_key2_t));
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getShiftKey3Type() ) {
        key_cmp = (shift_key3_t*)do_malloc(sizeof(shift_key3_t));
        memcpy(key_cmp, ((ShiftKey3*)key)->priv, sizeof(shift_key3_t));
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->cash, 
                    ((ShiftKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->shift, 
                    ((ShiftKey0*)key)->priv->shift))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->date, 
                    ((ShiftKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->time, 
                    ((ShiftKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->date_b, 
                    ((ShiftKey2*)key)->priv->date_b))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->time_b, 
                    ((ShiftKey2*)key)->priv->time_b))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->sklad, 
                    ((ShiftKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->date, 
                    ((ShiftKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->time, 
                    ((ShiftKey3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Shift_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
            status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
            status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
            status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
            status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Shift_iter_ge(Shift* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getShiftKey0Type() ) {
        key_cmp = (shift_key0_t*)do_malloc(sizeof(shift_key0_t));
        memcpy(key_cmp, ((ShiftKey0*)key)->priv, sizeof(shift_key0_t));
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getShiftKey1Type() ) {
        key_cmp = (shift_key1_t*)do_malloc(sizeof(shift_key1_t));
        memcpy(key_cmp, ((ShiftKey1*)key)->priv, sizeof(shift_key1_t));
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getShiftKey2Type() ) {
        key_cmp = (shift_key2_t*)do_malloc(sizeof(shift_key2_t));
        memcpy(key_cmp, ((ShiftKey2*)key)->priv, sizeof(shift_key2_t));
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getShiftKey3Type() ) {
        key_cmp = (shift_key3_t*)do_malloc(sizeof(shift_key3_t));
        memcpy(key_cmp, ((ShiftKey3*)key)->priv, sizeof(shift_key3_t));
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->cash, 
                    ((ShiftKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->shift, 
                    ((ShiftKey0*)key)->priv->shift))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->date, 
                    ((ShiftKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->time, 
                    ((ShiftKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->date_b, 
                    ((ShiftKey2*)key)->priv->date_b))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->time_b, 
                    ((ShiftKey2*)key)->priv->time_b))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->sklad, 
                    ((ShiftKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->date, 
                    ((ShiftKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->time, 
                    ((ShiftKey3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Shift_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
            status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
            status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
            status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
            status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Shift_iter_equal(Shift* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getShiftKey0Type() ) {
        key_cmp = (shift_key0_t*)do_malloc(sizeof(shift_key0_t));
        memcpy(key_cmp, ((ShiftKey0*)key)->priv, sizeof(shift_key0_t));
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getShiftKey1Type() ) {
        key_cmp = (shift_key1_t*)do_malloc(sizeof(shift_key1_t));
        memcpy(key_cmp, ((ShiftKey1*)key)->priv, sizeof(shift_key1_t));
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getShiftKey2Type() ) {
        key_cmp = (shift_key2_t*)do_malloc(sizeof(shift_key2_t));
        memcpy(key_cmp, ((ShiftKey2*)key)->priv, sizeof(shift_key2_t));
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getShiftKey3Type() ) {
        key_cmp = (shift_key3_t*)do_malloc(sizeof(shift_key3_t));
        memcpy(key_cmp, ((ShiftKey3*)key)->priv, sizeof(shift_key3_t));
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->cash, 
                    ((ShiftKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->shift, 
                    ((ShiftKey0*)key)->priv->shift))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->date, 
                    ((ShiftKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->time, 
                    ((ShiftKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->date_b, 
                    ((ShiftKey2*)key)->priv->date_b))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->time_b, 
                    ((ShiftKey2*)key)->priv->time_b))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->sklad, 
                    ((ShiftKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->date, 
                    ((ShiftKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->time, 
                    ((ShiftKey3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Shift_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
            status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
            status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
            status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
            status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Shift_iter_lt(Shift* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getShiftKey0Type() ) {
        key_cmp = (shift_key0_t*)do_malloc(sizeof(shift_key0_t));
        memcpy(key_cmp, ((ShiftKey0*)key)->priv, sizeof(shift_key0_t));
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getShiftKey1Type() ) {
        key_cmp = (shift_key1_t*)do_malloc(sizeof(shift_key1_t));
        memcpy(key_cmp, ((ShiftKey1*)key)->priv, sizeof(shift_key1_t));
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getShiftKey2Type() ) {
        key_cmp = (shift_key2_t*)do_malloc(sizeof(shift_key2_t));
        memcpy(key_cmp, ((ShiftKey2*)key)->priv, sizeof(shift_key2_t));
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getShiftKey3Type() ) {
        key_cmp = (shift_key3_t*)do_malloc(sizeof(shift_key3_t));
        memcpy(key_cmp, ((ShiftKey3*)key)->priv, sizeof(shift_key3_t));
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->cash, 
                    ((ShiftKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->shift, 
                    ((ShiftKey0*)key)->priv->shift))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->date, 
                    ((ShiftKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->time, 
                    ((ShiftKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->date_b, 
                    ((ShiftKey2*)key)->priv->date_b))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->time_b, 
                    ((ShiftKey2*)key)->priv->time_b))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->sklad, 
                    ((ShiftKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->date, 
                    ((ShiftKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->time, 
                    ((ShiftKey3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Shift_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
            status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
            status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
            status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
            status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Shift_iter_first(Shift* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getShiftKey0Type() ) {
        key_cmp = (shift_key0_t*)do_malloc(sizeof(shift_key0_t));
        memcpy(key_cmp, ((ShiftKey0*)key)->priv, sizeof(shift_key0_t));
        status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getShiftKey1Type() ) {
        key_cmp = (shift_key1_t*)do_malloc(sizeof(shift_key1_t));
        memcpy(key_cmp, ((ShiftKey1*)key)->priv, sizeof(shift_key1_t));
        status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getShiftKey2Type() ) {
        key_cmp = (shift_key2_t*)do_malloc(sizeof(shift_key2_t));
        memcpy(key_cmp, ((ShiftKey2*)key)->priv, sizeof(shift_key2_t));
        status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getShiftKey3Type() ) {
        key_cmp = (shift_key3_t*)do_malloc(sizeof(shift_key3_t));
        memcpy(key_cmp, ((ShiftKey3*)key)->priv, sizeof(shift_key3_t));
        status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->cash, 
                    ((ShiftKey0*)key)->priv->cash))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key0_t*)key_cmp)->shift, 
                    ((ShiftKey0*)key)->priv->shift))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->date, 
                    ((ShiftKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key1_t*)key_cmp)->time, 
                    ((ShiftKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->date_b, 
                    ((ShiftKey2*)key)->priv->date_b))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key2_t*)key_cmp)->time_b, 
                    ((ShiftKey2*)key)->priv->time_b))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->sklad, 
                    ((ShiftKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->date, 
                    ((ShiftKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((shift_key3_t*)key_cmp)->time, 
                    ((ShiftKey3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Shift_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getShiftKey0Type() ) {
            status = do_shift_get0(self->alias->alias, self->priv, ((ShiftKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey1Type() ) {
            status = do_shift_get1(self->alias->alias, self->priv, ((ShiftKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey2Type() ) {
            status = do_shift_get2(self->alias->alias, self->priv, ((ShiftKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getShiftKey3Type() ) {
            status = do_shift_get3(self->alias->alias, self->priv, ((ShiftKey3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Shift_insert(Shift* self)
{
    int status;
    status = do_shift_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_update(Shift* self)
{
    int status;
    status = do_shift_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_delete(Shift* self)
{
    int status;
    status = do_shift_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Shift_set_alias(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Shift*)self)->alias);
    Py_INCREF(obj);
    ((Shift*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Shift_set(Shift* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getShiftType() ) {
        memcpy(self->priv, ((Shift*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getShiftKey0Type() ) {

        do_cpy(self->priv->data.cash, ((ShiftKey0*)obj)->priv->cash);

        do_cpy(self->priv->data.shift, ((ShiftKey0*)obj)->priv->shift);

    }

    else 
    if ( Py_TYPE(obj) == getShiftKey1Type() ) {

        do_cpy(self->priv->data.date, ((ShiftKey1*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((ShiftKey1*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getShiftKey2Type() ) {

        do_cpy(self->priv->data.date_b, ((ShiftKey2*)obj)->priv->date_b);

        do_cpy(self->priv->data.time_b, ((ShiftKey2*)obj)->priv->time_b);

    }

    else 
    if ( Py_TYPE(obj) == getShiftKey3Type() ) {

        do_cpy(self->priv->data.date, ((ShiftKey3*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((ShiftKey3*)obj)->priv->time);

        do_cpy(self->priv->data.sklad, ((ShiftKey3*)obj)->priv->sklad);

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
            if ( !strcmp("total_check", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_total_check", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sale_sum", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sale_sum", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date_start", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date_start", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time_start", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time_start", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Shift\"does not contain \"%s\"", field_name);
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

static PyObject *Shift_fields(Shift* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Shift");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Shift_cash(self,NULL);
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

    value =  Shift_shift(self,NULL);
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

    value =  Shift_date(self,NULL);
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

    value =  Shift_time(self,NULL);
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

    value =  Shift_total_check(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("total_check", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Shift_sale_sum(self,NULL);
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

    value =  Shift_date_start(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date_start", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Shift_time_start(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time_start", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Shift_store(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef Shift_getset[] = {

    {"cash",(getter)Shift_cash},

    {"shift",(getter)Shift_shift},

    {"date",(getter)Shift_date},

    {"date_raw",(getter)Shift_date_raw},

    {"time",(getter)Shift_time},

    {"time_raw",(getter)Shift_time_raw},

    {"total_check",(getter)Shift_total_check},

    {"sale_sum",(getter)Shift_sale_sum},

    {"date_start",(getter)Shift_date_start},

    {"time_start",(getter)Shift_time_start},

    {"store",(getter)Shift_store},

    {NULL}
};

static PyMethodDef Shift_methods[] = {
    {"set", (PyCFunction)Shift_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Shift_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Shift_clone, METH_NOARGS, "clone"},

    {"set_cash", (PyCFunction)Shift_set_cash, METH_VARARGS|METH_KEYWORDS, "Shift_set_cash"},

    {"set_shift", (PyCFunction)Shift_set_shift, METH_VARARGS|METH_KEYWORDS, "Shift_set_shift"},

    {"set_date", (PyCFunction)Shift_set_date, METH_VARARGS|METH_KEYWORDS, "Shift_set_date"},

    {"set_time", (PyCFunction)Shift_set_time, METH_VARARGS|METH_KEYWORDS, "Shift_set_time"},

    {"set_total_check", (PyCFunction)Shift_set_total_check, METH_VARARGS|METH_KEYWORDS, "Shift_set_total_check"},

    {"set_sale_sum", (PyCFunction)Shift_set_sale_sum, METH_VARARGS|METH_KEYWORDS, "Shift_set_sale_sum"},

    {"set_date_start", (PyCFunction)Shift_set_date_start, METH_VARARGS|METH_KEYWORDS, "Shift_set_date_start"},

    {"set_time_start", (PyCFunction)Shift_set_time_start, METH_VARARGS|METH_KEYWORDS, "Shift_set_time_start"},

    {"set_store", (PyCFunction)Shift_set_store, METH_VARARGS|METH_KEYWORDS, "Shift_set_store"},

    {"get_gt", (PyCFunction)Shift_gt, METH_VARARGS|METH_KEYWORDS, "Shift_gt"},

    {"get_last", (PyCFunction)Shift_last, METH_VARARGS|METH_KEYWORDS, "Shift_last"},

    {"get_next", (PyCFunction)Shift_next, METH_VARARGS|METH_KEYWORDS, "Shift_next"},

    {"get_le", (PyCFunction)Shift_le, METH_VARARGS|METH_KEYWORDS, "Shift_le"},

    {"get_lt", (PyCFunction)Shift_lt, METH_VARARGS|METH_KEYWORDS, "Shift_lt"},

    {"get_equal", (PyCFunction)Shift_equal, METH_VARARGS|METH_KEYWORDS, "Shift_equal"},

    {"get_ge", (PyCFunction)Shift_ge, METH_VARARGS|METH_KEYWORDS, "Shift_ge"},

    {"get_prev", (PyCFunction)Shift_prev, METH_VARARGS|METH_KEYWORDS, "Shift_prev"},

    {"get_first", (PyCFunction)Shift_first, METH_VARARGS|METH_KEYWORDS, "Shift_first"},

    {"gets_gt", (PyCFunction)Shift_iter_gt, METH_VARARGS|METH_KEYWORDS, "Shift_iter_gt"},

    {"gets_last", (PyCFunction)Shift_iter_last, METH_VARARGS|METH_KEYWORDS, "Shift_iter_last"},

    {"gets_le", (PyCFunction)Shift_iter_le, METH_VARARGS|METH_KEYWORDS, "Shift_iter_le"},

    {"gets_ge", (PyCFunction)Shift_iter_ge, METH_VARARGS|METH_KEYWORDS, "Shift_iter_ge"},

    {"gets_equal", (PyCFunction)Shift_iter_equal, METH_VARARGS|METH_KEYWORDS, "Shift_iter_equal"},

    {"gets_lt", (PyCFunction)Shift_iter_lt, METH_VARARGS|METH_KEYWORDS, "Shift_iter_lt"},

    {"gets_first", (PyCFunction)Shift_iter_first, METH_VARARGS|METH_KEYWORDS, "Shift_iter_first"},

    {"insert", (PyCFunction)Shift_insert, METH_VARARGS|METH_KEYWORDS, "Shift_insert"},

    {"update", (PyCFunction)Shift_update, METH_VARARGS|METH_KEYWORDS, "Shift_update"},

    {"delete", (PyCFunction)Shift_delete, METH_VARARGS|METH_KEYWORDS, "Shift_delete"},

    {NULL}
};

static PyTypeObject ShiftType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Shift",             /*tp_name*/
    sizeof(Shift),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Shift_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Shift_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Shift objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Shift_methods,             /* tp_methods */
    0,
    Shift_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Shift_init,      /* tp_init */
    0,                         /* tp_alloc */
    Shift_new,                 /* tp_new */
};
PyTypeObject *getShiftType()
{
    return &ShiftType_;
}

static void ShiftKey0_dealloc(PyObject* self)
{
    do_free(((ShiftKey0*)self)->priv);
    Py_DECREF(((ShiftKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ShiftKey0 free\n");
}
static PyObject *ShiftKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ShiftKey0 new\n");
    ShiftKey0 *self;
    self = PyObject_NEW(ShiftKey0, type);
    //self = (ShiftKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(shift_key0_t));
    return (PyObject *)self;
}
static int ShiftKey0_init(ShiftKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ShiftKey0 init\n");
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

static PyObject *ShiftKey0_clone(ShiftKey0 *self)
{
    ShiftKey0 *result;
    result = (ShiftKey0*)PyObject_CallFunction((PyObject*)getShiftKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ShiftKey0_cash(ShiftKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->cash);

    return result;
}

static PyObject *ShiftKey0_set_cash(ShiftKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ShiftKey0_shift(ShiftKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->shift);

    return result;
}

static PyObject *ShiftKey0_set_shift(ShiftKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ShiftKey0_gt(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_last(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_next(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_le(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_lt(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_equal(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_ge(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_prev(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_first(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey0_iter_gt(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_GT);
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

 
        item = ShiftKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey0_iter_last(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_LAST);
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

 
        item = ShiftKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey0_iter_le(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_LE);
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

 
        item = ShiftKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey0_iter_ge(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_GE);
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

 
        item = ShiftKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey0_iter_equal(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
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

 
        item = ShiftKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey0_iter_lt(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_LT);
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

 
        item = ShiftKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey0_iter_first(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key0(self->alias->alias, self->priv, DO_GET_FIRST);
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

 
        item = ShiftKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey0_cmp(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getShiftKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->cash - ((ShiftKey0*)obj)->priv->cash;
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->shift - ((ShiftKey0*)obj)->priv->shift;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ShiftKey0_set_alias(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ShiftKey0*)self)->alias);
    Py_INCREF(obj);
    ((ShiftKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ShiftKey0_set(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getShiftKey0Type() ) {
        memcpy(self->priv, ((ShiftKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getShiftType() ) {

        do_cpy(self->priv->cash, ((Shift*)obj)->priv->data.cash);

        do_cpy(self->priv->shift, ((Shift*)obj)->priv->data.shift);

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

            else {
                do_log(LOG_ERR, "Class \"ShiftKey0\"does not contain \"%s\"", field_name);
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

static PyObject *ShiftKey0_fields(ShiftKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ShiftKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ShiftKey0_cash(self,NULL);
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

    value =  ShiftKey0_shift(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef ShiftKey0_getset[] = {

    {"cash",(getter)ShiftKey0_cash},

    {"shift",(getter)ShiftKey0_shift},

    {NULL}
};

static PyMethodDef ShiftKey0_methods[] = {
    {"set", (PyCFunction)ShiftKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ShiftKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ShiftKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ShiftKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_cash", (PyCFunction)ShiftKey0_set_cash, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_set_cash"},

    {"set_shift", (PyCFunction)ShiftKey0_set_shift, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_set_shift"},

    {"get_gt", (PyCFunction)ShiftKey0_gt, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_gt"},

    {"get_last", (PyCFunction)ShiftKey0_last, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_last"},

    {"get_next", (PyCFunction)ShiftKey0_next, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_next"},

    {"get_le", (PyCFunction)ShiftKey0_le, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_le"},

    {"get_lt", (PyCFunction)ShiftKey0_lt, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_lt"},

    {"get_equal", (PyCFunction)ShiftKey0_equal, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_equal"},

    {"get_ge", (PyCFunction)ShiftKey0_ge, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_ge"},

    {"get_prev", (PyCFunction)ShiftKey0_prev, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_prev"},

    {"get_first", (PyCFunction)ShiftKey0_first, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_first"},

    {"gets_gt", (PyCFunction)ShiftKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_iter_gt"},

    {"gets_last", (PyCFunction)ShiftKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_iter_last"},

    {"gets_le", (PyCFunction)ShiftKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_iter_le"},

    {"gets_ge", (PyCFunction)ShiftKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_iter_ge"},

    {"gets_equal", (PyCFunction)ShiftKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_iter_equal"},

    {"gets_lt", (PyCFunction)ShiftKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_iter_lt"},

    {"gets_first", (PyCFunction)ShiftKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "ShiftKey0_iter_first"},

    {NULL}
};

static PyTypeObject ShiftKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ShiftKey0",             /*tp_name*/
    sizeof(ShiftKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ShiftKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ShiftKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ShiftKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ShiftKey0_methods,             /* tp_methods */
    0,
    ShiftKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ShiftKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ShiftKey0_new,                 /* tp_new */
};
PyTypeObject *getShiftKey0Type()
{
    return &ShiftKey0Type_;
}

static void ShiftKey1_dealloc(PyObject* self)
{
    do_free(((ShiftKey1*)self)->priv);
    Py_DECREF(((ShiftKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ShiftKey1 free\n");
}
static PyObject *ShiftKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ShiftKey1 new\n");
    ShiftKey1 *self;
    self = PyObject_NEW(ShiftKey1, type);
    //self = (ShiftKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(shift_key1_t));
    return (PyObject *)self;
}
static int ShiftKey1_init(ShiftKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ShiftKey1 init\n");
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

static PyObject *ShiftKey1_clone(ShiftKey1 *self)
{
    ShiftKey1 *result;
    result = (ShiftKey1*)PyObject_CallFunction((PyObject*)getShiftKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ShiftKey1_date(ShiftKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ShiftKey1_date_raw(ShiftKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ShiftKey1_set_date(ShiftKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ShiftKey1_time(ShiftKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *ShiftKey1_time_raw(ShiftKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *ShiftKey1_set_time(ShiftKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ShiftKey1_gt(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_last(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_next(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_le(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_lt(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_equal(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_ge(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_prev(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_first(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey1_iter_gt(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_GT);
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

 
        item = ShiftKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey1_iter_last(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_LAST);
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

 
        item = ShiftKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey1_iter_le(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_LE);
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

 
        item = ShiftKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey1_iter_ge(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_GE);
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

 
        item = ShiftKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey1_iter_equal(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
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

 
        item = ShiftKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey1_iter_lt(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_LT);
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

 
        item = ShiftKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey1_iter_first(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key1(self->alias->alias, self->priv, DO_GET_FIRST);
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

 
        item = ShiftKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey1_cmp(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getShiftKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->date - ((ShiftKey1*)obj)->priv->date;
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->time - ((ShiftKey1*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ShiftKey1_set_alias(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ShiftKey1*)self)->alias);
    Py_INCREF(obj);
    ((ShiftKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ShiftKey1_set(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getShiftKey1Type() ) {
        memcpy(self->priv, ((ShiftKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getShiftType() ) {

        do_cpy(self->priv->date, ((Shift*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Shift*)obj)->priv->data.time);

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
                do_log(LOG_ERR, "Class \"ShiftKey1\"does not contain \"%s\"", field_name);
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

static PyObject *ShiftKey1_fields(ShiftKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ShiftKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ShiftKey1_date(self,NULL);
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

    value =  ShiftKey1_time(self,NULL);
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

static PyGetSetDef ShiftKey1_getset[] = {

    {"date",(getter)ShiftKey1_date},

    {"date_raw",(getter)ShiftKey1_date_raw},

    {"time",(getter)ShiftKey1_time},

    {"time_raw",(getter)ShiftKey1_time_raw},

    {NULL}
};

static PyMethodDef ShiftKey1_methods[] = {
    {"set", (PyCFunction)ShiftKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ShiftKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ShiftKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ShiftKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_date", (PyCFunction)ShiftKey1_set_date, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_set_date"},

    {"set_time", (PyCFunction)ShiftKey1_set_time, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_set_time"},

    {"get_gt", (PyCFunction)ShiftKey1_gt, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_gt"},

    {"get_last", (PyCFunction)ShiftKey1_last, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_last"},

    {"get_next", (PyCFunction)ShiftKey1_next, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_next"},

    {"get_le", (PyCFunction)ShiftKey1_le, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_le"},

    {"get_lt", (PyCFunction)ShiftKey1_lt, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_lt"},

    {"get_equal", (PyCFunction)ShiftKey1_equal, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_equal"},

    {"get_ge", (PyCFunction)ShiftKey1_ge, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_ge"},

    {"get_prev", (PyCFunction)ShiftKey1_prev, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_prev"},

    {"get_first", (PyCFunction)ShiftKey1_first, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_first"},

    {"gets_gt", (PyCFunction)ShiftKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_iter_gt"},

    {"gets_last", (PyCFunction)ShiftKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_iter_last"},

    {"gets_le", (PyCFunction)ShiftKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_iter_le"},

    {"gets_ge", (PyCFunction)ShiftKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_iter_ge"},

    {"gets_equal", (PyCFunction)ShiftKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_iter_equal"},

    {"gets_lt", (PyCFunction)ShiftKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_iter_lt"},

    {"gets_first", (PyCFunction)ShiftKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "ShiftKey1_iter_first"},

    {NULL}
};

static PyTypeObject ShiftKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ShiftKey1",             /*tp_name*/
    sizeof(ShiftKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ShiftKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ShiftKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ShiftKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ShiftKey1_methods,             /* tp_methods */
    0,
    ShiftKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ShiftKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ShiftKey1_new,                 /* tp_new */
};
PyTypeObject *getShiftKey1Type()
{
    return &ShiftKey1Type_;
}

static void ShiftKey2_dealloc(PyObject* self)
{
    do_free(((ShiftKey2*)self)->priv);
    Py_DECREF(((ShiftKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ShiftKey2 free\n");
}
static PyObject *ShiftKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ShiftKey2 new\n");
    ShiftKey2 *self;
    self = PyObject_NEW(ShiftKey2, type);
    //self = (ShiftKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(shift_key2_t));
    return (PyObject *)self;
}
static int ShiftKey2_init(ShiftKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ShiftKey2 init\n");
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

static PyObject *ShiftKey2_clone(ShiftKey2 *self)
{
    ShiftKey2 *result;
    result = (ShiftKey2*)PyObject_CallFunction((PyObject*)getShiftKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ShiftKey2_date_start(ShiftKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->date_b, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ShiftKey2_set_date_start(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date_b");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date_b");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->date_b = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->date_b, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ShiftKey2_time_start(ShiftKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->time_b, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *ShiftKey2_set_time_start(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time_b");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time_b");
            return NULL;
        }
        if ( PyTime_Check(value) ) {
            tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
        }
        else {
            tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
        }
        self->priv->time_b = do_timetosql(tm);
    }
    /*do_sqltotime(self->priv->time_b, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ShiftKey2_gt(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_last(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_next(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_le(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_lt(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_equal(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_ge(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_prev(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_first(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey2_iter_gt(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date_b, 
                 self->priv->date_b))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time_b, 
                 self->priv->time_b))
               break;
       }

 
        item = ShiftKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey2_iter_last(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date_b, 
                 self->priv->date_b))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time_b, 
                 self->priv->time_b))
               break;
       }

 
        item = ShiftKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey2_iter_le(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date_b, 
                 self->priv->date_b))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time_b, 
                 self->priv->time_b))
               break;
       }

 
        item = ShiftKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey2_iter_ge(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date_b, 
                 self->priv->date_b))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time_b, 
                 self->priv->time_b))
               break;
       }

 
        item = ShiftKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey2_iter_equal(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date_b, 
                 self->priv->date_b))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time_b, 
                 self->priv->time_b))
               break;
       }

 
        item = ShiftKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey2_iter_lt(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date_b, 
                 self->priv->date_b))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time_b, 
                 self->priv->time_b))
               break;
       }

 
        item = ShiftKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey2_iter_first(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date_b, 
                 self->priv->date_b))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time_b, 
                 self->priv->time_b))
               break;
       }

 
        item = ShiftKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey2_cmp(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getShiftKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->date_b - ((ShiftKey2*)obj)->priv->date_b;
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->time_b - ((ShiftKey2*)obj)->priv->time_b;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ShiftKey2_set_alias(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ShiftKey2*)self)->alias);
    Py_INCREF(obj);
    ((ShiftKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ShiftKey2_set(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getShiftKey2Type() ) {
        memcpy(self->priv, ((ShiftKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getShiftType() ) {

        do_cpy(self->priv->date_b, ((Shift*)obj)->priv->data.date_b);

        do_cpy(self->priv->time_b, ((Shift*)obj)->priv->data.time_b);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date_start", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_time_start", "O", value) )
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
            if ( !strcmp("date_start", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date_start", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time_start", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time_start", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ShiftKey2\"does not contain \"%s\"", field_name);
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

static PyObject *ShiftKey2_fields(ShiftKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ShiftKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ShiftKey2_date_start(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date_start", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ShiftKey2_time_start(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time_start", value);
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

static PyGetSetDef ShiftKey2_getset[] = {

    {"date_start",(getter)ShiftKey2_date_start},

    {"time_start",(getter)ShiftKey2_time_start},

    {NULL}
};

static PyMethodDef ShiftKey2_methods[] = {
    {"set", (PyCFunction)ShiftKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ShiftKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ShiftKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ShiftKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_date_start", (PyCFunction)ShiftKey2_set_date_start, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_set_date_start"},

    {"set_time_start", (PyCFunction)ShiftKey2_set_time_start, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_set_time_start"},

    {"get_gt", (PyCFunction)ShiftKey2_gt, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_gt"},

    {"get_last", (PyCFunction)ShiftKey2_last, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_last"},

    {"get_next", (PyCFunction)ShiftKey2_next, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_next"},

    {"get_le", (PyCFunction)ShiftKey2_le, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_le"},

    {"get_lt", (PyCFunction)ShiftKey2_lt, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_lt"},

    {"get_equal", (PyCFunction)ShiftKey2_equal, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_equal"},

    {"get_ge", (PyCFunction)ShiftKey2_ge, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_ge"},

    {"get_prev", (PyCFunction)ShiftKey2_prev, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_prev"},

    {"get_first", (PyCFunction)ShiftKey2_first, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_first"},

    {"gets_gt", (PyCFunction)ShiftKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_iter_gt"},

    {"gets_last", (PyCFunction)ShiftKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_iter_last"},

    {"gets_le", (PyCFunction)ShiftKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_iter_le"},

    {"gets_ge", (PyCFunction)ShiftKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_iter_ge"},

    {"gets_equal", (PyCFunction)ShiftKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_iter_equal"},

    {"gets_lt", (PyCFunction)ShiftKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_iter_lt"},

    {"gets_first", (PyCFunction)ShiftKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "ShiftKey2_iter_first"},

    {NULL}
};

static PyTypeObject ShiftKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ShiftKey2",             /*tp_name*/
    sizeof(ShiftKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ShiftKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ShiftKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ShiftKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ShiftKey2_methods,             /* tp_methods */
    0,
    ShiftKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ShiftKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    ShiftKey2_new,                 /* tp_new */
};
PyTypeObject *getShiftKey2Type()
{
    return &ShiftKey2Type_;
}

static void ShiftKey3_dealloc(PyObject* self)
{
    do_free(((ShiftKey3*)self)->priv);
    Py_DECREF(((ShiftKey3*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ShiftKey3 free\n");
}
static PyObject *ShiftKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ShiftKey3 new\n");
    ShiftKey3 *self;
    self = PyObject_NEW(ShiftKey3, type);
    //self = (ShiftKey3 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(shift_key3_t));
    return (PyObject *)self;
}
static int ShiftKey3_init(ShiftKey3 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ShiftKey3 init\n");
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

static PyObject *ShiftKey3_clone(ShiftKey3 *self)
{
    ShiftKey3 *result;
    result = (ShiftKey3*)PyObject_CallFunction((PyObject*)getShiftKey3Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ShiftKey3_store(ShiftKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ShiftKey3_set_store(ShiftKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *ShiftKey3_date(ShiftKey3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ShiftKey3_date_raw(ShiftKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ShiftKey3_set_date(ShiftKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *ShiftKey3_time(ShiftKey3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltotime(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *ShiftKey3_time_raw(ShiftKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *ShiftKey3_set_time(ShiftKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *ShiftKey3_gt(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_last(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_next(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_le(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_lt(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_equal(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_ge(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_prev(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_first(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ShiftKey3_iter_gt(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_GT);
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

 
        item = ShiftKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey3_iter_last(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_LAST);
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

 
        item = ShiftKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey3_iter_le(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_LE);
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

 
        item = ShiftKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey3_iter_ge(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_GE);
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

 
        item = ShiftKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey3_iter_equal(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_EQUAL);
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

 
        item = ShiftKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey3_iter_lt(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_LT);
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

 
        item = ShiftKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey3_iter_first(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    shift_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_shift_key3(self->alias->alias, self->priv, DO_GET_FIRST);
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

 
        item = ShiftKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_shift_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ShiftKey3_cmp(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getShiftKey3Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->sklad, ((ShiftKey3*)obj)->priv->sklad);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((ShiftKey3*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->time - ((ShiftKey3*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ShiftKey3_set_alias(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ShiftKey3*)self)->alias);
    Py_INCREF(obj);
    ((ShiftKey3*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ShiftKey3_set(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getShiftKey3Type() ) {
        memcpy(self->priv, ((ShiftKey3*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getShiftType() ) {

        do_cpy(self->priv->sklad, ((Shift*)obj)->priv->data.sklad);

        do_cpy(self->priv->date, ((Shift*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Shift*)obj)->priv->data.time);

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
                do_log(LOG_ERR, "Class \"ShiftKey3\"does not contain \"%s\"", field_name);
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

static PyObject *ShiftKey3_fields(ShiftKey3* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ShiftKey3");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ShiftKey3_store(self,NULL);
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

    value =  ShiftKey3_date(self,NULL);
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

    value =  ShiftKey3_time(self,NULL);
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

static PyGetSetDef ShiftKey3_getset[] = {

    {"store",(getter)ShiftKey3_store},

    {"date",(getter)ShiftKey3_date},

    {"date_raw",(getter)ShiftKey3_date_raw},

    {"time",(getter)ShiftKey3_time},

    {"time_raw",(getter)ShiftKey3_time_raw},

    {NULL}
};

static PyMethodDef ShiftKey3_methods[] = {
    {"set", (PyCFunction)ShiftKey3_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ShiftKey3_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ShiftKey3_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ShiftKey3_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_store", (PyCFunction)ShiftKey3_set_store, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_set_store"},

    {"set_date", (PyCFunction)ShiftKey3_set_date, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_set_date"},

    {"set_time", (PyCFunction)ShiftKey3_set_time, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_set_time"},

    {"get_gt", (PyCFunction)ShiftKey3_gt, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_gt"},

    {"get_last", (PyCFunction)ShiftKey3_last, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_last"},

    {"get_next", (PyCFunction)ShiftKey3_next, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_next"},

    {"get_le", (PyCFunction)ShiftKey3_le, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_le"},

    {"get_lt", (PyCFunction)ShiftKey3_lt, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_lt"},

    {"get_equal", (PyCFunction)ShiftKey3_equal, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_equal"},

    {"get_ge", (PyCFunction)ShiftKey3_ge, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_ge"},

    {"get_prev", (PyCFunction)ShiftKey3_prev, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_prev"},

    {"get_first", (PyCFunction)ShiftKey3_first, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_first"},

    {"gets_gt", (PyCFunction)ShiftKey3_iter_gt, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_iter_gt"},

    {"gets_last", (PyCFunction)ShiftKey3_iter_last, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_iter_last"},

    {"gets_le", (PyCFunction)ShiftKey3_iter_le, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_iter_le"},

    {"gets_ge", (PyCFunction)ShiftKey3_iter_ge, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_iter_ge"},

    {"gets_equal", (PyCFunction)ShiftKey3_iter_equal, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_iter_equal"},

    {"gets_lt", (PyCFunction)ShiftKey3_iter_lt, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_iter_lt"},

    {"gets_first", (PyCFunction)ShiftKey3_iter_first, METH_VARARGS|METH_KEYWORDS, "ShiftKey3_iter_first"},

    {NULL}
};

static PyTypeObject ShiftKey3Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ShiftKey3",             /*tp_name*/
    sizeof(ShiftKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ShiftKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ShiftKey3_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ShiftKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ShiftKey3_methods,             /* tp_methods */
    0,
    ShiftKey3_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ShiftKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    ShiftKey3_new,                 /* tp_new */
};
PyTypeObject *getShiftKey3Type()
{
    return &ShiftKey3Type_;
}
