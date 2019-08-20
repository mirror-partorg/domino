
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "product_view.h"

static void ProductView_dealloc(PyObject* self)
{
    do_free(((ProductView*)self)->priv);
    Py_DECREF(((ProductView*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductView free\n");
}
static PyObject *ProductView_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductView new\n");
    ProductView *self;
    self = PyObject_NEW(ProductView, type);
    //self = (ProductView *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_view_rec_t));
    return (PyObject *)self;
}
static int ProductView_init(ProductView *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductView init\n");
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

    do_text_set_empty(self->priv->data.barcode);

    do_text_set_empty(self->priv->data.sklad);

    do_text_set_empty(self->priv->data.sort);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductView_clone(ProductView *self)
{
    ProductView *result;
    result = (ProductView*)PyObject_CallFunction((PyObject*)getProductViewType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *ProductView_record_type(ProductView* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type);

    return result;
}

static PyObject *ProductView_set_record_type(ProductView* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductView_code(ProductView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductView_set_code(ProductView* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductView_barcode(ProductView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.barcode);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductView_set_barcode(ProductView* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.barcode");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.barcode, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.barcode);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductView_store(ProductView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductView_set_store(ProductView* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductView_sort(ProductView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sort);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductView_set_sort(ProductView* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sort");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.sort, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.sort);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductView_gt(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_last(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_next(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_le(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_lt(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_equal(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_ge(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_prev(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_first(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProductViewKey0Type() )
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() )
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_iter_gt(ProductView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductViewKey0Type() ) {
        key_cmp = (product_view_key0_t*)do_malloc(sizeof(product_view_key0_t));
        memcpy(key_cmp, ((ProductViewKey0*)key)->priv, sizeof(product_view_key0_t));
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() ) {
        key_cmp = (product_view_key1_t*)do_malloc(sizeof(product_view_key1_t));
        memcpy(key_cmp, ((ProductViewKey1*)key)->priv, sizeof(product_view_key1_t));
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->type, 
                    ((ProductViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sklad, 
                    ((ProductViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sort, 
                    ((ProductViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->code, 
                    ((ProductViewKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->code, 
                    ((ProductViewKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->sklad, 
                    ((ProductViewKey1*)key)->priv->sklad))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
            status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
            status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductView_iter_last(ProductView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductViewKey0Type() ) {
        key_cmp = (product_view_key0_t*)do_malloc(sizeof(product_view_key0_t));
        memcpy(key_cmp, ((ProductViewKey0*)key)->priv, sizeof(product_view_key0_t));
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() ) {
        key_cmp = (product_view_key1_t*)do_malloc(sizeof(product_view_key1_t));
        memcpy(key_cmp, ((ProductViewKey1*)key)->priv, sizeof(product_view_key1_t));
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->type, 
                    ((ProductViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sklad, 
                    ((ProductViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sort, 
                    ((ProductViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->code, 
                    ((ProductViewKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->code, 
                    ((ProductViewKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->sklad, 
                    ((ProductViewKey1*)key)->priv->sklad))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
            status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
            status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductView_iter_le(ProductView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductViewKey0Type() ) {
        key_cmp = (product_view_key0_t*)do_malloc(sizeof(product_view_key0_t));
        memcpy(key_cmp, ((ProductViewKey0*)key)->priv, sizeof(product_view_key0_t));
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() ) {
        key_cmp = (product_view_key1_t*)do_malloc(sizeof(product_view_key1_t));
        memcpy(key_cmp, ((ProductViewKey1*)key)->priv, sizeof(product_view_key1_t));
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->type, 
                    ((ProductViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sklad, 
                    ((ProductViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sort, 
                    ((ProductViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->code, 
                    ((ProductViewKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->code, 
                    ((ProductViewKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->sklad, 
                    ((ProductViewKey1*)key)->priv->sklad))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
            status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
            status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductView_iter_ge(ProductView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductViewKey0Type() ) {
        key_cmp = (product_view_key0_t*)do_malloc(sizeof(product_view_key0_t));
        memcpy(key_cmp, ((ProductViewKey0*)key)->priv, sizeof(product_view_key0_t));
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() ) {
        key_cmp = (product_view_key1_t*)do_malloc(sizeof(product_view_key1_t));
        memcpy(key_cmp, ((ProductViewKey1*)key)->priv, sizeof(product_view_key1_t));
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->type, 
                    ((ProductViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sklad, 
                    ((ProductViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sort, 
                    ((ProductViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->code, 
                    ((ProductViewKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->code, 
                    ((ProductViewKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->sklad, 
                    ((ProductViewKey1*)key)->priv->sklad))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
            status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
            status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductView_iter_equal(ProductView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductViewKey0Type() ) {
        key_cmp = (product_view_key0_t*)do_malloc(sizeof(product_view_key0_t));
        memcpy(key_cmp, ((ProductViewKey0*)key)->priv, sizeof(product_view_key0_t));
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() ) {
        key_cmp = (product_view_key1_t*)do_malloc(sizeof(product_view_key1_t));
        memcpy(key_cmp, ((ProductViewKey1*)key)->priv, sizeof(product_view_key1_t));
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->type, 
                    ((ProductViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sklad, 
                    ((ProductViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sort, 
                    ((ProductViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->code, 
                    ((ProductViewKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->code, 
                    ((ProductViewKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->sklad, 
                    ((ProductViewKey1*)key)->priv->sklad))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
            status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
            status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductView_iter_lt(ProductView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductViewKey0Type() ) {
        key_cmp = (product_view_key0_t*)do_malloc(sizeof(product_view_key0_t));
        memcpy(key_cmp, ((ProductViewKey0*)key)->priv, sizeof(product_view_key0_t));
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() ) {
        key_cmp = (product_view_key1_t*)do_malloc(sizeof(product_view_key1_t));
        memcpy(key_cmp, ((ProductViewKey1*)key)->priv, sizeof(product_view_key1_t));
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->type, 
                    ((ProductViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sklad, 
                    ((ProductViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sort, 
                    ((ProductViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->code, 
                    ((ProductViewKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->code, 
                    ((ProductViewKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->sklad, 
                    ((ProductViewKey1*)key)->priv->sklad))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
            status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
            status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *ProductView_iter_first(ProductView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProductViewKey0Type() ) {
        key_cmp = (product_view_key0_t*)do_malloc(sizeof(product_view_key0_t));
        memcpy(key_cmp, ((ProductViewKey0*)key)->priv, sizeof(product_view_key0_t));
        status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProductViewKey1Type() ) {
        key_cmp = (product_view_key1_t*)do_malloc(sizeof(product_view_key1_t));
        memcpy(key_cmp, ((ProductViewKey1*)key)->priv, sizeof(product_view_key1_t));
        status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->type, 
                    ((ProductViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sklad, 
                    ((ProductViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->sort, 
                    ((ProductViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((product_view_key0_t*)key_cmp)->code, 
                    ((ProductViewKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->code, 
                    ((ProductViewKey1*)key)->priv->code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((product_view_key1_t*)key_cmp)->sklad, 
                    ((ProductViewKey1*)key)->priv->sklad))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = ProductView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProductViewKey0Type() ) {
            status = do_product_view_get0(self->alias->alias, self->priv, ((ProductViewKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProductViewKey1Type() ) {
            status = do_product_view_get1(self->alias->alias, self->priv, ((ProductViewKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *ProductView_insert(ProductView* self)
{
    int status;
    status = do_product_view_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_update(ProductView* self)
{
    int status;
    status = do_product_view_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_delete(ProductView* self)
{
    int status;
    status = do_product_view_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductView_set_alias(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductView*)self)->alias);
    Py_INCREF(obj);
    ((ProductView*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductView_set(ProductView* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductViewType() ) {
        memcpy(self->priv, ((ProductView*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {

        do_cpy(self->priv->data.type, ((ProductViewKey0*)obj)->priv->type);

        do_cpy(self->priv->data.code, ((ProductViewKey0*)obj)->priv->code);

        do_cpy(self->priv->data.sklad, ((ProductViewKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.sort, ((ProductViewKey0*)obj)->priv->sort);

    }

    else 
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {

        do_cpy(self->priv->data.code, ((ProductViewKey1*)obj)->priv->code);

        do_cpy(self->priv->data.sklad, ((ProductViewKey1*)obj)->priv->sklad);

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
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("barcode", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_barcode", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProductView\"does not contain \"%s\"", field_name);
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

static PyObject *ProductView_fields(ProductView* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductView");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductView_record_type(self,NULL);
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

    value =  ProductView_code(self,NULL);
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

    value =  ProductView_barcode(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("barcode", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductView_store(self,NULL);
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

    value =  ProductView_sort(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("sort", value);
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

static PyGetSetDef ProductView_getset[] = {

    {"record_type",(getter)ProductView_record_type},

    {"code",(getter)ProductView_code},

    {"barcode",(getter)ProductView_barcode},

    {"store",(getter)ProductView_store},

    {"sort",(getter)ProductView_sort},

    {NULL}
};

static PyMethodDef ProductView_methods[] = {
    {"set", (PyCFunction)ProductView_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductView_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductView_clone, METH_NOARGS, "clone"},

    {"set_record_type", (PyCFunction)ProductView_set_record_type, METH_VARARGS|METH_KEYWORDS, "ProductView_set_record_type"},

    {"set_code", (PyCFunction)ProductView_set_code, METH_VARARGS|METH_KEYWORDS, "ProductView_set_code"},

    {"set_barcode", (PyCFunction)ProductView_set_barcode, METH_VARARGS|METH_KEYWORDS, "ProductView_set_barcode"},

    {"set_store", (PyCFunction)ProductView_set_store, METH_VARARGS|METH_KEYWORDS, "ProductView_set_store"},

    {"set_sort", (PyCFunction)ProductView_set_sort, METH_VARARGS|METH_KEYWORDS, "ProductView_set_sort"},

    {"get_gt", (PyCFunction)ProductView_gt, METH_VARARGS|METH_KEYWORDS, "ProductView_gt"},

    {"get_last", (PyCFunction)ProductView_last, METH_VARARGS|METH_KEYWORDS, "ProductView_last"},

    {"get_next", (PyCFunction)ProductView_next, METH_VARARGS|METH_KEYWORDS, "ProductView_next"},

    {"get_le", (PyCFunction)ProductView_le, METH_VARARGS|METH_KEYWORDS, "ProductView_le"},

    {"get_lt", (PyCFunction)ProductView_lt, METH_VARARGS|METH_KEYWORDS, "ProductView_lt"},

    {"get_equal", (PyCFunction)ProductView_equal, METH_VARARGS|METH_KEYWORDS, "ProductView_equal"},

    {"get_ge", (PyCFunction)ProductView_ge, METH_VARARGS|METH_KEYWORDS, "ProductView_ge"},

    {"get_prev", (PyCFunction)ProductView_prev, METH_VARARGS|METH_KEYWORDS, "ProductView_prev"},

    {"get_first", (PyCFunction)ProductView_first, METH_VARARGS|METH_KEYWORDS, "ProductView_first"},

    {"gets_gt", (PyCFunction)ProductView_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductView_iter_gt"},

    {"gets_last", (PyCFunction)ProductView_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductView_iter_last"},

    {"gets_le", (PyCFunction)ProductView_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductView_iter_le"},

    {"gets_ge", (PyCFunction)ProductView_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductView_iter_ge"},

    {"gets_equal", (PyCFunction)ProductView_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductView_iter_equal"},

    {"gets_lt", (PyCFunction)ProductView_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductView_iter_lt"},

    {"gets_first", (PyCFunction)ProductView_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductView_iter_first"},

    {"insert", (PyCFunction)ProductView_insert, METH_VARARGS|METH_KEYWORDS, "ProductView_insert"},

    {"update", (PyCFunction)ProductView_update, METH_VARARGS|METH_KEYWORDS, "ProductView_update"},

    {"delete", (PyCFunction)ProductView_delete, METH_VARARGS|METH_KEYWORDS, "ProductView_delete"},

    {NULL}
};

static PyTypeObject ProductViewType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductView",             /*tp_name*/
    sizeof(ProductView),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductView_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductView_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductView objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductView_methods,             /* tp_methods */
    0,
    ProductView_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductView_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductView_new,                 /* tp_new */
};
PyTypeObject *getProductViewType()
{
    return &ProductViewType_;
}

static void ProductViewKey0_dealloc(PyObject* self)
{
    do_free(((ProductViewKey0*)self)->priv);
    Py_DECREF(((ProductViewKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductViewKey0 free\n");
}
static PyObject *ProductViewKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductViewKey0 new\n");
    ProductViewKey0 *self;
    self = PyObject_NEW(ProductViewKey0, type);
    //self = (ProductViewKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_view_key0_t));
    return (PyObject *)self;
}
static int ProductViewKey0_init(ProductViewKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductViewKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->sort);

    do_text_set_empty(self->priv->code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProductViewKey0_clone(ProductViewKey0 *self)
{
    ProductViewKey0 *result;
    result = (ProductViewKey0*)PyObject_CallFunction((PyObject*)getProductViewKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductViewKey0_record_type(ProductViewKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *ProductViewKey0_set_record_type(ProductViewKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductViewKey0_store(ProductViewKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductViewKey0_set_store(ProductViewKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductViewKey0_sort(ProductViewKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sort);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductViewKey0_set_sort(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sort");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->sort, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->sort);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProductViewKey0_code(ProductViewKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductViewKey0_set_code(ProductViewKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductViewKey0_gt(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_last(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_next(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_le(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_lt(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_equal(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_ge(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_prev(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_first(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey0_iter_gt(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey0_iter_last(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey0_iter_le(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey0_iter_ge(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey0_iter_equal(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey0_iter_lt(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey0_iter_first(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = ProductViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey0_cmp(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->type - ((ProductViewKey0*)obj)->priv->type;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((ProductViewKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->sort, ((ProductViewKey0*)obj)->priv->sort);
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->code, ((ProductViewKey0*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductViewKey0_set_alias(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductViewKey0*)self)->alias);
    Py_INCREF(obj);
    ((ProductViewKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductViewKey0_set(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductViewKey0Type() ) {
        memcpy(self->priv, ((ProductViewKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductViewType() ) {

        do_cpy(self->priv->type, ((ProductView*)obj)->priv->data.type);

        do_cpy(self->priv->sklad, ((ProductView*)obj)->priv->data.sklad);

        do_cpy(self->priv->sort, ((ProductView*)obj)->priv->data.sort);

        do_cpy(self->priv->code, ((ProductView*)obj)->priv->data.code);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                        return NULL;
                    break;

                case 3:
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
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProductViewKey0\"does not contain \"%s\"", field_name);
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

static PyObject *ProductViewKey0_fields(ProductViewKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductViewKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductViewKey0_record_type(self,NULL);
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

    value =  ProductViewKey0_store(self,NULL);
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

    value =  ProductViewKey0_sort(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("sort", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProductViewKey0_code(self,NULL);
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

static PyGetSetDef ProductViewKey0_getset[] = {

    {"record_type",(getter)ProductViewKey0_record_type},

    {"store",(getter)ProductViewKey0_store},

    {"sort",(getter)ProductViewKey0_sort},

    {"code",(getter)ProductViewKey0_code},

    {NULL}
};

static PyMethodDef ProductViewKey0_methods[] = {
    {"set", (PyCFunction)ProductViewKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductViewKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductViewKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductViewKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_record_type", (PyCFunction)ProductViewKey0_set_record_type, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_set_record_type"},

    {"set_store", (PyCFunction)ProductViewKey0_set_store, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_set_store"},

    {"set_sort", (PyCFunction)ProductViewKey0_set_sort, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_set_sort"},

    {"set_code", (PyCFunction)ProductViewKey0_set_code, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_set_code"},

    {"get_gt", (PyCFunction)ProductViewKey0_gt, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_gt"},

    {"get_last", (PyCFunction)ProductViewKey0_last, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_last"},

    {"get_next", (PyCFunction)ProductViewKey0_next, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_next"},

    {"get_le", (PyCFunction)ProductViewKey0_le, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_le"},

    {"get_lt", (PyCFunction)ProductViewKey0_lt, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_lt"},

    {"get_equal", (PyCFunction)ProductViewKey0_equal, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_equal"},

    {"get_ge", (PyCFunction)ProductViewKey0_ge, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_ge"},

    {"get_prev", (PyCFunction)ProductViewKey0_prev, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_prev"},

    {"get_first", (PyCFunction)ProductViewKey0_first, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_first"},

    {"gets_gt", (PyCFunction)ProductViewKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_iter_gt"},

    {"gets_last", (PyCFunction)ProductViewKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_iter_last"},

    {"gets_le", (PyCFunction)ProductViewKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_iter_le"},

    {"gets_ge", (PyCFunction)ProductViewKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_iter_ge"},

    {"gets_equal", (PyCFunction)ProductViewKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_iter_equal"},

    {"gets_lt", (PyCFunction)ProductViewKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_iter_lt"},

    {"gets_first", (PyCFunction)ProductViewKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductViewKey0_iter_first"},

    {NULL}
};

static PyTypeObject ProductViewKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductViewKey0",             /*tp_name*/
    sizeof(ProductViewKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductViewKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductViewKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductViewKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductViewKey0_methods,             /* tp_methods */
    0,
    ProductViewKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductViewKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductViewKey0_new,                 /* tp_new */
};
PyTypeObject *getProductViewKey0Type()
{
    return &ProductViewKey0Type_;
}

static void ProductViewKey1_dealloc(PyObject* self)
{
    do_free(((ProductViewKey1*)self)->priv);
    Py_DECREF(((ProductViewKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProductViewKey1 free\n");
}
static PyObject *ProductViewKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProductViewKey1 new\n");
    ProductViewKey1 *self;
    self = PyObject_NEW(ProductViewKey1, type);
    //self = (ProductViewKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(product_view_key1_t));
    return (PyObject *)self;
}
static int ProductViewKey1_init(ProductViewKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProductViewKey1 init\n");
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

static PyObject *ProductViewKey1_clone(ProductViewKey1 *self)
{
    ProductViewKey1 *result;
    result = (ProductViewKey1*)PyObject_CallFunction((PyObject*)getProductViewKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProductViewKey1_code(ProductViewKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductViewKey1_set_code(ProductViewKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductViewKey1_store(ProductViewKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProductViewKey1_set_store(ProductViewKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProductViewKey1_gt(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_last(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_next(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_le(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_lt(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_equal(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_ge(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_prev(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_first(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProductViewKey1_iter_gt(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_GT);
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

 
        item = ProductViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey1_iter_last(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_LAST);
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

 
        item = ProductViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey1_iter_le(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_LE);
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

 
        item = ProductViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey1_iter_ge(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_GE);
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

 
        item = ProductViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey1_iter_equal(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
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

 
        item = ProductViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey1_iter_lt(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_LT);
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

 
        item = ProductViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey1_iter_first(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    product_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_FIRST);
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

 
        item = ProductViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_product_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProductViewKey1_cmp(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((ProductViewKey1*)obj)->priv->code);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((ProductViewKey1*)obj)->priv->sklad);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProductViewKey1_set_alias(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProductViewKey1*)self)->alias);
    Py_INCREF(obj);
    ((ProductViewKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProductViewKey1_set(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProductViewKey1Type() ) {
        memcpy(self->priv, ((ProductViewKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProductViewType() ) {

        do_cpy(self->priv->code, ((ProductView*)obj)->priv->data.code);

        do_cpy(self->priv->sklad, ((ProductView*)obj)->priv->data.sklad);

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
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProductViewKey1\"does not contain \"%s\"", field_name);
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

static PyObject *ProductViewKey1_fields(ProductViewKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProductViewKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProductViewKey1_code(self,NULL);
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

    value =  ProductViewKey1_store(self,NULL);
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

static PyGetSetDef ProductViewKey1_getset[] = {

    {"code",(getter)ProductViewKey1_code},

    {"store",(getter)ProductViewKey1_store},

    {NULL}
};

static PyMethodDef ProductViewKey1_methods[] = {
    {"set", (PyCFunction)ProductViewKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProductViewKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProductViewKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProductViewKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)ProductViewKey1_set_code, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_set_code"},

    {"set_store", (PyCFunction)ProductViewKey1_set_store, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_set_store"},

    {"get_gt", (PyCFunction)ProductViewKey1_gt, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_gt"},

    {"get_last", (PyCFunction)ProductViewKey1_last, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_last"},

    {"get_next", (PyCFunction)ProductViewKey1_next, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_next"},

    {"get_le", (PyCFunction)ProductViewKey1_le, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_le"},

    {"get_lt", (PyCFunction)ProductViewKey1_lt, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_lt"},

    {"get_equal", (PyCFunction)ProductViewKey1_equal, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_equal"},

    {"get_ge", (PyCFunction)ProductViewKey1_ge, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_ge"},

    {"get_prev", (PyCFunction)ProductViewKey1_prev, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_prev"},

    {"get_first", (PyCFunction)ProductViewKey1_first, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_first"},

    {"gets_gt", (PyCFunction)ProductViewKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_iter_gt"},

    {"gets_last", (PyCFunction)ProductViewKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_iter_last"},

    {"gets_le", (PyCFunction)ProductViewKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_iter_le"},

    {"gets_ge", (PyCFunction)ProductViewKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_iter_ge"},

    {"gets_equal", (PyCFunction)ProductViewKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_iter_equal"},

    {"gets_lt", (PyCFunction)ProductViewKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_iter_lt"},

    {"gets_first", (PyCFunction)ProductViewKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "ProductViewKey1_iter_first"},

    {NULL}
};

static PyTypeObject ProductViewKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProductViewKey1",             /*tp_name*/
    sizeof(ProductViewKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProductViewKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProductViewKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProductViewKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProductViewKey1_methods,             /* tp_methods */
    0,
    ProductViewKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProductViewKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProductViewKey1_new,                 /* tp_new */
};
PyTypeObject *getProductViewKey1Type()
{
    return &ProductViewKey1Type_;
}
