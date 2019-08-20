
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "otdel.h"

static void Division_dealloc(PyObject* self)
{
    do_free(((Division*)self)->priv);
    Py_DECREF(((Division*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Division free\n");
}
static PyObject *Division_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Division new\n");
    Division *self;
    self = PyObject_NEW(Division, type);
    //self = (Division *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(otdel_rec_t));
    return (PyObject *)self;
}
static int Division_init(Division *self, PyObject *args, PyObject *kwds)
{
    //LOG("Division init\n");
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

    do_otdel_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Division_clone(Division *self)
{
    Division *result;
    result = (Division*)PyObject_CallFunction((PyObject*)getDivisionType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Division_code(Division* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Division_set_code(Division* self, PyObject *args, PyObject *kwds)
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

static PyObject *Division_name(Division* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Division_set_name(Division* self, PyObject *args, PyObject *kwds)
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

static PyObject *Division_set_param(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_otdel_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_otdel_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_otdel_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Division_set_params(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_otdel_params_clear(self->alias->alias,self->priv);
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
                 do_otdel_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_otdel_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_otdel_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Division_get_param(Division* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_otdel_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Division_params_clear(Division* self)
{
    do_otdel_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Division_get_params(Division* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *Division_gt(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_last(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_next(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_le(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_lt(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_equal(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_ge(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_prev(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_first(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDivisionKey0Type() )
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_iter_gt(Division* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDivisionKey0Type() ) {
        key_cmp = (otdel_key0_t*)do_malloc(sizeof(otdel_key0_t));
        memcpy(key_cmp, ((DivisionKey0*)key)->priv, sizeof(otdel_key0_t));
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((otdel_key0_t*)key_cmp)->code, 
                    ((DivisionKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Division_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
            status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Division_iter_last(Division* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDivisionKey0Type() ) {
        key_cmp = (otdel_key0_t*)do_malloc(sizeof(otdel_key0_t));
        memcpy(key_cmp, ((DivisionKey0*)key)->priv, sizeof(otdel_key0_t));
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((otdel_key0_t*)key_cmp)->code, 
                    ((DivisionKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Division_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
            status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Division_iter_le(Division* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDivisionKey0Type() ) {
        key_cmp = (otdel_key0_t*)do_malloc(sizeof(otdel_key0_t));
        memcpy(key_cmp, ((DivisionKey0*)key)->priv, sizeof(otdel_key0_t));
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((otdel_key0_t*)key_cmp)->code, 
                    ((DivisionKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Division_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
            status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Division_iter_ge(Division* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDivisionKey0Type() ) {
        key_cmp = (otdel_key0_t*)do_malloc(sizeof(otdel_key0_t));
        memcpy(key_cmp, ((DivisionKey0*)key)->priv, sizeof(otdel_key0_t));
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((otdel_key0_t*)key_cmp)->code, 
                    ((DivisionKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Division_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
            status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Division_iter_equal(Division* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDivisionKey0Type() ) {
        key_cmp = (otdel_key0_t*)do_malloc(sizeof(otdel_key0_t));
        memcpy(key_cmp, ((DivisionKey0*)key)->priv, sizeof(otdel_key0_t));
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((otdel_key0_t*)key_cmp)->code, 
                    ((DivisionKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Division_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
            status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Division_iter_lt(Division* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDivisionKey0Type() ) {
        key_cmp = (otdel_key0_t*)do_malloc(sizeof(otdel_key0_t));
        memcpy(key_cmp, ((DivisionKey0*)key)->priv, sizeof(otdel_key0_t));
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((otdel_key0_t*)key_cmp)->code, 
                    ((DivisionKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Division_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
            status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Division_iter_first(Division* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDivisionKey0Type() ) {
        key_cmp = (otdel_key0_t*)do_malloc(sizeof(otdel_key0_t));
        memcpy(key_cmp, ((DivisionKey0*)key)->priv, sizeof(otdel_key0_t));
        status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((otdel_key0_t*)key_cmp)->code, 
                    ((DivisionKey0*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Division_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDivisionKey0Type() ) {
            status = do_otdel_get0(self->alias->alias, self->priv, ((DivisionKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Division_insert(Division* self)
{
    int status;
    status = do_otdel_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_update(Division* self)
{
    int status;
    status = do_otdel_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_delete(Division* self)
{
    int status;
    status = do_otdel_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Division_set_alias(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Division*)self)->alias);
    Py_INCREF(obj);
    ((Division*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Division_set(Division* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDivisionType() ) {
        memcpy(self->priv, ((Division*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDivisionKey0Type() ) {

        do_cpy(self->priv->data.code, ((DivisionKey0*)obj)->priv->code);

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
                do_log(LOG_ERR, "Class \"Division\"does not contain \"%s\"", field_name);
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

static PyObject *Division_fields(Division* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Division");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Division_code(self,NULL);
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

    value =  Division_name(self,NULL);
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

    value = Division_get_params(self, NULL);
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

static PyGetSetDef Division_getset[] = {

    {"code",(getter)Division_code},

    {"name",(getter)Division_name},

    {"params",(getter)Division_get_params},

    {NULL}
};

static PyMethodDef Division_methods[] = {
    {"set", (PyCFunction)Division_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Division_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Division_clone, METH_NOARGS, "clone"},

    {"set_code", (PyCFunction)Division_set_code, METH_VARARGS|METH_KEYWORDS, "Division_set_code"},

    {"set_name", (PyCFunction)Division_set_name, METH_VARARGS|METH_KEYWORDS, "Division_set_name"},

    {"set_param", (PyCFunction)Division_set_param, METH_VARARGS|METH_KEYWORDS, "do_Division_param_set"},
    {"param", (PyCFunction)Division_get_param, METH_VARARGS|METH_KEYWORDS, "do_Division_param"},
    {"clear_params", (PyCFunction)Division_params_clear, METH_NOARGS, "do_Division_param_clear"},
    {"set_params", (PyCFunction)Division_set_params, METH_VARARGS|METH_KEYWORDS, "do_Division_set_params"},

    {"get_gt", (PyCFunction)Division_gt, METH_VARARGS|METH_KEYWORDS, "Division_gt"},

    {"get_last", (PyCFunction)Division_last, METH_VARARGS|METH_KEYWORDS, "Division_last"},

    {"get_next", (PyCFunction)Division_next, METH_VARARGS|METH_KEYWORDS, "Division_next"},

    {"get_le", (PyCFunction)Division_le, METH_VARARGS|METH_KEYWORDS, "Division_le"},

    {"get_lt", (PyCFunction)Division_lt, METH_VARARGS|METH_KEYWORDS, "Division_lt"},

    {"get_equal", (PyCFunction)Division_equal, METH_VARARGS|METH_KEYWORDS, "Division_equal"},

    {"get_ge", (PyCFunction)Division_ge, METH_VARARGS|METH_KEYWORDS, "Division_ge"},

    {"get_prev", (PyCFunction)Division_prev, METH_VARARGS|METH_KEYWORDS, "Division_prev"},

    {"get_first", (PyCFunction)Division_first, METH_VARARGS|METH_KEYWORDS, "Division_first"},

    {"gets_gt", (PyCFunction)Division_iter_gt, METH_VARARGS|METH_KEYWORDS, "Division_iter_gt"},

    {"gets_last", (PyCFunction)Division_iter_last, METH_VARARGS|METH_KEYWORDS, "Division_iter_last"},

    {"gets_le", (PyCFunction)Division_iter_le, METH_VARARGS|METH_KEYWORDS, "Division_iter_le"},

    {"gets_ge", (PyCFunction)Division_iter_ge, METH_VARARGS|METH_KEYWORDS, "Division_iter_ge"},

    {"gets_equal", (PyCFunction)Division_iter_equal, METH_VARARGS|METH_KEYWORDS, "Division_iter_equal"},

    {"gets_lt", (PyCFunction)Division_iter_lt, METH_VARARGS|METH_KEYWORDS, "Division_iter_lt"},

    {"gets_first", (PyCFunction)Division_iter_first, METH_VARARGS|METH_KEYWORDS, "Division_iter_first"},

    {"insert", (PyCFunction)Division_insert, METH_VARARGS|METH_KEYWORDS, "Division_insert"},

    {"update", (PyCFunction)Division_update, METH_VARARGS|METH_KEYWORDS, "Division_update"},

    {"delete", (PyCFunction)Division_delete, METH_VARARGS|METH_KEYWORDS, "Division_delete"},

    {NULL}
};

static PyTypeObject DivisionType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Division",             /*tp_name*/
    sizeof(Division),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Division_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Division_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Division objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Division_methods,             /* tp_methods */
    0,
    Division_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Division_init,      /* tp_init */
    0,                         /* tp_alloc */
    Division_new,                 /* tp_new */
};
PyTypeObject *getDivisionType()
{
    return &DivisionType_;
}

static void DivisionKey0_dealloc(PyObject* self)
{
    do_free(((DivisionKey0*)self)->priv);
    Py_DECREF(((DivisionKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DivisionKey0 free\n");
}
static PyObject *DivisionKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DivisionKey0 new\n");
    DivisionKey0 *self;
    self = PyObject_NEW(DivisionKey0, type);
    //self = (DivisionKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(otdel_key0_t));
    return (PyObject *)self;
}
static int DivisionKey0_init(DivisionKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DivisionKey0 init\n");
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

static PyObject *DivisionKey0_clone(DivisionKey0 *self)
{
    DivisionKey0 *result;
    result = (DivisionKey0*)PyObject_CallFunction((PyObject*)getDivisionKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DivisionKey0_code(DivisionKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DivisionKey0_set_code(DivisionKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DivisionKey0_gt(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_last(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_next(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_le(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_lt(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_equal(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_ge(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_prev(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_first(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DivisionKey0_iter_gt(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    otdel_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DivisionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DivisionKey0_iter_last(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    otdel_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DivisionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DivisionKey0_iter_le(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    otdel_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DivisionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DivisionKey0_iter_ge(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    otdel_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DivisionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DivisionKey0_iter_equal(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    otdel_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DivisionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DivisionKey0_iter_lt(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    otdel_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DivisionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DivisionKey0_iter_first(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    otdel_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = DivisionKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_otdel_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DivisionKey0_cmp(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDivisionKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((DivisionKey0*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DivisionKey0_set_alias(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DivisionKey0*)self)->alias);
    Py_INCREF(obj);
    ((DivisionKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DivisionKey0_set(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDivisionKey0Type() ) {
        memcpy(self->priv, ((DivisionKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDivisionType() ) {

        do_cpy(self->priv->code, ((Division*)obj)->priv->data.code);

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
                do_log(LOG_ERR, "Class \"DivisionKey0\"does not contain \"%s\"", field_name);
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

static PyObject *DivisionKey0_fields(DivisionKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DivisionKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DivisionKey0_code(self,NULL);
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

static PyGetSetDef DivisionKey0_getset[] = {

    {"code",(getter)DivisionKey0_code},

    {NULL}
};

static PyMethodDef DivisionKey0_methods[] = {
    {"set", (PyCFunction)DivisionKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DivisionKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DivisionKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DivisionKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)DivisionKey0_set_code, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_set_code"},

    {"get_gt", (PyCFunction)DivisionKey0_gt, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_gt"},

    {"get_last", (PyCFunction)DivisionKey0_last, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_last"},

    {"get_next", (PyCFunction)DivisionKey0_next, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_next"},

    {"get_le", (PyCFunction)DivisionKey0_le, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_le"},

    {"get_lt", (PyCFunction)DivisionKey0_lt, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_lt"},

    {"get_equal", (PyCFunction)DivisionKey0_equal, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_equal"},

    {"get_ge", (PyCFunction)DivisionKey0_ge, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_ge"},

    {"get_prev", (PyCFunction)DivisionKey0_prev, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_prev"},

    {"get_first", (PyCFunction)DivisionKey0_first, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_first"},

    {"gets_gt", (PyCFunction)DivisionKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_iter_gt"},

    {"gets_last", (PyCFunction)DivisionKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_iter_last"},

    {"gets_le", (PyCFunction)DivisionKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_iter_le"},

    {"gets_ge", (PyCFunction)DivisionKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_iter_ge"},

    {"gets_equal", (PyCFunction)DivisionKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_iter_equal"},

    {"gets_lt", (PyCFunction)DivisionKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_iter_lt"},

    {"gets_first", (PyCFunction)DivisionKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "DivisionKey0_iter_first"},

    {NULL}
};

static PyTypeObject DivisionKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DivisionKey0",             /*tp_name*/
    sizeof(DivisionKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DivisionKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DivisionKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DivisionKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DivisionKey0_methods,             /* tp_methods */
    0,
    DivisionKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DivisionKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DivisionKey0_new,                 /* tp_new */
};
PyTypeObject *getDivisionKey0Type()
{
    return &DivisionKey0Type_;
}
