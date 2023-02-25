
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "sklad.h"

static void Store_dealloc(PyObject* self)
{
    do_free(((Store*)self)->priv);
    Py_DECREF(((Store*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Store free\n");
}
static PyObject *Store_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Store new\n");
    Store *self;
    self = PyObject_NEW(Store, type);
    //self = (Store *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(sklad_rec_t));
    return (PyObject *)self;
}
static int Store_init(Store *self, PyObject *args, PyObject *kwds)
{
    //LOG("Store init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.code);

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.type_);

    do_text_set_empty(self->priv->data.p_g_code);

    do_text_set_empty(self->priv->data.mol);

    do_text_set_empty(self->priv->data.mol1);

    do_sklad_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Store_clone(Store *self)
{
    Store *result;
    result = (Store*)PyObject_CallFunction((PyObject*)getStoreType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Store_code(Store* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Store_set_code(Store* self, PyObject *args, PyObject *kwds)
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

static PyObject *Store_name(Store* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Store_set_name(Store* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.name");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.name, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Store_type(Store* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.type_);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Store_set_type(Store* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.type_");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.type_, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.type_);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Store_partner_region_code(Store* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Store_set_partner_region_code(Store* self, PyObject *args, PyObject *kwds)
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

static PyObject *Store_partner_code(Store* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.p_code);

    return result;
}

static PyObject *Store_set_partner_code(Store* self, PyObject *args, PyObject *kwds)
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

static PyObject *Store_m_o_l(Store* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.mol);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Store_set_m_o_l(Store* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.mol");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.mol, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.mol);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Store_m_o_l_secondary(Store* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.mol1);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Store_set_m_o_l_secondary(Store* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.mol1");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.mol1, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.mol1);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Store_reporting_period_date(Store* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.reporting_period_date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Store_set_reporting_period_date(Store* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.reporting_period_date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.reporting_period_date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.reporting_period_date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->data.reporting_period_date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->data.reporting_period_date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Store_state(Store* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.stat);

    return result;
}

static PyObject *Store_set_state(Store* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.stat");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.stat = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.stat = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.stat");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.stat);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Store_set_param(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_sklad_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_sklad_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_sklad_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Store_set_params(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_sklad_params_clear(self->alias->alias,self->priv);
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
                 do_sklad_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_sklad_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_sklad_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Store_get_param(Store* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_sklad_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Store_params_clear(Store* self)
{
    do_sklad_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Store_get_params(Store* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *Store_equal(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_next(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_prev(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_gt(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_ge(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_lt(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_le(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_first(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_last(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStoreKey0Type() )
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_iter_equal(Store* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStoreKey0Type() ) {
        key_cmp = (sklad_key0_t*)do_malloc(sizeof(sklad_key0_t));
        memcpy(key_cmp, ((StoreKey0*)key)->priv, sizeof(sklad_key0_t));
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((sklad_key0_t*)key_cmp)->code, 
                    ((StoreKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Store_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
            status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Store_iter_gt(Store* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStoreKey0Type() ) {
        key_cmp = (sklad_key0_t*)do_malloc(sizeof(sklad_key0_t));
        memcpy(key_cmp, ((StoreKey0*)key)->priv, sizeof(sklad_key0_t));
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((sklad_key0_t*)key_cmp)->code, 
                    ((StoreKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Store_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
            status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Store_iter_ge(Store* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStoreKey0Type() ) {
        key_cmp = (sklad_key0_t*)do_malloc(sizeof(sklad_key0_t));
        memcpy(key_cmp, ((StoreKey0*)key)->priv, sizeof(sklad_key0_t));
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((sklad_key0_t*)key_cmp)->code, 
                    ((StoreKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Store_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
            status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Store_iter_lt(Store* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStoreKey0Type() ) {
        key_cmp = (sklad_key0_t*)do_malloc(sizeof(sklad_key0_t));
        memcpy(key_cmp, ((StoreKey0*)key)->priv, sizeof(sklad_key0_t));
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((sklad_key0_t*)key_cmp)->code, 
                    ((StoreKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Store_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
            status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Store_iter_le(Store* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStoreKey0Type() ) {
        key_cmp = (sklad_key0_t*)do_malloc(sizeof(sklad_key0_t));
        memcpy(key_cmp, ((StoreKey0*)key)->priv, sizeof(sklad_key0_t));
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((sklad_key0_t*)key_cmp)->code, 
                    ((StoreKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Store_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
            status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Store_iter_first(Store* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStoreKey0Type() ) {
        key_cmp = (sklad_key0_t*)do_malloc(sizeof(sklad_key0_t));
        memcpy(key_cmp, ((StoreKey0*)key)->priv, sizeof(sklad_key0_t));
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((sklad_key0_t*)key_cmp)->code, 
                    ((StoreKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Store_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
            status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Store_iter_last(Store* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStoreKey0Type() ) {
        key_cmp = (sklad_key0_t*)do_malloc(sizeof(sklad_key0_t));
        memcpy(key_cmp, ((StoreKey0*)key)->priv, sizeof(sklad_key0_t));
        status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((sklad_key0_t*)key_cmp)->code, 
                    ((StoreKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Store_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStoreKey0Type() ) {
            status = do_sklad_get0(self->alias->alias, self->priv, ((StoreKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Store_insert(Store* self)
{
    int status;
    status = do_sklad_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_update(Store* self)
{
    int status;
    status = do_sklad_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_delete(Store* self)
{
    int status;
    status = do_sklad_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Store_set_alias(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Store*)self)->alias);
    Py_INCREF(obj);
    ((Store*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Store_set(Store* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getStoreType() ) {
        memcpy(self->priv, ((Store*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getStoreKey0Type() ) {

        do_cpy(self->priv->data.code, ((StoreKey0*)obj)->priv->code);

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
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_type", "O", value) )
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
            if ( !strcmp("m_o_l", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_m_o_l", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("m_o_l_secondary", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_m_o_l_secondary", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("reporting_period_date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_reporting_period_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("state", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_state", "O", value) )
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
                do_log(LOG_ERR, "Class \"Store\"does not contain \"%s\"", field_name);
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

static PyObject *Store_fields(Store* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Store");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Store_code(self,NULL);
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

    value =  Store_name(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("name", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Store_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Store_partner_region_code(self,NULL);
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

    value =  Store_partner_code(self,NULL);
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

    value =  Store_m_o_l(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("m_o_l", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Store_m_o_l_secondary(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("m_o_l_secondary", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Store_reporting_period_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("reporting_period_date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Store_state(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("state", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Store_get_params(self, NULL);
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

static PyGetSetDef Store_getset[] = {

    {"code",(getter)Store_code},

    {"name",(getter)Store_name},

    {"type",(getter)Store_type},

    {"partner_region_code",(getter)Store_partner_region_code},

    {"partner_code",(getter)Store_partner_code},

    {"m_o_l",(getter)Store_m_o_l},

    {"m_o_l_secondary",(getter)Store_m_o_l_secondary},

    {"reporting_period_date",(getter)Store_reporting_period_date},

    {"state",(getter)Store_state},

    {"params",(getter)Store_get_params},

    {NULL}
};

static PyMethodDef Store_methods[] = {
    {"set", (PyCFunction)Store_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Store_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Store_clone, METH_NOARGS, "clone"},

    {"set_code", (PyCFunction)Store_set_code, METH_VARARGS|METH_KEYWORDS, "Store_set_code"},

    {"set_name", (PyCFunction)Store_set_name, METH_VARARGS|METH_KEYWORDS, "Store_set_name"},

    {"set_type", (PyCFunction)Store_set_type, METH_VARARGS|METH_KEYWORDS, "Store_set_type"},

    {"set_partner_region_code", (PyCFunction)Store_set_partner_region_code, METH_VARARGS|METH_KEYWORDS, "Store_set_partner_region_code"},

    {"set_partner_code", (PyCFunction)Store_set_partner_code, METH_VARARGS|METH_KEYWORDS, "Store_set_partner_code"},

    {"set_m_o_l", (PyCFunction)Store_set_m_o_l, METH_VARARGS|METH_KEYWORDS, "Store_set_m_o_l"},

    {"set_m_o_l_secondary", (PyCFunction)Store_set_m_o_l_secondary, METH_VARARGS|METH_KEYWORDS, "Store_set_m_o_l_secondary"},

    {"set_reporting_period_date", (PyCFunction)Store_set_reporting_period_date, METH_VARARGS|METH_KEYWORDS, "Store_set_reporting_period_date"},

    {"set_state", (PyCFunction)Store_set_state, METH_VARARGS|METH_KEYWORDS, "Store_set_state"},

    {"set_param", (PyCFunction)Store_set_param, METH_VARARGS|METH_KEYWORDS, "do_Store_param_set"},
    {"param", (PyCFunction)Store_get_param, METH_VARARGS|METH_KEYWORDS, "do_Store_param"},
    {"clear_params", (PyCFunction)Store_params_clear, METH_NOARGS, "do_Store_param_clear"},
    {"set_params", (PyCFunction)Store_set_params, METH_VARARGS|METH_KEYWORDS, "do_Store_set_params"},

    {"get_equal", (PyCFunction)Store_equal, METH_VARARGS|METH_KEYWORDS, "Store_equal"},

    {"get_next", (PyCFunction)Store_next, METH_VARARGS|METH_KEYWORDS, "Store_next"},

    {"get_prev", (PyCFunction)Store_prev, METH_VARARGS|METH_KEYWORDS, "Store_prev"},

    {"get_gt", (PyCFunction)Store_gt, METH_VARARGS|METH_KEYWORDS, "Store_gt"},

    {"get_ge", (PyCFunction)Store_ge, METH_VARARGS|METH_KEYWORDS, "Store_ge"},

    {"get_lt", (PyCFunction)Store_lt, METH_VARARGS|METH_KEYWORDS, "Store_lt"},

    {"get_le", (PyCFunction)Store_le, METH_VARARGS|METH_KEYWORDS, "Store_le"},

    {"get_first", (PyCFunction)Store_first, METH_VARARGS|METH_KEYWORDS, "Store_first"},

    {"get_last", (PyCFunction)Store_last, METH_VARARGS|METH_KEYWORDS, "Store_last"},

    {"gets_equal", (PyCFunction)Store_iter_equal, METH_VARARGS|METH_KEYWORDS, "Store_iter_equal"},

    {"gets_gt", (PyCFunction)Store_iter_gt, METH_VARARGS|METH_KEYWORDS, "Store_iter_gt"},

    {"gets_ge", (PyCFunction)Store_iter_ge, METH_VARARGS|METH_KEYWORDS, "Store_iter_ge"},

    {"gets_lt", (PyCFunction)Store_iter_lt, METH_VARARGS|METH_KEYWORDS, "Store_iter_lt"},

    {"gets_le", (PyCFunction)Store_iter_le, METH_VARARGS|METH_KEYWORDS, "Store_iter_le"},

    {"gets_first", (PyCFunction)Store_iter_first, METH_VARARGS|METH_KEYWORDS, "Store_iter_first"},

    {"gets_last", (PyCFunction)Store_iter_last, METH_VARARGS|METH_KEYWORDS, "Store_iter_last"},

    {"insert", (PyCFunction)Store_insert, METH_VARARGS|METH_KEYWORDS, "Store_insert"},

    {"update", (PyCFunction)Store_update, METH_VARARGS|METH_KEYWORDS, "Store_update"},

    {"delete", (PyCFunction)Store_delete, METH_VARARGS|METH_KEYWORDS, "Store_delete"},

    {NULL}
};

static PyTypeObject StoreType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Store",             /*tp_name*/
    sizeof(Store),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Store_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Store_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Store objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Store_methods,             /* tp_methods */
    0,
    Store_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Store_init,      /* tp_init */
    0,                         /* tp_alloc */
    Store_new,                 /* tp_new */
};
PyTypeObject *getStoreType()
{
    return &StoreType_;
}

static void StoreKey0_dealloc(PyObject* self)
{
    do_free(((StoreKey0*)self)->priv);
    Py_DECREF(((StoreKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("StoreKey0 free\n");
}
static PyObject *StoreKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("StoreKey0 new\n");
    StoreKey0 *self;
    self = PyObject_NEW(StoreKey0, type);
    //self = (StoreKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(sklad_key0_t));
    return (PyObject *)self;
}
static int StoreKey0_init(StoreKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("StoreKey0 init\n");
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

static PyObject *StoreKey0_clone(StoreKey0 *self)
{
    StoreKey0 *result;
    result = (StoreKey0*)PyObject_CallFunction((PyObject*)getStoreKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *StoreKey0_code(StoreKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *StoreKey0_set_code(StoreKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *StoreKey0_equal(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_next(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_prev(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_gt(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_ge(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_lt(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_le(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_first(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_last(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StoreKey0_iter_equal(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    sklad_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StoreKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StoreKey0_iter_gt(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    sklad_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StoreKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StoreKey0_iter_ge(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    sklad_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StoreKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StoreKey0_iter_lt(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    sklad_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StoreKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StoreKey0_iter_le(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    sklad_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StoreKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StoreKey0_iter_first(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    sklad_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StoreKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StoreKey0_iter_last(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    sklad_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StoreKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_sklad_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StoreKey0_cmp(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getStoreKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((StoreKey0*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *StoreKey0_set_alias(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((StoreKey0*)self)->alias);
    Py_INCREF(obj);
    ((StoreKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *StoreKey0_set(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getStoreKey0Type() ) {
        memcpy(self->priv, ((StoreKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getStoreType() ) {

        do_cpy(self->priv->code, ((Store*)obj)->priv->data.code);

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

            else {
                do_log(LOG_ERR, "Class \"StoreKey0\"does not contain \"%s\"", field_name);
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

static PyObject *StoreKey0_fields(StoreKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class StoreKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  StoreKey0_code(self,NULL);
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

static PyGetSetDef StoreKey0_getset[] = {

    {"code",(getter)StoreKey0_code},

    {NULL}
};

static PyMethodDef StoreKey0_methods[] = {
    {"set", (PyCFunction)StoreKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)StoreKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)StoreKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)StoreKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)StoreKey0_set_code, METH_VARARGS|METH_KEYWORDS, "StoreKey0_set_code"},

    {"get_equal", (PyCFunction)StoreKey0_equal, METH_VARARGS|METH_KEYWORDS, "StoreKey0_equal"},

    {"get_next", (PyCFunction)StoreKey0_next, METH_VARARGS|METH_KEYWORDS, "StoreKey0_next"},

    {"get_prev", (PyCFunction)StoreKey0_prev, METH_VARARGS|METH_KEYWORDS, "StoreKey0_prev"},

    {"get_gt", (PyCFunction)StoreKey0_gt, METH_VARARGS|METH_KEYWORDS, "StoreKey0_gt"},

    {"get_ge", (PyCFunction)StoreKey0_ge, METH_VARARGS|METH_KEYWORDS, "StoreKey0_ge"},

    {"get_lt", (PyCFunction)StoreKey0_lt, METH_VARARGS|METH_KEYWORDS, "StoreKey0_lt"},

    {"get_le", (PyCFunction)StoreKey0_le, METH_VARARGS|METH_KEYWORDS, "StoreKey0_le"},

    {"get_first", (PyCFunction)StoreKey0_first, METH_VARARGS|METH_KEYWORDS, "StoreKey0_first"},

    {"get_last", (PyCFunction)StoreKey0_last, METH_VARARGS|METH_KEYWORDS, "StoreKey0_last"},

    {"gets_equal", (PyCFunction)StoreKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "StoreKey0_iter_equal"},

    {"gets_gt", (PyCFunction)StoreKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "StoreKey0_iter_gt"},

    {"gets_ge", (PyCFunction)StoreKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "StoreKey0_iter_ge"},

    {"gets_lt", (PyCFunction)StoreKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "StoreKey0_iter_lt"},

    {"gets_le", (PyCFunction)StoreKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "StoreKey0_iter_le"},

    {"gets_first", (PyCFunction)StoreKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "StoreKey0_iter_first"},

    {"gets_last", (PyCFunction)StoreKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "StoreKey0_iter_last"},

    {NULL}
};

static PyTypeObject StoreKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.StoreKey0",             /*tp_name*/
    sizeof(StoreKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)StoreKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)StoreKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "StoreKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    StoreKey0_methods,             /* tp_methods */
    0,
    StoreKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)StoreKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    StoreKey0_new,                 /* tp_new */
};
PyTypeObject *getStoreKey0Type()
{
    return &StoreKey0Type_;
}
