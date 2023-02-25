
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "barcode.h"

static void Barcode_dealloc(PyObject* self)
{
    do_free(((Barcode*)self)->priv);
    Py_DECREF(((Barcode*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Barcode free\n");
}
static PyObject *Barcode_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Barcode new\n");
    Barcode *self;
    self = PyObject_NEW(Barcode, type);
    //self = (Barcode *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(barcode_rec_t));
    return (PyObject *)self;
}
static int Barcode_init(Barcode *self, PyObject *args, PyObject *kwds)
{
    //LOG("Barcode init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.barcode);

    do_text_set_empty(self->priv->data.code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Barcode_clone(Barcode *self)
{
    Barcode *result;
    result = (Barcode*)PyObject_CallFunction((PyObject*)getBarcodeType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Barcode_barcode(Barcode* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.barcode);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Barcode_set_barcode(Barcode* self, PyObject *args, PyObject *kwds)
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

static PyObject *Barcode_code(Barcode* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Barcode_set_code(Barcode* self, PyObject *args, PyObject *kwds)
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

static PyObject *Barcode_total(Barcode* self, void *unused)
{
    PyObject *result;

    result=PyFloat_FromDouble(do_pdecimal(self->priv->data.total, 7, 3));

    return result;
}

static PyObject *Barcode_set_total(Barcode* self, PyObject *args, PyObject *kwds)
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
             do_pdecimal_set(self->priv->data.total, 7, 3,  MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_pdecimal_set(self->priv->data.total, 7, 3,  PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.total");
            return NULL;
        }    
    }
    //result=PyFloat_FromDouble(do_pdecimal(self->priv->data.total, 7, 3));

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Barcode_discount(Barcode* self, void *unused)
{
    PyObject *result;

    result=PyFloat_FromDouble(do_pdecimal(self->priv->data.sk, 3, 0));

    return result;
}

static PyObject *Barcode_set_discount(Barcode* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sk");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_pdecimal_set(self->priv->data.sk, 3, 0,  MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_pdecimal_set(self->priv->data.sk, 3, 0,  PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sk");
            return NULL;
        }    
    }
    //result=PyFloat_FromDouble(do_pdecimal(self->priv->data.sk, 3, 0));

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Barcode_equal(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_next(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_prev(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_gt(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_ge(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_lt(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_le(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_first(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_last(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getBarcodeKey0Type() )
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() )
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_iter_equal(Barcode* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
        key_cmp = (barcode_key0_t*)do_malloc(sizeof(barcode_key0_t));
        memcpy(key_cmp, ((BarcodeKey0*)key)->priv, sizeof(barcode_key0_t));
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
        key_cmp = (barcode_key1_t*)do_malloc(sizeof(barcode_key1_t));
        memcpy(key_cmp, ((BarcodeKey1*)key)->priv, sizeof(barcode_key1_t));
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key0_t*)key_cmp)->barcode, 
                    ((BarcodeKey0*)key)->priv->barcode))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key1_t*)key_cmp)->code, 
                    ((BarcodeKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Barcode_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
            status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
            status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Barcode_iter_gt(Barcode* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
        key_cmp = (barcode_key0_t*)do_malloc(sizeof(barcode_key0_t));
        memcpy(key_cmp, ((BarcodeKey0*)key)->priv, sizeof(barcode_key0_t));
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
        key_cmp = (barcode_key1_t*)do_malloc(sizeof(barcode_key1_t));
        memcpy(key_cmp, ((BarcodeKey1*)key)->priv, sizeof(barcode_key1_t));
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key0_t*)key_cmp)->barcode, 
                    ((BarcodeKey0*)key)->priv->barcode))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key1_t*)key_cmp)->code, 
                    ((BarcodeKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Barcode_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
            status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
            status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Barcode_iter_ge(Barcode* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
        key_cmp = (barcode_key0_t*)do_malloc(sizeof(barcode_key0_t));
        memcpy(key_cmp, ((BarcodeKey0*)key)->priv, sizeof(barcode_key0_t));
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
        key_cmp = (barcode_key1_t*)do_malloc(sizeof(barcode_key1_t));
        memcpy(key_cmp, ((BarcodeKey1*)key)->priv, sizeof(barcode_key1_t));
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key0_t*)key_cmp)->barcode, 
                    ((BarcodeKey0*)key)->priv->barcode))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key1_t*)key_cmp)->code, 
                    ((BarcodeKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Barcode_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
            status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
            status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Barcode_iter_lt(Barcode* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
        key_cmp = (barcode_key0_t*)do_malloc(sizeof(barcode_key0_t));
        memcpy(key_cmp, ((BarcodeKey0*)key)->priv, sizeof(barcode_key0_t));
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
        key_cmp = (barcode_key1_t*)do_malloc(sizeof(barcode_key1_t));
        memcpy(key_cmp, ((BarcodeKey1*)key)->priv, sizeof(barcode_key1_t));
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key0_t*)key_cmp)->barcode, 
                    ((BarcodeKey0*)key)->priv->barcode))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key1_t*)key_cmp)->code, 
                    ((BarcodeKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Barcode_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
            status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
            status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Barcode_iter_le(Barcode* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
        key_cmp = (barcode_key0_t*)do_malloc(sizeof(barcode_key0_t));
        memcpy(key_cmp, ((BarcodeKey0*)key)->priv, sizeof(barcode_key0_t));
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
        key_cmp = (barcode_key1_t*)do_malloc(sizeof(barcode_key1_t));
        memcpy(key_cmp, ((BarcodeKey1*)key)->priv, sizeof(barcode_key1_t));
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key0_t*)key_cmp)->barcode, 
                    ((BarcodeKey0*)key)->priv->barcode))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key1_t*)key_cmp)->code, 
                    ((BarcodeKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Barcode_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
            status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
            status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Barcode_iter_first(Barcode* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
        key_cmp = (barcode_key0_t*)do_malloc(sizeof(barcode_key0_t));
        memcpy(key_cmp, ((BarcodeKey0*)key)->priv, sizeof(barcode_key0_t));
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
        key_cmp = (barcode_key1_t*)do_malloc(sizeof(barcode_key1_t));
        memcpy(key_cmp, ((BarcodeKey1*)key)->priv, sizeof(barcode_key1_t));
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key0_t*)key_cmp)->barcode, 
                    ((BarcodeKey0*)key)->priv->barcode))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key1_t*)key_cmp)->code, 
                    ((BarcodeKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Barcode_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
            status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
            status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Barcode_iter_last(Barcode* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
        key_cmp = (barcode_key0_t*)do_malloc(sizeof(barcode_key0_t));
        memcpy(key_cmp, ((BarcodeKey0*)key)->priv, sizeof(barcode_key0_t));
        status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
        key_cmp = (barcode_key1_t*)do_malloc(sizeof(barcode_key1_t));
        memcpy(key_cmp, ((BarcodeKey1*)key)->priv, sizeof(barcode_key1_t));
        status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key0_t*)key_cmp)->barcode, 
                    ((BarcodeKey0*)key)->priv->barcode))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((barcode_key1_t*)key_cmp)->code, 
                    ((BarcodeKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Barcode_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getBarcodeKey0Type() ) {
            status = do_barcode_get0(self->alias->alias, self->priv, ((BarcodeKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getBarcodeKey1Type() ) {
            status = do_barcode_get1(self->alias->alias, self->priv, ((BarcodeKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Barcode_insert(Barcode* self)
{
    int status;
    status = do_barcode_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_update(Barcode* self)
{
    int status;
    status = do_barcode_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_delete(Barcode* self)
{
    int status;
    status = do_barcode_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Barcode_set_alias(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Barcode*)self)->alias);
    Py_INCREF(obj);
    ((Barcode*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Barcode_set(Barcode* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getBarcodeType() ) {
        memcpy(self->priv, ((Barcode*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getBarcodeKey0Type() ) {

        do_cpy(self->priv->data.barcode, ((BarcodeKey0*)obj)->priv->barcode);

    }

    else 
    if ( Py_TYPE(obj) == getBarcodeKey1Type() ) {

        do_cpy(self->priv->data.code, ((BarcodeKey1*)obj)->priv->code);

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
            if ( !strcmp("barcode", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_barcode", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_total", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("discount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_discount", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Barcode\"does not contain \"%s\"", field_name);
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

static PyObject *Barcode_fields(Barcode* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Barcode");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Barcode_barcode(self,NULL);
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

    value =  Barcode_code(self,NULL);
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

    
    buf = do_strdup_printf("total:%f", 
    do_pdecimal(self->priv->data.total, 7, 3));
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    
    buf = do_strdup_printf("discount:%f", 
    do_pdecimal(self->priv->data.sk, 3, 0));
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

static PyGetSetDef Barcode_getset[] = {

    {"barcode",(getter)Barcode_barcode},

    {"code",(getter)Barcode_code},

    {"total",(getter)Barcode_total},

    {"discount",(getter)Barcode_discount},

    {NULL}
};

static PyMethodDef Barcode_methods[] = {
    {"set", (PyCFunction)Barcode_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Barcode_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Barcode_clone, METH_NOARGS, "clone"},

    {"set_barcode", (PyCFunction)Barcode_set_barcode, METH_VARARGS|METH_KEYWORDS, "Barcode_set_barcode"},

    {"set_code", (PyCFunction)Barcode_set_code, METH_VARARGS|METH_KEYWORDS, "Barcode_set_code"},

    {"set_total", (PyCFunction)Barcode_set_total, METH_VARARGS|METH_KEYWORDS, "Barcode_set_total"},

    {"set_discount", (PyCFunction)Barcode_set_discount, METH_VARARGS|METH_KEYWORDS, "Barcode_set_discount"},

    {"get_equal", (PyCFunction)Barcode_equal, METH_VARARGS|METH_KEYWORDS, "Barcode_equal"},

    {"get_next", (PyCFunction)Barcode_next, METH_VARARGS|METH_KEYWORDS, "Barcode_next"},

    {"get_prev", (PyCFunction)Barcode_prev, METH_VARARGS|METH_KEYWORDS, "Barcode_prev"},

    {"get_gt", (PyCFunction)Barcode_gt, METH_VARARGS|METH_KEYWORDS, "Barcode_gt"},

    {"get_ge", (PyCFunction)Barcode_ge, METH_VARARGS|METH_KEYWORDS, "Barcode_ge"},

    {"get_lt", (PyCFunction)Barcode_lt, METH_VARARGS|METH_KEYWORDS, "Barcode_lt"},

    {"get_le", (PyCFunction)Barcode_le, METH_VARARGS|METH_KEYWORDS, "Barcode_le"},

    {"get_first", (PyCFunction)Barcode_first, METH_VARARGS|METH_KEYWORDS, "Barcode_first"},

    {"get_last", (PyCFunction)Barcode_last, METH_VARARGS|METH_KEYWORDS, "Barcode_last"},

    {"gets_equal", (PyCFunction)Barcode_iter_equal, METH_VARARGS|METH_KEYWORDS, "Barcode_iter_equal"},

    {"gets_gt", (PyCFunction)Barcode_iter_gt, METH_VARARGS|METH_KEYWORDS, "Barcode_iter_gt"},

    {"gets_ge", (PyCFunction)Barcode_iter_ge, METH_VARARGS|METH_KEYWORDS, "Barcode_iter_ge"},

    {"gets_lt", (PyCFunction)Barcode_iter_lt, METH_VARARGS|METH_KEYWORDS, "Barcode_iter_lt"},

    {"gets_le", (PyCFunction)Barcode_iter_le, METH_VARARGS|METH_KEYWORDS, "Barcode_iter_le"},

    {"gets_first", (PyCFunction)Barcode_iter_first, METH_VARARGS|METH_KEYWORDS, "Barcode_iter_first"},

    {"gets_last", (PyCFunction)Barcode_iter_last, METH_VARARGS|METH_KEYWORDS, "Barcode_iter_last"},

    {"insert", (PyCFunction)Barcode_insert, METH_VARARGS|METH_KEYWORDS, "Barcode_insert"},

    {"update", (PyCFunction)Barcode_update, METH_VARARGS|METH_KEYWORDS, "Barcode_update"},

    {"delete", (PyCFunction)Barcode_delete, METH_VARARGS|METH_KEYWORDS, "Barcode_delete"},

    {NULL}
};

static PyTypeObject BarcodeType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Barcode",             /*tp_name*/
    sizeof(Barcode),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Barcode_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Barcode_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Barcode objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Barcode_methods,             /* tp_methods */
    0,
    Barcode_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Barcode_init,      /* tp_init */
    0,                         /* tp_alloc */
    Barcode_new,                 /* tp_new */
};
PyTypeObject *getBarcodeType()
{
    return &BarcodeType_;
}

static void BarcodeKey0_dealloc(PyObject* self)
{
    do_free(((BarcodeKey0*)self)->priv);
    Py_DECREF(((BarcodeKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("BarcodeKey0 free\n");
}
static PyObject *BarcodeKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("BarcodeKey0 new\n");
    BarcodeKey0 *self;
    self = PyObject_NEW(BarcodeKey0, type);
    //self = (BarcodeKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(barcode_key0_t));
    return (PyObject *)self;
}
static int BarcodeKey0_init(BarcodeKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("BarcodeKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->barcode);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *BarcodeKey0_clone(BarcodeKey0 *self)
{
    BarcodeKey0 *result;
    result = (BarcodeKey0*)PyObject_CallFunction((PyObject*)getBarcodeKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *BarcodeKey0_barcode(BarcodeKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->barcode);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *BarcodeKey0_set_barcode(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set barcode");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->barcode, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->barcode);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *BarcodeKey0_equal(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_next(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_prev(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_gt(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_ge(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_lt(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_le(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_first(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_last(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey0_iter_equal(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.barcode, 
                 self->priv->barcode))
               break;
       }

 
        item = BarcodeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey0_iter_gt(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.barcode, 
                 self->priv->barcode))
               break;
       }

 
        item = BarcodeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey0_iter_ge(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.barcode, 
                 self->priv->barcode))
               break;
       }

 
        item = BarcodeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey0_iter_lt(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.barcode, 
                 self->priv->barcode))
               break;
       }

 
        item = BarcodeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey0_iter_le(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.barcode, 
                 self->priv->barcode))
               break;
       }

 
        item = BarcodeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey0_iter_first(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.barcode, 
                 self->priv->barcode))
               break;
       }

 
        item = BarcodeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey0_iter_last(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.barcode, 
                 self->priv->barcode))
               break;
       }

 
        item = BarcodeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey0_cmp(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getBarcodeKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->barcode, ((BarcodeKey0*)obj)->priv->barcode);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *BarcodeKey0_set_alias(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((BarcodeKey0*)self)->alias);
    Py_INCREF(obj);
    ((BarcodeKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *BarcodeKey0_set(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getBarcodeKey0Type() ) {
        memcpy(self->priv, ((BarcodeKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getBarcodeType() ) {

        do_cpy(self->priv->barcode, ((Barcode*)obj)->priv->data.barcode);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_barcode", "O", value) )
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
            if ( !strcmp("barcode", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_barcode", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"BarcodeKey0\"does not contain \"%s\"", field_name);
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

static PyObject *BarcodeKey0_fields(BarcodeKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class BarcodeKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  BarcodeKey0_barcode(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef BarcodeKey0_getset[] = {

    {"barcode",(getter)BarcodeKey0_barcode},

    {NULL}
};

static PyMethodDef BarcodeKey0_methods[] = {
    {"set", (PyCFunction)BarcodeKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)BarcodeKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)BarcodeKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)BarcodeKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_barcode", (PyCFunction)BarcodeKey0_set_barcode, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_set_barcode"},

    {"get_equal", (PyCFunction)BarcodeKey0_equal, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_equal"},

    {"get_next", (PyCFunction)BarcodeKey0_next, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_next"},

    {"get_prev", (PyCFunction)BarcodeKey0_prev, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_prev"},

    {"get_gt", (PyCFunction)BarcodeKey0_gt, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_gt"},

    {"get_ge", (PyCFunction)BarcodeKey0_ge, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_ge"},

    {"get_lt", (PyCFunction)BarcodeKey0_lt, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_lt"},

    {"get_le", (PyCFunction)BarcodeKey0_le, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_le"},

    {"get_first", (PyCFunction)BarcodeKey0_first, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_first"},

    {"get_last", (PyCFunction)BarcodeKey0_last, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_last"},

    {"gets_equal", (PyCFunction)BarcodeKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_iter_equal"},

    {"gets_gt", (PyCFunction)BarcodeKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_iter_gt"},

    {"gets_ge", (PyCFunction)BarcodeKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_iter_ge"},

    {"gets_lt", (PyCFunction)BarcodeKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_iter_lt"},

    {"gets_le", (PyCFunction)BarcodeKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_iter_le"},

    {"gets_first", (PyCFunction)BarcodeKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_iter_first"},

    {"gets_last", (PyCFunction)BarcodeKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "BarcodeKey0_iter_last"},

    {NULL}
};

static PyTypeObject BarcodeKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.BarcodeKey0",             /*tp_name*/
    sizeof(BarcodeKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)BarcodeKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)BarcodeKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "BarcodeKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    BarcodeKey0_methods,             /* tp_methods */
    0,
    BarcodeKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)BarcodeKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    BarcodeKey0_new,                 /* tp_new */
};
PyTypeObject *getBarcodeKey0Type()
{
    return &BarcodeKey0Type_;
}

static void BarcodeKey1_dealloc(PyObject* self)
{
    do_free(((BarcodeKey1*)self)->priv);
    Py_DECREF(((BarcodeKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("BarcodeKey1 free\n");
}
static PyObject *BarcodeKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("BarcodeKey1 new\n");
    BarcodeKey1 *self;
    self = PyObject_NEW(BarcodeKey1, type);
    //self = (BarcodeKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(barcode_key1_t));
    return (PyObject *)self;
}
static int BarcodeKey1_init(BarcodeKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("BarcodeKey1 init\n");
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

static PyObject *BarcodeKey1_clone(BarcodeKey1 *self)
{
    BarcodeKey1 *result;
    result = (BarcodeKey1*)PyObject_CallFunction((PyObject*)getBarcodeKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *BarcodeKey1_code(BarcodeKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *BarcodeKey1_set_code(BarcodeKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *BarcodeKey1_equal(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_next(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_prev(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_gt(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_ge(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_lt(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_le(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_first(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_last(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *BarcodeKey1_iter_equal(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = BarcodeKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey1_iter_gt(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = BarcodeKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey1_iter_ge(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = BarcodeKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey1_iter_lt(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = BarcodeKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey1_iter_le(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = BarcodeKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey1_iter_first(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = BarcodeKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey1_iter_last(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    barcode_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = BarcodeKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_barcode_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *BarcodeKey1_cmp(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getBarcodeKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((BarcodeKey1*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *BarcodeKey1_set_alias(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((BarcodeKey1*)self)->alias);
    Py_INCREF(obj);
    ((BarcodeKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *BarcodeKey1_set(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getBarcodeKey1Type() ) {
        memcpy(self->priv, ((BarcodeKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getBarcodeType() ) {

        do_cpy(self->priv->code, ((Barcode*)obj)->priv->data.code);

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
                do_log(LOG_ERR, "Class \"BarcodeKey1\"does not contain \"%s\"", field_name);
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

static PyObject *BarcodeKey1_fields(BarcodeKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class BarcodeKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  BarcodeKey1_code(self,NULL);
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

static PyGetSetDef BarcodeKey1_getset[] = {

    {"code",(getter)BarcodeKey1_code},

    {NULL}
};

static PyMethodDef BarcodeKey1_methods[] = {
    {"set", (PyCFunction)BarcodeKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)BarcodeKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)BarcodeKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)BarcodeKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)BarcodeKey1_set_code, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_set_code"},

    {"get_equal", (PyCFunction)BarcodeKey1_equal, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_equal"},

    {"get_next", (PyCFunction)BarcodeKey1_next, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_next"},

    {"get_prev", (PyCFunction)BarcodeKey1_prev, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_prev"},

    {"get_gt", (PyCFunction)BarcodeKey1_gt, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_gt"},

    {"get_ge", (PyCFunction)BarcodeKey1_ge, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_ge"},

    {"get_lt", (PyCFunction)BarcodeKey1_lt, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_lt"},

    {"get_le", (PyCFunction)BarcodeKey1_le, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_le"},

    {"get_first", (PyCFunction)BarcodeKey1_first, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_first"},

    {"get_last", (PyCFunction)BarcodeKey1_last, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_last"},

    {"gets_equal", (PyCFunction)BarcodeKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_iter_equal"},

    {"gets_gt", (PyCFunction)BarcodeKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_iter_gt"},

    {"gets_ge", (PyCFunction)BarcodeKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_iter_ge"},

    {"gets_lt", (PyCFunction)BarcodeKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_iter_lt"},

    {"gets_le", (PyCFunction)BarcodeKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_iter_le"},

    {"gets_first", (PyCFunction)BarcodeKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_iter_first"},

    {"gets_last", (PyCFunction)BarcodeKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "BarcodeKey1_iter_last"},

    {NULL}
};

static PyTypeObject BarcodeKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.BarcodeKey1",             /*tp_name*/
    sizeof(BarcodeKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)BarcodeKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)BarcodeKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "BarcodeKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    BarcodeKey1_methods,             /* tp_methods */
    0,
    BarcodeKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)BarcodeKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    BarcodeKey1_new,                 /* tp_new */
};
PyTypeObject *getBarcodeKey1Type()
{
    return &BarcodeKey1Type_;
}
