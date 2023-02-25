
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "protocol.h"

static void Protocol_dealloc(PyObject* self)
{
    do_free(((Protocol*)self)->priv);
    Py_DECREF(((Protocol*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Protocol free\n");
}
static PyObject *Protocol_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Protocol new\n");
    Protocol *self;
    self = PyObject_NEW(Protocol, type);
    //self = (Protocol *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(protocol_rec_t));
    return (PyObject *)self;
}
static int Protocol_init(Protocol *self, PyObject *args, PyObject *kwds)
{
    //LOG("Protocol init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.objectName);

    do_text_set_empty(self->priv->data.userName);

    do_text_set_empty(self->priv->data.action);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Protocol_clone(Protocol *self)
{
    Protocol *result;
    result = (Protocol*)PyObject_CallFunction((PyObject*)getProtocolType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Protocol_object_name(Protocol* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.objectName);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Protocol_set_object_name(Protocol* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.objectName");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.objectName, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.objectName);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Protocol_user_name(Protocol* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.userName);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Protocol_set_user_name(Protocol* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.userName");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.userName, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.userName);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Protocol_date(Protocol* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Protocol_date_raw(Protocol* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *Protocol_set_date(Protocol* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->data.date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Protocol_time(Protocol* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Protocol_time_raw(Protocol* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.time);

    return result;
}

static PyObject *Protocol_set_time(Protocol* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.time = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             self->priv->data.time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Protocol_action(Protocol* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.action);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Protocol_set_action(Protocol* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.action");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.action, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.action);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Protocol_equal(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_next(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_prev(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_gt(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_ge(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_lt(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_le(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_first(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_last(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProtocolKey0Type() )
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() )
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() )
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_iter_equal(Protocol* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProtocolKey0Type() ) {
        key_cmp = (protocol_key0_t*)do_malloc(sizeof(protocol_key0_t));
        memcpy(key_cmp, ((ProtocolKey0*)key)->priv, sizeof(protocol_key0_t));
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() ) {
        key_cmp = (protocol_key1_t*)do_malloc(sizeof(protocol_key1_t));
        memcpy(key_cmp, ((ProtocolKey1*)key)->priv, sizeof(protocol_key1_t));
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() ) {
        key_cmp = (protocol_key2_t*)do_malloc(sizeof(protocol_key2_t));
        memcpy(key_cmp, ((ProtocolKey2*)key)->priv, sizeof(protocol_key2_t));
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->objectName, 
                    ((ProtocolKey0*)key)->priv->objectName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->date, 
                    ((ProtocolKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->time, 
                    ((ProtocolKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->date, 
                    ((ProtocolKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->time, 
                    ((ProtocolKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->userName, 
                    ((ProtocolKey2*)key)->priv->userName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->date, 
                    ((ProtocolKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->time, 
                    ((ProtocolKey2*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Protocol_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
            status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
            status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
            status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *Protocol_iter_gt(Protocol* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProtocolKey0Type() ) {
        key_cmp = (protocol_key0_t*)do_malloc(sizeof(protocol_key0_t));
        memcpy(key_cmp, ((ProtocolKey0*)key)->priv, sizeof(protocol_key0_t));
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() ) {
        key_cmp = (protocol_key1_t*)do_malloc(sizeof(protocol_key1_t));
        memcpy(key_cmp, ((ProtocolKey1*)key)->priv, sizeof(protocol_key1_t));
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() ) {
        key_cmp = (protocol_key2_t*)do_malloc(sizeof(protocol_key2_t));
        memcpy(key_cmp, ((ProtocolKey2*)key)->priv, sizeof(protocol_key2_t));
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->objectName, 
                    ((ProtocolKey0*)key)->priv->objectName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->date, 
                    ((ProtocolKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->time, 
                    ((ProtocolKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->date, 
                    ((ProtocolKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->time, 
                    ((ProtocolKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->userName, 
                    ((ProtocolKey2*)key)->priv->userName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->date, 
                    ((ProtocolKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->time, 
                    ((ProtocolKey2*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Protocol_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
            status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
            status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
            status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *Protocol_iter_ge(Protocol* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProtocolKey0Type() ) {
        key_cmp = (protocol_key0_t*)do_malloc(sizeof(protocol_key0_t));
        memcpy(key_cmp, ((ProtocolKey0*)key)->priv, sizeof(protocol_key0_t));
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() ) {
        key_cmp = (protocol_key1_t*)do_malloc(sizeof(protocol_key1_t));
        memcpy(key_cmp, ((ProtocolKey1*)key)->priv, sizeof(protocol_key1_t));
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() ) {
        key_cmp = (protocol_key2_t*)do_malloc(sizeof(protocol_key2_t));
        memcpy(key_cmp, ((ProtocolKey2*)key)->priv, sizeof(protocol_key2_t));
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->objectName, 
                    ((ProtocolKey0*)key)->priv->objectName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->date, 
                    ((ProtocolKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->time, 
                    ((ProtocolKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->date, 
                    ((ProtocolKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->time, 
                    ((ProtocolKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->userName, 
                    ((ProtocolKey2*)key)->priv->userName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->date, 
                    ((ProtocolKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->time, 
                    ((ProtocolKey2*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Protocol_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
            status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
            status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
            status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *Protocol_iter_lt(Protocol* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProtocolKey0Type() ) {
        key_cmp = (protocol_key0_t*)do_malloc(sizeof(protocol_key0_t));
        memcpy(key_cmp, ((ProtocolKey0*)key)->priv, sizeof(protocol_key0_t));
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() ) {
        key_cmp = (protocol_key1_t*)do_malloc(sizeof(protocol_key1_t));
        memcpy(key_cmp, ((ProtocolKey1*)key)->priv, sizeof(protocol_key1_t));
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() ) {
        key_cmp = (protocol_key2_t*)do_malloc(sizeof(protocol_key2_t));
        memcpy(key_cmp, ((ProtocolKey2*)key)->priv, sizeof(protocol_key2_t));
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->objectName, 
                    ((ProtocolKey0*)key)->priv->objectName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->date, 
                    ((ProtocolKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->time, 
                    ((ProtocolKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->date, 
                    ((ProtocolKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->time, 
                    ((ProtocolKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->userName, 
                    ((ProtocolKey2*)key)->priv->userName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->date, 
                    ((ProtocolKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->time, 
                    ((ProtocolKey2*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Protocol_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
            status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
            status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
            status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Protocol_iter_le(Protocol* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProtocolKey0Type() ) {
        key_cmp = (protocol_key0_t*)do_malloc(sizeof(protocol_key0_t));
        memcpy(key_cmp, ((ProtocolKey0*)key)->priv, sizeof(protocol_key0_t));
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() ) {
        key_cmp = (protocol_key1_t*)do_malloc(sizeof(protocol_key1_t));
        memcpy(key_cmp, ((ProtocolKey1*)key)->priv, sizeof(protocol_key1_t));
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() ) {
        key_cmp = (protocol_key2_t*)do_malloc(sizeof(protocol_key2_t));
        memcpy(key_cmp, ((ProtocolKey2*)key)->priv, sizeof(protocol_key2_t));
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->objectName, 
                    ((ProtocolKey0*)key)->priv->objectName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->date, 
                    ((ProtocolKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->time, 
                    ((ProtocolKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->date, 
                    ((ProtocolKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->time, 
                    ((ProtocolKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->userName, 
                    ((ProtocolKey2*)key)->priv->userName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->date, 
                    ((ProtocolKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->time, 
                    ((ProtocolKey2*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Protocol_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
            status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
            status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
            status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Protocol_iter_first(Protocol* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProtocolKey0Type() ) {
        key_cmp = (protocol_key0_t*)do_malloc(sizeof(protocol_key0_t));
        memcpy(key_cmp, ((ProtocolKey0*)key)->priv, sizeof(protocol_key0_t));
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() ) {
        key_cmp = (protocol_key1_t*)do_malloc(sizeof(protocol_key1_t));
        memcpy(key_cmp, ((ProtocolKey1*)key)->priv, sizeof(protocol_key1_t));
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() ) {
        key_cmp = (protocol_key2_t*)do_malloc(sizeof(protocol_key2_t));
        memcpy(key_cmp, ((ProtocolKey2*)key)->priv, sizeof(protocol_key2_t));
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->objectName, 
                    ((ProtocolKey0*)key)->priv->objectName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->date, 
                    ((ProtocolKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->time, 
                    ((ProtocolKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->date, 
                    ((ProtocolKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->time, 
                    ((ProtocolKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->userName, 
                    ((ProtocolKey2*)key)->priv->userName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->date, 
                    ((ProtocolKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->time, 
                    ((ProtocolKey2*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Protocol_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
            status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
            status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
            status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_NEXT);
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

static PyObject *Protocol_iter_last(Protocol* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProtocolKey0Type() ) {
        key_cmp = (protocol_key0_t*)do_malloc(sizeof(protocol_key0_t));
        memcpy(key_cmp, ((ProtocolKey0*)key)->priv, sizeof(protocol_key0_t));
        status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey1Type() ) {
        key_cmp = (protocol_key1_t*)do_malloc(sizeof(protocol_key1_t));
        memcpy(key_cmp, ((ProtocolKey1*)key)->priv, sizeof(protocol_key1_t));
        status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProtocolKey2Type() ) {
        key_cmp = (protocol_key2_t*)do_malloc(sizeof(protocol_key2_t));
        memcpy(key_cmp, ((ProtocolKey2*)key)->priv, sizeof(protocol_key2_t));
        status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->objectName, 
                    ((ProtocolKey0*)key)->priv->objectName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->date, 
                    ((ProtocolKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key0_t*)key_cmp)->time, 
                    ((ProtocolKey0*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->date, 
                    ((ProtocolKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key1_t*)key_cmp)->time, 
                    ((ProtocolKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->userName, 
                    ((ProtocolKey2*)key)->priv->userName))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->date, 
                    ((ProtocolKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((protocol_key2_t*)key_cmp)->time, 
                    ((ProtocolKey2*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Protocol_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProtocolKey0Type() ) {
            status = do_protocol_get0(self->alias->alias, self->priv, ((ProtocolKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey1Type() ) {
            status = do_protocol_get1(self->alias->alias, self->priv, ((ProtocolKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProtocolKey2Type() ) {
            status = do_protocol_get2(self->alias->alias, self->priv, ((ProtocolKey2*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Protocol_insert(Protocol* self)
{
    int status;
    status = do_protocol_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_update(Protocol* self)
{
    int status;
    status = do_protocol_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_delete(Protocol* self)
{
    int status;
    status = do_protocol_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Protocol_set_alias(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Protocol*)self)->alias);
    Py_INCREF(obj);
    ((Protocol*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Protocol_set(Protocol* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProtocolType() ) {
        memcpy(self->priv, ((Protocol*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProtocolKey0Type() ) {

        do_cpy(self->priv->data.objectName, ((ProtocolKey0*)obj)->priv->objectName);

        do_cpy(self->priv->data.date, ((ProtocolKey0*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((ProtocolKey0*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getProtocolKey1Type() ) {

        do_cpy(self->priv->data.date, ((ProtocolKey1*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((ProtocolKey1*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getProtocolKey2Type() ) {

        do_cpy(self->priv->data.userName, ((ProtocolKey2*)obj)->priv->userName);

        do_cpy(self->priv->data.date, ((ProtocolKey2*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((ProtocolKey2*)obj)->priv->time);

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
            if ( !strcmp("object_name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_object_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("user_name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_user_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("action", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_action", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Protocol\"does not contain \"%s\"", field_name);
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

static PyObject *Protocol_fields(Protocol* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Protocol");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Protocol_object_name(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("object_name", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Protocol_user_name(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("user_name", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Protocol_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Protocol_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Protocol_action(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("action", value);
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

static PyGetSetDef Protocol_getset[] = {

    {"object_name",(getter)Protocol_object_name},

    {"user_name",(getter)Protocol_user_name},

    {"date",(getter)Protocol_date},

    {"date_raw",(getter)Protocol_date_raw},

    {"time",(getter)Protocol_time},

    {"time_raw",(getter)Protocol_time_raw},

    {"action",(getter)Protocol_action},

    {NULL}
};

static PyMethodDef Protocol_methods[] = {
    {"set", (PyCFunction)Protocol_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Protocol_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Protocol_clone, METH_NOARGS, "clone"},

    {"set_object_name", (PyCFunction)Protocol_set_object_name, METH_VARARGS|METH_KEYWORDS, "Protocol_set_object_name"},

    {"set_user_name", (PyCFunction)Protocol_set_user_name, METH_VARARGS|METH_KEYWORDS, "Protocol_set_user_name"},

    {"set_date", (PyCFunction)Protocol_set_date, METH_VARARGS|METH_KEYWORDS, "Protocol_set_date"},

    {"set_time", (PyCFunction)Protocol_set_time, METH_VARARGS|METH_KEYWORDS, "Protocol_set_time"},

    {"set_action", (PyCFunction)Protocol_set_action, METH_VARARGS|METH_KEYWORDS, "Protocol_set_action"},

    {"get_equal", (PyCFunction)Protocol_equal, METH_VARARGS|METH_KEYWORDS, "Protocol_equal"},

    {"get_next", (PyCFunction)Protocol_next, METH_VARARGS|METH_KEYWORDS, "Protocol_next"},

    {"get_prev", (PyCFunction)Protocol_prev, METH_VARARGS|METH_KEYWORDS, "Protocol_prev"},

    {"get_gt", (PyCFunction)Protocol_gt, METH_VARARGS|METH_KEYWORDS, "Protocol_gt"},

    {"get_ge", (PyCFunction)Protocol_ge, METH_VARARGS|METH_KEYWORDS, "Protocol_ge"},

    {"get_lt", (PyCFunction)Protocol_lt, METH_VARARGS|METH_KEYWORDS, "Protocol_lt"},

    {"get_le", (PyCFunction)Protocol_le, METH_VARARGS|METH_KEYWORDS, "Protocol_le"},

    {"get_first", (PyCFunction)Protocol_first, METH_VARARGS|METH_KEYWORDS, "Protocol_first"},

    {"get_last", (PyCFunction)Protocol_last, METH_VARARGS|METH_KEYWORDS, "Protocol_last"},

    {"gets_equal", (PyCFunction)Protocol_iter_equal, METH_VARARGS|METH_KEYWORDS, "Protocol_iter_equal"},

    {"gets_gt", (PyCFunction)Protocol_iter_gt, METH_VARARGS|METH_KEYWORDS, "Protocol_iter_gt"},

    {"gets_ge", (PyCFunction)Protocol_iter_ge, METH_VARARGS|METH_KEYWORDS, "Protocol_iter_ge"},

    {"gets_lt", (PyCFunction)Protocol_iter_lt, METH_VARARGS|METH_KEYWORDS, "Protocol_iter_lt"},

    {"gets_le", (PyCFunction)Protocol_iter_le, METH_VARARGS|METH_KEYWORDS, "Protocol_iter_le"},

    {"gets_first", (PyCFunction)Protocol_iter_first, METH_VARARGS|METH_KEYWORDS, "Protocol_iter_first"},

    {"gets_last", (PyCFunction)Protocol_iter_last, METH_VARARGS|METH_KEYWORDS, "Protocol_iter_last"},

    {"insert", (PyCFunction)Protocol_insert, METH_VARARGS|METH_KEYWORDS, "Protocol_insert"},

    {"update", (PyCFunction)Protocol_update, METH_VARARGS|METH_KEYWORDS, "Protocol_update"},

    {"delete", (PyCFunction)Protocol_delete, METH_VARARGS|METH_KEYWORDS, "Protocol_delete"},

    {NULL}
};

static PyTypeObject ProtocolType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Protocol",             /*tp_name*/
    sizeof(Protocol),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Protocol_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Protocol_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Protocol objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Protocol_methods,             /* tp_methods */
    0,
    Protocol_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Protocol_init,      /* tp_init */
    0,                         /* tp_alloc */
    Protocol_new,                 /* tp_new */
};
PyTypeObject *getProtocolType()
{
    return &ProtocolType_;
}

static void ProtocolKey0_dealloc(PyObject* self)
{
    do_free(((ProtocolKey0*)self)->priv);
    Py_DECREF(((ProtocolKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProtocolKey0 free\n");
}
static PyObject *ProtocolKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProtocolKey0 new\n");
    ProtocolKey0 *self;
    self = PyObject_NEW(ProtocolKey0, type);
    //self = (ProtocolKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(protocol_key0_t));
    return (PyObject *)self;
}
static int ProtocolKey0_init(ProtocolKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProtocolKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->objectName);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProtocolKey0_clone(ProtocolKey0 *self)
{
    ProtocolKey0 *result;
    result = (ProtocolKey0*)PyObject_CallFunction((PyObject*)getProtocolKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProtocolKey0_object_name(ProtocolKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->objectName);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProtocolKey0_set_object_name(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set objectName");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->objectName, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->objectName);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProtocolKey0_date(ProtocolKey0* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ProtocolKey0_date_raw(ProtocolKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ProtocolKey0_set_date(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProtocolKey0_time(ProtocolKey0* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *ProtocolKey0_time_raw(ProtocolKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *ProtocolKey0_set_time(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->time = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             self->priv->time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProtocolKey0_equal(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_next(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_prev(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_gt(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_ge(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_lt(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_le(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_first(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_last(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey0_iter_equal(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.objectName, 
                 self->priv->objectName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey0_iter_gt(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.objectName, 
                 self->priv->objectName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey0_iter_ge(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.objectName, 
                 self->priv->objectName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey0_iter_lt(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.objectName, 
                 self->priv->objectName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey0_iter_le(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.objectName, 
                 self->priv->objectName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey0_iter_first(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.objectName, 
                 self->priv->objectName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey0_iter_last(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.objectName, 
                 self->priv->objectName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey0_cmp(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProtocolKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->objectName, ((ProtocolKey0*)obj)->priv->objectName);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((ProtocolKey0*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->time - ((ProtocolKey0*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProtocolKey0_set_alias(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProtocolKey0*)self)->alias);
    Py_INCREF(obj);
    ((ProtocolKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProtocolKey0_set(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProtocolKey0Type() ) {
        memcpy(self->priv, ((ProtocolKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProtocolType() ) {

        do_cpy(self->priv->objectName, ((Protocol*)obj)->priv->data.objectName);

        do_cpy(self->priv->date, ((Protocol*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Protocol*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_object_name", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
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
            if ( !strcmp("object_name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_object_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProtocolKey0\"does not contain \"%s\"", field_name);
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

static PyObject *ProtocolKey0_fields(ProtocolKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProtocolKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProtocolKey0_object_name(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("object_name", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProtocolKey0_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProtocolKey0_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time", value);
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

static PyGetSetDef ProtocolKey0_getset[] = {

    {"object_name",(getter)ProtocolKey0_object_name},

    {"date",(getter)ProtocolKey0_date},

    {"date_raw",(getter)ProtocolKey0_date_raw},

    {"time",(getter)ProtocolKey0_time},

    {"time_raw",(getter)ProtocolKey0_time_raw},

    {NULL}
};

static PyMethodDef ProtocolKey0_methods[] = {
    {"set", (PyCFunction)ProtocolKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProtocolKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProtocolKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProtocolKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_object_name", (PyCFunction)ProtocolKey0_set_object_name, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_set_object_name"},

    {"set_date", (PyCFunction)ProtocolKey0_set_date, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_set_date"},

    {"set_time", (PyCFunction)ProtocolKey0_set_time, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_set_time"},

    {"get_equal", (PyCFunction)ProtocolKey0_equal, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_equal"},

    {"get_next", (PyCFunction)ProtocolKey0_next, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_next"},

    {"get_prev", (PyCFunction)ProtocolKey0_prev, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_prev"},

    {"get_gt", (PyCFunction)ProtocolKey0_gt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_gt"},

    {"get_ge", (PyCFunction)ProtocolKey0_ge, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_ge"},

    {"get_lt", (PyCFunction)ProtocolKey0_lt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_lt"},

    {"get_le", (PyCFunction)ProtocolKey0_le, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_le"},

    {"get_first", (PyCFunction)ProtocolKey0_first, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_first"},

    {"get_last", (PyCFunction)ProtocolKey0_last, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_last"},

    {"gets_equal", (PyCFunction)ProtocolKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_iter_equal"},

    {"gets_gt", (PyCFunction)ProtocolKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_iter_gt"},

    {"gets_ge", (PyCFunction)ProtocolKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_iter_ge"},

    {"gets_lt", (PyCFunction)ProtocolKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_iter_lt"},

    {"gets_le", (PyCFunction)ProtocolKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_iter_le"},

    {"gets_first", (PyCFunction)ProtocolKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_iter_first"},

    {"gets_last", (PyCFunction)ProtocolKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "ProtocolKey0_iter_last"},

    {NULL}
};

static PyTypeObject ProtocolKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProtocolKey0",             /*tp_name*/
    sizeof(ProtocolKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProtocolKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProtocolKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProtocolKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProtocolKey0_methods,             /* tp_methods */
    0,
    ProtocolKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProtocolKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProtocolKey0_new,                 /* tp_new */
};
PyTypeObject *getProtocolKey0Type()
{
    return &ProtocolKey0Type_;
}

static void ProtocolKey1_dealloc(PyObject* self)
{
    do_free(((ProtocolKey1*)self)->priv);
    Py_DECREF(((ProtocolKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProtocolKey1 free\n");
}
static PyObject *ProtocolKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProtocolKey1 new\n");
    ProtocolKey1 *self;
    self = PyObject_NEW(ProtocolKey1, type);
    //self = (ProtocolKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(protocol_key1_t));
    return (PyObject *)self;
}
static int ProtocolKey1_init(ProtocolKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProtocolKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProtocolKey1_clone(ProtocolKey1 *self)
{
    ProtocolKey1 *result;
    result = (ProtocolKey1*)PyObject_CallFunction((PyObject*)getProtocolKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProtocolKey1_date(ProtocolKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ProtocolKey1_date_raw(ProtocolKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ProtocolKey1_set_date(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProtocolKey1_time(ProtocolKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *ProtocolKey1_time_raw(ProtocolKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *ProtocolKey1_set_time(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->time = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             self->priv->time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProtocolKey1_equal(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_next(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_prev(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_gt(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_ge(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_lt(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_le(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_first(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_last(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey1_iter_equal(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey1_iter_gt(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey1_iter_ge(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey1_iter_lt(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey1_iter_le(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey1_iter_first(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey1_iter_last(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey1_cmp(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProtocolKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->date - ((ProtocolKey1*)obj)->priv->date;
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->time - ((ProtocolKey1*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProtocolKey1_set_alias(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProtocolKey1*)self)->alias);
    Py_INCREF(obj);
    ((ProtocolKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProtocolKey1_set(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProtocolKey1Type() ) {
        memcpy(self->priv, ((ProtocolKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProtocolType() ) {

        do_cpy(self->priv->date, ((Protocol*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Protocol*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
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
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProtocolKey1\"does not contain \"%s\"", field_name);
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

static PyObject *ProtocolKey1_fields(ProtocolKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProtocolKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProtocolKey1_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProtocolKey1_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time", value);
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

static PyGetSetDef ProtocolKey1_getset[] = {

    {"date",(getter)ProtocolKey1_date},

    {"date_raw",(getter)ProtocolKey1_date_raw},

    {"time",(getter)ProtocolKey1_time},

    {"time_raw",(getter)ProtocolKey1_time_raw},

    {NULL}
};

static PyMethodDef ProtocolKey1_methods[] = {
    {"set", (PyCFunction)ProtocolKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProtocolKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProtocolKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProtocolKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_date", (PyCFunction)ProtocolKey1_set_date, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_set_date"},

    {"set_time", (PyCFunction)ProtocolKey1_set_time, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_set_time"},

    {"get_equal", (PyCFunction)ProtocolKey1_equal, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_equal"},

    {"get_next", (PyCFunction)ProtocolKey1_next, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_next"},

    {"get_prev", (PyCFunction)ProtocolKey1_prev, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_prev"},

    {"get_gt", (PyCFunction)ProtocolKey1_gt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_gt"},

    {"get_ge", (PyCFunction)ProtocolKey1_ge, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_ge"},

    {"get_lt", (PyCFunction)ProtocolKey1_lt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_lt"},

    {"get_le", (PyCFunction)ProtocolKey1_le, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_le"},

    {"get_first", (PyCFunction)ProtocolKey1_first, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_first"},

    {"get_last", (PyCFunction)ProtocolKey1_last, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_last"},

    {"gets_equal", (PyCFunction)ProtocolKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_iter_equal"},

    {"gets_gt", (PyCFunction)ProtocolKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_iter_gt"},

    {"gets_ge", (PyCFunction)ProtocolKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_iter_ge"},

    {"gets_lt", (PyCFunction)ProtocolKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_iter_lt"},

    {"gets_le", (PyCFunction)ProtocolKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_iter_le"},

    {"gets_first", (PyCFunction)ProtocolKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_iter_first"},

    {"gets_last", (PyCFunction)ProtocolKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "ProtocolKey1_iter_last"},

    {NULL}
};

static PyTypeObject ProtocolKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProtocolKey1",             /*tp_name*/
    sizeof(ProtocolKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProtocolKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProtocolKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProtocolKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProtocolKey1_methods,             /* tp_methods */
    0,
    ProtocolKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProtocolKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProtocolKey1_new,                 /* tp_new */
};
PyTypeObject *getProtocolKey1Type()
{
    return &ProtocolKey1Type_;
}

static void ProtocolKey2_dealloc(PyObject* self)
{
    do_free(((ProtocolKey2*)self)->priv);
    Py_DECREF(((ProtocolKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProtocolKey2 free\n");
}
static PyObject *ProtocolKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProtocolKey2 new\n");
    ProtocolKey2 *self;
    self = PyObject_NEW(ProtocolKey2, type);
    //self = (ProtocolKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(protocol_key2_t));
    return (PyObject *)self;
}
static int ProtocolKey2_init(ProtocolKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProtocolKey2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->userName);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProtocolKey2_clone(ProtocolKey2 *self)
{
    ProtocolKey2 *result;
    result = (ProtocolKey2*)PyObject_CallFunction((PyObject*)getProtocolKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProtocolKey2_user_name(ProtocolKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->userName);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProtocolKey2_set_user_name(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set userName");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->userName, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->userName);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProtocolKey2_date(ProtocolKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ProtocolKey2_date_raw(ProtocolKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ProtocolKey2_set_date(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProtocolKey2_time(ProtocolKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *ProtocolKey2_time_raw(ProtocolKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *ProtocolKey2_set_time(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->time = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             self->priv->time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProtocolKey2_equal(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_next(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_prev(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_gt(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_ge(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_lt(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_le(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_first(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_last(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProtocolKey2_iter_equal(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.userName, 
                 self->priv->userName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey2_iter_gt(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.userName, 
                 self->priv->userName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey2_iter_ge(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.userName, 
                 self->priv->userName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey2_iter_lt(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.userName, 
                 self->priv->userName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey2_iter_le(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.userName, 
                 self->priv->userName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey2_iter_first(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.userName, 
                 self->priv->userName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey2_iter_last(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    protocol_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.userName, 
                 self->priv->userName))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = ProtocolKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_protocol_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProtocolKey2_cmp(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProtocolKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->userName, ((ProtocolKey2*)obj)->priv->userName);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((ProtocolKey2*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->time - ((ProtocolKey2*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProtocolKey2_set_alias(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProtocolKey2*)self)->alias);
    Py_INCREF(obj);
    ((ProtocolKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProtocolKey2_set(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProtocolKey2Type() ) {
        memcpy(self->priv, ((ProtocolKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProtocolType() ) {

        do_cpy(self->priv->userName, ((Protocol*)obj)->priv->data.userName);

        do_cpy(self->priv->date, ((Protocol*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Protocol*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_user_name", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
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
            if ( !strcmp("user_name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_user_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_time", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProtocolKey2\"does not contain \"%s\"", field_name);
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

static PyObject *ProtocolKey2_fields(ProtocolKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProtocolKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProtocolKey2_user_name(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("user_name", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProtocolKey2_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProtocolKey2_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("time", value);
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

static PyGetSetDef ProtocolKey2_getset[] = {

    {"user_name",(getter)ProtocolKey2_user_name},

    {"date",(getter)ProtocolKey2_date},

    {"date_raw",(getter)ProtocolKey2_date_raw},

    {"time",(getter)ProtocolKey2_time},

    {"time_raw",(getter)ProtocolKey2_time_raw},

    {NULL}
};

static PyMethodDef ProtocolKey2_methods[] = {
    {"set", (PyCFunction)ProtocolKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProtocolKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProtocolKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProtocolKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_user_name", (PyCFunction)ProtocolKey2_set_user_name, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_set_user_name"},

    {"set_date", (PyCFunction)ProtocolKey2_set_date, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_set_date"},

    {"set_time", (PyCFunction)ProtocolKey2_set_time, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_set_time"},

    {"get_equal", (PyCFunction)ProtocolKey2_equal, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_equal"},

    {"get_next", (PyCFunction)ProtocolKey2_next, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_next"},

    {"get_prev", (PyCFunction)ProtocolKey2_prev, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_prev"},

    {"get_gt", (PyCFunction)ProtocolKey2_gt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_gt"},

    {"get_ge", (PyCFunction)ProtocolKey2_ge, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_ge"},

    {"get_lt", (PyCFunction)ProtocolKey2_lt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_lt"},

    {"get_le", (PyCFunction)ProtocolKey2_le, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_le"},

    {"get_first", (PyCFunction)ProtocolKey2_first, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_first"},

    {"get_last", (PyCFunction)ProtocolKey2_last, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_last"},

    {"gets_equal", (PyCFunction)ProtocolKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_iter_equal"},

    {"gets_gt", (PyCFunction)ProtocolKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_iter_gt"},

    {"gets_ge", (PyCFunction)ProtocolKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_iter_ge"},

    {"gets_lt", (PyCFunction)ProtocolKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_iter_lt"},

    {"gets_le", (PyCFunction)ProtocolKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_iter_le"},

    {"gets_first", (PyCFunction)ProtocolKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_iter_first"},

    {"gets_last", (PyCFunction)ProtocolKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "ProtocolKey2_iter_last"},

    {NULL}
};

static PyTypeObject ProtocolKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProtocolKey2",             /*tp_name*/
    sizeof(ProtocolKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProtocolKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProtocolKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProtocolKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProtocolKey2_methods,             /* tp_methods */
    0,
    ProtocolKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProtocolKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProtocolKey2_new,                 /* tp_new */
};
PyTypeObject *getProtocolKey2Type()
{
    return &ProtocolKey2Type_;
}
