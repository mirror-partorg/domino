
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "document.h"

static void Document_dealloc(PyObject* self)
{
    do_free(((Document*)self)->priv);
    Py_DECREF(((Document*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Document free\n");
}
static PyObject *Document_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Document new\n");
    Document *self;
    self = PyObject_NEW(Document, type);
    //self = (Document *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_rec_t));
    return (PyObject *)self;
}
static int Document_init(Document *self, PyObject *args, PyObject *kwds)
{
    //LOG("Document init\n");
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

    do_text_set_empty(self->priv->data.sklad);

    do_text_set_empty(self->priv->data.document);

    do_text_set_empty(self->priv->data.p_g_code);

    do_text_set_empty(self->priv->data.p_doc);

    do_text_set_empty(self->priv->data.party);

    do_document_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Document_clone(Document *self)
{
    Document *result;
    result = (Document*)PyObject_CallFunction((PyObject*)getDocumentType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Document_document_type(Document* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Document_set_document_type(Document* self, PyObject *args, PyObject *kwds)
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

static PyObject *Document_document_operation(Document* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.dtype1);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Document_set_document_operation(Document* self, PyObject *args, PyObject *kwds)
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

static PyObject *Document_name(Document* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Document_set_name(Document* self, PyObject *args, PyObject *kwds)
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

static PyObject *Document_store(Document* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Document_set_store(Document* self, PyObject *args, PyObject *kwds)
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

static PyObject *Document_document(Document* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Document_set_document(Document* self, PyObject *args, PyObject *kwds)
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

static PyObject *Document_date(Document* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Document_date_raw(Document* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *Document_set_date(Document* self, PyObject *args, PyObject *kwds)
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

static PyObject *Document_time(Document* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->data.time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Document_time_raw(Document* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.time);

    return result;
}

static PyObject *Document_set_time(Document* self, PyObject *args, PyObject *kwds)
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

static PyObject *Document_partner_region_code(Document* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Document_set_partner_region_code(Document* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Document_partner_code(Document* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.p_code);

    return result;
}

static PyObject *Document_set_partner_code(Document* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Document_partner_document(Document* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.p_doc);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Document_set_partner_document(Document* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_doc");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.p_doc, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.p_doc);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Document_partner_document_date(Document* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.p_date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Document_set_partner_document_date(Document* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_date");
        return NULL;
    }
    struct tm tm;
    BTI_LONG date;
    if ( value ) {
        if ( !PyDate_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_date");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.p_date = MyLong_AsLong(value);
        } else {
            do_date_set_ymd(&tm, PyDateTime_GET_YEAR(value), PyDateTime_GET_MONTH(value), PyDateTime_GET_DAY(value));
            self->priv->data.p_date = do_date_set(&date, tm);
        }
    }
    /*do_date(self->priv->data.p_date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Document_partner_document_time(Document* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->data.p_time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *Document_set_partner_document_time(Document* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_time");
        return NULL;
    }
    struct tm tm;
    BTI_LONG time_;
    if ( value ) {
        if ( !PyTime_Check(value) && !PyDateTime_Check(value) && !MyLong_Check(value) ) {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.p_time");
            return NULL;
        }
        if ( MyLong_Check(value) ) {
            self->priv->data.p_time = MyLong_AsLong(value);
        } else {
            if ( PyTime_Check(value) ) {
                tm.tm_hour = PyDateTime_TIME_GET_HOUR(value);tm.tm_min=PyDateTime_TIME_GET_MINUTE(value);tm.tm_sec=PyDateTime_TIME_GET_SECOND(value);
             }
             else {
                 tm.tm_hour = PyDateTime_DATE_GET_HOUR(value);tm.tm_min=PyDateTime_DATE_GET_MINUTE(value);tm.tm_sec=PyDateTime_DATE_GET_SECOND(value);
             }
             self->priv->data.p_time = do_time_set(&time_, tm);
        }
    }
    /*do_time(self->priv->data.p_time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Document_party(Document* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.party);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Document_set_party(Document* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.party");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.party, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.party);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Document_accepted(Document* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.accepted);

    return result;
}

static PyObject *Document_set_accepted(Document* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.accepted");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.accepted = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.accepted = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.accepted");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.accepted);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Document_set_param(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_document_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_document_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_document_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Document_set_params(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_params_clear(self->alias->alias,self->priv);
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
                 do_document_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_document_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Document_get_param(Document* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_document_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Document_params_clear(Document* self)
{
    do_document_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Document_get_params(Document* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}

static PyObject *Document_get_sum(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get data.sumincl");
        return NULL;
    }
    result = PyFloat_FromDouble(do_document_sum(self->alias->alias,self->priv,number));

    return result;
}
static PyObject *Document_set_sum(Document* self, PyObject *args, PyObject *kwds)
{

    double value;
    int number;
    static char *kwlist[] = {"number","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "id|", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set data.sumincl");
        return NULL;
    }
    do_document_sum_set(self->alias->alias,self->priv,number,value);
    
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Document_sums_clear(Document* self)
{
    do_document_sum_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Document_get_sums(Document* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    char *sum;
    do_list_t *list;
    struct {
       int number;
       double value;
    } rec, *item;
    
    sum = do_document_param(self->alias->alias,self->priv, "SUM_MAP");
    list = do_list_new(TRUE);    

    char *cp = sum, *head;
    for (i = 0; *cp != '\0'; i++) {
        head = cp;
        while (*cp != '\0' && *cp != ',') cp++;
        if (head < cp + 1) {
            rec.number = atoi(head);
            do_list_add_alloc(list, &rec, sizeof(rec));
        }
        if (*cp != '\0') cp++;
    }
    do_free(sum);
    sum = do_document_param(self->alias->alias,self->priv, "SUM");
    cp = sum;
    i = 0;
    while (*cp != '\0') {
        head = cp;
        while (*cp != '\0' && *cp != ',') cp++;
        item=list->list[i++];
        if (head != cp) {
           if ( *cp != '\0' ) {
               *cp = '\0';
               item->value=  do_atof(head);
               cp++;
           }
           else {
               item->value=  do_atof(head);
               break;
           }
        }
        else {
            item->value=0;
            if ( *cp != '\0' ) 
               cp++;
        }
    }
    do_free(sum);
    
    for ( i = 0; i < list->count; i++ ) {
        item=list->list[i];
        val = PyFloat_FromDouble(item->value);
        key = MyLong_FromLong(item->number);
        PyDict_SetItem(result, key, val);
    }
    do_list_free(list);
    return result;
}
static PyObject *Document_set_sums(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_document_sum_clear(self->alias->alias,self->priv);
        int i;
        PyObject *pKeys = PyDict_Keys(value);
        for ( i = 0; i < PyList_Size(pKeys); ++i ) {
             PyObject *pKey = PyList_GetItem(pKeys, i);
             PyObject *pValue;
             int number;
             if ( !pKey ) {
                do_log(LOG_ERR, "Invalid key");
                break;
             }
             if ( MyLong_Check(pKey) )
                  number = MyLong_AsLong(pKey);
             else {
                 do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
                 return NULL;
             }
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_document_sum_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_document_sum_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
             else {
                 do_log(LOG_ERR, "Invalid argument \"value iter\": wrong type");
                 return NULL;
             }
        }
    }
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Document_equal(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_next(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_prev(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_gt(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_ge(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_lt(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_le(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_first(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_last(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getDocumentKey0Type() )
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() )
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() )
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() )
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() )
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_iter_equal(Document* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentKey0Type() ) {
        key_cmp = (document_key0_t*)do_malloc(sizeof(document_key0_t));
        memcpy(key_cmp, ((DocumentKey0*)key)->priv, sizeof(document_key0_t));
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() ) {
        key_cmp = (document_key1_t*)do_malloc(sizeof(document_key1_t));
        memcpy(key_cmp, ((DocumentKey1*)key)->priv, sizeof(document_key1_t));
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() ) {
        key_cmp = (document_key2_t*)do_malloc(sizeof(document_key2_t));
        memcpy(key_cmp, ((DocumentKey2*)key)->priv, sizeof(document_key2_t));
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() ) {
        key_cmp = (document_key3_t*)do_malloc(sizeof(document_key3_t));
        memcpy(key_cmp, ((DocumentKey3*)key)->priv, sizeof(document_key3_t));
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() ) {
        key_cmp = (document_key4_t*)do_malloc(sizeof(document_key4_t));
        memcpy(key_cmp, ((DocumentKey4*)key)->priv, sizeof(document_key4_t));
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->dtype, 
                    ((DocumentKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->sklad, 
                    ((DocumentKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->document, 
                    ((DocumentKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->dtype, 
                    ((DocumentKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->date, 
                    ((DocumentKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->time, 
                    ((DocumentKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_g_code, 
                    ((DocumentKey2*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_code, 
                    ((DocumentKey2*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->party, 
                    ((DocumentKey2*)key)->priv->party))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->date, 
                    ((DocumentKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->time, 
                    ((DocumentKey2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->dtype, 
                    ((DocumentKey3*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->sklad, 
                    ((DocumentKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->date, 
                    ((DocumentKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->time, 
                    ((DocumentKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->dtype, 
                    ((DocumentKey4*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_g_code, 
                    ((DocumentKey4*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_code, 
                    ((DocumentKey4*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->date, 
                    ((DocumentKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->time, 
                    ((DocumentKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Document_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
            status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
            status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
            status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
            status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
            status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Document_iter_gt(Document* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentKey0Type() ) {
        key_cmp = (document_key0_t*)do_malloc(sizeof(document_key0_t));
        memcpy(key_cmp, ((DocumentKey0*)key)->priv, sizeof(document_key0_t));
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() ) {
        key_cmp = (document_key1_t*)do_malloc(sizeof(document_key1_t));
        memcpy(key_cmp, ((DocumentKey1*)key)->priv, sizeof(document_key1_t));
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() ) {
        key_cmp = (document_key2_t*)do_malloc(sizeof(document_key2_t));
        memcpy(key_cmp, ((DocumentKey2*)key)->priv, sizeof(document_key2_t));
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() ) {
        key_cmp = (document_key3_t*)do_malloc(sizeof(document_key3_t));
        memcpy(key_cmp, ((DocumentKey3*)key)->priv, sizeof(document_key3_t));
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() ) {
        key_cmp = (document_key4_t*)do_malloc(sizeof(document_key4_t));
        memcpy(key_cmp, ((DocumentKey4*)key)->priv, sizeof(document_key4_t));
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->dtype, 
                    ((DocumentKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->sklad, 
                    ((DocumentKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->document, 
                    ((DocumentKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->dtype, 
                    ((DocumentKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->date, 
                    ((DocumentKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->time, 
                    ((DocumentKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_g_code, 
                    ((DocumentKey2*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_code, 
                    ((DocumentKey2*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->party, 
                    ((DocumentKey2*)key)->priv->party))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->date, 
                    ((DocumentKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->time, 
                    ((DocumentKey2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->dtype, 
                    ((DocumentKey3*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->sklad, 
                    ((DocumentKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->date, 
                    ((DocumentKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->time, 
                    ((DocumentKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->dtype, 
                    ((DocumentKey4*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_g_code, 
                    ((DocumentKey4*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_code, 
                    ((DocumentKey4*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->date, 
                    ((DocumentKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->time, 
                    ((DocumentKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Document_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
            status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
            status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
            status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
            status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
            status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Document_iter_ge(Document* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentKey0Type() ) {
        key_cmp = (document_key0_t*)do_malloc(sizeof(document_key0_t));
        memcpy(key_cmp, ((DocumentKey0*)key)->priv, sizeof(document_key0_t));
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() ) {
        key_cmp = (document_key1_t*)do_malloc(sizeof(document_key1_t));
        memcpy(key_cmp, ((DocumentKey1*)key)->priv, sizeof(document_key1_t));
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() ) {
        key_cmp = (document_key2_t*)do_malloc(sizeof(document_key2_t));
        memcpy(key_cmp, ((DocumentKey2*)key)->priv, sizeof(document_key2_t));
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() ) {
        key_cmp = (document_key3_t*)do_malloc(sizeof(document_key3_t));
        memcpy(key_cmp, ((DocumentKey3*)key)->priv, sizeof(document_key3_t));
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() ) {
        key_cmp = (document_key4_t*)do_malloc(sizeof(document_key4_t));
        memcpy(key_cmp, ((DocumentKey4*)key)->priv, sizeof(document_key4_t));
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->dtype, 
                    ((DocumentKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->sklad, 
                    ((DocumentKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->document, 
                    ((DocumentKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->dtype, 
                    ((DocumentKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->date, 
                    ((DocumentKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->time, 
                    ((DocumentKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_g_code, 
                    ((DocumentKey2*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_code, 
                    ((DocumentKey2*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->party, 
                    ((DocumentKey2*)key)->priv->party))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->date, 
                    ((DocumentKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->time, 
                    ((DocumentKey2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->dtype, 
                    ((DocumentKey3*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->sklad, 
                    ((DocumentKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->date, 
                    ((DocumentKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->time, 
                    ((DocumentKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->dtype, 
                    ((DocumentKey4*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_g_code, 
                    ((DocumentKey4*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_code, 
                    ((DocumentKey4*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->date, 
                    ((DocumentKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->time, 
                    ((DocumentKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Document_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
            status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
            status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
            status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
            status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
            status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Document_iter_lt(Document* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentKey0Type() ) {
        key_cmp = (document_key0_t*)do_malloc(sizeof(document_key0_t));
        memcpy(key_cmp, ((DocumentKey0*)key)->priv, sizeof(document_key0_t));
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() ) {
        key_cmp = (document_key1_t*)do_malloc(sizeof(document_key1_t));
        memcpy(key_cmp, ((DocumentKey1*)key)->priv, sizeof(document_key1_t));
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() ) {
        key_cmp = (document_key2_t*)do_malloc(sizeof(document_key2_t));
        memcpy(key_cmp, ((DocumentKey2*)key)->priv, sizeof(document_key2_t));
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() ) {
        key_cmp = (document_key3_t*)do_malloc(sizeof(document_key3_t));
        memcpy(key_cmp, ((DocumentKey3*)key)->priv, sizeof(document_key3_t));
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() ) {
        key_cmp = (document_key4_t*)do_malloc(sizeof(document_key4_t));
        memcpy(key_cmp, ((DocumentKey4*)key)->priv, sizeof(document_key4_t));
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->dtype, 
                    ((DocumentKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->sklad, 
                    ((DocumentKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->document, 
                    ((DocumentKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->dtype, 
                    ((DocumentKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->date, 
                    ((DocumentKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->time, 
                    ((DocumentKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_g_code, 
                    ((DocumentKey2*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_code, 
                    ((DocumentKey2*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->party, 
                    ((DocumentKey2*)key)->priv->party))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->date, 
                    ((DocumentKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->time, 
                    ((DocumentKey2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->dtype, 
                    ((DocumentKey3*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->sklad, 
                    ((DocumentKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->date, 
                    ((DocumentKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->time, 
                    ((DocumentKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->dtype, 
                    ((DocumentKey4*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_g_code, 
                    ((DocumentKey4*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_code, 
                    ((DocumentKey4*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->date, 
                    ((DocumentKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->time, 
                    ((DocumentKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Document_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
            status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
            status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
            status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
            status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
            status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Document_iter_le(Document* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentKey0Type() ) {
        key_cmp = (document_key0_t*)do_malloc(sizeof(document_key0_t));
        memcpy(key_cmp, ((DocumentKey0*)key)->priv, sizeof(document_key0_t));
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() ) {
        key_cmp = (document_key1_t*)do_malloc(sizeof(document_key1_t));
        memcpy(key_cmp, ((DocumentKey1*)key)->priv, sizeof(document_key1_t));
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() ) {
        key_cmp = (document_key2_t*)do_malloc(sizeof(document_key2_t));
        memcpy(key_cmp, ((DocumentKey2*)key)->priv, sizeof(document_key2_t));
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() ) {
        key_cmp = (document_key3_t*)do_malloc(sizeof(document_key3_t));
        memcpy(key_cmp, ((DocumentKey3*)key)->priv, sizeof(document_key3_t));
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() ) {
        key_cmp = (document_key4_t*)do_malloc(sizeof(document_key4_t));
        memcpy(key_cmp, ((DocumentKey4*)key)->priv, sizeof(document_key4_t));
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->dtype, 
                    ((DocumentKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->sklad, 
                    ((DocumentKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->document, 
                    ((DocumentKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->dtype, 
                    ((DocumentKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->date, 
                    ((DocumentKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->time, 
                    ((DocumentKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_g_code, 
                    ((DocumentKey2*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_code, 
                    ((DocumentKey2*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->party, 
                    ((DocumentKey2*)key)->priv->party))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->date, 
                    ((DocumentKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->time, 
                    ((DocumentKey2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->dtype, 
                    ((DocumentKey3*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->sklad, 
                    ((DocumentKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->date, 
                    ((DocumentKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->time, 
                    ((DocumentKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->dtype, 
                    ((DocumentKey4*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_g_code, 
                    ((DocumentKey4*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_code, 
                    ((DocumentKey4*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->date, 
                    ((DocumentKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->time, 
                    ((DocumentKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Document_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
            status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
            status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
            status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
            status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
            status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Document_iter_first(Document* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentKey0Type() ) {
        key_cmp = (document_key0_t*)do_malloc(sizeof(document_key0_t));
        memcpy(key_cmp, ((DocumentKey0*)key)->priv, sizeof(document_key0_t));
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() ) {
        key_cmp = (document_key1_t*)do_malloc(sizeof(document_key1_t));
        memcpy(key_cmp, ((DocumentKey1*)key)->priv, sizeof(document_key1_t));
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() ) {
        key_cmp = (document_key2_t*)do_malloc(sizeof(document_key2_t));
        memcpy(key_cmp, ((DocumentKey2*)key)->priv, sizeof(document_key2_t));
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() ) {
        key_cmp = (document_key3_t*)do_malloc(sizeof(document_key3_t));
        memcpy(key_cmp, ((DocumentKey3*)key)->priv, sizeof(document_key3_t));
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() ) {
        key_cmp = (document_key4_t*)do_malloc(sizeof(document_key4_t));
        memcpy(key_cmp, ((DocumentKey4*)key)->priv, sizeof(document_key4_t));
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->dtype, 
                    ((DocumentKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->sklad, 
                    ((DocumentKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->document, 
                    ((DocumentKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->dtype, 
                    ((DocumentKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->date, 
                    ((DocumentKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->time, 
                    ((DocumentKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_g_code, 
                    ((DocumentKey2*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_code, 
                    ((DocumentKey2*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->party, 
                    ((DocumentKey2*)key)->priv->party))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->date, 
                    ((DocumentKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->time, 
                    ((DocumentKey2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->dtype, 
                    ((DocumentKey3*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->sklad, 
                    ((DocumentKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->date, 
                    ((DocumentKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->time, 
                    ((DocumentKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->dtype, 
                    ((DocumentKey4*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_g_code, 
                    ((DocumentKey4*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_code, 
                    ((DocumentKey4*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->date, 
                    ((DocumentKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->time, 
                    ((DocumentKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Document_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
            status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
            status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
            status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
            status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
            status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Document_iter_last(Document* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getDocumentKey0Type() ) {
        key_cmp = (document_key0_t*)do_malloc(sizeof(document_key0_t));
        memcpy(key_cmp, ((DocumentKey0*)key)->priv, sizeof(document_key0_t));
        status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey1Type() ) {
        key_cmp = (document_key1_t*)do_malloc(sizeof(document_key1_t));
        memcpy(key_cmp, ((DocumentKey1*)key)->priv, sizeof(document_key1_t));
        status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey2Type() ) {
        key_cmp = (document_key2_t*)do_malloc(sizeof(document_key2_t));
        memcpy(key_cmp, ((DocumentKey2*)key)->priv, sizeof(document_key2_t));
        status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey3Type() ) {
        key_cmp = (document_key3_t*)do_malloc(sizeof(document_key3_t));
        memcpy(key_cmp, ((DocumentKey3*)key)->priv, sizeof(document_key3_t));
        status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getDocumentKey4Type() ) {
        key_cmp = (document_key4_t*)do_malloc(sizeof(document_key4_t));
        memcpy(key_cmp, ((DocumentKey4*)key)->priv, sizeof(document_key4_t));
        status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->dtype, 
                    ((DocumentKey0*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->sklad, 
                    ((DocumentKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key0_t*)key_cmp)->document, 
                    ((DocumentKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->dtype, 
                    ((DocumentKey1*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->date, 
                    ((DocumentKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key1_t*)key_cmp)->time, 
                    ((DocumentKey1*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_g_code, 
                    ((DocumentKey2*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->p_code, 
                    ((DocumentKey2*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->party, 
                    ((DocumentKey2*)key)->priv->party))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->date, 
                    ((DocumentKey2*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key2_t*)key_cmp)->time, 
                    ((DocumentKey2*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->dtype, 
                    ((DocumentKey3*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->sklad, 
                    ((DocumentKey3*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->date, 
                    ((DocumentKey3*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key3_t*)key_cmp)->time, 
                    ((DocumentKey3*)key)->priv->time))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->dtype, 
                    ((DocumentKey4*)key)->priv->dtype))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_g_code, 
                    ((DocumentKey4*)key)->priv->p_g_code))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->p_code, 
                    ((DocumentKey4*)key)->priv->p_code))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->date, 
                    ((DocumentKey4*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((document_key4_t*)key_cmp)->time, 
                    ((DocumentKey4*)key)->priv->time))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Document_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getDocumentKey0Type() ) {
            status = do_document_get0(self->alias->alias, self->priv, ((DocumentKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey1Type() ) {
            status = do_document_get1(self->alias->alias, self->priv, ((DocumentKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey2Type() ) {
            status = do_document_get2(self->alias->alias, self->priv, ((DocumentKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey3Type() ) {
            status = do_document_get3(self->alias->alias, self->priv, ((DocumentKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getDocumentKey4Type() ) {
            status = do_document_get4(self->alias->alias, self->priv, ((DocumentKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Document_insert(Document* self)
{
    int status;
    status = do_document_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_update(Document* self)
{
    int status;
    status = do_document_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_delete(Document* self)
{
    int status;
    status = do_document_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Document_set_alias(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Document*)self)->alias);
    Py_INCREF(obj);
    ((Document*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Document_set(Document* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentType() ) {
        memcpy(self->priv, ((Document*)obj)->priv, sizeof(*self->priv));
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

        do_cpy(self->priv->data.date, ((DocumentKey1*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((DocumentKey1*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {

        do_cpy(self->priv->data.date, ((DocumentKey2*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((DocumentKey2*)obj)->priv->time);

        do_cpy(self->priv->data.p_g_code, ((DocumentKey2*)obj)->priv->p_g_code);

        do_cpy(self->priv->data.p_code, ((DocumentKey2*)obj)->priv->p_code);

        do_cpy(self->priv->data.party, ((DocumentKey2*)obj)->priv->party);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey3*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentKey3*)obj)->priv->sklad);

        do_cpy(self->priv->data.date, ((DocumentKey3*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((DocumentKey3*)obj)->priv->time);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentKey4*)obj)->priv->dtype);

        do_cpy(self->priv->data.date, ((DocumentKey4*)obj)->priv->date);

        do_cpy(self->priv->data.time, ((DocumentKey4*)obj)->priv->time);

        do_cpy(self->priv->data.p_g_code, ((DocumentKey4*)obj)->priv->p_g_code);

        do_cpy(self->priv->data.p_code, ((DocumentKey4*)obj)->priv->p_code);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->data.dtype, ((DocumentOrder*)obj)->priv->data.dtype);

        do_cpy(self->priv->data.dtype1, ((DocumentOrder*)obj)->priv->data.dtype1);

        do_cpy(self->priv->data.sklad, ((DocumentOrder*)obj)->priv->data.sklad);

        do_cpy(self->priv->data.document, ((DocumentOrder*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentOrderKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentOrderKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentOrderKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentOrderKey1*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentOrderKey1*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentOrderKey1*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentOrderKey2*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentOrderKey2*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentOrderKey2*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {

        do_cpy(self->priv->data.sklad, ((DocumentOrderKey4*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

        do_cpy(self->priv->data.dtype, ((DocumentData*)obj)->priv->data.dtype);

        do_cpy(self->priv->data.sklad, ((DocumentData*)obj)->priv->data.sklad);

        do_cpy(self->priv->data.document, ((DocumentData*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentDataKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentDataKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentDataKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentDataKey1*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentDataKey1*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentDataKey1*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewType() ) {

        do_cpy(self->priv->data.dtype, ((DocumentView*)obj)->priv->data.dtype);

        do_cpy(self->priv->data.sklad, ((DocumentView*)obj)->priv->data.sklad);

        do_cpy(self->priv->data.document, ((DocumentView*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentViewKey0*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentViewKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentViewKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {

        do_cpy(self->priv->data.dtype, ((DocumentViewKey1*)obj)->priv->dtype);

        do_cpy(self->priv->data.sklad, ((DocumentViewKey1*)obj)->priv->sklad);

        do_cpy(self->priv->data.document, ((DocumentViewKey1*)obj)->priv->document);

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
            if ( !strcmp("partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_document_date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_document_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_document_time", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_document_time", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("party", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_party", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("accepted", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_accepted", "O", value) )
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

            else
            if ( !strcmp("sum", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sum", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sums", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sums", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Document\"does not contain \"%s\"", field_name);
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

static PyObject *Document_fields(Document* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Document");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Document_document_type(self,NULL);
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

    value =  Document_document_operation(self,NULL);
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

    value =  Document_name(self,NULL);
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

    value =  Document_store(self,NULL);
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

    value =  Document_document(self,NULL);
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

    value =  Document_date(self,NULL);
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

    value =  Document_time(self,NULL);
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

    value =  Document_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Document_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Document_partner_document(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_document", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Document_partner_document_date(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_document_date", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Document_partner_document_time(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_document_time", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Document_party(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("party", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Document_accepted(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("accepted", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Document_get_params(self, NULL);
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

static PyGetSetDef Document_getset[] = {

    {"document_type",(getter)Document_document_type},

    {"document_operation",(getter)Document_document_operation},

    {"name",(getter)Document_name},

    {"store",(getter)Document_store},

    {"document",(getter)Document_document},

    {"date",(getter)Document_date},

    {"date_raw",(getter)Document_date_raw},

    {"time",(getter)Document_time},

    {"time_raw",(getter)Document_time_raw},

    {"partner_region_code",(getter)Document_partner_region_code},

    {"partner_code",(getter)Document_partner_code},

    {"partner_document",(getter)Document_partner_document},

    {"partner_document_date",(getter)Document_partner_document_date},

    {"partner_document_time",(getter)Document_partner_document_time},

    {"party",(getter)Document_party},

    {"accepted",(getter)Document_accepted},

    {"params",(getter)Document_get_params},

    {"sums",(getter)Document_get_sums},

    {NULL}
};

static PyMethodDef Document_methods[] = {
    {"set", (PyCFunction)Document_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Document_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Document_clone, METH_NOARGS, "clone"},

    {"clear_sums", (PyCFunction)Document_sums_clear, METH_NOARGS, "do_Document_sums_clear"},
    {"sum", (PyCFunction)Document_get_sum, METH_VARARGS|METH_KEYWORDS, "do_Document_sum"},
    {"set_sums", (PyCFunction)Document_set_sums, METH_VARARGS|METH_KEYWORDS, "do_Document_set_sums"},

    {"set_sum", (PyCFunction)Document_set_sum, METH_VARARGS|METH_KEYWORDS, "Document_set_sum"},

    {"set_document_type", (PyCFunction)Document_set_document_type, METH_VARARGS|METH_KEYWORDS, "Document_set_document_type"},

    {"set_document_operation", (PyCFunction)Document_set_document_operation, METH_VARARGS|METH_KEYWORDS, "Document_set_document_operation"},

    {"set_name", (PyCFunction)Document_set_name, METH_VARARGS|METH_KEYWORDS, "Document_set_name"},

    {"set_store", (PyCFunction)Document_set_store, METH_VARARGS|METH_KEYWORDS, "Document_set_store"},

    {"set_document", (PyCFunction)Document_set_document, METH_VARARGS|METH_KEYWORDS, "Document_set_document"},

    {"set_date", (PyCFunction)Document_set_date, METH_VARARGS|METH_KEYWORDS, "Document_set_date"},

    {"set_time", (PyCFunction)Document_set_time, METH_VARARGS|METH_KEYWORDS, "Document_set_time"},

    {"set_partner_region_code", (PyCFunction)Document_set_partner_region_code, METH_VARARGS|METH_KEYWORDS, "Document_set_partner_region_code"},

    {"set_partner_code", (PyCFunction)Document_set_partner_code, METH_VARARGS|METH_KEYWORDS, "Document_set_partner_code"},

    {"set_partner_document", (PyCFunction)Document_set_partner_document, METH_VARARGS|METH_KEYWORDS, "Document_set_partner_document"},

    {"set_partner_document_date", (PyCFunction)Document_set_partner_document_date, METH_VARARGS|METH_KEYWORDS, "Document_set_partner_document_date"},

    {"set_partner_document_time", (PyCFunction)Document_set_partner_document_time, METH_VARARGS|METH_KEYWORDS, "Document_set_partner_document_time"},

    {"set_party", (PyCFunction)Document_set_party, METH_VARARGS|METH_KEYWORDS, "Document_set_party"},

    {"set_accepted", (PyCFunction)Document_set_accepted, METH_VARARGS|METH_KEYWORDS, "Document_set_accepted"},

    {"set_param", (PyCFunction)Document_set_param, METH_VARARGS|METH_KEYWORDS, "do_Document_param_set"},
    {"param", (PyCFunction)Document_get_param, METH_VARARGS|METH_KEYWORDS, "do_Document_param"},
    {"clear_params", (PyCFunction)Document_params_clear, METH_NOARGS, "do_Document_param_clear"},
    {"set_params", (PyCFunction)Document_set_params, METH_VARARGS|METH_KEYWORDS, "do_Document_set_params"},

    {"get_equal", (PyCFunction)Document_equal, METH_VARARGS|METH_KEYWORDS, "Document_equal"},

    {"get_next", (PyCFunction)Document_next, METH_VARARGS|METH_KEYWORDS, "Document_next"},

    {"get_prev", (PyCFunction)Document_prev, METH_VARARGS|METH_KEYWORDS, "Document_prev"},

    {"get_gt", (PyCFunction)Document_gt, METH_VARARGS|METH_KEYWORDS, "Document_gt"},

    {"get_ge", (PyCFunction)Document_ge, METH_VARARGS|METH_KEYWORDS, "Document_ge"},

    {"get_lt", (PyCFunction)Document_lt, METH_VARARGS|METH_KEYWORDS, "Document_lt"},

    {"get_le", (PyCFunction)Document_le, METH_VARARGS|METH_KEYWORDS, "Document_le"},

    {"get_first", (PyCFunction)Document_first, METH_VARARGS|METH_KEYWORDS, "Document_first"},

    {"get_last", (PyCFunction)Document_last, METH_VARARGS|METH_KEYWORDS, "Document_last"},

    {"gets_equal", (PyCFunction)Document_iter_equal, METH_VARARGS|METH_KEYWORDS, "Document_iter_equal"},

    {"gets_gt", (PyCFunction)Document_iter_gt, METH_VARARGS|METH_KEYWORDS, "Document_iter_gt"},

    {"gets_ge", (PyCFunction)Document_iter_ge, METH_VARARGS|METH_KEYWORDS, "Document_iter_ge"},

    {"gets_lt", (PyCFunction)Document_iter_lt, METH_VARARGS|METH_KEYWORDS, "Document_iter_lt"},

    {"gets_le", (PyCFunction)Document_iter_le, METH_VARARGS|METH_KEYWORDS, "Document_iter_le"},

    {"gets_first", (PyCFunction)Document_iter_first, METH_VARARGS|METH_KEYWORDS, "Document_iter_first"},

    {"gets_last", (PyCFunction)Document_iter_last, METH_VARARGS|METH_KEYWORDS, "Document_iter_last"},

    {"insert", (PyCFunction)Document_insert, METH_VARARGS|METH_KEYWORDS, "Document_insert"},

    {"update", (PyCFunction)Document_update, METH_VARARGS|METH_KEYWORDS, "Document_update"},

    {"delete", (PyCFunction)Document_delete, METH_VARARGS|METH_KEYWORDS, "Document_delete"},

    {NULL}
};

static PyTypeObject DocumentType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Document",             /*tp_name*/
    sizeof(Document),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Document_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Document_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Document objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Document_methods,             /* tp_methods */
    0,
    Document_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Document_init,      /* tp_init */
    0,                         /* tp_alloc */
    Document_new,                 /* tp_new */
};
PyTypeObject *getDocumentType()
{
    return &DocumentType_;
}

static void DocumentKey0_dealloc(PyObject* self)
{
    do_free(((DocumentKey0*)self)->priv);
    Py_DECREF(((DocumentKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentKey0 free\n");
}
static PyObject *DocumentKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey0 new\n");
    DocumentKey0 *self;
    self = PyObject_NEW(DocumentKey0, type);
    //self = (DocumentKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_key0_t));
    return (PyObject *)self;
}
static int DocumentKey0_init(DocumentKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentKey0 init\n");
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

static PyObject *DocumentKey0_clone(DocumentKey0 *self)
{
    DocumentKey0 *result;
    result = (DocumentKey0*)PyObject_CallFunction((PyObject*)getDocumentKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentKey0_document_type(DocumentKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey0_set_document_type(DocumentKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey0_store(DocumentKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey0_set_store(DocumentKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey0_document(DocumentKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey0_set_document(DocumentKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey0_equal(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_next(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_prev(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_gt(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_ge(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_lt(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_le(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_first(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_last(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey0_iter_equal(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
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

 
        item = DocumentKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey0_iter_gt(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key0(self->alias->alias, self->priv, DO_GET_GT);
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

 
        item = DocumentKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey0_iter_ge(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key0(self->alias->alias, self->priv, DO_GET_GE);
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

 
        item = DocumentKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey0_iter_lt(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key0(self->alias->alias, self->priv, DO_GET_LT);
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

 
        item = DocumentKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey0_iter_le(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key0(self->alias->alias, self->priv, DO_GET_LE);
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

 
        item = DocumentKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey0_iter_first(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key0(self->alias->alias, self->priv, DO_GET_FIRST);
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

 
        item = DocumentKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey0_iter_last(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key0(self->alias->alias, self->priv, DO_GET_LAST);
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

 
        item = DocumentKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey0_cmp(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentKey0*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }
        
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentOrderKey0*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentOrderKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentOrderKey0*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }
        
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentOrderKey1*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentOrderKey1*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentOrderKey1*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }
        
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentOrderKey2*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentOrderKey2*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->document, ((DocumentOrderKey2*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentKey0_set_alias(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentKey0*)self)->alias);
    Py_INCREF(obj);
    ((DocumentKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentKey0_set(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentKey0Type() ) {
        memcpy(self->priv, ((DocumentKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((Document*)obj)->priv->data.document);

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

                default:
                    do_log(LOG_ERR, "%s size larger than the number of fields", PyList_Check(obj) ? "List":"Tuple");
                    return NULL;
            }
        }
    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->dtype, ((DocumentOrder*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentOrder*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentOrder*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentOrderKey0*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentOrderKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey1*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentOrderKey1*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentOrderKey1*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey2*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentOrderKey2*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentOrderKey2*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {

        do_cpy(self->priv->sklad, ((DocumentOrderKey4*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

        do_cpy(self->priv->dtype, ((DocumentData*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentData*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentData*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentDataKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentDataKey0*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentDataKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentDataKey1*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentDataKey1*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentDataKey1*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewType() ) {

        do_cpy(self->priv->dtype, ((DocumentView*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentView*)obj)->priv->data.sklad);

        do_cpy(self->priv->document, ((DocumentView*)obj)->priv->data.document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentViewKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentViewKey0*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentViewKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentViewKey1*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentViewKey1*)obj)->priv->sklad);

        do_cpy(self->priv->document, ((DocumentViewKey1*)obj)->priv->document);

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

            else {
                do_log(LOG_ERR, "Class \"DocumentKey0\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentKey0_fields(DocumentKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentKey0_document_type(self,NULL);
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

    value =  DocumentKey0_store(self,NULL);
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

    value =  DocumentKey0_document(self,NULL);
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

static PyGetSetDef DocumentKey0_getset[] = {

    {"document_type",(getter)DocumentKey0_document_type},

    {"store",(getter)DocumentKey0_store},

    {"document",(getter)DocumentKey0_document},

    {NULL}
};

static PyMethodDef DocumentKey0_methods[] = {
    {"set", (PyCFunction)DocumentKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentKey0_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_set_document_type"},

    {"set_store", (PyCFunction)DocumentKey0_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_set_store"},

    {"set_document", (PyCFunction)DocumentKey0_set_document, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_set_document"},

    {"get_equal", (PyCFunction)DocumentKey0_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_equal"},

    {"get_next", (PyCFunction)DocumentKey0_next, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_next"},

    {"get_prev", (PyCFunction)DocumentKey0_prev, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_prev"},

    {"get_gt", (PyCFunction)DocumentKey0_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_gt"},

    {"get_ge", (PyCFunction)DocumentKey0_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_ge"},

    {"get_lt", (PyCFunction)DocumentKey0_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_lt"},

    {"get_le", (PyCFunction)DocumentKey0_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_le"},

    {"get_first", (PyCFunction)DocumentKey0_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_first"},

    {"get_last", (PyCFunction)DocumentKey0_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_last"},

    {"gets_equal", (PyCFunction)DocumentKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_iter_lt"},

    {"gets_le", (PyCFunction)DocumentKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_iter_le"},

    {"gets_first", (PyCFunction)DocumentKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_iter_first"},

    {"gets_last", (PyCFunction)DocumentKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey0_iter_last"},

    {NULL}
};

static PyTypeObject DocumentKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentKey0",             /*tp_name*/
    sizeof(DocumentKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey0_methods,             /* tp_methods */
    0,
    DocumentKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey0_new,                 /* tp_new */
};
PyTypeObject *getDocumentKey0Type()
{
    return &DocumentKey0Type_;
}

static void DocumentKey1_dealloc(PyObject* self)
{
    do_free(((DocumentKey1*)self)->priv);
    Py_DECREF(((DocumentKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentKey1 free\n");
}
static PyObject *DocumentKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey1 new\n");
    DocumentKey1 *self;
    self = PyObject_NEW(DocumentKey1, type);
    //self = (DocumentKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_key1_t));
    return (PyObject *)self;
}
static int DocumentKey1_init(DocumentKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->dtype);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentKey1_clone(DocumentKey1 *self)
{
    DocumentKey1 *result;
    result = (DocumentKey1*)PyObject_CallFunction((PyObject*)getDocumentKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentKey1_document_type(DocumentKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey1_set_document_type(DocumentKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey1_date(DocumentKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *DocumentKey1_date_raw(DocumentKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *DocumentKey1_set_date(DocumentKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey1_time(DocumentKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *DocumentKey1_time_raw(DocumentKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *DocumentKey1_set_time(DocumentKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey1_equal(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_next(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_prev(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_gt(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_ge(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_lt(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_le(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_first(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_last(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey1_iter_equal(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
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

 
        item = DocumentKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey1_iter_gt(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
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

 
        item = DocumentKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey1_iter_ge(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
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

 
        item = DocumentKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey1_iter_lt(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
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

 
        item = DocumentKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey1_iter_le(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
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

 
        item = DocumentKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey1_iter_first(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
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

 
        item = DocumentKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey1_iter_last(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
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

 
        item = DocumentKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey1_cmp(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentKey1*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((DocumentKey1*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->time - ((DocumentKey1*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentKey1_set_alias(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentKey1*)self)->alias);
    Py_INCREF(obj);
    ((DocumentKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentKey1_set(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentKey1Type() ) {
        memcpy(self->priv, ((DocumentKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->date, ((Document*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Document*)obj)->priv->data.time);

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
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->dtype, ((DocumentOrder*)obj)->priv->data.dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey0*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey2*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

        do_cpy(self->priv->dtype, ((DocumentData*)obj)->priv->data.dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentDataKey0*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentDataKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewType() ) {

        do_cpy(self->priv->dtype, ((DocumentView*)obj)->priv->data.dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentViewKey0*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentViewKey1*)obj)->priv->dtype);

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
                do_log(LOG_ERR, "Class \"DocumentKey1\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentKey1_fields(DocumentKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentKey1_document_type(self,NULL);
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

    value =  DocumentKey1_date(self,NULL);
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

    value =  DocumentKey1_time(self,NULL);
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

static PyGetSetDef DocumentKey1_getset[] = {

    {"document_type",(getter)DocumentKey1_document_type},

    {"date",(getter)DocumentKey1_date},

    {"date_raw",(getter)DocumentKey1_date_raw},

    {"time",(getter)DocumentKey1_time},

    {"time_raw",(getter)DocumentKey1_time_raw},

    {NULL}
};

static PyMethodDef DocumentKey1_methods[] = {
    {"set", (PyCFunction)DocumentKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentKey1_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_set_document_type"},

    {"set_date", (PyCFunction)DocumentKey1_set_date, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_set_date"},

    {"set_time", (PyCFunction)DocumentKey1_set_time, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_set_time"},

    {"get_equal", (PyCFunction)DocumentKey1_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_equal"},

    {"get_next", (PyCFunction)DocumentKey1_next, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_next"},

    {"get_prev", (PyCFunction)DocumentKey1_prev, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_prev"},

    {"get_gt", (PyCFunction)DocumentKey1_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_gt"},

    {"get_ge", (PyCFunction)DocumentKey1_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_ge"},

    {"get_lt", (PyCFunction)DocumentKey1_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_lt"},

    {"get_le", (PyCFunction)DocumentKey1_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_le"},

    {"get_first", (PyCFunction)DocumentKey1_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_first"},

    {"get_last", (PyCFunction)DocumentKey1_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_last"},

    {"gets_equal", (PyCFunction)DocumentKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_iter_lt"},

    {"gets_le", (PyCFunction)DocumentKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_iter_le"},

    {"gets_first", (PyCFunction)DocumentKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_iter_first"},

    {"gets_last", (PyCFunction)DocumentKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey1_iter_last"},

    {NULL}
};

static PyTypeObject DocumentKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentKey1",             /*tp_name*/
    sizeof(DocumentKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey1_methods,             /* tp_methods */
    0,
    DocumentKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey1_new,                 /* tp_new */
};
PyTypeObject *getDocumentKey1Type()
{
    return &DocumentKey1Type_;
}

static void DocumentKey2_dealloc(PyObject* self)
{
    do_free(((DocumentKey2*)self)->priv);
    Py_DECREF(((DocumentKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentKey2 free\n");
}
static PyObject *DocumentKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey2 new\n");
    DocumentKey2 *self;
    self = PyObject_NEW(DocumentKey2, type);
    //self = (DocumentKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_key2_t));
    return (PyObject *)self;
}
static int DocumentKey2_init(DocumentKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentKey2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->p_g_code);

    do_text_set_empty(self->priv->party);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentKey2_clone(DocumentKey2 *self)
{
    DocumentKey2 *result;
    result = (DocumentKey2*)PyObject_CallFunction((PyObject*)getDocumentKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentKey2_partner_region_code(DocumentKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey2_set_partner_region_code(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentKey2_partner_code(DocumentKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->p_code);

    return result;
}

static PyObject *DocumentKey2_set_partner_code(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentKey2_party(DocumentKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->party);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey2_set_party(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set party");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->party, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->party);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentKey2_date(DocumentKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *DocumentKey2_date_raw(DocumentKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *DocumentKey2_set_date(DocumentKey2* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey2_time(DocumentKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *DocumentKey2_time_raw(DocumentKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *DocumentKey2_set_time(DocumentKey2* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey2_equal(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_next(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_prev(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_gt(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_ge(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_lt(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_le(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_first(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_last(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey2_iter_equal(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey2_iter_gt(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey2_iter_ge(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey2_iter_lt(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey2_iter_le(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey2_iter_first(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey2_iter_last(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.party, 
                 self->priv->party))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey2_cmp(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->p_g_code, ((DocumentKey2*)obj)->priv->p_g_code);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->p_code - ((DocumentKey2*)obj)->priv->p_code;
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->party, ((DocumentKey2*)obj)->priv->party);
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->date - ((DocumentKey2*)obj)->priv->date;
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->time - ((DocumentKey2*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentKey2_set_alias(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentKey2*)self)->alias);
    Py_INCREF(obj);
    ((DocumentKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentKey2_set(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentKey2Type() ) {
        memcpy(self->priv, ((DocumentKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->p_g_code, ((Document*)obj)->priv->data.p_g_code);

        do_cpy(self->priv->p_code, ((Document*)obj)->priv->data.p_code);

        do_cpy(self->priv->party, ((Document*)obj)->priv->data.party);

        do_cpy(self->priv->date, ((Document*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Document*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_partner_region_code", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_partner_code", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_party", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 4:
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
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewType() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {

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
            if ( !strcmp("partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("party", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_party", "O", value) )
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
                do_log(LOG_ERR, "Class \"DocumentKey2\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentKey2_fields(DocumentKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentKey2_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentKey2_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentKey2_party(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("party", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentKey2_date(self,NULL);
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

    value =  DocumentKey2_time(self,NULL);
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

static PyGetSetDef DocumentKey2_getset[] = {

    {"partner_region_code",(getter)DocumentKey2_partner_region_code},

    {"partner_code",(getter)DocumentKey2_partner_code},

    {"party",(getter)DocumentKey2_party},

    {"date",(getter)DocumentKey2_date},

    {"date_raw",(getter)DocumentKey2_date_raw},

    {"time",(getter)DocumentKey2_time},

    {"time_raw",(getter)DocumentKey2_time_raw},

    {NULL}
};

static PyMethodDef DocumentKey2_methods[] = {
    {"set", (PyCFunction)DocumentKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_partner_region_code", (PyCFunction)DocumentKey2_set_partner_region_code, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_set_partner_region_code"},

    {"set_partner_code", (PyCFunction)DocumentKey2_set_partner_code, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_set_partner_code"},

    {"set_party", (PyCFunction)DocumentKey2_set_party, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_set_party"},

    {"set_date", (PyCFunction)DocumentKey2_set_date, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_set_date"},

    {"set_time", (PyCFunction)DocumentKey2_set_time, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_set_time"},

    {"get_equal", (PyCFunction)DocumentKey2_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_equal"},

    {"get_next", (PyCFunction)DocumentKey2_next, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_next"},

    {"get_prev", (PyCFunction)DocumentKey2_prev, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_prev"},

    {"get_gt", (PyCFunction)DocumentKey2_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_gt"},

    {"get_ge", (PyCFunction)DocumentKey2_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_ge"},

    {"get_lt", (PyCFunction)DocumentKey2_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_lt"},

    {"get_le", (PyCFunction)DocumentKey2_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_le"},

    {"get_first", (PyCFunction)DocumentKey2_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_first"},

    {"get_last", (PyCFunction)DocumentKey2_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_last"},

    {"gets_equal", (PyCFunction)DocumentKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_iter_lt"},

    {"gets_le", (PyCFunction)DocumentKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_iter_le"},

    {"gets_first", (PyCFunction)DocumentKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_iter_first"},

    {"gets_last", (PyCFunction)DocumentKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey2_iter_last"},

    {NULL}
};

static PyTypeObject DocumentKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentKey2",             /*tp_name*/
    sizeof(DocumentKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey2_methods,             /* tp_methods */
    0,
    DocumentKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey2_new,                 /* tp_new */
};
PyTypeObject *getDocumentKey2Type()
{
    return &DocumentKey2Type_;
}

static void DocumentKey3_dealloc(PyObject* self)
{
    do_free(((DocumentKey3*)self)->priv);
    Py_DECREF(((DocumentKey3*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentKey3 free\n");
}
static PyObject *DocumentKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey3 new\n");
    DocumentKey3 *self;
    self = PyObject_NEW(DocumentKey3, type);
    //self = (DocumentKey3 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_key3_t));
    return (PyObject *)self;
}
static int DocumentKey3_init(DocumentKey3 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentKey3 init\n");
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

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentKey3_clone(DocumentKey3 *self)
{
    DocumentKey3 *result;
    result = (DocumentKey3*)PyObject_CallFunction((PyObject*)getDocumentKey3Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentKey3_document_type(DocumentKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey3_set_document_type(DocumentKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey3_store(DocumentKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey3_set_store(DocumentKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey3_date(DocumentKey3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *DocumentKey3_date_raw(DocumentKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *DocumentKey3_set_date(DocumentKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey3_time(DocumentKey3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *DocumentKey3_time_raw(DocumentKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *DocumentKey3_set_time(DocumentKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey3_equal(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_next(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_prev(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_gt(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_ge(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_lt(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_le(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_first(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_last(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key3(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey3_iter_equal(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key3(self->alias->alias, self->priv, DO_GET_EQUAL);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey3_iter_gt(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key3(self->alias->alias, self->priv, DO_GET_GT);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey3_iter_ge(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key3(self->alias->alias, self->priv, DO_GET_GE);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey3_iter_lt(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key3(self->alias->alias, self->priv, DO_GET_LT);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey3_iter_le(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key3(self->alias->alias, self->priv, DO_GET_LE);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey3_iter_first(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key3(self->alias->alias, self->priv, DO_GET_FIRST);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey3_iter_last(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key3(self->alias->alias, self->priv, DO_GET_LAST);
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
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey3_cmp(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentKey3*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->sklad, ((DocumentKey3*)obj)->priv->sklad);
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->date - ((DocumentKey3*)obj)->priv->date;
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->time - ((DocumentKey3*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentKey3_set_alias(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentKey3*)self)->alias);
    Py_INCREF(obj);
    ((DocumentKey3*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentKey3_set(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentKey3Type() ) {
        memcpy(self->priv, ((DocumentKey3*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((Document*)obj)->priv->data.sklad);

        do_cpy(self->priv->date, ((Document*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Document*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 3:
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
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->dtype, ((DocumentOrder*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentOrder*)obj)->priv->data.sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentOrderKey0*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey1*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentOrderKey1*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey2*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentOrderKey2*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {

        do_cpy(self->priv->sklad, ((DocumentOrderKey4*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

        do_cpy(self->priv->dtype, ((DocumentData*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentData*)obj)->priv->data.sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentDataKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentDataKey0*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentDataKey1*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentDataKey1*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewType() ) {

        do_cpy(self->priv->dtype, ((DocumentView*)obj)->priv->data.dtype);

        do_cpy(self->priv->sklad, ((DocumentView*)obj)->priv->data.sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentViewKey0*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentViewKey0*)obj)->priv->sklad);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentViewKey1*)obj)->priv->dtype);

        do_cpy(self->priv->sklad, ((DocumentViewKey1*)obj)->priv->sklad);

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
                do_log(LOG_ERR, "Class \"DocumentKey3\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentKey3_fields(DocumentKey3* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentKey3");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentKey3_document_type(self,NULL);
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

    value =  DocumentKey3_store(self,NULL);
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

    value =  DocumentKey3_date(self,NULL);
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

    value =  DocumentKey3_time(self,NULL);
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

static PyGetSetDef DocumentKey3_getset[] = {

    {"document_type",(getter)DocumentKey3_document_type},

    {"store",(getter)DocumentKey3_store},

    {"date",(getter)DocumentKey3_date},

    {"date_raw",(getter)DocumentKey3_date_raw},

    {"time",(getter)DocumentKey3_time},

    {"time_raw",(getter)DocumentKey3_time_raw},

    {NULL}
};

static PyMethodDef DocumentKey3_methods[] = {
    {"set", (PyCFunction)DocumentKey3_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentKey3_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentKey3_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentKey3_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentKey3_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_set_document_type"},

    {"set_store", (PyCFunction)DocumentKey3_set_store, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_set_store"},

    {"set_date", (PyCFunction)DocumentKey3_set_date, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_set_date"},

    {"set_time", (PyCFunction)DocumentKey3_set_time, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_set_time"},

    {"get_equal", (PyCFunction)DocumentKey3_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_equal"},

    {"get_next", (PyCFunction)DocumentKey3_next, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_next"},

    {"get_prev", (PyCFunction)DocumentKey3_prev, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_prev"},

    {"get_gt", (PyCFunction)DocumentKey3_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_gt"},

    {"get_ge", (PyCFunction)DocumentKey3_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_ge"},

    {"get_lt", (PyCFunction)DocumentKey3_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_lt"},

    {"get_le", (PyCFunction)DocumentKey3_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_le"},

    {"get_first", (PyCFunction)DocumentKey3_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_first"},

    {"get_last", (PyCFunction)DocumentKey3_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_last"},

    {"gets_equal", (PyCFunction)DocumentKey3_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentKey3_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentKey3_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentKey3_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_iter_lt"},

    {"gets_le", (PyCFunction)DocumentKey3_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_iter_le"},

    {"gets_first", (PyCFunction)DocumentKey3_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_iter_first"},

    {"gets_last", (PyCFunction)DocumentKey3_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey3_iter_last"},

    {NULL}
};

static PyTypeObject DocumentKey3Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentKey3",             /*tp_name*/
    sizeof(DocumentKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentKey3_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey3_methods,             /* tp_methods */
    0,
    DocumentKey3_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey3_new,                 /* tp_new */
};
PyTypeObject *getDocumentKey3Type()
{
    return &DocumentKey3Type_;
}

static void DocumentKey4_dealloc(PyObject* self)
{
    do_free(((DocumentKey4*)self)->priv);
    Py_DECREF(((DocumentKey4*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("DocumentKey4 free\n");
}
static PyObject *DocumentKey4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("DocumentKey4 new\n");
    DocumentKey4 *self;
    self = PyObject_NEW(DocumentKey4, type);
    //self = (DocumentKey4 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(document_key4_t));
    return (PyObject *)self;
}
static int DocumentKey4_init(DocumentKey4 *self, PyObject *args, PyObject *kwds)
{
    //LOG("DocumentKey4 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->dtype);

    do_text_set_empty(self->priv->p_g_code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *DocumentKey4_clone(DocumentKey4 *self)
{
    DocumentKey4 *result;
    result = (DocumentKey4*)PyObject_CallFunction((PyObject*)getDocumentKey4Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *DocumentKey4_document_type(DocumentKey4* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->dtype);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey4_set_document_type(DocumentKey4* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey4_partner_region_code(DocumentKey4* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *DocumentKey4_set_partner_region_code(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentKey4_partner_code(DocumentKey4* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->p_code);

    return result;
}

static PyObject *DocumentKey4_set_partner_code(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *DocumentKey4_date(DocumentKey4* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *DocumentKey4_date_raw(DocumentKey4* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *DocumentKey4_set_date(DocumentKey4* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey4_time(DocumentKey4* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_time(self->priv->time, &tm);
    result = PyTime_FromTime(tm.tm_hour,tm.tm_min,tm.tm_sec,0);

    return result;
}

static PyObject *DocumentKey4_time_raw(DocumentKey4* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->time);

    return result;
}

static PyObject *DocumentKey4_set_time(DocumentKey4* self, PyObject *args, PyObject *kwds)
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

static PyObject *DocumentKey4_equal(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_next(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_prev(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_gt(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_ge(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_lt(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_le(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_first(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_last(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_document_key4(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *DocumentKey4_iter_equal(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key4(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey4_iter_gt(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key4(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey4_iter_ge(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key4(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey4_iter_lt(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key4(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey4_iter_le(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key4(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey4_iter_first(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key4(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey4_iter_last(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    document_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_document_key4(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.dtype, 
                 self->priv->dtype))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.p_g_code, 
                 self->priv->p_g_code))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.p_code, 
                 self->priv->p_code))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.time, 
                 self->priv->time))
               break;
       }

 
        item = DocumentKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_document_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *DocumentKey4_cmp(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->dtype, ((DocumentKey4*)obj)->priv->dtype);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->p_g_code, ((DocumentKey4*)obj)->priv->p_g_code);
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->p_code - ((DocumentKey4*)obj)->priv->p_code;
       }

       if ( !res && depth >= 4 ) {
           res = self->priv->date - ((DocumentKey4*)obj)->priv->date;
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->time - ((DocumentKey4*)obj)->priv->time;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *DocumentKey4_set_alias(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((DocumentKey4*)self)->alias);
    Py_INCREF(obj);
    ((DocumentKey4*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *DocumentKey4_set(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getDocumentKey4Type() ) {
        memcpy(self->priv, ((DocumentKey4*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getDocumentType() ) {

        do_cpy(self->priv->dtype, ((Document*)obj)->priv->data.dtype);

        do_cpy(self->priv->p_g_code, ((Document*)obj)->priv->data.p_g_code);

        do_cpy(self->priv->p_code, ((Document*)obj)->priv->data.p_code);

        do_cpy(self->priv->date, ((Document*)obj)->priv->data.date);

        do_cpy(self->priv->time, ((Document*)obj)->priv->data.time);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_partner_region_code", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_partner_code", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 4:
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
    if ( Py_TYPE(obj) == getDocumentOrderType() ) {

        do_cpy(self->priv->dtype, ((DocumentOrder*)obj)->priv->data.dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey0*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey2Type() ) {

        do_cpy(self->priv->dtype, ((DocumentOrderKey2*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey3Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentOrderKey4Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataType() ) {

        do_cpy(self->priv->dtype, ((DocumentData*)obj)->priv->data.dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentDataKey0*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentDataKey1*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentDataKey2Type() ) {

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewType() ) {

        do_cpy(self->priv->dtype, ((DocumentView*)obj)->priv->data.dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey0Type() ) {

        do_cpy(self->priv->dtype, ((DocumentViewKey0*)obj)->priv->dtype);

    }

    else 
    if ( Py_TYPE(obj) == getDocumentViewKey1Type() ) {

        do_cpy(self->priv->dtype, ((DocumentViewKey1*)obj)->priv->dtype);

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
            if ( !strcmp("partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_partner_code", "O", value) )
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
                do_log(LOG_ERR, "Class \"DocumentKey4\"does not contain \"%s\"", field_name);
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

static PyObject *DocumentKey4_fields(DocumentKey4* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class DocumentKey4");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  DocumentKey4_document_type(self,NULL);
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

    value =  DocumentKey4_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentKey4_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  DocumentKey4_date(self,NULL);
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

    value =  DocumentKey4_time(self,NULL);
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

static PyGetSetDef DocumentKey4_getset[] = {

    {"document_type",(getter)DocumentKey4_document_type},

    {"partner_region_code",(getter)DocumentKey4_partner_region_code},

    {"partner_code",(getter)DocumentKey4_partner_code},

    {"date",(getter)DocumentKey4_date},

    {"date_raw",(getter)DocumentKey4_date_raw},

    {"time",(getter)DocumentKey4_time},

    {"time_raw",(getter)DocumentKey4_time_raw},

    {NULL}
};

static PyMethodDef DocumentKey4_methods[] = {
    {"set", (PyCFunction)DocumentKey4_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)DocumentKey4_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)DocumentKey4_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)DocumentKey4_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_document_type", (PyCFunction)DocumentKey4_set_document_type, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_set_document_type"},

    {"set_partner_region_code", (PyCFunction)DocumentKey4_set_partner_region_code, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_set_partner_region_code"},

    {"set_partner_code", (PyCFunction)DocumentKey4_set_partner_code, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_set_partner_code"},

    {"set_date", (PyCFunction)DocumentKey4_set_date, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_set_date"},

    {"set_time", (PyCFunction)DocumentKey4_set_time, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_set_time"},

    {"get_equal", (PyCFunction)DocumentKey4_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_equal"},

    {"get_next", (PyCFunction)DocumentKey4_next, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_next"},

    {"get_prev", (PyCFunction)DocumentKey4_prev, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_prev"},

    {"get_gt", (PyCFunction)DocumentKey4_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_gt"},

    {"get_ge", (PyCFunction)DocumentKey4_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_ge"},

    {"get_lt", (PyCFunction)DocumentKey4_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_lt"},

    {"get_le", (PyCFunction)DocumentKey4_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_le"},

    {"get_first", (PyCFunction)DocumentKey4_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_first"},

    {"get_last", (PyCFunction)DocumentKey4_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_last"},

    {"gets_equal", (PyCFunction)DocumentKey4_iter_equal, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_iter_equal"},

    {"gets_gt", (PyCFunction)DocumentKey4_iter_gt, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_iter_gt"},

    {"gets_ge", (PyCFunction)DocumentKey4_iter_ge, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_iter_ge"},

    {"gets_lt", (PyCFunction)DocumentKey4_iter_lt, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_iter_lt"},

    {"gets_le", (PyCFunction)DocumentKey4_iter_le, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_iter_le"},

    {"gets_first", (PyCFunction)DocumentKey4_iter_first, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_iter_first"},

    {"gets_last", (PyCFunction)DocumentKey4_iter_last, METH_VARARGS|METH_KEYWORDS, "DocumentKey4_iter_last"},

    {NULL}
};

static PyTypeObject DocumentKey4Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.DocumentKey4",             /*tp_name*/
    sizeof(DocumentKey4),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)DocumentKey4_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)DocumentKey4_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "DocumentKey4 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    DocumentKey4_methods,             /* tp_methods */
    0,
    DocumentKey4_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)DocumentKey4_init,      /* tp_init */
    0,                         /* tp_alloc */
    DocumentKey4_new,                 /* tp_new */
};
PyTypeObject *getDocumentKey4Type()
{
    return &DocumentKey4Type_;
}
