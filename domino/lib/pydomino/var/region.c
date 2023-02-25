
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "region.h"

static void Region_dealloc(PyObject* self)
{
    do_free(((Region*)self)->priv);
    Py_DECREF(((Region*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Region free\n");
}
static PyObject *Region_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Region new\n");
    Region *self;
    self = PyObject_NEW(Region, type);
    //self = (Region *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(region_rec_t));
    return (PyObject *)self;
}
static int Region_init(Region *self, PyObject *args, PyObject *kwds)
{
    //LOG("Region init\n");
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

    do_region_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Region_clone(Region *self)
{
    Region *result;
    result = (Region*)PyObject_CallFunction((PyObject*)getRegionType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Region_code(Region* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Region_set_code(Region* self, PyObject *args, PyObject *kwds)
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

static PyObject *Region_name(Region* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Region_set_name(Region* self, PyObject *args, PyObject *kwds)
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

static PyObject *Region_state(Region* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.stat);

    return result;
}

static PyObject *Region_set_state(Region* self, PyObject *args, PyObject *kwds)
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

static PyObject *Region_set_param(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_region_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_region_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_region_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Region_set_params(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_region_params_clear(self->alias->alias,self->priv);
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
                 do_region_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_region_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_region_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Region_get_param(Region* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_region_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Region_params_clear(Region* self)
{
    do_region_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Region_get_params(Region* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *Region_equal(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_next(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_prev(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_gt(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_ge(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_lt(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_le(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_first(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_last(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getRegionKey0Type() )
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getRegionKey1Type() )
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_iter_equal(Region* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRegionKey0Type() ) {
        key_cmp = (region_key0_t*)do_malloc(sizeof(region_key0_t));
        memcpy(key_cmp, ((RegionKey0*)key)->priv, sizeof(region_key0_t));
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getRegionKey1Type() ) {
        key_cmp = (region_key1_t*)do_malloc(sizeof(region_key1_t));
        memcpy(key_cmp, ((RegionKey1*)key)->priv, sizeof(region_key1_t));
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key0_t*)key_cmp)->code, 
                    ((RegionKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key1_t*)key_cmp)->name, 
                    ((RegionKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Region_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
            status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
            status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Region_iter_gt(Region* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRegionKey0Type() ) {
        key_cmp = (region_key0_t*)do_malloc(sizeof(region_key0_t));
        memcpy(key_cmp, ((RegionKey0*)key)->priv, sizeof(region_key0_t));
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getRegionKey1Type() ) {
        key_cmp = (region_key1_t*)do_malloc(sizeof(region_key1_t));
        memcpy(key_cmp, ((RegionKey1*)key)->priv, sizeof(region_key1_t));
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key0_t*)key_cmp)->code, 
                    ((RegionKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key1_t*)key_cmp)->name, 
                    ((RegionKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Region_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
            status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
            status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Region_iter_ge(Region* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRegionKey0Type() ) {
        key_cmp = (region_key0_t*)do_malloc(sizeof(region_key0_t));
        memcpy(key_cmp, ((RegionKey0*)key)->priv, sizeof(region_key0_t));
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getRegionKey1Type() ) {
        key_cmp = (region_key1_t*)do_malloc(sizeof(region_key1_t));
        memcpy(key_cmp, ((RegionKey1*)key)->priv, sizeof(region_key1_t));
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key0_t*)key_cmp)->code, 
                    ((RegionKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key1_t*)key_cmp)->name, 
                    ((RegionKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Region_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
            status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
            status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Region_iter_lt(Region* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRegionKey0Type() ) {
        key_cmp = (region_key0_t*)do_malloc(sizeof(region_key0_t));
        memcpy(key_cmp, ((RegionKey0*)key)->priv, sizeof(region_key0_t));
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getRegionKey1Type() ) {
        key_cmp = (region_key1_t*)do_malloc(sizeof(region_key1_t));
        memcpy(key_cmp, ((RegionKey1*)key)->priv, sizeof(region_key1_t));
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key0_t*)key_cmp)->code, 
                    ((RegionKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key1_t*)key_cmp)->name, 
                    ((RegionKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Region_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
            status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
            status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Region_iter_le(Region* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRegionKey0Type() ) {
        key_cmp = (region_key0_t*)do_malloc(sizeof(region_key0_t));
        memcpy(key_cmp, ((RegionKey0*)key)->priv, sizeof(region_key0_t));
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getRegionKey1Type() ) {
        key_cmp = (region_key1_t*)do_malloc(sizeof(region_key1_t));
        memcpy(key_cmp, ((RegionKey1*)key)->priv, sizeof(region_key1_t));
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key0_t*)key_cmp)->code, 
                    ((RegionKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key1_t*)key_cmp)->name, 
                    ((RegionKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Region_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
            status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
            status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Region_iter_first(Region* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRegionKey0Type() ) {
        key_cmp = (region_key0_t*)do_malloc(sizeof(region_key0_t));
        memcpy(key_cmp, ((RegionKey0*)key)->priv, sizeof(region_key0_t));
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getRegionKey1Type() ) {
        key_cmp = (region_key1_t*)do_malloc(sizeof(region_key1_t));
        memcpy(key_cmp, ((RegionKey1*)key)->priv, sizeof(region_key1_t));
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key0_t*)key_cmp)->code, 
                    ((RegionKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key1_t*)key_cmp)->name, 
                    ((RegionKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Region_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
            status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
            status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Region_iter_last(Region* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getRegionKey0Type() ) {
        key_cmp = (region_key0_t*)do_malloc(sizeof(region_key0_t));
        memcpy(key_cmp, ((RegionKey0*)key)->priv, sizeof(region_key0_t));
        status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getRegionKey1Type() ) {
        key_cmp = (region_key1_t*)do_malloc(sizeof(region_key1_t));
        memcpy(key_cmp, ((RegionKey1*)key)->priv, sizeof(region_key1_t));
        status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key0_t*)key_cmp)->code, 
                    ((RegionKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((region_key1_t*)key_cmp)->name, 
                    ((RegionKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Region_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getRegionKey0Type() ) {
            status = do_region_get0(self->alias->alias, self->priv, ((RegionKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getRegionKey1Type() ) {
            status = do_region_get1(self->alias->alias, self->priv, ((RegionKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Region_insert(Region* self)
{
    int status;
    status = do_region_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_update(Region* self)
{
    int status;
    status = do_region_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_delete(Region* self)
{
    int status;
    status = do_region_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Region_set_alias(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Region*)self)->alias);
    Py_INCREF(obj);
    ((Region*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Region_set(Region* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRegionType() ) {
        memcpy(self->priv, ((Region*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRegionKey0Type() ) {

        do_cpy(self->priv->data.code, ((RegionKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getRegionKey1Type() ) {

        do_cpy(self->priv->data.name, ((RegionKey1*)obj)->priv->name);

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
                do_log(LOG_ERR, "Class \"Region\"does not contain \"%s\"", field_name);
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

static PyObject *Region_fields(Region* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Region");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Region_code(self,NULL);
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

    value =  Region_name(self,NULL);
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

    value =  Region_state(self,NULL);
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

    value = Region_get_params(self, NULL);
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

static PyGetSetDef Region_getset[] = {

    {"code",(getter)Region_code},

    {"name",(getter)Region_name},

    {"state",(getter)Region_state},

    {"params",(getter)Region_get_params},

    {NULL}
};

static PyMethodDef Region_methods[] = {
    {"set", (PyCFunction)Region_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Region_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Region_clone, METH_NOARGS, "clone"},

    {"set_code", (PyCFunction)Region_set_code, METH_VARARGS|METH_KEYWORDS, "Region_set_code"},

    {"set_name", (PyCFunction)Region_set_name, METH_VARARGS|METH_KEYWORDS, "Region_set_name"},

    {"set_state", (PyCFunction)Region_set_state, METH_VARARGS|METH_KEYWORDS, "Region_set_state"},

    {"set_param", (PyCFunction)Region_set_param, METH_VARARGS|METH_KEYWORDS, "do_Region_param_set"},
    {"param", (PyCFunction)Region_get_param, METH_VARARGS|METH_KEYWORDS, "do_Region_param"},
    {"clear_params", (PyCFunction)Region_params_clear, METH_NOARGS, "do_Region_param_clear"},
    {"set_params", (PyCFunction)Region_set_params, METH_VARARGS|METH_KEYWORDS, "do_Region_set_params"},

    {"get_equal", (PyCFunction)Region_equal, METH_VARARGS|METH_KEYWORDS, "Region_equal"},

    {"get_next", (PyCFunction)Region_next, METH_VARARGS|METH_KEYWORDS, "Region_next"},

    {"get_prev", (PyCFunction)Region_prev, METH_VARARGS|METH_KEYWORDS, "Region_prev"},

    {"get_gt", (PyCFunction)Region_gt, METH_VARARGS|METH_KEYWORDS, "Region_gt"},

    {"get_ge", (PyCFunction)Region_ge, METH_VARARGS|METH_KEYWORDS, "Region_ge"},

    {"get_lt", (PyCFunction)Region_lt, METH_VARARGS|METH_KEYWORDS, "Region_lt"},

    {"get_le", (PyCFunction)Region_le, METH_VARARGS|METH_KEYWORDS, "Region_le"},

    {"get_first", (PyCFunction)Region_first, METH_VARARGS|METH_KEYWORDS, "Region_first"},

    {"get_last", (PyCFunction)Region_last, METH_VARARGS|METH_KEYWORDS, "Region_last"},

    {"gets_equal", (PyCFunction)Region_iter_equal, METH_VARARGS|METH_KEYWORDS, "Region_iter_equal"},

    {"gets_gt", (PyCFunction)Region_iter_gt, METH_VARARGS|METH_KEYWORDS, "Region_iter_gt"},

    {"gets_ge", (PyCFunction)Region_iter_ge, METH_VARARGS|METH_KEYWORDS, "Region_iter_ge"},

    {"gets_lt", (PyCFunction)Region_iter_lt, METH_VARARGS|METH_KEYWORDS, "Region_iter_lt"},

    {"gets_le", (PyCFunction)Region_iter_le, METH_VARARGS|METH_KEYWORDS, "Region_iter_le"},

    {"gets_first", (PyCFunction)Region_iter_first, METH_VARARGS|METH_KEYWORDS, "Region_iter_first"},

    {"gets_last", (PyCFunction)Region_iter_last, METH_VARARGS|METH_KEYWORDS, "Region_iter_last"},

    {"insert", (PyCFunction)Region_insert, METH_VARARGS|METH_KEYWORDS, "Region_insert"},

    {"update", (PyCFunction)Region_update, METH_VARARGS|METH_KEYWORDS, "Region_update"},

    {"delete", (PyCFunction)Region_delete, METH_VARARGS|METH_KEYWORDS, "Region_delete"},

    {NULL}
};

static PyTypeObject RegionType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Region",             /*tp_name*/
    sizeof(Region),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Region_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Region_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Region objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Region_methods,             /* tp_methods */
    0,
    Region_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Region_init,      /* tp_init */
    0,                         /* tp_alloc */
    Region_new,                 /* tp_new */
};
PyTypeObject *getRegionType()
{
    return &RegionType_;
}

static void RegionKey0_dealloc(PyObject* self)
{
    do_free(((RegionKey0*)self)->priv);
    Py_DECREF(((RegionKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("RegionKey0 free\n");
}
static PyObject *RegionKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("RegionKey0 new\n");
    RegionKey0 *self;
    self = PyObject_NEW(RegionKey0, type);
    //self = (RegionKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(region_key0_t));
    return (PyObject *)self;
}
static int RegionKey0_init(RegionKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("RegionKey0 init\n");
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

static PyObject *RegionKey0_clone(RegionKey0 *self)
{
    RegionKey0 *result;
    result = (RegionKey0*)PyObject_CallFunction((PyObject*)getRegionKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *RegionKey0_code(RegionKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RegionKey0_set_code(RegionKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *RegionKey0_equal(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_next(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_prev(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_gt(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_ge(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_lt(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_le(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_first(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_last(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey0_iter_equal(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RegionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey0_iter_gt(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RegionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey0_iter_ge(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RegionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey0_iter_lt(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RegionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey0_iter_le(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RegionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey0_iter_first(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RegionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey0_iter_last(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = RegionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey0_cmp(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getRegionKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((RegionKey0*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *RegionKey0_set_alias(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((RegionKey0*)self)->alias);
    Py_INCREF(obj);
    ((RegionKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *RegionKey0_set(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRegionKey0Type() ) {
        memcpy(self->priv, ((RegionKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRegionType() ) {

        do_cpy(self->priv->code, ((Region*)obj)->priv->data.code);

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
                do_log(LOG_ERR, "Class \"RegionKey0\"does not contain \"%s\"", field_name);
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

static PyObject *RegionKey0_fields(RegionKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class RegionKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  RegionKey0_code(self,NULL);
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

static PyGetSetDef RegionKey0_getset[] = {

    {"code",(getter)RegionKey0_code},

    {NULL}
};

static PyMethodDef RegionKey0_methods[] = {
    {"set", (PyCFunction)RegionKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)RegionKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)RegionKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)RegionKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)RegionKey0_set_code, METH_VARARGS|METH_KEYWORDS, "RegionKey0_set_code"},

    {"get_equal", (PyCFunction)RegionKey0_equal, METH_VARARGS|METH_KEYWORDS, "RegionKey0_equal"},

    {"get_next", (PyCFunction)RegionKey0_next, METH_VARARGS|METH_KEYWORDS, "RegionKey0_next"},

    {"get_prev", (PyCFunction)RegionKey0_prev, METH_VARARGS|METH_KEYWORDS, "RegionKey0_prev"},

    {"get_gt", (PyCFunction)RegionKey0_gt, METH_VARARGS|METH_KEYWORDS, "RegionKey0_gt"},

    {"get_ge", (PyCFunction)RegionKey0_ge, METH_VARARGS|METH_KEYWORDS, "RegionKey0_ge"},

    {"get_lt", (PyCFunction)RegionKey0_lt, METH_VARARGS|METH_KEYWORDS, "RegionKey0_lt"},

    {"get_le", (PyCFunction)RegionKey0_le, METH_VARARGS|METH_KEYWORDS, "RegionKey0_le"},

    {"get_first", (PyCFunction)RegionKey0_first, METH_VARARGS|METH_KEYWORDS, "RegionKey0_first"},

    {"get_last", (PyCFunction)RegionKey0_last, METH_VARARGS|METH_KEYWORDS, "RegionKey0_last"},

    {"gets_equal", (PyCFunction)RegionKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "RegionKey0_iter_equal"},

    {"gets_gt", (PyCFunction)RegionKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "RegionKey0_iter_gt"},

    {"gets_ge", (PyCFunction)RegionKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "RegionKey0_iter_ge"},

    {"gets_lt", (PyCFunction)RegionKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "RegionKey0_iter_lt"},

    {"gets_le", (PyCFunction)RegionKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "RegionKey0_iter_le"},

    {"gets_first", (PyCFunction)RegionKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "RegionKey0_iter_first"},

    {"gets_last", (PyCFunction)RegionKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "RegionKey0_iter_last"},

    {NULL}
};

static PyTypeObject RegionKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.RegionKey0",             /*tp_name*/
    sizeof(RegionKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RegionKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)RegionKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RegionKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RegionKey0_methods,             /* tp_methods */
    0,
    RegionKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RegionKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    RegionKey0_new,                 /* tp_new */
};
PyTypeObject *getRegionKey0Type()
{
    return &RegionKey0Type_;
}

static void RegionKey1_dealloc(PyObject* self)
{
    do_free(((RegionKey1*)self)->priv);
    Py_DECREF(((RegionKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("RegionKey1 free\n");
}
static PyObject *RegionKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("RegionKey1 new\n");
    RegionKey1 *self;
    self = PyObject_NEW(RegionKey1, type);
    //self = (RegionKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(region_key1_t));
    return (PyObject *)self;
}
static int RegionKey1_init(RegionKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("RegionKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *RegionKey1_clone(RegionKey1 *self)
{
    RegionKey1 *result;
    result = (RegionKey1*)PyObject_CallFunction((PyObject*)getRegionKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *RegionKey1_name(RegionKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *RegionKey1_set_name(RegionKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set name");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->name, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *RegionKey1_equal(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_next(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_prev(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_gt(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_ge(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_lt(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_le(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_first(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_last(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_region_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *RegionKey1_iter_equal(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = RegionKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey1_iter_gt(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = RegionKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey1_iter_ge(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = RegionKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey1_iter_lt(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = RegionKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey1_iter_le(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = RegionKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey1_iter_first(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = RegionKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey1_iter_last(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    region_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_region_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = RegionKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_region_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *RegionKey1_cmp(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getRegionKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->name, ((RegionKey1*)obj)->priv->name);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *RegionKey1_set_alias(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((RegionKey1*)self)->alias);
    Py_INCREF(obj);
    ((RegionKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *RegionKey1_set(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getRegionKey1Type() ) {
        memcpy(self->priv, ((RegionKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getRegionType() ) {

        do_cpy(self->priv->name, ((Region*)obj)->priv->data.name);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
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
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"RegionKey1\"does not contain \"%s\"", field_name);
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

static PyObject *RegionKey1_fields(RegionKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class RegionKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  RegionKey1_name(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef RegionKey1_getset[] = {

    {"name",(getter)RegionKey1_name},

    {NULL}
};

static PyMethodDef RegionKey1_methods[] = {
    {"set", (PyCFunction)RegionKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)RegionKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)RegionKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)RegionKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_name", (PyCFunction)RegionKey1_set_name, METH_VARARGS|METH_KEYWORDS, "RegionKey1_set_name"},

    {"get_equal", (PyCFunction)RegionKey1_equal, METH_VARARGS|METH_KEYWORDS, "RegionKey1_equal"},

    {"get_next", (PyCFunction)RegionKey1_next, METH_VARARGS|METH_KEYWORDS, "RegionKey1_next"},

    {"get_prev", (PyCFunction)RegionKey1_prev, METH_VARARGS|METH_KEYWORDS, "RegionKey1_prev"},

    {"get_gt", (PyCFunction)RegionKey1_gt, METH_VARARGS|METH_KEYWORDS, "RegionKey1_gt"},

    {"get_ge", (PyCFunction)RegionKey1_ge, METH_VARARGS|METH_KEYWORDS, "RegionKey1_ge"},

    {"get_lt", (PyCFunction)RegionKey1_lt, METH_VARARGS|METH_KEYWORDS, "RegionKey1_lt"},

    {"get_le", (PyCFunction)RegionKey1_le, METH_VARARGS|METH_KEYWORDS, "RegionKey1_le"},

    {"get_first", (PyCFunction)RegionKey1_first, METH_VARARGS|METH_KEYWORDS, "RegionKey1_first"},

    {"get_last", (PyCFunction)RegionKey1_last, METH_VARARGS|METH_KEYWORDS, "RegionKey1_last"},

    {"gets_equal", (PyCFunction)RegionKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "RegionKey1_iter_equal"},

    {"gets_gt", (PyCFunction)RegionKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "RegionKey1_iter_gt"},

    {"gets_ge", (PyCFunction)RegionKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "RegionKey1_iter_ge"},

    {"gets_lt", (PyCFunction)RegionKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "RegionKey1_iter_lt"},

    {"gets_le", (PyCFunction)RegionKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "RegionKey1_iter_le"},

    {"gets_first", (PyCFunction)RegionKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "RegionKey1_iter_first"},

    {"gets_last", (PyCFunction)RegionKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "RegionKey1_iter_last"},

    {NULL}
};

static PyTypeObject RegionKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.RegionKey1",             /*tp_name*/
    sizeof(RegionKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)RegionKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)RegionKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "RegionKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    RegionKey1_methods,             /* tp_methods */
    0,
    RegionKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)RegionKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    RegionKey1_new,                 /* tp_new */
};
PyTypeObject *getRegionKey1Type()
{
    return &RegionKey1Type_;
}
