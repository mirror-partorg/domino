
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "product_data.h"

static void ProductData_dealloc(PyObject* self)
{
    do_free(((ProductData*)self)->priv);
    Py_DECREF(((ProductData*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductData free\n");
}
static PyObject *ProductData_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductData new\n");
    ProductData *self;
    self = PyObject_NEW(ProductData, type);
    //self = (ProductData *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_data_rec_t));
    return (PyObject *)self;
}
static int ProductData_init(ProductData *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductData init\n");
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

    do_product_data_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductData_clone(ProductData *self)
{
    ProductData *result;
    result = (ProductData*)PyObject_CallFunction((PyObject*)getProductDataType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *ProductData_code(ProductData* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductData_set_code(ProductData* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductData_record_type(ProductData* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type);

    return result;
}

static PyObject *ProductData_set_record_type(ProductData* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductData_number(ProductData* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.number);

    return result;
}

static PyObject *ProductData_set_number(ProductData* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductData_set_param(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_product_data_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_product_data_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_product_data_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *ProductData_set_params(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_product_data_params_clear(self->alias->alias,self->priv);
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
                 do_product_data_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_product_data_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_product_data_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *ProductData_get_param(ProductData* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_product_data_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *ProductData_params_clear(ProductData* self)
{
    do_product_data_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *ProductData_get_params(ProductData* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *ProductData_prev(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_gt(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_next(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_ge(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_equal(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_last(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_lt(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_le(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_first(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductDataKey0Type() )
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_iter_gt(ProductData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductDataKey0Type() ) {
        key_cmp = (product_data_key0_t*)do_malloc(sizeof(product_data_key0_t));
        memcpy(key_cmp, ((ProductDataKey0*)key)->priv, sizeof(product_data_key0_t));
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->code, 
                    ((ProductDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->type, 
                    ((ProductDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->number, 
                    ((ProductDataKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
            status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductData_iter_ge(ProductData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductDataKey0Type() ) {
        key_cmp = (product_data_key0_t*)do_malloc(sizeof(product_data_key0_t));
        memcpy(key_cmp, ((ProductDataKey0*)key)->priv, sizeof(product_data_key0_t));
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->code, 
                    ((ProductDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->type, 
                    ((ProductDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->number, 
                    ((ProductDataKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
            status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductData_iter_equal(ProductData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductDataKey0Type() ) {
        key_cmp = (product_data_key0_t*)do_malloc(sizeof(product_data_key0_t));
        memcpy(key_cmp, ((ProductDataKey0*)key)->priv, sizeof(product_data_key0_t));
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->code, 
                    ((ProductDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->type, 
                    ((ProductDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->number, 
                    ((ProductDataKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
            status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductData_iter_last(ProductData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductDataKey0Type() ) {
        key_cmp = (product_data_key0_t*)do_malloc(sizeof(product_data_key0_t));
        memcpy(key_cmp, ((ProductDataKey0*)key)->priv, sizeof(product_data_key0_t));
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->code, 
                    ((ProductDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->type, 
                    ((ProductDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->number, 
                    ((ProductDataKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
            status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductData_iter_lt(ProductData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductDataKey0Type() ) {
        key_cmp = (product_data_key0_t*)do_malloc(sizeof(product_data_key0_t));
        memcpy(key_cmp, ((ProductDataKey0*)key)->priv, sizeof(product_data_key0_t));
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->code, 
                    ((ProductDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->type, 
                    ((ProductDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->number, 
                    ((ProductDataKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
            status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductData_iter_le(ProductData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductDataKey0Type() ) {
        key_cmp = (product_data_key0_t*)do_malloc(sizeof(product_data_key0_t));
        memcpy(key_cmp, ((ProductDataKey0*)key)->priv, sizeof(product_data_key0_t));
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->code, 
                    ((ProductDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->type, 
                    ((ProductDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->number, 
                    ((ProductDataKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
            status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductData_iter_first(ProductData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductDataKey0Type() ) {
        key_cmp = (product_data_key0_t*)do_malloc(sizeof(product_data_key0_t));
        memcpy(key_cmp, ((ProductDataKey0*)key)->priv, sizeof(product_data_key0_t));
        status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->code, 
                    ((ProductDataKey0*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->type, 
                    ((ProductDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_data_key0_t*)key_cmp)->number, 
                    ((ProductDataKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductDataKey0Type() ) {
            status = do_product_data_get0(self->alias->alias, self->priv, ((ProductDataKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductData_update(ProductData* self)
{
    int status;
    status = do_product_data_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_insert(ProductData* self)
{
    int status;
    status = do_product_data_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_delete(ProductData* self)
{
    int status;
    status = do_product_data_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductData_set_alias(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductData*)self)->alias);
    Py_INCREF(obj);
    ((ProductData*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductData_set(ProductData* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductDataType() ) {
        memcpy(self->priv, ((ProductData*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {

        do_cpy(self->priv->data.code, ((ProductDataKey0*)obj)->priv->code);

        do_cpy(self->priv->data.type, ((ProductDataKey0*)obj)->priv->type);

        do_cpy(self->priv->data.number, ((ProductDataKey0*)obj)->priv->number);

    }

    else 
    if ( Py_TYPE(obj) == getProductType() ) {

        do_cpy(self->priv->data.code, ((Product*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getProductKey0Type() ) {

        do_cpy(self->priv->data.code, ((ProductKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductKey1Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductKey4Type() ) {

        do_cpy(self->priv->data.code, ((ProductKey4*)obj)->priv->code);

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
                do_log(LOG_ERR, "Class \"ProductData\"does not contain \"%s\"", field_name);
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

static PyObject *ProductData_fields(ProductData* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductData");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductData_code(self,NULL);
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

    value =  ProductData_record_type(self,NULL);
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

    value =  ProductData_number(self,NULL);
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

    value = ProductData_get_params(self, NULL);
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

static PyGetSetDef ProductData_getset[] = {

    {"code",(getter)ProductData_code},

    {"record_type",(getter)ProductData_record_type},

    {"number",(getter)ProductData_number},

    {"params",(getter)ProductData_get_params},

    {NULL}
};

static PyMethodDef ProductData_methods[] = {
    {"set", (PyCFunction)ProductData_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductData_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductData_clone, METH_NOARGS, "clone"},

    {"set_code", (PyCFunction)ProductData_set_code, METH_VARARGS|METH_KEYWORDS, "ProductData_set_code"},

    {"set_record_type", (PyCFunction)ProductData_set_record_type, METH_VARARGS|METH_KEYWORDS, "ProductData_set_record_type"},

    {"set_number", (PyCFunction)ProductData_set_number, METH_VARARGS|METH_KEYWORDS, "ProductData_set_number"},

    {"set_param", (PyCFunction)ProductData_set_param, METH_VARARGS|METH_KEYWORDS, "do_ProductData_param_set"},
    {"param", (PyCFunction)ProductData_get_param, METH_VARARGS|METH_KEYWORDS, "do_ProductData_param"},
    {"clear_params", (PyCFunction)ProductData_params_clear, METH_NOARGS, "do_ProductData_param_clear"},
    {"set_params", (PyCFunction)ProductData_set_params, METH_VARARGS|METH_KEYWORDS, "do_ProductData_set_params"},

    {"get_prev", (PyCFunction)ProductData_prev, METH_VARARGS|METH_KEYWORDS, "ProductData_prev"},

    {"get_gt", (PyCFunction)ProductData_gt, METH_VARARGS|METH_KEYWORDS, "ProductData_gt"},

    {"get_next", (PyCFunction)ProductData_next, METH_VARARGS|METH_KEYWORDS, "ProductData_next"},

    {"get_ge", (PyCFunction)ProductData_ge, METH_VARARGS|METH_KEYWORDS, "ProductData_ge"},

    {"get_equal", (PyCFunction)ProductData_equal, METH_VARARGS|METH_KEYWORDS, "ProductData_equal"},

    {"get_last", (PyCFunction)ProductData_last, METH_VARARGS|METH_KEYWORDS, "ProductData_last"},

    {"get_lt", (PyCFunction)ProductData_lt, METH_VARARGS|METH_KEYWORDS, "ProductData_lt"},

    {"get_le", (PyCFunction)ProductData_le, METH_VARARGS|METH_KEYWORDS, "ProductData_le"},

    {"get_first", (PyCFunction)ProductData_first, METH_VARARGS|METH_KEYWORDS, "ProductData_first"},

    {"gets_gt", (PyCFunction)ProductData_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductData_iter_gt"},

    {"gets_ge", (PyCFunction)ProductData_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductData_iter_ge"},

    {"gets_equal", (PyCFunction)ProductData_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductData_iter_equal"},

    {"gets_last", (PyCFunction)ProductData_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductData_iter_last"},

    {"gets_lt", (PyCFunction)ProductData_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductData_iter_lt"},

    {"gets_le", (PyCFunction)ProductData_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductData_iter_le"},

    {"gets_first", (PyCFunction)ProductData_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductData_iter_first"},

    {"update", (PyCFunction)ProductData_update, METH_VARARGS|METH_KEYWORDS, "ProductData_update"},

    {"insert", (PyCFunction)ProductData_insert, METH_VARARGS|METH_KEYWORDS, "ProductData_insert"},

    {"delete", (PyCFunction)ProductData_delete, METH_VARARGS|METH_KEYWORDS, "ProductData_delete"},

    {NULL}
};

static PyTypeObject ProductDataType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductData",             /*tp_name*/
    sizeof(ProductData),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductData_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductData_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductData objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductData_methods,             /* tp_methods */
    0,
    ProductData_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductData_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductData_new,                 /* tp_new */
};
PyTypeObject *getProductDataType()
{
    return &ProductDataType_;
}

static void ProductDataKey0_dealloc(PyObject* self)
{
    do_free(((ProductDataKey0*)self)->priv);
    Py_DECREF(((ProductDataKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductDataKey0 free\n");
}
static PyObject *ProductDataKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductDataKey0 new\n");
    ProductDataKey0 *self;
    self = PyObject_NEW(ProductDataKey0, type);
    //self = (ProductDataKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_data_key0_t));
    return (PyObject *)self;
}
static int ProductDataKey0_init(ProductDataKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductDataKey0 init\n");
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

static PyObject *ProductDataKey0_clone(ProductDataKey0 *self)
{
    ProductDataKey0 *result;
    result = (ProductDataKey0*)PyObject_CallFunction((PyObject*)getProductDataKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductDataKey0_code(ProductDataKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductDataKey0_set_code(ProductDataKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductDataKey0_record_type(ProductDataKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *ProductDataKey0_set_record_type(ProductDataKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductDataKey0_number(ProductDataKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->number);

    return result;
}

static PyObject *ProductDataKey0_set_number(ProductDataKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductDataKey0_prev(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_gt(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_next(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_ge(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_equal(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_last(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_lt(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_le(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_first(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductDataKey0_iter_gt(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
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

 
        item = ProductDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductDataKey0_iter_ge(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
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

 
        item = ProductDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductDataKey0_iter_equal(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
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

 
        item = ProductDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductDataKey0_iter_last(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
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

 
        item = ProductDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductDataKey0_iter_lt(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
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

 
        item = ProductDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductDataKey0_iter_le(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
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

 
        item = ProductDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductDataKey0_iter_first(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
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

 
        item = ProductDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductDataKey0_cmp(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((ProductDataKey0*)obj)->priv->code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->type - ((ProductDataKey0*)obj)->priv->type;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->number - ((ProductDataKey0*)obj)->priv->number;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductDataKey0_set_alias(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductDataKey0*)self)->alias);
    Py_INCREF(obj);
    ((ProductDataKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductDataKey0_set(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductDataKey0Type() ) {
        memcpy(self->priv, ((ProductDataKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductDataType() ) {

        do_cpy(self->priv->code, ((ProductData*)obj)->priv->data.code);

        do_cpy(self->priv->type, ((ProductData*)obj)->priv->data.type);

        do_cpy(self->priv->number, ((ProductData*)obj)->priv->data.number);

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
    if ( Py_TYPE(obj) == getProductType() ) {

        do_cpy(self->priv->code, ((Product*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getProductKey0Type() ) {

        do_cpy(self->priv->code, ((ProductKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getProductKey1Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getProductKey4Type() ) {

        do_cpy(self->priv->code, ((ProductKey4*)obj)->priv->code);

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
                do_log(LOG_ERR, "Class \"ProductDataKey0\"does not contain \"%s\"", field_name);
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

static PyObject *ProductDataKey0_fields(ProductDataKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductDataKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductDataKey0_code(self,NULL);
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

    value =  ProductDataKey0_record_type(self,NULL);
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

    value =  ProductDataKey0_number(self,NULL);
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

static PyGetSetDef ProductDataKey0_getset[] = {

    {"code",(getter)ProductDataKey0_code},

    {"record_type",(getter)ProductDataKey0_record_type},

    {"number",(getter)ProductDataKey0_number},

    {NULL}
};

static PyMethodDef ProductDataKey0_methods[] = {
    {"set", (PyCFunction)ProductDataKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductDataKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductDataKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductDataKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)ProductDataKey0_set_code, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_set_code"},

    {"set_record_type", (PyCFunction)ProductDataKey0_set_record_type, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_set_record_type"},

    {"set_number", (PyCFunction)ProductDataKey0_set_number, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_set_number"},

    {"get_prev", (PyCFunction)ProductDataKey0_prev, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_prev"},

    {"get_gt", (PyCFunction)ProductDataKey0_gt, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_gt"},

    {"get_next", (PyCFunction)ProductDataKey0_next, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_next"},

    {"get_ge", (PyCFunction)ProductDataKey0_ge, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_ge"},

    {"get_equal", (PyCFunction)ProductDataKey0_equal, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_equal"},

    {"get_last", (PyCFunction)ProductDataKey0_last, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_last"},

    {"get_lt", (PyCFunction)ProductDataKey0_lt, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_lt"},

    {"get_le", (PyCFunction)ProductDataKey0_le, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_le"},

    {"get_first", (PyCFunction)ProductDataKey0_first, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_first"},

    {"gets_gt", (PyCFunction)ProductDataKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_iter_gt"},

    {"gets_ge", (PyCFunction)ProductDataKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_iter_ge"},

    {"gets_equal", (PyCFunction)ProductDataKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_iter_equal"},

    {"gets_last", (PyCFunction)ProductDataKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_iter_last"},

    {"gets_lt", (PyCFunction)ProductDataKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_iter_lt"},

    {"gets_le", (PyCFunction)ProductDataKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_iter_le"},

    {"gets_first", (PyCFunction)ProductDataKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductDataKey0_iter_first"},

    {NULL}
};

static PyTypeObject ProductDataKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductDataKey0",             /*tp_name*/
    sizeof(ProductDataKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductDataKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductDataKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductDataKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductDataKey0_methods,             /* tp_methods */
    0,
    ProductDataKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductDataKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductDataKey0_new,                 /* tp_new */
};
PyTypeObject *getProductDataKey0Type()
{
    return &ProductDataKey0Type_;
}
