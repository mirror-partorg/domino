
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "document_data.h"

static void DocumentData_dealloc(PyObject* self)
{
    do_free(((DocumentData*)self)->priv);
    Py_DECREF(((DocumentData*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentData free\n");
}
static PyObject *DocumentData_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentData new\n");
    DocumentData *self;
    self = PyObject_NEW(DocumentData, type);
    //self = (DocumentData *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_data_rec_t));
    return (PyObject *)self;
}
static int DocumentData_init(DocumentData *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentData init\n");
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

    do_text_set_empty(self->priv->data.sklad);

    do_text_set_empty(self->priv->data.document);

    do_text_set_empty(self->priv->data.analitics);

    do_document_data_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentData_clone(DocumentData *self)
{
    DocumentData *result;
    result = (DocumentData*)PyObject_CallFunction((PyObject*)getDocumentDataType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *DocumentData_document_type(DocumentData* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentData_set_document_type(DocumentData* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentData_store(DocumentData* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentData_set_store(DocumentData* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentData_document(DocumentData* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentData_set_document(DocumentData* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentData_record_type(DocumentData* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type);

    return result;
}

static PyObject *DocumentData_set_record_type(DocumentData* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentData_number(DocumentData* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.number);

    return result;
}

static PyObject *DocumentData_set_number(DocumentData* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentData_analitics(DocumentData* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.analitics);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentData_set_analitics(DocumentData* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentData_set_param(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_document_data_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_document_data_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_document_data_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *DocumentData_set_params(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_data_params_clear(self->alias->alias,self->priv);
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
                 do_document_data_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_document_data_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_data_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *DocumentData_get_param(DocumentData* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_document_data_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *DocumentData_params_clear(DocumentData* self)
{
    do_document_data_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *DocumentData_get_params(DocumentData* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *DocumentData_prev(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_gt(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_next(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_ge(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_equal(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_last(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_lt(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_le(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_first(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentDataKey0Type() )
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() )
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() )
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_iter_gt(DocumentData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
        key_cmp = (document_data_key0_t*)do_malloc(sizeof(document_data_key0_t));
        memcpy(key_cmp, ((DocumentDataKey0*)key)->priv, sizeof(document_data_key0_t));
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
        key_cmp = (document_data_key1_t*)do_malloc(sizeof(document_data_key1_t));
        memcpy(key_cmp, ((DocumentDataKey1*)key)->priv, sizeof(document_data_key1_t));
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
        key_cmp = (document_data_key2_t*)do_malloc(sizeof(document_data_key2_t));
        memcpy(key_cmp, ((DocumentDataKey2*)key)->priv, sizeof(document_data_key2_t));
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->dtype, 
                    ((DocumentDataKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->sklad, 
                    ((DocumentDataKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->document, 
                    ((DocumentDataKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->type, 
                    ((DocumentDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->number, 
                    ((DocumentDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->dtype, 
                    ((DocumentDataKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->sklad, 
                    ((DocumentDataKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->document, 
                    ((DocumentDataKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->type, 
                    ((DocumentDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->analitics, 
                    ((DocumentDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->type, 
                    ((DocumentDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->analitics, 
                    ((DocumentDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
            status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
            status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
            status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentData_iter_ge(DocumentData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
        key_cmp = (document_data_key0_t*)do_malloc(sizeof(document_data_key0_t));
        memcpy(key_cmp, ((DocumentDataKey0*)key)->priv, sizeof(document_data_key0_t));
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
        key_cmp = (document_data_key1_t*)do_malloc(sizeof(document_data_key1_t));
        memcpy(key_cmp, ((DocumentDataKey1*)key)->priv, sizeof(document_data_key1_t));
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
        key_cmp = (document_data_key2_t*)do_malloc(sizeof(document_data_key2_t));
        memcpy(key_cmp, ((DocumentDataKey2*)key)->priv, sizeof(document_data_key2_t));
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->dtype, 
                    ((DocumentDataKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->sklad, 
                    ((DocumentDataKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->document, 
                    ((DocumentDataKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->type, 
                    ((DocumentDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->number, 
                    ((DocumentDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->dtype, 
                    ((DocumentDataKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->sklad, 
                    ((DocumentDataKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->document, 
                    ((DocumentDataKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->type, 
                    ((DocumentDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->analitics, 
                    ((DocumentDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->type, 
                    ((DocumentDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->analitics, 
                    ((DocumentDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
            status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
            status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
            status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentData_iter_equal(DocumentData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
        key_cmp = (document_data_key0_t*)do_malloc(sizeof(document_data_key0_t));
        memcpy(key_cmp, ((DocumentDataKey0*)key)->priv, sizeof(document_data_key0_t));
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
        key_cmp = (document_data_key1_t*)do_malloc(sizeof(document_data_key1_t));
        memcpy(key_cmp, ((DocumentDataKey1*)key)->priv, sizeof(document_data_key1_t));
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
        key_cmp = (document_data_key2_t*)do_malloc(sizeof(document_data_key2_t));
        memcpy(key_cmp, ((DocumentDataKey2*)key)->priv, sizeof(document_data_key2_t));
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->dtype, 
                    ((DocumentDataKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->sklad, 
                    ((DocumentDataKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->document, 
                    ((DocumentDataKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->type, 
                    ((DocumentDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->number, 
                    ((DocumentDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->dtype, 
                    ((DocumentDataKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->sklad, 
                    ((DocumentDataKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->document, 
                    ((DocumentDataKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->type, 
                    ((DocumentDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->analitics, 
                    ((DocumentDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->type, 
                    ((DocumentDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->analitics, 
                    ((DocumentDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
            status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
            status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
            status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentData_iter_last(DocumentData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
        key_cmp = (document_data_key0_t*)do_malloc(sizeof(document_data_key0_t));
        memcpy(key_cmp, ((DocumentDataKey0*)key)->priv, sizeof(document_data_key0_t));
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
        key_cmp = (document_data_key1_t*)do_malloc(sizeof(document_data_key1_t));
        memcpy(key_cmp, ((DocumentDataKey1*)key)->priv, sizeof(document_data_key1_t));
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
        key_cmp = (document_data_key2_t*)do_malloc(sizeof(document_data_key2_t));
        memcpy(key_cmp, ((DocumentDataKey2*)key)->priv, sizeof(document_data_key2_t));
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->dtype, 
                    ((DocumentDataKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->sklad, 
                    ((DocumentDataKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->document, 
                    ((DocumentDataKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->type, 
                    ((DocumentDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->number, 
                    ((DocumentDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->dtype, 
                    ((DocumentDataKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->sklad, 
                    ((DocumentDataKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->document, 
                    ((DocumentDataKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->type, 
                    ((DocumentDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->analitics, 
                    ((DocumentDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->type, 
                    ((DocumentDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->analitics, 
                    ((DocumentDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
            status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
            status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
            status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentData_iter_lt(DocumentData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
        key_cmp = (document_data_key0_t*)do_malloc(sizeof(document_data_key0_t));
        memcpy(key_cmp, ((DocumentDataKey0*)key)->priv, sizeof(document_data_key0_t));
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
        key_cmp = (document_data_key1_t*)do_malloc(sizeof(document_data_key1_t));
        memcpy(key_cmp, ((DocumentDataKey1*)key)->priv, sizeof(document_data_key1_t));
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
        key_cmp = (document_data_key2_t*)do_malloc(sizeof(document_data_key2_t));
        memcpy(key_cmp, ((DocumentDataKey2*)key)->priv, sizeof(document_data_key2_t));
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->dtype, 
                    ((DocumentDataKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->sklad, 
                    ((DocumentDataKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->document, 
                    ((DocumentDataKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->type, 
                    ((DocumentDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->number, 
                    ((DocumentDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->dtype, 
                    ((DocumentDataKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->sklad, 
                    ((DocumentDataKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->document, 
                    ((DocumentDataKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->type, 
                    ((DocumentDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->analitics, 
                    ((DocumentDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->type, 
                    ((DocumentDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->analitics, 
                    ((DocumentDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
            status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
            status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
            status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentData_iter_le(DocumentData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
        key_cmp = (document_data_key0_t*)do_malloc(sizeof(document_data_key0_t));
        memcpy(key_cmp, ((DocumentDataKey0*)key)->priv, sizeof(document_data_key0_t));
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
        key_cmp = (document_data_key1_t*)do_malloc(sizeof(document_data_key1_t));
        memcpy(key_cmp, ((DocumentDataKey1*)key)->priv, sizeof(document_data_key1_t));
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
        key_cmp = (document_data_key2_t*)do_malloc(sizeof(document_data_key2_t));
        memcpy(key_cmp, ((DocumentDataKey2*)key)->priv, sizeof(document_data_key2_t));
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->dtype, 
                    ((DocumentDataKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->sklad, 
                    ((DocumentDataKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->document, 
                    ((DocumentDataKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->type, 
                    ((DocumentDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->number, 
                    ((DocumentDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->dtype, 
                    ((DocumentDataKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->sklad, 
                    ((DocumentDataKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->document, 
                    ((DocumentDataKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->type, 
                    ((DocumentDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->analitics, 
                    ((DocumentDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->type, 
                    ((DocumentDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->analitics, 
                    ((DocumentDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
            status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
            status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
            status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentData_iter_first(DocumentData* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
        key_cmp = (document_data_key0_t*)do_malloc(sizeof(document_data_key0_t));
        memcpy(key_cmp, ((DocumentDataKey0*)key)->priv, sizeof(document_data_key0_t));
        status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
        key_cmp = (document_data_key1_t*)do_malloc(sizeof(document_data_key1_t));
        memcpy(key_cmp, ((DocumentDataKey1*)key)->priv, sizeof(document_data_key1_t));
        status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
        key_cmp = (document_data_key2_t*)do_malloc(sizeof(document_data_key2_t));
        memcpy(key_cmp, ((DocumentDataKey2*)key)->priv, sizeof(document_data_key2_t));
        status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->dtype, 
                    ((DocumentDataKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->sklad, 
                    ((DocumentDataKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->document, 
                    ((DocumentDataKey0*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->type, 
                    ((DocumentDataKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key0_t*)key_cmp)->number, 
                    ((DocumentDataKey0*)key)->priv->number))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->dtype, 
                    ((DocumentDataKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->sklad, 
                    ((DocumentDataKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->document, 
                    ((DocumentDataKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->type, 
                    ((DocumentDataKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_data_key1_t*)key_cmp)->analitics, 
                    ((DocumentDataKey1*)key)->priv->analitics))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->type, 
                    ((DocumentDataKey2*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_data_key2_t*)key_cmp)->analitics, 
                    ((DocumentDataKey2*)key)->priv->analitics))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentData_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentDataKey0Type() ) {
            status = do_document_data_get0(self->alias->alias, self->priv, ((DocumentDataKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey1Type() ) {
            status = do_document_data_get1(self->alias->alias, self->priv, ((DocumentDataKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentDataKey2Type() ) {
            status = do_document_data_get2(self->alias->alias, self->priv, ((DocumentDataKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentData_update(DocumentData* self)
{
    int status;
    status = do_document_data_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_insert(DocumentData* self)
{
    int status;
    status = do_document_data_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_delete(DocumentData* self)
{
    int status;
    status = do_document_data_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentData_set_alias(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentData*)self)->alias);
    Py_INCREF(obj);
    ((DocumentData*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentData_set(DocumentData* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentDataType() ) {
        memcpy(self->priv, ((DocumentData*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentDataKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentDataKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentDataKey0*)obj)->priv->document);

        do_cpy(self->priv->data.type, ((DocumentDataKey0*)obj)->priv->type);

        do_cpy(self->priv->data.number, ((DocumentDataKey0*)obj)->priv->number);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentDataKey1*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentDataKey1*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentDataKey1*)obj)->priv->document);

        do_cpy(self->priv->data.type, ((DocumentDataKey1*)obj)->priv->type);

        do_cpy(self->priv->data.analitics, ((DocumentDataKey1*)obj)->priv->analitics);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {

        do_cpy(self->priv->data.type, ((DocumentDataKey2*)obj)->priv->type);

        do_cpy(self->priv->data.analitics, ((DocumentDataKey2*)obj)->priv->analitics);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->data.dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->data.sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->data.document, ((Document*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey3*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentKey3*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey4*)obj)->priv->dtype);

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
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
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
                do_log(LOG_ERR, "Class \"DocumentData\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentData_fields(DocumentData* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentData");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentData_document_type(self,NULL);
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

    value =  DocumentData_store(self,NULL);
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

    value =  DocumentData_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentData_record_type(self,NULL);
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

    value =  DocumentData_number(self,NULL);
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

    value =  DocumentData_analitics(self,NULL);
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

    value = DocumentData_get_params(self, NULL);
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

static PyGetSetDef DocumentData_getset[] = {

    {"document_type",(getter)DocumentData_document_type},

    {"store",(getter)DocumentData_store},

    {"document",(getter)DocumentData_document},

    {"record_type",(getter)DocumentData_record_type},

    {"number",(getter)DocumentData_number},

    {"analitics",(getter)DocumentData_analitics},

    {"params",(getter)DocumentData_get_params},

    {NULL}
};

static PyMethodDef DocumentData_methods[] = {
    {"set", (PyCFunction)DocumentData_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentData_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentData_clone, METH_NOARGS, "clone"},

    {"set_document_type", (PyCFunction)DocumentData_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentData_set_document_type"},

    {"set_store", (PyCFunction)DocumentData_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentData_set_store"},

    {"set_document", (PyCFunction)DocumentData_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentData_set_document"},

    {"set_record_type", (PyCFunction)DocumentData_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentData_set_record_type"},

    {"set_number", (PyCFunction)DocumentData_set_number, METH_VARARGS|METH_KEYWORDS, "DocumentData_set_number"},

    {"set_analitics", (PyCFunction)DocumentData_set_analitics, METH_VARARGS|METH_KEYWORDS, "DocumentData_set_analitics"},

    {"set_param", (PyCFunction)DocumentData_set_param, METH_VARARGS|METH_KEYWORDS, "do_DocumentData_param_set"},
    {"param", (PyCFunction)DocumentData_get_param, METH_VARARGS|METH_KEYWORDS, "do_DocumentData_param"},
    {"clear_params", (PyCFunction)DocumentData_params_clear, METH_NOARGS, "do_DocumentData_param_clear"},
    {"set_params", (PyCFunction)DocumentData_set_params, METH_VARARGS|METH_KEYWORDS, "do_DocumentData_set_params"},

    {"get_prev", (PyCFunction)DocumentData_prev, METH_VARARGS|METH_KEYWORDS, "DocumentData_prev"},

    {"get_gt", (PyCFunction)DocumentData_gt, METH_VARARGS|METH_KEYWORDS, "DocumentData_gt"},

    {"get_next", (PyCFunction)DocumentData_next, METH_VARARGS|METH_KEYWORDS, "DocumentData_next"},

    {"get_ge", (PyCFunction)DocumentData_ge, METH_VARARGS|METH_KEYWORDS, "DocumentData_ge"},

    {"get_equal", (PyCFunction)DocumentData_equal, METH_VARARGS|METH_KEYWORDS, "DocumentData_equal"},

    {"get_last", (PyCFunction)DocumentData_last, METH_VARARGS|METH_KEYWORDS, "DocumentData_last"},

    {"get_lt", (PyCFunction)DocumentData_lt, METH_VARARGS|METH_KEYWORDS, "DocumentData_lt"},

    {"get_le", (PyCFunction)DocumentData_le, METH_VARARGS|METH_KEYWORDS, "DocumentData_le"},

    {"get_first", (PyCFunction)DocumentData_first, METH_VARARGS|METH_KEYWORDS, "DocumentData_first"},

    {"gets_gt", (PyCFunction)DocumentData_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentData_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentData_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentData_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentData_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentData_iter_equal"},

    {"gets_last", (PyCFunction)DocumentData_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentData_iter_last"},

    {"gets_lt", (PyCFunction)DocumentData_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentData_iter_lt"},

    {"gets_le", (PyCFunction)DocumentData_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentData_iter_le"},

    {"gets_first", (PyCFunction)DocumentData_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentData_iter_first"},

    {"update", (PyCFunction)DocumentData_update, METH_VARARGS|METH_KEYWORDS, "DocumentData_update"},

    {"insert", (PyCFunction)DocumentData_insert, METH_VARARGS|METH_KEYWORDS, "DocumentData_insert"},

    {"delete", (PyCFunction)DocumentData_delete, METH_VARARGS|METH_KEYWORDS, "DocumentData_delete"},

    {NULL}
};

static PyTypeObject DocumentDataType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentData",             /*tp_name*/
    sizeof(DocumentData),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentData_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentData_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentData objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentData_methods,             /* tp_methods */
    0,
    DocumentData_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentData_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentData_new,                 /* tp_new */
};
PyTypeObject *getDocumentDataType()
{
    return &DocumentDataType_;
}

static void DocumentDataKey0_dealloc(PyObject* self)
{
    do_free(((DocumentDataKey0*)self)->priv);
    Py_DECREF(((DocumentDataKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentDataKey0 free\n");
}
static PyObject *DocumentDataKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentDataKey0 new\n");
    DocumentDataKey0 *self;
    self = PyObject_NEW(DocumentDataKey0, type);
    //self = (DocumentDataKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_data_key0_t));
    return (PyObject *)self;
}
static int DocumentDataKey0_init(DocumentDataKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentDataKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->dtype);

    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentDataKey0_clone(DocumentDataKey0 *self)
{
    DocumentDataKey0 *result;
    result = (DocumentDataKey0*)PyObject_CallFunction((PyObject*)getDocumentDataKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentDataKey0_document_type(DocumentDataKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentDataKey0_set_document_type(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey0_store(DocumentDataKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentDataKey0_set_store(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey0_document(DocumentDataKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentDataKey0_set_document(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentDataKey0_record_type(DocumentDataKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *DocumentDataKey0_set_record_type(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey0_number(DocumentDataKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->number);

    return result;
}

static PyObject *DocumentDataKey0_set_number(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey0_prev(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_gt(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_next(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_ge(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_equal(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_last(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_lt(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_le(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_first(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey0_iter_gt(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey0_iter_ge(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey0_iter_equal(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey0_iter_last(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey0_iter_lt(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey0_iter_le(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey0_iter_first(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.number, 
                 self->priv->number))
               break;
       }

 
        item = DocumentDataKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey0_cmp(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentDataKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentDataKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentDataKey0*)obj)->priv->document);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->type - ((DocumentDataKey0*)obj)->priv->type;
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->number - ((DocumentDataKey0*)obj)->priv->number;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentDataKey0_set_alias(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentDataKey0*)self)->alias);
    Py_INCREF(obj);
    ((DocumentDataKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentDataKey0_set(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {
        memcpy(self->priv, ((DocumentDataKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

        do_cpy(self->priv->dtype, ((DocumentData*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentData*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentData*)obj)->priv->data.document);

        do_cpy(self->priv->type, ((DocumentData*)obj)->priv->data.type);

        do_cpy(self->priv->number, ((DocumentData*)obj)->priv->data.number);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                        return NULL;
                    break;

                case 4:
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
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((Document*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey3*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey3*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey4*)obj)->priv->dtype);

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
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
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
                do_log(LOG_ERR, "Class \"DocumentDataKey0\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentDataKey0_fields(DocumentDataKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentDataKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentDataKey0_document_type(self,NULL);
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

    value =  DocumentDataKey0_store(self,NULL);
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

    value =  DocumentDataKey0_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentDataKey0_record_type(self,NULL);
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

    value =  DocumentDataKey0_number(self,NULL);
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

static PyGetSetDef DocumentDataKey0_getset[] = {

    {"document_type",(getter)DocumentDataKey0_document_type},

    {"store",(getter)DocumentDataKey0_store},

    {"document",(getter)DocumentDataKey0_document},

    {"record_type",(getter)DocumentDataKey0_record_type},

    {"number",(getter)DocumentDataKey0_number},

    {NULL}
};

static PyMethodDef DocumentDataKey0_methods[] = {
    {"set", (PyCFunction)DocumentDataKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentDataKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentDataKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentDataKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentDataKey0_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_set_document_type"},

    {"set_store", (PyCFunction)DocumentDataKey0_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_set_store"},

    {"set_document", (PyCFunction)DocumentDataKey0_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_set_document"},

    {"set_record_type", (PyCFunction)DocumentDataKey0_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_set_record_type"},

    {"set_number", (PyCFunction)DocumentDataKey0_set_number, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_set_number"},

    {"get_prev", (PyCFunction)DocumentDataKey0_prev, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_prev"},

    {"get_gt", (PyCFunction)DocumentDataKey0_gt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_gt"},

    {"get_next", (PyCFunction)DocumentDataKey0_next, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_next"},

    {"get_ge", (PyCFunction)DocumentDataKey0_ge, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_ge"},

    {"get_equal", (PyCFunction)DocumentDataKey0_equal, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_equal"},

    {"get_last", (PyCFunction)DocumentDataKey0_last, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_last"},

    {"get_lt", (PyCFunction)DocumentDataKey0_lt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_lt"},

    {"get_le", (PyCFunction)DocumentDataKey0_le, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_le"},

    {"get_first", (PyCFunction)DocumentDataKey0_first, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_first"},

    {"gets_gt", (PyCFunction)DocumentDataKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentDataKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentDataKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_iter_equal"},

    {"gets_last", (PyCFunction)DocumentDataKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_iter_last"},

    {"gets_lt", (PyCFunction)DocumentDataKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_iter_lt"},

    {"gets_le", (PyCFunction)DocumentDataKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_iter_le"},

    {"gets_first", (PyCFunction)DocumentDataKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey0_iter_first"},

    {NULL}
};

static PyTypeObject DocumentDataKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentDataKey0",             /*tp_name*/
    sizeof(DocumentDataKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentDataKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentDataKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentDataKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentDataKey0_methods,             /* tp_methods */
    0,
    DocumentDataKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentDataKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentDataKey0_new,                 /* tp_new */
};
PyTypeObject *getDocumentDataKey0Type()
{
    return &DocumentDataKey0Type_;
}

static void DocumentDataKey1_dealloc(PyObject* self)
{
    do_free(((DocumentDataKey1*)self)->priv);
    Py_DECREF(((DocumentDataKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentDataKey1 free\n");
}
static PyObject *DocumentDataKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentDataKey1 new\n");
    DocumentDataKey1 *self;
    self = PyObject_NEW(DocumentDataKey1, type);
    //self = (DocumentDataKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_data_key1_t));
    return (PyObject *)self;
}
static int DocumentDataKey1_init(DocumentDataKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentDataKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->dtype);

    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->document);

    do_text_set_empty(self->priv->analitics);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentDataKey1_clone(DocumentDataKey1 *self)
{
    DocumentDataKey1 *result;
    result = (DocumentDataKey1*)PyObject_CallFunction((PyObject*)getDocumentDataKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentDataKey1_document_type(DocumentDataKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentDataKey1_set_document_type(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey1_store(DocumentDataKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentDataKey1_set_store(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey1_document(DocumentDataKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentDataKey1_set_document(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentDataKey1_record_type(DocumentDataKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *DocumentDataKey1_set_record_type(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey1_analitics(DocumentDataKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->analitics);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentDataKey1_set_analitics(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey1_prev(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_gt(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_next(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_ge(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_equal(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_last(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_lt(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_le(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_first(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey1_iter_gt(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = DocumentDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey1_iter_ge(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = DocumentDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey1_iter_equal(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = DocumentDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey1_iter_last(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = DocumentDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey1_iter_lt(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = DocumentDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey1_iter_le(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = DocumentDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey1_iter_first(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.analitics, 
                 self->priv->analitics))
               break;
       }

 
        item = DocumentDataKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey1_cmp(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentDataKey1*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentDataKey1*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentDataKey1*)obj)->priv->document);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->type - ((DocumentDataKey1*)obj)->priv->type;
       }

       if ( !res && depth >= 5 ) {
           res = do_cmp(self->priv->analitics, ((DocumentDataKey1*)obj)->priv->analitics);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentDataKey1_set_alias(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentDataKey1*)self)->alias);
    Py_INCREF(obj);
    ((DocumentDataKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentDataKey1_set(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {
        memcpy(self->priv, ((DocumentDataKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

        do_cpy(self->priv->dtype, ((DocumentData*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentData*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentData*)obj)->priv->data.document);

        do_cpy(self->priv->type, ((DocumentData*)obj)->priv->data.type);

        do_cpy(self->priv->analitics, ((DocumentData*)obj)->priv->data.analitics);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                        return NULL;
                    break;

                case 4:
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
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((Document*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey3*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentKey3*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

        do_cpy(self->priv->dtype, ((DocumentKey4*)obj)->priv->dtype);

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
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
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
                do_log(LOG_ERR, "Class \"DocumentDataKey1\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentDataKey1_fields(DocumentDataKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentDataKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentDataKey1_document_type(self,NULL);
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

    value =  DocumentDataKey1_store(self,NULL);
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

    value =  DocumentDataKey1_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentDataKey1_record_type(self,NULL);
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

    value =  DocumentDataKey1_analitics(self,NULL);
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

static PyGetSetDef DocumentDataKey1_getset[] = {

    {"document_type",(getter)DocumentDataKey1_document_type},

    {"store",(getter)DocumentDataKey1_store},

    {"document",(getter)DocumentDataKey1_document},

    {"record_type",(getter)DocumentDataKey1_record_type},

    {"analitics",(getter)DocumentDataKey1_analitics},

    {NULL}
};

static PyMethodDef DocumentDataKey1_methods[] = {
    {"set", (PyCFunction)DocumentDataKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentDataKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentDataKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentDataKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentDataKey1_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_set_document_type"},

    {"set_store", (PyCFunction)DocumentDataKey1_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_set_store"},

    {"set_document", (PyCFunction)DocumentDataKey1_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_set_document"},

    {"set_record_type", (PyCFunction)DocumentDataKey1_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_set_record_type"},

    {"set_analitics", (PyCFunction)DocumentDataKey1_set_analitics, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_set_analitics"},

    {"get_prev", (PyCFunction)DocumentDataKey1_prev, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_prev"},

    {"get_gt", (PyCFunction)DocumentDataKey1_gt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_gt"},

    {"get_next", (PyCFunction)DocumentDataKey1_next, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_next"},

    {"get_ge", (PyCFunction)DocumentDataKey1_ge, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_ge"},

    {"get_equal", (PyCFunction)DocumentDataKey1_equal, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_equal"},

    {"get_last", (PyCFunction)DocumentDataKey1_last, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_last"},

    {"get_lt", (PyCFunction)DocumentDataKey1_lt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_lt"},

    {"get_le", (PyCFunction)DocumentDataKey1_le, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_le"},

    {"get_first", (PyCFunction)DocumentDataKey1_first, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_first"},

    {"gets_gt", (PyCFunction)DocumentDataKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentDataKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentDataKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_iter_equal"},

    {"gets_last", (PyCFunction)DocumentDataKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_iter_last"},

    {"gets_lt", (PyCFunction)DocumentDataKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_iter_lt"},

    {"gets_le", (PyCFunction)DocumentDataKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_iter_le"},

    {"gets_first", (PyCFunction)DocumentDataKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey1_iter_first"},

    {NULL}
};

static PyTypeObject DocumentDataKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentDataKey1",             /*tp_name*/
    sizeof(DocumentDataKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentDataKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentDataKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentDataKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentDataKey1_methods,             /* tp_methods */
    0,
    DocumentDataKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentDataKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentDataKey1_new,                 /* tp_new */
};
PyTypeObject *getDocumentDataKey1Type()
{
    return &DocumentDataKey1Type_;
}

static void DocumentDataKey2_dealloc(PyObject* self)
{
    do_free(((DocumentDataKey2*)self)->priv);
    Py_DECREF(((DocumentDataKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentDataKey2 free\n");
}
static PyObject *DocumentDataKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentDataKey2 new\n");
    DocumentDataKey2 *self;
    self = PyObject_NEW(DocumentDataKey2, type);
    //self = (DocumentDataKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_data_key2_t));
    return (PyObject *)self;
}
static int DocumentDataKey2_init(DocumentDataKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentDataKey2 init\n");
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

static PyObject *DocumentDataKey2_clone(DocumentDataKey2 *self)
{
    DocumentDataKey2 *result;
    result = (DocumentDataKey2*)PyObject_CallFunction((PyObject*)getDocumentDataKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentDataKey2_record_type(DocumentDataKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *DocumentDataKey2_set_record_type(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey2_analitics(DocumentDataKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->analitics);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentDataKey2_set_analitics(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentDataKey2_prev(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_gt(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_next(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_ge(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_equal(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_last(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_lt(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_le(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_first(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentDataKey2_iter_gt(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_GT);
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

 
        item = DocumentDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey2_iter_ge(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_GE);
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

 
        item = DocumentDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey2_iter_equal(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
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

 
        item = DocumentDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey2_iter_last(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_LAST);
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

 
        item = DocumentDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey2_iter_lt(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_LT);
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

 
        item = DocumentDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey2_iter_le(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_LE);
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

 
        item = DocumentDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey2_iter_first(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_data_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_FIRST);
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

 
        item = DocumentDataKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_data_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentDataKey2_cmp(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->type - ((DocumentDataKey2*)obj)->priv->type;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->analitics, ((DocumentDataKey2*)obj)->priv->analitics);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentDataKey2_set_alias(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentDataKey2*)self)->alias);
    Py_INCREF(obj);
    ((DocumentDataKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentDataKey2_set(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {
        memcpy(self->priv, ((DocumentDataKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

        do_cpy(self->priv->type, ((DocumentData*)obj)->priv->data.type);

        do_cpy(self->priv->analitics, ((DocumentData*)obj)->priv->data.analitics);

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
    if ( Py_TYPE(obj) == getDocumentType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

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
                do_log(LOG_ERR, "Class \"DocumentDataKey2\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentDataKey2_fields(DocumentDataKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentDataKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentDataKey2_record_type(self,NULL);
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

    value =  DocumentDataKey2_analitics(self,NULL);
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

static PyGetSetDef DocumentDataKey2_getset[] = {

    {"record_type",(getter)DocumentDataKey2_record_type},

    {"analitics",(getter)DocumentDataKey2_analitics},

    {NULL}
};

static PyMethodDef DocumentDataKey2_methods[] = {
    {"set", (PyCFunction)DocumentDataKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentDataKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentDataKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentDataKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_record_type", (PyCFunction)DocumentDataKey2_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_set_record_type"},

    {"set_analitics", (PyCFunction)DocumentDataKey2_set_analitics, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_set_analitics"},

    {"get_prev", (PyCFunction)DocumentDataKey2_prev, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_prev"},

    {"get_gt", (PyCFunction)DocumentDataKey2_gt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_gt"},

    {"get_next", (PyCFunction)DocumentDataKey2_next, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_next"},

    {"get_ge", (PyCFunction)DocumentDataKey2_ge, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_ge"},

    {"get_equal", (PyCFunction)DocumentDataKey2_equal, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_equal"},

    {"get_last", (PyCFunction)DocumentDataKey2_last, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_last"},

    {"get_lt", (PyCFunction)DocumentDataKey2_lt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_lt"},

    {"get_le", (PyCFunction)DocumentDataKey2_le, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_le"},

    {"get_first", (PyCFunction)DocumentDataKey2_first, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_first"},

    {"gets_gt", (PyCFunction)DocumentDataKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentDataKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentDataKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_iter_equal"},

    {"gets_last", (PyCFunction)DocumentDataKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_iter_last"},

    {"gets_lt", (PyCFunction)DocumentDataKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_iter_lt"},

    {"gets_le", (PyCFunction)DocumentDataKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_iter_le"},

    {"gets_first", (PyCFunction)DocumentDataKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentDataKey2_iter_first"},

    {NULL}
};

static PyTypeObject DocumentDataKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentDataKey2",             /*tp_name*/
    sizeof(DocumentDataKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentDataKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentDataKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentDataKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentDataKey2_methods,             /* tp_methods */
    0,
    DocumentDataKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentDataKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentDataKey2_new,                 /* tp_new */
};
PyTypeObject *getDocumentDataKey2Type()
{
    return &DocumentDataKey2Type_;
}
