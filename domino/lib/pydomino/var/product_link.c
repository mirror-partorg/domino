
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "product_link.h"

static void ProductLink_dealloc(PyObject* self)
{
    do_free(((ProductLink*)self)->priv);
    Py_DECREF(((ProductLink*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductLink free\n");
}
static PyObject *ProductLink_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductLink new\n");
    ProductLink *self;
    self = PyObject_NEW(ProductLink, type);
    //self = (ProductLink *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_link_rec_t));
    return (PyObject *)self;
}
static int ProductLink_init(ProductLink *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductLink init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.m_code);

    do_text_set_empty(self->priv->data.s_code);

    do_product_link_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductLink_clone(ProductLink *self)
{
    ProductLink *result;
    result = (ProductLink*)PyObject_CallFunction((PyObject*)getProductLinkType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *ProductLink_record_type(ProductLink* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type);

    return result;
}

static PyObject *ProductLink_set_record_type(ProductLink* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductLink_master_code(ProductLink* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.m_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductLink_set_master_code(ProductLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.m_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.m_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.m_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductLink_slave_code(ProductLink* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.s_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductLink_set_slave_code(ProductLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.s_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.s_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.s_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductLink_number(ProductLink* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.number);

    return result;
}

static PyObject *ProductLink_set_number(ProductLink* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductLink_set_param(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_product_link_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_product_link_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_product_link_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *ProductLink_set_params(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_product_link_params_clear(self->alias->alias,self->priv);
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
                 do_product_link_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_product_link_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_product_link_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *ProductLink_get_param(ProductLink* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_product_link_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *ProductLink_params_clear(ProductLink* self)
{
    do_product_link_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *ProductLink_get_params(ProductLink* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *ProductLink_equal(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_next(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_prev(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_gt(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_ge(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_lt(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_le(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_first(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_last(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductLinkKey0Type() )
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() )
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_iter_equal(ProductLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
        key_cmp = (product_link_key0_t*)do_malloc(sizeof(product_link_key0_t));
        memcpy(key_cmp, ((ProductLinkKey0*)key)->priv, sizeof(product_link_key0_t));
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
        key_cmp = (product_link_key1_t*)do_malloc(sizeof(product_link_key1_t));
        memcpy(key_cmp, ((ProductLinkKey1*)key)->priv, sizeof(product_link_key1_t));
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->m_code, 
                    ((ProductLinkKey0*)key)->priv->m_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->type, 
                    ((ProductLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->number, 
                    ((ProductLinkKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->s_code, 
                    ((ProductLinkKey1*)key)->priv->s_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->type, 
                    ((ProductLinkKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
            status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
            status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductLink_iter_gt(ProductLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
        key_cmp = (product_link_key0_t*)do_malloc(sizeof(product_link_key0_t));
        memcpy(key_cmp, ((ProductLinkKey0*)key)->priv, sizeof(product_link_key0_t));
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
        key_cmp = (product_link_key1_t*)do_malloc(sizeof(product_link_key1_t));
        memcpy(key_cmp, ((ProductLinkKey1*)key)->priv, sizeof(product_link_key1_t));
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->m_code, 
                    ((ProductLinkKey0*)key)->priv->m_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->type, 
                    ((ProductLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->number, 
                    ((ProductLinkKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->s_code, 
                    ((ProductLinkKey1*)key)->priv->s_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->type, 
                    ((ProductLinkKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
            status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
            status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductLink_iter_ge(ProductLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
        key_cmp = (product_link_key0_t*)do_malloc(sizeof(product_link_key0_t));
        memcpy(key_cmp, ((ProductLinkKey0*)key)->priv, sizeof(product_link_key0_t));
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
        key_cmp = (product_link_key1_t*)do_malloc(sizeof(product_link_key1_t));
        memcpy(key_cmp, ((ProductLinkKey1*)key)->priv, sizeof(product_link_key1_t));
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->m_code, 
                    ((ProductLinkKey0*)key)->priv->m_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->type, 
                    ((ProductLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->number, 
                    ((ProductLinkKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->s_code, 
                    ((ProductLinkKey1*)key)->priv->s_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->type, 
                    ((ProductLinkKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
            status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
            status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductLink_iter_lt(ProductLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
        key_cmp = (product_link_key0_t*)do_malloc(sizeof(product_link_key0_t));
        memcpy(key_cmp, ((ProductLinkKey0*)key)->priv, sizeof(product_link_key0_t));
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
        key_cmp = (product_link_key1_t*)do_malloc(sizeof(product_link_key1_t));
        memcpy(key_cmp, ((ProductLinkKey1*)key)->priv, sizeof(product_link_key1_t));
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->m_code, 
                    ((ProductLinkKey0*)key)->priv->m_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->type, 
                    ((ProductLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->number, 
                    ((ProductLinkKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->s_code, 
                    ((ProductLinkKey1*)key)->priv->s_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->type, 
                    ((ProductLinkKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
            status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
            status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductLink_iter_le(ProductLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
        key_cmp = (product_link_key0_t*)do_malloc(sizeof(product_link_key0_t));
        memcpy(key_cmp, ((ProductLinkKey0*)key)->priv, sizeof(product_link_key0_t));
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
        key_cmp = (product_link_key1_t*)do_malloc(sizeof(product_link_key1_t));
        memcpy(key_cmp, ((ProductLinkKey1*)key)->priv, sizeof(product_link_key1_t));
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->m_code, 
                    ((ProductLinkKey0*)key)->priv->m_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->type, 
                    ((ProductLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->number, 
                    ((ProductLinkKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->s_code, 
                    ((ProductLinkKey1*)key)->priv->s_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->type, 
                    ((ProductLinkKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
            status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
            status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductLink_iter_first(ProductLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
        key_cmp = (product_link_key0_t*)do_malloc(sizeof(product_link_key0_t));
        memcpy(key_cmp, ((ProductLinkKey0*)key)->priv, sizeof(product_link_key0_t));
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
        key_cmp = (product_link_key1_t*)do_malloc(sizeof(product_link_key1_t));
        memcpy(key_cmp, ((ProductLinkKey1*)key)->priv, sizeof(product_link_key1_t));
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->m_code, 
                    ((ProductLinkKey0*)key)->priv->m_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->type, 
                    ((ProductLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->number, 
                    ((ProductLinkKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->s_code, 
                    ((ProductLinkKey1*)key)->priv->s_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->type, 
                    ((ProductLinkKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
            status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
            status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductLink_iter_last(ProductLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
        key_cmp = (product_link_key0_t*)do_malloc(sizeof(product_link_key0_t));
        memcpy(key_cmp, ((ProductLinkKey0*)key)->priv, sizeof(product_link_key0_t));
        status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
        key_cmp = (product_link_key1_t*)do_malloc(sizeof(product_link_key1_t));
        memcpy(key_cmp, ((ProductLinkKey1*)key)->priv, sizeof(product_link_key1_t));
        status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->m_code, 
                    ((ProductLinkKey0*)key)->priv->m_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->type, 
                    ((ProductLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_link_key0_t*)key_cmp)->number, 
                    ((ProductLinkKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->s_code, 
                    ((ProductLinkKey1*)key)->priv->s_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_link_key1_t*)key_cmp)->type, 
                    ((ProductLinkKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductLinkKey0Type() ) {
            status = do_product_link_get0(self->alias->alias, self->priv, ((ProductLinkKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductLinkKey1Type() ) {
            status = do_product_link_get1(self->alias->alias, self->priv, ((ProductLinkKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductLink_insert(ProductLink* self)
{
    int status;
    status = do_product_link_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_update(ProductLink* self)
{
    int status;
    status = do_product_link_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_delete(ProductLink* self)
{
    int status;
    status = do_product_link_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLink_set_alias(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductLink*)self)->alias);
    Py_INCREF(obj);
    ((ProductLink*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductLink_set(ProductLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductLinkType() ) {
        memcpy(self->priv, ((ProductLink*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductLinkKey0Type() ) {

        do_cpy(self->priv->data.type, ((ProductLinkKey0*)obj)->priv->type);

        do_cpy(self->priv->data.m_code, ((ProductLinkKey0*)obj)->priv->m_code);

        do_cpy(self->priv->data.number, ((ProductLinkKey0*)obj)->priv->number);

    }

    else 
    if ( Py_TYPE(obj) == getProductLinkKey1Type() ) {

        do_cpy(self->priv->data.type, ((ProductLinkKey1*)obj)->priv->type);

        do_cpy(self->priv->data.s_code, ((ProductLinkKey1*)obj)->priv->s_code);

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
            if ( !strcmp("master_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_master_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("slave_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_slave_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("number", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_number", "O", value) )
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
                do_log(LOG_ERR, "Class \"ProductLink\"does not contain \"%s\"", field_name);
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

static PyObject *ProductLink_fields(ProductLink* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductLink");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductLink_record_type(self,NULL);
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

    value =  ProductLink_master_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("master_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductLink_slave_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("slave_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductLink_number(self,NULL);
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

    value = ProductLink_get_params(self, NULL);
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

static PyGetSetDef ProductLink_getset[] = {

    {"record_type",(getter)ProductLink_record_type},

    {"master_code",(getter)ProductLink_master_code},

    {"slave_code",(getter)ProductLink_slave_code},

    {"number",(getter)ProductLink_number},

    {"params",(getter)ProductLink_get_params},

    {NULL}
};

static PyMethodDef ProductLink_methods[] = {
    {"set", (PyCFunction)ProductLink_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductLink_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductLink_clone, METH_NOARGS, "clone"},

    {"set_record_type", (PyCFunction)ProductLink_set_record_type, METH_VARARGS|METH_KEYWORDS, "ProductLink_set_record_type"},

    {"set_master_code", (PyCFunction)ProductLink_set_master_code, METH_VARARGS|METH_KEYWORDS, "ProductLink_set_master_code"},

    {"set_slave_code", (PyCFunction)ProductLink_set_slave_code, METH_VARARGS|METH_KEYWORDS, "ProductLink_set_slave_code"},

    {"set_number", (PyCFunction)ProductLink_set_number, METH_VARARGS|METH_KEYWORDS, "ProductLink_set_number"},

    {"set_param", (PyCFunction)ProductLink_set_param, METH_VARARGS|METH_KEYWORDS, "do_ProductLink_param_set"},
    {"param", (PyCFunction)ProductLink_get_param, METH_VARARGS|METH_KEYWORDS, "do_ProductLink_param"},
    {"clear_params", (PyCFunction)ProductLink_params_clear, METH_NOARGS, "do_ProductLink_param_clear"},
    {"set_params", (PyCFunction)ProductLink_set_params, METH_VARARGS|METH_KEYWORDS, "do_ProductLink_set_params"},

    {"get_equal", (PyCFunction)ProductLink_equal, METH_VARARGS|METH_KEYWORDS, "ProductLink_equal"},

    {"get_next", (PyCFunction)ProductLink_next, METH_VARARGS|METH_KEYWORDS, "ProductLink_next"},

    {"get_prev", (PyCFunction)ProductLink_prev, METH_VARARGS|METH_KEYWORDS, "ProductLink_prev"},

    {"get_gt", (PyCFunction)ProductLink_gt, METH_VARARGS|METH_KEYWORDS, "ProductLink_gt"},

    {"get_ge", (PyCFunction)ProductLink_ge, METH_VARARGS|METH_KEYWORDS, "ProductLink_ge"},

    {"get_lt", (PyCFunction)ProductLink_lt, METH_VARARGS|METH_KEYWORDS, "ProductLink_lt"},

    {"get_le", (PyCFunction)ProductLink_le, METH_VARARGS|METH_KEYWORDS, "ProductLink_le"},

    {"get_first", (PyCFunction)ProductLink_first, METH_VARARGS|METH_KEYWORDS, "ProductLink_first"},

    {"get_last", (PyCFunction)ProductLink_last, METH_VARARGS|METH_KEYWORDS, "ProductLink_last"},

    {"gets_equal", (PyCFunction)ProductLink_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductLink_iter_equal"},

    {"gets_gt", (PyCFunction)ProductLink_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductLink_iter_gt"},

    {"gets_ge", (PyCFunction)ProductLink_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductLink_iter_ge"},

    {"gets_lt", (PyCFunction)ProductLink_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductLink_iter_lt"},

    {"gets_le", (PyCFunction)ProductLink_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductLink_iter_le"},

    {"gets_first", (PyCFunction)ProductLink_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductLink_iter_first"},

    {"gets_last", (PyCFunction)ProductLink_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductLink_iter_last"},

    {"insert", (PyCFunction)ProductLink_insert, METH_VARARGS|METH_KEYWORDS, "ProductLink_insert"},

    {"update", (PyCFunction)ProductLink_update, METH_VARARGS|METH_KEYWORDS, "ProductLink_update"},

    {"delete", (PyCFunction)ProductLink_delete, METH_VARARGS|METH_KEYWORDS, "ProductLink_delete"},

    {NULL}
};

static PyTypeObject ProductLinkType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductLink",             /*tp_name*/
    sizeof(ProductLink),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductLink_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductLink_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductLink objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductLink_methods,             /* tp_methods */
    0,
    ProductLink_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductLink_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductLink_new,                 /* tp_new */
};
PyTypeObject *getProductLinkType()
{
    return &ProductLinkType_;
}

static void ProductLinkKey0_dealloc(PyObject* self)
{
    do_free(((ProductLinkKey0*)self)->priv);
    Py_DECREF(((ProductLinkKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductLinkKey0 free\n");
}
static PyObject *ProductLinkKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductLinkKey0 new\n");
    ProductLinkKey0 *self;
    self = PyObject_NEW(ProductLinkKey0, type);
    //self = (ProductLinkKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_link_key0_t));
    return (PyObject *)self;
}
static int ProductLinkKey0_init(ProductLinkKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductLinkKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->m_code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductLinkKey0_clone(ProductLinkKey0 *self)
{
    ProductLinkKey0 *result;
    result = (ProductLinkKey0*)PyObject_CallFunction((PyObject*)getProductLinkKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductLinkKey0_master_code(ProductLinkKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->m_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductLinkKey0_set_master_code(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set m_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->m_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->m_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductLinkKey0_record_type(ProductLinkKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *ProductLinkKey0_set_record_type(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductLinkKey0_number(ProductLinkKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->number);

    return result;
}

static PyObject *ProductLinkKey0_set_number(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductLinkKey0_equal(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_next(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_prev(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_gt(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_ge(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_lt(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_le(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_first(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_last(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey0_iter_equal(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_code, 
                 self->priv->m_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = ProductLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey0_iter_gt(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_code, 
                 self->priv->m_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = ProductLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey0_iter_ge(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_code, 
                 self->priv->m_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = ProductLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey0_iter_lt(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_code, 
                 self->priv->m_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = ProductLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey0_iter_le(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_code, 
                 self->priv->m_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = ProductLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey0_iter_first(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_code, 
                 self->priv->m_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = ProductLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey0_iter_last(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_code, 
                 self->priv->m_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = ProductLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey0_cmp(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductLinkKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->m_code, ((ProductLinkKey0*)obj)->priv->m_code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->type - ((ProductLinkKey0*)obj)->priv->type;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->number - ((ProductLinkKey0*)obj)->priv->number;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductLinkKey0_set_alias(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductLinkKey0*)self)->alias);
    Py_INCREF(obj);
    ((ProductLinkKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductLinkKey0_set(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductLinkKey0Type() ) {
        memcpy(self->priv, ((ProductLinkKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductLinkType() ) {

        do_cpy(self->priv->m_code, ((ProductLink*)obj)->priv->data.m_code);

        do_cpy(self->priv->type, ((ProductLink*)obj)->priv->data.type);

        do_cpy(self->priv->number, ((ProductLink*)obj)->priv->data.number);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_master_code", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                        return NULL;
                    break;

                case 2:
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
            if ( !strcmp("master_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_master_code", "O", value) )
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
                do_log(LOG_ERR, "Class \"ProductLinkKey0\"does not contain \"%s\"", field_name);
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

static PyObject *ProductLinkKey0_fields(ProductLinkKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductLinkKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductLinkKey0_master_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("master_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductLinkKey0_record_type(self,NULL);
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

    value =  ProductLinkKey0_number(self,NULL);
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

static PyGetSetDef ProductLinkKey0_getset[] = {

    {"master_code",(getter)ProductLinkKey0_master_code},

    {"record_type",(getter)ProductLinkKey0_record_type},

    {"number",(getter)ProductLinkKey0_number},

    {NULL}
};

static PyMethodDef ProductLinkKey0_methods[] = {
    {"set", (PyCFunction)ProductLinkKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductLinkKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductLinkKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductLinkKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_master_code", (PyCFunction)ProductLinkKey0_set_master_code, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_set_master_code"},

    {"set_record_type", (PyCFunction)ProductLinkKey0_set_record_type, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_set_record_type"},

    {"set_number", (PyCFunction)ProductLinkKey0_set_number, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_set_number"},

    {"get_equal", (PyCFunction)ProductLinkKey0_equal, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_equal"},

    {"get_next", (PyCFunction)ProductLinkKey0_next, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_next"},

    {"get_prev", (PyCFunction)ProductLinkKey0_prev, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_prev"},

    {"get_gt", (PyCFunction)ProductLinkKey0_gt, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_gt"},

    {"get_ge", (PyCFunction)ProductLinkKey0_ge, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_ge"},

    {"get_lt", (PyCFunction)ProductLinkKey0_lt, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_lt"},

    {"get_le", (PyCFunction)ProductLinkKey0_le, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_le"},

    {"get_first", (PyCFunction)ProductLinkKey0_first, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_first"},

    {"get_last", (PyCFunction)ProductLinkKey0_last, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_last"},

    {"gets_equal", (PyCFunction)ProductLinkKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_iter_equal"},

    {"gets_gt", (PyCFunction)ProductLinkKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_iter_gt"},

    {"gets_ge", (PyCFunction)ProductLinkKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_iter_ge"},

    {"gets_lt", (PyCFunction)ProductLinkKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_iter_lt"},

    {"gets_le", (PyCFunction)ProductLinkKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_iter_le"},

    {"gets_first", (PyCFunction)ProductLinkKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_iter_first"},

    {"gets_last", (PyCFunction)ProductLinkKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey0_iter_last"},

    {NULL}
};

static PyTypeObject ProductLinkKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductLinkKey0",             /*tp_name*/
    sizeof(ProductLinkKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductLinkKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductLinkKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductLinkKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductLinkKey0_methods,             /* tp_methods */
    0,
    ProductLinkKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductLinkKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductLinkKey0_new,                 /* tp_new */
};
PyTypeObject *getProductLinkKey0Type()
{
    return &ProductLinkKey0Type_;
}

static void ProductLinkKey1_dealloc(PyObject* self)
{
    do_free(((ProductLinkKey1*)self)->priv);
    Py_DECREF(((ProductLinkKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductLinkKey1 free\n");
}
static PyObject *ProductLinkKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductLinkKey1 new\n");
    ProductLinkKey1 *self;
    self = PyObject_NEW(ProductLinkKey1, type);
    //self = (ProductLinkKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_link_key1_t));
    return (PyObject *)self;
}
static int ProductLinkKey1_init(ProductLinkKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductLinkKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->s_code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductLinkKey1_clone(ProductLinkKey1 *self)
{
    ProductLinkKey1 *result;
    result = (ProductLinkKey1*)PyObject_CallFunction((PyObject*)getProductLinkKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductLinkKey1_slave_code(ProductLinkKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->s_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductLinkKey1_set_slave_code(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set s_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->s_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->s_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductLinkKey1_record_type(ProductLinkKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *ProductLinkKey1_set_record_type(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductLinkKey1_equal(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_next(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_prev(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_gt(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_ge(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_lt(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_le(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_first(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_last(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductLinkKey1_iter_equal(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_code, 
                 self->priv->s_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

 
        item = ProductLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey1_iter_gt(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_code, 
                 self->priv->s_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

 
        item = ProductLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey1_iter_ge(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_code, 
                 self->priv->s_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

 
        item = ProductLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey1_iter_lt(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_code, 
                 self->priv->s_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

 
        item = ProductLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey1_iter_le(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_code, 
                 self->priv->s_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

 
        item = ProductLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey1_iter_first(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_code, 
                 self->priv->s_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

 
        item = ProductLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey1_iter_last(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_code, 
                 self->priv->s_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

 
        item = ProductLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_link_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductLinkKey1_cmp(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductLinkKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->s_code, ((ProductLinkKey1*)obj)->priv->s_code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->type - ((ProductLinkKey1*)obj)->priv->type;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductLinkKey1_set_alias(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductLinkKey1*)self)->alias);
    Py_INCREF(obj);
    ((ProductLinkKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductLinkKey1_set(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductLinkKey1Type() ) {
        memcpy(self->priv, ((ProductLinkKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductLinkType() ) {

        do_cpy(self->priv->s_code, ((ProductLink*)obj)->priv->data.s_code);

        do_cpy(self->priv->type, ((ProductLink*)obj)->priv->data.type);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_slave_code", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
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
            if ( !strcmp("slave_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_slave_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProductLinkKey1\"does not contain \"%s\"", field_name);
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

static PyObject *ProductLinkKey1_fields(ProductLinkKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductLinkKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductLinkKey1_slave_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("slave_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductLinkKey1_record_type(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef ProductLinkKey1_getset[] = {

    {"slave_code",(getter)ProductLinkKey1_slave_code},

    {"record_type",(getter)ProductLinkKey1_record_type},

    {NULL}
};

static PyMethodDef ProductLinkKey1_methods[] = {
    {"set", (PyCFunction)ProductLinkKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductLinkKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductLinkKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductLinkKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_slave_code", (PyCFunction)ProductLinkKey1_set_slave_code, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_set_slave_code"},

    {"set_record_type", (PyCFunction)ProductLinkKey1_set_record_type, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_set_record_type"},

    {"get_equal", (PyCFunction)ProductLinkKey1_equal, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_equal"},

    {"get_next", (PyCFunction)ProductLinkKey1_next, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_next"},

    {"get_prev", (PyCFunction)ProductLinkKey1_prev, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_prev"},

    {"get_gt", (PyCFunction)ProductLinkKey1_gt, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_gt"},

    {"get_ge", (PyCFunction)ProductLinkKey1_ge, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_ge"},

    {"get_lt", (PyCFunction)ProductLinkKey1_lt, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_lt"},

    {"get_le", (PyCFunction)ProductLinkKey1_le, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_le"},

    {"get_first", (PyCFunction)ProductLinkKey1_first, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_first"},

    {"get_last", (PyCFunction)ProductLinkKey1_last, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_last"},

    {"gets_equal", (PyCFunction)ProductLinkKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_iter_equal"},

    {"gets_gt", (PyCFunction)ProductLinkKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_iter_gt"},

    {"gets_ge", (PyCFunction)ProductLinkKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_iter_ge"},

    {"gets_lt", (PyCFunction)ProductLinkKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_iter_lt"},

    {"gets_le", (PyCFunction)ProductLinkKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_iter_le"},

    {"gets_first", (PyCFunction)ProductLinkKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_iter_first"},

    {"gets_last", (PyCFunction)ProductLinkKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductLinkKey1_iter_last"},

    {NULL}
};

static PyTypeObject ProductLinkKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductLinkKey1",             /*tp_name*/
    sizeof(ProductLinkKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductLinkKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductLinkKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductLinkKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductLinkKey1_methods,             /* tp_methods */
    0,
    ProductLinkKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductLinkKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductLinkKey1_new,                 /* tp_new */
};
PyTypeObject *getProductLinkKey1Type()
{
    return &ProductLinkKey1Type_;
}
