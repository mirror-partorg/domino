
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "document_type.h"

static void DocumentType_dealloc(PyObject* self)
{
    do_free(((DocumentType*)self)->priv);
    Py_DECREF(((DocumentType*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentType free\n");
}
static PyObject *DocumentType_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentType new\n");
    DocumentType *self;
    self = PyObject_NEW(DocumentType, type);
    //self = (DocumentType *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_type_rec_t));
    return (PyObject *)self;
}
static int DocumentType_init(DocumentType *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentType init\n");
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

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.sname);

    do_text_set_empty(self->priv->data.prices);

    do_text_set_empty(self->priv->data.print);

    do_text_set_empty(self->priv->data.form);

    do_text_set_empty(self->priv->data.accept_cmd);

    do_text_set_empty(self->priv->data.deaccept_cmd);

    do_text_set_empty(self->priv->data.c_dtypes);

    do_text_set_empty(self->priv->data.d_param);

    do_document_type_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentType_clone(DocumentType *self)
{
    DocumentType *result;
    result = (DocumentType*)PyObject_CallFunction((PyObject*)getDocumentTypeType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *DocumentType_document_type(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_document_type(DocumentType* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentType_document_operation(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype1);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_document_operation(DocumentType* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentType_name(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_name(DocumentType* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentType_short_name(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sname);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_short_name(DocumentType* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sname");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.sname, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.sname);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentType_prices(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.prices);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_prices(DocumentType* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.prices");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.prices, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.prices);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentType_print(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.print);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_print(DocumentType* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.print");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.print, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.print);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentType_form(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.form);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_form(DocumentType* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.form");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.form, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.form);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentType_accept_cmd(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.accept_cmd);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_accept_cmd(DocumentType* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.accept_cmd");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.accept_cmd, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.accept_cmd);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentType_deaccept_cmd(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.deaccept_cmd);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_deaccept_cmd(DocumentType* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.deaccept_cmd");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.deaccept_cmd, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.deaccept_cmd);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentType_c_dtypes(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.c_dtypes);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_c_dtypes(DocumentType* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.c_dtypes");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.c_dtypes, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.c_dtypes);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentType_document_param(DocumentType* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.d_param);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentType_set_document_param(DocumentType* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.d_param");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.d_param, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.d_param);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentType_set_param(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_document_type_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_document_type_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_document_type_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *DocumentType_set_params(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_type_params_clear(self->alias->alias,self->priv);
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
                 do_document_type_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_document_type_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_type_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *DocumentType_get_param(DocumentType* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_document_type_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *DocumentType_params_clear(DocumentType* self)
{
    do_document_type_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *DocumentType_get_params(DocumentType* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *DocumentType_equal(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_next(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_prev(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_gt(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_ge(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_lt(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_le(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_first(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_last(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() )
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_iter_equal(DocumentType* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
        key_cmp = (document_type_key0_t*)do_malloc(sizeof(document_type_key0_t));
        memcpy(key_cmp, ((DocumentTypeKey0*)key)->priv, sizeof(document_type_key0_t));
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype, 
                    ((DocumentTypeKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype1, 
                    ((DocumentTypeKey0*)key)->priv->dtype1))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentType_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
            status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentType_iter_gt(DocumentType* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
        key_cmp = (document_type_key0_t*)do_malloc(sizeof(document_type_key0_t));
        memcpy(key_cmp, ((DocumentTypeKey0*)key)->priv, sizeof(document_type_key0_t));
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype, 
                    ((DocumentTypeKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype1, 
                    ((DocumentTypeKey0*)key)->priv->dtype1))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentType_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
            status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentType_iter_ge(DocumentType* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
        key_cmp = (document_type_key0_t*)do_malloc(sizeof(document_type_key0_t));
        memcpy(key_cmp, ((DocumentTypeKey0*)key)->priv, sizeof(document_type_key0_t));
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype, 
                    ((DocumentTypeKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype1, 
                    ((DocumentTypeKey0*)key)->priv->dtype1))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentType_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
            status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentType_iter_lt(DocumentType* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
        key_cmp = (document_type_key0_t*)do_malloc(sizeof(document_type_key0_t));
        memcpy(key_cmp, ((DocumentTypeKey0*)key)->priv, sizeof(document_type_key0_t));
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype, 
                    ((DocumentTypeKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype1, 
                    ((DocumentTypeKey0*)key)->priv->dtype1))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentType_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
            status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentType_iter_le(DocumentType* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
        key_cmp = (document_type_key0_t*)do_malloc(sizeof(document_type_key0_t));
        memcpy(key_cmp, ((DocumentTypeKey0*)key)->priv, sizeof(document_type_key0_t));
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype, 
                    ((DocumentTypeKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype1, 
                    ((DocumentTypeKey0*)key)->priv->dtype1))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentType_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
            status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentType_iter_first(DocumentType* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
        key_cmp = (document_type_key0_t*)do_malloc(sizeof(document_type_key0_t));
        memcpy(key_cmp, ((DocumentTypeKey0*)key)->priv, sizeof(document_type_key0_t));
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype, 
                    ((DocumentTypeKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype1, 
                    ((DocumentTypeKey0*)key)->priv->dtype1))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentType_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
            status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentType_iter_last(DocumentType* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
        key_cmp = (document_type_key0_t*)do_malloc(sizeof(document_type_key0_t));
        memcpy(key_cmp, ((DocumentTypeKey0*)key)->priv, sizeof(document_type_key0_t));
        status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype, 
                    ((DocumentTypeKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_type_key0_t*)key_cmp)->dtype1, 
                    ((DocumentTypeKey0*)key)->priv->dtype1))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentType_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentTypeKey0Type() ) {
            status = do_document_type_get0(self->alias->alias, self->priv, ((DocumentTypeKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentType_insert(DocumentType* self)
{
    int status;
    status = do_document_type_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_update(DocumentType* self)
{
    int status;
    status = do_document_type_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_delete(DocumentType* self)
{
    int status;
    status = do_document_type_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentType_set_alias(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentType*)self)->alias);
    Py_INCREF(obj);
    ((DocumentType*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentType_set(DocumentType* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentTypeType() ) {
        memcpy(self->priv, ((DocumentType*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentTypeKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentTypeKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.dtype1, ((DocumentTypeKey0*)obj)->priv->dtype1);

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
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("short_name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_short_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("prices", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_prices", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("print", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_print", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("form", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_form", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("accept_cmd", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_accept_cmd", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("deaccept_cmd", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_deaccept_cmd", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("c_dtypes", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_c_dtypes", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document_param", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_param", "O", value) )
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
                do_log(LOG_ERR, "Class \"DocumentType\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentType_fields(DocumentType* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentType");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentType_document_type(self,NULL);
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

    value =  DocumentType_document_operation(self,NULL);
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

    value =  DocumentType_name(self,NULL);
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

    value =  DocumentType_short_name(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("short_name", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentType_prices(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("prices", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentType_print(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("print", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentType_form(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("form", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentType_accept_cmd(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("accept_cmd", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentType_deaccept_cmd(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("deaccept_cmd", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentType_c_dtypes(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("c_dtypes", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentType_document_param(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("document_param", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = DocumentType_get_params(self, NULL);
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

static PyGetSetDef DocumentType_getset[] = {

    {"document_type",(getter)DocumentType_document_type},

    {"document_operation",(getter)DocumentType_document_operation},

    {"name",(getter)DocumentType_name},

    {"short_name",(getter)DocumentType_short_name},

    {"prices",(getter)DocumentType_prices},

    {"print",(getter)DocumentType_print},

    {"form",(getter)DocumentType_form},

    {"accept_cmd",(getter)DocumentType_accept_cmd},

    {"deaccept_cmd",(getter)DocumentType_deaccept_cmd},

    {"c_dtypes",(getter)DocumentType_c_dtypes},

    {"document_param",(getter)DocumentType_document_param},

    {"params",(getter)DocumentType_get_params},

    {NULL}
};

static PyMethodDef DocumentType_methods[] = {
    {"set", (PyCFunction)DocumentType_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentType_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentType_clone, METH_NOARGS, "clone"},

    {"set_document_type", (PyCFunction)DocumentType_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_document_type"},

    {"set_document_operation", (PyCFunction)DocumentType_set_document_operation, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_document_operation"},

    {"set_name", (PyCFunction)DocumentType_set_name, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_name"},

    {"set_short_name", (PyCFunction)DocumentType_set_short_name, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_short_name"},

    {"set_prices", (PyCFunction)DocumentType_set_prices, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_prices"},

    {"set_print", (PyCFunction)DocumentType_set_print, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_print"},

    {"set_form", (PyCFunction)DocumentType_set_form, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_form"},

    {"set_accept_cmd", (PyCFunction)DocumentType_set_accept_cmd, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_accept_cmd"},

    {"set_deaccept_cmd", (PyCFunction)DocumentType_set_deaccept_cmd, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_deaccept_cmd"},

    {"set_c_dtypes", (PyCFunction)DocumentType_set_c_dtypes, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_c_dtypes"},

    {"set_document_param", (PyCFunction)DocumentType_set_document_param, METH_VARARGS|METH_KEYWORDS, "DocumentType_set_document_param"},

    {"set_param", (PyCFunction)DocumentType_set_param, METH_VARARGS|METH_KEYWORDS, "do_DocumentType_param_set"},
    {"param", (PyCFunction)DocumentType_get_param, METH_VARARGS|METH_KEYWORDS, "do_DocumentType_param"},
    {"clear_params", (PyCFunction)DocumentType_params_clear, METH_NOARGS, "do_DocumentType_param_clear"},
    {"set_params", (PyCFunction)DocumentType_set_params, METH_VARARGS|METH_KEYWORDS, "do_DocumentType_set_params"},

    {"get_equal", (PyCFunction)DocumentType_equal, METH_VARARGS|METH_KEYWORDS, "DocumentType_equal"},

    {"get_next", (PyCFunction)DocumentType_next, METH_VARARGS|METH_KEYWORDS, "DocumentType_next"},

    {"get_prev", (PyCFunction)DocumentType_prev, METH_VARARGS|METH_KEYWORDS, "DocumentType_prev"},

    {"get_gt", (PyCFunction)DocumentType_gt, METH_VARARGS|METH_KEYWORDS, "DocumentType_gt"},

    {"get_ge", (PyCFunction)DocumentType_ge, METH_VARARGS|METH_KEYWORDS, "DocumentType_ge"},

    {"get_lt", (PyCFunction)DocumentType_lt, METH_VARARGS|METH_KEYWORDS, "DocumentType_lt"},

    {"get_le", (PyCFunction)DocumentType_le, METH_VARARGS|METH_KEYWORDS, "DocumentType_le"},

    {"get_first", (PyCFunction)DocumentType_first, METH_VARARGS|METH_KEYWORDS, "DocumentType_first"},

    {"get_last", (PyCFunction)DocumentType_last, METH_VARARGS|METH_KEYWORDS, "DocumentType_last"},

    {"gets_equal", (PyCFunction)DocumentType_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentType_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentType_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentType_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentType_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentType_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentType_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentType_iter_lt"},

    {"gets_le", (PyCFunction)DocumentType_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentType_iter_le"},

    {"gets_first", (PyCFunction)DocumentType_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentType_iter_first"},

    {"gets_last", (PyCFunction)DocumentType_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentType_iter_last"},

    {"insert", (PyCFunction)DocumentType_insert, METH_VARARGS|METH_KEYWORDS, "DocumentType_insert"},

    {"update", (PyCFunction)DocumentType_update, METH_VARARGS|METH_KEYWORDS, "DocumentType_update"},

    {"delete", (PyCFunction)DocumentType_delete, METH_VARARGS|METH_KEYWORDS, "DocumentType_delete"},

    {NULL}
};

static PyTypeObject DocumentTypeType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentType",             /*tp_name*/
    sizeof(DocumentType),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentType_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentType_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentType objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentType_methods,             /* tp_methods */
    0,
    DocumentType_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentType_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentType_new,                 /* tp_new */
};
PyTypeObject *getDocumentTypeType()
{
    return &DocumentTypeType_;
}

static void DocumentTypeKey0_dealloc(PyObject* self)
{
    do_free(((DocumentTypeKey0*)self)->priv);
    Py_DECREF(((DocumentTypeKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentTypeKey0 free\n");
}
static PyObject *DocumentTypeKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentTypeKey0 new\n");
    DocumentTypeKey0 *self;
    self = PyObject_NEW(DocumentTypeKey0, type);
    //self = (DocumentTypeKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_type_key0_t));
    return (PyObject *)self;
}
static int DocumentTypeKey0_init(DocumentTypeKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentTypeKey0 init\n");
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

static PyObject *DocumentTypeKey0_clone(DocumentTypeKey0 *self)
{
    DocumentTypeKey0 *result;
    result = (DocumentTypeKey0*)PyObject_CallFunction((PyObject*)getDocumentTypeKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentTypeKey0_document_type(DocumentTypeKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentTypeKey0_set_document_type(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentTypeKey0_document_operation(DocumentTypeKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype1);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentTypeKey0_set_document_operation(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentTypeKey0_equal(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_next(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_prev(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_gt(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_ge(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_lt(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_le(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_first(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_last(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentTypeKey0_iter_equal(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_type_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
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

 
        item = DocumentTypeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentTypeKey0_iter_gt(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_type_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_GT);
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

 
        item = DocumentTypeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentTypeKey0_iter_ge(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_type_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_GE);
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

 
        item = DocumentTypeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentTypeKey0_iter_lt(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_type_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_LT);
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

 
        item = DocumentTypeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentTypeKey0_iter_le(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_type_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_LE);
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

 
        item = DocumentTypeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentTypeKey0_iter_first(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_type_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_FIRST);
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

 
        item = DocumentTypeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentTypeKey0_iter_last(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_type_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_LAST);
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

 
        item = DocumentTypeKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_type_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentTypeKey0_cmp(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentTypeKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentTypeKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->dtype1, ((DocumentTypeKey0*)obj)->priv->dtype1);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentTypeKey0_set_alias(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentTypeKey0*)self)->alias);
    Py_INCREF(obj);
    ((DocumentTypeKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentTypeKey0_set(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentTypeKey0Type() ) {
        memcpy(self->priv, ((DocumentTypeKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentTypeType() ) {

        do_cpy(self->priv->dtype, ((DocumentType*)obj)->priv->data.dtype);

        do_cpy(self->priv->dtype1, ((DocumentType*)obj)->priv->data.dtype1);

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

            else {
                do_log(LOG_ERR, "Class \"DocumentTypeKey0\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentTypeKey0_fields(DocumentTypeKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentTypeKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentTypeKey0_document_type(self,NULL);
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

    value =  DocumentTypeKey0_document_operation(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef DocumentTypeKey0_getset[] = {

    {"document_type",(getter)DocumentTypeKey0_document_type},

    {"document_operation",(getter)DocumentTypeKey0_document_operation},

    {NULL}
};

static PyMethodDef DocumentTypeKey0_methods[] = {
    {"set", (PyCFunction)DocumentTypeKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentTypeKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentTypeKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentTypeKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentTypeKey0_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_set_document_type"},

    {"set_document_operation", (PyCFunction)DocumentTypeKey0_set_document_operation, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_set_document_operation"},

    {"get_equal", (PyCFunction)DocumentTypeKey0_equal, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_equal"},

    {"get_next", (PyCFunction)DocumentTypeKey0_next, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_next"},

    {"get_prev", (PyCFunction)DocumentTypeKey0_prev, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_prev"},

    {"get_gt", (PyCFunction)DocumentTypeKey0_gt, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_gt"},

    {"get_ge", (PyCFunction)DocumentTypeKey0_ge, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_ge"},

    {"get_lt", (PyCFunction)DocumentTypeKey0_lt, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_lt"},

    {"get_le", (PyCFunction)DocumentTypeKey0_le, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_le"},

    {"get_first", (PyCFunction)DocumentTypeKey0_first, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_first"},

    {"get_last", (PyCFunction)DocumentTypeKey0_last, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_last"},

    {"gets_equal", (PyCFunction)DocumentTypeKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentTypeKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentTypeKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentTypeKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_iter_lt"},

    {"gets_le", (PyCFunction)DocumentTypeKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_iter_le"},

    {"gets_first", (PyCFunction)DocumentTypeKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_iter_first"},

    {"gets_last", (PyCFunction)DocumentTypeKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentTypeKey0_iter_last"},

    {NULL}
};

static PyTypeObject DocumentTypeKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentTypeKey0",             /*tp_name*/
    sizeof(DocumentTypeKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentTypeKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentTypeKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentTypeKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentTypeKey0_methods,             /* tp_methods */
    0,
    DocumentTypeKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentTypeKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentTypeKey0_new,                 /* tp_new */
};
PyTypeObject *getDocumentTypeKey0Type()
{
    return &DocumentTypeKey0Type_;
}
