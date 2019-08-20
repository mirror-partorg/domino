
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "user.h"

static void User_dealloc(PyObject* self)
{
    do_free(((User*)self)->priv);
    Py_DECREF(((User*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("User free\n");
}
static PyObject *User_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("User new\n");
    User *self;
    self = PyObject_NEW(User, type);
    //self = (User *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(user_rec_t));
    return (PyObject *)self;
}
static int User_init(User *self, PyObject *args, PyObject *kwds)
{
    //LOG("User init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    self->priv->data.type=' ';

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.pwd);

    do_text_set_empty(self->priv->data.userini);

    do_text_set_empty(self->priv->data.groups);

    do_user_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *User_clone(User *self)
{
    User *result;
    result = (User*)PyObject_CallFunction((PyObject*)getUserType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *User_record_type(User* self, void *unused)
{
    PyObject *result;

    char buf[2];
    buf[0] = self->priv->data.type;
    buf[1] = '\0';
    char *res=do_text(self->alias->alias, buf);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *User_set_record_type(User* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.type");
        return NULL;
    }
    if ( value ) {
        char buf[1];
        do_text_set(self->alias->alias, buf, value);
        self->priv->data.type=buf[0];
    }
    //char buf[2];
    //buf[0] = self->priv->data.type;
    //buf[1] = '\0';
    //char *res=do_text(self->alias->alias, buf);
    //result = MyString_FromString(res);
    //do_free(res);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *User_name(User* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *User_set_name(User* self, PyObject *args, PyObject *kwds)
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

static PyObject *User_password(User* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.pwd);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *User_set_password(User* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.pwd");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.pwd, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.pwd);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *User_supervisore(User* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.supervisore);

    return result;
}

static PyObject *User_set_supervisore(User* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.supervisore");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.supervisore = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.supervisore = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.supervisore");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.supervisore);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *User_user_profile(User* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.userini);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *User_set_user_profile(User* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.userini");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.userini, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.userini);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *User_groups(User* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.groups);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *User_set_groups(User* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.groups");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.groups, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.groups);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *User_set_param(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_user_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_user_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_user_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *User_set_params(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_user_params_clear(self->alias->alias,self->priv);
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
                 do_user_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_user_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_user_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *User_get_param(User* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_user_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *User_params_clear(User* self)
{
    do_user_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *User_get_params(User* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *User_gt(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_last(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_next(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_le(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_lt(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_equal(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_ge(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_prev(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_first(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getUserKey0Type() )
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_iter_gt(User* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getUserKey0Type() ) {
        key_cmp = (user_key0_t*)do_malloc(sizeof(user_key0_t));
        memcpy(key_cmp, ((UserKey0*)key)->priv, sizeof(user_key0_t));
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getUserKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->type, 
                    ((UserKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->name, 
                    ((UserKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = User_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getUserKey0Type() ) {
            status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *User_iter_last(User* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getUserKey0Type() ) {
        key_cmp = (user_key0_t*)do_malloc(sizeof(user_key0_t));
        memcpy(key_cmp, ((UserKey0*)key)->priv, sizeof(user_key0_t));
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getUserKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->type, 
                    ((UserKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->name, 
                    ((UserKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = User_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getUserKey0Type() ) {
            status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *User_iter_le(User* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getUserKey0Type() ) {
        key_cmp = (user_key0_t*)do_malloc(sizeof(user_key0_t));
        memcpy(key_cmp, ((UserKey0*)key)->priv, sizeof(user_key0_t));
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getUserKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->type, 
                    ((UserKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->name, 
                    ((UserKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = User_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getUserKey0Type() ) {
            status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *User_iter_ge(User* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getUserKey0Type() ) {
        key_cmp = (user_key0_t*)do_malloc(sizeof(user_key0_t));
        memcpy(key_cmp, ((UserKey0*)key)->priv, sizeof(user_key0_t));
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getUserKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->type, 
                    ((UserKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->name, 
                    ((UserKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = User_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getUserKey0Type() ) {
            status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *User_iter_equal(User* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getUserKey0Type() ) {
        key_cmp = (user_key0_t*)do_malloc(sizeof(user_key0_t));
        memcpy(key_cmp, ((UserKey0*)key)->priv, sizeof(user_key0_t));
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getUserKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->type, 
                    ((UserKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->name, 
                    ((UserKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = User_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getUserKey0Type() ) {
            status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *User_iter_lt(User* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getUserKey0Type() ) {
        key_cmp = (user_key0_t*)do_malloc(sizeof(user_key0_t));
        memcpy(key_cmp, ((UserKey0*)key)->priv, sizeof(user_key0_t));
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getUserKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->type, 
                    ((UserKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->name, 
                    ((UserKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = User_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getUserKey0Type() ) {
            status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *User_iter_first(User* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getUserKey0Type() ) {
        key_cmp = (user_key0_t*)do_malloc(sizeof(user_key0_t));
        memcpy(key_cmp, ((UserKey0*)key)->priv, sizeof(user_key0_t));
        status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getUserKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->type, 
                    ((UserKey0*)key)->priv->type))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((user_key0_t*)key_cmp)->name, 
                    ((UserKey0*)key)->priv->name))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = User_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getUserKey0Type() ) {
            status = do_user_get0(self->alias->alias, self->priv, ((UserKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *User_insert(User* self)
{
    int status;
    status = do_user_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_update(User* self)
{
    int status;
    status = do_user_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_delete(User* self)
{
    int status;
    status = do_user_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *User_set_alias(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((User*)self)->alias);
    Py_INCREF(obj);
    ((User*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *User_set(User* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getUserType() ) {
        memcpy(self->priv, ((User*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getUserKey0Type() ) {

        do_cpy(self->priv->data.type, ((UserKey0*)obj)->priv->type);

        do_cpy(self->priv->data.name, ((UserKey0*)obj)->priv->name);

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
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("password", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_password", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("supervisore", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_supervisore", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("user_profile", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_user_profile", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("groups", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_groups", "O", value) )
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
                do_log(LOG_ERR, "Class \"User\"does not contain \"%s\"", field_name);
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

static PyObject *User_fields(User* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class User");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  User_record_type(self,NULL);
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

    value =  User_name(self,NULL);
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

    value =  User_password(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("password", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  User_supervisore(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("supervisore", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  User_user_profile(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("user_profile", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  User_groups(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("groups", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = User_get_params(self, NULL);
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

static PyGetSetDef User_getset[] = {

    {"record_type",(getter)User_record_type},

    {"name",(getter)User_name},

    {"password",(getter)User_password},

    {"supervisore",(getter)User_supervisore},

    {"user_profile",(getter)User_user_profile},

    {"groups",(getter)User_groups},

    {"params",(getter)User_get_params},

    {NULL}
};

static PyMethodDef User_methods[] = {
    {"set", (PyCFunction)User_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)User_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)User_clone, METH_NOARGS, "clone"},

    {"set_record_type", (PyCFunction)User_set_record_type, METH_VARARGS|METH_KEYWORDS, "User_set_record_type"},

    {"set_name", (PyCFunction)User_set_name, METH_VARARGS|METH_KEYWORDS, "User_set_name"},

    {"set_password", (PyCFunction)User_set_password, METH_VARARGS|METH_KEYWORDS, "User_set_password"},

    {"set_supervisore", (PyCFunction)User_set_supervisore, METH_VARARGS|METH_KEYWORDS, "User_set_supervisore"},

    {"set_user_profile", (PyCFunction)User_set_user_profile, METH_VARARGS|METH_KEYWORDS, "User_set_user_profile"},

    {"set_groups", (PyCFunction)User_set_groups, METH_VARARGS|METH_KEYWORDS, "User_set_groups"},

    {"set_param", (PyCFunction)User_set_param, METH_VARARGS|METH_KEYWORDS, "do_User_param_set"},
    {"param", (PyCFunction)User_get_param, METH_VARARGS|METH_KEYWORDS, "do_User_param"},
    {"clear_params", (PyCFunction)User_params_clear, METH_NOARGS, "do_User_param_clear"},
    {"set_params", (PyCFunction)User_set_params, METH_VARARGS|METH_KEYWORDS, "do_User_set_params"},

    {"get_gt", (PyCFunction)User_gt, METH_VARARGS|METH_KEYWORDS, "User_gt"},

    {"get_last", (PyCFunction)User_last, METH_VARARGS|METH_KEYWORDS, "User_last"},

    {"get_next", (PyCFunction)User_next, METH_VARARGS|METH_KEYWORDS, "User_next"},

    {"get_le", (PyCFunction)User_le, METH_VARARGS|METH_KEYWORDS, "User_le"},

    {"get_lt", (PyCFunction)User_lt, METH_VARARGS|METH_KEYWORDS, "User_lt"},

    {"get_equal", (PyCFunction)User_equal, METH_VARARGS|METH_KEYWORDS, "User_equal"},

    {"get_ge", (PyCFunction)User_ge, METH_VARARGS|METH_KEYWORDS, "User_ge"},

    {"get_prev", (PyCFunction)User_prev, METH_VARARGS|METH_KEYWORDS, "User_prev"},

    {"get_first", (PyCFunction)User_first, METH_VARARGS|METH_KEYWORDS, "User_first"},

    {"gets_gt", (PyCFunction)User_iter_gt, METH_VARARGS|METH_KEYWORDS, "User_iter_gt"},

    {"gets_last", (PyCFunction)User_iter_last, METH_VARARGS|METH_KEYWORDS, "User_iter_last"},

    {"gets_le", (PyCFunction)User_iter_le, METH_VARARGS|METH_KEYWORDS, "User_iter_le"},

    {"gets_ge", (PyCFunction)User_iter_ge, METH_VARARGS|METH_KEYWORDS, "User_iter_ge"},

    {"gets_equal", (PyCFunction)User_iter_equal, METH_VARARGS|METH_KEYWORDS, "User_iter_equal"},

    {"gets_lt", (PyCFunction)User_iter_lt, METH_VARARGS|METH_KEYWORDS, "User_iter_lt"},

    {"gets_first", (PyCFunction)User_iter_first, METH_VARARGS|METH_KEYWORDS, "User_iter_first"},

    {"insert", (PyCFunction)User_insert, METH_VARARGS|METH_KEYWORDS, "User_insert"},

    {"update", (PyCFunction)User_update, METH_VARARGS|METH_KEYWORDS, "User_update"},

    {"delete", (PyCFunction)User_delete, METH_VARARGS|METH_KEYWORDS, "User_delete"},

    {NULL}
};

static PyTypeObject UserType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.User",             /*tp_name*/
    sizeof(User),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)User_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)User_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "User objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    User_methods,             /* tp_methods */
    0,
    User_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)User_init,      /* tp_init */
    0,                         /* tp_alloc */
    User_new,                 /* tp_new */
};
PyTypeObject *getUserType()
{
    return &UserType_;
}

static void UserKey0_dealloc(PyObject* self)
{
    do_free(((UserKey0*)self)->priv);
    Py_DECREF(((UserKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("UserKey0 free\n");
}
static PyObject *UserKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("UserKey0 new\n");
    UserKey0 *self;
    self = PyObject_NEW(UserKey0, type);
    //self = (UserKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(user_key0_t));
    return (PyObject *)self;
}
static int UserKey0_init(UserKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("UserKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->type=' ';

    do_text_set_empty(self->priv->name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *UserKey0_clone(UserKey0 *self)
{
    UserKey0 *result;
    result = (UserKey0*)PyObject_CallFunction((PyObject*)getUserKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *UserKey0_record_type(UserKey0* self, void *unused)
{
    PyObject *result;

    char buf[2];
    buf[0] = self->priv->type;
    buf[1] = '\0';
    char *res=do_text(self->alias->alias, buf);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *UserKey0_set_record_type(UserKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type");
        return NULL;
    }
    if ( value ) {
        char buf[1];
        do_text_set(self->alias->alias, buf, value);
        self->priv->type=buf[0];
    }
    //char buf[2];
    //buf[0] = self->priv->type;
    //buf[1] = '\0';
    //char *res=do_text(self->alias->alias, buf);
    //result = MyString_FromString(res);
    //do_free(res);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *UserKey0_name(UserKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *UserKey0_set_name(UserKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *UserKey0_gt(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_last(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_next(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_le(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_lt(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_equal(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_ge(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_prev(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_first(UserKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_user_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *UserKey0_iter_gt(UserKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    user_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_user_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = UserKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_user_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *UserKey0_iter_last(UserKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    user_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_user_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = UserKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_user_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *UserKey0_iter_le(UserKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    user_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_user_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = UserKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_user_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *UserKey0_iter_ge(UserKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    user_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_user_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = UserKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_user_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *UserKey0_iter_equal(UserKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    user_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_user_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = UserKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_user_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *UserKey0_iter_lt(UserKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    user_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_user_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = UserKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_user_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *UserKey0_iter_first(UserKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    user_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_user_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.type, 
                 self->priv->type))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = UserKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_user_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *UserKey0_cmp(UserKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getUserKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->type, ((UserKey0*)obj)->priv->type);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->name, ((UserKey0*)obj)->priv->name);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *UserKey0_set_alias(UserKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((UserKey0*)self)->alias);
    Py_INCREF(obj);
    ((UserKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *UserKey0_set(UserKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getUserKey0Type() ) {
        memcpy(self->priv, ((UserKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getUserType() ) {

        do_cpy(self->priv->type, ((User*)obj)->priv->data.type);

        do_cpy(self->priv->name, ((User*)obj)->priv->data.name);

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
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"UserKey0\"does not contain \"%s\"", field_name);
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

static PyObject *UserKey0_fields(UserKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class UserKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  UserKey0_record_type(self,NULL);
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

    value =  UserKey0_name(self,NULL);
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

static PyGetSetDef UserKey0_getset[] = {

    {"record_type",(getter)UserKey0_record_type},

    {"name",(getter)UserKey0_name},

    {NULL}
};

static PyMethodDef UserKey0_methods[] = {
    {"set", (PyCFunction)UserKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)UserKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)UserKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)UserKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_record_type", (PyCFunction)UserKey0_set_record_type, METH_VARARGS|METH_KEYWORDS, "UserKey0_set_record_type"},

    {"set_name", (PyCFunction)UserKey0_set_name, METH_VARARGS|METH_KEYWORDS, "UserKey0_set_name"},

    {"get_gt", (PyCFunction)UserKey0_gt, METH_VARARGS|METH_KEYWORDS, "UserKey0_gt"},

    {"get_last", (PyCFunction)UserKey0_last, METH_VARARGS|METH_KEYWORDS, "UserKey0_last"},

    {"get_next", (PyCFunction)UserKey0_next, METH_VARARGS|METH_KEYWORDS, "UserKey0_next"},

    {"get_le", (PyCFunction)UserKey0_le, METH_VARARGS|METH_KEYWORDS, "UserKey0_le"},

    {"get_lt", (PyCFunction)UserKey0_lt, METH_VARARGS|METH_KEYWORDS, "UserKey0_lt"},

    {"get_equal", (PyCFunction)UserKey0_equal, METH_VARARGS|METH_KEYWORDS, "UserKey0_equal"},

    {"get_ge", (PyCFunction)UserKey0_ge, METH_VARARGS|METH_KEYWORDS, "UserKey0_ge"},

    {"get_prev", (PyCFunction)UserKey0_prev, METH_VARARGS|METH_KEYWORDS, "UserKey0_prev"},

    {"get_first", (PyCFunction)UserKey0_first, METH_VARARGS|METH_KEYWORDS, "UserKey0_first"},

    {"gets_gt", (PyCFunction)UserKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "UserKey0_iter_gt"},

    {"gets_last", (PyCFunction)UserKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "UserKey0_iter_last"},

    {"gets_le", (PyCFunction)UserKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "UserKey0_iter_le"},

    {"gets_ge", (PyCFunction)UserKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "UserKey0_iter_ge"},

    {"gets_equal", (PyCFunction)UserKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "UserKey0_iter_equal"},

    {"gets_lt", (PyCFunction)UserKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "UserKey0_iter_lt"},

    {"gets_first", (PyCFunction)UserKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "UserKey0_iter_first"},

    {NULL}
};

static PyTypeObject UserKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.UserKey0",             /*tp_name*/
    sizeof(UserKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)UserKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)UserKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "UserKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    UserKey0_methods,             /* tp_methods */
    0,
    UserKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)UserKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    UserKey0_new,                 /* tp_new */
};
PyTypeObject *getUserKey0Type()
{
    return &UserKey0Type_;
}
