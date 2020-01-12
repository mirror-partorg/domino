
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "partner_data.h"

static void PartnerData_dealloc(PyObject* self)
{
    do_free(((PartnerData*)self)->priv);
    Py_DECREF(((PartnerData*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("PartnerData free\n");
}
static PyObject *PartnerData_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PartnerData new\n");
    PartnerData *self;
    self = PyObject_NEW(PartnerData, type);
    //self = (PartnerData *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_data_rec_t));
    return (PyObject *)self;
}
static int PartnerData_init(PartnerData *self, PyObject *args, PyObject *kwds)
{
    //LOG("PartnerData init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.g_code);

    do_text_set_empty(self->priv->data.analitics);

    do_partner_data_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *PartnerData_clone(PartnerData *self)
{
    PartnerData *result;
    result = (PartnerData*)PyObject_CallFunction((PyObject*)getPartnerDataType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *PartnerData_region_code(PartnerData* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerData_set_region_code(PartnerData* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerData_code(PartnerData* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.code);

    return result;
}

static PyObject *PartnerData_set_code(PartnerData* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerData_record_type(PartnerData* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type);

    return result;
}

static PyObject *PartnerData_set_record_type(PartnerData* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.type");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.type = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.type = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.type");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.type);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerData_number(PartnerData* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.number);

    return result;
}

static PyObject *PartnerData_set_number(PartnerData* self, PyObject *args, PyObject *kwds)
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

static PyObject *PartnerData_analitics(PartnerData* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.analitics);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerData_set_analitics(PartnerData* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.analitics");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.analitics, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.analitics);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerData_set_param(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_partner_data_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_partner_data_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_partner_data_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *PartnerData_set_params(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_partner_data_params_clear(self->alias->alias,self->priv);
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
                 do_partner_data_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_partner_data_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_partner_data_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *PartnerData_get_param(PartnerData* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_partner_data_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *PartnerData_params_clear(PartnerData* self)
{
    do_partner_data_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *PartnerData_get_params(PartnerData* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *PartnerData_prev(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_gt(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_next(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_ge(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_equal(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_last(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_lt(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_le(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_first(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerDataKey0Type() )
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() )
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() )
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_iter_gt(PartnerData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
        key_cmp = (partner_data_key0_t*)do_malloc(sizeof(partner_data_key0_t));
        memcpy(key_cmp, ((PartnerDataKey0*)key)->priv, sizeof(partner_data_key0_t));
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
        key_cmp = (partner_data_key1_t*)do_malloc(sizeof(partner_data_key1_t));
        memcpy(key_cmp, ((PartnerDataKey1*)key)->priv, sizeof(partner_data_key1_t));
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
        key_cmp = (partner_data_key2_t*)do_malloc(sizeof(partner_data_key2_t));
        memcpy(key_cmp, ((PartnerDataKey2*)key)->priv, sizeof(partner_data_key2_t));
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->g_code, 
                    ((PartnerDataKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->code, 
                    ((PartnerDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->type, 
                    ((PartnerDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->number, 
                    ((PartnerDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->g_code, 
                    ((PartnerDataKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->code, 
                    ((PartnerDataKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->type, 
                    ((PartnerDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->analitics, 
                    ((PartnerDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->type, 
                    ((PartnerDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->analitics, 
                    ((PartnerDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = PartnerData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
            status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
            status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
            status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *PartnerData_iter_ge(PartnerData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
        key_cmp = (partner_data_key0_t*)do_malloc(sizeof(partner_data_key0_t));
        memcpy(key_cmp, ((PartnerDataKey0*)key)->priv, sizeof(partner_data_key0_t));
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
        key_cmp = (partner_data_key1_t*)do_malloc(sizeof(partner_data_key1_t));
        memcpy(key_cmp, ((PartnerDataKey1*)key)->priv, sizeof(partner_data_key1_t));
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
        key_cmp = (partner_data_key2_t*)do_malloc(sizeof(partner_data_key2_t));
        memcpy(key_cmp, ((PartnerDataKey2*)key)->priv, sizeof(partner_data_key2_t));
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->g_code, 
                    ((PartnerDataKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->code, 
                    ((PartnerDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->type, 
                    ((PartnerDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->number, 
                    ((PartnerDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->g_code, 
                    ((PartnerDataKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->code, 
                    ((PartnerDataKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->type, 
                    ((PartnerDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->analitics, 
                    ((PartnerDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->type, 
                    ((PartnerDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->analitics, 
                    ((PartnerDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = PartnerData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
            status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
            status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
            status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *PartnerData_iter_equal(PartnerData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
        key_cmp = (partner_data_key0_t*)do_malloc(sizeof(partner_data_key0_t));
        memcpy(key_cmp, ((PartnerDataKey0*)key)->priv, sizeof(partner_data_key0_t));
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
        key_cmp = (partner_data_key1_t*)do_malloc(sizeof(partner_data_key1_t));
        memcpy(key_cmp, ((PartnerDataKey1*)key)->priv, sizeof(partner_data_key1_t));
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
        key_cmp = (partner_data_key2_t*)do_malloc(sizeof(partner_data_key2_t));
        memcpy(key_cmp, ((PartnerDataKey2*)key)->priv, sizeof(partner_data_key2_t));
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->g_code, 
                    ((PartnerDataKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->code, 
                    ((PartnerDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->type, 
                    ((PartnerDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->number, 
                    ((PartnerDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->g_code, 
                    ((PartnerDataKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->code, 
                    ((PartnerDataKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->type, 
                    ((PartnerDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->analitics, 
                    ((PartnerDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->type, 
                    ((PartnerDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->analitics, 
                    ((PartnerDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = PartnerData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
            status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
            status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
            status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *PartnerData_iter_last(PartnerData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
        key_cmp = (partner_data_key0_t*)do_malloc(sizeof(partner_data_key0_t));
        memcpy(key_cmp, ((PartnerDataKey0*)key)->priv, sizeof(partner_data_key0_t));
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
        key_cmp = (partner_data_key1_t*)do_malloc(sizeof(partner_data_key1_t));
        memcpy(key_cmp, ((PartnerDataKey1*)key)->priv, sizeof(partner_data_key1_t));
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
        key_cmp = (partner_data_key2_t*)do_malloc(sizeof(partner_data_key2_t));
        memcpy(key_cmp, ((PartnerDataKey2*)key)->priv, sizeof(partner_data_key2_t));
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->g_code, 
                    ((PartnerDataKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->code, 
                    ((PartnerDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->type, 
                    ((PartnerDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->number, 
                    ((PartnerDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->g_code, 
                    ((PartnerDataKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->code, 
                    ((PartnerDataKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->type, 
                    ((PartnerDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->analitics, 
                    ((PartnerDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->type, 
                    ((PartnerDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->analitics, 
                    ((PartnerDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = PartnerData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
            status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
            status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
            status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *PartnerData_iter_lt(PartnerData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
        key_cmp = (partner_data_key0_t*)do_malloc(sizeof(partner_data_key0_t));
        memcpy(key_cmp, ((PartnerDataKey0*)key)->priv, sizeof(partner_data_key0_t));
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
        key_cmp = (partner_data_key1_t*)do_malloc(sizeof(partner_data_key1_t));
        memcpy(key_cmp, ((PartnerDataKey1*)key)->priv, sizeof(partner_data_key1_t));
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
        key_cmp = (partner_data_key2_t*)do_malloc(sizeof(partner_data_key2_t));
        memcpy(key_cmp, ((PartnerDataKey2*)key)->priv, sizeof(partner_data_key2_t));
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->g_code, 
                    ((PartnerDataKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->code, 
                    ((PartnerDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->type, 
                    ((PartnerDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->number, 
                    ((PartnerDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->g_code, 
                    ((PartnerDataKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->code, 
                    ((PartnerDataKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->type, 
                    ((PartnerDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->analitics, 
                    ((PartnerDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->type, 
                    ((PartnerDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->analitics, 
                    ((PartnerDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = PartnerData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
            status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
            status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
            status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *PartnerData_iter_le(PartnerData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
        key_cmp = (partner_data_key0_t*)do_malloc(sizeof(partner_data_key0_t));
        memcpy(key_cmp, ((PartnerDataKey0*)key)->priv, sizeof(partner_data_key0_t));
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
        key_cmp = (partner_data_key1_t*)do_malloc(sizeof(partner_data_key1_t));
        memcpy(key_cmp, ((PartnerDataKey1*)key)->priv, sizeof(partner_data_key1_t));
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
        key_cmp = (partner_data_key2_t*)do_malloc(sizeof(partner_data_key2_t));
        memcpy(key_cmp, ((PartnerDataKey2*)key)->priv, sizeof(partner_data_key2_t));
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->g_code, 
                    ((PartnerDataKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->code, 
                    ((PartnerDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->type, 
                    ((PartnerDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->number, 
                    ((PartnerDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->g_code, 
                    ((PartnerDataKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->code, 
                    ((PartnerDataKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->type, 
                    ((PartnerDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->analitics, 
                    ((PartnerDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->type, 
                    ((PartnerDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->analitics, 
                    ((PartnerDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = PartnerData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
            status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
            status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
            status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *PartnerData_iter_first(PartnerData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
        key_cmp = (partner_data_key0_t*)do_malloc(sizeof(partner_data_key0_t));
        memcpy(key_cmp, ((PartnerDataKey0*)key)->priv, sizeof(partner_data_key0_t));
        status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
        key_cmp = (partner_data_key1_t*)do_malloc(sizeof(partner_data_key1_t));
        memcpy(key_cmp, ((PartnerDataKey1*)key)->priv, sizeof(partner_data_key1_t));
        status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
        key_cmp = (partner_data_key2_t*)do_malloc(sizeof(partner_data_key2_t));
        memcpy(key_cmp, ((PartnerDataKey2*)key)->priv, sizeof(partner_data_key2_t));
        status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->g_code, 
                    ((PartnerDataKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->code, 
                    ((PartnerDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->type, 
                    ((PartnerDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key0_t*)key_cmp)->number, 
                    ((PartnerDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->g_code, 
                    ((PartnerDataKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->code, 
                    ((PartnerDataKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->type, 
                    ((PartnerDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((partner_data_key1_t*)key_cmp)->analitics, 
                    ((PartnerDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->type, 
                    ((PartnerDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_data_key2_t*)key_cmp)->analitics, 
                    ((PartnerDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = PartnerData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerDataKey0Type() ) {
            status = do_partner_data_get0(self->alias->alias, self->priv, ((PartnerDataKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey1Type() ) {
            status = do_partner_data_get1(self->alias->alias, self->priv, ((PartnerDataKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerDataKey2Type() ) {
            status = do_partner_data_get2(self->alias->alias, self->priv, ((PartnerDataKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *PartnerData_update(PartnerData* self)
{
    int status;
    status = do_partner_data_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_insert(PartnerData* self)
{
    int status;
    status = do_partner_data_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_delete(PartnerData* self)
{
    int status;
    status = do_partner_data_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerData_set_alias(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((PartnerData*)self)->alias);
    Py_INCREF(obj);
    ((PartnerData*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PartnerData_set(PartnerData* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerDataType() ) {
        memcpy(self->priv, ((PartnerData*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey0Type() ) {

        do_cpy(self->priv->data.g_code, ((PartnerDataKey0*)obj)->priv->g_code);

        do_cpy(self->priv->data.code, ((PartnerDataKey0*)obj)->priv->code);

        do_cpy(self->priv->data.type, ((PartnerDataKey0*)obj)->priv->type);

        do_cpy(self->priv->data.number, ((PartnerDataKey0*)obj)->priv->number);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey1Type() ) {

        do_cpy(self->priv->data.g_code, ((PartnerDataKey1*)obj)->priv->g_code);

        do_cpy(self->priv->data.code, ((PartnerDataKey1*)obj)->priv->code);

        do_cpy(self->priv->data.type, ((PartnerDataKey1*)obj)->priv->type);

        do_cpy(self->priv->data.analitics, ((PartnerDataKey1*)obj)->priv->analitics);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey2Type() ) {

        do_cpy(self->priv->data.type, ((PartnerDataKey2*)obj)->priv->type);

        do_cpy(self->priv->data.analitics, ((PartnerDataKey2*)obj)->priv->analitics);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerType() ) {

        do_cpy(self->priv->data.g_code, ((Partner*)obj)->priv->data.g_code);

        do_cpy(self->priv->data.code, ((Partner*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey0Type() ) {

        do_cpy(self->priv->data.g_code, ((PartnerKey0*)obj)->priv->g_code);

        do_cpy(self->priv->data.code, ((PartnerKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey1Type() ) {

        do_cpy(self->priv->data.g_code, ((PartnerKey1*)obj)->priv->g_code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey3Type() ) {

        do_cpy(self->priv->data.code, ((PartnerKey3*)obj)->priv->code);

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
            if ( !strcmp("region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("number", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_number", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("analitics", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_analitics", "O", value) )
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
                do_log(LOG_ERR, "Class \"PartnerData\"does not contain \"%s\"", field_name);
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

static PyObject *PartnerData_fields(PartnerData* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class PartnerData");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  PartnerData_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerData_code(self,NULL);
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

    value =  PartnerData_record_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("record_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerData_number(self,NULL);
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

    value =  PartnerData_analitics(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("analitics", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = PartnerData_get_params(self, NULL);
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

static PyGetSetDef PartnerData_getset[] = {

    {"region_code",(getter)PartnerData_region_code},

    {"code",(getter)PartnerData_code},

    {"record_type",(getter)PartnerData_record_type},

    {"number",(getter)PartnerData_number},

    {"analitics",(getter)PartnerData_analitics},

    {"params",(getter)PartnerData_get_params},

    {NULL}
};

static PyMethodDef PartnerData_methods[] = {
    {"set", (PyCFunction)PartnerData_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)PartnerData_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)PartnerData_clone, METH_NOARGS, "clone"},

    {"set_region_code", (PyCFunction)PartnerData_set_region_code, METH_VARARGS|METH_KEYWORDS, "PartnerData_set_region_code"},

    {"set_code", (PyCFunction)PartnerData_set_code, METH_VARARGS|METH_KEYWORDS, "PartnerData_set_code"},

    {"set_record_type", (PyCFunction)PartnerData_set_record_type, METH_VARARGS|METH_KEYWORDS, "PartnerData_set_record_type"},

    {"set_number", (PyCFunction)PartnerData_set_number, METH_VARARGS|METH_KEYWORDS, "PartnerData_set_number"},

    {"set_analitics", (PyCFunction)PartnerData_set_analitics, METH_VARARGS|METH_KEYWORDS, "PartnerData_set_analitics"},

    {"set_param", (PyCFunction)PartnerData_set_param, METH_VARARGS|METH_KEYWORDS, "do_PartnerData_param_set"},
    {"param", (PyCFunction)PartnerData_get_param, METH_VARARGS|METH_KEYWORDS, "do_PartnerData_param"},
    {"clear_params", (PyCFunction)PartnerData_params_clear, METH_NOARGS, "do_PartnerData_param_clear"},
    {"set_params", (PyCFunction)PartnerData_set_params, METH_VARARGS|METH_KEYWORDS, "do_PartnerData_set_params"},

    {"get_prev", (PyCFunction)PartnerData_prev, METH_VARARGS|METH_KEYWORDS, "PartnerData_prev"},

    {"get_gt", (PyCFunction)PartnerData_gt, METH_VARARGS|METH_KEYWORDS, "PartnerData_gt"},

    {"get_next", (PyCFunction)PartnerData_next, METH_VARARGS|METH_KEYWORDS, "PartnerData_next"},

    {"get_ge", (PyCFunction)PartnerData_ge, METH_VARARGS|METH_KEYWORDS, "PartnerData_ge"},

    {"get_equal", (PyCFunction)PartnerData_equal, METH_VARARGS|METH_KEYWORDS, "PartnerData_equal"},

    {"get_last", (PyCFunction)PartnerData_last, METH_VARARGS|METH_KEYWORDS, "PartnerData_last"},

    {"get_lt", (PyCFunction)PartnerData_lt, METH_VARARGS|METH_KEYWORDS, "PartnerData_lt"},

    {"get_le", (PyCFunction)PartnerData_le, METH_VARARGS|METH_KEYWORDS, "PartnerData_le"},

    {"get_first", (PyCFunction)PartnerData_first, METH_VARARGS|METH_KEYWORDS, "PartnerData_first"},

    {"gets_gt", (PyCFunction)PartnerData_iter_gt, METH_VARARGS|METH_KEYWORDS, "PartnerData_iter_gt"},

    {"gets_ge", (PyCFunction)PartnerData_iter_ge, METH_VARARGS|METH_KEYWORDS, "PartnerData_iter_ge"},

    {"gets_equal", (PyCFunction)PartnerData_iter_equal, METH_VARARGS|METH_KEYWORDS, "PartnerData_iter_equal"},

    {"gets_last", (PyCFunction)PartnerData_iter_last, METH_VARARGS|METH_KEYWORDS, "PartnerData_iter_last"},

    {"gets_lt", (PyCFunction)PartnerData_iter_lt, METH_VARARGS|METH_KEYWORDS, "PartnerData_iter_lt"},

    {"gets_le", (PyCFunction)PartnerData_iter_le, METH_VARARGS|METH_KEYWORDS, "PartnerData_iter_le"},

    {"gets_first", (PyCFunction)PartnerData_iter_first, METH_VARARGS|METH_KEYWORDS, "PartnerData_iter_first"},

    {"update", (PyCFunction)PartnerData_update, METH_VARARGS|METH_KEYWORDS, "PartnerData_update"},

    {"insert", (PyCFunction)PartnerData_insert, METH_VARARGS|METH_KEYWORDS, "PartnerData_insert"},

    {"delete", (PyCFunction)PartnerData_delete, METH_VARARGS|METH_KEYWORDS, "PartnerData_delete"},

    {NULL}
};

static PyTypeObject PartnerDataType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PartnerData",             /*tp_name*/
    sizeof(PartnerData),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerData_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PartnerData_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerData objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerData_methods,             /* tp_methods */
    0,
    PartnerData_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerData_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerData_new,                 /* tp_new */
};
PyTypeObject *getPartnerDataType()
{
    return &PartnerDataType_;
}

static void PartnerDataKey0_dealloc(PyObject* self)
{
    do_free(((PartnerDataKey0*)self)->priv);
    Py_DECREF(((PartnerDataKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("PartnerDataKey0 free\n");
}
static PyObject *PartnerDataKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PartnerDataKey0 new\n");
    PartnerDataKey0 *self;
    self = PyObject_NEW(PartnerDataKey0, type);
    //self = (PartnerDataKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_data_key0_t));
    return (PyObject *)self;
}
static int PartnerDataKey0_init(PartnerDataKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("PartnerDataKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->g_code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *PartnerDataKey0_clone(PartnerDataKey0 *self)
{
    PartnerDataKey0 *result;
    result = (PartnerDataKey0*)PyObject_CallFunction((PyObject*)getPartnerDataKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *PartnerDataKey0_region_code(PartnerDataKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerDataKey0_set_region_code(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey0_code(PartnerDataKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->code);

    return result;
}

static PyObject *PartnerDataKey0_set_code(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey0_record_type(PartnerDataKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *PartnerDataKey0_set_record_type(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->type = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->type = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->type);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey0_number(PartnerDataKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->number);

    return result;
}

static PyObject *PartnerDataKey0_set_number(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *PartnerDataKey0_prev(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_gt(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_next(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_ge(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_equal(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_last(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_lt(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_le(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_first(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey0_iter_gt(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = PartnerDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey0_iter_ge(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = PartnerDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey0_iter_equal(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = PartnerDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey0_iter_last(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = PartnerDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey0_iter_lt(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = PartnerDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey0_iter_le(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = PartnerDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey0_iter_first(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = PartnerDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey0_cmp(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getPartnerDataKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->g_code, ((PartnerDataKey0*)obj)->priv->g_code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->code - ((PartnerDataKey0*)obj)->priv->code;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->type - ((PartnerDataKey0*)obj)->priv->type;
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->number - ((PartnerDataKey0*)obj)->priv->number;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *PartnerDataKey0_set_alias(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((PartnerDataKey0*)self)->alias);
    Py_INCREF(obj);
    ((PartnerDataKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PartnerDataKey0_set(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerDataKey0Type() ) {
        memcpy(self->priv, ((PartnerDataKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataType() ) {

        do_cpy(self->priv->g_code, ((PartnerData*)obj)->priv->data.g_code);

        do_cpy(self->priv->code, ((PartnerData*)obj)->priv->data.code);

        do_cpy(self->priv->type, ((PartnerData*)obj)->priv->data.type);

        do_cpy(self->priv->number, ((PartnerData*)obj)->priv->data.number);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_number", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getPartnerType() ) {

        do_cpy(self->priv->g_code, ((Partner*)obj)->priv->data.g_code);

        do_cpy(self->priv->code, ((Partner*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey0Type() ) {

        do_cpy(self->priv->g_code, ((PartnerKey0*)obj)->priv->g_code);

        do_cpy(self->priv->code, ((PartnerKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey1Type() ) {

        do_cpy(self->priv->g_code, ((PartnerKey1*)obj)->priv->g_code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey3Type() ) {

        do_cpy(self->priv->code, ((PartnerKey3*)obj)->priv->code);

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
            if ( !strcmp("region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("number", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_number", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"PartnerDataKey0\"does not contain \"%s\"", field_name);
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

static PyObject *PartnerDataKey0_fields(PartnerDataKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class PartnerDataKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  PartnerDataKey0_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerDataKey0_code(self,NULL);
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

    value =  PartnerDataKey0_record_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("record_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerDataKey0_number(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef PartnerDataKey0_getset[] = {

    {"region_code",(getter)PartnerDataKey0_region_code},

    {"code",(getter)PartnerDataKey0_code},

    {"record_type",(getter)PartnerDataKey0_record_type},

    {"number",(getter)PartnerDataKey0_number},

    {NULL}
};

static PyMethodDef PartnerDataKey0_methods[] = {
    {"set", (PyCFunction)PartnerDataKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)PartnerDataKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)PartnerDataKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)PartnerDataKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_region_code", (PyCFunction)PartnerDataKey0_set_region_code, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_set_region_code"},

    {"set_code", (PyCFunction)PartnerDataKey0_set_code, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_set_code"},

    {"set_record_type", (PyCFunction)PartnerDataKey0_set_record_type, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_set_record_type"},

    {"set_number", (PyCFunction)PartnerDataKey0_set_number, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_set_number"},

    {"get_prev", (PyCFunction)PartnerDataKey0_prev, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_prev"},

    {"get_gt", (PyCFunction)PartnerDataKey0_gt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_gt"},

    {"get_next", (PyCFunction)PartnerDataKey0_next, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_next"},

    {"get_ge", (PyCFunction)PartnerDataKey0_ge, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_ge"},

    {"get_equal", (PyCFunction)PartnerDataKey0_equal, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_equal"},

    {"get_last", (PyCFunction)PartnerDataKey0_last, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_last"},

    {"get_lt", (PyCFunction)PartnerDataKey0_lt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_lt"},

    {"get_le", (PyCFunction)PartnerDataKey0_le, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_le"},

    {"get_first", (PyCFunction)PartnerDataKey0_first, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_first"},

    {"gets_gt", (PyCFunction)PartnerDataKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_iter_gt"},

    {"gets_ge", (PyCFunction)PartnerDataKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_iter_ge"},

    {"gets_equal", (PyCFunction)PartnerDataKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_iter_equal"},

    {"gets_last", (PyCFunction)PartnerDataKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_iter_last"},

    {"gets_lt", (PyCFunction)PartnerDataKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_iter_lt"},

    {"gets_le", (PyCFunction)PartnerDataKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_iter_le"},

    {"gets_first", (PyCFunction)PartnerDataKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey0_iter_first"},

    {NULL}
};

static PyTypeObject PartnerDataKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PartnerDataKey0",             /*tp_name*/
    sizeof(PartnerDataKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerDataKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PartnerDataKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerDataKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerDataKey0_methods,             /* tp_methods */
    0,
    PartnerDataKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerDataKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerDataKey0_new,                 /* tp_new */
};
PyTypeObject *getPartnerDataKey0Type()
{
    return &PartnerDataKey0Type_;
}

static void PartnerDataKey1_dealloc(PyObject* self)
{
    do_free(((PartnerDataKey1*)self)->priv);
    Py_DECREF(((PartnerDataKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("PartnerDataKey1 free\n");
}
static PyObject *PartnerDataKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PartnerDataKey1 new\n");
    PartnerDataKey1 *self;
    self = PyObject_NEW(PartnerDataKey1, type);
    //self = (PartnerDataKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_data_key1_t));
    return (PyObject *)self;
}
static int PartnerDataKey1_init(PartnerDataKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("PartnerDataKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->g_code);

    do_text_set_empty(self->priv->analitics);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *PartnerDataKey1_clone(PartnerDataKey1 *self)
{
    PartnerDataKey1 *result;
    result = (PartnerDataKey1*)PyObject_CallFunction((PyObject*)getPartnerDataKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *PartnerDataKey1_region_code(PartnerDataKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerDataKey1_set_region_code(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey1_code(PartnerDataKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->code);

    return result;
}

static PyObject *PartnerDataKey1_set_code(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey1_record_type(PartnerDataKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *PartnerDataKey1_set_record_type(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->type = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->type = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->type);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey1_analitics(PartnerDataKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->analitics);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerDataKey1_set_analitics(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set analitics");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->analitics, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->analitics);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey1_prev(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_gt(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_next(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_ge(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_equal(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_last(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_lt(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_le(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_first(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey1_iter_gt(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey1_iter_ge(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey1_iter_equal(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey1_iter_last(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey1_iter_lt(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey1_iter_le(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey1_iter_first(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey1_cmp(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getPartnerDataKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->g_code, ((PartnerDataKey1*)obj)->priv->g_code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->code - ((PartnerDataKey1*)obj)->priv->code;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->type - ((PartnerDataKey1*)obj)->priv->type;
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->analitics, ((PartnerDataKey1*)obj)->priv->analitics);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *PartnerDataKey1_set_alias(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((PartnerDataKey1*)self)->alias);
    Py_INCREF(obj);
    ((PartnerDataKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PartnerDataKey1_set(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerDataKey1Type() ) {
        memcpy(self->priv, ((PartnerDataKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataType() ) {

        do_cpy(self->priv->g_code, ((PartnerData*)obj)->priv->data.g_code);

        do_cpy(self->priv->code, ((PartnerData*)obj)->priv->data.code);

        do_cpy(self->priv->type, ((PartnerData*)obj)->priv->data.type);

        do_cpy(self->priv->analitics, ((PartnerData*)obj)->priv->data.analitics);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_analitics", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getPartnerType() ) {

        do_cpy(self->priv->g_code, ((Partner*)obj)->priv->data.g_code);

        do_cpy(self->priv->code, ((Partner*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey0Type() ) {

        do_cpy(self->priv->g_code, ((PartnerKey0*)obj)->priv->g_code);

        do_cpy(self->priv->code, ((PartnerKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey1Type() ) {

        do_cpy(self->priv->g_code, ((PartnerKey1*)obj)->priv->g_code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey3Type() ) {

        do_cpy(self->priv->code, ((PartnerKey3*)obj)->priv->code);

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
            if ( !strcmp("region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("analitics", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_analitics", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"PartnerDataKey1\"does not contain \"%s\"", field_name);
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

static PyObject *PartnerDataKey1_fields(PartnerDataKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class PartnerDataKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  PartnerDataKey1_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerDataKey1_code(self,NULL);
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

    value =  PartnerDataKey1_record_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("record_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerDataKey1_analitics(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("analitics", value);
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

static PyGetSetDef PartnerDataKey1_getset[] = {

    {"region_code",(getter)PartnerDataKey1_region_code},

    {"code",(getter)PartnerDataKey1_code},

    {"record_type",(getter)PartnerDataKey1_record_type},

    {"analitics",(getter)PartnerDataKey1_analitics},

    {NULL}
};

static PyMethodDef PartnerDataKey1_methods[] = {
    {"set", (PyCFunction)PartnerDataKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)PartnerDataKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)PartnerDataKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)PartnerDataKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_region_code", (PyCFunction)PartnerDataKey1_set_region_code, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_set_region_code"},

    {"set_code", (PyCFunction)PartnerDataKey1_set_code, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_set_code"},

    {"set_record_type", (PyCFunction)PartnerDataKey1_set_record_type, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_set_record_type"},

    {"set_analitics", (PyCFunction)PartnerDataKey1_set_analitics, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_set_analitics"},

    {"get_prev", (PyCFunction)PartnerDataKey1_prev, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_prev"},

    {"get_gt", (PyCFunction)PartnerDataKey1_gt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_gt"},

    {"get_next", (PyCFunction)PartnerDataKey1_next, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_next"},

    {"get_ge", (PyCFunction)PartnerDataKey1_ge, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_ge"},

    {"get_equal", (PyCFunction)PartnerDataKey1_equal, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_equal"},

    {"get_last", (PyCFunction)PartnerDataKey1_last, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_last"},

    {"get_lt", (PyCFunction)PartnerDataKey1_lt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_lt"},

    {"get_le", (PyCFunction)PartnerDataKey1_le, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_le"},

    {"get_first", (PyCFunction)PartnerDataKey1_first, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_first"},

    {"gets_gt", (PyCFunction)PartnerDataKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_iter_gt"},

    {"gets_ge", (PyCFunction)PartnerDataKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_iter_ge"},

    {"gets_equal", (PyCFunction)PartnerDataKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_iter_equal"},

    {"gets_last", (PyCFunction)PartnerDataKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_iter_last"},

    {"gets_lt", (PyCFunction)PartnerDataKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_iter_lt"},

    {"gets_le", (PyCFunction)PartnerDataKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_iter_le"},

    {"gets_first", (PyCFunction)PartnerDataKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey1_iter_first"},

    {NULL}
};

static PyTypeObject PartnerDataKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PartnerDataKey1",             /*tp_name*/
    sizeof(PartnerDataKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerDataKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PartnerDataKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerDataKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerDataKey1_methods,             /* tp_methods */
    0,
    PartnerDataKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerDataKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerDataKey1_new,                 /* tp_new */
};
PyTypeObject *getPartnerDataKey1Type()
{
    return &PartnerDataKey1Type_;
}

static void PartnerDataKey2_dealloc(PyObject* self)
{
    do_free(((PartnerDataKey2*)self)->priv);
    Py_DECREF(((PartnerDataKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("PartnerDataKey2 free\n");
}
static PyObject *PartnerDataKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PartnerDataKey2 new\n");
    PartnerDataKey2 *self;
    self = PyObject_NEW(PartnerDataKey2, type);
    //self = (PartnerDataKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_data_key2_t));
    return (PyObject *)self;
}
static int PartnerDataKey2_init(PartnerDataKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("PartnerDataKey2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->analitics);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *PartnerDataKey2_clone(PartnerDataKey2 *self)
{
    PartnerDataKey2 *result;
    result = (PartnerDataKey2*)PyObject_CallFunction((PyObject*)getPartnerDataKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *PartnerDataKey2_record_type(PartnerDataKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *PartnerDataKey2_set_record_type(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->type = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->type = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->type);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey2_analitics(PartnerDataKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->analitics);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerDataKey2_set_analitics(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set analitics");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->analitics, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->analitics);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerDataKey2_prev(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_gt(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_next(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_ge(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_equal(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_last(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_lt(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_le(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_first(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerDataKey2_iter_gt(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey2_iter_ge(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey2_iter_equal(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey2_iter_last(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey2_iter_lt(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey2_iter_le(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey2_iter_first(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = PartnerDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerDataKey2_cmp(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getPartnerDataKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->type - ((PartnerDataKey2*)obj)->priv->type;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->analitics, ((PartnerDataKey2*)obj)->priv->analitics);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *PartnerDataKey2_set_alias(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((PartnerDataKey2*)self)->alias);
    Py_INCREF(obj);
    ((PartnerDataKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PartnerDataKey2_set(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerDataKey2Type() ) {
        memcpy(self->priv, ((PartnerDataKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataType() ) {

        do_cpy(self->priv->type, ((PartnerData*)obj)->priv->data.type);

        do_cpy(self->priv->analitics, ((PartnerData*)obj)->priv->data.analitics);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_analitics", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getPartnerType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey1Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey3Type() ) {

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
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("analitics", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_analitics", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"PartnerDataKey2\"does not contain \"%s\"", field_name);
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

static PyObject *PartnerDataKey2_fields(PartnerDataKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class PartnerDataKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  PartnerDataKey2_record_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("record_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerDataKey2_analitics(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("analitics", value);
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

static PyGetSetDef PartnerDataKey2_getset[] = {

    {"record_type",(getter)PartnerDataKey2_record_type},

    {"analitics",(getter)PartnerDataKey2_analitics},

    {NULL}
};

static PyMethodDef PartnerDataKey2_methods[] = {
    {"set", (PyCFunction)PartnerDataKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)PartnerDataKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)PartnerDataKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)PartnerDataKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_record_type", (PyCFunction)PartnerDataKey2_set_record_type, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_set_record_type"},

    {"set_analitics", (PyCFunction)PartnerDataKey2_set_analitics, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_set_analitics"},

    {"get_prev", (PyCFunction)PartnerDataKey2_prev, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_prev"},

    {"get_gt", (PyCFunction)PartnerDataKey2_gt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_gt"},

    {"get_next", (PyCFunction)PartnerDataKey2_next, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_next"},

    {"get_ge", (PyCFunction)PartnerDataKey2_ge, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_ge"},

    {"get_equal", (PyCFunction)PartnerDataKey2_equal, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_equal"},

    {"get_last", (PyCFunction)PartnerDataKey2_last, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_last"},

    {"get_lt", (PyCFunction)PartnerDataKey2_lt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_lt"},

    {"get_le", (PyCFunction)PartnerDataKey2_le, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_le"},

    {"get_first", (PyCFunction)PartnerDataKey2_first, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_first"},

    {"gets_gt", (PyCFunction)PartnerDataKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_iter_gt"},

    {"gets_ge", (PyCFunction)PartnerDataKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_iter_ge"},

    {"gets_equal", (PyCFunction)PartnerDataKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_iter_equal"},

    {"gets_last", (PyCFunction)PartnerDataKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_iter_last"},

    {"gets_lt", (PyCFunction)PartnerDataKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_iter_lt"},

    {"gets_le", (PyCFunction)PartnerDataKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_iter_le"},

    {"gets_first", (PyCFunction)PartnerDataKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "PartnerDataKey2_iter_first"},

    {NULL}
};

static PyTypeObject PartnerDataKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PartnerDataKey2",             /*tp_name*/
    sizeof(PartnerDataKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerDataKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PartnerDataKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerDataKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerDataKey2_methods,             /* tp_methods */
    0,
    PartnerDataKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerDataKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerDataKey2_new,                 /* tp_new */
};
PyTypeObject *getPartnerDataKey2Type()
{
    return &PartnerDataKey2Type_;
}
