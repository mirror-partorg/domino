
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "document_prow.h"

static void DocumentProw_dealloc(PyObject* self)
{
    do_free(((DocumentProw*)self)->priv);
    Py_DECREF(((DocumentProw*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentProw free\n");
}
static PyObject *DocumentProw_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentProw new\n");
    DocumentProw *self;
    self = PyObject_NEW(DocumentProw, type);
    //self = (DocumentProw *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_prow_rec_t));
    return (PyObject *)self;
}
static int DocumentProw_init(DocumentProw *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentProw init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.dtype);

    do_text_set_empty(self->priv->data.dtype1);

    do_document_prow_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentProw_clone(DocumentProw *self)
{
    DocumentProw *result;
    result = (DocumentProw*)PyObject_CallFunction((PyObject*)getDocumentProwType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *DocumentProw_document_type(DocumentProw* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentProw_set_document_type(DocumentProw* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentProw_document_operation(DocumentProw* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype1);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentProw_set_document_operation(DocumentProw* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.dtype1");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.dtype1, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.dtype1);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentProw_number(DocumentProw* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.number);

    return result;
}

static PyObject *DocumentProw_set_number(DocumentProw* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentProw_set_param(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_document_prow_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_document_prow_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_document_prow_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *DocumentProw_set_params(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_prow_params_clear(self->alias->alias,self->priv);
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
                 do_document_prow_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_document_prow_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_prow_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *DocumentProw_get_param(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_document_prow_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *DocumentProw_params_clear(DocumentProw* self)
{
    do_document_prow_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *DocumentProw_get_params(DocumentProw* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *DocumentProw_equal(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_next(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_prev(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_gt(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_ge(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_lt(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_le(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_first(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_last(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentProwKey0Type() )
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_iter_equal(DocumentProw* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
        key_cmp = (document_prow_key0_t*)do_malloc(sizeof(document_prow_key0_t));
        memcpy(key_cmp, ((DocumentProwKey0*)key)->priv, sizeof(document_prow_key0_t));
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype, 
                    ((DocumentProwKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype1, 
                    ((DocumentProwKey0*)key)->priv->dtype1))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->number, 
                    ((DocumentProwKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentProw_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
            status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentProw_iter_gt(DocumentProw* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
        key_cmp = (document_prow_key0_t*)do_malloc(sizeof(document_prow_key0_t));
        memcpy(key_cmp, ((DocumentProwKey0*)key)->priv, sizeof(document_prow_key0_t));
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype, 
                    ((DocumentProwKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype1, 
                    ((DocumentProwKey0*)key)->priv->dtype1))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->number, 
                    ((DocumentProwKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentProw_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
            status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentProw_iter_ge(DocumentProw* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
        key_cmp = (document_prow_key0_t*)do_malloc(sizeof(document_prow_key0_t));
        memcpy(key_cmp, ((DocumentProwKey0*)key)->priv, sizeof(document_prow_key0_t));
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype, 
                    ((DocumentProwKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype1, 
                    ((DocumentProwKey0*)key)->priv->dtype1))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->number, 
                    ((DocumentProwKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentProw_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
            status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentProw_iter_lt(DocumentProw* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
        key_cmp = (document_prow_key0_t*)do_malloc(sizeof(document_prow_key0_t));
        memcpy(key_cmp, ((DocumentProwKey0*)key)->priv, sizeof(document_prow_key0_t));
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype, 
                    ((DocumentProwKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype1, 
                    ((DocumentProwKey0*)key)->priv->dtype1))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->number, 
                    ((DocumentProwKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentProw_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
            status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentProw_iter_le(DocumentProw* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
        key_cmp = (document_prow_key0_t*)do_malloc(sizeof(document_prow_key0_t));
        memcpy(key_cmp, ((DocumentProwKey0*)key)->priv, sizeof(document_prow_key0_t));
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype, 
                    ((DocumentProwKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype1, 
                    ((DocumentProwKey0*)key)->priv->dtype1))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->number, 
                    ((DocumentProwKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentProw_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
            status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentProw_iter_first(DocumentProw* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
        key_cmp = (document_prow_key0_t*)do_malloc(sizeof(document_prow_key0_t));
        memcpy(key_cmp, ((DocumentProwKey0*)key)->priv, sizeof(document_prow_key0_t));
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype, 
                    ((DocumentProwKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype1, 
                    ((DocumentProwKey0*)key)->priv->dtype1))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->number, 
                    ((DocumentProwKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentProw_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
            status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentProw_iter_last(DocumentProw* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
        key_cmp = (document_prow_key0_t*)do_malloc(sizeof(document_prow_key0_t));
        memcpy(key_cmp, ((DocumentProwKey0*)key)->priv, sizeof(document_prow_key0_t));
        status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype, 
                    ((DocumentProwKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->dtype1, 
                    ((DocumentProwKey0*)key)->priv->dtype1))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_prow_key0_t*)key_cmp)->number, 
                    ((DocumentProwKey0*)key)->priv->number))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentProw_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentProwKey0Type() ) {
            status = do_document_prow_get0(self->alias->alias, self->priv, ((DocumentProwKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentProw_insert(DocumentProw* self)
{
    int status;
    status = do_document_prow_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_update(DocumentProw* self)
{
    int status;
    status = do_document_prow_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_delete(DocumentProw* self)
{
    int status;
    status = do_document_prow_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProw_set_alias(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentProw*)self)->alias);
    Py_INCREF(obj);
    ((DocumentProw*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentProw_set(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentProwType() ) {
        memcpy(self->priv, ((DocumentProw*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentProwKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentProwKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.dtype1, ((DocumentProwKey0*)obj)->priv->dtype1);

        do_cpy(self->priv->data.number, ((DocumentProwKey0*)obj)->priv->number);

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
            if ( !strcmp("document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document_operation", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_operation", "O", value) )
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
                do_log(LOG_ERR, "Class \"DocumentProw\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentProw_fields(DocumentProw* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentProw");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentProw_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentProw_document_operation(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_operation", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentProw_number(self,NULL);
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

    value = DocumentProw_get_params(self, NULL);
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

static PyGetSetDef DocumentProw_getset[] = {

    {"document_type",(getter)DocumentProw_document_type},

    {"document_operation",(getter)DocumentProw_document_operation},

    {"number",(getter)DocumentProw_number},

    {"params",(getter)DocumentProw_get_params},

    {NULL}
};

static PyMethodDef DocumentProw_methods[] = {
    {"set", (PyCFunction)DocumentProw_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentProw_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentProw_clone, METH_NOARGS, "clone"},

    {"set_document_type", (PyCFunction)DocumentProw_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentProw_set_document_type"},

    {"set_document_operation", (PyCFunction)DocumentProw_set_document_operation, METH_VARARGS|METH_KEYWORDS, "DocumentProw_set_document_operation"},

    {"set_number", (PyCFunction)DocumentProw_set_number, METH_VARARGS|METH_KEYWORDS, "DocumentProw_set_number"},

    {"set_param", (PyCFunction)DocumentProw_set_param, METH_VARARGS|METH_KEYWORDS, "do_DocumentProw_param_set"},
    {"param", (PyCFunction)DocumentProw_get_param, METH_VARARGS|METH_KEYWORDS, "do_DocumentProw_param"},
    {"clear_params", (PyCFunction)DocumentProw_params_clear, METH_NOARGS, "do_DocumentProw_param_clear"},
    {"set_params", (PyCFunction)DocumentProw_set_params, METH_VARARGS|METH_KEYWORDS, "do_DocumentProw_set_params"},

    {"get_equal", (PyCFunction)DocumentProw_equal, METH_VARARGS|METH_KEYWORDS, "DocumentProw_equal"},

    {"get_next", (PyCFunction)DocumentProw_next, METH_VARARGS|METH_KEYWORDS, "DocumentProw_next"},

    {"get_prev", (PyCFunction)DocumentProw_prev, METH_VARARGS|METH_KEYWORDS, "DocumentProw_prev"},

    {"get_gt", (PyCFunction)DocumentProw_gt, METH_VARARGS|METH_KEYWORDS, "DocumentProw_gt"},

    {"get_ge", (PyCFunction)DocumentProw_ge, METH_VARARGS|METH_KEYWORDS, "DocumentProw_ge"},

    {"get_lt", (PyCFunction)DocumentProw_lt, METH_VARARGS|METH_KEYWORDS, "DocumentProw_lt"},

    {"get_le", (PyCFunction)DocumentProw_le, METH_VARARGS|METH_KEYWORDS, "DocumentProw_le"},

    {"get_first", (PyCFunction)DocumentProw_first, METH_VARARGS|METH_KEYWORDS, "DocumentProw_first"},

    {"get_last", (PyCFunction)DocumentProw_last, METH_VARARGS|METH_KEYWORDS, "DocumentProw_last"},

    {"gets_equal", (PyCFunction)DocumentProw_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentProw_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentProw_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentProw_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentProw_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentProw_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentProw_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentProw_iter_lt"},

    {"gets_le", (PyCFunction)DocumentProw_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentProw_iter_le"},

    {"gets_first", (PyCFunction)DocumentProw_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentProw_iter_first"},

    {"gets_last", (PyCFunction)DocumentProw_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentProw_iter_last"},

    {"insert", (PyCFunction)DocumentProw_insert, METH_VARARGS|METH_KEYWORDS, "DocumentProw_insert"},

    {"update", (PyCFunction)DocumentProw_update, METH_VARARGS|METH_KEYWORDS, "DocumentProw_update"},

    {"delete", (PyCFunction)DocumentProw_delete, METH_VARARGS|METH_KEYWORDS, "DocumentProw_delete"},

    {NULL}
};

static PyTypeObject DocumentProwType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentProw",             /*tp_name*/
    sizeof(DocumentProw),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentProw_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentProw_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentProw objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentProw_methods,             /* tp_methods */
    0,
    DocumentProw_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentProw_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentProw_new,                 /* tp_new */
};
PyTypeObject *getDocumentProwType()
{
    return &DocumentProwType_;
}

static void DocumentProwKey0_dealloc(PyObject* self)
{
    do_free(((DocumentProwKey0*)self)->priv);
    Py_DECREF(((DocumentProwKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentProwKey0 free\n");
}
static PyObject *DocumentProwKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentProwKey0 new\n");
    DocumentProwKey0 *self;
    self = PyObject_NEW(DocumentProwKey0, type);
    //self = (DocumentProwKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_prow_key0_t));
    return (PyObject *)self;
}
static int DocumentProwKey0_init(DocumentProwKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentProwKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->dtype);

    do_text_set_empty(self->priv->dtype1);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentProwKey0_clone(DocumentProwKey0 *self)
{
    DocumentProwKey0 *result;
    result = (DocumentProwKey0*)PyObject_CallFunction((PyObject*)getDocumentProwKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentProwKey0_document_type(DocumentProwKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentProwKey0_set_document_type(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentProwKey0_document_operation(DocumentProwKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype1);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentProwKey0_set_document_operation(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set dtype1");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->dtype1, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->dtype1);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentProwKey0_number(DocumentProwKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->number);

    return result;
}

static PyObject *DocumentProwKey0_set_number(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentProwKey0_equal(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_next(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_prev(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_gt(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_ge(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_lt(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_le(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_first(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_last(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentProwKey0_iter_equal(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_prow_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.dtype1, 
                 self->priv->dtype1))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentProwKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentProwKey0_iter_gt(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_prow_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.dtype1, 
                 self->priv->dtype1))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentProwKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentProwKey0_iter_ge(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_prow_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.dtype1, 
                 self->priv->dtype1))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentProwKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentProwKey0_iter_lt(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_prow_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.dtype1, 
                 self->priv->dtype1))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentProwKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentProwKey0_iter_le(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_prow_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.dtype1, 
                 self->priv->dtype1))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentProwKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentProwKey0_iter_first(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_prow_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.dtype1, 
                 self->priv->dtype1))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentProwKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentProwKey0_iter_last(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_prow_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.dtype1, 
                 self->priv->dtype1))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentProwKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_prow_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentProwKey0_cmp(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentProwKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentProwKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->dtype1, ((DocumentProwKey0*)obj)->priv->dtype1);
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->number - ((DocumentProwKey0*)obj)->priv->number;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentProwKey0_set_alias(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentProwKey0*)self)->alias);
    Py_INCREF(obj);
    ((DocumentProwKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentProwKey0_set(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentProwKey0Type() ) {
        memcpy(self->priv, ((DocumentProwKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentProwType() ) {

        do_cpy(self->priv->dtype, ((DocumentProw*)obj)->priv->data.dtype);

        do_cpy(self->priv->dtype1, ((DocumentProw*)obj)->priv->data.dtype1);

        do_cpy(self->priv->number, ((DocumentProw*)obj)->priv->data.number);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document_operation", "O", value) )
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
            if ( !strcmp("document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document_operation", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_operation", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("number", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_number", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentProwKey0\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentProwKey0_fields(DocumentProwKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentProwKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentProwKey0_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentProwKey0_document_operation(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_operation", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentProwKey0_number(self,NULL);
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

static PyGetSetDef DocumentProwKey0_getset[] = {

    {"document_type",(getter)DocumentProwKey0_document_type},

    {"document_operation",(getter)DocumentProwKey0_document_operation},

    {"number",(getter)DocumentProwKey0_number},

    {NULL}
};

static PyMethodDef DocumentProwKey0_methods[] = {
    {"set", (PyCFunction)DocumentProwKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentProwKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentProwKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentProwKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentProwKey0_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_set_document_type"},

    {"set_document_operation", (PyCFunction)DocumentProwKey0_set_document_operation, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_set_document_operation"},

    {"set_number", (PyCFunction)DocumentProwKey0_set_number, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_set_number"},

    {"get_equal", (PyCFunction)DocumentProwKey0_equal, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_equal"},

    {"get_next", (PyCFunction)DocumentProwKey0_next, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_next"},

    {"get_prev", (PyCFunction)DocumentProwKey0_prev, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_prev"},

    {"get_gt", (PyCFunction)DocumentProwKey0_gt, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_gt"},

    {"get_ge", (PyCFunction)DocumentProwKey0_ge, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_ge"},

    {"get_lt", (PyCFunction)DocumentProwKey0_lt, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_lt"},

    {"get_le", (PyCFunction)DocumentProwKey0_le, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_le"},

    {"get_first", (PyCFunction)DocumentProwKey0_first, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_first"},

    {"get_last", (PyCFunction)DocumentProwKey0_last, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_last"},

    {"gets_equal", (PyCFunction)DocumentProwKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentProwKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentProwKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentProwKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_iter_lt"},

    {"gets_le", (PyCFunction)DocumentProwKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_iter_le"},

    {"gets_first", (PyCFunction)DocumentProwKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_iter_first"},

    {"gets_last", (PyCFunction)DocumentProwKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentProwKey0_iter_last"},

    {NULL}
};

static PyTypeObject DocumentProwKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentProwKey0",             /*tp_name*/
    sizeof(DocumentProwKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentProwKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentProwKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentProwKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentProwKey0_methods,             /* tp_methods */
    0,
    DocumentProwKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentProwKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentProwKey0_new,                 /* tp_new */
};
PyTypeObject *getDocumentProwKey0Type()
{
    return &DocumentProwKey0Type_;
}
