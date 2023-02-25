
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "subgroup.h"

static void Subgroup_dealloc(PyObject* self)
{
    do_free(((Subgroup*)self)->priv);
    Py_DECREF(((Subgroup*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Subgroup free\n");
}
static PyObject *Subgroup_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Subgroup new\n");
    Subgroup *self;
    self = PyObject_NEW(Subgroup, type);
    //self = (Subgroup *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(subgroup_rec_t));
    return (PyObject *)self;
}
static int Subgroup_init(Subgroup *self, PyObject *args, PyObject *kwds)
{
    //LOG("Subgroup init\n");
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

    do_text_set_empty(self->priv->data.subgroup_id);

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.class_id);

    do_subgroup_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Subgroup_clone(Subgroup *self)
{
    Subgroup *result;
    result = (Subgroup*)PyObject_CallFunction((PyObject*)getSubgroupType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Subgroup_group_id(Subgroup* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.group_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Subgroup_set_group_id(Subgroup* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subgroup_subgroup_id(Subgroup* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.subgroup_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Subgroup_set_subgroup_id(Subgroup* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.subgroup_id");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.subgroup_id, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.subgroup_id);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Subgroup_name(Subgroup* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Subgroup_set_name(Subgroup* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subgroup_class_id(Subgroup* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Subgroup_set_class_id(Subgroup* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subgroup_state(Subgroup* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.stat);

    return result;
}

static PyObject *Subgroup_set_state(Subgroup* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.stat");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.stat = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.stat = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.stat");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.stat);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Subgroup_set_param(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_subgroup_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_subgroup_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_subgroup_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Subgroup_set_params(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_subgroup_params_clear(self->alias->alias,self->priv);
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
                 do_subgroup_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_subgroup_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_subgroup_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Subgroup_get_param(Subgroup* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_subgroup_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Subgroup_params_clear(Subgroup* self)
{
    do_subgroup_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Subgroup_get_params(Subgroup* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *Subgroup_equal(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_next(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_prev(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_gt(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_ge(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_lt(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_le(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_first(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_last(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubgroupKey0Type() )
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() )
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_iter_equal(Subgroup* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
        key_cmp = (subgroup_key0_t*)do_malloc(sizeof(subgroup_key0_t));
        memcpy(key_cmp, ((SubgroupKey0*)key)->priv, sizeof(subgroup_key0_t));
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
        key_cmp = (subgroup_key1_t*)do_malloc(sizeof(subgroup_key1_t));
        memcpy(key_cmp, ((SubgroupKey1*)key)->priv, sizeof(subgroup_key1_t));
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->class_id, 
                    ((SubgroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->group_id, 
                    ((SubgroupKey0*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->subgroup_id, 
                    ((SubgroupKey0*)key)->priv->subgroup_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->class_id, 
                    ((SubgroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->group_id, 
                    ((SubgroupKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->name, 
                    ((SubgroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subgroup_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
            status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
            status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Subgroup_iter_gt(Subgroup* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
        key_cmp = (subgroup_key0_t*)do_malloc(sizeof(subgroup_key0_t));
        memcpy(key_cmp, ((SubgroupKey0*)key)->priv, sizeof(subgroup_key0_t));
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
        key_cmp = (subgroup_key1_t*)do_malloc(sizeof(subgroup_key1_t));
        memcpy(key_cmp, ((SubgroupKey1*)key)->priv, sizeof(subgroup_key1_t));
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->class_id, 
                    ((SubgroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->group_id, 
                    ((SubgroupKey0*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->subgroup_id, 
                    ((SubgroupKey0*)key)->priv->subgroup_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->class_id, 
                    ((SubgroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->group_id, 
                    ((SubgroupKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->name, 
                    ((SubgroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subgroup_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
            status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
            status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Subgroup_iter_ge(Subgroup* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
        key_cmp = (subgroup_key0_t*)do_malloc(sizeof(subgroup_key0_t));
        memcpy(key_cmp, ((SubgroupKey0*)key)->priv, sizeof(subgroup_key0_t));
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
        key_cmp = (subgroup_key1_t*)do_malloc(sizeof(subgroup_key1_t));
        memcpy(key_cmp, ((SubgroupKey1*)key)->priv, sizeof(subgroup_key1_t));
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->class_id, 
                    ((SubgroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->group_id, 
                    ((SubgroupKey0*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->subgroup_id, 
                    ((SubgroupKey0*)key)->priv->subgroup_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->class_id, 
                    ((SubgroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->group_id, 
                    ((SubgroupKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->name, 
                    ((SubgroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subgroup_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
            status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
            status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Subgroup_iter_lt(Subgroup* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
        key_cmp = (subgroup_key0_t*)do_malloc(sizeof(subgroup_key0_t));
        memcpy(key_cmp, ((SubgroupKey0*)key)->priv, sizeof(subgroup_key0_t));
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
        key_cmp = (subgroup_key1_t*)do_malloc(sizeof(subgroup_key1_t));
        memcpy(key_cmp, ((SubgroupKey1*)key)->priv, sizeof(subgroup_key1_t));
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->class_id, 
                    ((SubgroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->group_id, 
                    ((SubgroupKey0*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->subgroup_id, 
                    ((SubgroupKey0*)key)->priv->subgroup_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->class_id, 
                    ((SubgroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->group_id, 
                    ((SubgroupKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->name, 
                    ((SubgroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subgroup_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
            status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
            status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Subgroup_iter_le(Subgroup* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
        key_cmp = (subgroup_key0_t*)do_malloc(sizeof(subgroup_key0_t));
        memcpy(key_cmp, ((SubgroupKey0*)key)->priv, sizeof(subgroup_key0_t));
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
        key_cmp = (subgroup_key1_t*)do_malloc(sizeof(subgroup_key1_t));
        memcpy(key_cmp, ((SubgroupKey1*)key)->priv, sizeof(subgroup_key1_t));
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->class_id, 
                    ((SubgroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->group_id, 
                    ((SubgroupKey0*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->subgroup_id, 
                    ((SubgroupKey0*)key)->priv->subgroup_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->class_id, 
                    ((SubgroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->group_id, 
                    ((SubgroupKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->name, 
                    ((SubgroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subgroup_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
            status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
            status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Subgroup_iter_first(Subgroup* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
        key_cmp = (subgroup_key0_t*)do_malloc(sizeof(subgroup_key0_t));
        memcpy(key_cmp, ((SubgroupKey0*)key)->priv, sizeof(subgroup_key0_t));
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
        key_cmp = (subgroup_key1_t*)do_malloc(sizeof(subgroup_key1_t));
        memcpy(key_cmp, ((SubgroupKey1*)key)->priv, sizeof(subgroup_key1_t));
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->class_id, 
                    ((SubgroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->group_id, 
                    ((SubgroupKey0*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->subgroup_id, 
                    ((SubgroupKey0*)key)->priv->subgroup_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->class_id, 
                    ((SubgroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->group_id, 
                    ((SubgroupKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->name, 
                    ((SubgroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subgroup_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
            status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
            status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Subgroup_iter_last(Subgroup* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
        key_cmp = (subgroup_key0_t*)do_malloc(sizeof(subgroup_key0_t));
        memcpy(key_cmp, ((SubgroupKey0*)key)->priv, sizeof(subgroup_key0_t));
        status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
        key_cmp = (subgroup_key1_t*)do_malloc(sizeof(subgroup_key1_t));
        memcpy(key_cmp, ((SubgroupKey1*)key)->priv, sizeof(subgroup_key1_t));
        status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->class_id, 
                    ((SubgroupKey0*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->group_id, 
                    ((SubgroupKey0*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key0_t*)key_cmp)->subgroup_id, 
                    ((SubgroupKey0*)key)->priv->subgroup_id))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->class_id, 
                    ((SubgroupKey1*)key)->priv->class_id))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->group_id, 
                    ((SubgroupKey1*)key)->priv->group_id))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subgroup_key1_t*)key_cmp)->name, 
                    ((SubgroupKey1*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subgroup_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubgroupKey0Type() ) {
            status = do_subgroup_get0(self->alias->alias, self->priv, ((SubgroupKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getSubgroupKey1Type() ) {
            status = do_subgroup_get1(self->alias->alias, self->priv, ((SubgroupKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Subgroup_insert(Subgroup* self)
{
    int status;
    status = do_subgroup_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_update(Subgroup* self)
{
    int status;
    status = do_subgroup_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_delete(Subgroup* self)
{
    int status;
    status = do_subgroup_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subgroup_set_alias(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Subgroup*)self)->alias);
    Py_INCREF(obj);
    ((Subgroup*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Subgroup_set(Subgroup* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getSubgroupType() ) {
        memcpy(self->priv, ((Subgroup*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey0Type() ) {

        do_cpy(self->priv->data.group_id, ((SubgroupKey0*)obj)->priv->group_id);

        do_cpy(self->priv->data.subgroup_id, ((SubgroupKey0*)obj)->priv->subgroup_id);

        do_cpy(self->priv->data.class_id, ((SubgroupKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getSubgroupKey1Type() ) {

        do_cpy(self->priv->data.group_id, ((SubgroupKey1*)obj)->priv->group_id);

        do_cpy(self->priv->data.name, ((SubgroupKey1*)obj)->priv->name);

        do_cpy(self->priv->data.class_id, ((SubgroupKey1*)obj)->priv->class_id);

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
    if ( Py_TYPE(obj) == getGroupType() ) {

        do_cpy(self->priv->data.group_id, ((Group*)obj)->priv->data.group_id);

        do_cpy(self->priv->data.class_id, ((Group*)obj)->priv->data.class_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey0Type() ) {

        do_cpy(self->priv->data.group_id, ((GroupKey0*)obj)->priv->group_id);

        do_cpy(self->priv->data.class_id, ((GroupKey0*)obj)->priv->class_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey1Type() ) {

        do_cpy(self->priv->data.class_id, ((GroupKey1*)obj)->priv->class_id);

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
            if ( !strcmp("subgroup_id", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_subgroup_id", "O", value) )
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

            else
            if ( !strcmp("state", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_state", "O", value) )
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
                do_log(LOG_ERR, "Class \"Subgroup\"does not contain \"%s\"", field_name);
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

static PyObject *Subgroup_fields(Subgroup* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Subgroup");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Subgroup_group_id(self,NULL);
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

    value =  Subgroup_subgroup_id(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("subgroup_id", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Subgroup_name(self,NULL);
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

    value =  Subgroup_class_id(self,NULL);
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

    value =  Subgroup_state(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("state", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Subgroup_get_params(self, NULL);
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

static PyGetSetDef Subgroup_getset[] = {

    {"group_id",(getter)Subgroup_group_id},

    {"subgroup_id",(getter)Subgroup_subgroup_id},

    {"name",(getter)Subgroup_name},

    {"class_id",(getter)Subgroup_class_id},

    {"state",(getter)Subgroup_state},

    {"params",(getter)Subgroup_get_params},

    {NULL}
};

static PyMethodDef Subgroup_methods[] = {
    {"set", (PyCFunction)Subgroup_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Subgroup_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Subgroup_clone, METH_NOARGS, "clone"},

    {"set_group_id", (PyCFunction)Subgroup_set_group_id, METH_VARARGS|METH_KEYWORDS, "Subgroup_set_group_id"},

    {"set_subgroup_id", (PyCFunction)Subgroup_set_subgroup_id, METH_VARARGS|METH_KEYWORDS, "Subgroup_set_subgroup_id"},

    {"set_name", (PyCFunction)Subgroup_set_name, METH_VARARGS|METH_KEYWORDS, "Subgroup_set_name"},

    {"set_class_id", (PyCFunction)Subgroup_set_class_id, METH_VARARGS|METH_KEYWORDS, "Subgroup_set_class_id"},

    {"set_state", (PyCFunction)Subgroup_set_state, METH_VARARGS|METH_KEYWORDS, "Subgroup_set_state"},

    {"set_param", (PyCFunction)Subgroup_set_param, METH_VARARGS|METH_KEYWORDS, "do_Subgroup_param_set"},
    {"param", (PyCFunction)Subgroup_get_param, METH_VARARGS|METH_KEYWORDS, "do_Subgroup_param"},
    {"clear_params", (PyCFunction)Subgroup_params_clear, METH_NOARGS, "do_Subgroup_param_clear"},
    {"set_params", (PyCFunction)Subgroup_set_params, METH_VARARGS|METH_KEYWORDS, "do_Subgroup_set_params"},

    {"get_equal", (PyCFunction)Subgroup_equal, METH_VARARGS|METH_KEYWORDS, "Subgroup_equal"},

    {"get_next", (PyCFunction)Subgroup_next, METH_VARARGS|METH_KEYWORDS, "Subgroup_next"},

    {"get_prev", (PyCFunction)Subgroup_prev, METH_VARARGS|METH_KEYWORDS, "Subgroup_prev"},

    {"get_gt", (PyCFunction)Subgroup_gt, METH_VARARGS|METH_KEYWORDS, "Subgroup_gt"},

    {"get_ge", (PyCFunction)Subgroup_ge, METH_VARARGS|METH_KEYWORDS, "Subgroup_ge"},

    {"get_lt", (PyCFunction)Subgroup_lt, METH_VARARGS|METH_KEYWORDS, "Subgroup_lt"},

    {"get_le", (PyCFunction)Subgroup_le, METH_VARARGS|METH_KEYWORDS, "Subgroup_le"},

    {"get_first", (PyCFunction)Subgroup_first, METH_VARARGS|METH_KEYWORDS, "Subgroup_first"},

    {"get_last", (PyCFunction)Subgroup_last, METH_VARARGS|METH_KEYWORDS, "Subgroup_last"},

    {"gets_equal", (PyCFunction)Subgroup_iter_equal, METH_VARARGS|METH_KEYWORDS, "Subgroup_iter_equal"},

    {"gets_gt", (PyCFunction)Subgroup_iter_gt, METH_VARARGS|METH_KEYWORDS, "Subgroup_iter_gt"},

    {"gets_ge", (PyCFunction)Subgroup_iter_ge, METH_VARARGS|METH_KEYWORDS, "Subgroup_iter_ge"},

    {"gets_lt", (PyCFunction)Subgroup_iter_lt, METH_VARARGS|METH_KEYWORDS, "Subgroup_iter_lt"},

    {"gets_le", (PyCFunction)Subgroup_iter_le, METH_VARARGS|METH_KEYWORDS, "Subgroup_iter_le"},

    {"gets_first", (PyCFunction)Subgroup_iter_first, METH_VARARGS|METH_KEYWORDS, "Subgroup_iter_first"},

    {"gets_last", (PyCFunction)Subgroup_iter_last, METH_VARARGS|METH_KEYWORDS, "Subgroup_iter_last"},

    {"insert", (PyCFunction)Subgroup_insert, METH_VARARGS|METH_KEYWORDS, "Subgroup_insert"},

    {"update", (PyCFunction)Subgroup_update, METH_VARARGS|METH_KEYWORDS, "Subgroup_update"},

    {"delete", (PyCFunction)Subgroup_delete, METH_VARARGS|METH_KEYWORDS, "Subgroup_delete"},

    {NULL}
};

static PyTypeObject SubgroupType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Subgroup",             /*tp_name*/
    sizeof(Subgroup),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Subgroup_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Subgroup_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Subgroup objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Subgroup_methods,             /* tp_methods */
    0,
    Subgroup_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Subgroup_init,      /* tp_init */
    0,                         /* tp_alloc */
    Subgroup_new,                 /* tp_new */
};
PyTypeObject *getSubgroupType()
{
    return &SubgroupType_;
}

static void SubgroupKey0_dealloc(PyObject* self)
{
    do_free(((SubgroupKey0*)self)->priv);
    Py_DECREF(((SubgroupKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("SubgroupKey0 free\n");
}
static PyObject *SubgroupKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("SubgroupKey0 new\n");
    SubgroupKey0 *self;
    self = PyObject_NEW(SubgroupKey0, type);
    //self = (SubgroupKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(subgroup_key0_t));
    return (PyObject *)self;
}
static int SubgroupKey0_init(SubgroupKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("SubgroupKey0 init\n");
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

    do_text_set_empty(self->priv->subgroup_id);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *SubgroupKey0_clone(SubgroupKey0 *self)
{
    SubgroupKey0 *result;
    result = (SubgroupKey0*)PyObject_CallFunction((PyObject*)getSubgroupKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *SubgroupKey0_class_id(SubgroupKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SubgroupKey0_set_class_id(SubgroupKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *SubgroupKey0_group_id(SubgroupKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->group_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SubgroupKey0_set_group_id(SubgroupKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *SubgroupKey0_subgroup_id(SubgroupKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->subgroup_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SubgroupKey0_set_subgroup_id(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set subgroup_id");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->subgroup_id, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->subgroup_id);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SubgroupKey0_equal(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_next(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_prev(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_gt(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_ge(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_lt(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_le(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_first(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_last(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey0_iter_equal(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

 
        item = SubgroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey0_iter_gt(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_GT);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

 
        item = SubgroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey0_iter_ge(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_GE);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

 
        item = SubgroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey0_iter_lt(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_LT);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

 
        item = SubgroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey0_iter_le(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_LE);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

 
        item = SubgroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey0_iter_first(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_FIRST);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

 
        item = SubgroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey0_iter_last(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_LAST);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.subgroup_id, 
                 self->priv->subgroup_id))
               break;
       }

 
        item = SubgroupKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey0_cmp(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getSubgroupKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->class_id, ((SubgroupKey0*)obj)->priv->class_id);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->group_id, ((SubgroupKey0*)obj)->priv->group_id);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->subgroup_id, ((SubgroupKey0*)obj)->priv->subgroup_id);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *SubgroupKey0_set_alias(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((SubgroupKey0*)self)->alias);
    Py_INCREF(obj);
    ((SubgroupKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *SubgroupKey0_set(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getSubgroupKey0Type() ) {
        memcpy(self->priv, ((SubgroupKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getSubgroupType() ) {

        do_cpy(self->priv->class_id, ((Subgroup*)obj)->priv->data.class_id);

        do_cpy(self->priv->group_id, ((Subgroup*)obj)->priv->data.group_id);

        do_cpy(self->priv->subgroup_id, ((Subgroup*)obj)->priv->data.subgroup_id);

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

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_subgroup_id", "O", value) )
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
    if ( Py_TYPE(obj) == getGroupType() ) {

        do_cpy(self->priv->class_id, ((Group*)obj)->priv->data.class_id);

        do_cpy(self->priv->group_id, ((Group*)obj)->priv->data.group_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey0Type() ) {

        do_cpy(self->priv->class_id, ((GroupKey0*)obj)->priv->class_id);

        do_cpy(self->priv->group_id, ((GroupKey0*)obj)->priv->group_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey1Type() ) {

        do_cpy(self->priv->class_id, ((GroupKey1*)obj)->priv->class_id);

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

            else
            if ( !strcmp("subgroup_id", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_subgroup_id", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"SubgroupKey0\"does not contain \"%s\"", field_name);
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

static PyObject *SubgroupKey0_fields(SubgroupKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class SubgroupKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  SubgroupKey0_class_id(self,NULL);
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

    value =  SubgroupKey0_group_id(self,NULL);
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

    value =  SubgroupKey0_subgroup_id(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("subgroup_id", value);
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

static PyGetSetDef SubgroupKey0_getset[] = {

    {"class_id",(getter)SubgroupKey0_class_id},

    {"group_id",(getter)SubgroupKey0_group_id},

    {"subgroup_id",(getter)SubgroupKey0_subgroup_id},

    {NULL}
};

static PyMethodDef SubgroupKey0_methods[] = {
    {"set", (PyCFunction)SubgroupKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)SubgroupKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)SubgroupKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)SubgroupKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_class_id", (PyCFunction)SubgroupKey0_set_class_id, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_set_class_id"},

    {"set_group_id", (PyCFunction)SubgroupKey0_set_group_id, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_set_group_id"},

    {"set_subgroup_id", (PyCFunction)SubgroupKey0_set_subgroup_id, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_set_subgroup_id"},

    {"get_equal", (PyCFunction)SubgroupKey0_equal, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_equal"},

    {"get_next", (PyCFunction)SubgroupKey0_next, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_next"},

    {"get_prev", (PyCFunction)SubgroupKey0_prev, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_prev"},

    {"get_gt", (PyCFunction)SubgroupKey0_gt, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_gt"},

    {"get_ge", (PyCFunction)SubgroupKey0_ge, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_ge"},

    {"get_lt", (PyCFunction)SubgroupKey0_lt, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_lt"},

    {"get_le", (PyCFunction)SubgroupKey0_le, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_le"},

    {"get_first", (PyCFunction)SubgroupKey0_first, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_first"},

    {"get_last", (PyCFunction)SubgroupKey0_last, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_last"},

    {"gets_equal", (PyCFunction)SubgroupKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_iter_equal"},

    {"gets_gt", (PyCFunction)SubgroupKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_iter_gt"},

    {"gets_ge", (PyCFunction)SubgroupKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_iter_ge"},

    {"gets_lt", (PyCFunction)SubgroupKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_iter_lt"},

    {"gets_le", (PyCFunction)SubgroupKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_iter_le"},

    {"gets_first", (PyCFunction)SubgroupKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_iter_first"},

    {"gets_last", (PyCFunction)SubgroupKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "SubgroupKey0_iter_last"},

    {NULL}
};

static PyTypeObject SubgroupKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.SubgroupKey0",             /*tp_name*/
    sizeof(SubgroupKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SubgroupKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)SubgroupKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SubgroupKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SubgroupKey0_methods,             /* tp_methods */
    0,
    SubgroupKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SubgroupKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    SubgroupKey0_new,                 /* tp_new */
};
PyTypeObject *getSubgroupKey0Type()
{
    return &SubgroupKey0Type_;
}

static void SubgroupKey1_dealloc(PyObject* self)
{
    do_free(((SubgroupKey1*)self)->priv);
    Py_DECREF(((SubgroupKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("SubgroupKey1 free\n");
}
static PyObject *SubgroupKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("SubgroupKey1 new\n");
    SubgroupKey1 *self;
    self = PyObject_NEW(SubgroupKey1, type);
    //self = (SubgroupKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(subgroup_key1_t));
    return (PyObject *)self;
}
static int SubgroupKey1_init(SubgroupKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("SubgroupKey1 init\n");
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

    do_text_set_empty(self->priv->name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *SubgroupKey1_clone(SubgroupKey1 *self)
{
    SubgroupKey1 *result;
    result = (SubgroupKey1*)PyObject_CallFunction((PyObject*)getSubgroupKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *SubgroupKey1_class_id(SubgroupKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->class_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SubgroupKey1_set_class_id(SubgroupKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *SubgroupKey1_group_id(SubgroupKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->group_id);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SubgroupKey1_set_group_id(SubgroupKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *SubgroupKey1_name(SubgroupKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SubgroupKey1_set_name(SubgroupKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *SubgroupKey1_equal(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_next(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_prev(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_gt(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_ge(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_lt(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_le(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_first(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_last(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubgroupKey1_iter_equal(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = SubgroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey1_iter_gt(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_GT);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = SubgroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey1_iter_ge(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_GE);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = SubgroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey1_iter_lt(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_LT);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = SubgroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey1_iter_le(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_LE);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = SubgroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey1_iter_first(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_FIRST);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = SubgroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey1_iter_last(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subgroup_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_LAST);
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

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = SubgroupKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subgroup_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubgroupKey1_cmp(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getSubgroupKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->class_id, ((SubgroupKey1*)obj)->priv->class_id);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->group_id, ((SubgroupKey1*)obj)->priv->group_id);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->name, ((SubgroupKey1*)obj)->priv->name);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *SubgroupKey1_set_alias(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((SubgroupKey1*)self)->alias);
    Py_INCREF(obj);
    ((SubgroupKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *SubgroupKey1_set(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getSubgroupKey1Type() ) {
        memcpy(self->priv, ((SubgroupKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getSubgroupType() ) {

        do_cpy(self->priv->class_id, ((Subgroup*)obj)->priv->data.class_id);

        do_cpy(self->priv->group_id, ((Subgroup*)obj)->priv->data.group_id);

        do_cpy(self->priv->name, ((Subgroup*)obj)->priv->data.name);

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

                case 2:
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
    if ( Py_TYPE(obj) == getGroupType() ) {

        do_cpy(self->priv->class_id, ((Group*)obj)->priv->data.class_id);

        do_cpy(self->priv->group_id, ((Group*)obj)->priv->data.group_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey0Type() ) {

        do_cpy(self->priv->class_id, ((GroupKey0*)obj)->priv->class_id);

        do_cpy(self->priv->group_id, ((GroupKey0*)obj)->priv->group_id);

    }

    else 
    if ( Py_TYPE(obj) == getGroupKey1Type() ) {

        do_cpy(self->priv->class_id, ((GroupKey1*)obj)->priv->class_id);

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

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"SubgroupKey1\"does not contain \"%s\"", field_name);
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

static PyObject *SubgroupKey1_fields(SubgroupKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class SubgroupKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  SubgroupKey1_class_id(self,NULL);
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

    value =  SubgroupKey1_group_id(self,NULL);
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

    value =  SubgroupKey1_name(self,NULL);
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

static PyGetSetDef SubgroupKey1_getset[] = {

    {"class_id",(getter)SubgroupKey1_class_id},

    {"group_id",(getter)SubgroupKey1_group_id},

    {"name",(getter)SubgroupKey1_name},

    {NULL}
};

static PyMethodDef SubgroupKey1_methods[] = {
    {"set", (PyCFunction)SubgroupKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)SubgroupKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)SubgroupKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)SubgroupKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_class_id", (PyCFunction)SubgroupKey1_set_class_id, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_set_class_id"},

    {"set_group_id", (PyCFunction)SubgroupKey1_set_group_id, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_set_group_id"},

    {"set_name", (PyCFunction)SubgroupKey1_set_name, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_set_name"},

    {"get_equal", (PyCFunction)SubgroupKey1_equal, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_equal"},

    {"get_next", (PyCFunction)SubgroupKey1_next, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_next"},

    {"get_prev", (PyCFunction)SubgroupKey1_prev, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_prev"},

    {"get_gt", (PyCFunction)SubgroupKey1_gt, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_gt"},

    {"get_ge", (PyCFunction)SubgroupKey1_ge, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_ge"},

    {"get_lt", (PyCFunction)SubgroupKey1_lt, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_lt"},

    {"get_le", (PyCFunction)SubgroupKey1_le, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_le"},

    {"get_first", (PyCFunction)SubgroupKey1_first, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_first"},

    {"get_last", (PyCFunction)SubgroupKey1_last, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_last"},

    {"gets_equal", (PyCFunction)SubgroupKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_iter_equal"},

    {"gets_gt", (PyCFunction)SubgroupKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_iter_gt"},

    {"gets_ge", (PyCFunction)SubgroupKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_iter_ge"},

    {"gets_lt", (PyCFunction)SubgroupKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_iter_lt"},

    {"gets_le", (PyCFunction)SubgroupKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_iter_le"},

    {"gets_first", (PyCFunction)SubgroupKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_iter_first"},

    {"gets_last", (PyCFunction)SubgroupKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "SubgroupKey1_iter_last"},

    {NULL}
};

static PyTypeObject SubgroupKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.SubgroupKey1",             /*tp_name*/
    sizeof(SubgroupKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SubgroupKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)SubgroupKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SubgroupKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SubgroupKey1_methods,             /* tp_methods */
    0,
    SubgroupKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SubgroupKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    SubgroupKey1_new,                 /* tp_new */
};
PyTypeObject *getSubgroupKey1Type()
{
    return &SubgroupKey1Type_;
}
