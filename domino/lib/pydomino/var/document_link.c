
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "document_link.h"

static void DocumentLink_dealloc(PyObject* self)
{
    do_free(((DocumentLink*)self)->priv);
    Py_DECREF(((DocumentLink*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentLink free\n");
}
static PyObject *DocumentLink_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentLink new\n");
    DocumentLink *self;
    self = PyObject_NEW(DocumentLink, type);
    //self = (DocumentLink *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_link_rec_t));
    return (PyObject *)self;
}
static int DocumentLink_init(DocumentLink *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentLink init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.m_dtype);

    do_text_set_empty(self->priv->data.m_sklad);

    do_text_set_empty(self->priv->data.m_document);

    do_text_set_empty(self->priv->data.s_dtype);

    do_text_set_empty(self->priv->data.s_sklad);

    do_text_set_empty(self->priv->data.s_document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentLink_clone(DocumentLink *self)
{
    DocumentLink *result;
    result = (DocumentLink*)PyObject_CallFunction((PyObject*)getDocumentLinkType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *DocumentLink_record_type(DocumentLink* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type);

    return result;
}

static PyObject *DocumentLink_set_record_type(DocumentLink* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentLink_master_document_type(DocumentLink* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.m_dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLink_set_master_document_type(DocumentLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.m_dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.m_dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.m_dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLink_master_store(DocumentLink* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.m_sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLink_set_master_store(DocumentLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.m_sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.m_sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.m_sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLink_master_document(DocumentLink* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.m_document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLink_set_master_document(DocumentLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.m_document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.m_document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.m_document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLink_slave_document_type(DocumentLink* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.s_dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLink_set_slave_document_type(DocumentLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.s_dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.s_dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.s_dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLink_slave_store(DocumentLink* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.s_sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLink_set_slave_store(DocumentLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.s_sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.s_sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.s_sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLink_slave_document(DocumentLink* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.s_document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLink_set_slave_document(DocumentLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.s_document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.s_document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.s_document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLink_sort(DocumentLink* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.sort);

    return result;
}

static PyObject *DocumentLink_set_sort(DocumentLink* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sort");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.sort = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.sort = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sort");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.sort);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLink_gt(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_last(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_next(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_le(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_lt(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_equal(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_ge(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_prev(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_first(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() )
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() )
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_iter_gt(DocumentLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
        key_cmp = (document_link_key0_t*)do_malloc(sizeof(document_link_key0_t));
        memcpy(key_cmp, ((DocumentLinkKey0*)key)->priv, sizeof(document_link_key0_t));
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
        key_cmp = (document_link_key1_t*)do_malloc(sizeof(document_link_key1_t));
        memcpy(key_cmp, ((DocumentLinkKey1*)key)->priv, sizeof(document_link_key1_t));
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_dtype, 
                    ((DocumentLinkKey0*)key)->priv->m_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_sklad, 
                    ((DocumentLinkKey0*)key)->priv->m_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_document, 
                    ((DocumentLinkKey0*)key)->priv->m_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->type, 
                    ((DocumentLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->sort, 
                    ((DocumentLinkKey0*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_dtype, 
                    ((DocumentLinkKey1*)key)->priv->s_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_sklad, 
                    ((DocumentLinkKey1*)key)->priv->s_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_document, 
                    ((DocumentLinkKey1*)key)->priv->s_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->type, 
                    ((DocumentLinkKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->sort, 
                    ((DocumentLinkKey1*)key)->priv->sort))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
            status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
            status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentLink_iter_last(DocumentLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
        key_cmp = (document_link_key0_t*)do_malloc(sizeof(document_link_key0_t));
        memcpy(key_cmp, ((DocumentLinkKey0*)key)->priv, sizeof(document_link_key0_t));
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
        key_cmp = (document_link_key1_t*)do_malloc(sizeof(document_link_key1_t));
        memcpy(key_cmp, ((DocumentLinkKey1*)key)->priv, sizeof(document_link_key1_t));
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_dtype, 
                    ((DocumentLinkKey0*)key)->priv->m_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_sklad, 
                    ((DocumentLinkKey0*)key)->priv->m_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_document, 
                    ((DocumentLinkKey0*)key)->priv->m_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->type, 
                    ((DocumentLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->sort, 
                    ((DocumentLinkKey0*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_dtype, 
                    ((DocumentLinkKey1*)key)->priv->s_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_sklad, 
                    ((DocumentLinkKey1*)key)->priv->s_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_document, 
                    ((DocumentLinkKey1*)key)->priv->s_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->type, 
                    ((DocumentLinkKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->sort, 
                    ((DocumentLinkKey1*)key)->priv->sort))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
            status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
            status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentLink_iter_le(DocumentLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
        key_cmp = (document_link_key0_t*)do_malloc(sizeof(document_link_key0_t));
        memcpy(key_cmp, ((DocumentLinkKey0*)key)->priv, sizeof(document_link_key0_t));
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
        key_cmp = (document_link_key1_t*)do_malloc(sizeof(document_link_key1_t));
        memcpy(key_cmp, ((DocumentLinkKey1*)key)->priv, sizeof(document_link_key1_t));
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_dtype, 
                    ((DocumentLinkKey0*)key)->priv->m_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_sklad, 
                    ((DocumentLinkKey0*)key)->priv->m_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_document, 
                    ((DocumentLinkKey0*)key)->priv->m_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->type, 
                    ((DocumentLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->sort, 
                    ((DocumentLinkKey0*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_dtype, 
                    ((DocumentLinkKey1*)key)->priv->s_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_sklad, 
                    ((DocumentLinkKey1*)key)->priv->s_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_document, 
                    ((DocumentLinkKey1*)key)->priv->s_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->type, 
                    ((DocumentLinkKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->sort, 
                    ((DocumentLinkKey1*)key)->priv->sort))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
            status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
            status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentLink_iter_ge(DocumentLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
        key_cmp = (document_link_key0_t*)do_malloc(sizeof(document_link_key0_t));
        memcpy(key_cmp, ((DocumentLinkKey0*)key)->priv, sizeof(document_link_key0_t));
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
        key_cmp = (document_link_key1_t*)do_malloc(sizeof(document_link_key1_t));
        memcpy(key_cmp, ((DocumentLinkKey1*)key)->priv, sizeof(document_link_key1_t));
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_dtype, 
                    ((DocumentLinkKey0*)key)->priv->m_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_sklad, 
                    ((DocumentLinkKey0*)key)->priv->m_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_document, 
                    ((DocumentLinkKey0*)key)->priv->m_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->type, 
                    ((DocumentLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->sort, 
                    ((DocumentLinkKey0*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_dtype, 
                    ((DocumentLinkKey1*)key)->priv->s_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_sklad, 
                    ((DocumentLinkKey1*)key)->priv->s_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_document, 
                    ((DocumentLinkKey1*)key)->priv->s_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->type, 
                    ((DocumentLinkKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->sort, 
                    ((DocumentLinkKey1*)key)->priv->sort))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
            status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
            status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentLink_iter_equal(DocumentLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
        key_cmp = (document_link_key0_t*)do_malloc(sizeof(document_link_key0_t));
        memcpy(key_cmp, ((DocumentLinkKey0*)key)->priv, sizeof(document_link_key0_t));
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
        key_cmp = (document_link_key1_t*)do_malloc(sizeof(document_link_key1_t));
        memcpy(key_cmp, ((DocumentLinkKey1*)key)->priv, sizeof(document_link_key1_t));
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_dtype, 
                    ((DocumentLinkKey0*)key)->priv->m_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_sklad, 
                    ((DocumentLinkKey0*)key)->priv->m_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_document, 
                    ((DocumentLinkKey0*)key)->priv->m_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->type, 
                    ((DocumentLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->sort, 
                    ((DocumentLinkKey0*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_dtype, 
                    ((DocumentLinkKey1*)key)->priv->s_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_sklad, 
                    ((DocumentLinkKey1*)key)->priv->s_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_document, 
                    ((DocumentLinkKey1*)key)->priv->s_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->type, 
                    ((DocumentLinkKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->sort, 
                    ((DocumentLinkKey1*)key)->priv->sort))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
            status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
            status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentLink_iter_lt(DocumentLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
        key_cmp = (document_link_key0_t*)do_malloc(sizeof(document_link_key0_t));
        memcpy(key_cmp, ((DocumentLinkKey0*)key)->priv, sizeof(document_link_key0_t));
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
        key_cmp = (document_link_key1_t*)do_malloc(sizeof(document_link_key1_t));
        memcpy(key_cmp, ((DocumentLinkKey1*)key)->priv, sizeof(document_link_key1_t));
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_dtype, 
                    ((DocumentLinkKey0*)key)->priv->m_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_sklad, 
                    ((DocumentLinkKey0*)key)->priv->m_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_document, 
                    ((DocumentLinkKey0*)key)->priv->m_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->type, 
                    ((DocumentLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->sort, 
                    ((DocumentLinkKey0*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_dtype, 
                    ((DocumentLinkKey1*)key)->priv->s_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_sklad, 
                    ((DocumentLinkKey1*)key)->priv->s_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_document, 
                    ((DocumentLinkKey1*)key)->priv->s_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->type, 
                    ((DocumentLinkKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->sort, 
                    ((DocumentLinkKey1*)key)->priv->sort))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
            status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
            status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentLink_iter_first(DocumentLink* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
        key_cmp = (document_link_key0_t*)do_malloc(sizeof(document_link_key0_t));
        memcpy(key_cmp, ((DocumentLinkKey0*)key)->priv, sizeof(document_link_key0_t));
        status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
        key_cmp = (document_link_key1_t*)do_malloc(sizeof(document_link_key1_t));
        memcpy(key_cmp, ((DocumentLinkKey1*)key)->priv, sizeof(document_link_key1_t));
        status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_dtype, 
                    ((DocumentLinkKey0*)key)->priv->m_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_sklad, 
                    ((DocumentLinkKey0*)key)->priv->m_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->m_document, 
                    ((DocumentLinkKey0*)key)->priv->m_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->type, 
                    ((DocumentLinkKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key0_t*)key_cmp)->sort, 
                    ((DocumentLinkKey0*)key)->priv->sort))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_dtype, 
                    ((DocumentLinkKey1*)key)->priv->s_dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_sklad, 
                    ((DocumentLinkKey1*)key)->priv->s_sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->s_document, 
                    ((DocumentLinkKey1*)key)->priv->s_document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->type, 
                    ((DocumentLinkKey1*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_link_key1_t*)key_cmp)->sort, 
                    ((DocumentLinkKey1*)key)->priv->sort))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentLink_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentLinkKey0Type() ) {
            status = do_document_link_get0(self->alias->alias, self->priv, ((DocumentLinkKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentLinkKey1Type() ) {
            status = do_document_link_get1(self->alias->alias, self->priv, ((DocumentLinkKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentLink_insert(DocumentLink* self)
{
    int status;
    status = do_document_link_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_update(DocumentLink* self)
{
    int status;
    status = do_document_link_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_delete(DocumentLink* self)
{
    int status;
    status = do_document_link_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLink_set_alias(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentLink*)self)->alias);
    Py_INCREF(obj);
    ((DocumentLink*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentLink_set(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentLinkType() ) {
        memcpy(self->priv, ((DocumentLink*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentLinkKey0Type() ) {

        do_cpy(self->priv->data.type, ((DocumentLinkKey0*)obj)->priv->type);

        do_cpy(self->priv->data.m_dtype, ((DocumentLinkKey0*)obj)->priv->m_dtype);

        do_cpy(self->priv->data.m_sklad, ((DocumentLinkKey0*)obj)->priv->m_sklad);

        do_cpy(self->priv->data.m_document, ((DocumentLinkKey0*)obj)->priv->m_document);

        do_cpy(self->priv->data.sort, ((DocumentLinkKey0*)obj)->priv->sort);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentLinkKey1Type() ) {

        do_cpy(self->priv->data.type, ((DocumentLinkKey1*)obj)->priv->type);

        do_cpy(self->priv->data.s_dtype, ((DocumentLinkKey1*)obj)->priv->s_dtype);

        do_cpy(self->priv->data.s_sklad, ((DocumentLinkKey1*)obj)->priv->s_sklad);

        do_cpy(self->priv->data.s_document, ((DocumentLinkKey1*)obj)->priv->s_document);

        do_cpy(self->priv->data.sort, ((DocumentLinkKey1*)obj)->priv->sort);

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
            if ( !strcmp("master_document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_master_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("master_store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_master_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("master_document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_master_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("slave_document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_slave_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("slave_store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_slave_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("slave_document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_slave_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentLink\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentLink_fields(DocumentLink* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentLink");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentLink_record_type(self,NULL);
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

    value =  DocumentLink_master_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("master_document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLink_master_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("master_store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLink_master_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("master_document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLink_slave_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("slave_document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLink_slave_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("slave_store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLink_slave_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("slave_document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLink_sort(self,NULL);
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

static PyGetSetDef DocumentLink_getset[] = {

    {"record_type",(getter)DocumentLink_record_type},

    {"master_document_type",(getter)DocumentLink_master_document_type},

    {"master_store",(getter)DocumentLink_master_store},

    {"master_document",(getter)DocumentLink_master_document},

    {"slave_document_type",(getter)DocumentLink_slave_document_type},

    {"slave_store",(getter)DocumentLink_slave_store},

    {"slave_document",(getter)DocumentLink_slave_document},

    {"sort",(getter)DocumentLink_sort},

    {NULL}
};

static PyMethodDef DocumentLink_methods[] = {
    {"set", (PyCFunction)DocumentLink_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentLink_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentLink_clone, METH_NOARGS, "clone"},

    {"set_record_type", (PyCFunction)DocumentLink_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentLink_set_record_type"},

    {"set_master_document_type", (PyCFunction)DocumentLink_set_master_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentLink_set_master_document_type"},

    {"set_master_store", (PyCFunction)DocumentLink_set_master_store, METH_VARARGS|METH_KEYWORDS, "DocumentLink_set_master_store"},

    {"set_master_document", (PyCFunction)DocumentLink_set_master_document, METH_VARARGS|METH_KEYWORDS, "DocumentLink_set_master_document"},

    {"set_slave_document_type", (PyCFunction)DocumentLink_set_slave_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentLink_set_slave_document_type"},

    {"set_slave_store", (PyCFunction)DocumentLink_set_slave_store, METH_VARARGS|METH_KEYWORDS, "DocumentLink_set_slave_store"},

    {"set_slave_document", (PyCFunction)DocumentLink_set_slave_document, METH_VARARGS|METH_KEYWORDS, "DocumentLink_set_slave_document"},

    {"set_sort", (PyCFunction)DocumentLink_set_sort, METH_VARARGS|METH_KEYWORDS, "DocumentLink_set_sort"},

    {"get_gt", (PyCFunction)DocumentLink_gt, METH_VARARGS|METH_KEYWORDS, "DocumentLink_gt"},

    {"get_last", (PyCFunction)DocumentLink_last, METH_VARARGS|METH_KEYWORDS, "DocumentLink_last"},

    {"get_next", (PyCFunction)DocumentLink_next, METH_VARARGS|METH_KEYWORDS, "DocumentLink_next"},

    {"get_le", (PyCFunction)DocumentLink_le, METH_VARARGS|METH_KEYWORDS, "DocumentLink_le"},

    {"get_lt", (PyCFunction)DocumentLink_lt, METH_VARARGS|METH_KEYWORDS, "DocumentLink_lt"},

    {"get_equal", (PyCFunction)DocumentLink_equal, METH_VARARGS|METH_KEYWORDS, "DocumentLink_equal"},

    {"get_ge", (PyCFunction)DocumentLink_ge, METH_VARARGS|METH_KEYWORDS, "DocumentLink_ge"},

    {"get_prev", (PyCFunction)DocumentLink_prev, METH_VARARGS|METH_KEYWORDS, "DocumentLink_prev"},

    {"get_first", (PyCFunction)DocumentLink_first, METH_VARARGS|METH_KEYWORDS, "DocumentLink_first"},

    {"gets_gt", (PyCFunction)DocumentLink_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentLink_iter_gt"},

    {"gets_last", (PyCFunction)DocumentLink_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentLink_iter_last"},

    {"gets_le", (PyCFunction)DocumentLink_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentLink_iter_le"},

    {"gets_ge", (PyCFunction)DocumentLink_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentLink_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentLink_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentLink_iter_equal"},

    {"gets_lt", (PyCFunction)DocumentLink_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentLink_iter_lt"},

    {"gets_first", (PyCFunction)DocumentLink_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentLink_iter_first"},

    {"insert", (PyCFunction)DocumentLink_insert, METH_VARARGS|METH_KEYWORDS, "DocumentLink_insert"},

    {"update", (PyCFunction)DocumentLink_update, METH_VARARGS|METH_KEYWORDS, "DocumentLink_update"},

    {"delete", (PyCFunction)DocumentLink_delete, METH_VARARGS|METH_KEYWORDS, "DocumentLink_delete"},

    {NULL}
};

static PyTypeObject DocumentLinkType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentLink",             /*tp_name*/
    sizeof(DocumentLink),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentLink_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentLink_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentLink objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentLink_methods,             /* tp_methods */
    0,
    DocumentLink_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentLink_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentLink_new,                 /* tp_new */
};
PyTypeObject *getDocumentLinkType()
{
    return &DocumentLinkType_;
}

static void DocumentLinkKey0_dealloc(PyObject* self)
{
    do_free(((DocumentLinkKey0*)self)->priv);
    Py_DECREF(((DocumentLinkKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentLinkKey0 free\n");
}
static PyObject *DocumentLinkKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentLinkKey0 new\n");
    DocumentLinkKey0 *self;
    self = PyObject_NEW(DocumentLinkKey0, type);
    //self = (DocumentLinkKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_link_key0_t));
    return (PyObject *)self;
}
static int DocumentLinkKey0_init(DocumentLinkKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentLinkKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->m_dtype);

    do_text_set_empty(self->priv->m_sklad);

    do_text_set_empty(self->priv->m_document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentLinkKey0_clone(DocumentLinkKey0 *self)
{
    DocumentLinkKey0 *result;
    result = (DocumentLinkKey0*)PyObject_CallFunction((PyObject*)getDocumentLinkKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentLinkKey0_master_document_type(DocumentLinkKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->m_dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLinkKey0_set_master_document_type(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set m_dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->m_dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->m_dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLinkKey0_master_store(DocumentLinkKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->m_sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLinkKey0_set_master_store(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set m_sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->m_sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->m_sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLinkKey0_master_document(DocumentLinkKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->m_document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLinkKey0_set_master_document(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set m_document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->m_document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->m_document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLinkKey0_record_type(DocumentLinkKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *DocumentLinkKey0_set_record_type(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentLinkKey0_sort(DocumentLinkKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->sort);

    return result;
}

static PyObject *DocumentLinkKey0_set_sort(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sort");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->sort = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->sort = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sort");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->sort);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLinkKey0_gt(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_last(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_next(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_le(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_lt(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_equal(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_ge(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_prev(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_first(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey0_iter_gt(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_dtype, 
                 self->priv->m_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.m_sklad, 
                 self->priv->m_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.m_document, 
                 self->priv->m_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey0_iter_last(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_dtype, 
                 self->priv->m_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.m_sklad, 
                 self->priv->m_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.m_document, 
                 self->priv->m_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey0_iter_le(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_dtype, 
                 self->priv->m_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.m_sklad, 
                 self->priv->m_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.m_document, 
                 self->priv->m_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey0_iter_ge(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_dtype, 
                 self->priv->m_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.m_sklad, 
                 self->priv->m_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.m_document, 
                 self->priv->m_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey0_iter_equal(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_dtype, 
                 self->priv->m_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.m_sklad, 
                 self->priv->m_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.m_document, 
                 self->priv->m_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey0_iter_lt(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_dtype, 
                 self->priv->m_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.m_sklad, 
                 self->priv->m_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.m_document, 
                 self->priv->m_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey0_iter_first(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.m_dtype, 
                 self->priv->m_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.m_sklad, 
                 self->priv->m_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.m_document, 
                 self->priv->m_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey0_cmp(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentLinkKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->m_dtype, ((DocumentLinkKey0*)obj)->priv->m_dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->m_sklad, ((DocumentLinkKey0*)obj)->priv->m_sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->m_document, ((DocumentLinkKey0*)obj)->priv->m_document);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->type - ((DocumentLinkKey0*)obj)->priv->type;
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->sort - ((DocumentLinkKey0*)obj)->priv->sort;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentLinkKey0_set_alias(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentLinkKey0*)self)->alias);
    Py_INCREF(obj);
    ((DocumentLinkKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentLinkKey0_set(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentLinkKey0Type() ) {
        memcpy(self->priv, ((DocumentLinkKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentLinkType() ) {

        do_cpy(self->priv->m_dtype, ((DocumentLink*)obj)->priv->data.m_dtype);

        do_cpy(self->priv->m_sklad, ((DocumentLink*)obj)->priv->data.m_sklad);

        do_cpy(self->priv->m_document, ((DocumentLink*)obj)->priv->data.m_document);

        do_cpy(self->priv->type, ((DocumentLink*)obj)->priv->data.type);

        do_cpy(self->priv->sort, ((DocumentLink*)obj)->priv->data.sort);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_master_document_type", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_master_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_master_document", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
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
            if ( !strcmp("master_document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_master_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("master_store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_master_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("master_document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_master_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentLinkKey0\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentLinkKey0_fields(DocumentLinkKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentLinkKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentLinkKey0_master_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("master_document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLinkKey0_master_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("master_store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLinkKey0_master_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("master_document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLinkKey0_record_type(self,NULL);
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

    value =  DocumentLinkKey0_sort(self,NULL);
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

static PyGetSetDef DocumentLinkKey0_getset[] = {

    {"master_document_type",(getter)DocumentLinkKey0_master_document_type},

    {"master_store",(getter)DocumentLinkKey0_master_store},

    {"master_document",(getter)DocumentLinkKey0_master_document},

    {"record_type",(getter)DocumentLinkKey0_record_type},

    {"sort",(getter)DocumentLinkKey0_sort},

    {NULL}
};

static PyMethodDef DocumentLinkKey0_methods[] = {
    {"set", (PyCFunction)DocumentLinkKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentLinkKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentLinkKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentLinkKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_master_document_type", (PyCFunction)DocumentLinkKey0_set_master_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_set_master_document_type"},

    {"set_master_store", (PyCFunction)DocumentLinkKey0_set_master_store, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_set_master_store"},

    {"set_master_document", (PyCFunction)DocumentLinkKey0_set_master_document, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_set_master_document"},

    {"set_record_type", (PyCFunction)DocumentLinkKey0_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_set_record_type"},

    {"set_sort", (PyCFunction)DocumentLinkKey0_set_sort, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_set_sort"},

    {"get_gt", (PyCFunction)DocumentLinkKey0_gt, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_gt"},

    {"get_last", (PyCFunction)DocumentLinkKey0_last, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_last"},

    {"get_next", (PyCFunction)DocumentLinkKey0_next, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_next"},

    {"get_le", (PyCFunction)DocumentLinkKey0_le, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_le"},

    {"get_lt", (PyCFunction)DocumentLinkKey0_lt, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_lt"},

    {"get_equal", (PyCFunction)DocumentLinkKey0_equal, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_equal"},

    {"get_ge", (PyCFunction)DocumentLinkKey0_ge, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_ge"},

    {"get_prev", (PyCFunction)DocumentLinkKey0_prev, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_prev"},

    {"get_first", (PyCFunction)DocumentLinkKey0_first, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_first"},

    {"gets_gt", (PyCFunction)DocumentLinkKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_iter_gt"},

    {"gets_last", (PyCFunction)DocumentLinkKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_iter_last"},

    {"gets_le", (PyCFunction)DocumentLinkKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_iter_le"},

    {"gets_ge", (PyCFunction)DocumentLinkKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentLinkKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_iter_equal"},

    {"gets_lt", (PyCFunction)DocumentLinkKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_iter_lt"},

    {"gets_first", (PyCFunction)DocumentLinkKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey0_iter_first"},

    {NULL}
};

static PyTypeObject DocumentLinkKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentLinkKey0",             /*tp_name*/
    sizeof(DocumentLinkKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentLinkKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentLinkKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentLinkKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentLinkKey0_methods,             /* tp_methods */
    0,
    DocumentLinkKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentLinkKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentLinkKey0_new,                 /* tp_new */
};
PyTypeObject *getDocumentLinkKey0Type()
{
    return &DocumentLinkKey0Type_;
}

static void DocumentLinkKey1_dealloc(PyObject* self)
{
    do_free(((DocumentLinkKey1*)self)->priv);
    Py_DECREF(((DocumentLinkKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentLinkKey1 free\n");
}
static PyObject *DocumentLinkKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentLinkKey1 new\n");
    DocumentLinkKey1 *self;
    self = PyObject_NEW(DocumentLinkKey1, type);
    //self = (DocumentLinkKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_link_key1_t));
    return (PyObject *)self;
}
static int DocumentLinkKey1_init(DocumentLinkKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentLinkKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->s_dtype);

    do_text_set_empty(self->priv->s_sklad);

    do_text_set_empty(self->priv->s_document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentLinkKey1_clone(DocumentLinkKey1 *self)
{
    DocumentLinkKey1 *result;
    result = (DocumentLinkKey1*)PyObject_CallFunction((PyObject*)getDocumentLinkKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentLinkKey1_slave_document_type(DocumentLinkKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->s_dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLinkKey1_set_slave_document_type(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set s_dtype");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->s_dtype, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->s_dtype);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLinkKey1_slave_store(DocumentLinkKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->s_sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLinkKey1_set_slave_store(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set s_sklad");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->s_sklad, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->s_sklad);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLinkKey1_slave_document(DocumentLinkKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->s_document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentLinkKey1_set_slave_document(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set s_document");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->s_document, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->s_document);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLinkKey1_record_type(DocumentLinkKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *DocumentLinkKey1_set_record_type(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentLinkKey1_sort(DocumentLinkKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->sort);

    return result;
}

static PyObject *DocumentLinkKey1_set_sort(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sort");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->sort = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->sort = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set sort");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->sort);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentLinkKey1_gt(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_last(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_next(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_le(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_lt(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_equal(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_ge(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_prev(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_first(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentLinkKey1_iter_gt(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_dtype, 
                 self->priv->s_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.s_sklad, 
                 self->priv->s_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.s_document, 
                 self->priv->s_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey1_iter_last(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_dtype, 
                 self->priv->s_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.s_sklad, 
                 self->priv->s_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.s_document, 
                 self->priv->s_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey1_iter_le(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_dtype, 
                 self->priv->s_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.s_sklad, 
                 self->priv->s_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.s_document, 
                 self->priv->s_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey1_iter_ge(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_dtype, 
                 self->priv->s_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.s_sklad, 
                 self->priv->s_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.s_document, 
                 self->priv->s_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey1_iter_equal(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_dtype, 
                 self->priv->s_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.s_sklad, 
                 self->priv->s_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.s_document, 
                 self->priv->s_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey1_iter_lt(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_dtype, 
                 self->priv->s_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.s_sklad, 
                 self->priv->s_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.s_document, 
                 self->priv->s_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey1_iter_first(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_link_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.s_dtype, 
                 self->priv->s_dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.s_sklad, 
                 self->priv->s_sklad))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.s_document, 
                 self->priv->s_document))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

 
        item = DocumentLinkKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_link_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentLinkKey1_cmp(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentLinkKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->s_dtype, ((DocumentLinkKey1*)obj)->priv->s_dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->s_sklad, ((DocumentLinkKey1*)obj)->priv->s_sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->s_document, ((DocumentLinkKey1*)obj)->priv->s_document);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->type - ((DocumentLinkKey1*)obj)->priv->type;
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->sort - ((DocumentLinkKey1*)obj)->priv->sort;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentLinkKey1_set_alias(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentLinkKey1*)self)->alias);
    Py_INCREF(obj);
    ((DocumentLinkKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentLinkKey1_set(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentLinkKey1Type() ) {
        memcpy(self->priv, ((DocumentLinkKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentLinkType() ) {

        do_cpy(self->priv->s_dtype, ((DocumentLink*)obj)->priv->data.s_dtype);

        do_cpy(self->priv->s_sklad, ((DocumentLink*)obj)->priv->data.s_sklad);

        do_cpy(self->priv->s_document, ((DocumentLink*)obj)->priv->data.s_document);

        do_cpy(self->priv->type, ((DocumentLink*)obj)->priv->data.type);

        do_cpy(self->priv->sort, ((DocumentLink*)obj)->priv->data.sort);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_slave_document_type", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_slave_store", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_slave_document", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
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
            if ( !strcmp("slave_document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_slave_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("slave_store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_slave_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("slave_document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_slave_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentLinkKey1\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentLinkKey1_fields(DocumentLinkKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentLinkKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentLinkKey1_slave_document_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("slave_document_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLinkKey1_slave_store(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("slave_store", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLinkKey1_slave_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("slave_document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentLinkKey1_record_type(self,NULL);
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

    value =  DocumentLinkKey1_sort(self,NULL);
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

static PyGetSetDef DocumentLinkKey1_getset[] = {

    {"slave_document_type",(getter)DocumentLinkKey1_slave_document_type},

    {"slave_store",(getter)DocumentLinkKey1_slave_store},

    {"slave_document",(getter)DocumentLinkKey1_slave_document},

    {"record_type",(getter)DocumentLinkKey1_record_type},

    {"sort",(getter)DocumentLinkKey1_sort},

    {NULL}
};

static PyMethodDef DocumentLinkKey1_methods[] = {
    {"set", (PyCFunction)DocumentLinkKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentLinkKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentLinkKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentLinkKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_slave_document_type", (PyCFunction)DocumentLinkKey1_set_slave_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_set_slave_document_type"},

    {"set_slave_store", (PyCFunction)DocumentLinkKey1_set_slave_store, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_set_slave_store"},

    {"set_slave_document", (PyCFunction)DocumentLinkKey1_set_slave_document, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_set_slave_document"},

    {"set_record_type", (PyCFunction)DocumentLinkKey1_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_set_record_type"},

    {"set_sort", (PyCFunction)DocumentLinkKey1_set_sort, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_set_sort"},

    {"get_gt", (PyCFunction)DocumentLinkKey1_gt, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_gt"},

    {"get_last", (PyCFunction)DocumentLinkKey1_last, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_last"},

    {"get_next", (PyCFunction)DocumentLinkKey1_next, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_next"},

    {"get_le", (PyCFunction)DocumentLinkKey1_le, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_le"},

    {"get_lt", (PyCFunction)DocumentLinkKey1_lt, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_lt"},

    {"get_equal", (PyCFunction)DocumentLinkKey1_equal, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_equal"},

    {"get_ge", (PyCFunction)DocumentLinkKey1_ge, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_ge"},

    {"get_prev", (PyCFunction)DocumentLinkKey1_prev, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_prev"},

    {"get_first", (PyCFunction)DocumentLinkKey1_first, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_first"},

    {"gets_gt", (PyCFunction)DocumentLinkKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_iter_gt"},

    {"gets_last", (PyCFunction)DocumentLinkKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_iter_last"},

    {"gets_le", (PyCFunction)DocumentLinkKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_iter_le"},

    {"gets_ge", (PyCFunction)DocumentLinkKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentLinkKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_iter_equal"},

    {"gets_lt", (PyCFunction)DocumentLinkKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_iter_lt"},

    {"gets_first", (PyCFunction)DocumentLinkKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentLinkKey1_iter_first"},

    {NULL}
};

static PyTypeObject DocumentLinkKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentLinkKey1",             /*tp_name*/
    sizeof(DocumentLinkKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentLinkKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentLinkKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentLinkKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentLinkKey1_methods,             /* tp_methods */
    0,
    DocumentLinkKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentLinkKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentLinkKey1_new,                 /* tp_new */
};
PyTypeObject *getDocumentLinkKey1Type()
{
    return &DocumentLinkKey1Type_;
}
