
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "partner.h"

static void Partner_dealloc(PyObject* self)
{
    do_free(((Partner*)self)->priv);
    Py_DECREF(((Partner*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Partner free\n");
}
static PyObject *Partner_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Partner new\n");
    Partner *self;
    self = PyObject_NEW(Partner, type);
    //self = (Partner *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_rec_t));
    return (PyObject *)self;
}
static int Partner_init(Partner *self, PyObject *args, PyObject *kwds)
{
    //LOG("Partner init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.g_code);

    do_text_set_empty(self->priv->data.otdel);

    do_text_set_empty(self->priv->data.sklad);

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.bank);

    do_text_set_empty(self->priv->data.fio);

    do_text_set_empty(self->priv->data.Klass);

    do_text_set_empty(self->priv->data.Param);

    do_partner_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Partner_clone(Partner *self)
{
    Partner *result;
    result = (Partner*)PyObject_CallFunction((PyObject*)getPartnerType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Partner_region_code(Partner* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Partner_set_region_code(Partner* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Partner_code(Partner* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.code);

    return result;
}

static PyObject *Partner_set_code(Partner* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Partner_division(Partner* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.otdel);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Partner_set_division(Partner* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.otdel");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.otdel, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.otdel);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Partner_store(Partner* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Partner_set_store(Partner* self, PyObject *args, PyObject *kwds)
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

static PyObject *Partner_name(Partner* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Partner_set_name(Partner* self, PyObject *args, PyObject *kwds)
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

static PyObject *Partner_bank(Partner* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.bank);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Partner_set_bank(Partner* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.bank");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.bank, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.bank);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Partner_f_i_o(Partner* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.fio);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Partner_set_f_i_o(Partner* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.fio");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.fio, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.fio);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Partner_state(Partner* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.state);

    return result;
}

static PyObject *Partner_set_state(Partner* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.state");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.state = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.state = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.state");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.state);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Partner_record_type(Partner* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type);

    return result;
}

static PyObject *Partner_set_record_type(Partner* self, PyObject *args, PyObject *kwds)
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

static PyObject *Partner_class(Partner* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.Klass);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Partner_set_class(Partner* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.Klass");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.Klass, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.Klass);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Partner_parameter(Partner* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.Param);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Partner_set_parameter(Partner* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.Param");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.Param, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.Param);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Partner_set_param(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_partner_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_partner_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_partner_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Partner_set_params(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_partner_params_clear(self->alias->alias,self->priv);
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
                 do_partner_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_partner_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_partner_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Partner_get_param(Partner* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_partner_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Partner_params_clear(Partner* self)
{
    do_partner_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Partner_get_params(Partner* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *Partner_gt(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_last(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_next(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_le(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_lt(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_equal(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_ge(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_prev(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_first(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getPartnerKey0Type() )
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() )
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() )
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() )
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_iter_gt(Partner* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerKey0Type() ) {
        key_cmp = (partner_key0_t*)do_malloc(sizeof(partner_key0_t));
        memcpy(key_cmp, ((PartnerKey0*)key)->priv, sizeof(partner_key0_t));
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() ) {
        key_cmp = (partner_key1_t*)do_malloc(sizeof(partner_key1_t));
        memcpy(key_cmp, ((PartnerKey1*)key)->priv, sizeof(partner_key1_t));
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() ) {
        key_cmp = (partner_key2_t*)do_malloc(sizeof(partner_key2_t));
        memcpy(key_cmp, ((PartnerKey2*)key)->priv, sizeof(partner_key2_t));
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() ) {
        key_cmp = (partner_key3_t*)do_malloc(sizeof(partner_key3_t));
        memcpy(key_cmp, ((PartnerKey3*)key)->priv, sizeof(partner_key3_t));
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->g_code, 
                    ((PartnerKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->code, 
                    ((PartnerKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->g_code, 
                    ((PartnerKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->name, 
                    ((PartnerKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key2_t*)key_cmp)->name, 
                    ((PartnerKey2*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key3_t*)key_cmp)->code, 
                    ((PartnerKey3*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Partner_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
            status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
            status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
            status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
            status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Partner_iter_last(Partner* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerKey0Type() ) {
        key_cmp = (partner_key0_t*)do_malloc(sizeof(partner_key0_t));
        memcpy(key_cmp, ((PartnerKey0*)key)->priv, sizeof(partner_key0_t));
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() ) {
        key_cmp = (partner_key1_t*)do_malloc(sizeof(partner_key1_t));
        memcpy(key_cmp, ((PartnerKey1*)key)->priv, sizeof(partner_key1_t));
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() ) {
        key_cmp = (partner_key2_t*)do_malloc(sizeof(partner_key2_t));
        memcpy(key_cmp, ((PartnerKey2*)key)->priv, sizeof(partner_key2_t));
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() ) {
        key_cmp = (partner_key3_t*)do_malloc(sizeof(partner_key3_t));
        memcpy(key_cmp, ((PartnerKey3*)key)->priv, sizeof(partner_key3_t));
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->g_code, 
                    ((PartnerKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->code, 
                    ((PartnerKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->g_code, 
                    ((PartnerKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->name, 
                    ((PartnerKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key2_t*)key_cmp)->name, 
                    ((PartnerKey2*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key3_t*)key_cmp)->code, 
                    ((PartnerKey3*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Partner_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
            status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
            status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
            status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
            status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Partner_iter_le(Partner* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerKey0Type() ) {
        key_cmp = (partner_key0_t*)do_malloc(sizeof(partner_key0_t));
        memcpy(key_cmp, ((PartnerKey0*)key)->priv, sizeof(partner_key0_t));
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() ) {
        key_cmp = (partner_key1_t*)do_malloc(sizeof(partner_key1_t));
        memcpy(key_cmp, ((PartnerKey1*)key)->priv, sizeof(partner_key1_t));
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() ) {
        key_cmp = (partner_key2_t*)do_malloc(sizeof(partner_key2_t));
        memcpy(key_cmp, ((PartnerKey2*)key)->priv, sizeof(partner_key2_t));
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() ) {
        key_cmp = (partner_key3_t*)do_malloc(sizeof(partner_key3_t));
        memcpy(key_cmp, ((PartnerKey3*)key)->priv, sizeof(partner_key3_t));
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->g_code, 
                    ((PartnerKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->code, 
                    ((PartnerKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->g_code, 
                    ((PartnerKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->name, 
                    ((PartnerKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key2_t*)key_cmp)->name, 
                    ((PartnerKey2*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key3_t*)key_cmp)->code, 
                    ((PartnerKey3*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Partner_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
            status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
            status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
            status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
            status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Partner_iter_ge(Partner* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerKey0Type() ) {
        key_cmp = (partner_key0_t*)do_malloc(sizeof(partner_key0_t));
        memcpy(key_cmp, ((PartnerKey0*)key)->priv, sizeof(partner_key0_t));
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() ) {
        key_cmp = (partner_key1_t*)do_malloc(sizeof(partner_key1_t));
        memcpy(key_cmp, ((PartnerKey1*)key)->priv, sizeof(partner_key1_t));
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() ) {
        key_cmp = (partner_key2_t*)do_malloc(sizeof(partner_key2_t));
        memcpy(key_cmp, ((PartnerKey2*)key)->priv, sizeof(partner_key2_t));
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() ) {
        key_cmp = (partner_key3_t*)do_malloc(sizeof(partner_key3_t));
        memcpy(key_cmp, ((PartnerKey3*)key)->priv, sizeof(partner_key3_t));
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->g_code, 
                    ((PartnerKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->code, 
                    ((PartnerKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->g_code, 
                    ((PartnerKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->name, 
                    ((PartnerKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key2_t*)key_cmp)->name, 
                    ((PartnerKey2*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key3_t*)key_cmp)->code, 
                    ((PartnerKey3*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Partner_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
            status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
            status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
            status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
            status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Partner_iter_equal(Partner* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerKey0Type() ) {
        key_cmp = (partner_key0_t*)do_malloc(sizeof(partner_key0_t));
        memcpy(key_cmp, ((PartnerKey0*)key)->priv, sizeof(partner_key0_t));
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() ) {
        key_cmp = (partner_key1_t*)do_malloc(sizeof(partner_key1_t));
        memcpy(key_cmp, ((PartnerKey1*)key)->priv, sizeof(partner_key1_t));
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() ) {
        key_cmp = (partner_key2_t*)do_malloc(sizeof(partner_key2_t));
        memcpy(key_cmp, ((PartnerKey2*)key)->priv, sizeof(partner_key2_t));
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() ) {
        key_cmp = (partner_key3_t*)do_malloc(sizeof(partner_key3_t));
        memcpy(key_cmp, ((PartnerKey3*)key)->priv, sizeof(partner_key3_t));
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->g_code, 
                    ((PartnerKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->code, 
                    ((PartnerKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->g_code, 
                    ((PartnerKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->name, 
                    ((PartnerKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key2_t*)key_cmp)->name, 
                    ((PartnerKey2*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key3_t*)key_cmp)->code, 
                    ((PartnerKey3*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Partner_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
            status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
            status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
            status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
            status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Partner_iter_lt(Partner* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerKey0Type() ) {
        key_cmp = (partner_key0_t*)do_malloc(sizeof(partner_key0_t));
        memcpy(key_cmp, ((PartnerKey0*)key)->priv, sizeof(partner_key0_t));
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() ) {
        key_cmp = (partner_key1_t*)do_malloc(sizeof(partner_key1_t));
        memcpy(key_cmp, ((PartnerKey1*)key)->priv, sizeof(partner_key1_t));
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() ) {
        key_cmp = (partner_key2_t*)do_malloc(sizeof(partner_key2_t));
        memcpy(key_cmp, ((PartnerKey2*)key)->priv, sizeof(partner_key2_t));
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() ) {
        key_cmp = (partner_key3_t*)do_malloc(sizeof(partner_key3_t));
        memcpy(key_cmp, ((PartnerKey3*)key)->priv, sizeof(partner_key3_t));
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->g_code, 
                    ((PartnerKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->code, 
                    ((PartnerKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->g_code, 
                    ((PartnerKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->name, 
                    ((PartnerKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key2_t*)key_cmp)->name, 
                    ((PartnerKey2*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key3_t*)key_cmp)->code, 
                    ((PartnerKey3*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Partner_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
            status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
            status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
            status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
            status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Partner_iter_first(Partner* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getPartnerKey0Type() ) {
        key_cmp = (partner_key0_t*)do_malloc(sizeof(partner_key0_t));
        memcpy(key_cmp, ((PartnerKey0*)key)->priv, sizeof(partner_key0_t));
        status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey1Type() ) {
        key_cmp = (partner_key1_t*)do_malloc(sizeof(partner_key1_t));
        memcpy(key_cmp, ((PartnerKey1*)key)->priv, sizeof(partner_key1_t));
        status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey2Type() ) {
        key_cmp = (partner_key2_t*)do_malloc(sizeof(partner_key2_t));
        memcpy(key_cmp, ((PartnerKey2*)key)->priv, sizeof(partner_key2_t));
        status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getPartnerKey3Type() ) {
        key_cmp = (partner_key3_t*)do_malloc(sizeof(partner_key3_t));
        memcpy(key_cmp, ((PartnerKey3*)key)->priv, sizeof(partner_key3_t));
        status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->g_code, 
                    ((PartnerKey0*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key0_t*)key_cmp)->code, 
                    ((PartnerKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->g_code, 
                    ((PartnerKey1*)key)->priv->g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((partner_key1_t*)key_cmp)->name, 
                    ((PartnerKey1*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key2_t*)key_cmp)->name, 
                    ((PartnerKey2*)key)->priv->name))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((partner_key3_t*)key_cmp)->code, 
                    ((PartnerKey3*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Partner_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getPartnerKey0Type() ) {
            status = do_partner_get0(self->alias->alias, self->priv, ((PartnerKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey1Type() ) {
            status = do_partner_get1(self->alias->alias, self->priv, ((PartnerKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey2Type() ) {
            status = do_partner_get2(self->alias->alias, self->priv, ((PartnerKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getPartnerKey3Type() ) {
            status = do_partner_get3(self->alias->alias, self->priv, ((PartnerKey3*)key)->priv, DO_GET_NEXT);
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

static PyObject *Partner_insert(Partner* self)
{
    int status;
    status = do_partner_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_update(Partner* self)
{
    int status;
    status = do_partner_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_delete(Partner* self)
{
    int status;
    status = do_partner_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Partner_set_alias(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Partner*)self)->alias);
    Py_INCREF(obj);
    ((Partner*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Partner_set(Partner* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerType() ) {
        memcpy(self->priv, ((Partner*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey0Type() ) {

        do_cpy(self->priv->data.g_code, ((PartnerKey0*)obj)->priv->g_code);

        do_cpy(self->priv->data.code, ((PartnerKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey1Type() ) {

        do_cpy(self->priv->data.g_code, ((PartnerKey1*)obj)->priv->g_code);

        do_cpy(self->priv->data.name, ((PartnerKey1*)obj)->priv->name);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey2Type() ) {

        do_cpy(self->priv->data.name, ((PartnerKey2*)obj)->priv->name);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerKey3Type() ) {

        do_cpy(self->priv->data.code, ((PartnerKey3*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataType() ) {

        do_cpy(self->priv->data.g_code, ((PartnerData*)obj)->priv->data.g_code);

        do_cpy(self->priv->data.code, ((PartnerData*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey0Type() ) {

        do_cpy(self->priv->data.g_code, ((PartnerDataKey0*)obj)->priv->g_code);

        do_cpy(self->priv->data.code, ((PartnerDataKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey1Type() ) {

        do_cpy(self->priv->data.g_code, ((PartnerDataKey1*)obj)->priv->g_code);

        do_cpy(self->priv->data.code, ((PartnerDataKey1*)obj)->priv->code);

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
            if ( !strcmp("region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("bank", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_bank", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("f_i_o", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_f_i_o", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("state", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_state", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("record_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_record_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("class", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_class", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("parameter", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_parameter", "O", value) )
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
                do_log(LOG_ERR, "Class \"Partner\"does not contain \"%s\"", field_name);
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

static PyObject *Partner_fields(Partner* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Partner");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Partner_division(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("division", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Partner_store(self,NULL);
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

    value =  Partner_name(self,NULL);
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

    value =  Partner_bank(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("bank", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Partner_f_i_o(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("f_i_o", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Partner_state(self,NULL);
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

    value =  Partner_record_type(self,NULL);
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

    value =  Partner_class(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("class", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Partner_parameter(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("parameter", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Partner_get_params(self, NULL);
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

static PyGetSetDef Partner_getset[] = {

    {"region_code",(getter)Partner_region_code},

    {"code",(getter)Partner_code},

    {"division",(getter)Partner_division},

    {"store",(getter)Partner_store},

    {"name",(getter)Partner_name},

    {"bank",(getter)Partner_bank},

    {"f_i_o",(getter)Partner_f_i_o},

    {"state",(getter)Partner_state},

    {"record_type",(getter)Partner_record_type},

    {"class",(getter)Partner_class},

    {"parameter",(getter)Partner_parameter},

    {"params",(getter)Partner_get_params},

    {NULL}
};

static PyMethodDef Partner_methods[] = {
    {"set", (PyCFunction)Partner_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Partner_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Partner_clone, METH_NOARGS, "clone"},

    {"set_region_code", (PyCFunction)Partner_set_region_code, METH_VARARGS|METH_KEYWORDS, "Partner_set_region_code"},

    {"set_code", (PyCFunction)Partner_set_code, METH_VARARGS|METH_KEYWORDS, "Partner_set_code"},

    {"set_division", (PyCFunction)Partner_set_division, METH_VARARGS|METH_KEYWORDS, "Partner_set_division"},

    {"set_store", (PyCFunction)Partner_set_store, METH_VARARGS|METH_KEYWORDS, "Partner_set_store"},

    {"set_name", (PyCFunction)Partner_set_name, METH_VARARGS|METH_KEYWORDS, "Partner_set_name"},

    {"set_bank", (PyCFunction)Partner_set_bank, METH_VARARGS|METH_KEYWORDS, "Partner_set_bank"},

    {"set_f_i_o", (PyCFunction)Partner_set_f_i_o, METH_VARARGS|METH_KEYWORDS, "Partner_set_f_i_o"},

    {"set_state", (PyCFunction)Partner_set_state, METH_VARARGS|METH_KEYWORDS, "Partner_set_state"},

    {"set_record_type", (PyCFunction)Partner_set_record_type, METH_VARARGS|METH_KEYWORDS, "Partner_set_record_type"},

    {"set_class", (PyCFunction)Partner_set_class, METH_VARARGS|METH_KEYWORDS, "Partner_set_class"},

    {"set_parameter", (PyCFunction)Partner_set_parameter, METH_VARARGS|METH_KEYWORDS, "Partner_set_parameter"},

    {"set_param", (PyCFunction)Partner_set_param, METH_VARARGS|METH_KEYWORDS, "do_Partner_param_set"},
    {"param", (PyCFunction)Partner_get_param, METH_VARARGS|METH_KEYWORDS, "do_Partner_param"},
    {"clear_params", (PyCFunction)Partner_params_clear, METH_NOARGS, "do_Partner_param_clear"},
    {"set_params", (PyCFunction)Partner_set_params, METH_VARARGS|METH_KEYWORDS, "do_Partner_set_params"},

    {"get_gt", (PyCFunction)Partner_gt, METH_VARARGS|METH_KEYWORDS, "Partner_gt"},

    {"get_last", (PyCFunction)Partner_last, METH_VARARGS|METH_KEYWORDS, "Partner_last"},

    {"get_next", (PyCFunction)Partner_next, METH_VARARGS|METH_KEYWORDS, "Partner_next"},

    {"get_le", (PyCFunction)Partner_le, METH_VARARGS|METH_KEYWORDS, "Partner_le"},

    {"get_lt", (PyCFunction)Partner_lt, METH_VARARGS|METH_KEYWORDS, "Partner_lt"},

    {"get_equal", (PyCFunction)Partner_equal, METH_VARARGS|METH_KEYWORDS, "Partner_equal"},

    {"get_ge", (PyCFunction)Partner_ge, METH_VARARGS|METH_KEYWORDS, "Partner_ge"},

    {"get_prev", (PyCFunction)Partner_prev, METH_VARARGS|METH_KEYWORDS, "Partner_prev"},

    {"get_first", (PyCFunction)Partner_first, METH_VARARGS|METH_KEYWORDS, "Partner_first"},

    {"gets_gt", (PyCFunction)Partner_iter_gt, METH_VARARGS|METH_KEYWORDS, "Partner_iter_gt"},

    {"gets_last", (PyCFunction)Partner_iter_last, METH_VARARGS|METH_KEYWORDS, "Partner_iter_last"},

    {"gets_le", (PyCFunction)Partner_iter_le, METH_VARARGS|METH_KEYWORDS, "Partner_iter_le"},

    {"gets_ge", (PyCFunction)Partner_iter_ge, METH_VARARGS|METH_KEYWORDS, "Partner_iter_ge"},

    {"gets_equal", (PyCFunction)Partner_iter_equal, METH_VARARGS|METH_KEYWORDS, "Partner_iter_equal"},

    {"gets_lt", (PyCFunction)Partner_iter_lt, METH_VARARGS|METH_KEYWORDS, "Partner_iter_lt"},

    {"gets_first", (PyCFunction)Partner_iter_first, METH_VARARGS|METH_KEYWORDS, "Partner_iter_first"},

    {"insert", (PyCFunction)Partner_insert, METH_VARARGS|METH_KEYWORDS, "Partner_insert"},

    {"update", (PyCFunction)Partner_update, METH_VARARGS|METH_KEYWORDS, "Partner_update"},

    {"delete", (PyCFunction)Partner_delete, METH_VARARGS|METH_KEYWORDS, "Partner_delete"},

    {NULL}
};

static PyTypeObject PartnerType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Partner",             /*tp_name*/
    sizeof(Partner),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Partner_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Partner_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Partner objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Partner_methods,             /* tp_methods */
    0,
    Partner_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Partner_init,      /* tp_init */
    0,                         /* tp_alloc */
    Partner_new,                 /* tp_new */
};
PyTypeObject *getPartnerType()
{
    return &PartnerType_;
}

static void PartnerKey0_dealloc(PyObject* self)
{
    do_free(((PartnerKey0*)self)->priv);
    Py_DECREF(((PartnerKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("PartnerKey0 free\n");
}
static PyObject *PartnerKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PartnerKey0 new\n");
    PartnerKey0 *self;
    self = PyObject_NEW(PartnerKey0, type);
    //self = (PartnerKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_key0_t));
    return (PyObject *)self;
}
static int PartnerKey0_init(PartnerKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("PartnerKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->g_code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *PartnerKey0_clone(PartnerKey0 *self)
{
    PartnerKey0 *result;
    result = (PartnerKey0*)PyObject_CallFunction((PyObject*)getPartnerKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *PartnerKey0_region_code(PartnerKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerKey0_set_region_code(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerKey0_code(PartnerKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->code);

    return result;
}

static PyObject *PartnerKey0_set_code(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerKey0_gt(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_last(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_next(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_le(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_lt(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_equal(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_ge(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_prev(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_first(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey0_iter_gt(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey0_iter_last(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey0_iter_le(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey0_iter_ge(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey0_iter_equal(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey0_iter_lt(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey0_iter_first(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey0_cmp(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getPartnerKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->g_code, ((PartnerKey0*)obj)->priv->g_code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->code - ((PartnerKey0*)obj)->priv->code;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *PartnerKey0_set_alias(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((PartnerKey0*)self)->alias);
    Py_INCREF(obj);
    ((PartnerKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PartnerKey0_set(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerKey0Type() ) {
        memcpy(self->priv, ((PartnerKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerType() ) {

        do_cpy(self->priv->g_code, ((Partner*)obj)->priv->data.g_code);

        do_cpy(self->priv->code, ((Partner*)obj)->priv->data.code);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataType() ) {

        do_cpy(self->priv->g_code, ((PartnerData*)obj)->priv->data.g_code);

        do_cpy(self->priv->code, ((PartnerData*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey0Type() ) {

        do_cpy(self->priv->g_code, ((PartnerDataKey0*)obj)->priv->g_code);

        do_cpy(self->priv->code, ((PartnerDataKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey1Type() ) {

        do_cpy(self->priv->g_code, ((PartnerDataKey1*)obj)->priv->g_code);

        do_cpy(self->priv->code, ((PartnerDataKey1*)obj)->priv->code);

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
            if ( !strcmp("region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"PartnerKey0\"does not contain \"%s\"", field_name);
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

static PyObject *PartnerKey0_fields(PartnerKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class PartnerKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  PartnerKey0_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerKey0_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("code", value);
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

static PyGetSetDef PartnerKey0_getset[] = {

    {"region_code",(getter)PartnerKey0_region_code},

    {"code",(getter)PartnerKey0_code},

    {NULL}
};

static PyMethodDef PartnerKey0_methods[] = {
    {"set", (PyCFunction)PartnerKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)PartnerKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)PartnerKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)PartnerKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_region_code", (PyCFunction)PartnerKey0_set_region_code, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_set_region_code"},

    {"set_code", (PyCFunction)PartnerKey0_set_code, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_set_code"},

    {"get_gt", (PyCFunction)PartnerKey0_gt, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_gt"},

    {"get_last", (PyCFunction)PartnerKey0_last, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_last"},

    {"get_next", (PyCFunction)PartnerKey0_next, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_next"},

    {"get_le", (PyCFunction)PartnerKey0_le, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_le"},

    {"get_lt", (PyCFunction)PartnerKey0_lt, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_lt"},

    {"get_equal", (PyCFunction)PartnerKey0_equal, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_equal"},

    {"get_ge", (PyCFunction)PartnerKey0_ge, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_ge"},

    {"get_prev", (PyCFunction)PartnerKey0_prev, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_prev"},

    {"get_first", (PyCFunction)PartnerKey0_first, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_first"},

    {"gets_gt", (PyCFunction)PartnerKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_iter_gt"},

    {"gets_last", (PyCFunction)PartnerKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_iter_last"},

    {"gets_le", (PyCFunction)PartnerKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_iter_le"},

    {"gets_ge", (PyCFunction)PartnerKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_iter_ge"},

    {"gets_equal", (PyCFunction)PartnerKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_iter_equal"},

    {"gets_lt", (PyCFunction)PartnerKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_iter_lt"},

    {"gets_first", (PyCFunction)PartnerKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "PartnerKey0_iter_first"},

    {NULL}
};

static PyTypeObject PartnerKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PartnerKey0",             /*tp_name*/
    sizeof(PartnerKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PartnerKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerKey0_methods,             /* tp_methods */
    0,
    PartnerKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerKey0_new,                 /* tp_new */
};
PyTypeObject *getPartnerKey0Type()
{
    return &PartnerKey0Type_;
}

static void PartnerKey1_dealloc(PyObject* self)
{
    do_free(((PartnerKey1*)self)->priv);
    Py_DECREF(((PartnerKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("PartnerKey1 free\n");
}
static PyObject *PartnerKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PartnerKey1 new\n");
    PartnerKey1 *self;
    self = PyObject_NEW(PartnerKey1, type);
    //self = (PartnerKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_key1_t));
    return (PyObject *)self;
}
static int PartnerKey1_init(PartnerKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("PartnerKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->g_code);

    do_text_set_empty(self->priv->name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *PartnerKey1_clone(PartnerKey1 *self)
{
    PartnerKey1 *result;
    result = (PartnerKey1*)PyObject_CallFunction((PyObject*)getPartnerKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *PartnerKey1_region_code(PartnerKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerKey1_set_region_code(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerKey1_name(PartnerKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerKey1_set_name(PartnerKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *PartnerKey1_gt(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_last(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_next(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_le(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_lt(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_equal(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_ge(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_prev(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_first(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey1_iter_gt(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey1_iter_last(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey1_iter_le(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey1_iter_ge(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey1_iter_equal(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey1_iter_lt(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey1_iter_first(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.g_code, 
                 self->priv->g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey1_cmp(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getPartnerKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->g_code, ((PartnerKey1*)obj)->priv->g_code);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->name, ((PartnerKey1*)obj)->priv->name);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *PartnerKey1_set_alias(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((PartnerKey1*)self)->alias);
    Py_INCREF(obj);
    ((PartnerKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PartnerKey1_set(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerKey1Type() ) {
        memcpy(self->priv, ((PartnerKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerType() ) {

        do_cpy(self->priv->g_code, ((Partner*)obj)->priv->data.g_code);

        do_cpy(self->priv->name, ((Partner*)obj)->priv->data.name);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
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
    if ( Py_TYPE(obj) == getPartnerDataType() ) {

        do_cpy(self->priv->g_code, ((PartnerData*)obj)->priv->data.g_code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey0Type() ) {

        do_cpy(self->priv->g_code, ((PartnerDataKey0*)obj)->priv->g_code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey1Type() ) {

        do_cpy(self->priv->g_code, ((PartnerDataKey1*)obj)->priv->g_code);

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
            if ( !strcmp("region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"PartnerKey1\"does not contain \"%s\"", field_name);
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

static PyObject *PartnerKey1_fields(PartnerKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class PartnerKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  PartnerKey1_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  PartnerKey1_name(self,NULL);
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

static PyGetSetDef PartnerKey1_getset[] = {

    {"region_code",(getter)PartnerKey1_region_code},

    {"name",(getter)PartnerKey1_name},

    {NULL}
};

static PyMethodDef PartnerKey1_methods[] = {
    {"set", (PyCFunction)PartnerKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)PartnerKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)PartnerKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)PartnerKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_region_code", (PyCFunction)PartnerKey1_set_region_code, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_set_region_code"},

    {"set_name", (PyCFunction)PartnerKey1_set_name, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_set_name"},

    {"get_gt", (PyCFunction)PartnerKey1_gt, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_gt"},

    {"get_last", (PyCFunction)PartnerKey1_last, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_last"},

    {"get_next", (PyCFunction)PartnerKey1_next, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_next"},

    {"get_le", (PyCFunction)PartnerKey1_le, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_le"},

    {"get_lt", (PyCFunction)PartnerKey1_lt, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_lt"},

    {"get_equal", (PyCFunction)PartnerKey1_equal, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_equal"},

    {"get_ge", (PyCFunction)PartnerKey1_ge, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_ge"},

    {"get_prev", (PyCFunction)PartnerKey1_prev, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_prev"},

    {"get_first", (PyCFunction)PartnerKey1_first, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_first"},

    {"gets_gt", (PyCFunction)PartnerKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_iter_gt"},

    {"gets_last", (PyCFunction)PartnerKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_iter_last"},

    {"gets_le", (PyCFunction)PartnerKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_iter_le"},

    {"gets_ge", (PyCFunction)PartnerKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_iter_ge"},

    {"gets_equal", (PyCFunction)PartnerKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_iter_equal"},

    {"gets_lt", (PyCFunction)PartnerKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_iter_lt"},

    {"gets_first", (PyCFunction)PartnerKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "PartnerKey1_iter_first"},

    {NULL}
};

static PyTypeObject PartnerKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PartnerKey1",             /*tp_name*/
    sizeof(PartnerKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PartnerKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerKey1_methods,             /* tp_methods */
    0,
    PartnerKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerKey1_new,                 /* tp_new */
};
PyTypeObject *getPartnerKey1Type()
{
    return &PartnerKey1Type_;
}

static void PartnerKey2_dealloc(PyObject* self)
{
    do_free(((PartnerKey2*)self)->priv);
    Py_DECREF(((PartnerKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("PartnerKey2 free\n");
}
static PyObject *PartnerKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PartnerKey2 new\n");
    PartnerKey2 *self;
    self = PyObject_NEW(PartnerKey2, type);
    //self = (PartnerKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_key2_t));
    return (PyObject *)self;
}
static int PartnerKey2_init(PartnerKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("PartnerKey2 init\n");
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

static PyObject *PartnerKey2_clone(PartnerKey2 *self)
{
    PartnerKey2 *result;
    result = (PartnerKey2*)PyObject_CallFunction((PyObject*)getPartnerKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *PartnerKey2_name(PartnerKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *PartnerKey2_set_name(PartnerKey2* self, PyObject *args, PyObject *kwds)
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

static PyObject *PartnerKey2_gt(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_last(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_next(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_le(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_lt(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_equal(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_ge(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_prev(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_first(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey2_iter_gt(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey2_iter_last(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey2_iter_le(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey2_iter_ge(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey2_iter_equal(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey2_iter_lt(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey2_iter_first(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.name, 
                 self->priv->name))
               break;
       }

 
        item = PartnerKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey2_cmp(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getPartnerKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->name, ((PartnerKey2*)obj)->priv->name);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *PartnerKey2_set_alias(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((PartnerKey2*)self)->alias);
    Py_INCREF(obj);
    ((PartnerKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PartnerKey2_set(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerKey2Type() ) {
        memcpy(self->priv, ((PartnerKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerType() ) {

        do_cpy(self->priv->name, ((Partner*)obj)->priv->data.name);

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
    if ( Py_TYPE(obj) == getPartnerDataType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey1Type() ) {

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
                do_log(LOG_ERR, "Class \"PartnerKey2\"does not contain \"%s\"", field_name);
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

static PyObject *PartnerKey2_fields(PartnerKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class PartnerKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  PartnerKey2_name(self,NULL);
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

static PyGetSetDef PartnerKey2_getset[] = {

    {"name",(getter)PartnerKey2_name},

    {NULL}
};

static PyMethodDef PartnerKey2_methods[] = {
    {"set", (PyCFunction)PartnerKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)PartnerKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)PartnerKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)PartnerKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_name", (PyCFunction)PartnerKey2_set_name, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_set_name"},

    {"get_gt", (PyCFunction)PartnerKey2_gt, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_gt"},

    {"get_last", (PyCFunction)PartnerKey2_last, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_last"},

    {"get_next", (PyCFunction)PartnerKey2_next, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_next"},

    {"get_le", (PyCFunction)PartnerKey2_le, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_le"},

    {"get_lt", (PyCFunction)PartnerKey2_lt, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_lt"},

    {"get_equal", (PyCFunction)PartnerKey2_equal, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_equal"},

    {"get_ge", (PyCFunction)PartnerKey2_ge, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_ge"},

    {"get_prev", (PyCFunction)PartnerKey2_prev, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_prev"},

    {"get_first", (PyCFunction)PartnerKey2_first, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_first"},

    {"gets_gt", (PyCFunction)PartnerKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_iter_gt"},

    {"gets_last", (PyCFunction)PartnerKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_iter_last"},

    {"gets_le", (PyCFunction)PartnerKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_iter_le"},

    {"gets_ge", (PyCFunction)PartnerKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_iter_ge"},

    {"gets_equal", (PyCFunction)PartnerKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_iter_equal"},

    {"gets_lt", (PyCFunction)PartnerKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_iter_lt"},

    {"gets_first", (PyCFunction)PartnerKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "PartnerKey2_iter_first"},

    {NULL}
};

static PyTypeObject PartnerKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PartnerKey2",             /*tp_name*/
    sizeof(PartnerKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PartnerKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerKey2_methods,             /* tp_methods */
    0,
    PartnerKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerKey2_new,                 /* tp_new */
};
PyTypeObject *getPartnerKey2Type()
{
    return &PartnerKey2Type_;
}

static void PartnerKey3_dealloc(PyObject* self)
{
    do_free(((PartnerKey3*)self)->priv);
    Py_DECREF(((PartnerKey3*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("PartnerKey3 free\n");
}
static PyObject *PartnerKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("PartnerKey3 new\n");
    PartnerKey3 *self;
    self = PyObject_NEW(PartnerKey3, type);
    //self = (PartnerKey3 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(partner_key3_t));
    return (PyObject *)self;
}
static int PartnerKey3_init(PartnerKey3 *self, PyObject *args, PyObject *kwds)
{
    //LOG("PartnerKey3 init\n");
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

static PyObject *PartnerKey3_clone(PartnerKey3 *self)
{
    PartnerKey3 *result;
    result = (PartnerKey3*)PyObject_CallFunction((PyObject*)getPartnerKey3Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *PartnerKey3_code(PartnerKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->code);

    return result;
}

static PyObject *PartnerKey3_set_code(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *PartnerKey3_gt(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_last(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_next(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_le(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_lt(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_equal(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_ge(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_prev(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_first(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *PartnerKey3_iter_gt(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey3_iter_last(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey3_iter_le(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey3_iter_ge(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey3_iter_equal(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey3_iter_lt(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey3_iter_first(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    partner_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_partner_key3(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = PartnerKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_partner_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *PartnerKey3_cmp(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getPartnerKey3Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = self->priv->code - ((PartnerKey3*)obj)->priv->code;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *PartnerKey3_set_alias(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((PartnerKey3*)self)->alias);
    Py_INCREF(obj);
    ((PartnerKey3*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *PartnerKey3_set(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getPartnerKey3Type() ) {
        memcpy(self->priv, ((PartnerKey3*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getPartnerType() ) {

        do_cpy(self->priv->code, ((Partner*)obj)->priv->data.code);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                        return NULL;
                    break;

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataType() ) {

        do_cpy(self->priv->code, ((PartnerData*)obj)->priv->data.code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey0Type() ) {

        do_cpy(self->priv->code, ((PartnerDataKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getPartnerDataKey1Type() ) {

        do_cpy(self->priv->code, ((PartnerDataKey1*)obj)->priv->code);

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
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"PartnerKey3\"does not contain \"%s\"", field_name);
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

static PyObject *PartnerKey3_fields(PartnerKey3* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class PartnerKey3");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  PartnerKey3_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("code", value);
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

static PyGetSetDef PartnerKey3_getset[] = {

    {"code",(getter)PartnerKey3_code},

    {NULL}
};

static PyMethodDef PartnerKey3_methods[] = {
    {"set", (PyCFunction)PartnerKey3_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)PartnerKey3_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)PartnerKey3_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)PartnerKey3_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)PartnerKey3_set_code, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_set_code"},

    {"get_gt", (PyCFunction)PartnerKey3_gt, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_gt"},

    {"get_last", (PyCFunction)PartnerKey3_last, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_last"},

    {"get_next", (PyCFunction)PartnerKey3_next, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_next"},

    {"get_le", (PyCFunction)PartnerKey3_le, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_le"},

    {"get_lt", (PyCFunction)PartnerKey3_lt, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_lt"},

    {"get_equal", (PyCFunction)PartnerKey3_equal, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_equal"},

    {"get_ge", (PyCFunction)PartnerKey3_ge, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_ge"},

    {"get_prev", (PyCFunction)PartnerKey3_prev, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_prev"},

    {"get_first", (PyCFunction)PartnerKey3_first, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_first"},

    {"gets_gt", (PyCFunction)PartnerKey3_iter_gt, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_iter_gt"},

    {"gets_last", (PyCFunction)PartnerKey3_iter_last, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_iter_last"},

    {"gets_le", (PyCFunction)PartnerKey3_iter_le, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_iter_le"},

    {"gets_ge", (PyCFunction)PartnerKey3_iter_ge, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_iter_ge"},

    {"gets_equal", (PyCFunction)PartnerKey3_iter_equal, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_iter_equal"},

    {"gets_lt", (PyCFunction)PartnerKey3_iter_lt, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_iter_lt"},

    {"gets_first", (PyCFunction)PartnerKey3_iter_first, METH_VARARGS|METH_KEYWORDS, "PartnerKey3_iter_first"},

    {NULL}
};

static PyTypeObject PartnerKey3Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.PartnerKey3",             /*tp_name*/
    sizeof(PartnerKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)PartnerKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)PartnerKey3_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "PartnerKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    PartnerKey3_methods,             /* tp_methods */
    0,
    PartnerKey3_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)PartnerKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    PartnerKey3_new,                 /* tp_new */
};
PyTypeObject *getPartnerKey3Type()
{
    return &PartnerKey3Type_;
}
