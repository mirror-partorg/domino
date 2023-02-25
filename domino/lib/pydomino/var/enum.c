
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "enum.h"

static void Enum_dealloc(PyObject* self)
{
    do_free(((Enum*)self)->priv);
    Py_DECREF(((Enum*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Enum free\n");
}
static PyObject *Enum_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Enum new\n");
    Enum *self;
    self = PyObject_NEW(Enum, type);
    //self = (Enum *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(enum_rec_t));
    return (PyObject *)self;
}
static int Enum_init(Enum *self, PyObject *args, PyObject *kwds)
{
    //LOG("Enum init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.format);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Enum_clone(Enum *self)
{
    Enum *result;
    result = (Enum*)PyObject_CallFunction((PyObject*)getEnumType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Enum_name(Enum* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Enum_set_name(Enum* self, PyObject *args, PyObject *kwds)
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

static PyObject *Enum_last_value(Enum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.lastvalue);

    return result;
}

static PyObject *Enum_set_last_value(Enum* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.lastvalue");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.lastvalue = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.lastvalue = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.lastvalue");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.lastvalue);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Enum_step(Enum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.step);

    return result;
}

static PyObject *Enum_set_step(Enum* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.step");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.step = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.step = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.step");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.step);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Enum_low_bound(Enum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.LowBound);

    return result;
}

static PyObject *Enum_set_low_bound(Enum* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.LowBound");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.LowBound = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.LowBound = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.LowBound");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.LowBound);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Enum_hi_bound(Enum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.hiBound);

    return result;
}

static PyObject *Enum_set_hi_bound(Enum* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.hiBound");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.hiBound = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.hiBound = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.hiBound");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.hiBound);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Enum_cyclical(Enum* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.cycleFl);

    return result;
}

