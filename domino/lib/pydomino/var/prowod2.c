
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "prowod2.h"

static void Prowod2_dealloc(PyObject* self)
{
    do_free(((Prowod2*)self)->priv);
    Py_DECREF(((Prowod2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Prowod2 free\n");
}
static PyObject *Prowod2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2 new\n");
    Prowod2 *self;
    self = PyObject_NEW(Prowod2, type);
    //self = (Prowod2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod2_rec_t));
    return (PyObject *)self;
}
static int Prowod2_init(Prowod2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("Prowod2 init\n");
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

    do_text_set_empty(self->priv->data.sklad);

    do_text_set_empty(self->priv->data.document);

    do_text_set_empty(self->priv->data.otdel);

    do_prowod2_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Prowod2_clone(Prowod2 *self)
{
    Prowod2 *result;
    result = (Prowod2*)PyObject_CallFunction((PyObject*)getProwod2Type(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Prowod2_document_type(Prowod2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2_set_document_type(Prowod2* self, PyObject *args, PyObject *kwds)
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

static PyObject *Prowod2_store(Prowod2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2_set_store(Prowod2* self, PyObject *args, PyObject *kwds)
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

static PyObject *Prowod2_document(Prowod2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2_set_document(Prowod2* self, PyObject *args, PyObject *kwds)
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

static PyObject *Prowod2_number(Prowod2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.number);

    return result;
}

static PyObject *Prowod2_set_number(Prowod2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.number");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.number = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.number = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.number");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.number);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2_division(Prowod2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.otdel);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2_set_division(Prowod2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.otdel");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.otdel, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.otdel);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2_date(Prowod2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Prowod2_date_raw(Prowod2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *Prowod2_set_date(Prowod2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->data.date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2_time(Prowod2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Prowod2_time_raw(Prowod2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.time);

    return result;
}

static PyObject *Prowod2_set_time(Prowod2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
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
             self->priv->data.time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2_saldo_debit(Prowod2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.saldo_d);

    return result;
}

static PyObject *Prowod2_set_saldo_debit(Prowod2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.saldo_d");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.saldo_d = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.saldo_d = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.saldo_d");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.saldo_d);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2_saldo_credit(Prowod2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.saldo_k);

    return result;
}

static PyObject *Prowod2_set_saldo_credit(Prowod2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.saldo_k");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.saldo_k = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.saldo_k = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.saldo_k");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.saldo_k);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2_get_sum(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get sum");
        return NULL;
    }
    double sum;
    sum = do_prowod2_sum(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(sum);

    return result;
}

static PyObject *Prowod2_set_sum(Prowod2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value=NULL;
    int number;
    static char *kwlist[] = {"number", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set sum");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_prowod2_sum_set(self->alias->alias,self->priv, number, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_prowod2_sum_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sum");
            return NULL;
        }    
    }
    /*double sum;
    sum = do_prowod2_sum(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(sum);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2_set_param(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_prowod2_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_prowod2_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_prowod2_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Prowod2_set_params(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_prowod2_params_clear(self->alias->alias,self->priv);
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
                 do_prowod2_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_prowod2_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_prowod2_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Prowod2_get_param(Prowod2* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_prowod2_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Prowod2_params_clear(Prowod2* self)
{
    do_prowod2_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Prowod2_get_params(Prowod2* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}


static PyObject *Prowod2_sums_clear(Prowod2* self)
{
    do_prowod2_sum_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Prowod2_get_sums(Prowod2* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 5; i++ ) {
        number = self->priv->data.nsum[i];
        if ( !number ) break;
        quant = do_prowod2_sum(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *Prowod2_set_sums(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_prowod2_sum_clear(self->alias->alias,self->priv);
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
             if ( number > 2 ) continue;
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_prowod2_sum_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_prowod2_sum_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
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

static PyObject *Prowod2_equal(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_next(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_prev(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_gt(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_ge(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_lt(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_le(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_first(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_last(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwod2Key0Type() )
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() )
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() )
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() )
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_iter_equal(Prowod2* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwod2Key0Type() ) {
        key_cmp = (prowod2_key0_t*)do_malloc(sizeof(prowod2_key0_t));
        memcpy(key_cmp, ((Prowod2Key0*)key)->priv, sizeof(prowod2_key0_t));
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() ) {
        key_cmp = (prowod2_key1_t*)do_malloc(sizeof(prowod2_key1_t));
        memcpy(key_cmp, ((Prowod2Key1*)key)->priv, sizeof(prowod2_key1_t));
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() ) {
        key_cmp = (prowod2_key2_t*)do_malloc(sizeof(prowod2_key2_t));
        memcpy(key_cmp, ((Prowod2Key2*)key)->priv, sizeof(prowod2_key2_t));
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() ) {
        key_cmp = (prowod2_key3_t*)do_malloc(sizeof(prowod2_key3_t));
        memcpy(key_cmp, ((Prowod2Key3*)key)->priv, sizeof(prowod2_key3_t));
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->dtype, 
                    ((Prowod2Key0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->sklad, 
                    ((Prowod2Key0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->document, 
                    ((Prowod2Key0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->number, 
                    ((Prowod2Key0*)key)->priv->number))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->otdel, 
                    ((Prowod2Key0*)key)->priv->otdel))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->otdel, 
                    ((Prowod2Key1*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->date, 
                    ((Prowod2Key1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->time, 
                    ((Prowod2Key1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->otdel, 
                    ((Prowod2Key2*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->saldo_d, 
                    ((Prowod2Key2*)key)->priv->saldo_d))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->date, 
                    ((Prowod2Key2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->time, 
                    ((Prowod2Key2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->otdel, 
                    ((Prowod2Key3*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->saldo_k, 
                    ((Prowod2Key3*)key)->priv->saldo_k))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->date, 
                    ((Prowod2Key3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->time, 
                    ((Prowod2Key3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod2_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
            status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
            status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
            status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
            status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Prowod2_iter_gt(Prowod2* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwod2Key0Type() ) {
        key_cmp = (prowod2_key0_t*)do_malloc(sizeof(prowod2_key0_t));
        memcpy(key_cmp, ((Prowod2Key0*)key)->priv, sizeof(prowod2_key0_t));
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() ) {
        key_cmp = (prowod2_key1_t*)do_malloc(sizeof(prowod2_key1_t));
        memcpy(key_cmp, ((Prowod2Key1*)key)->priv, sizeof(prowod2_key1_t));
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() ) {
        key_cmp = (prowod2_key2_t*)do_malloc(sizeof(prowod2_key2_t));
        memcpy(key_cmp, ((Prowod2Key2*)key)->priv, sizeof(prowod2_key2_t));
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() ) {
        key_cmp = (prowod2_key3_t*)do_malloc(sizeof(prowod2_key3_t));
        memcpy(key_cmp, ((Prowod2Key3*)key)->priv, sizeof(prowod2_key3_t));
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->dtype, 
                    ((Prowod2Key0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->sklad, 
                    ((Prowod2Key0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->document, 
                    ((Prowod2Key0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->number, 
                    ((Prowod2Key0*)key)->priv->number))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->otdel, 
                    ((Prowod2Key0*)key)->priv->otdel))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->otdel, 
                    ((Prowod2Key1*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->date, 
                    ((Prowod2Key1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->time, 
                    ((Prowod2Key1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->otdel, 
                    ((Prowod2Key2*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->saldo_d, 
                    ((Prowod2Key2*)key)->priv->saldo_d))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->date, 
                    ((Prowod2Key2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->time, 
                    ((Prowod2Key2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->otdel, 
                    ((Prowod2Key3*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->saldo_k, 
                    ((Prowod2Key3*)key)->priv->saldo_k))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->date, 
                    ((Prowod2Key3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->time, 
                    ((Prowod2Key3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod2_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
            status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
            status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
            status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
            status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Prowod2_iter_ge(Prowod2* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwod2Key0Type() ) {
        key_cmp = (prowod2_key0_t*)do_malloc(sizeof(prowod2_key0_t));
        memcpy(key_cmp, ((Prowod2Key0*)key)->priv, sizeof(prowod2_key0_t));
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() ) {
        key_cmp = (prowod2_key1_t*)do_malloc(sizeof(prowod2_key1_t));
        memcpy(key_cmp, ((Prowod2Key1*)key)->priv, sizeof(prowod2_key1_t));
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() ) {
        key_cmp = (prowod2_key2_t*)do_malloc(sizeof(prowod2_key2_t));
        memcpy(key_cmp, ((Prowod2Key2*)key)->priv, sizeof(prowod2_key2_t));
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() ) {
        key_cmp = (prowod2_key3_t*)do_malloc(sizeof(prowod2_key3_t));
        memcpy(key_cmp, ((Prowod2Key3*)key)->priv, sizeof(prowod2_key3_t));
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->dtype, 
                    ((Prowod2Key0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->sklad, 
                    ((Prowod2Key0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->document, 
                    ((Prowod2Key0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->number, 
                    ((Prowod2Key0*)key)->priv->number))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->otdel, 
                    ((Prowod2Key0*)key)->priv->otdel))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->otdel, 
                    ((Prowod2Key1*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->date, 
                    ((Prowod2Key1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->time, 
                    ((Prowod2Key1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->otdel, 
                    ((Prowod2Key2*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->saldo_d, 
                    ((Prowod2Key2*)key)->priv->saldo_d))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->date, 
                    ((Prowod2Key2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->time, 
                    ((Prowod2Key2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->otdel, 
                    ((Prowod2Key3*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->saldo_k, 
                    ((Prowod2Key3*)key)->priv->saldo_k))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->date, 
                    ((Prowod2Key3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->time, 
                    ((Prowod2Key3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod2_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
            status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
            status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
            status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
            status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Prowod2_iter_lt(Prowod2* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwod2Key0Type() ) {
        key_cmp = (prowod2_key0_t*)do_malloc(sizeof(prowod2_key0_t));
        memcpy(key_cmp, ((Prowod2Key0*)key)->priv, sizeof(prowod2_key0_t));
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() ) {
        key_cmp = (prowod2_key1_t*)do_malloc(sizeof(prowod2_key1_t));
        memcpy(key_cmp, ((Prowod2Key1*)key)->priv, sizeof(prowod2_key1_t));
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() ) {
        key_cmp = (prowod2_key2_t*)do_malloc(sizeof(prowod2_key2_t));
        memcpy(key_cmp, ((Prowod2Key2*)key)->priv, sizeof(prowod2_key2_t));
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() ) {
        key_cmp = (prowod2_key3_t*)do_malloc(sizeof(prowod2_key3_t));
        memcpy(key_cmp, ((Prowod2Key3*)key)->priv, sizeof(prowod2_key3_t));
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->dtype, 
                    ((Prowod2Key0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->sklad, 
                    ((Prowod2Key0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->document, 
                    ((Prowod2Key0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->number, 
                    ((Prowod2Key0*)key)->priv->number))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->otdel, 
                    ((Prowod2Key0*)key)->priv->otdel))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->otdel, 
                    ((Prowod2Key1*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->date, 
                    ((Prowod2Key1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->time, 
                    ((Prowod2Key1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->otdel, 
                    ((Prowod2Key2*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->saldo_d, 
                    ((Prowod2Key2*)key)->priv->saldo_d))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->date, 
                    ((Prowod2Key2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->time, 
                    ((Prowod2Key2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->otdel, 
                    ((Prowod2Key3*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->saldo_k, 
                    ((Prowod2Key3*)key)->priv->saldo_k))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->date, 
                    ((Prowod2Key3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->time, 
                    ((Prowod2Key3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod2_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
            status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
            status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
            status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
            status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Prowod2_iter_le(Prowod2* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwod2Key0Type() ) {
        key_cmp = (prowod2_key0_t*)do_malloc(sizeof(prowod2_key0_t));
        memcpy(key_cmp, ((Prowod2Key0*)key)->priv, sizeof(prowod2_key0_t));
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() ) {
        key_cmp = (prowod2_key1_t*)do_malloc(sizeof(prowod2_key1_t));
        memcpy(key_cmp, ((Prowod2Key1*)key)->priv, sizeof(prowod2_key1_t));
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() ) {
        key_cmp = (prowod2_key2_t*)do_malloc(sizeof(prowod2_key2_t));
        memcpy(key_cmp, ((Prowod2Key2*)key)->priv, sizeof(prowod2_key2_t));
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() ) {
        key_cmp = (prowod2_key3_t*)do_malloc(sizeof(prowod2_key3_t));
        memcpy(key_cmp, ((Prowod2Key3*)key)->priv, sizeof(prowod2_key3_t));
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->dtype, 
                    ((Prowod2Key0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->sklad, 
                    ((Prowod2Key0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->document, 
                    ((Prowod2Key0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->number, 
                    ((Prowod2Key0*)key)->priv->number))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->otdel, 
                    ((Prowod2Key0*)key)->priv->otdel))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->otdel, 
                    ((Prowod2Key1*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->date, 
                    ((Prowod2Key1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->time, 
                    ((Prowod2Key1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->otdel, 
                    ((Prowod2Key2*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->saldo_d, 
                    ((Prowod2Key2*)key)->priv->saldo_d))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->date, 
                    ((Prowod2Key2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->time, 
                    ((Prowod2Key2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->otdel, 
                    ((Prowod2Key3*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->saldo_k, 
                    ((Prowod2Key3*)key)->priv->saldo_k))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->date, 
                    ((Prowod2Key3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->time, 
                    ((Prowod2Key3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod2_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
            status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
            status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
            status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
            status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Prowod2_iter_first(Prowod2* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwod2Key0Type() ) {
        key_cmp = (prowod2_key0_t*)do_malloc(sizeof(prowod2_key0_t));
        memcpy(key_cmp, ((Prowod2Key0*)key)->priv, sizeof(prowod2_key0_t));
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() ) {
        key_cmp = (prowod2_key1_t*)do_malloc(sizeof(prowod2_key1_t));
        memcpy(key_cmp, ((Prowod2Key1*)key)->priv, sizeof(prowod2_key1_t));
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() ) {
        key_cmp = (prowod2_key2_t*)do_malloc(sizeof(prowod2_key2_t));
        memcpy(key_cmp, ((Prowod2Key2*)key)->priv, sizeof(prowod2_key2_t));
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() ) {
        key_cmp = (prowod2_key3_t*)do_malloc(sizeof(prowod2_key3_t));
        memcpy(key_cmp, ((Prowod2Key3*)key)->priv, sizeof(prowod2_key3_t));
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->dtype, 
                    ((Prowod2Key0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->sklad, 
                    ((Prowod2Key0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->document, 
                    ((Prowod2Key0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->number, 
                    ((Prowod2Key0*)key)->priv->number))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->otdel, 
                    ((Prowod2Key0*)key)->priv->otdel))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->otdel, 
                    ((Prowod2Key1*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->date, 
                    ((Prowod2Key1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->time, 
                    ((Prowod2Key1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->otdel, 
                    ((Prowod2Key2*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->saldo_d, 
                    ((Prowod2Key2*)key)->priv->saldo_d))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->date, 
                    ((Prowod2Key2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->time, 
                    ((Prowod2Key2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->otdel, 
                    ((Prowod2Key3*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->saldo_k, 
                    ((Prowod2Key3*)key)->priv->saldo_k))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->date, 
                    ((Prowod2Key3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->time, 
                    ((Prowod2Key3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod2_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
            status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
            status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
            status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
            status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Prowod2_iter_last(Prowod2* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwod2Key0Type() ) {
        key_cmp = (prowod2_key0_t*)do_malloc(sizeof(prowod2_key0_t));
        memcpy(key_cmp, ((Prowod2Key0*)key)->priv, sizeof(prowod2_key0_t));
        status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key1Type() ) {
        key_cmp = (prowod2_key1_t*)do_malloc(sizeof(prowod2_key1_t));
        memcpy(key_cmp, ((Prowod2Key1*)key)->priv, sizeof(prowod2_key1_t));
        status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key2Type() ) {
        key_cmp = (prowod2_key2_t*)do_malloc(sizeof(prowod2_key2_t));
        memcpy(key_cmp, ((Prowod2Key2*)key)->priv, sizeof(prowod2_key2_t));
        status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProwod2Key3Type() ) {
        key_cmp = (prowod2_key3_t*)do_malloc(sizeof(prowod2_key3_t));
        memcpy(key_cmp, ((Prowod2Key3*)key)->priv, sizeof(prowod2_key3_t));
        status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->dtype, 
                    ((Prowod2Key0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->sklad, 
                    ((Prowod2Key0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->document, 
                    ((Prowod2Key0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->number, 
                    ((Prowod2Key0*)key)->priv->number))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod2_key0_t*)key_cmp)->otdel, 
                    ((Prowod2Key0*)key)->priv->otdel))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->otdel, 
                    ((Prowod2Key1*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->date, 
                    ((Prowod2Key1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key1_t*)key_cmp)->time, 
                    ((Prowod2Key1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->otdel, 
                    ((Prowod2Key2*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->saldo_d, 
                    ((Prowod2Key2*)key)->priv->saldo_d))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->date, 
                    ((Prowod2Key2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key2_t*)key_cmp)->time, 
                    ((Prowod2Key2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->otdel, 
                    ((Prowod2Key3*)key)->priv->otdel))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->saldo_k, 
                    ((Prowod2Key3*)key)->priv->saldo_k))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->date, 
                    ((Prowod2Key3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod2_key3_t*)key_cmp)->time, 
                    ((Prowod2Key3*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod2_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwod2Key0Type() ) {
            status = do_prowod2_get0(self->alias->alias, self->priv, ((Prowod2Key0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key1Type() ) {
            status = do_prowod2_get1(self->alias->alias, self->priv, ((Prowod2Key1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key2Type() ) {
            status = do_prowod2_get2(self->alias->alias, self->priv, ((Prowod2Key2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwod2Key3Type() ) {
            status = do_prowod2_get3(self->alias->alias, self->priv, ((Prowod2Key3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Prowod2_insert(Prowod2* self)
{
    int status;
    status = do_prowod2_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_update(Prowod2* self)
{
    int status;
    status = do_prowod2_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_delete(Prowod2* self)
{
    int status;
    status = do_prowod2_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2_set_alias(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Prowod2*)self)->alias);
    Py_INCREF(obj);
    ((Prowod2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Prowod2_set(Prowod2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwod2Type() ) {
        memcpy(self->priv, ((Prowod2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwod2Key0Type() ) {

        do_cpy(self->priv->data.dtype, ((Prowod2Key0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((Prowod2Key0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((Prowod2Key0*)obj)->priv->document);

        do_cpy(self->priv->data.number, ((Prowod2Key0*)obj)->priv->number);

        do_cpy(self->priv->data.otdel, ((Prowod2Key0*)obj)->priv->otdel);

    }

    else 
    if ( Py_TYPE(obj) == getProwod2Key1Type() ) {

        do_cpy(self->priv->data.otdel, ((Prowod2Key1*)obj)->priv->otdel);

        do_cpy(self->priv->data.date, ((Prowod2Key1*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((Prowod2Key1*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getProwod2Key2Type() ) {

        do_cpy(self->priv->data.otdel, ((Prowod2Key2*)obj)->priv->otdel);

        do_cpy(self->priv->data.date, ((Prowod2Key2*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((Prowod2Key2*)obj)->priv->time);

        do_cpy(self->priv->data.saldo_d, ((Prowod2Key2*)obj)->priv->saldo_d);

    }

    else 
    if ( Py_TYPE(obj) == getProwod2Key3Type() ) {

        do_cpy(self->priv->data.otdel, ((Prowod2Key3*)obj)->priv->otdel);

        do_cpy(self->priv->data.date, ((Prowod2Key3*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((Prowod2Key3*)obj)->priv->time);

        do_cpy(self->priv->data.saldo_k, ((Prowod2Key3*)obj)->priv->saldo_k);

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
            if ( !strcmp("number", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_number", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
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
            if ( !strcmp("saldo_debit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_saldo_debit", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("saldo_credit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_saldo_credit", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sum", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sum", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sums", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sums", "O", value) )
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
                do_log(LOG_ERR, "Class \"Prowod2\"does not contain \"%s\"", field_name);
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

static PyObject *Prowod2_fields(Prowod2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Prowod2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Prowod2_document_type(self,NULL);
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

    value =  Prowod2_store(self,NULL);
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

    value =  Prowod2_document(self,NULL);
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

    value =  Prowod2_number(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("number", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2_division(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("division", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2_date(self,NULL);
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

    value =  Prowod2_time(self,NULL);
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

    value =  Prowod2_saldo_debit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("saldo_debit", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2_saldo_credit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("saldo_credit", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Prowod2_get_sums(self, NULL);
    buf = get_quants_desc(value, "sums");
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Prowod2_get_params(self, NULL);
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

static PyGetSetDef Prowod2_getset[] = {

    {"document_type",(getter)Prowod2_document_type},

    {"store",(getter)Prowod2_store},

    {"document",(getter)Prowod2_document},

    {"number",(getter)Prowod2_number},

    {"division",(getter)Prowod2_division},

    {"date",(getter)Prowod2_date},

    {"date_raw",(getter)Prowod2_date_raw},

    {"time",(getter)Prowod2_time},

    {"time_raw",(getter)Prowod2_time_raw},

    {"saldo_debit",(getter)Prowod2_saldo_debit},

    {"saldo_credit",(getter)Prowod2_saldo_credit},

    {"sums",(getter)Prowod2_get_sums},

    {"params",(getter)Prowod2_get_params},

    {NULL}
};

static PyMethodDef Prowod2_methods[] = {
    {"set", (PyCFunction)Prowod2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Prowod2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Prowod2_clone, METH_NOARGS, "clone"},

    {"clear_sums", (PyCFunction)Prowod2_sums_clear, METH_NOARGS, "do_Prowod2_sums_clear"},
    {"sum", (PyCFunction)Prowod2_get_sum, METH_VARARGS|METH_KEYWORDS, "do_Prowod2_sum"},
    {"set_sums", (PyCFunction)Prowod2_set_sums, METH_VARARGS|METH_KEYWORDS, "do_Prowod2_set_sums"},

    {"set_document_type", (PyCFunction)Prowod2_set_document_type, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_document_type"},

    {"set_store", (PyCFunction)Prowod2_set_store, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_store"},

    {"set_document", (PyCFunction)Prowod2_set_document, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_document"},

    {"set_number", (PyCFunction)Prowod2_set_number, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_number"},

    {"set_division", (PyCFunction)Prowod2_set_division, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_division"},

    {"set_date", (PyCFunction)Prowod2_set_date, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_date"},

    {"set_time", (PyCFunction)Prowod2_set_time, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_time"},

    {"set_saldo_debit", (PyCFunction)Prowod2_set_saldo_debit, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_saldo_debit"},

    {"set_saldo_credit", (PyCFunction)Prowod2_set_saldo_credit, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_saldo_credit"},

    {"set_sum", (PyCFunction)Prowod2_set_sum, METH_VARARGS|METH_KEYWORDS, "Prowod2_set_sum"},

    {"set_param", (PyCFunction)Prowod2_set_param, METH_VARARGS|METH_KEYWORDS, "do_Prowod2_param_set"},
    {"param", (PyCFunction)Prowod2_get_param, METH_VARARGS|METH_KEYWORDS, "do_Prowod2_param"},
    {"clear_params", (PyCFunction)Prowod2_params_clear, METH_NOARGS, "do_Prowod2_param_clear"},
    {"set_params", (PyCFunction)Prowod2_set_params, METH_VARARGS|METH_KEYWORDS, "do_Prowod2_set_params"},

    {"get_equal", (PyCFunction)Prowod2_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2_equal"},

    {"get_next", (PyCFunction)Prowod2_next, METH_VARARGS|METH_KEYWORDS, "Prowod2_next"},

    {"get_prev", (PyCFunction)Prowod2_prev, METH_VARARGS|METH_KEYWORDS, "Prowod2_prev"},

    {"get_gt", (PyCFunction)Prowod2_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2_gt"},

    {"get_ge", (PyCFunction)Prowod2_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2_ge"},

    {"get_lt", (PyCFunction)Prowod2_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2_lt"},

    {"get_le", (PyCFunction)Prowod2_le, METH_VARARGS|METH_KEYWORDS, "Prowod2_le"},

    {"get_first", (PyCFunction)Prowod2_first, METH_VARARGS|METH_KEYWORDS, "Prowod2_first"},

    {"get_last", (PyCFunction)Prowod2_last, METH_VARARGS|METH_KEYWORDS, "Prowod2_last"},

    {"gets_equal", (PyCFunction)Prowod2_iter_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2_iter_equal"},

    {"gets_gt", (PyCFunction)Prowod2_iter_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2_iter_gt"},

    {"gets_ge", (PyCFunction)Prowod2_iter_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2_iter_ge"},

    {"gets_lt", (PyCFunction)Prowod2_iter_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2_iter_lt"},

    {"gets_le", (PyCFunction)Prowod2_iter_le, METH_VARARGS|METH_KEYWORDS, "Prowod2_iter_le"},

    {"gets_first", (PyCFunction)Prowod2_iter_first, METH_VARARGS|METH_KEYWORDS, "Prowod2_iter_first"},

    {"gets_last", (PyCFunction)Prowod2_iter_last, METH_VARARGS|METH_KEYWORDS, "Prowod2_iter_last"},

    {"insert", (PyCFunction)Prowod2_insert, METH_VARARGS|METH_KEYWORDS, "Prowod2_insert"},

    {"update", (PyCFunction)Prowod2_update, METH_VARARGS|METH_KEYWORDS, "Prowod2_update"},

    {"delete", (PyCFunction)Prowod2_delete, METH_VARARGS|METH_KEYWORDS, "Prowod2_delete"},

    {NULL}
};

static PyTypeObject Prowod2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Prowod2",             /*tp_name*/
    sizeof(Prowod2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Prowod2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2_methods,             /* tp_methods */
    0,
    Prowod2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2_new,                 /* tp_new */
};
PyTypeObject *getProwod2Type()
{
    return &Prowod2Type_;
}

static void Prowod2Key0_dealloc(PyObject* self)
{
    do_free(((Prowod2Key0*)self)->priv);
    Py_DECREF(((Prowod2Key0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Prowod2Key0 free\n");
}
static PyObject *Prowod2Key0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2Key0 new\n");
    Prowod2Key0 *self;
    self = PyObject_NEW(Prowod2Key0, type);
    //self = (Prowod2Key0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod2_key0_t));
    return (PyObject *)self;
}
static int Prowod2Key0_init(Prowod2Key0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("Prowod2Key0 init\n");
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

    do_text_set_empty(self->priv->otdel);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Prowod2Key0_clone(Prowod2Key0 *self)
{
    Prowod2Key0 *result;
    result = (Prowod2Key0*)PyObject_CallFunction((PyObject*)getProwod2Key0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *Prowod2Key0_document_type(Prowod2Key0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2Key0_set_document_type(Prowod2Key0* self, PyObject *args, PyObject *kwds)
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

static PyObject *Prowod2Key0_store(Prowod2Key0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2Key0_set_store(Prowod2Key0* self, PyObject *args, PyObject *kwds)
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

static PyObject *Prowod2Key0_document(Prowod2Key0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2Key0_set_document(Prowod2Key0* self, PyObject *args, PyObject *kwds)
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

static PyObject *Prowod2Key0_number(Prowod2Key0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->number);

    return result;
}

static PyObject *Prowod2Key0_set_number(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set number");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->number = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->number = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set number");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->number);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key0_division(Prowod2Key0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->otdel);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2Key0_set_division(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set otdel");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->otdel, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->otdel);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key0_equal(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_next(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_prev(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_gt(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_ge(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_lt(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_le(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_first(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_last(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key0_iter_equal(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
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
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

 
        item = Prowod2Key0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key0_iter_gt(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_GT);
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
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

 
        item = Prowod2Key0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key0_iter_ge(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_GE);
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
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

 
        item = Prowod2Key0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key0_iter_lt(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_LT);
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
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

 
        item = Prowod2Key0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key0_iter_le(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_LE);
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
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

 
        item = Prowod2Key0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key0_iter_first(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_FIRST);
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
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

 
        item = Prowod2Key0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key0_iter_last(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_LAST);
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
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

 
        item = Prowod2Key0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key0_cmp(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwod2Key0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((Prowod2Key0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((Prowod2Key0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((Prowod2Key0*)obj)->priv->document);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->number - ((Prowod2Key0*)obj)->priv->number;
       }

       if ( !res && depth >= 5 ) {
           res = do_cmp(self->priv->otdel, ((Prowod2Key0*)obj)->priv->otdel);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *Prowod2Key0_set_alias(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Prowod2Key0*)self)->alias);
    Py_INCREF(obj);
    ((Prowod2Key0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Prowod2Key0_set(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwod2Key0Type() ) {
        memcpy(self->priv, ((Prowod2Key0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwod2Type() ) {

        do_cpy(self->priv->dtype, ((Prowod2*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((Prowod2*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((Prowod2*)obj)->priv->data.document);

        do_cpy(self->priv->number, ((Prowod2*)obj)->priv->data.number);

        do_cpy(self->priv->otdel, ((Prowod2*)obj)->priv->data.otdel);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_number", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
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
            if ( !strcmp("number", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_number", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Prowod2Key0\"does not contain \"%s\"", field_name);
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

static PyObject *Prowod2Key0_fields(Prowod2Key0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Prowod2Key0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Prowod2Key0_document_type(self,NULL);
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

    value =  Prowod2Key0_store(self,NULL);
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

    value =  Prowod2Key0_document(self,NULL);
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

    value =  Prowod2Key0_number(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("number", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2Key0_division(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("division", value);
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

static PyGetSetDef Prowod2Key0_getset[] = {

    {"document_type",(getter)Prowod2Key0_document_type},

    {"store",(getter)Prowod2Key0_store},

    {"document",(getter)Prowod2Key0_document},

    {"number",(getter)Prowod2Key0_number},

    {"division",(getter)Prowod2Key0_division},

    {NULL}
};

static PyMethodDef Prowod2Key0_methods[] = {
    {"set", (PyCFunction)Prowod2Key0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Prowod2Key0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Prowod2Key0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)Prowod2Key0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)Prowod2Key0_set_document_type, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_set_document_type"},

    {"set_store", (PyCFunction)Prowod2Key0_set_store, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_set_store"},

    {"set_document", (PyCFunction)Prowod2Key0_set_document, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_set_document"},

    {"set_number", (PyCFunction)Prowod2Key0_set_number, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_set_number"},

    {"set_division", (PyCFunction)Prowod2Key0_set_division, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_set_division"},

    {"get_equal", (PyCFunction)Prowod2Key0_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_equal"},

    {"get_next", (PyCFunction)Prowod2Key0_next, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_next"},

    {"get_prev", (PyCFunction)Prowod2Key0_prev, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_prev"},

    {"get_gt", (PyCFunction)Prowod2Key0_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_gt"},

    {"get_ge", (PyCFunction)Prowod2Key0_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_ge"},

    {"get_lt", (PyCFunction)Prowod2Key0_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_lt"},

    {"get_le", (PyCFunction)Prowod2Key0_le, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_le"},

    {"get_first", (PyCFunction)Prowod2Key0_first, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_first"},

    {"get_last", (PyCFunction)Prowod2Key0_last, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_last"},

    {"gets_equal", (PyCFunction)Prowod2Key0_iter_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_iter_equal"},

    {"gets_gt", (PyCFunction)Prowod2Key0_iter_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_iter_gt"},

    {"gets_ge", (PyCFunction)Prowod2Key0_iter_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_iter_ge"},

    {"gets_lt", (PyCFunction)Prowod2Key0_iter_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_iter_lt"},

    {"gets_le", (PyCFunction)Prowod2Key0_iter_le, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_iter_le"},

    {"gets_first", (PyCFunction)Prowod2Key0_iter_first, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_iter_first"},

    {"gets_last", (PyCFunction)Prowod2Key0_iter_last, METH_VARARGS|METH_KEYWORDS, "Prowod2Key0_iter_last"},

    {NULL}
};

static PyTypeObject Prowod2Key0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Prowod2Key0",             /*tp_name*/
    sizeof(Prowod2Key0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2Key0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Prowod2Key0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2Key0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2Key0_methods,             /* tp_methods */
    0,
    Prowod2Key0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2Key0_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2Key0_new,                 /* tp_new */
};
PyTypeObject *getProwod2Key0Type()
{
    return &Prowod2Key0Type_;
}

static void Prowod2Key1_dealloc(PyObject* self)
{
    do_free(((Prowod2Key1*)self)->priv);
    Py_DECREF(((Prowod2Key1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Prowod2Key1 free\n");
}
static PyObject *Prowod2Key1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2Key1 new\n");
    Prowod2Key1 *self;
    self = PyObject_NEW(Prowod2Key1, type);
    //self = (Prowod2Key1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod2_key1_t));
    return (PyObject *)self;
}
static int Prowod2Key1_init(Prowod2Key1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("Prowod2Key1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->otdel);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Prowod2Key1_clone(Prowod2Key1 *self)
{
    Prowod2Key1 *result;
    result = (Prowod2Key1*)PyObject_CallFunction((PyObject*)getProwod2Key1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *Prowod2Key1_division(Prowod2Key1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->otdel);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2Key1_set_division(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set otdel");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->otdel, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->otdel);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key1_date(Prowod2Key1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Prowod2Key1_date_raw(Prowod2Key1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *Prowod2Key1_set_date(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key1_time(Prowod2Key1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Prowod2Key1_time_raw(Prowod2Key1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *Prowod2Key1_set_time(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
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
             self->priv->time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key1_equal(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_next(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_prev(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_gt(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_ge(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_lt(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_le(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_first(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_last(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key1_iter_equal(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
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

 
        item = Prowod2Key1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key1_iter_gt(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
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

 
        item = Prowod2Key1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key1_iter_ge(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
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

 
        item = Prowod2Key1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key1_iter_lt(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
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

 
        item = Prowod2Key1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key1_iter_le(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
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

 
        item = Prowod2Key1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key1_iter_first(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
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

 
        item = Prowod2Key1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key1_iter_last(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
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

 
        item = Prowod2Key1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key1_cmp(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwod2Key1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->otdel, ((Prowod2Key1*)obj)->priv->otdel);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((Prowod2Key1*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->time - ((Prowod2Key1*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *Prowod2Key1_set_alias(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Prowod2Key1*)self)->alias);
    Py_INCREF(obj);
    ((Prowod2Key1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Prowod2Key1_set(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwod2Key1Type() ) {
        memcpy(self->priv, ((Prowod2Key1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwod2Type() ) {

        do_cpy(self->priv->otdel, ((Prowod2*)obj)->priv->data.otdel);

        do_cpy(self->priv->date, ((Prowod2*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Prowod2*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
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
                do_log(LOG_ERR, "Class \"Prowod2Key1\"does not contain \"%s\"", field_name);
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

static PyObject *Prowod2Key1_fields(Prowod2Key1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Prowod2Key1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Prowod2Key1_division(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("division", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2Key1_date(self,NULL);
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

    value =  Prowod2Key1_time(self,NULL);
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

static PyGetSetDef Prowod2Key1_getset[] = {

    {"division",(getter)Prowod2Key1_division},

    {"date",(getter)Prowod2Key1_date},

    {"date_raw",(getter)Prowod2Key1_date_raw},

    {"time",(getter)Prowod2Key1_time},

    {"time_raw",(getter)Prowod2Key1_time_raw},

    {NULL}
};

static PyMethodDef Prowod2Key1_methods[] = {
    {"set", (PyCFunction)Prowod2Key1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Prowod2Key1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Prowod2Key1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)Prowod2Key1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)Prowod2Key1_set_division, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_set_division"},

    {"set_date", (PyCFunction)Prowod2Key1_set_date, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_set_date"},

    {"set_time", (PyCFunction)Prowod2Key1_set_time, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_set_time"},

    {"get_equal", (PyCFunction)Prowod2Key1_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_equal"},

    {"get_next", (PyCFunction)Prowod2Key1_next, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_next"},

    {"get_prev", (PyCFunction)Prowod2Key1_prev, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_prev"},

    {"get_gt", (PyCFunction)Prowod2Key1_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_gt"},

    {"get_ge", (PyCFunction)Prowod2Key1_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_ge"},

    {"get_lt", (PyCFunction)Prowod2Key1_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_lt"},

    {"get_le", (PyCFunction)Prowod2Key1_le, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_le"},

    {"get_first", (PyCFunction)Prowod2Key1_first, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_first"},

    {"get_last", (PyCFunction)Prowod2Key1_last, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_last"},

    {"gets_equal", (PyCFunction)Prowod2Key1_iter_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_iter_equal"},

    {"gets_gt", (PyCFunction)Prowod2Key1_iter_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_iter_gt"},

    {"gets_ge", (PyCFunction)Prowod2Key1_iter_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_iter_ge"},

    {"gets_lt", (PyCFunction)Prowod2Key1_iter_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_iter_lt"},

    {"gets_le", (PyCFunction)Prowod2Key1_iter_le, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_iter_le"},

    {"gets_first", (PyCFunction)Prowod2Key1_iter_first, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_iter_first"},

    {"gets_last", (PyCFunction)Prowod2Key1_iter_last, METH_VARARGS|METH_KEYWORDS, "Prowod2Key1_iter_last"},

    {NULL}
};

static PyTypeObject Prowod2Key1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Prowod2Key1",             /*tp_name*/
    sizeof(Prowod2Key1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2Key1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Prowod2Key1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2Key1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2Key1_methods,             /* tp_methods */
    0,
    Prowod2Key1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2Key1_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2Key1_new,                 /* tp_new */
};
PyTypeObject *getProwod2Key1Type()
{
    return &Prowod2Key1Type_;
}

static void Prowod2Key2_dealloc(PyObject* self)
{
    do_free(((Prowod2Key2*)self)->priv);
    Py_DECREF(((Prowod2Key2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Prowod2Key2 free\n");
}
static PyObject *Prowod2Key2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2Key2 new\n");
    Prowod2Key2 *self;
    self = PyObject_NEW(Prowod2Key2, type);
    //self = (Prowod2Key2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod2_key2_t));
    return (PyObject *)self;
}
static int Prowod2Key2_init(Prowod2Key2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("Prowod2Key2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->otdel);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Prowod2Key2_clone(Prowod2Key2 *self)
{
    Prowod2Key2 *result;
    result = (Prowod2Key2*)PyObject_CallFunction((PyObject*)getProwod2Key2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *Prowod2Key2_division(Prowod2Key2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->otdel);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2Key2_set_division(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set otdel");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->otdel, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->otdel);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key2_saldo_debit(Prowod2Key2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->saldo_d);

    return result;
}

static PyObject *Prowod2Key2_set_saldo_debit(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set saldo_d");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->saldo_d = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->saldo_d = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set saldo_d");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->saldo_d);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key2_date(Prowod2Key2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Prowod2Key2_date_raw(Prowod2Key2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *Prowod2Key2_set_date(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key2_time(Prowod2Key2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Prowod2Key2_time_raw(Prowod2Key2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *Prowod2Key2_set_time(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
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
             self->priv->time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key2_equal(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_next(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_prev(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_gt(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_ge(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_lt(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_le(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_first(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_last(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key2_iter_equal(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_d, 
                 self->priv->saldo_d))
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

 
        item = Prowod2Key2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key2_iter_gt(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_d, 
                 self->priv->saldo_d))
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

 
        item = Prowod2Key2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key2_iter_ge(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_d, 
                 self->priv->saldo_d))
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

 
        item = Prowod2Key2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key2_iter_lt(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_d, 
                 self->priv->saldo_d))
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

 
        item = Prowod2Key2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key2_iter_le(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_d, 
                 self->priv->saldo_d))
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

 
        item = Prowod2Key2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key2_iter_first(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_d, 
                 self->priv->saldo_d))
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

 
        item = Prowod2Key2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key2_iter_last(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_d, 
                 self->priv->saldo_d))
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

 
        item = Prowod2Key2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key2_cmp(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwod2Key2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->otdel, ((Prowod2Key2*)obj)->priv->otdel);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->saldo_d - ((Prowod2Key2*)obj)->priv->saldo_d;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->date - ((Prowod2Key2*)obj)->priv->date;
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->time - ((Prowod2Key2*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *Prowod2Key2_set_alias(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Prowod2Key2*)self)->alias);
    Py_INCREF(obj);
    ((Prowod2Key2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Prowod2Key2_set(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwod2Key2Type() ) {
        memcpy(self->priv, ((Prowod2Key2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwod2Type() ) {

        do_cpy(self->priv->otdel, ((Prowod2*)obj)->priv->data.otdel);

        do_cpy(self->priv->saldo_d, ((Prowod2*)obj)->priv->data.saldo_d);

        do_cpy(self->priv->date, ((Prowod2*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Prowod2*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_saldo_debit", "O", value) )
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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("saldo_debit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_saldo_debit", "O", value) )
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
                do_log(LOG_ERR, "Class \"Prowod2Key2\"does not contain \"%s\"", field_name);
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

static PyObject *Prowod2Key2_fields(Prowod2Key2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Prowod2Key2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Prowod2Key2_division(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("division", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2Key2_saldo_debit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("saldo_debit", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2Key2_date(self,NULL);
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

    value =  Prowod2Key2_time(self,NULL);
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

static PyGetSetDef Prowod2Key2_getset[] = {

    {"division",(getter)Prowod2Key2_division},

    {"saldo_debit",(getter)Prowod2Key2_saldo_debit},

    {"date",(getter)Prowod2Key2_date},

    {"date_raw",(getter)Prowod2Key2_date_raw},

    {"time",(getter)Prowod2Key2_time},

    {"time_raw",(getter)Prowod2Key2_time_raw},

    {NULL}
};

static PyMethodDef Prowod2Key2_methods[] = {
    {"set", (PyCFunction)Prowod2Key2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Prowod2Key2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Prowod2Key2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)Prowod2Key2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)Prowod2Key2_set_division, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_set_division"},

    {"set_saldo_debit", (PyCFunction)Prowod2Key2_set_saldo_debit, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_set_saldo_debit"},

    {"set_date", (PyCFunction)Prowod2Key2_set_date, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_set_date"},

    {"set_time", (PyCFunction)Prowod2Key2_set_time, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_set_time"},

    {"get_equal", (PyCFunction)Prowod2Key2_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_equal"},

    {"get_next", (PyCFunction)Prowod2Key2_next, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_next"},

    {"get_prev", (PyCFunction)Prowod2Key2_prev, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_prev"},

    {"get_gt", (PyCFunction)Prowod2Key2_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_gt"},

    {"get_ge", (PyCFunction)Prowod2Key2_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_ge"},

    {"get_lt", (PyCFunction)Prowod2Key2_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_lt"},

    {"get_le", (PyCFunction)Prowod2Key2_le, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_le"},

    {"get_first", (PyCFunction)Prowod2Key2_first, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_first"},

    {"get_last", (PyCFunction)Prowod2Key2_last, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_last"},

    {"gets_equal", (PyCFunction)Prowod2Key2_iter_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_iter_equal"},

    {"gets_gt", (PyCFunction)Prowod2Key2_iter_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_iter_gt"},

    {"gets_ge", (PyCFunction)Prowod2Key2_iter_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_iter_ge"},

    {"gets_lt", (PyCFunction)Prowod2Key2_iter_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_iter_lt"},

    {"gets_le", (PyCFunction)Prowod2Key2_iter_le, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_iter_le"},

    {"gets_first", (PyCFunction)Prowod2Key2_iter_first, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_iter_first"},

    {"gets_last", (PyCFunction)Prowod2Key2_iter_last, METH_VARARGS|METH_KEYWORDS, "Prowod2Key2_iter_last"},

    {NULL}
};

static PyTypeObject Prowod2Key2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Prowod2Key2",             /*tp_name*/
    sizeof(Prowod2Key2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2Key2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Prowod2Key2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2Key2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2Key2_methods,             /* tp_methods */
    0,
    Prowod2Key2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2Key2_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2Key2_new,                 /* tp_new */
};
PyTypeObject *getProwod2Key2Type()
{
    return &Prowod2Key2Type_;
}

static void Prowod2Key3_dealloc(PyObject* self)
{
    do_free(((Prowod2Key3*)self)->priv);
    Py_DECREF(((Prowod2Key3*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Prowod2Key3 free\n");
}
static PyObject *Prowod2Key3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Prowod2Key3 new\n");
    Prowod2Key3 *self;
    self = PyObject_NEW(Prowod2Key3, type);
    //self = (Prowod2Key3 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod2_key3_t));
    return (PyObject *)self;
}
static int Prowod2Key3_init(Prowod2Key3 *self, PyObject *args, PyObject *kwds)
{
    //LOG("Prowod2Key3 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->otdel);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Prowod2Key3_clone(Prowod2Key3 *self)
{
    Prowod2Key3 *result;
    result = (Prowod2Key3*)PyObject_CallFunction((PyObject*)getProwod2Key3Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *Prowod2Key3_division(Prowod2Key3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->otdel);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod2Key3_set_division(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set otdel");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->otdel, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->otdel);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key3_saldo_credit(Prowod2Key3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->saldo_k);

    return result;
}

static PyObject *Prowod2Key3_set_saldo_credit(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set saldo_k");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->saldo_k = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->saldo_k = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set saldo_k");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->saldo_k);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key3_date(Prowod2Key3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Prowod2Key3_date_raw(Prowod2Key3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *Prowod2Key3_set_date(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key3_time(Prowod2Key3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Prowod2Key3_time_raw(Prowod2Key3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *Prowod2Key3_set_time(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
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
             self->priv->time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod2Key3_equal(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_next(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_prev(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_gt(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_ge(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_lt(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_le(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_first(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_last(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod2Key3_iter_equal(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_k, 
                 self->priv->saldo_k))
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

 
        item = Prowod2Key3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key3_iter_gt(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_k, 
                 self->priv->saldo_k))
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

 
        item = Prowod2Key3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key3_iter_ge(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_k, 
                 self->priv->saldo_k))
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

 
        item = Prowod2Key3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key3_iter_lt(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_k, 
                 self->priv->saldo_k))
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

 
        item = Prowod2Key3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key3_iter_le(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_k, 
                 self->priv->saldo_k))
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

 
        item = Prowod2Key3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key3_iter_first(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_k, 
                 self->priv->saldo_k))
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

 
        item = Prowod2Key3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key3_iter_last(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod2_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.otdel, 
                 self->priv->otdel))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.saldo_k, 
                 self->priv->saldo_k))
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

 
        item = Prowod2Key3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod2_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *Prowod2Key3_cmp(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwod2Key3Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->otdel, ((Prowod2Key3*)obj)->priv->otdel);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->saldo_k - ((Prowod2Key3*)obj)->priv->saldo_k;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->date - ((Prowod2Key3*)obj)->priv->date;
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->time - ((Prowod2Key3*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *Prowod2Key3_set_alias(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Prowod2Key3*)self)->alias);
    Py_INCREF(obj);
    ((Prowod2Key3*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Prowod2Key3_set(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwod2Key3Type() ) {
        memcpy(self->priv, ((Prowod2Key3*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwod2Type() ) {

        do_cpy(self->priv->otdel, ((Prowod2*)obj)->priv->data.otdel);

        do_cpy(self->priv->saldo_k, ((Prowod2*)obj)->priv->data.saldo_k);

        do_cpy(self->priv->date, ((Prowod2*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Prowod2*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_saldo_credit", "O", value) )
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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("saldo_credit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_saldo_credit", "O", value) )
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
                do_log(LOG_ERR, "Class \"Prowod2Key3\"does not contain \"%s\"", field_name);
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

static PyObject *Prowod2Key3_fields(Prowod2Key3* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Prowod2Key3");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Prowod2Key3_division(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("division", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2Key3_saldo_credit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("saldo_credit", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod2Key3_date(self,NULL);
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

    value =  Prowod2Key3_time(self,NULL);
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

static PyGetSetDef Prowod2Key3_getset[] = {

    {"division",(getter)Prowod2Key3_division},

    {"saldo_credit",(getter)Prowod2Key3_saldo_credit},

    {"date",(getter)Prowod2Key3_date},

    {"date_raw",(getter)Prowod2Key3_date_raw},

    {"time",(getter)Prowod2Key3_time},

    {"time_raw",(getter)Prowod2Key3_time_raw},

    {NULL}
};

static PyMethodDef Prowod2Key3_methods[] = {
    {"set", (PyCFunction)Prowod2Key3_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Prowod2Key3_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Prowod2Key3_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)Prowod2Key3_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)Prowod2Key3_set_division, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_set_division"},

    {"set_saldo_credit", (PyCFunction)Prowod2Key3_set_saldo_credit, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_set_saldo_credit"},

    {"set_date", (PyCFunction)Prowod2Key3_set_date, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_set_date"},

    {"set_time", (PyCFunction)Prowod2Key3_set_time, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_set_time"},

    {"get_equal", (PyCFunction)Prowod2Key3_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_equal"},

    {"get_next", (PyCFunction)Prowod2Key3_next, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_next"},

    {"get_prev", (PyCFunction)Prowod2Key3_prev, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_prev"},

    {"get_gt", (PyCFunction)Prowod2Key3_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_gt"},

    {"get_ge", (PyCFunction)Prowod2Key3_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_ge"},

    {"get_lt", (PyCFunction)Prowod2Key3_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_lt"},

    {"get_le", (PyCFunction)Prowod2Key3_le, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_le"},

    {"get_first", (PyCFunction)Prowod2Key3_first, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_first"},

    {"get_last", (PyCFunction)Prowod2Key3_last, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_last"},

    {"gets_equal", (PyCFunction)Prowod2Key3_iter_equal, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_iter_equal"},

    {"gets_gt", (PyCFunction)Prowod2Key3_iter_gt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_iter_gt"},

    {"gets_ge", (PyCFunction)Prowod2Key3_iter_ge, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_iter_ge"},

    {"gets_lt", (PyCFunction)Prowod2Key3_iter_lt, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_iter_lt"},

    {"gets_le", (PyCFunction)Prowod2Key3_iter_le, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_iter_le"},

    {"gets_first", (PyCFunction)Prowod2Key3_iter_first, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_iter_first"},

    {"gets_last", (PyCFunction)Prowod2Key3_iter_last, METH_VARARGS|METH_KEYWORDS, "Prowod2Key3_iter_last"},

    {NULL}
};

static PyTypeObject Prowod2Key3Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Prowod2Key3",             /*tp_name*/
    sizeof(Prowod2Key3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod2Key3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Prowod2Key3_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod2Key3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod2Key3_methods,             /* tp_methods */
    0,
    Prowod2Key3_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod2Key3_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod2Key3_new,                 /* tp_new */
};
PyTypeObject *getProwod2Key3Type()
{
    return &Prowod2Key3Type_;
}
