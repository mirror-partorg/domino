
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "realization.h"

static void Realization_dealloc(PyObject* self)
{
    do_free(((Realization*)self)->priv);
    Py_DECREF(((Realization*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Realization free\n");
}
static PyObject *Realization_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Realization new\n");
    Realization *self;
    self = PyObject_NEW(Realization, type);
    //self = (Realization *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(realization_rec_t));
    return (PyObject *)self;
}
static int Realization_init(Realization *self, PyObject *args, PyObject *kwds)
{
    //LOG("Realization init\n");
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

    do_text_set_empty(self->priv->data.code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Realization_clone(Realization *self)
{
    Realization *result;
    result = (Realization*)PyObject_CallFunction((PyObject*)getRealizationType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Realization_store(Realization* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Realization_set_store(Realization* self, PyObject *args, PyObject *kwds)
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

static PyObject *Realization_code(Realization* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Realization_set_code(Realization* self, PyObject *args, PyObject *kwds)
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

static PyObject *Realization_month(Realization* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->data.month, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Realization_set_month(Realization* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.month");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.month");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->data.month = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->data.month, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Realization_total(Realization* self, void *unused)
{
    PyObject *result;

    result = PyFloat_FromDouble(self->priv->data.total);

    return result;
}

static PyObject *Realization_set_total(Realization* self, PyObject *args, PyObject *kwds)
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
    //result = PyFloat_FromDouble(self->priv->data.total);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Realization_sale_sum(Realization* self, void *unused)
{
    PyObject *result;

    result = PyFloat_FromDouble(self->priv->data.sale_sum/10000.);

    return result;
}

static PyObject *Realization_set_sale_sum(Realization* self, PyObject *args, PyObject *kwds)
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
             self->priv->data.sale_sum = MyLong_AsLong(value)*10000;
        else
        if ( PyFloat_Check(value) )
             self->priv->data.sale_sum = PyFloat_AsDouble(value)*10000;
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sale_sum");
            return NULL;
        }
    }
    //result = PyFloat_FromDouble(self->priv->data.sale_sum/10000.);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Realization_profit(Realization* self, void *unused)
{
    PyObject *result;

    result = PyFloat_FromDouble(self->priv->data.profit/10000.);

    return result;
}

static PyObject *Realization_set_profit(Realization* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.profit");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.profit = MyLong_AsLong(value)*10000;
        else
        if ( PyFloat_Check(value) )
             self->priv->data.profit = PyFloat_AsDouble(value)*10000;
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.profit");
            return NULL;
        }
    }
    //result = PyFloat_FromDouble(self->priv->data.profit/10000.);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Realization_gt(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_last(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_next(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_le(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_lt(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_equal(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_ge(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_prev(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_first(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRealizationKey0Type() )
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() )
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() )
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() )
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() )
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_iter_gt(Realization* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRealizationKey0Type() ) {
        key_cmp = (realization_key0_t*)do_malloc(sizeof(realization_key0_t));
        memcpy(key_cmp, ((RealizationKey0*)key)->priv, sizeof(realization_key0_t));
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() ) {
        key_cmp = (realization_key1_t*)do_malloc(sizeof(realization_key1_t));
        memcpy(key_cmp, ((RealizationKey1*)key)->priv, sizeof(realization_key1_t));
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() ) {
        key_cmp = (realization_key2_t*)do_malloc(sizeof(realization_key2_t));
        memcpy(key_cmp, ((RealizationKey2*)key)->priv, sizeof(realization_key2_t));
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() ) {
        key_cmp = (realization_key3_t*)do_malloc(sizeof(realization_key3_t));
        memcpy(key_cmp, ((RealizationKey3*)key)->priv, sizeof(realization_key3_t));
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() ) {
        key_cmp = (realization_key4_t*)do_malloc(sizeof(realization_key4_t));
        memcpy(key_cmp, ((RealizationKey4*)key)->priv, sizeof(realization_key4_t));
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->code, 
                    ((RealizationKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->month, 
                    ((RealizationKey0*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->sklad, 
                    ((RealizationKey0*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->sklad, 
                    ((RealizationKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->month, 
                    ((RealizationKey1*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->code, 
                    ((RealizationKey1*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->month, 
                    ((RealizationKey2*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->code, 
                    ((RealizationKey2*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->sklad, 
                    ((RealizationKey2*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->month, 
                    ((RealizationKey3*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->sklad, 
                    ((RealizationKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->code, 
                    ((RealizationKey3*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->code, 
                    ((RealizationKey4*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->sklad, 
                    ((RealizationKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->month, 
                    ((RealizationKey4*)key)->priv->month))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Realization_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
            status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
            status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
            status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
            status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
            status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Realization_iter_last(Realization* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRealizationKey0Type() ) {
        key_cmp = (realization_key0_t*)do_malloc(sizeof(realization_key0_t));
        memcpy(key_cmp, ((RealizationKey0*)key)->priv, sizeof(realization_key0_t));
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() ) {
        key_cmp = (realization_key1_t*)do_malloc(sizeof(realization_key1_t));
        memcpy(key_cmp, ((RealizationKey1*)key)->priv, sizeof(realization_key1_t));
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() ) {
        key_cmp = (realization_key2_t*)do_malloc(sizeof(realization_key2_t));
        memcpy(key_cmp, ((RealizationKey2*)key)->priv, sizeof(realization_key2_t));
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() ) {
        key_cmp = (realization_key3_t*)do_malloc(sizeof(realization_key3_t));
        memcpy(key_cmp, ((RealizationKey3*)key)->priv, sizeof(realization_key3_t));
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() ) {
        key_cmp = (realization_key4_t*)do_malloc(sizeof(realization_key4_t));
        memcpy(key_cmp, ((RealizationKey4*)key)->priv, sizeof(realization_key4_t));
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->code, 
                    ((RealizationKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->month, 
                    ((RealizationKey0*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->sklad, 
                    ((RealizationKey0*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->sklad, 
                    ((RealizationKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->month, 
                    ((RealizationKey1*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->code, 
                    ((RealizationKey1*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->month, 
                    ((RealizationKey2*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->code, 
                    ((RealizationKey2*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->sklad, 
                    ((RealizationKey2*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->month, 
                    ((RealizationKey3*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->sklad, 
                    ((RealizationKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->code, 
                    ((RealizationKey3*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->code, 
                    ((RealizationKey4*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->sklad, 
                    ((RealizationKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->month, 
                    ((RealizationKey4*)key)->priv->month))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Realization_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
            status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
            status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
            status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
            status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
            status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Realization_iter_le(Realization* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRealizationKey0Type() ) {
        key_cmp = (realization_key0_t*)do_malloc(sizeof(realization_key0_t));
        memcpy(key_cmp, ((RealizationKey0*)key)->priv, sizeof(realization_key0_t));
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() ) {
        key_cmp = (realization_key1_t*)do_malloc(sizeof(realization_key1_t));
        memcpy(key_cmp, ((RealizationKey1*)key)->priv, sizeof(realization_key1_t));
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() ) {
        key_cmp = (realization_key2_t*)do_malloc(sizeof(realization_key2_t));
        memcpy(key_cmp, ((RealizationKey2*)key)->priv, sizeof(realization_key2_t));
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() ) {
        key_cmp = (realization_key3_t*)do_malloc(sizeof(realization_key3_t));
        memcpy(key_cmp, ((RealizationKey3*)key)->priv, sizeof(realization_key3_t));
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() ) {
        key_cmp = (realization_key4_t*)do_malloc(sizeof(realization_key4_t));
        memcpy(key_cmp, ((RealizationKey4*)key)->priv, sizeof(realization_key4_t));
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->code, 
                    ((RealizationKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->month, 
                    ((RealizationKey0*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->sklad, 
                    ((RealizationKey0*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->sklad, 
                    ((RealizationKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->month, 
                    ((RealizationKey1*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->code, 
                    ((RealizationKey1*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->month, 
                    ((RealizationKey2*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->code, 
                    ((RealizationKey2*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->sklad, 
                    ((RealizationKey2*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->month, 
                    ((RealizationKey3*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->sklad, 
                    ((RealizationKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->code, 
                    ((RealizationKey3*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->code, 
                    ((RealizationKey4*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->sklad, 
                    ((RealizationKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->month, 
                    ((RealizationKey4*)key)->priv->month))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Realization_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
            status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
            status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
            status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
            status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
            status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Realization_iter_ge(Realization* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRealizationKey0Type() ) {
        key_cmp = (realization_key0_t*)do_malloc(sizeof(realization_key0_t));
        memcpy(key_cmp, ((RealizationKey0*)key)->priv, sizeof(realization_key0_t));
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() ) {
        key_cmp = (realization_key1_t*)do_malloc(sizeof(realization_key1_t));
        memcpy(key_cmp, ((RealizationKey1*)key)->priv, sizeof(realization_key1_t));
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() ) {
        key_cmp = (realization_key2_t*)do_malloc(sizeof(realization_key2_t));
        memcpy(key_cmp, ((RealizationKey2*)key)->priv, sizeof(realization_key2_t));
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() ) {
        key_cmp = (realization_key3_t*)do_malloc(sizeof(realization_key3_t));
        memcpy(key_cmp, ((RealizationKey3*)key)->priv, sizeof(realization_key3_t));
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() ) {
        key_cmp = (realization_key4_t*)do_malloc(sizeof(realization_key4_t));
        memcpy(key_cmp, ((RealizationKey4*)key)->priv, sizeof(realization_key4_t));
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->code, 
                    ((RealizationKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->month, 
                    ((RealizationKey0*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->sklad, 
                    ((RealizationKey0*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->sklad, 
                    ((RealizationKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->month, 
                    ((RealizationKey1*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->code, 
                    ((RealizationKey1*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->month, 
                    ((RealizationKey2*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->code, 
                    ((RealizationKey2*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->sklad, 
                    ((RealizationKey2*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->month, 
                    ((RealizationKey3*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->sklad, 
                    ((RealizationKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->code, 
                    ((RealizationKey3*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->code, 
                    ((RealizationKey4*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->sklad, 
                    ((RealizationKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->month, 
                    ((RealizationKey4*)key)->priv->month))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Realization_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
            status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
            status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
            status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
            status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
            status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Realization_iter_equal(Realization* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRealizationKey0Type() ) {
        key_cmp = (realization_key0_t*)do_malloc(sizeof(realization_key0_t));
        memcpy(key_cmp, ((RealizationKey0*)key)->priv, sizeof(realization_key0_t));
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() ) {
        key_cmp = (realization_key1_t*)do_malloc(sizeof(realization_key1_t));
        memcpy(key_cmp, ((RealizationKey1*)key)->priv, sizeof(realization_key1_t));
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() ) {
        key_cmp = (realization_key2_t*)do_malloc(sizeof(realization_key2_t));
        memcpy(key_cmp, ((RealizationKey2*)key)->priv, sizeof(realization_key2_t));
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() ) {
        key_cmp = (realization_key3_t*)do_malloc(sizeof(realization_key3_t));
        memcpy(key_cmp, ((RealizationKey3*)key)->priv, sizeof(realization_key3_t));
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() ) {
        key_cmp = (realization_key4_t*)do_malloc(sizeof(realization_key4_t));
        memcpy(key_cmp, ((RealizationKey4*)key)->priv, sizeof(realization_key4_t));
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->code, 
                    ((RealizationKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->month, 
                    ((RealizationKey0*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->sklad, 
                    ((RealizationKey0*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->sklad, 
                    ((RealizationKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->month, 
                    ((RealizationKey1*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->code, 
                    ((RealizationKey1*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->month, 
                    ((RealizationKey2*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->code, 
                    ((RealizationKey2*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->sklad, 
                    ((RealizationKey2*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->month, 
                    ((RealizationKey3*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->sklad, 
                    ((RealizationKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->code, 
                    ((RealizationKey3*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->code, 
                    ((RealizationKey4*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->sklad, 
                    ((RealizationKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->month, 
                    ((RealizationKey4*)key)->priv->month))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Realization_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
            status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
            status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
            status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
            status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
            status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Realization_iter_lt(Realization* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRealizationKey0Type() ) {
        key_cmp = (realization_key0_t*)do_malloc(sizeof(realization_key0_t));
        memcpy(key_cmp, ((RealizationKey0*)key)->priv, sizeof(realization_key0_t));
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() ) {
        key_cmp = (realization_key1_t*)do_malloc(sizeof(realization_key1_t));
        memcpy(key_cmp, ((RealizationKey1*)key)->priv, sizeof(realization_key1_t));
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() ) {
        key_cmp = (realization_key2_t*)do_malloc(sizeof(realization_key2_t));
        memcpy(key_cmp, ((RealizationKey2*)key)->priv, sizeof(realization_key2_t));
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() ) {
        key_cmp = (realization_key3_t*)do_malloc(sizeof(realization_key3_t));
        memcpy(key_cmp, ((RealizationKey3*)key)->priv, sizeof(realization_key3_t));
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() ) {
        key_cmp = (realization_key4_t*)do_malloc(sizeof(realization_key4_t));
        memcpy(key_cmp, ((RealizationKey4*)key)->priv, sizeof(realization_key4_t));
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->code, 
                    ((RealizationKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->month, 
                    ((RealizationKey0*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->sklad, 
                    ((RealizationKey0*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->sklad, 
                    ((RealizationKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->month, 
                    ((RealizationKey1*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->code, 
                    ((RealizationKey1*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->month, 
                    ((RealizationKey2*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->code, 
                    ((RealizationKey2*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->sklad, 
                    ((RealizationKey2*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->month, 
                    ((RealizationKey3*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->sklad, 
                    ((RealizationKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->code, 
                    ((RealizationKey3*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->code, 
                    ((RealizationKey4*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->sklad, 
                    ((RealizationKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->month, 
                    ((RealizationKey4*)key)->priv->month))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Realization_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
            status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
            status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
            status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
            status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
            status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Realization_iter_first(Realization* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRealizationKey0Type() ) {
        key_cmp = (realization_key0_t*)do_malloc(sizeof(realization_key0_t));
        memcpy(key_cmp, ((RealizationKey0*)key)->priv, sizeof(realization_key0_t));
        status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey1Type() ) {
        key_cmp = (realization_key1_t*)do_malloc(sizeof(realization_key1_t));
        memcpy(key_cmp, ((RealizationKey1*)key)->priv, sizeof(realization_key1_t));
        status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey2Type() ) {
        key_cmp = (realization_key2_t*)do_malloc(sizeof(realization_key2_t));
        memcpy(key_cmp, ((RealizationKey2*)key)->priv, sizeof(realization_key2_t));
        status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey3Type() ) {
        key_cmp = (realization_key3_t*)do_malloc(sizeof(realization_key3_t));
        memcpy(key_cmp, ((RealizationKey3*)key)->priv, sizeof(realization_key3_t));
        status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getRealizationKey4Type() ) {
        key_cmp = (realization_key4_t*)do_malloc(sizeof(realization_key4_t));
        memcpy(key_cmp, ((RealizationKey4*)key)->priv, sizeof(realization_key4_t));
        status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->code, 
                    ((RealizationKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->month, 
                    ((RealizationKey0*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key0_t*)key_cmp)->sklad, 
                    ((RealizationKey0*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->sklad, 
                    ((RealizationKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->month, 
                    ((RealizationKey1*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key1_t*)key_cmp)->code, 
                    ((RealizationKey1*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->month, 
                    ((RealizationKey2*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->code, 
                    ((RealizationKey2*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key2_t*)key_cmp)->sklad, 
                    ((RealizationKey2*)key)->priv->sklad))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->month, 
                    ((RealizationKey3*)key)->priv->month))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->sklad, 
                    ((RealizationKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key3_t*)key_cmp)->code, 
                    ((RealizationKey3*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->code, 
                    ((RealizationKey4*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->sklad, 
                    ((RealizationKey4*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((realization_key4_t*)key_cmp)->month, 
                    ((RealizationKey4*)key)->priv->month))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Realization_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRealizationKey0Type() ) {
            status = do_realization_get0(self->alias->alias, self->priv, ((RealizationKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey1Type() ) {
            status = do_realization_get1(self->alias->alias, self->priv, ((RealizationKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey2Type() ) {
            status = do_realization_get2(self->alias->alias, self->priv, ((RealizationKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey3Type() ) {
            status = do_realization_get3(self->alias->alias, self->priv, ((RealizationKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRealizationKey4Type() ) {
            status = do_realization_get4(self->alias->alias, self->priv, ((RealizationKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Realization_insert(Realization* self)
{
    int status;
    status = do_realization_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_update(Realization* self)
{
    int status;
    status = do_realization_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_delete(Realization* self)
{
    int status;
    status = do_realization_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Realization_set_alias(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Realization*)self)->alias);
    Py_INCREF(obj);
    ((Realization*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Realization_set(Realization* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRealizationType() ) {
        memcpy(self->priv, ((Realization*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRealizationKey0Type() ) {

        do_cpy(self->priv->data.sklad, ((RealizationKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.code, ((RealizationKey0*)obj)->priv->code);

        do_cpy(self->priv->data.month, ((RealizationKey0*)obj)->priv->month);

    }

    else 
    if ( Py_TYPE(obj) == getRealizationKey1Type() ) {

        do_cpy(self->priv->data.sklad, ((RealizationKey1*)obj)->priv->sklad);

        do_cpy(self->priv->data.code, ((RealizationKey1*)obj)->priv->code);

        do_cpy(self->priv->data.month, ((RealizationKey1*)obj)->priv->month);

    }

    else 
    if ( Py_TYPE(obj) == getRealizationKey2Type() ) {

        do_cpy(self->priv->data.sklad, ((RealizationKey2*)obj)->priv->sklad);

        do_cpy(self->priv->data.code, ((RealizationKey2*)obj)->priv->code);

        do_cpy(self->priv->data.month, ((RealizationKey2*)obj)->priv->month);

    }

    else 
    if ( Py_TYPE(obj) == getRealizationKey3Type() ) {

        do_cpy(self->priv->data.sklad, ((RealizationKey3*)obj)->priv->sklad);

        do_cpy(self->priv->data.code, ((RealizationKey3*)obj)->priv->code);

        do_cpy(self->priv->data.month, ((RealizationKey3*)obj)->priv->month);

    }

    else 
    if ( Py_TYPE(obj) == getRealizationKey4Type() ) {

        do_cpy(self->priv->data.sklad, ((RealizationKey4*)obj)->priv->sklad);

        do_cpy(self->priv->data.code, ((RealizationKey4*)obj)->priv->code);

        do_cpy(self->priv->data.month, ((RealizationKey4*)obj)->priv->month);

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
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("month", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_total", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sale_sum", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sale_sum", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("profit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_profit", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Realization\"does not contain \"%s\"", field_name);
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

static PyObject *Realization_fields(Realization* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Realization");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Realization_store(self,NULL);
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

    value =  Realization_code(self,NULL);
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

    value =  Realization_month(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("month", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Realization_total(self,NULL);
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

    value =  Realization_sale_sum(self,NULL);
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

    value =  Realization_profit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("profit", value);
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

static PyGetSetDef Realization_getset[] = {

    {"store",(getter)Realization_store},

    {"code",(getter)Realization_code},

    {"month",(getter)Realization_month},

    {"total",(getter)Realization_total},

    {"sale_sum",(getter)Realization_sale_sum},

    {"profit",(getter)Realization_profit},

    {NULL}
};

static PyMethodDef Realization_methods[] = {
    {"set", (PyCFunction)Realization_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Realization_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Realization_clone, METH_NOARGS, "clone"},

    {"set_store", (PyCFunction)Realization_set_store, METH_VARARGS|METH_KEYWORDS, "Realization_set_store"},

    {"set_code", (PyCFunction)Realization_set_code, METH_VARARGS|METH_KEYWORDS, "Realization_set_code"},

    {"set_month", (PyCFunction)Realization_set_month, METH_VARARGS|METH_KEYWORDS, "Realization_set_month"},

    {"set_total", (PyCFunction)Realization_set_total, METH_VARARGS|METH_KEYWORDS, "Realization_set_total"},

    {"set_sale_sum", (PyCFunction)Realization_set_sale_sum, METH_VARARGS|METH_KEYWORDS, "Realization_set_sale_sum"},

    {"set_profit", (PyCFunction)Realization_set_profit, METH_VARARGS|METH_KEYWORDS, "Realization_set_profit"},

    {"get_gt", (PyCFunction)Realization_gt, METH_VARARGS|METH_KEYWORDS, "Realization_gt"},

    {"get_last", (PyCFunction)Realization_last, METH_VARARGS|METH_KEYWORDS, "Realization_last"},

    {"get_next", (PyCFunction)Realization_next, METH_VARARGS|METH_KEYWORDS, "Realization_next"},

    {"get_le", (PyCFunction)Realization_le, METH_VARARGS|METH_KEYWORDS, "Realization_le"},

    {"get_lt", (PyCFunction)Realization_lt, METH_VARARGS|METH_KEYWORDS, "Realization_lt"},

    {"get_equal", (PyCFunction)Realization_equal, METH_VARARGS|METH_KEYWORDS, "Realization_equal"},

    {"get_ge", (PyCFunction)Realization_ge, METH_VARARGS|METH_KEYWORDS, "Realization_ge"},

    {"get_prev", (PyCFunction)Realization_prev, METH_VARARGS|METH_KEYWORDS, "Realization_prev"},

    {"get_first", (PyCFunction)Realization_first, METH_VARARGS|METH_KEYWORDS, "Realization_first"},

    {"gets_gt", (PyCFunction)Realization_iter_gt, METH_VARARGS|METH_KEYWORDS, "Realization_iter_gt"},

    {"gets_last", (PyCFunction)Realization_iter_last, METH_VARARGS|METH_KEYWORDS, "Realization_iter_last"},

    {"gets_le", (PyCFunction)Realization_iter_le, METH_VARARGS|METH_KEYWORDS, "Realization_iter_le"},

    {"gets_ge", (PyCFunction)Realization_iter_ge, METH_VARARGS|METH_KEYWORDS, "Realization_iter_ge"},

    {"gets_equal", (PyCFunction)Realization_iter_equal, METH_VARARGS|METH_KEYWORDS, "Realization_iter_equal"},

    {"gets_lt", (PyCFunction)Realization_iter_lt, METH_VARARGS|METH_KEYWORDS, "Realization_iter_lt"},

    {"gets_first", (PyCFunction)Realization_iter_first, METH_VARARGS|METH_KEYWORDS, "Realization_iter_first"},

    {"insert", (PyCFunction)Realization_insert, METH_VARARGS|METH_KEYWORDS, "Realization_insert"},

    {"update", (PyCFunction)Realization_update, METH_VARARGS|METH_KEYWORDS, "Realization_update"},

    {"delete", (PyCFunction)Realization_delete, METH_VARARGS|METH_KEYWORDS, "Realization_delete"},

    {NULL}
};

static PyTypeObject RealizationType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Realization",             /*tp_name*/
    sizeof(Realization),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Realization_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Realization_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Realization objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Realization_methods,             /* tp_methods */
    0,
    Realization_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Realization_init,      /* tp_init */
    0,                         /* tp_alloc */
    Realization_new,                 /* tp_new */
};
PyTypeObject *getRealizationType()
{
    return &RealizationType_;
}

static void RealizationKey0_dealloc(PyObject* self)
{
    do_free(((RealizationKey0*)self)->priv);
    Py_DECREF(((RealizationKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("RealizationKey0 free\n");
}
static PyObject *RealizationKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey0 new\n");
    RealizationKey0 *self;
    self = PyObject_NEW(RealizationKey0, type);
    //self = (RealizationKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(realization_key0_t));
    return (PyObject *)self;
}
static int RealizationKey0_init(RealizationKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("RealizationKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->code);

    do_text_set_empty(self->priv->sklad);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *RealizationKey0_clone(RealizationKey0 *self)
{
    RealizationKey0 *result;
    result = (RealizationKey0*)PyObject_CallFunction((PyObject*)getRealizationKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *RealizationKey0_code(RealizationKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey0_set_code(RealizationKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey0_month(RealizationKey0* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *RealizationKey0_set_month(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->month = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *RealizationKey0_store(RealizationKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey0_set_store(RealizationKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey0_gt(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_last(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_next(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_le(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_lt(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_equal(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_ge(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_prev(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_first(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey0_iter_gt(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey0_iter_last(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey0_iter_le(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey0_iter_ge(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey0_iter_equal(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey0_iter_lt(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey0_iter_first(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey0_cmp(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getRealizationKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((RealizationKey0*)obj)->priv->code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->month - ((RealizationKey0*)obj)->priv->month;
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->sklad, ((RealizationKey0*)obj)->priv->sklad);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *RealizationKey0_set_alias(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((RealizationKey0*)self)->alias);
    Py_INCREF(obj);
    ((RealizationKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *RealizationKey0_set(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRealizationKey0Type() ) {
        memcpy(self->priv, ((RealizationKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRealizationType() ) {

        do_cpy(self->priv->code, ((Realization*)obj)->priv->data.code);

        do_cpy(self->priv->month, ((Realization*)obj)->priv->data.month);

        do_cpy(self->priv->sklad, ((Realization*)obj)->priv->data.sklad);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
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
            if ( !strcmp("month", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"RealizationKey0\"does not contain \"%s\"", field_name);
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

static PyObject *RealizationKey0_fields(RealizationKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class RealizationKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  RealizationKey0_code(self,NULL);
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

    value =  RealizationKey0_month(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("month", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  RealizationKey0_store(self,NULL);
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

static PyGetSetDef RealizationKey0_getset[] = {

    {"code",(getter)RealizationKey0_code},

    {"month",(getter)RealizationKey0_month},

    {"store",(getter)RealizationKey0_store},

    {NULL}
};

static PyMethodDef RealizationKey0_methods[] = {
    {"set", (PyCFunction)RealizationKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)RealizationKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)RealizationKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)RealizationKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)RealizationKey0_set_code, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_set_code"},

    {"set_month", (PyCFunction)RealizationKey0_set_month, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_set_month"},

    {"set_store", (PyCFunction)RealizationKey0_set_store, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_set_store"},

    {"get_gt", (PyCFunction)RealizationKey0_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_gt"},

    {"get_last", (PyCFunction)RealizationKey0_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_last"},

    {"get_next", (PyCFunction)RealizationKey0_next, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_next"},

    {"get_le", (PyCFunction)RealizationKey0_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_le"},

    {"get_lt", (PyCFunction)RealizationKey0_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_lt"},

    {"get_equal", (PyCFunction)RealizationKey0_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_equal"},

    {"get_ge", (PyCFunction)RealizationKey0_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_ge"},

    {"get_prev", (PyCFunction)RealizationKey0_prev, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_prev"},

    {"get_first", (PyCFunction)RealizationKey0_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_first"},

    {"gets_gt", (PyCFunction)RealizationKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_iter_gt"},

    {"gets_last", (PyCFunction)RealizationKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_iter_last"},

    {"gets_le", (PyCFunction)RealizationKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_iter_le"},

    {"gets_ge", (PyCFunction)RealizationKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_iter_ge"},

    {"gets_equal", (PyCFunction)RealizationKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_iter_equal"},

    {"gets_lt", (PyCFunction)RealizationKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_iter_lt"},

    {"gets_first", (PyCFunction)RealizationKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey0_iter_first"},

    {NULL}
};

static PyTypeObject RealizationKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.RealizationKey0",             /*tp_name*/
    sizeof(RealizationKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)RealizationKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey0_methods,             /* tp_methods */
    0,
    RealizationKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey0_new,                 /* tp_new */
};
PyTypeObject *getRealizationKey0Type()
{
    return &RealizationKey0Type_;
}

static void RealizationKey1_dealloc(PyObject* self)
{
    do_free(((RealizationKey1*)self)->priv);
    Py_DECREF(((RealizationKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("RealizationKey1 free\n");
}
static PyObject *RealizationKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey1 new\n");
    RealizationKey1 *self;
    self = PyObject_NEW(RealizationKey1, type);
    //self = (RealizationKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(realization_key1_t));
    return (PyObject *)self;
}
static int RealizationKey1_init(RealizationKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("RealizationKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *RealizationKey1_clone(RealizationKey1 *self)
{
    RealizationKey1 *result;
    result = (RealizationKey1*)PyObject_CallFunction((PyObject*)getRealizationKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *RealizationKey1_store(RealizationKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey1_set_store(RealizationKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey1_month(RealizationKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *RealizationKey1_set_month(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->month = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *RealizationKey1_code(RealizationKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey1_set_code(RealizationKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey1_gt(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_last(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_next(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_le(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_lt(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_equal(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_ge(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_prev(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_first(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey1_iter_gt(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey1_iter_last(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey1_iter_le(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey1_iter_ge(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey1_iter_equal(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey1_iter_lt(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey1_iter_first(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey1_cmp(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getRealizationKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->sklad, ((RealizationKey1*)obj)->priv->sklad);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->month - ((RealizationKey1*)obj)->priv->month;
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->code, ((RealizationKey1*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *RealizationKey1_set_alias(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((RealizationKey1*)self)->alias);
    Py_INCREF(obj);
    ((RealizationKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *RealizationKey1_set(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRealizationKey1Type() ) {
        memcpy(self->priv, ((RealizationKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRealizationType() ) {

        do_cpy(self->priv->sklad, ((Realization*)obj)->priv->data.sklad);

        do_cpy(self->priv->month, ((Realization*)obj)->priv->data.month);

        do_cpy(self->priv->code, ((Realization*)obj)->priv->data.code);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                        return NULL;
                    break;

                case 2:
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
            if ( !strcmp("month", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"RealizationKey1\"does not contain \"%s\"", field_name);
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

static PyObject *RealizationKey1_fields(RealizationKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class RealizationKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  RealizationKey1_store(self,NULL);
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

    value =  RealizationKey1_month(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("month", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  RealizationKey1_code(self,NULL);
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

static PyGetSetDef RealizationKey1_getset[] = {

    {"store",(getter)RealizationKey1_store},

    {"month",(getter)RealizationKey1_month},

    {"code",(getter)RealizationKey1_code},

    {NULL}
};

static PyMethodDef RealizationKey1_methods[] = {
    {"set", (PyCFunction)RealizationKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)RealizationKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)RealizationKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)RealizationKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_store", (PyCFunction)RealizationKey1_set_store, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_set_store"},

    {"set_month", (PyCFunction)RealizationKey1_set_month, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_set_month"},

    {"set_code", (PyCFunction)RealizationKey1_set_code, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_set_code"},

    {"get_gt", (PyCFunction)RealizationKey1_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_gt"},

    {"get_last", (PyCFunction)RealizationKey1_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_last"},

    {"get_next", (PyCFunction)RealizationKey1_next, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_next"},

    {"get_le", (PyCFunction)RealizationKey1_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_le"},

    {"get_lt", (PyCFunction)RealizationKey1_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_lt"},

    {"get_equal", (PyCFunction)RealizationKey1_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_equal"},

    {"get_ge", (PyCFunction)RealizationKey1_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_ge"},

    {"get_prev", (PyCFunction)RealizationKey1_prev, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_prev"},

    {"get_first", (PyCFunction)RealizationKey1_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_first"},

    {"gets_gt", (PyCFunction)RealizationKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_iter_gt"},

    {"gets_last", (PyCFunction)RealizationKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_iter_last"},

    {"gets_le", (PyCFunction)RealizationKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_iter_le"},

    {"gets_ge", (PyCFunction)RealizationKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_iter_ge"},

    {"gets_equal", (PyCFunction)RealizationKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_iter_equal"},

    {"gets_lt", (PyCFunction)RealizationKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_iter_lt"},

    {"gets_first", (PyCFunction)RealizationKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey1_iter_first"},

    {NULL}
};

static PyTypeObject RealizationKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.RealizationKey1",             /*tp_name*/
    sizeof(RealizationKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)RealizationKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey1_methods,             /* tp_methods */
    0,
    RealizationKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey1_new,                 /* tp_new */
};
PyTypeObject *getRealizationKey1Type()
{
    return &RealizationKey1Type_;
}

static void RealizationKey2_dealloc(PyObject* self)
{
    do_free(((RealizationKey2*)self)->priv);
    Py_DECREF(((RealizationKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("RealizationKey2 free\n");
}
static PyObject *RealizationKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey2 new\n");
    RealizationKey2 *self;
    self = PyObject_NEW(RealizationKey2, type);
    //self = (RealizationKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(realization_key2_t));
    return (PyObject *)self;
}
static int RealizationKey2_init(RealizationKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("RealizationKey2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->code);

    do_text_set_empty(self->priv->sklad);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *RealizationKey2_clone(RealizationKey2 *self)
{
    RealizationKey2 *result;
    result = (RealizationKey2*)PyObject_CallFunction((PyObject*)getRealizationKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *RealizationKey2_month(RealizationKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *RealizationKey2_set_month(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->month = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *RealizationKey2_code(RealizationKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey2_set_code(RealizationKey2* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey2_store(RealizationKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey2_set_store(RealizationKey2* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey2_gt(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_last(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_next(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_le(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_lt(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_equal(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_ge(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_prev(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_first(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey2_iter_gt(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey2_iter_last(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey2_iter_le(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey2_iter_ge(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey2_iter_equal(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey2_iter_lt(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey2_iter_first(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

 
        item = RealizationKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey2_cmp(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getRealizationKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->month - ((RealizationKey2*)obj)->priv->month;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->code, ((RealizationKey2*)obj)->priv->code);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->sklad, ((RealizationKey2*)obj)->priv->sklad);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *RealizationKey2_set_alias(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((RealizationKey2*)self)->alias);
    Py_INCREF(obj);
    ((RealizationKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *RealizationKey2_set(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRealizationKey2Type() ) {
        memcpy(self->priv, ((RealizationKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRealizationType() ) {

        do_cpy(self->priv->month, ((Realization*)obj)->priv->data.month);

        do_cpy(self->priv->code, ((Realization*)obj)->priv->data.code);

        do_cpy(self->priv->sklad, ((Realization*)obj)->priv->data.sklad);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
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
            if ( !strcmp("month", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"RealizationKey2\"does not contain \"%s\"", field_name);
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

static PyObject *RealizationKey2_fields(RealizationKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class RealizationKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  RealizationKey2_month(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("month", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  RealizationKey2_code(self,NULL);
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

    value =  RealizationKey2_store(self,NULL);
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

static PyGetSetDef RealizationKey2_getset[] = {

    {"month",(getter)RealizationKey2_month},

    {"code",(getter)RealizationKey2_code},

    {"store",(getter)RealizationKey2_store},

    {NULL}
};

static PyMethodDef RealizationKey2_methods[] = {
    {"set", (PyCFunction)RealizationKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)RealizationKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)RealizationKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)RealizationKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_month", (PyCFunction)RealizationKey2_set_month, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_set_month"},

    {"set_code", (PyCFunction)RealizationKey2_set_code, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_set_code"},

    {"set_store", (PyCFunction)RealizationKey2_set_store, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_set_store"},

    {"get_gt", (PyCFunction)RealizationKey2_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_gt"},

    {"get_last", (PyCFunction)RealizationKey2_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_last"},

    {"get_next", (PyCFunction)RealizationKey2_next, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_next"},

    {"get_le", (PyCFunction)RealizationKey2_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_le"},

    {"get_lt", (PyCFunction)RealizationKey2_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_lt"},

    {"get_equal", (PyCFunction)RealizationKey2_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_equal"},

    {"get_ge", (PyCFunction)RealizationKey2_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_ge"},

    {"get_prev", (PyCFunction)RealizationKey2_prev, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_prev"},

    {"get_first", (PyCFunction)RealizationKey2_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_first"},

    {"gets_gt", (PyCFunction)RealizationKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_iter_gt"},

    {"gets_last", (PyCFunction)RealizationKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_iter_last"},

    {"gets_le", (PyCFunction)RealizationKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_iter_le"},

    {"gets_ge", (PyCFunction)RealizationKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_iter_ge"},

    {"gets_equal", (PyCFunction)RealizationKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_iter_equal"},

    {"gets_lt", (PyCFunction)RealizationKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_iter_lt"},

    {"gets_first", (PyCFunction)RealizationKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey2_iter_first"},

    {NULL}
};

static PyTypeObject RealizationKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.RealizationKey2",             /*tp_name*/
    sizeof(RealizationKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)RealizationKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey2_methods,             /* tp_methods */
    0,
    RealizationKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey2_new,                 /* tp_new */
};
PyTypeObject *getRealizationKey2Type()
{
    return &RealizationKey2Type_;
}

static void RealizationKey3_dealloc(PyObject* self)
{
    do_free(((RealizationKey3*)self)->priv);
    Py_DECREF(((RealizationKey3*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("RealizationKey3 free\n");
}
static PyObject *RealizationKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey3 new\n");
    RealizationKey3 *self;
    self = PyObject_NEW(RealizationKey3, type);
    //self = (RealizationKey3 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(realization_key3_t));
    return (PyObject *)self;
}
static int RealizationKey3_init(RealizationKey3 *self, PyObject *args, PyObject *kwds)
{
    //LOG("RealizationKey3 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *RealizationKey3_clone(RealizationKey3 *self)
{
    RealizationKey3 *result;
    result = (RealizationKey3*)PyObject_CallFunction((PyObject*)getRealizationKey3Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *RealizationKey3_month(RealizationKey3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *RealizationKey3_set_month(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->month = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *RealizationKey3_store(RealizationKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey3_set_store(RealizationKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey3_code(RealizationKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey3_set_code(RealizationKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey3_gt(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_last(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_next(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_le(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_lt(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_equal(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_ge(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_prev(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_first(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey3_iter_gt(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey3_iter_last(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey3_iter_le(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey3_iter_ge(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey3_iter_equal(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey3_iter_lt(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey3_iter_first(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key3(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RealizationKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey3_cmp(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getRealizationKey3Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->month - ((RealizationKey3*)obj)->priv->month;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((RealizationKey3*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->code, ((RealizationKey3*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *RealizationKey3_set_alias(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((RealizationKey3*)self)->alias);
    Py_INCREF(obj);
    ((RealizationKey3*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *RealizationKey3_set(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRealizationKey3Type() ) {
        memcpy(self->priv, ((RealizationKey3*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRealizationType() ) {

        do_cpy(self->priv->month, ((Realization*)obj)->priv->data.month);

        do_cpy(self->priv->sklad, ((Realization*)obj)->priv->data.sklad);

        do_cpy(self->priv->code, ((Realization*)obj)->priv->data.code);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 2:
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
            if ( !strcmp("month", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"RealizationKey3\"does not contain \"%s\"", field_name);
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

static PyObject *RealizationKey3_fields(RealizationKey3* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class RealizationKey3");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  RealizationKey3_month(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("month", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  RealizationKey3_store(self,NULL);
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

    value =  RealizationKey3_code(self,NULL);
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

static PyGetSetDef RealizationKey3_getset[] = {

    {"month",(getter)RealizationKey3_month},

    {"store",(getter)RealizationKey3_store},

    {"code",(getter)RealizationKey3_code},

    {NULL}
};

static PyMethodDef RealizationKey3_methods[] = {
    {"set", (PyCFunction)RealizationKey3_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)RealizationKey3_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)RealizationKey3_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)RealizationKey3_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_month", (PyCFunction)RealizationKey3_set_month, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_set_month"},

    {"set_store", (PyCFunction)RealizationKey3_set_store, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_set_store"},

    {"set_code", (PyCFunction)RealizationKey3_set_code, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_set_code"},

    {"get_gt", (PyCFunction)RealizationKey3_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_gt"},

    {"get_last", (PyCFunction)RealizationKey3_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_last"},

    {"get_next", (PyCFunction)RealizationKey3_next, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_next"},

    {"get_le", (PyCFunction)RealizationKey3_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_le"},

    {"get_lt", (PyCFunction)RealizationKey3_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_lt"},

    {"get_equal", (PyCFunction)RealizationKey3_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_equal"},

    {"get_ge", (PyCFunction)RealizationKey3_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_ge"},

    {"get_prev", (PyCFunction)RealizationKey3_prev, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_prev"},

    {"get_first", (PyCFunction)RealizationKey3_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_first"},

    {"gets_gt", (PyCFunction)RealizationKey3_iter_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_iter_gt"},

    {"gets_last", (PyCFunction)RealizationKey3_iter_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_iter_last"},

    {"gets_le", (PyCFunction)RealizationKey3_iter_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_iter_le"},

    {"gets_ge", (PyCFunction)RealizationKey3_iter_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_iter_ge"},

    {"gets_equal", (PyCFunction)RealizationKey3_iter_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_iter_equal"},

    {"gets_lt", (PyCFunction)RealizationKey3_iter_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_iter_lt"},

    {"gets_first", (PyCFunction)RealizationKey3_iter_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey3_iter_first"},

    {NULL}
};

static PyTypeObject RealizationKey3Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.RealizationKey3",             /*tp_name*/
    sizeof(RealizationKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)RealizationKey3_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey3_methods,             /* tp_methods */
    0,
    RealizationKey3_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey3_new,                 /* tp_new */
};
PyTypeObject *getRealizationKey3Type()
{
    return &RealizationKey3Type_;
}

static void RealizationKey4_dealloc(PyObject* self)
{
    do_free(((RealizationKey4*)self)->priv);
    Py_DECREF(((RealizationKey4*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("RealizationKey4 free\n");
}
static PyObject *RealizationKey4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("RealizationKey4 new\n");
    RealizationKey4 *self;
    self = PyObject_NEW(RealizationKey4, type);
    //self = (RealizationKey4 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(realization_key4_t));
    return (PyObject *)self;
}
static int RealizationKey4_init(RealizationKey4 *self, PyObject *args, PyObject *kwds)
{
    //LOG("RealizationKey4 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->code);

    do_text_set_empty(self->priv->sklad);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *RealizationKey4_clone(RealizationKey4 *self)
{
    RealizationKey4 *result;
    result = (RealizationKey4*)PyObject_CallFunction((PyObject*)getRealizationKey4Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *RealizationKey4_code(RealizationKey4* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey4_set_code(RealizationKey4* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey4_store(RealizationKey4* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RealizationKey4_set_store(RealizationKey4* self, PyObject *args, PyObject *kwds)
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

static PyObject *RealizationKey4_month(RealizationKey4* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *RealizationKey4_set_month(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
        return NULL;
    }
    struct tm tm;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set month");
            return NULL;
        }
        do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
        self->priv->month = do_datetosql(tm);
    }
    /*do_sqltodate(self->priv->month, &tm);
    result = PyDate_FromDate(tm.tm_your+1900r,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *RealizationKey4_gt(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_last(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_next(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_le(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_lt(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_equal(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_ge(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_prev(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_first(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RealizationKey4_iter_gt(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

 
        item = RealizationKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey4_iter_last(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

 
        item = RealizationKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey4_iter_le(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

 
        item = RealizationKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey4_iter_ge(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

 
        item = RealizationKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey4_iter_equal(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

 
        item = RealizationKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey4_iter_lt(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

 
        item = RealizationKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey4_iter_first(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    realization_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_realization_key4(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.month, 
                 self->priv->month))
               break;
       }

 
        item = RealizationKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_realization_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RealizationKey4_cmp(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getRealizationKey4Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((RealizationKey4*)obj)->priv->code);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((RealizationKey4*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->month - ((RealizationKey4*)obj)->priv->month;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *RealizationKey4_set_alias(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((RealizationKey4*)self)->alias);
    Py_INCREF(obj);
    ((RealizationKey4*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *RealizationKey4_set(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRealizationKey4Type() ) {
        memcpy(self->priv, ((RealizationKey4*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRealizationType() ) {

        do_cpy(self->priv->code, ((Realization*)obj)->priv->data.code);

        do_cpy(self->priv->sklad, ((Realization*)obj)->priv->data.sklad);

        do_cpy(self->priv->month, ((Realization*)obj)->priv->data.month);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
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
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("month", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_month", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"RealizationKey4\"does not contain \"%s\"", field_name);
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

static PyObject *RealizationKey4_fields(RealizationKey4* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class RealizationKey4");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  RealizationKey4_code(self,NULL);
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

    value =  RealizationKey4_store(self,NULL);
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

    value =  RealizationKey4_month(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("month", value);
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

static PyGetSetDef RealizationKey4_getset[] = {

    {"code",(getter)RealizationKey4_code},

    {"store",(getter)RealizationKey4_store},

    {"month",(getter)RealizationKey4_month},

    {NULL}
};

static PyMethodDef RealizationKey4_methods[] = {
    {"set", (PyCFunction)RealizationKey4_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)RealizationKey4_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)RealizationKey4_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)RealizationKey4_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)RealizationKey4_set_code, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_set_code"},

    {"set_store", (PyCFunction)RealizationKey4_set_store, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_set_store"},

    {"set_month", (PyCFunction)RealizationKey4_set_month, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_set_month"},

    {"get_gt", (PyCFunction)RealizationKey4_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_gt"},

    {"get_last", (PyCFunction)RealizationKey4_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_last"},

    {"get_next", (PyCFunction)RealizationKey4_next, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_next"},

    {"get_le", (PyCFunction)RealizationKey4_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_le"},

    {"get_lt", (PyCFunction)RealizationKey4_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_lt"},

    {"get_equal", (PyCFunction)RealizationKey4_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_equal"},

    {"get_ge", (PyCFunction)RealizationKey4_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_ge"},

    {"get_prev", (PyCFunction)RealizationKey4_prev, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_prev"},

    {"get_first", (PyCFunction)RealizationKey4_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_first"},

    {"gets_gt", (PyCFunction)RealizationKey4_iter_gt, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_iter_gt"},

    {"gets_last", (PyCFunction)RealizationKey4_iter_last, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_iter_last"},

    {"gets_le", (PyCFunction)RealizationKey4_iter_le, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_iter_le"},

    {"gets_ge", (PyCFunction)RealizationKey4_iter_ge, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_iter_ge"},

    {"gets_equal", (PyCFunction)RealizationKey4_iter_equal, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_iter_equal"},

    {"gets_lt", (PyCFunction)RealizationKey4_iter_lt, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_iter_lt"},

    {"gets_first", (PyCFunction)RealizationKey4_iter_first, METH_VARARGS|METH_KEYWORDS, "RealizationKey4_iter_first"},

    {NULL}
};

static PyTypeObject RealizationKey4Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.RealizationKey4",             /*tp_name*/
    sizeof(RealizationKey4),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RealizationKey4_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)RealizationKey4_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RealizationKey4 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RealizationKey4_methods,             /* tp_methods */
    0,
    RealizationKey4_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RealizationKey4_init,      /* tp_init */
    0,                         /* tp_alloc */
    RealizationKey4_new,                 /* tp_new */
};
PyTypeObject *getRealizationKey4Type()
{
    return &RealizationKey4Type_;
}
