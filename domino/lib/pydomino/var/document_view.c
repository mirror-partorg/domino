
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "document_view.h"

static void DocumentView_dealloc(PyObject* self)
{
    do_free(((DocumentView*)self)->priv);
    Py_DECREF(((DocumentView*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentView free\n");
}
static PyObject *DocumentView_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentView new\n");
    DocumentView *self;
    self = PyObject_NEW(DocumentView, type);
    //self = (DocumentView *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_view_rec_t));
    return (PyObject *)self;
}
static int DocumentView_init(DocumentView *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentView init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.sort);

    do_text_set_empty(self->priv->data.dtype);

    do_text_set_empty(self->priv->data.sklad);

    do_text_set_empty(self->priv->data.document);

    do_text_set_empty(self->priv->data.dtype1);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentView_clone(DocumentView *self)
{
    DocumentView *result;
    result = (DocumentView*)PyObject_CallFunction((PyObject*)getDocumentViewType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *DocumentView_record_type(DocumentView* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type);

    return result;
}

static PyObject *DocumentView_set_record_type(DocumentView* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentView_sort(DocumentView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sort);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentView_set_sort(DocumentView* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentView_document_type(DocumentView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentView_set_document_type(DocumentView* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentView_store(DocumentView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentView_set_store(DocumentView* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentView_document(DocumentView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentView_set_document(DocumentView* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentView_document_operation(DocumentView* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype1);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentView_set_document_operation(DocumentView* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentView_prev(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_gt(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_next(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_ge(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_equal(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_last(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_lt(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_le(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_first(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentViewKey0Type() )
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() )
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_iter_gt(DocumentView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
        key_cmp = (document_view_key0_t*)do_malloc(sizeof(document_view_key0_t));
        memcpy(key_cmp, ((DocumentViewKey0*)key)->priv, sizeof(document_view_key0_t));
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
        key_cmp = (document_view_key1_t*)do_malloc(sizeof(document_view_key1_t));
        memcpy(key_cmp, ((DocumentViewKey1*)key)->priv, sizeof(document_view_key1_t));
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->type, 
                    ((DocumentViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sort, 
                    ((DocumentViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->dtype, 
                    ((DocumentViewKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sklad, 
                    ((DocumentViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->document, 
                    ((DocumentViewKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->dtype, 
                    ((DocumentViewKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->sklad, 
                    ((DocumentViewKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->document, 
                    ((DocumentViewKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->type, 
                    ((DocumentViewKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
            status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
            status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentView_iter_ge(DocumentView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
        key_cmp = (document_view_key0_t*)do_malloc(sizeof(document_view_key0_t));
        memcpy(key_cmp, ((DocumentViewKey0*)key)->priv, sizeof(document_view_key0_t));
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
        key_cmp = (document_view_key1_t*)do_malloc(sizeof(document_view_key1_t));
        memcpy(key_cmp, ((DocumentViewKey1*)key)->priv, sizeof(document_view_key1_t));
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->type, 
                    ((DocumentViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sort, 
                    ((DocumentViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->dtype, 
                    ((DocumentViewKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sklad, 
                    ((DocumentViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->document, 
                    ((DocumentViewKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->dtype, 
                    ((DocumentViewKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->sklad, 
                    ((DocumentViewKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->document, 
                    ((DocumentViewKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->type, 
                    ((DocumentViewKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
            status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
            status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentView_iter_equal(DocumentView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
        key_cmp = (document_view_key0_t*)do_malloc(sizeof(document_view_key0_t));
        memcpy(key_cmp, ((DocumentViewKey0*)key)->priv, sizeof(document_view_key0_t));
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
        key_cmp = (document_view_key1_t*)do_malloc(sizeof(document_view_key1_t));
        memcpy(key_cmp, ((DocumentViewKey1*)key)->priv, sizeof(document_view_key1_t));
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->type, 
                    ((DocumentViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sort, 
                    ((DocumentViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->dtype, 
                    ((DocumentViewKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sklad, 
                    ((DocumentViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->document, 
                    ((DocumentViewKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->dtype, 
                    ((DocumentViewKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->sklad, 
                    ((DocumentViewKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->document, 
                    ((DocumentViewKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->type, 
                    ((DocumentViewKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
            status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
            status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentView_iter_last(DocumentView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
        key_cmp = (document_view_key0_t*)do_malloc(sizeof(document_view_key0_t));
        memcpy(key_cmp, ((DocumentViewKey0*)key)->priv, sizeof(document_view_key0_t));
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
        key_cmp = (document_view_key1_t*)do_malloc(sizeof(document_view_key1_t));
        memcpy(key_cmp, ((DocumentViewKey1*)key)->priv, sizeof(document_view_key1_t));
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->type, 
                    ((DocumentViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sort, 
                    ((DocumentViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->dtype, 
                    ((DocumentViewKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sklad, 
                    ((DocumentViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->document, 
                    ((DocumentViewKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->dtype, 
                    ((DocumentViewKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->sklad, 
                    ((DocumentViewKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->document, 
                    ((DocumentViewKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->type, 
                    ((DocumentViewKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
            status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
            status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentView_iter_lt(DocumentView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
        key_cmp = (document_view_key0_t*)do_malloc(sizeof(document_view_key0_t));
        memcpy(key_cmp, ((DocumentViewKey0*)key)->priv, sizeof(document_view_key0_t));
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
        key_cmp = (document_view_key1_t*)do_malloc(sizeof(document_view_key1_t));
        memcpy(key_cmp, ((DocumentViewKey1*)key)->priv, sizeof(document_view_key1_t));
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->type, 
                    ((DocumentViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sort, 
                    ((DocumentViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->dtype, 
                    ((DocumentViewKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sklad, 
                    ((DocumentViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->document, 
                    ((DocumentViewKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->dtype, 
                    ((DocumentViewKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->sklad, 
                    ((DocumentViewKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->document, 
                    ((DocumentViewKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->type, 
                    ((DocumentViewKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
            status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
            status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentView_iter_le(DocumentView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
        key_cmp = (document_view_key0_t*)do_malloc(sizeof(document_view_key0_t));
        memcpy(key_cmp, ((DocumentViewKey0*)key)->priv, sizeof(document_view_key0_t));
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
        key_cmp = (document_view_key1_t*)do_malloc(sizeof(document_view_key1_t));
        memcpy(key_cmp, ((DocumentViewKey1*)key)->priv, sizeof(document_view_key1_t));
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->type, 
                    ((DocumentViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sort, 
                    ((DocumentViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->dtype, 
                    ((DocumentViewKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sklad, 
                    ((DocumentViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->document, 
                    ((DocumentViewKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->dtype, 
                    ((DocumentViewKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->sklad, 
                    ((DocumentViewKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->document, 
                    ((DocumentViewKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->type, 
                    ((DocumentViewKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
            status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
            status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *DocumentView_iter_first(DocumentView* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
        key_cmp = (document_view_key0_t*)do_malloc(sizeof(document_view_key0_t));
        memcpy(key_cmp, ((DocumentViewKey0*)key)->priv, sizeof(document_view_key0_t));
        status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
        key_cmp = (document_view_key1_t*)do_malloc(sizeof(document_view_key1_t));
        memcpy(key_cmp, ((DocumentViewKey1*)key)->priv, sizeof(document_view_key1_t));
        status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->type, 
                    ((DocumentViewKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sort, 
                    ((DocumentViewKey0*)key)->priv->sort))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->dtype, 
                    ((DocumentViewKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->sklad, 
                    ((DocumentViewKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_view_key0_t*)key_cmp)->document, 
                    ((DocumentViewKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->dtype, 
                    ((DocumentViewKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->sklad, 
                    ((DocumentViewKey1*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->document, 
                    ((DocumentViewKey1*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_view_key1_t*)key_cmp)->type, 
                    ((DocumentViewKey1*)key)->priv->type))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = DocumentView_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentViewKey0Type() ) {
            status = do_document_view_get0(self->alias->alias, self->priv, ((DocumentViewKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentViewKey1Type() ) {
            status = do_document_view_get1(self->alias->alias, self->priv, ((DocumentViewKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *DocumentView_update(DocumentView* self)
{
    int status;
    status = do_document_view_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_insert(DocumentView* self)
{
    int status;
    status = do_document_view_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_delete(DocumentView* self)
{
    int status;
    status = do_document_view_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentView_set_alias(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentView*)self)->alias);
    Py_INCREF(obj);
    ((DocumentView*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentView_set(DocumentView* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentViewType() ) {
        memcpy(self->priv, ((DocumentView*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {

        do_cpy(self->priv->data.type, ((DocumentViewKey0*)obj)->priv->type);

        do_cpy(self->priv->data.sort, ((DocumentViewKey0*)obj)->priv->sort);

        do_cpy(self->priv->data.dtype, ((DocumentViewKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentViewKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentViewKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {

        do_cpy(self->priv->data.type, ((DocumentViewKey1*)obj)->priv->type);

        do_cpy(self->priv->data.dtype, ((DocumentViewKey1*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentViewKey1*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentViewKey1*)obj)->priv->document);

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
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
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
            if ( !strcmp("document_operation", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_operation", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"DocumentView\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentView_fields(DocumentView* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentView");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentView_record_type(self,NULL);
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

    value =  DocumentView_sort(self,NULL);
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

    value =  DocumentView_document_type(self,NULL);
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

    value =  DocumentView_store(self,NULL);
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

    value =  DocumentView_document(self,NULL);
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

    value =  DocumentView_document_operation(self,NULL);
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

static PyGetSetDef DocumentView_getset[] = {

    {"record_type",(getter)DocumentView_record_type},

    {"sort",(getter)DocumentView_sort},

    {"document_type",(getter)DocumentView_document_type},

    {"store",(getter)DocumentView_store},

    {"document",(getter)DocumentView_document},

    {"document_operation",(getter)DocumentView_document_operation},

    {NULL}
};

static PyMethodDef DocumentView_methods[] = {
    {"set", (PyCFunction)DocumentView_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentView_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentView_clone, METH_NOARGS, "clone"},

    {"set_record_type", (PyCFunction)DocumentView_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentView_set_record_type"},

    {"set_sort", (PyCFunction)DocumentView_set_sort, METH_VARARGS|METH_KEYWORDS, "DocumentView_set_sort"},

    {"set_document_type", (PyCFunction)DocumentView_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentView_set_document_type"},

    {"set_store", (PyCFunction)DocumentView_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentView_set_store"},

    {"set_document", (PyCFunction)DocumentView_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentView_set_document"},

    {"set_document_operation", (PyCFunction)DocumentView_set_document_operation, METH_VARARGS|METH_KEYWORDS, "DocumentView_set_document_operation"},

    {"get_prev", (PyCFunction)DocumentView_prev, METH_VARARGS|METH_KEYWORDS, "DocumentView_prev"},

    {"get_gt", (PyCFunction)DocumentView_gt, METH_VARARGS|METH_KEYWORDS, "DocumentView_gt"},

    {"get_next", (PyCFunction)DocumentView_next, METH_VARARGS|METH_KEYWORDS, "DocumentView_next"},

    {"get_ge", (PyCFunction)DocumentView_ge, METH_VARARGS|METH_KEYWORDS, "DocumentView_ge"},

    {"get_equal", (PyCFunction)DocumentView_equal, METH_VARARGS|METH_KEYWORDS, "DocumentView_equal"},

    {"get_last", (PyCFunction)DocumentView_last, METH_VARARGS|METH_KEYWORDS, "DocumentView_last"},

    {"get_lt", (PyCFunction)DocumentView_lt, METH_VARARGS|METH_KEYWORDS, "DocumentView_lt"},

    {"get_le", (PyCFunction)DocumentView_le, METH_VARARGS|METH_KEYWORDS, "DocumentView_le"},

    {"get_first", (PyCFunction)DocumentView_first, METH_VARARGS|METH_KEYWORDS, "DocumentView_first"},

    {"gets_gt", (PyCFunction)DocumentView_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentView_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentView_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentView_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentView_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentView_iter_equal"},

    {"gets_last", (PyCFunction)DocumentView_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentView_iter_last"},

    {"gets_lt", (PyCFunction)DocumentView_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentView_iter_lt"},

    {"gets_le", (PyCFunction)DocumentView_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentView_iter_le"},

    {"gets_first", (PyCFunction)DocumentView_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentView_iter_first"},

    {"update", (PyCFunction)DocumentView_update, METH_VARARGS|METH_KEYWORDS, "DocumentView_update"},

    {"insert", (PyCFunction)DocumentView_insert, METH_VARARGS|METH_KEYWORDS, "DocumentView_insert"},

    {"delete", (PyCFunction)DocumentView_delete, METH_VARARGS|METH_KEYWORDS, "DocumentView_delete"},

    {NULL}
};

static PyTypeObject DocumentViewType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentView",             /*tp_name*/
    sizeof(DocumentView),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentView_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentView_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentView objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentView_methods,             /* tp_methods */
    0,
    DocumentView_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentView_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentView_new,                 /* tp_new */
};
PyTypeObject *getDocumentViewType()
{
    return &DocumentViewType_;
}

static void DocumentViewKey0_dealloc(PyObject* self)
{
    do_free(((DocumentViewKey0*)self)->priv);
    Py_DECREF(((DocumentViewKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentViewKey0 free\n");
}
static PyObject *DocumentViewKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentViewKey0 new\n");
    DocumentViewKey0 *self;
    self = PyObject_NEW(DocumentViewKey0, type);
    //self = (DocumentViewKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_view_key0_t));
    return (PyObject *)self;
}
static int DocumentViewKey0_init(DocumentViewKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentViewKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->sort);

    do_text_set_empty(self->priv->dtype);

    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentViewKey0_clone(DocumentViewKey0 *self)
{
    DocumentViewKey0 *result;
    result = (DocumentViewKey0*)PyObject_CallFunction((PyObject*)getDocumentViewKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentViewKey0_record_type(DocumentViewKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *DocumentViewKey0_set_record_type(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey0_sort(DocumentViewKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sort);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentViewKey0_set_sort(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey0_document_type(DocumentViewKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentViewKey0_set_document_type(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey0_store(DocumentViewKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentViewKey0_set_store(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey0_document(DocumentViewKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentViewKey0_set_document(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey0_prev(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_gt(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_next(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_ge(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_equal(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_last(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_lt(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_le(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_first(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey0_iter_gt(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = DocumentViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey0_iter_ge(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = DocumentViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey0_iter_equal(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = DocumentViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey0_iter_last(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = DocumentViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey0_iter_lt(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = DocumentViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey0_iter_le(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = DocumentViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey0_iter_first(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.sort, 
                 self->priv->sort))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = DocumentViewKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey0_cmp(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->type - ((DocumentViewKey0*)obj)->priv->type;
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sort, ((DocumentViewKey0*)obj)->priv->sort);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->dtype, ((DocumentViewKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->sklad, ((DocumentViewKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 5 ) {
           res = do_cmp(self->priv->document, ((DocumentViewKey0*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentViewKey0_set_alias(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentViewKey0*)self)->alias);
    Py_INCREF(obj);
    ((DocumentViewKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentViewKey0_set(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {
        memcpy(self->priv, ((DocumentViewKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewType() ) {

        do_cpy(self->priv->type, ((DocumentView*)obj)->priv->data.type);

        do_cpy(self->priv->sort, ((DocumentView*)obj)->priv->data.sort);

        do_cpy(self->priv->dtype, ((DocumentView*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentView*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentView*)obj)->priv->data.document);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
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
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sort", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sort", "O", value) )
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

            else {
                do_log(LOG_ERR, "Class \"DocumentViewKey0\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentViewKey0_fields(DocumentViewKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentViewKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentViewKey0_record_type(self,NULL);
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

    value =  DocumentViewKey0_sort(self,NULL);
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

    value =  DocumentViewKey0_document_type(self,NULL);
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

    value =  DocumentViewKey0_store(self,NULL);
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

    value =  DocumentViewKey0_document(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef DocumentViewKey0_getset[] = {

    {"record_type",(getter)DocumentViewKey0_record_type},

    {"sort",(getter)DocumentViewKey0_sort},

    {"document_type",(getter)DocumentViewKey0_document_type},

    {"store",(getter)DocumentViewKey0_store},

    {"document",(getter)DocumentViewKey0_document},

    {NULL}
};

static PyMethodDef DocumentViewKey0_methods[] = {
    {"set", (PyCFunction)DocumentViewKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentViewKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentViewKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentViewKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_record_type", (PyCFunction)DocumentViewKey0_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_set_record_type"},

    {"set_sort", (PyCFunction)DocumentViewKey0_set_sort, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_set_sort"},

    {"set_document_type", (PyCFunction)DocumentViewKey0_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_set_document_type"},

    {"set_store", (PyCFunction)DocumentViewKey0_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_set_store"},

    {"set_document", (PyCFunction)DocumentViewKey0_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_set_document"},

    {"get_prev", (PyCFunction)DocumentViewKey0_prev, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_prev"},

    {"get_gt", (PyCFunction)DocumentViewKey0_gt, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_gt"},

    {"get_next", (PyCFunction)DocumentViewKey0_next, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_next"},

    {"get_ge", (PyCFunction)DocumentViewKey0_ge, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_ge"},

    {"get_equal", (PyCFunction)DocumentViewKey0_equal, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_equal"},

    {"get_last", (PyCFunction)DocumentViewKey0_last, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_last"},

    {"get_lt", (PyCFunction)DocumentViewKey0_lt, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_lt"},

    {"get_le", (PyCFunction)DocumentViewKey0_le, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_le"},

    {"get_first", (PyCFunction)DocumentViewKey0_first, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_first"},

    {"gets_gt", (PyCFunction)DocumentViewKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentViewKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentViewKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_iter_equal"},

    {"gets_last", (PyCFunction)DocumentViewKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_iter_last"},

    {"gets_lt", (PyCFunction)DocumentViewKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_iter_lt"},

    {"gets_le", (PyCFunction)DocumentViewKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_iter_le"},

    {"gets_first", (PyCFunction)DocumentViewKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey0_iter_first"},

    {NULL}
};

static PyTypeObject DocumentViewKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentViewKey0",             /*tp_name*/
    sizeof(DocumentViewKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentViewKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentViewKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentViewKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentViewKey0_methods,             /* tp_methods */
    0,
    DocumentViewKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentViewKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentViewKey0_new,                 /* tp_new */
};
PyTypeObject *getDocumentViewKey0Type()
{
    return &DocumentViewKey0Type_;
}

static void DocumentViewKey1_dealloc(PyObject* self)
{
    do_free(((DocumentViewKey1*)self)->priv);
    Py_DECREF(((DocumentViewKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentViewKey1 free\n");
}
static PyObject *DocumentViewKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentViewKey1 new\n");
    DocumentViewKey1 *self;
    self = PyObject_NEW(DocumentViewKey1, type);
    //self = (DocumentViewKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_view_key1_t));
    return (PyObject *)self;
}
static int DocumentViewKey1_init(DocumentViewKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentViewKey1 init\n");
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

static PyObject *DocumentViewKey1_clone(DocumentViewKey1 *self)
{
    DocumentViewKey1 *result;
    result = (DocumentViewKey1*)PyObject_CallFunction((PyObject*)getDocumentViewKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentViewKey1_document_type(DocumentViewKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentViewKey1_set_document_type(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey1_store(DocumentViewKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentViewKey1_set_store(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey1_document(DocumentViewKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentViewKey1_set_document(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey1_record_type(DocumentViewKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->type);

    return result;
}

static PyObject *DocumentViewKey1_set_record_type(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentViewKey1_prev(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_gt(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_next(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_ge(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_equal(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_last(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_lt(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_le(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_first(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentViewKey1_iter_gt(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_GT);
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

 
        item = DocumentViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey1_iter_ge(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_GE);
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

 
        item = DocumentViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey1_iter_equal(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
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

 
        item = DocumentViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey1_iter_last(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_LAST);
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

 
        item = DocumentViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey1_iter_lt(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_LT);
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

 
        item = DocumentViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey1_iter_le(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_LE);
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

 
        item = DocumentViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey1_iter_first(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_view_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_FIRST);
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

 
        item = DocumentViewKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_view_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentViewKey1_cmp(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentViewKey1*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentViewKey1*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentViewKey1*)obj)->priv->document);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->type - ((DocumentViewKey1*)obj)->priv->type;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentViewKey1_set_alias(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentViewKey1*)self)->alias);
    Py_INCREF(obj);
    ((DocumentViewKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentViewKey1_set(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {
        memcpy(self->priv, ((DocumentViewKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewType() ) {

        do_cpy(self->priv->dtype, ((DocumentView*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentView*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentView*)obj)->priv->data.document);

        do_cpy(self->priv->type, ((DocumentView*)obj)->priv->data.type);

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

            else {
                do_log(LOG_ERR, "Class \"DocumentViewKey1\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentViewKey1_fields(DocumentViewKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentViewKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentViewKey1_document_type(self,NULL);
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

    value =  DocumentViewKey1_store(self,NULL);
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

    value =  DocumentViewKey1_document(self,NULL);
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

    value =  DocumentViewKey1_record_type(self,NULL);
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

static PyGetSetDef DocumentViewKey1_getset[] = {

    {"document_type",(getter)DocumentViewKey1_document_type},

    {"store",(getter)DocumentViewKey1_store},

    {"document",(getter)DocumentViewKey1_document},

    {"record_type",(getter)DocumentViewKey1_record_type},

    {NULL}
};

static PyMethodDef DocumentViewKey1_methods[] = {
    {"set", (PyCFunction)DocumentViewKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentViewKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentViewKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentViewKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentViewKey1_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_set_document_type"},

    {"set_store", (PyCFunction)DocumentViewKey1_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_set_store"},

    {"set_document", (PyCFunction)DocumentViewKey1_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_set_document"},

    {"set_record_type", (PyCFunction)DocumentViewKey1_set_record_type, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_set_record_type"},

    {"get_prev", (PyCFunction)DocumentViewKey1_prev, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_prev"},

    {"get_gt", (PyCFunction)DocumentViewKey1_gt, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_gt"},

    {"get_next", (PyCFunction)DocumentViewKey1_next, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_next"},

    {"get_ge", (PyCFunction)DocumentViewKey1_ge, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_ge"},

    {"get_equal", (PyCFunction)DocumentViewKey1_equal, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_equal"},

    {"get_last", (PyCFunction)DocumentViewKey1_last, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_last"},

    {"get_lt", (PyCFunction)DocumentViewKey1_lt, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_lt"},

    {"get_le", (PyCFunction)DocumentViewKey1_le, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_le"},

    {"get_first", (PyCFunction)DocumentViewKey1_first, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_first"},

    {"gets_gt", (PyCFunction)DocumentViewKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentViewKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_iter_ge"},

    {"gets_equal", (PyCFunction)DocumentViewKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_iter_equal"},

    {"gets_last", (PyCFunction)DocumentViewKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_iter_last"},

    {"gets_lt", (PyCFunction)DocumentViewKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_iter_lt"},

    {"gets_le", (PyCFunction)DocumentViewKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_iter_le"},

    {"gets_first", (PyCFunction)DocumentViewKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentViewKey1_iter_first"},

    {NULL}
};

static PyTypeObject DocumentViewKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentViewKey1",             /*tp_name*/
    sizeof(DocumentViewKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentViewKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentViewKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentViewKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentViewKey1_methods,             /* tp_methods */
    0,
    DocumentViewKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentViewKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentViewKey1_new,                 /* tp_new */
};
PyTypeObject *getDocumentViewKey1Type()
{
    return &DocumentViewKey1Type_;
}