static PyObject *Enum_set_cyclical(Enum* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.cycleFl");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.cycleFl = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.cycleFl = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.cycleFl");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.cycleFl);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Enum_format(Enum* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.format);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Enum_set_format(Enum* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.format");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.format, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.format);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Enum_equal(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_next(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_prev(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_gt(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_ge(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_lt(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_le(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_first(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_last(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getEnumKey0Type() )
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_iter_equal(Enum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getEnumKey0Type() ) {
        key_cmp = (enum_key0_t*)do_malloc(sizeof(enum_key0_t));
        memcpy(key_cmp, ((EnumKey0*)key)->priv, sizeof(enum_key0_t));
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((enum_key0_t*)key_cmp)->name, 
                    ((EnumKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Enum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
            status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Enum_iter_gt(Enum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getEnumKey0Type() ) {
        key_cmp = (enum_key0_t*)do_malloc(sizeof(enum_key0_t));
        memcpy(key_cmp, ((EnumKey0*)key)->priv, sizeof(enum_key0_t));
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((enum_key0_t*)key_cmp)->name, 
                    ((EnumKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Enum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
            status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Enum_iter_ge(Enum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getEnumKey0Type() ) {
        key_cmp = (enum_key0_t*)do_malloc(sizeof(enum_key0_t));
        memcpy(key_cmp, ((EnumKey0*)key)->priv, sizeof(enum_key0_t));
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((enum_key0_t*)key_cmp)->name, 
                    ((EnumKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Enum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
            status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Enum_iter_lt(Enum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getEnumKey0Type() ) {
        key_cmp = (enum_key0_t*)do_malloc(sizeof(enum_key0_t));
        memcpy(key_cmp, ((EnumKey0*)key)->priv, sizeof(enum_key0_t));
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((enum_key0_t*)key_cmp)->name, 
                    ((EnumKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Enum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
            status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Enum_iter_le(Enum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getEnumKey0Type() ) {
        key_cmp = (enum_key0_t*)do_malloc(sizeof(enum_key0_t));
        memcpy(key_cmp, ((EnumKey0*)key)->priv, sizeof(enum_key0_t));
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((enum_key0_t*)key_cmp)->name, 
                    ((EnumKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Enum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
            status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Enum_iter_first(Enum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getEnumKey0Type() ) {
        key_cmp = (enum_key0_t*)do_malloc(sizeof(enum_key0_t));
        memcpy(key_cmp, ((EnumKey0*)key)->priv, sizeof(enum_key0_t));
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((enum_key0_t*)key_cmp)->name, 
                    ((EnumKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Enum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
            status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Enum_iter_last(Enum* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getEnumKey0Type() ) {
        key_cmp = (enum_key0_t*)do_malloc(sizeof(enum_key0_t));
        memcpy(key_cmp, ((EnumKey0*)key)->priv, sizeof(enum_key0_t));
        status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((enum_key0_t*)key_cmp)->name, 
                    ((EnumKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Enum_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getEnumKey0Type() ) {
            status = do_enum_get0(self->alias->alias, self->priv, ((EnumKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Enum_insert(Enum* self)
{
    int status;
    status = do_enum_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_update(Enum* self)
{
    int status;
    status = do_enum_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_delete(Enum* self)
{
    int status;
    status = do_enum_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Enum_set_alias(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Enum*)self)->alias);
    Py_INCREF(obj);
    ((Enum*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Enum_set(Enum* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getEnumType() ) {
        memcpy(self->priv, ((Enum*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getEnumKey0Type() ) {

        do_cpy(self->priv->data.name, ((EnumKey0*)obj)->priv->name);

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
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("last_value", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_last_value", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("step", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_step", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("low_bound", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_low_bound", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("hi_bound", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_hi_bound", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("cyclical", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_cyclical", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("format", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_format", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Enum\"does not contain \"%s\"", field_name);
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

static PyObject *Enum_fields(Enum* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Enum");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Enum_name(self,NULL);
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

    value =  Enum_last_value(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("last_value", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Enum_step(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("step", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Enum_low_bound(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("low_bound", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Enum_hi_bound(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("hi_bound", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Enum_cyclical(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("cyclical", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Enum_format(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("format", value);
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

static PyGetSetDef Enum_getset[] = {

    {"name",(getter)Enum_name},

    {"last_value",(getter)Enum_last_value},

    {"step",(getter)Enum_step},

    {"low_bound",(getter)Enum_low_bound},

    {"hi_bound",(getter)Enum_hi_bound},

    {"cyclical",(getter)Enum_cyclical},

    {"format",(getter)Enum_format},

    {NULL}
};

static PyMethodDef Enum_methods[] = {
    {"set", (PyCFunction)Enum_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Enum_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Enum_clone, METH_NOARGS, "clone"},

    {"set_name", (PyCFunction)Enum_set_name, METH_VARARGS|METH_KEYWORDS, "Enum_set_name"},

    {"set_last_value", (PyCFunction)Enum_set_last_value, METH_VARARGS|METH_KEYWORDS, "Enum_set_last_value"},

    {"set_step", (PyCFunction)Enum_set_step, METH_VARARGS|METH_KEYWORDS, "Enum_set_step"},

    {"set_low_bound", (PyCFunction)Enum_set_low_bound, METH_VARARGS|METH_KEYWORDS, "Enum_set_low_bound"},

    {"set_hi_bound", (PyCFunction)Enum_set_hi_bound, METH_VARARGS|METH_KEYWORDS, "Enum_set_hi_bound"},

    {"set_cyclical", (PyCFunction)Enum_set_cyclical, METH_VARARGS|METH_KEYWORDS, "Enum_set_cyclical"},

    {"set_format", (PyCFunction)Enum_set_format, METH_VARARGS|METH_KEYWORDS, "Enum_set_format"},

    {"get_equal", (PyCFunction)Enum_equal, METH_VARARGS|METH_KEYWORDS, "Enum_equal"},

    {"get_next", (PyCFunction)Enum_next, METH_VARARGS|METH_KEYWORDS, "Enum_next"},

    {"get_prev", (PyCFunction)Enum_prev, METH_VARARGS|METH_KEYWORDS, "Enum_prev"},

    {"get_gt", (PyCFunction)Enum_gt, METH_VARARGS|METH_KEYWORDS, "Enum_gt"},

    {"get_ge", (PyCFunction)Enum_ge, METH_VARARGS|METH_KEYWORDS, "Enum_ge"},

    {"get_lt", (PyCFunction)Enum_lt, METH_VARARGS|METH_KEYWORDS, "Enum_lt"},

    {"get_le", (PyCFunction)Enum_le, METH_VARARGS|METH_KEYWORDS, "Enum_le"},

    {"get_first", (PyCFunction)Enum_first, METH_VARARGS|METH_KEYWORDS, "Enum_first"},

    {"get_last", (PyCFunction)Enum_last, METH_VARARGS|METH_KEYWORDS, "Enum_last"},

    {"gets_equal", (PyCFunction)Enum_iter_equal, METH_VARARGS|METH_KEYWORDS, "Enum_iter_equal"},

    {"gets_gt", (PyCFunction)Enum_iter_gt, METH_VARARGS|METH_KEYWORDS, "Enum_iter_gt"},

    {"gets_ge", (PyCFunction)Enum_iter_ge, METH_VARARGS|METH_KEYWORDS, "Enum_iter_ge"},

    {"gets_lt", (PyCFunction)Enum_iter_lt, METH_VARARGS|METH_KEYWORDS, "Enum_iter_lt"},

    {"gets_le", (PyCFunction)Enum_iter_le, METH_VARARGS|METH_KEYWORDS, "Enum_iter_le"},

    {"gets_first", (PyCFunction)Enum_iter_first, METH_VARARGS|METH_KEYWORDS, "Enum_iter_first"},

    {"gets_last", (PyCFunction)Enum_iter_last, METH_VARARGS|METH_KEYWORDS, "Enum_iter_last"},

    {"insert", (PyCFunction)Enum_insert, METH_VARARGS|METH_KEYWORDS, "Enum_insert"},

    {"update", (PyCFunction)Enum_update, METH_VARARGS|METH_KEYWORDS, "Enum_update"},

    {"delete", (PyCFunction)Enum_delete, METH_VARARGS|METH_KEYWORDS, "Enum_delete"},

    {NULL}
};

static PyTypeObject EnumType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Enum",             /*tp_name*/
    sizeof(Enum),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Enum_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Enum_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Enum objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Enum_methods,             /* tp_methods */
    0,
    Enum_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Enum_init,      /* tp_init */
    0,                         /* tp_alloc */
    Enum_new,                 /* tp_new */
};
PyTypeObject *getEnumType()
{
    return &EnumType_;
}

static void EnumKey0_dealloc(PyObject* self)
{
    do_free(((EnumKey0*)self)->priv);
    Py_DECREF(((EnumKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("EnumKey0 free\n");
}
static PyObject *EnumKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("EnumKey0 new\n");
    EnumKey0 *self;
    self = PyObject_NEW(EnumKey0, type);
    //self = (EnumKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(enum_key0_t));
    return (PyObject *)self;
}
static int EnumKey0_init(EnumKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("EnumKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *EnumKey0_clone(EnumKey0 *self)
{
    EnumKey0 *result;
    result = (EnumKey0*)PyObject_CallFunction((PyObject*)getEnumKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *EnumKey0_name(EnumKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *EnumKey0_set_name(EnumKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *EnumKey0_equal(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_next(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_prev(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_gt(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_ge(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_lt(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_le(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_first(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_last(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *EnumKey0_iter_equal(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    enum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = EnumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_enum_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *EnumKey0_iter_gt(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    enum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = EnumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_enum_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *EnumKey0_iter_ge(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    enum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = EnumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_enum_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *EnumKey0_iter_lt(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    enum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = EnumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_enum_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *EnumKey0_iter_le(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    enum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = EnumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_enum_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *EnumKey0_iter_first(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    enum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = EnumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_enum_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *EnumKey0_iter_last(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    enum_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_enum_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = EnumKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_enum_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *EnumKey0_cmp(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getEnumKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->name, ((EnumKey0*)obj)->priv->name);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *EnumKey0_set_alias(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((EnumKey0*)self)->alias);
    Py_INCREF(obj);
    ((EnumKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *EnumKey0_set(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getEnumKey0Type() ) {
        memcpy(self->priv, ((EnumKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getEnumType() ) {

        do_cpy(self->priv->name, ((Enum*)obj)->priv->data.name);

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
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"EnumKey0\"does not contain \"%s\"", field_name);
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

static PyObject *EnumKey0_fields(EnumKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class EnumKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  EnumKey0_name(self,NULL);
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

static PyGetSetDef EnumKey0_getset[] = {

    {"name",(getter)EnumKey0_name},

    {NULL}
};

static PyMethodDef EnumKey0_methods[] = {
    {"set", (PyCFunction)EnumKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)EnumKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)EnumKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)EnumKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_name", (PyCFunction)EnumKey0_set_name, METH_VARARGS|METH_KEYWORDS, "EnumKey0_set_name"},

    {"get_equal", (PyCFunction)EnumKey0_equal, METH_VARARGS|METH_KEYWORDS, "EnumKey0_equal"},

    {"get_next", (PyCFunction)EnumKey0_next, METH_VARARGS|METH_KEYWORDS, "EnumKey0_next"},

    {"get_prev", (PyCFunction)EnumKey0_prev, METH_VARARGS|METH_KEYWORDS, "EnumKey0_prev"},

    {"get_gt", (PyCFunction)EnumKey0_gt, METH_VARARGS|METH_KEYWORDS, "EnumKey0_gt"},

    {"get_ge", (PyCFunction)EnumKey0_ge, METH_VARARGS|METH_KEYWORDS, "EnumKey0_ge"},

    {"get_lt", (PyCFunction)EnumKey0_lt, METH_VARARGS|METH_KEYWORDS, "EnumKey0_lt"},

    {"get_le", (PyCFunction)EnumKey0_le, METH_VARARGS|METH_KEYWORDS, "EnumKey0_le"},

    {"get_first", (PyCFunction)EnumKey0_first, METH_VARARGS|METH_KEYWORDS, "EnumKey0_first"},

    {"get_last", (PyCFunction)EnumKey0_last, METH_VARARGS|METH_KEYWORDS, "EnumKey0_last"},

    {"gets_equal", (PyCFunction)EnumKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "EnumKey0_iter_equal"},

    {"gets_gt", (PyCFunction)EnumKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "EnumKey0_iter_gt"},

    {"gets_ge", (PyCFunction)EnumKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "EnumKey0_iter_ge"},

    {"gets_lt", (PyCFunction)EnumKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "EnumKey0_iter_lt"},

    {"gets_le", (PyCFunction)EnumKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "EnumKey0_iter_le"},

    {"gets_first", (PyCFunction)EnumKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "EnumKey0_iter_first"},

    {"gets_last", (PyCFunction)EnumKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "EnumKey0_iter_last"},

    {NULL}
};

static PyTypeObject EnumKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.EnumKey0",             /*tp_name*/
    sizeof(EnumKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)EnumKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)EnumKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "EnumKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    EnumKey0_methods,             /* tp_methods */
    0,
    EnumKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)EnumKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    EnumKey0_new,                 /* tp_new */
};
PyTypeObject *getEnumKey0Type()
{
    return &EnumKey0Type_;
}
