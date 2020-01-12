
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "class.h"

static void Class_dealloc(PyObject* self)
{
    do_free(((Class*)self)->priv);
    Py_DECREF(((Class*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Class free\n");
}
static PyObject *Class_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Class new\n");
    Class *self;
    self = PyObject_NEW(Class, type);
    //self = (Class *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(class_rec_t));
    return (PyObject *)self;
}
static int Class_init(Class *self, PyObject *args, PyObject *kwds)
{
    //LOG("Class init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.class_id);

    do_text_set_empty(self->priv->data.name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Class_clone(Class *self)
{
    Class *result;
    result = (Class*)PyObject_CallFunction((PyObject*)getClassType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Class_class_id(Class* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Class_set_class_id(Class* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.class_id");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.class_id, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.class_id);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Class_name(Class* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Class_set_name(Class* self, PyObject *args, PyObject *kwds)
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

static PyObject *Class_prev(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_gt(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_next(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_ge(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_equal(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_last(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_lt(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_le(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_first(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getClassKey0Type() )
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_iter_gt(Class* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getClassKey0Type() ) {
        key_cmp = (class_key0_t*)do_malloc(sizeof(class_key0_t));
        memcpy(key_cmp, ((ClassKey0*)key)->priv, sizeof(class_key0_t));
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getClassKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((class_key0_t*)key_cmp)->class_id, 
                    ((ClassKey0*)key)->priv->class_id))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Class_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getClassKey0Type() ) {
            status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Class_iter_ge(Class* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getClassKey0Type() ) {
        key_cmp = (class_key0_t*)do_malloc(sizeof(class_key0_t));
        memcpy(key_cmp, ((ClassKey0*)key)->priv, sizeof(class_key0_t));
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getClassKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((class_key0_t*)key_cmp)->class_id, 
                    ((ClassKey0*)key)->priv->class_id))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Class_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getClassKey0Type() ) {
            status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Class_iter_equal(Class* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getClassKey0Type() ) {
        key_cmp = (class_key0_t*)do_malloc(sizeof(class_key0_t));
        memcpy(key_cmp, ((ClassKey0*)key)->priv, sizeof(class_key0_t));
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getClassKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((class_key0_t*)key_cmp)->class_id, 
                    ((ClassKey0*)key)->priv->class_id))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Class_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getClassKey0Type() ) {
            status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Class_iter_last(Class* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getClassKey0Type() ) {
        key_cmp = (class_key0_t*)do_malloc(sizeof(class_key0_t));
        memcpy(key_cmp, ((ClassKey0*)key)->priv, sizeof(class_key0_t));
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getClassKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((class_key0_t*)key_cmp)->class_id, 
                    ((ClassKey0*)key)->priv->class_id))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Class_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getClassKey0Type() ) {
            status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Class_iter_lt(Class* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getClassKey0Type() ) {
        key_cmp = (class_key0_t*)do_malloc(sizeof(class_key0_t));
        memcpy(key_cmp, ((ClassKey0*)key)->priv, sizeof(class_key0_t));
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getClassKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((class_key0_t*)key_cmp)->class_id, 
                    ((ClassKey0*)key)->priv->class_id))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Class_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getClassKey0Type() ) {
            status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Class_iter_le(Class* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getClassKey0Type() ) {
        key_cmp = (class_key0_t*)do_malloc(sizeof(class_key0_t));
        memcpy(key_cmp, ((ClassKey0*)key)->priv, sizeof(class_key0_t));
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getClassKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((class_key0_t*)key_cmp)->class_id, 
                    ((ClassKey0*)key)->priv->class_id))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Class_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getClassKey0Type() ) {
            status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Class_iter_first(Class* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getClassKey0Type() ) {
        key_cmp = (class_key0_t*)do_malloc(sizeof(class_key0_t));
        memcpy(key_cmp, ((ClassKey0*)key)->priv, sizeof(class_key0_t));
        status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getClassKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((class_key0_t*)key_cmp)->class_id, 
                    ((ClassKey0*)key)->priv->class_id))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Class_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getClassKey0Type() ) {
            status = do_class_get0(self->alias->alias, self->priv, ((ClassKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Class_update(Class* self)
{
    int status;
    status = do_class_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_insert(Class* self)
{
    int status;
    status = do_class_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_delete(Class* self)
{
    int status;
    status = do_class_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Class_set_alias(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Class*)self)->alias);
    Py_INCREF(obj);
    ((Class*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Class_set(Class* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getClassType() ) {
        memcpy(self->priv, ((Class*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getClassKey0Type() ) {

        do_cpy(self->priv->data.class_id, ((ClassKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupType() ) {

        do_cpy(self->priv->data.class_id, ((Group*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey0Type() ) {

        do_cpy(self->priv->data.class_id, ((GroupKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey1Type() ) {

        do_cpy(self->priv->data.class_id, ((GroupKey1*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupType() ) {

        do_cpy(self->priv->data.class_id, ((Subgroup*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey0Type() ) {

        do_cpy(self->priv->data.class_id, ((SubgroupKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey1Type() ) {

        do_cpy(self->priv->data.class_id, ((SubgroupKey1*)obj)->priv->class_id);

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
            if ( !strcmp("class_id", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_class_id", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Class\"does not contain \"%s\"", field_name);
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

static PyObject *Class_fields(Class* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Class");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Class_class_id(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("class_id", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Class_name(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef Class_getset[] = {

    {"class_id",(getter)Class_class_id},

    {"name",(getter)Class_name},

    {NULL}
};

static PyMethodDef Class_methods[] = {
    {"set", (PyCFunction)Class_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Class_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Class_clone, METH_NOARGS, "clone"},

    {"set_class_id", (PyCFunction)Class_set_class_id, METH_VARARGS|METH_KEYWORDS, "Class_set_class_id"},

    {"set_name", (PyCFunction)Class_set_name, METH_VARARGS|METH_KEYWORDS, "Class_set_name"},

    {"get_prev", (PyCFunction)Class_prev, METH_VARARGS|METH_KEYWORDS, "Class_prev"},

    {"get_gt", (PyCFunction)Class_gt, METH_VARARGS|METH_KEYWORDS, "Class_gt"},

    {"get_next", (PyCFunction)Class_next, METH_VARARGS|METH_KEYWORDS, "Class_next"},

    {"get_ge", (PyCFunction)Class_ge, METH_VARARGS|METH_KEYWORDS, "Class_ge"},

    {"get_equal", (PyCFunction)Class_equal, METH_VARARGS|METH_KEYWORDS, "Class_equal"},

    {"get_last", (PyCFunction)Class_last, METH_VARARGS|METH_KEYWORDS, "Class_last"},

    {"get_lt", (PyCFunction)Class_lt, METH_VARARGS|METH_KEYWORDS, "Class_lt"},

    {"get_le", (PyCFunction)Class_le, METH_VARARGS|METH_KEYWORDS, "Class_le"},

    {"get_first", (PyCFunction)Class_first, METH_VARARGS|METH_KEYWORDS, "Class_first"},

    {"gets_gt", (PyCFunction)Class_iter_gt, METH_VARARGS|METH_KEYWORDS, "Class_iter_gt"},

    {"gets_ge", (PyCFunction)Class_iter_ge, METH_VARARGS|METH_KEYWORDS, "Class_iter_ge"},

    {"gets_equal", (PyCFunction)Class_iter_equal, METH_VARARGS|METH_KEYWORDS, "Class_iter_equal"},

    {"gets_last", (PyCFunction)Class_iter_last, METH_VARARGS|METH_KEYWORDS, "Class_iter_last"},

    {"gets_lt", (PyCFunction)Class_iter_lt, METH_VARARGS|METH_KEYWORDS, "Class_iter_lt"},

    {"gets_le", (PyCFunction)Class_iter_le, METH_VARARGS|METH_KEYWORDS, "Class_iter_le"},

    {"gets_first", (PyCFunction)Class_iter_first, METH_VARARGS|METH_KEYWORDS, "Class_iter_first"},

    {"update", (PyCFunction)Class_update, METH_VARARGS|METH_KEYWORDS, "Class_update"},

    {"insert", (PyCFunction)Class_insert, METH_VARARGS|METH_KEYWORDS, "Class_insert"},

    {"delete", (PyCFunction)Class_delete, METH_VARARGS|METH_KEYWORDS, "Class_delete"},

    {NULL}
};

static PyTypeObject ClassType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Class",             /*tp_name*/
    sizeof(Class),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Class_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Class_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Class objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Class_methods,             /* tp_methods */
    0,
    Class_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Class_init,      /* tp_init */
    0,                         /* tp_alloc */
    Class_new,                 /* tp_new */
};
PyTypeObject *getClassType()
{
    return &ClassType_;
}

static void ClassKey0_dealloc(PyObject* self)
{
    do_free(((ClassKey0*)self)->priv);
    Py_DECREF(((ClassKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ClassKey0 free\n");
}
static PyObject *ClassKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ClassKey0 new\n");
    ClassKey0 *self;
    self = PyObject_NEW(ClassKey0, type);
    //self = (ClassKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(class_key0_t));
    return (PyObject *)self;
}
static int ClassKey0_init(ClassKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ClassKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->class_id);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ClassKey0_clone(ClassKey0 *self)
{
    ClassKey0 *result;
    result = (ClassKey0*)PyObject_CallFunction((PyObject*)getClassKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ClassKey0_class_id(ClassKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ClassKey0_set_class_id(ClassKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set class_id");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->class_id, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->class_id);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ClassKey0_prev(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_gt(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_next(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_ge(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_equal(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_last(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_lt(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_le(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_first(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_class_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ClassKey0_iter_gt(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    class_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_class_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

 
        item = ClassKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_class_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ClassKey0_iter_ge(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    class_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_class_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

 
        item = ClassKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_class_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ClassKey0_iter_equal(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    class_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_class_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

 
        item = ClassKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_class_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ClassKey0_iter_last(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    class_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_class_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

 
        item = ClassKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_class_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ClassKey0_iter_lt(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    class_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_class_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

 
        item = ClassKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_class_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ClassKey0_iter_le(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    class_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_class_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

 
        item = ClassKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_class_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ClassKey0_iter_first(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    class_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_class_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

 
        item = ClassKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_class_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ClassKey0_cmp(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getClassKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->class_id, ((ClassKey0*)obj)->priv->class_id);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ClassKey0_set_alias(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ClassKey0*)self)->alias);
    Py_INCREF(obj);
    ((ClassKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ClassKey0_set(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getClassKey0Type() ) {
        memcpy(self->priv, ((ClassKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getClassType() ) {

        do_cpy(self->priv->class_id, ((Class*)obj)->priv->data.class_id);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_class_id", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getGroupType() ) {

        do_cpy(self->priv->class_id, ((Group*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey0Type() ) {

        do_cpy(self->priv->class_id, ((GroupKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey1Type() ) {

        do_cpy(self->priv->class_id, ((GroupKey1*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupType() ) {

        do_cpy(self->priv->class_id, ((Subgroup*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey0Type() ) {

        do_cpy(self->priv->class_id, ((SubgroupKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey1Type() ) {

        do_cpy(self->priv->class_id, ((SubgroupKey1*)obj)->priv->class_id);

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
            if ( !strcmp("class_id", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_class_id", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ClassKey0\"does not contain \"%s\"", field_name);
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

static PyObject *ClassKey0_fields(ClassKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ClassKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ClassKey0_class_id(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("class_id", value);
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

static PyGetSetDef ClassKey0_getset[] = {

    {"class_id",(getter)ClassKey0_class_id},

    {NULL}
};

static PyMethodDef ClassKey0_methods[] = {
    {"set", (PyCFunction)ClassKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ClassKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ClassKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ClassKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_class_id", (PyCFunction)ClassKey0_set_class_id, METH_VARARGS|METH_KEYWORDS, "ClassKey0_set_class_id"},

    {"get_prev", (PyCFunction)ClassKey0_prev, METH_VARARGS|METH_KEYWORDS, "ClassKey0_prev"},

    {"get_gt", (PyCFunction)ClassKey0_gt, METH_VARARGS|METH_KEYWORDS, "ClassKey0_gt"},

    {"get_next", (PyCFunction)ClassKey0_next, METH_VARARGS|METH_KEYWORDS, "ClassKey0_next"},

    {"get_ge", (PyCFunction)ClassKey0_ge, METH_VARARGS|METH_KEYWORDS, "ClassKey0_ge"},

    {"get_equal", (PyCFunction)ClassKey0_equal, METH_VARARGS|METH_KEYWORDS, "ClassKey0_equal"},

    {"get_last", (PyCFunction)ClassKey0_last, METH_VARARGS|METH_KEYWORDS, "ClassKey0_last"},

    {"get_lt", (PyCFunction)ClassKey0_lt, METH_VARARGS|METH_KEYWORDS, "ClassKey0_lt"},

    {"get_le", (PyCFunction)ClassKey0_le, METH_VARARGS|METH_KEYWORDS, "ClassKey0_le"},

    {"get_first", (PyCFunction)ClassKey0_first, METH_VARARGS|METH_KEYWORDS, "ClassKey0_first"},

    {"gets_gt", (PyCFunction)ClassKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "ClassKey0_iter_gt"},

    {"gets_ge", (PyCFunction)ClassKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "ClassKey0_iter_ge"},

    {"gets_equal", (PyCFunction)ClassKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "ClassKey0_iter_equal"},

    {"gets_last", (PyCFunction)ClassKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "ClassKey0_iter_last"},

    {"gets_lt", (PyCFunction)ClassKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "ClassKey0_iter_lt"},

    {"gets_le", (PyCFunction)ClassKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "ClassKey0_iter_le"},

    {"gets_first", (PyCFunction)ClassKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "ClassKey0_iter_first"},

    {NULL}
};

static PyTypeObject ClassKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ClassKey0",             /*tp_name*/
    sizeof(ClassKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ClassKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ClassKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ClassKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ClassKey0_methods,             /* tp_methods */
    0,
    ClassKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ClassKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ClassKey0_new,                 /* tp_new */
};
PyTypeObject *getClassKey0Type()
{
    return &ClassKey0Type_;
}
