
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "saldo.h"

static void Saldo_dealloc(PyObject* self)
{
    do_free(((Saldo*)self)->priv);
    Py_DECREF(((Saldo*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Saldo free\n");
}
static PyObject *Saldo_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Saldo new\n");
    Saldo *self;
    self = PyObject_NEW(Saldo, type);
    //self = (Saldo *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(saldo_rec_t));
    return (PyObject *)self;
}
static int Saldo_init(Saldo *self, PyObject *args, PyObject *kwds)
{
    //LOG("Saldo init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.division);

    do_text_set_empty(self->priv->data.layer);

    do_text_set_empty(self->priv->data.account);

    do_text_set_empty(self->priv->data.subaccount);

    do_text_set_empty(self->priv->data.p_g_code);

    do_text_set_empty(self->priv->data.other);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Saldo_clone(Saldo *self)
{
    Saldo *result;
    result = (Saldo*)PyObject_CallFunction((PyObject*)getSaldoType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Saldo_division(Saldo* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.division);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Saldo_set_division(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.division");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.division, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.division);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_layer(Saldo* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.layer);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Saldo_set_layer(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.layer");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.layer, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.layer);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_account(Saldo* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Saldo_set_account(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.account");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.account, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.account);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_account_total(Saldo* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.account_total);

    return result;
}

static PyObject *Saldo_set_account_total(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.account_total");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.account_total = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.account_total = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.account_total");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.account_total);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_subaccount(Saldo* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.subaccount);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Saldo_set_subaccount(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.subaccount");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.subaccount, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.subaccount);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_subaccount_total(Saldo* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.subaccount_total);

    return result;
}

static PyObject *Saldo_set_subaccount_total(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.subaccount_total");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.subaccount_total = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.subaccount_total = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.subaccount_total");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.subaccount_total);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_partner_region_code(Saldo* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Saldo_set_partner_region_code(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_partner_code(Saldo* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.p_code);

    return result;
}

static PyObject *Saldo_set_partner_code(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_other(Saldo* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.other);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Saldo_set_other(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.other");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.other, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.other);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_date_first(Saldo* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date_n, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Saldo_set_date_first(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date_n");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date_n");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.date_n = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            do_date_set(&self->priv->data.date_n, tm);
        }
    }
    /*do_date(self->priv->data.date_n, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_date(Saldo* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Saldo_date_raw(Saldo* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *Saldo_set_date(Saldo* self, PyObject *args, PyObject *kwds)
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

static PyObject *Saldo_date_modify(Saldo* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date_modify, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Saldo_set_date_modify(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date_modify");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date_modify");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.date_modify = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            do_date_set(&self->priv->data.date_modify, tm);
        }
    }
    /*do_date(self->priv->data.date_modify, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_time_modify(Saldo* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->data.time_modify, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Saldo_set_time_modify(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time_modify");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time_modify");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.time_modify = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             do_time_set(&self->priv->data.time_modify, tm);
        }
    }
    /*do_time(self->priv->data.time_modify, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_get_debit_sum(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get data.debit_sum");
        return NULL;
    }
    result=PyFloat_FromDouble(do_pdecimal(self->priv->data.debit_sum[number], 8, 2));

    return result;
}

static PyObject *Saldo_set_debit_sum(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set data.debit_sum");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_pdecimal_set(self->priv->data.debit_sum[number], 8, 2,  MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_pdecimal_set(self->priv->data.debit_sum[number], 8, 2,  PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit_sum");
            return NULL;
        }    
    }
    //result=PyFloat_FromDouble(do_pdecimal(self->priv->data.debit_sum[number], 8, 2));

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_get_credit_sum(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get data.credit_sum");
        return NULL;
    }
    result=PyFloat_FromDouble(do_pdecimal(self->priv->data.credit_sum[number], 8, 2));

    return result;
}

static PyObject *Saldo_set_credit_sum(Saldo* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set data.credit_sum");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_pdecimal_set(self->priv->data.credit_sum[number], 8, 2,  MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_pdecimal_set(self->priv->data.credit_sum[number], 8, 2,  PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit_sum");
            return NULL;
        }    
    }
    //result=PyFloat_FromDouble(do_pdecimal(self->priv->data.credit_sum[number], 8, 2));

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Saldo_get_debit_sums(Saldo* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 3; i++ ) {
        number = i;
        quant = do_pdecimal(self->priv->data.debit_sum[number], 8, 2);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *Saldo_get_credit_sums(Saldo* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 3; i++ ) {
        number = i;
        quant = do_pdecimal(self->priv->data.credit_sum[number], 8, 2);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}

static PyObject *Saldo_gt(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_last(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_next(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_le(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_lt(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_equal(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_ge(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_prev(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_first(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSaldoKey0Type() )
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_iter_gt(Saldo* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSaldoKey0Type() ) {
        key_cmp = (saldo_key0_t*)do_malloc(sizeof(saldo_key0_t));
        memcpy(key_cmp, ((SaldoKey0*)key)->priv, sizeof(saldo_key0_t));
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->division, 
                    ((SaldoKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->layer, 
                    ((SaldoKey0*)key)->priv->layer))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account, 
                    ((SaldoKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account_total, 
                    ((SaldoKey0*)key)->priv->account_total))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount, 
                    ((SaldoKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount_total, 
                    ((SaldoKey0*)key)->priv->subaccount_total))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_g_code, 
                    ((SaldoKey0*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 8 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_code, 
                    ((SaldoKey0*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 9 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->other, 
                    ((SaldoKey0*)key)->priv->other))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Saldo_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
            status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Saldo_iter_last(Saldo* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSaldoKey0Type() ) {
        key_cmp = (saldo_key0_t*)do_malloc(sizeof(saldo_key0_t));
        memcpy(key_cmp, ((SaldoKey0*)key)->priv, sizeof(saldo_key0_t));
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->division, 
                    ((SaldoKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->layer, 
                    ((SaldoKey0*)key)->priv->layer))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account, 
                    ((SaldoKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account_total, 
                    ((SaldoKey0*)key)->priv->account_total))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount, 
                    ((SaldoKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount_total, 
                    ((SaldoKey0*)key)->priv->subaccount_total))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_g_code, 
                    ((SaldoKey0*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 8 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_code, 
                    ((SaldoKey0*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 9 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->other, 
                    ((SaldoKey0*)key)->priv->other))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Saldo_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
            status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Saldo_iter_le(Saldo* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSaldoKey0Type() ) {
        key_cmp = (saldo_key0_t*)do_malloc(sizeof(saldo_key0_t));
        memcpy(key_cmp, ((SaldoKey0*)key)->priv, sizeof(saldo_key0_t));
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->division, 
                    ((SaldoKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->layer, 
                    ((SaldoKey0*)key)->priv->layer))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account, 
                    ((SaldoKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account_total, 
                    ((SaldoKey0*)key)->priv->account_total))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount, 
                    ((SaldoKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount_total, 
                    ((SaldoKey0*)key)->priv->subaccount_total))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_g_code, 
                    ((SaldoKey0*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 8 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_code, 
                    ((SaldoKey0*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 9 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->other, 
                    ((SaldoKey0*)key)->priv->other))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Saldo_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
            status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Saldo_iter_ge(Saldo* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSaldoKey0Type() ) {
        key_cmp = (saldo_key0_t*)do_malloc(sizeof(saldo_key0_t));
        memcpy(key_cmp, ((SaldoKey0*)key)->priv, sizeof(saldo_key0_t));
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->division, 
                    ((SaldoKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->layer, 
                    ((SaldoKey0*)key)->priv->layer))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account, 
                    ((SaldoKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account_total, 
                    ((SaldoKey0*)key)->priv->account_total))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount, 
                    ((SaldoKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount_total, 
                    ((SaldoKey0*)key)->priv->subaccount_total))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_g_code, 
                    ((SaldoKey0*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 8 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_code, 
                    ((SaldoKey0*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 9 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->other, 
                    ((SaldoKey0*)key)->priv->other))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Saldo_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
            status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Saldo_iter_equal(Saldo* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSaldoKey0Type() ) {
        key_cmp = (saldo_key0_t*)do_malloc(sizeof(saldo_key0_t));
        memcpy(key_cmp, ((SaldoKey0*)key)->priv, sizeof(saldo_key0_t));
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->division, 
                    ((SaldoKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->layer, 
                    ((SaldoKey0*)key)->priv->layer))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account, 
                    ((SaldoKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account_total, 
                    ((SaldoKey0*)key)->priv->account_total))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount, 
                    ((SaldoKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount_total, 
                    ((SaldoKey0*)key)->priv->subaccount_total))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_g_code, 
                    ((SaldoKey0*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 8 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_code, 
                    ((SaldoKey0*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 9 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->other, 
                    ((SaldoKey0*)key)->priv->other))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Saldo_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
            status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Saldo_iter_lt(Saldo* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSaldoKey0Type() ) {
        key_cmp = (saldo_key0_t*)do_malloc(sizeof(saldo_key0_t));
        memcpy(key_cmp, ((SaldoKey0*)key)->priv, sizeof(saldo_key0_t));
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->division, 
                    ((SaldoKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->layer, 
                    ((SaldoKey0*)key)->priv->layer))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account, 
                    ((SaldoKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account_total, 
                    ((SaldoKey0*)key)->priv->account_total))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount, 
                    ((SaldoKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount_total, 
                    ((SaldoKey0*)key)->priv->subaccount_total))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_g_code, 
                    ((SaldoKey0*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 8 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_code, 
                    ((SaldoKey0*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 9 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->other, 
                    ((SaldoKey0*)key)->priv->other))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Saldo_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
            status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Saldo_iter_first(Saldo* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSaldoKey0Type() ) {
        key_cmp = (saldo_key0_t*)do_malloc(sizeof(saldo_key0_t));
        memcpy(key_cmp, ((SaldoKey0*)key)->priv, sizeof(saldo_key0_t));
        status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->division, 
                    ((SaldoKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->layer, 
                    ((SaldoKey0*)key)->priv->layer))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account, 
                    ((SaldoKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->account_total, 
                    ((SaldoKey0*)key)->priv->account_total))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount, 
                    ((SaldoKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->subaccount_total, 
                    ((SaldoKey0*)key)->priv->subaccount_total))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_g_code, 
                    ((SaldoKey0*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 8 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->p_code, 
                    ((SaldoKey0*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 9 ) {
                if ( do_cmp(((saldo_key0_t*)key_cmp)->other, 
                    ((SaldoKey0*)key)->priv->other))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Saldo_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSaldoKey0Type() ) {
            status = do_saldo_get0(self->alias->alias, self->priv, ((SaldoKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Saldo_insert(Saldo* self)
{
    int status;
    status = do_saldo_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_update(Saldo* self)
{
    int status;
    status = do_saldo_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_delete(Saldo* self)
{
    int status;
    status = do_saldo_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Saldo_set_alias(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Saldo*)self)->alias);
    Py_INCREF(obj);
    ((Saldo*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Saldo_set(Saldo* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getSaldoType() ) {
        memcpy(self->priv, ((Saldo*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getSaldoKey0Type() ) {

        do_cpy(self->priv->data.division, ((SaldoKey0*)obj)->priv->division);

        do_cpy(self->priv->data.layer, ((SaldoKey0*)obj)->priv->layer);

        do_cpy(self->priv->data.account, ((SaldoKey0*)obj)->priv->account);

        do_cpy(self->priv->data.account_total, ((SaldoKey0*)obj)->priv->account_total);

        do_cpy(self->priv->data.subaccount, ((SaldoKey0*)obj)->priv->subaccount);

        do_cpy(self->priv->data.subaccount_total, ((SaldoKey0*)obj)->priv->subaccount_total);

        do_cpy(self->priv->data.p_g_code, ((SaldoKey0*)obj)->priv->p_g_code);

        do_cpy(self->priv->data.p_code, ((SaldoKey0*)obj)->priv->p_code);

        do_cpy(self->priv->data.other, ((SaldoKey0*)obj)->priv->other);

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
            if ( !strcmp("layer", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_layer", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("account", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_account", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("account_total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_account_total", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("subaccount_total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount_total", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("other", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_other", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date_first", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date_first", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date_modify", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date_modify", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time_modify", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time_modify", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_sum", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_sum", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_sum", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_sum", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Saldo\"does not contain \"%s\"", field_name);
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

static PyObject *Saldo_fields(Saldo* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Saldo");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Saldo_division(self,NULL);
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

    value =  Saldo_layer(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("layer", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_account(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("account", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_account_total(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("account_total", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_subaccount_total(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("subaccount_total", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_other(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("other", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_date_first(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date_first", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_date(self,NULL);
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

    value =  Saldo_date_modify(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date_modify", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Saldo_time_modify(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time_modify", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    
    buf = do_strdup_printf("debit_sum:[%f,%f,%f]", 
    do_pdecimal(self->priv->data.debit_sum[0], 8, 2),
    do_pdecimal(self->priv->data.debit_sum[1], 8, 2),
    do_pdecimal(self->priv->data.debit_sum[2], 8, 2));
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    
    buf = do_strdup_printf("credit_sum:[%f,%f,%f]", 
    do_pdecimal(self->priv->data.credit_sum[0], 8, 2),
    do_pdecimal(self->priv->data.credit_sum[1], 8, 2),
    do_pdecimal(self->priv->data.credit_sum[2], 8, 2));
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

static PyGetSetDef Saldo_getset[] = {

    {"division",(getter)Saldo_division},

    {"layer",(getter)Saldo_layer},

    {"account",(getter)Saldo_account},

    {"account_total",(getter)Saldo_account_total},

    {"subaccount",(getter)Saldo_subaccount},

    {"subaccount_total",(getter)Saldo_subaccount_total},

    {"partner_region_code",(getter)Saldo_partner_region_code},

    {"partner_code",(getter)Saldo_partner_code},

    {"other",(getter)Saldo_other},

    {"date_first",(getter)Saldo_date_first},

    {"date",(getter)Saldo_date},

    {"date_raw",(getter)Saldo_date_raw},

    {"date_modify",(getter)Saldo_date_modify},

    {"time_modify",(getter)Saldo_time_modify},

    {"debit_sums",(getter)Saldo_get_debit_sums},

    {"crebit_sums",(getter)Saldo_get_credit_sums},

    {NULL}
};

static PyMethodDef Saldo_methods[] = {
    {"set", (PyCFunction)Saldo_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Saldo_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Saldo_clone, METH_NOARGS, "clone"},

    {"set_division", (PyCFunction)Saldo_set_division, METH_VARARGS|METH_KEYWORDS, "Saldo_set_division"},

    {"set_layer", (PyCFunction)Saldo_set_layer, METH_VARARGS|METH_KEYWORDS, "Saldo_set_layer"},

    {"set_account", (PyCFunction)Saldo_set_account, METH_VARARGS|METH_KEYWORDS, "Saldo_set_account"},

    {"set_account_total", (PyCFunction)Saldo_set_account_total, METH_VARARGS|METH_KEYWORDS, "Saldo_set_account_total"},

    {"set_subaccount", (PyCFunction)Saldo_set_subaccount, METH_VARARGS|METH_KEYWORDS, "Saldo_set_subaccount"},

    {"set_subaccount_total", (PyCFunction)Saldo_set_subaccount_total, METH_VARARGS|METH_KEYWORDS, "Saldo_set_subaccount_total"},

    {"set_partner_region_code", (PyCFunction)Saldo_set_partner_region_code, METH_VARARGS|METH_KEYWORDS, "Saldo_set_partner_region_code"},

    {"set_partner_code", (PyCFunction)Saldo_set_partner_code, METH_VARARGS|METH_KEYWORDS, "Saldo_set_partner_code"},

    {"set_other", (PyCFunction)Saldo_set_other, METH_VARARGS|METH_KEYWORDS, "Saldo_set_other"},

    {"set_date_first", (PyCFunction)Saldo_set_date_first, METH_VARARGS|METH_KEYWORDS, "Saldo_set_date_first"},

    {"set_date", (PyCFunction)Saldo_set_date, METH_VARARGS|METH_KEYWORDS, "Saldo_set_date"},

    {"set_date_modify", (PyCFunction)Saldo_set_date_modify, METH_VARARGS|METH_KEYWORDS, "Saldo_set_date_modify"},

    {"set_time_modify", (PyCFunction)Saldo_set_time_modify, METH_VARARGS|METH_KEYWORDS, "Saldo_set_time_modify"},

    {"set_debit_sum", (PyCFunction)Saldo_set_debit_sum, METH_VARARGS|METH_KEYWORDS, "Saldo_set_debit_sum"},

    {"set_credit_sum", (PyCFunction)Saldo_set_credit_sum, METH_VARARGS|METH_KEYWORDS, "Saldo_set_credit_sum"},

    {"credit_sum", (PyCFunction)Saldo_get_credit_sum, METH_VARARGS|METH_KEYWORDS,"Saldo_get_credit_sum"},

    {"debit_sum", (PyCFunction)Saldo_get_debit_sum, METH_VARARGS|METH_KEYWORDS,"Saldo_get_debit_sum"},

    {"get_gt", (PyCFunction)Saldo_gt, METH_VARARGS|METH_KEYWORDS, "Saldo_gt"},

    {"get_last", (PyCFunction)Saldo_last, METH_VARARGS|METH_KEYWORDS, "Saldo_last"},

    {"get_next", (PyCFunction)Saldo_next, METH_VARARGS|METH_KEYWORDS, "Saldo_next"},

    {"get_le", (PyCFunction)Saldo_le, METH_VARARGS|METH_KEYWORDS, "Saldo_le"},

    {"get_lt", (PyCFunction)Saldo_lt, METH_VARARGS|METH_KEYWORDS, "Saldo_lt"},

    {"get_equal", (PyCFunction)Saldo_equal, METH_VARARGS|METH_KEYWORDS, "Saldo_equal"},

    {"get_ge", (PyCFunction)Saldo_ge, METH_VARARGS|METH_KEYWORDS, "Saldo_ge"},

    {"get_prev", (PyCFunction)Saldo_prev, METH_VARARGS|METH_KEYWORDS, "Saldo_prev"},

    {"get_first", (PyCFunction)Saldo_first, METH_VARARGS|METH_KEYWORDS, "Saldo_first"},

    {"gets_gt", (PyCFunction)Saldo_iter_gt, METH_VARARGS|METH_KEYWORDS, "Saldo_iter_gt"},

    {"gets_last", (PyCFunction)Saldo_iter_last, METH_VARARGS|METH_KEYWORDS, "Saldo_iter_last"},

    {"gets_le", (PyCFunction)Saldo_iter_le, METH_VARARGS|METH_KEYWORDS, "Saldo_iter_le"},

    {"gets_ge", (PyCFunction)Saldo_iter_ge, METH_VARARGS|METH_KEYWORDS, "Saldo_iter_ge"},

    {"gets_equal", (PyCFunction)Saldo_iter_equal, METH_VARARGS|METH_KEYWORDS, "Saldo_iter_equal"},

    {"gets_lt", (PyCFunction)Saldo_iter_lt, METH_VARARGS|METH_KEYWORDS, "Saldo_iter_lt"},

    {"gets_first", (PyCFunction)Saldo_iter_first, METH_VARARGS|METH_KEYWORDS, "Saldo_iter_first"},

    {"insert", (PyCFunction)Saldo_insert, METH_VARARGS|METH_KEYWORDS, "Saldo_insert"},

    {"update", (PyCFunction)Saldo_update, METH_VARARGS|METH_KEYWORDS, "Saldo_update"},

    {"delete", (PyCFunction)Saldo_delete, METH_VARARGS|METH_KEYWORDS, "Saldo_delete"},

    {NULL}
};

static PyTypeObject SaldoType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Saldo",             /*tp_name*/
    sizeof(Saldo),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Saldo_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Saldo_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Saldo objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Saldo_methods,             /* tp_methods */
    0,
    Saldo_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Saldo_init,      /* tp_init */
    0,                         /* tp_alloc */
    Saldo_new,                 /* tp_new */
};
PyTypeObject *getSaldoType()
{
    return &SaldoType_;
}

static void SaldoKey0_dealloc(PyObject* self)
{
    do_free(((SaldoKey0*)self)->priv);
    Py_DECREF(((SaldoKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("SaldoKey0 free\n");
}
static PyObject *SaldoKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("SaldoKey0 new\n");
    SaldoKey0 *self;
    self = PyObject_NEW(SaldoKey0, type);
    //self = (SaldoKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(saldo_key0_t));
    return (PyObject *)self;
}
static int SaldoKey0_init(SaldoKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("SaldoKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->division);

    do_text_set_empty(self->priv->layer);

    do_text_set_empty(self->priv->account);

    do_text_set_empty(self->priv->subaccount);

    do_text_set_empty(self->priv->p_g_code);

    do_text_set_empty(self->priv->other);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *SaldoKey0_clone(SaldoKey0 *self)
{
    SaldoKey0 *result;
    result = (SaldoKey0*)PyObject_CallFunction((PyObject*)getSaldoKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *SaldoKey0_division(SaldoKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SaldoKey0_set_division(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set division");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->division, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_layer(SaldoKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->layer);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SaldoKey0_set_layer(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set layer");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->layer, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->layer);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_account(SaldoKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SaldoKey0_set_account(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set account");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->account, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->account);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_account_total(SaldoKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->account_total);

    return result;
}

static PyObject *SaldoKey0_set_account_total(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set account_total");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->account_total = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->account_total = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set account_total");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->account_total);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_subaccount(SaldoKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->subaccount);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SaldoKey0_set_subaccount(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set subaccount");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->subaccount, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->subaccount);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_subaccount_total(SaldoKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->subaccount_total);

    return result;
}

static PyObject *SaldoKey0_set_subaccount_total(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set subaccount_total");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->subaccount_total = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->subaccount_total = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set subaccount_total");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->subaccount_total);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_partner_region_code(SaldoKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SaldoKey0_set_partner_region_code(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_partner_code(SaldoKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->p_code);

    return result;
}

static PyObject *SaldoKey0_set_partner_code(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_other(SaldoKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->other);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SaldoKey0_set_other(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set other");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->other, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->other);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SaldoKey0_gt(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_last(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_next(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_le(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_lt(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_equal(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_ge(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_prev(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_first(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SaldoKey0_iter_gt(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    saldo_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.layer, 
                 self->priv->layer))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.account_total, 
                 self->priv->account_total))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.subaccount_total, 
                 self->priv->subaccount_total))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 8 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 9 ) {
           if ( do_cmp(key_cmp.other, 
                 self->priv->other))
               break;
       }

 
        item = SaldoKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SaldoKey0_iter_last(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    saldo_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.layer, 
                 self->priv->layer))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.account_total, 
                 self->priv->account_total))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.subaccount_total, 
                 self->priv->subaccount_total))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 8 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 9 ) {
           if ( do_cmp(key_cmp.other, 
                 self->priv->other))
               break;
       }

 
        item = SaldoKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SaldoKey0_iter_le(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    saldo_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.layer, 
                 self->priv->layer))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.account_total, 
                 self->priv->account_total))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.subaccount_total, 
                 self->priv->subaccount_total))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 8 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 9 ) {
           if ( do_cmp(key_cmp.other, 
                 self->priv->other))
               break;
       }

 
        item = SaldoKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SaldoKey0_iter_ge(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    saldo_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.layer, 
                 self->priv->layer))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.account_total, 
                 self->priv->account_total))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.subaccount_total, 
                 self->priv->subaccount_total))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 8 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 9 ) {
           if ( do_cmp(key_cmp.other, 
                 self->priv->other))
               break;
       }

 
        item = SaldoKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SaldoKey0_iter_equal(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    saldo_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.layer, 
                 self->priv->layer))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.account_total, 
                 self->priv->account_total))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.subaccount_total, 
                 self->priv->subaccount_total))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 8 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 9 ) {
           if ( do_cmp(key_cmp.other, 
                 self->priv->other))
               break;
       }

 
        item = SaldoKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SaldoKey0_iter_lt(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    saldo_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.layer, 
                 self->priv->layer))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.account_total, 
                 self->priv->account_total))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.subaccount_total, 
                 self->priv->subaccount_total))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 8 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 9 ) {
           if ( do_cmp(key_cmp.other, 
                 self->priv->other))
               break;
       }

 
        item = SaldoKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SaldoKey0_iter_first(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    saldo_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.layer, 
                 self->priv->layer))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.account_total, 
                 self->priv->account_total))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.subaccount_total, 
                 self->priv->subaccount_total))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 8 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 9 ) {
           if ( do_cmp(key_cmp.other, 
                 self->priv->other))
               break;
       }

 
        item = SaldoKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_saldo_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SaldoKey0_cmp(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getSaldoKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->division, ((SaldoKey0*)obj)->priv->division);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->layer, ((SaldoKey0*)obj)->priv->layer);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->account, ((SaldoKey0*)obj)->priv->account);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->account_total - ((SaldoKey0*)obj)->priv->account_total;
       }

       if ( !res && depth >= 5 ) {
           res = do_cmp(self->priv->subaccount, ((SaldoKey0*)obj)->priv->subaccount);
       }

       if ( !res && depth >= 6 ) {
           res = self->priv->subaccount_total - ((SaldoKey0*)obj)->priv->subaccount_total;
       }

       if ( !res && depth >= 7 ) {
           res = do_cmp(self->priv->p_g_code, ((SaldoKey0*)obj)->priv->p_g_code);
       }

       if ( !res && depth >= 8 ) {
           res = self->priv->p_code - ((SaldoKey0*)obj)->priv->p_code;
       }

       if ( !res && depth >= 9 ) {
           res = do_cmp(self->priv->other, ((SaldoKey0*)obj)->priv->other);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *SaldoKey0_set_alias(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((SaldoKey0*)self)->alias);
    Py_INCREF(obj);
    ((SaldoKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *SaldoKey0_set(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getSaldoKey0Type() ) {
        memcpy(self->priv, ((SaldoKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getSaldoType() ) {

        do_cpy(self->priv->division, ((Saldo*)obj)->priv->data.division);

        do_cpy(self->priv->layer, ((Saldo*)obj)->priv->data.layer);

        do_cpy(self->priv->account, ((Saldo*)obj)->priv->data.account);

        do_cpy(self->priv->account_total, ((Saldo*)obj)->priv->data.account_total);

        do_cpy(self->priv->subaccount, ((Saldo*)obj)->priv->data.subaccount);

        do_cpy(self->priv->subaccount_total, ((Saldo*)obj)->priv->data.subaccount_total);

        do_cpy(self->priv->p_g_code, ((Saldo*)obj)->priv->data.p_g_code);

        do_cpy(self->priv->p_code, ((Saldo*)obj)->priv->data.p_code);

        do_cpy(self->priv->other, ((Saldo*)obj)->priv->data.other);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_layer", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_account", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_account_total", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount", "O", value) )
                        return NULL;
                    break;

                case 5:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount_total", "O", value) )
                        return NULL;
                    break;

                case 6:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_partner_region_code", "O", value) )
                        return NULL;
                    break;

                case 7:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_partner_code", "O", value) )
                        return NULL;
                    break;

                case 8:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_other", "O", value) )
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
            if ( !strcmp("layer", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_layer", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("account", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_account", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("account_total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_account_total", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("subaccount_total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount_total", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("other", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_other", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"SaldoKey0\"does not contain \"%s\"", field_name);
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

static PyObject *SaldoKey0_fields(SaldoKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class SaldoKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  SaldoKey0_division(self,NULL);
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

    value =  SaldoKey0_layer(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("layer", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  SaldoKey0_account(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("account", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  SaldoKey0_account_total(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("account_total", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  SaldoKey0_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  SaldoKey0_subaccount_total(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("subaccount_total", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  SaldoKey0_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  SaldoKey0_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  SaldoKey0_other(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("other", value);
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

static PyGetSetDef SaldoKey0_getset[] = {

    {"division",(getter)SaldoKey0_division},

    {"layer",(getter)SaldoKey0_layer},

    {"account",(getter)SaldoKey0_account},

    {"account_total",(getter)SaldoKey0_account_total},

    {"subaccount",(getter)SaldoKey0_subaccount},

    {"subaccount_total",(getter)SaldoKey0_subaccount_total},

    {"partner_region_code",(getter)SaldoKey0_partner_region_code},

    {"partner_code",(getter)SaldoKey0_partner_code},

    {"other",(getter)SaldoKey0_other},

    {NULL}
};

static PyMethodDef SaldoKey0_methods[] = {
    {"set", (PyCFunction)SaldoKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)SaldoKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)SaldoKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)SaldoKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)SaldoKey0_set_division, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_division"},

    {"set_layer", (PyCFunction)SaldoKey0_set_layer, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_layer"},

    {"set_account", (PyCFunction)SaldoKey0_set_account, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_account"},

    {"set_account_total", (PyCFunction)SaldoKey0_set_account_total, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_account_total"},

    {"set_subaccount", (PyCFunction)SaldoKey0_set_subaccount, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_subaccount"},

    {"set_subaccount_total", (PyCFunction)SaldoKey0_set_subaccount_total, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_subaccount_total"},

    {"set_partner_region_code", (PyCFunction)SaldoKey0_set_partner_region_code, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_partner_region_code"},

    {"set_partner_code", (PyCFunction)SaldoKey0_set_partner_code, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_partner_code"},

    {"set_other", (PyCFunction)SaldoKey0_set_other, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_set_other"},

    {"credit_sum", (PyCFunction)Saldo_get_credit_sum, METH_VARARGS|METH_KEYWORDS,"Saldo_get_credit_sum"},

    {"debit_sum", (PyCFunction)Saldo_get_debit_sum, METH_VARARGS|METH_KEYWORDS,"Saldo_get_debit_sum"},

    {"get_gt", (PyCFunction)SaldoKey0_gt, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_gt"},

    {"get_last", (PyCFunction)SaldoKey0_last, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_last"},

    {"get_next", (PyCFunction)SaldoKey0_next, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_next"},

    {"get_le", (PyCFunction)SaldoKey0_le, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_le"},

    {"get_lt", (PyCFunction)SaldoKey0_lt, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_lt"},

    {"get_equal", (PyCFunction)SaldoKey0_equal, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_equal"},

    {"get_ge", (PyCFunction)SaldoKey0_ge, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_ge"},

    {"get_prev", (PyCFunction)SaldoKey0_prev, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_prev"},

    {"get_first", (PyCFunction)SaldoKey0_first, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_first"},

    {"gets_gt", (PyCFunction)SaldoKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_iter_gt"},

    {"gets_last", (PyCFunction)SaldoKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_iter_last"},

    {"gets_le", (PyCFunction)SaldoKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_iter_le"},

    {"gets_ge", (PyCFunction)SaldoKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_iter_ge"},

    {"gets_equal", (PyCFunction)SaldoKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_iter_equal"},

    {"gets_lt", (PyCFunction)SaldoKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_iter_lt"},

    {"gets_first", (PyCFunction)SaldoKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "SaldoKey0_iter_first"},

    {NULL}
};

static PyTypeObject SaldoKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.SaldoKey0",             /*tp_name*/
    sizeof(SaldoKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SaldoKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)SaldoKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SaldoKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SaldoKey0_methods,             /* tp_methods */
    0,
    SaldoKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SaldoKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    SaldoKey0_new,                 /* tp_new */
};
PyTypeObject *getSaldoKey0Type()
{
    return &SaldoKey0Type_;
}
