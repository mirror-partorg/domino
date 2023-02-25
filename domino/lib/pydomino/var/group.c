
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "group.h"

static void Group_dealloc(PyObject* self)
{
    do_free(((Group*)self)->priv);
    Py_DECREF(((Group*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Group free\n");
}
static PyObject *Group_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Group new\n");
    Group *self;
    self = PyObject_NEW(Group, type);
    //self = (Group *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(group_rec_t));
    return (PyObject *)self;
}
static int Group_init(Group *self, PyObject *args, PyObject *kwds)
{
    //LOG("Group init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.group_id);

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.class_id);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Group_clone(Group *self)
{
    Group *result;
    result = (Group*)PyObject_CallFunction((PyObject*)getGroupType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Group_group_id(Group* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.group_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Group_set_group_id(Group* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.group_id");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.group_id, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.group_id);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Group_name(Group* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Group_set_name(Group* self, PyObject *args, PyObject *kwds)
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

static PyObject *Group_class_id(Group* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Group_set_class_id(Group* self, PyObject *args, PyObject *kwds)
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

static PyObject *Group_equal(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_next(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_prev(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_gt(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_ge(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_lt(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_le(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_first(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_last(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getGroupKey0Type() )
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getGroupKey1Type() )
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_iter_equal(Group* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getGroupKey0Type() ) {
        key_cmp = (group_key0_t*)do_malloc(sizeof(group_key0_t));
        memcpy(key_cmp, ((GroupKey0*)key)->priv, sizeof(group_key0_t));
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getGroupKey1Type() ) {
        key_cmp = (group_key1_t*)do_malloc(sizeof(group_key1_t));
        memcpy(key_cmp, ((GroupKey1*)key)->priv, sizeof(group_key1_t));
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->class_id, 
                    ((GroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->group_id, 
                    ((GroupKey0*)key)->priv->group_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->class_id, 
                    ((GroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->name, 
                    ((GroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Group_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
            status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
            status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Group_iter_gt(Group* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getGroupKey0Type() ) {
        key_cmp = (group_key0_t*)do_malloc(sizeof(group_key0_t));
        memcpy(key_cmp, ((GroupKey0*)key)->priv, sizeof(group_key0_t));
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getGroupKey1Type() ) {
        key_cmp = (group_key1_t*)do_malloc(sizeof(group_key1_t));
        memcpy(key_cmp, ((GroupKey1*)key)->priv, sizeof(group_key1_t));
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->class_id, 
                    ((GroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->group_id, 
                    ((GroupKey0*)key)->priv->group_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->class_id, 
                    ((GroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->name, 
                    ((GroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Group_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
            status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
            status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Group_iter_ge(Group* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getGroupKey0Type() ) {
        key_cmp = (group_key0_t*)do_malloc(sizeof(group_key0_t));
        memcpy(key_cmp, ((GroupKey0*)key)->priv, sizeof(group_key0_t));
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getGroupKey1Type() ) {
        key_cmp = (group_key1_t*)do_malloc(sizeof(group_key1_t));
        memcpy(key_cmp, ((GroupKey1*)key)->priv, sizeof(group_key1_t));
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->class_id, 
                    ((GroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->group_id, 
                    ((GroupKey0*)key)->priv->group_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->class_id, 
                    ((GroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->name, 
                    ((GroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Group_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
            status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
            status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Group_iter_lt(Group* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getGroupKey0Type() ) {
        key_cmp = (group_key0_t*)do_malloc(sizeof(group_key0_t));
        memcpy(key_cmp, ((GroupKey0*)key)->priv, sizeof(group_key0_t));
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getGroupKey1Type() ) {
        key_cmp = (group_key1_t*)do_malloc(sizeof(group_key1_t));
        memcpy(key_cmp, ((GroupKey1*)key)->priv, sizeof(group_key1_t));
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->class_id, 
                    ((GroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->group_id, 
                    ((GroupKey0*)key)->priv->group_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->class_id, 
                    ((GroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->name, 
                    ((GroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Group_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
            status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
            status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Group_iter_le(Group* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getGroupKey0Type() ) {
        key_cmp = (group_key0_t*)do_malloc(sizeof(group_key0_t));
        memcpy(key_cmp, ((GroupKey0*)key)->priv, sizeof(group_key0_t));
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getGroupKey1Type() ) {
        key_cmp = (group_key1_t*)do_malloc(sizeof(group_key1_t));
        memcpy(key_cmp, ((GroupKey1*)key)->priv, sizeof(group_key1_t));
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->class_id, 
                    ((GroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->group_id, 
                    ((GroupKey0*)key)->priv->group_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->class_id, 
                    ((GroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->name, 
                    ((GroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Group_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
            status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
            status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Group_iter_first(Group* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getGroupKey0Type() ) {
        key_cmp = (group_key0_t*)do_malloc(sizeof(group_key0_t));
        memcpy(key_cmp, ((GroupKey0*)key)->priv, sizeof(group_key0_t));
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getGroupKey1Type() ) {
        key_cmp = (group_key1_t*)do_malloc(sizeof(group_key1_t));
        memcpy(key_cmp, ((GroupKey1*)key)->priv, sizeof(group_key1_t));
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->class_id, 
                    ((GroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->group_id, 
                    ((GroupKey0*)key)->priv->group_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->class_id, 
                    ((GroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->name, 
                    ((GroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Group_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
            status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
            status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Group_iter_last(Group* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getGroupKey0Type() ) {
        key_cmp = (group_key0_t*)do_malloc(sizeof(group_key0_t));
        memcpy(key_cmp, ((GroupKey0*)key)->priv, sizeof(group_key0_t));
        status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getGroupKey1Type() ) {
        key_cmp = (group_key1_t*)do_malloc(sizeof(group_key1_t));
        memcpy(key_cmp, ((GroupKey1*)key)->priv, sizeof(group_key1_t));
        status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->class_id, 
                    ((GroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key0_t*)key_cmp)->group_id, 
                    ((GroupKey0*)key)->priv->group_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->class_id, 
                    ((GroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((group_key1_t*)key_cmp)->name, 
                    ((GroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Group_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getGroupKey0Type() ) {
            status = do_group_get0(self->alias->alias, self->priv, ((GroupKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getGroupKey1Type() ) {
            status = do_group_get1(self->alias->alias, self->priv, ((GroupKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Group_insert(Group* self)
{
    int status;
    status = do_group_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_update(Group* self)
{
    int status;
    status = do_group_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_delete(Group* self)
{
    int status;
    status = do_group_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Group_set_alias(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Group*)self)->alias);
    Py_INCREF(obj);
    ((Group*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Group_set(Group* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getGroupType() ) {
        memcpy(self->priv, ((Group*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getGroupKey0Type() ) {

        do_cpy(self->priv->data.group_id, ((GroupKey0*)obj)->priv->group_id);

        do_cpy(self->priv->data.class_id, ((GroupKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey1Type() ) {

        do_cpy(self->priv->data.name, ((GroupKey1*)obj)->priv->name);

        do_cpy(self->priv->data.class_id, ((GroupKey1*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getClassType() ) {

        do_cpy(self->priv->data.class_id, ((Class*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getClassKey0Type() ) {

        do_cpy(self->priv->data.class_id, ((ClassKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupType() ) {

        do_cpy(self->priv->data.group_id, ((Subgroup*)obj)->priv->data.group_id);

        do_cpy(self->priv->data.class_id, ((Subgroup*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey0Type() ) {

        do_cpy(self->priv->data.group_id, ((SubgroupKey0*)obj)->priv->group_id);

        do_cpy(self->priv->data.class_id, ((SubgroupKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey1Type() ) {

        do_cpy(self->priv->data.group_id, ((SubgroupKey1*)obj)->priv->group_id);

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
            if ( !strcmp("group_id", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_group_id", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("class_id", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_class_id", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Group\"does not contain \"%s\"", field_name);
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

static PyObject *Group_fields(Group* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Group");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Group_group_id(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("group_id", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Group_name(self,NULL);
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

    value =  Group_class_id(self,NULL);
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

static PyGetSetDef Group_getset[] = {

    {"group_id",(getter)Group_group_id},

    {"name",(getter)Group_name},

    {"class_id",(getter)Group_class_id},

    {NULL}
};

static PyMethodDef Group_methods[] = {
    {"set", (PyCFunction)Group_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Group_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Group_clone, METH_NOARGS, "clone"},

    {"set_group_id", (PyCFunction)Group_set_group_id, METH_VARARGS|METH_KEYWORDS, "Group_set_group_id"},

    {"set_name", (PyCFunction)Group_set_name, METH_VARARGS|METH_KEYWORDS, "Group_set_name"},

    {"set_class_id", (PyCFunction)Group_set_class_id, METH_VARARGS|METH_KEYWORDS, "Group_set_class_id"},

    {"get_equal", (PyCFunction)Group_equal, METH_VARARGS|METH_KEYWORDS, "Group_equal"},

    {"get_next", (PyCFunction)Group_next, METH_VARARGS|METH_KEYWORDS, "Group_next"},

    {"get_prev", (PyCFunction)Group_prev, METH_VARARGS|METH_KEYWORDS, "Group_prev"},

    {"get_gt", (PyCFunction)Group_gt, METH_VARARGS|METH_KEYWORDS, "Group_gt"},

    {"get_ge", (PyCFunction)Group_ge, METH_VARARGS|METH_KEYWORDS, "Group_ge"},

    {"get_lt", (PyCFunction)Group_lt, METH_VARARGS|METH_KEYWORDS, "Group_lt"},

    {"get_le", (PyCFunction)Group_le, METH_VARARGS|METH_KEYWORDS, "Group_le"},

    {"get_first", (PyCFunction)Group_first, METH_VARARGS|METH_KEYWORDS, "Group_first"},

    {"get_last", (PyCFunction)Group_last, METH_VARARGS|METH_KEYWORDS, "Group_last"},

    {"gets_equal", (PyCFunction)Group_iter_equal, METH_VARARGS|METH_KEYWORDS, "Group_iter_equal"},

    {"gets_gt", (PyCFunction)Group_iter_gt, METH_VARARGS|METH_KEYWORDS, "Group_iter_gt"},

    {"gets_ge", (PyCFunction)Group_iter_ge, METH_VARARGS|METH_KEYWORDS, "Group_iter_ge"},

    {"gets_lt", (PyCFunction)Group_iter_lt, METH_VARARGS|METH_KEYWORDS, "Group_iter_lt"},

    {"gets_le", (PyCFunction)Group_iter_le, METH_VARARGS|METH_KEYWORDS, "Group_iter_le"},

    {"gets_first", (PyCFunction)Group_iter_first, METH_VARARGS|METH_KEYWORDS, "Group_iter_first"},

    {"gets_last", (PyCFunction)Group_iter_last, METH_VARARGS|METH_KEYWORDS, "Group_iter_last"},

    {"insert", (PyCFunction)Group_insert, METH_VARARGS|METH_KEYWORDS, "Group_insert"},

    {"update", (PyCFunction)Group_update, METH_VARARGS|METH_KEYWORDS, "Group_update"},

    {"delete", (PyCFunction)Group_delete, METH_VARARGS|METH_KEYWORDS, "Group_delete"},

    {NULL}
};

static PyTypeObject GroupType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Group",             /*tp_name*/
    sizeof(Group),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Group_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Group_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Group objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Group_methods,             /* tp_methods */
    0,
    Group_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Group_init,      /* tp_init */
    0,                         /* tp_alloc */
    Group_new,                 /* tp_new */
};
PyTypeObject *getGroupType()
{
    return &GroupType_;
}

static void GroupKey0_dealloc(PyObject* self)
{
    do_free(((GroupKey0*)self)->priv);
    Py_DECREF(((GroupKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("GroupKey0 free\n");
}
static PyObject *GroupKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("GroupKey0 new\n");
    GroupKey0 *self;
    self = PyObject_NEW(GroupKey0, type);
    //self = (GroupKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(group_key0_t));
    return (PyObject *)self;
}
static int GroupKey0_init(GroupKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("GroupKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->class_id);

    do_text_set_empty(self->priv->group_id);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *GroupKey0_clone(GroupKey0 *self)
{
    GroupKey0 *result;
    result = (GroupKey0*)PyObject_CallFunction((PyObject*)getGroupKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *GroupKey0_class_id(GroupKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *GroupKey0_set_class_id(GroupKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *GroupKey0_group_id(GroupKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->group_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *GroupKey0_set_group_id(GroupKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set group_id");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->group_id, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->group_id);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *GroupKey0_equal(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_next(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_prev(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_gt(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_ge(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_lt(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_le(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_first(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_last(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey0_iter_equal(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

 
        item = GroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey0_iter_gt(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

 
        item = GroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey0_iter_ge(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

 
        item = GroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey0_iter_lt(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

 
        item = GroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey0_iter_le(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

 
        item = GroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey0_iter_first(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

 
        item = GroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey0_iter_last(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.group_id, 
                 self->priv->group_id))
               break;
       }

 
        item = GroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey0_cmp(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getGroupKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->class_id, ((GroupKey0*)obj)->priv->class_id);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->group_id, ((GroupKey0*)obj)->priv->group_id);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *GroupKey0_set_alias(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((GroupKey0*)self)->alias);
    Py_INCREF(obj);
    ((GroupKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *GroupKey0_set(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getGroupKey0Type() ) {
        memcpy(self->priv, ((GroupKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getGroupType() ) {

        do_cpy(self->priv->class_id, ((Group*)obj)->priv->data.class_id);

        do_cpy(self->priv->group_id, ((Group*)obj)->priv->data.group_id);

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

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_group_id", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getClassType() ) {

        do_cpy(self->priv->class_id, ((Class*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getClassKey0Type() ) {

        do_cpy(self->priv->class_id, ((ClassKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupType() ) {

        do_cpy(self->priv->class_id, ((Subgroup*)obj)->priv->data.class_id);

        do_cpy(self->priv->group_id, ((Subgroup*)obj)->priv->data.group_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey0Type() ) {

        do_cpy(self->priv->class_id, ((SubgroupKey0*)obj)->priv->class_id);

        do_cpy(self->priv->group_id, ((SubgroupKey0*)obj)->priv->group_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey1Type() ) {

        do_cpy(self->priv->class_id, ((SubgroupKey1*)obj)->priv->class_id);

        do_cpy(self->priv->group_id, ((SubgroupKey1*)obj)->priv->group_id);

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
            if ( !strcmp("group_id", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_group_id", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"GroupKey0\"does not contain \"%s\"", field_name);
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

static PyObject *GroupKey0_fields(GroupKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class GroupKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  GroupKey0_class_id(self,NULL);
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

    value =  GroupKey0_group_id(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("group_id", value);
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

static PyGetSetDef GroupKey0_getset[] = {

    {"class_id",(getter)GroupKey0_class_id},

    {"group_id",(getter)GroupKey0_group_id},

    {NULL}
};

static PyMethodDef GroupKey0_methods[] = {
    {"set", (PyCFunction)GroupKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)GroupKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)GroupKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)GroupKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_class_id", (PyCFunction)GroupKey0_set_class_id, METH_VARARGS|METH_KEYWORDS, "GroupKey0_set_class_id"},

    {"set_group_id", (PyCFunction)GroupKey0_set_group_id, METH_VARARGS|METH_KEYWORDS, "GroupKey0_set_group_id"},

    {"get_equal", (PyCFunction)GroupKey0_equal, METH_VARARGS|METH_KEYWORDS, "GroupKey0_equal"},

    {"get_next", (PyCFunction)GroupKey0_next, METH_VARARGS|METH_KEYWORDS, "GroupKey0_next"},

    {"get_prev", (PyCFunction)GroupKey0_prev, METH_VARARGS|METH_KEYWORDS, "GroupKey0_prev"},

    {"get_gt", (PyCFunction)GroupKey0_gt, METH_VARARGS|METH_KEYWORDS, "GroupKey0_gt"},

    {"get_ge", (PyCFunction)GroupKey0_ge, METH_VARARGS|METH_KEYWORDS, "GroupKey0_ge"},

    {"get_lt", (PyCFunction)GroupKey0_lt, METH_VARARGS|METH_KEYWORDS, "GroupKey0_lt"},

    {"get_le", (PyCFunction)GroupKey0_le, METH_VARARGS|METH_KEYWORDS, "GroupKey0_le"},

    {"get_first", (PyCFunction)GroupKey0_first, METH_VARARGS|METH_KEYWORDS, "GroupKey0_first"},

    {"get_last", (PyCFunction)GroupKey0_last, METH_VARARGS|METH_KEYWORDS, "GroupKey0_last"},

    {"gets_equal", (PyCFunction)GroupKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "GroupKey0_iter_equal"},

    {"gets_gt", (PyCFunction)GroupKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "GroupKey0_iter_gt"},

    {"gets_ge", (PyCFunction)GroupKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "GroupKey0_iter_ge"},

    {"gets_lt", (PyCFunction)GroupKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "GroupKey0_iter_lt"},

    {"gets_le", (PyCFunction)GroupKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "GroupKey0_iter_le"},

    {"gets_first", (PyCFunction)GroupKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "GroupKey0_iter_first"},

    {"gets_last", (PyCFunction)GroupKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "GroupKey0_iter_last"},

    {NULL}
};

static PyTypeObject GroupKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.GroupKey0",             /*tp_name*/
    sizeof(GroupKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)GroupKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)GroupKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "GroupKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    GroupKey0_methods,             /* tp_methods */
    0,
    GroupKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)GroupKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    GroupKey0_new,                 /* tp_new */
};
PyTypeObject *getGroupKey0Type()
{
    return &GroupKey0Type_;
}

static void GroupKey1_dealloc(PyObject* self)
{
    do_free(((GroupKey1*)self)->priv);
    Py_DECREF(((GroupKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("GroupKey1 free\n");
}
static PyObject *GroupKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("GroupKey1 new\n");
    GroupKey1 *self;
    self = PyObject_NEW(GroupKey1, type);
    //self = (GroupKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(group_key1_t));
    return (PyObject *)self;
}
static int GroupKey1_init(GroupKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("GroupKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->class_id);

    do_text_set_empty(self->priv->name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *GroupKey1_clone(GroupKey1 *self)
{
    GroupKey1 *result;
    result = (GroupKey1*)PyObject_CallFunction((PyObject*)getGroupKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *GroupKey1_class_id(GroupKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *GroupKey1_set_class_id(GroupKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *GroupKey1_name(GroupKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *GroupKey1_set_name(GroupKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set name");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->name, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *GroupKey1_equal(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_next(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_prev(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_gt(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_ge(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_lt(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_le(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_first(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_last(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_group_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *GroupKey1_iter_equal(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = GroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey1_iter_gt(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = GroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey1_iter_ge(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = GroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey1_iter_lt(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = GroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey1_iter_le(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = GroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey1_iter_first(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = GroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey1_iter_last(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    group_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_group_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.class_id, 
                 self->priv->class_id))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = GroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_group_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *GroupKey1_cmp(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getGroupKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->class_id, ((GroupKey1*)obj)->priv->class_id);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->name, ((GroupKey1*)obj)->priv->name);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *GroupKey1_set_alias(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((GroupKey1*)self)->alias);
    Py_INCREF(obj);
    ((GroupKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *GroupKey1_set(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getGroupKey1Type() ) {
        memcpy(self->priv, ((GroupKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getGroupType() ) {

        do_cpy(self->priv->class_id, ((Group*)obj)->priv->data.class_id);

        do_cpy(self->priv->name, ((Group*)obj)->priv->data.name);

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

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getClassType() ) {

        do_cpy(self->priv->class_id, ((Class*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getClassKey0Type() ) {

        do_cpy(self->priv->class_id, ((ClassKey0*)obj)->priv->class_id);

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

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"GroupKey1\"does not contain \"%s\"", field_name);
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

static PyObject *GroupKey1_fields(GroupKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class GroupKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  GroupKey1_class_id(self,NULL);
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

    value =  GroupKey1_name(self,NULL);
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

static PyGetSetDef GroupKey1_getset[] = {

    {"class_id",(getter)GroupKey1_class_id},

    {"name",(getter)GroupKey1_name},

    {NULL}
};

static PyMethodDef GroupKey1_methods[] = {
    {"set", (PyCFunction)GroupKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)GroupKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)GroupKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)GroupKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_class_id", (PyCFunction)GroupKey1_set_class_id, METH_VARARGS|METH_KEYWORDS, "GroupKey1_set_class_id"},

    {"set_name", (PyCFunction)GroupKey1_set_name, METH_VARARGS|METH_KEYWORDS, "GroupKey1_set_name"},

    {"get_equal", (PyCFunction)GroupKey1_equal, METH_VARARGS|METH_KEYWORDS, "GroupKey1_equal"},

    {"get_next", (PyCFunction)GroupKey1_next, METH_VARARGS|METH_KEYWORDS, "GroupKey1_next"},

    {"get_prev", (PyCFunction)GroupKey1_prev, METH_VARARGS|METH_KEYWORDS, "GroupKey1_prev"},

    {"get_gt", (PyCFunction)GroupKey1_gt, METH_VARARGS|METH_KEYWORDS, "GroupKey1_gt"},

    {"get_ge", (PyCFunction)GroupKey1_ge, METH_VARARGS|METH_KEYWORDS, "GroupKey1_ge"},

    {"get_lt", (PyCFunction)GroupKey1_lt, METH_VARARGS|METH_KEYWORDS, "GroupKey1_lt"},

    {"get_le", (PyCFunction)GroupKey1_le, METH_VARARGS|METH_KEYWORDS, "GroupKey1_le"},

    {"get_first", (PyCFunction)GroupKey1_first, METH_VARARGS|METH_KEYWORDS, "GroupKey1_first"},

    {"get_last", (PyCFunction)GroupKey1_last, METH_VARARGS|METH_KEYWORDS, "GroupKey1_last"},

    {"gets_equal", (PyCFunction)GroupKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "GroupKey1_iter_equal"},

    {"gets_gt", (PyCFunction)GroupKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "GroupKey1_iter_gt"},

    {"gets_ge", (PyCFunction)GroupKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "GroupKey1_iter_ge"},

    {"gets_lt", (PyCFunction)GroupKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "GroupKey1_iter_lt"},

    {"gets_le", (PyCFunction)GroupKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "GroupKey1_iter_le"},

    {"gets_first", (PyCFunction)GroupKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "GroupKey1_iter_first"},

    {"gets_last", (PyCFunction)GroupKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "GroupKey1_iter_last"},

    {NULL}
};

static PyTypeObject GroupKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.GroupKey1",             /*tp_name*/
    sizeof(GroupKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)GroupKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)GroupKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "GroupKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    GroupKey1_methods,             /* tp_methods */
    0,
    GroupKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)GroupKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    GroupKey1_new,                 /* tp_new */
};
PyTypeObject *getGroupKey1Type()
{
    return &GroupKey1Type_;
}
