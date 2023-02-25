
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "prowod.h"

static void Prowod_dealloc(PyObject* self)
{
    do_free(((Prowod*)self)->priv);
    Py_DECREF(((Prowod*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Prowod free\n");
}
static PyObject *Prowod_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Prowod new\n");
    Prowod *self;
    self = PyObject_NEW(Prowod, type);
    //self = (Prowod *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod_rec_t));
    return (PyObject *)self;
}
static int Prowod_init(Prowod *self, PyObject *args, PyObject *kwds)
{
    //LOG("Prowod init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.division);

    do_text_set_empty(self->priv->data.debit_account);

    do_text_set_empty(self->priv->data.debit_subaccount);

    do_text_set_empty(self->priv->data.debit_other);

    do_text_set_empty(self->priv->data.debit_p_g_code);

    do_text_set_empty(self->priv->data.credit_account);

    do_text_set_empty(self->priv->data.credit_subaccount);

    do_text_set_empty(self->priv->data.credit_other);

    do_text_set_empty(self->priv->data.credit_p_g_code);

    do_text_set_empty(self->priv->data.valuta);

    do_text_set_empty(self->priv->data.type_doc);

    do_text_set_empty(self->priv->data.oper);

    do_text_set_empty(self->priv->data.document);

    do_text_set_empty(self->priv->data.pach);

    do_prowod_params_clear(self->alias->alias, self->priv);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Prowod_clone(Prowod *self)
{
    Prowod *result;
    result = (Prowod*)PyObject_CallFunction((PyObject*)getProwodType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Prowod_division(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.division);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_division(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.division");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.division, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.division);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_date(Prowod* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->data.date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *Prowod_date_raw(Prowod* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.date);

    return result;
}

static PyObject *Prowod_set_date(Prowod* self, PyObject *args, PyObject *kwds)
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

static PyObject *Prowod_debit_account(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.debit_account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_debit_account(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit_account");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.debit_account, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.debit_account);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_debit_subaccount(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.debit_subaccount);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_debit_subaccount(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit_subaccount");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.debit_subaccount, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.debit_subaccount);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_debit_other(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.debit_other);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_debit_other(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit_other");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.debit_other, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.debit_other);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_debit_partner_region_code(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.debit_p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_debit_partner_region_code(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit_p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.debit_p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.debit_p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_debit_partner_code(Prowod* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.debit_p_code);

    return result;
}

static PyObject *Prowod_set_debit_partner_code(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit_p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.debit_p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.debit_p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit_p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.debit_p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_credit_account(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.credit_account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_credit_account(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit_account");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.credit_account, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.credit_account);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_credit_subaccount(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.credit_subaccount);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_credit_subaccount(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit_subaccount");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.credit_subaccount, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.credit_subaccount);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_credit_other(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.credit_other);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_credit_other(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit_other");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.credit_other, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.credit_other);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_credit_partner_region_code(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.credit_p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_credit_partner_region_code(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit_p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.credit_p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.credit_p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_credit_partner_code(Prowod* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.credit_p_code);

    return result;
}

static PyObject *Prowod_set_credit_partner_code(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit_p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.credit_p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.credit_p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit_p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.credit_p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_valuta(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.valuta);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_valuta(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.valuta");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.valuta, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.valuta);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_get_sum(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get data.sums");
        return NULL;
    }
    result=PyFloat_FromDouble(do_pdecimal(self->priv->data.sums[number], 8, 2));

    return result;
}

static PyObject *Prowod_set_sum(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set data.sums");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_pdecimal_set(self->priv->data.sums[number], 8, 2,  MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_pdecimal_set(self->priv->data.sums[number], 8, 2,  PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sums");
            return NULL;
        }    
    }
    //result=PyFloat_FromDouble(do_pdecimal(self->priv->data.sums[number], 8, 2));

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_document_type(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.type_doc);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_document_type(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.type_doc");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.type_doc, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.type_doc);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_operation_type(Prowod* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.type_oper);

    return result;
}

static PyObject *Prowod_set_operation_type(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.type_oper");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.type_oper = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.type_oper = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.type_oper");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.type_oper);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_operation(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.oper);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_operation(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.oper");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.oper, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.oper);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_document(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_document(Prowod* self, PyObject *args, PyObject *kwds)
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

static PyObject *Prowod_pach(Prowod* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.pach);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Prowod_set_pach(Prowod* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.pach");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.pach, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.pach);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Prowod_set_param(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    char *name;
    static char *kwlist[] = {"name", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "sO|", kwlist, &name, &value) )
        return NULL;
    if ( MyString_Check(value) ) {
        char *val;
        val = MyString_AsString(value);
        do_prowod_param_set(self->alias->alias,self->priv, name, val);
        do_free(val);
    }
    else
    if ( MyLong_Check(value) )
        do_prowod_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(value));
    else
    if ( PyFloat_Check(value) )
        do_prowod_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(value));
    else {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}
static PyObject *Prowod_set_params(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_prowod_params_clear(self->alias->alias,self->priv);
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
                 do_prowod_param_set(self->alias->alias,self->priv, name, val);
                 do_free(val);
             }
             else
             if ( MyLong_Check(pValue) )
                 do_prowod_param_int_set(self->alias->alias,self->priv, name, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_prowod_param_float_set(self->alias->alias,self->priv, name, PyFloat_AsDouble(pValue));
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
static PyObject *Prowod_get_param(Prowod* self, PyObject *args, PyObject *kwds)
{
    char *name;
    static char *kwlist[] = {"name", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s|", kwlist, &name) )
        return NULL;
    char *buf;
    buf = do_prowod_param(self->alias->alias,self->priv, name);
    PyObject *result;
    result = MyString_FromString(buf);
    do_free(buf);
    return result;
}

static PyObject *Prowod_params_clear(Prowod* self)
{
    do_prowod_params_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Prowod_get_params(Prowod* self, void *unused)
{   
    PyObject *res;
    char *params = do_alias_text_(self->alias->alias, self->priv->data.params, self->priv->size - (sizeof(self->priv->data) - sizeof(self->priv->data.params)));
   
    res = do_getparams(params);
    do_free(params);
    return res;
}


static PyObject *Prowod_sums_clear(Prowod* self)
{
    do_prowod_sum_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Prowod_get_sums(Prowod* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 3; i++ ) {
        number = i;
        quant = do_prowod_sum(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *Prowod_set_sums(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_prowod_sum_clear(self->alias->alias,self->priv);
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
             if ( number > 2 ) continue;
             pValue = PyDict_GetItem(value, pKey);
             if ( !pValue ) {
                do_log(LOG_ERR, "Invalid value");
                break;
             }
             if ( MyLong_Check(pValue) )
                 do_prowod_sum_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_prowod_sum_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
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

static PyObject *Prowod_equal(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_next(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_prev(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_gt(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_ge(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_lt(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_le(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_first(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_last(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getProwodKey0Type() )
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProwodKey1Type() )
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProwodKey2Type() )
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProwodKey3Type() )
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getProwodKey4Type() )
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_iter_equal(Prowod* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwodKey0Type() ) {
        key_cmp = (prowod_key0_t*)do_malloc(sizeof(prowod_key0_t));
        memcpy(key_cmp, ((ProwodKey0*)key)->priv, sizeof(prowod_key0_t));
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProwodKey1Type() ) {
        key_cmp = (prowod_key1_t*)do_malloc(sizeof(prowod_key1_t));
        memcpy(key_cmp, ((ProwodKey1*)key)->priv, sizeof(prowod_key1_t));
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProwodKey2Type() ) {
        key_cmp = (prowod_key2_t*)do_malloc(sizeof(prowod_key2_t));
        memcpy(key_cmp, ((ProwodKey2*)key)->priv, sizeof(prowod_key2_t));
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProwodKey3Type() ) {
        key_cmp = (prowod_key3_t*)do_malloc(sizeof(prowod_key3_t));
        memcpy(key_cmp, ((ProwodKey3*)key)->priv, sizeof(prowod_key3_t));
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getProwodKey4Type() ) {
        key_cmp = (prowod_key4_t*)do_malloc(sizeof(prowod_key4_t));
        memcpy(key_cmp, ((ProwodKey4*)key)->priv, sizeof(prowod_key4_t));
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->division, 
                    ((ProwodKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->date, 
                    ((ProwodKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->type_doc, 
                    ((ProwodKey0*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->document, 
                    ((ProwodKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->division, 
                    ((ProwodKey1*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->type_doc, 
                    ((ProwodKey1*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->date, 
                    ((ProwodKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->document, 
                    ((ProwodKey1*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->division, 
                    ((ProwodKey2*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_account, 
                    ((ProwodKey2*)key)->priv->debit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_subaccount, 
                    ((ProwodKey2*)key)->priv->debit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_g_code, 
                    ((ProwodKey2*)key)->priv->debit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_code, 
                    ((ProwodKey2*)key)->priv->debit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_other, 
                    ((ProwodKey2*)key)->priv->debit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->date, 
                    ((ProwodKey2*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->division, 
                    ((ProwodKey3*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_account, 
                    ((ProwodKey3*)key)->priv->credit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_subaccount, 
                    ((ProwodKey3*)key)->priv->credit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_g_code, 
                    ((ProwodKey3*)key)->priv->credit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_code, 
                    ((ProwodKey3*)key)->priv->credit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_other, 
                    ((ProwodKey3*)key)->priv->credit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->date, 
                    ((ProwodKey3*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->division, 
                    ((ProwodKey4*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->document, 
                    ((ProwodKey4*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->date, 
                    ((ProwodKey4*)key)->priv->date))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
            status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
            status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
            status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
            status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
            status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Prowod_iter_gt(Prowod* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwodKey0Type() ) {
        key_cmp = (prowod_key0_t*)do_malloc(sizeof(prowod_key0_t));
        memcpy(key_cmp, ((ProwodKey0*)key)->priv, sizeof(prowod_key0_t));
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProwodKey1Type() ) {
        key_cmp = (prowod_key1_t*)do_malloc(sizeof(prowod_key1_t));
        memcpy(key_cmp, ((ProwodKey1*)key)->priv, sizeof(prowod_key1_t));
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProwodKey2Type() ) {
        key_cmp = (prowod_key2_t*)do_malloc(sizeof(prowod_key2_t));
        memcpy(key_cmp, ((ProwodKey2*)key)->priv, sizeof(prowod_key2_t));
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProwodKey3Type() ) {
        key_cmp = (prowod_key3_t*)do_malloc(sizeof(prowod_key3_t));
        memcpy(key_cmp, ((ProwodKey3*)key)->priv, sizeof(prowod_key3_t));
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getProwodKey4Type() ) {
        key_cmp = (prowod_key4_t*)do_malloc(sizeof(prowod_key4_t));
        memcpy(key_cmp, ((ProwodKey4*)key)->priv, sizeof(prowod_key4_t));
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->division, 
                    ((ProwodKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->date, 
                    ((ProwodKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->type_doc, 
                    ((ProwodKey0*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->document, 
                    ((ProwodKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->division, 
                    ((ProwodKey1*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->type_doc, 
                    ((ProwodKey1*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->date, 
                    ((ProwodKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->document, 
                    ((ProwodKey1*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->division, 
                    ((ProwodKey2*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_account, 
                    ((ProwodKey2*)key)->priv->debit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_subaccount, 
                    ((ProwodKey2*)key)->priv->debit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_g_code, 
                    ((ProwodKey2*)key)->priv->debit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_code, 
                    ((ProwodKey2*)key)->priv->debit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_other, 
                    ((ProwodKey2*)key)->priv->debit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->date, 
                    ((ProwodKey2*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->division, 
                    ((ProwodKey3*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_account, 
                    ((ProwodKey3*)key)->priv->credit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_subaccount, 
                    ((ProwodKey3*)key)->priv->credit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_g_code, 
                    ((ProwodKey3*)key)->priv->credit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_code, 
                    ((ProwodKey3*)key)->priv->credit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_other, 
                    ((ProwodKey3*)key)->priv->credit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->date, 
                    ((ProwodKey3*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->division, 
                    ((ProwodKey4*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->document, 
                    ((ProwodKey4*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->date, 
                    ((ProwodKey4*)key)->priv->date))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
            status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
            status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
            status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
            status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
            status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Prowod_iter_ge(Prowod* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwodKey0Type() ) {
        key_cmp = (prowod_key0_t*)do_malloc(sizeof(prowod_key0_t));
        memcpy(key_cmp, ((ProwodKey0*)key)->priv, sizeof(prowod_key0_t));
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProwodKey1Type() ) {
        key_cmp = (prowod_key1_t*)do_malloc(sizeof(prowod_key1_t));
        memcpy(key_cmp, ((ProwodKey1*)key)->priv, sizeof(prowod_key1_t));
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProwodKey2Type() ) {
        key_cmp = (prowod_key2_t*)do_malloc(sizeof(prowod_key2_t));
        memcpy(key_cmp, ((ProwodKey2*)key)->priv, sizeof(prowod_key2_t));
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProwodKey3Type() ) {
        key_cmp = (prowod_key3_t*)do_malloc(sizeof(prowod_key3_t));
        memcpy(key_cmp, ((ProwodKey3*)key)->priv, sizeof(prowod_key3_t));
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getProwodKey4Type() ) {
        key_cmp = (prowod_key4_t*)do_malloc(sizeof(prowod_key4_t));
        memcpy(key_cmp, ((ProwodKey4*)key)->priv, sizeof(prowod_key4_t));
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->division, 
                    ((ProwodKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->date, 
                    ((ProwodKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->type_doc, 
                    ((ProwodKey0*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->document, 
                    ((ProwodKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->division, 
                    ((ProwodKey1*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->type_doc, 
                    ((ProwodKey1*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->date, 
                    ((ProwodKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->document, 
                    ((ProwodKey1*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->division, 
                    ((ProwodKey2*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_account, 
                    ((ProwodKey2*)key)->priv->debit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_subaccount, 
                    ((ProwodKey2*)key)->priv->debit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_g_code, 
                    ((ProwodKey2*)key)->priv->debit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_code, 
                    ((ProwodKey2*)key)->priv->debit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_other, 
                    ((ProwodKey2*)key)->priv->debit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->date, 
                    ((ProwodKey2*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->division, 
                    ((ProwodKey3*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_account, 
                    ((ProwodKey3*)key)->priv->credit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_subaccount, 
                    ((ProwodKey3*)key)->priv->credit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_g_code, 
                    ((ProwodKey3*)key)->priv->credit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_code, 
                    ((ProwodKey3*)key)->priv->credit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_other, 
                    ((ProwodKey3*)key)->priv->credit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->date, 
                    ((ProwodKey3*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->division, 
                    ((ProwodKey4*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->document, 
                    ((ProwodKey4*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->date, 
                    ((ProwodKey4*)key)->priv->date))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
            status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
            status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
            status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
            status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
            status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Prowod_iter_lt(Prowod* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwodKey0Type() ) {
        key_cmp = (prowod_key0_t*)do_malloc(sizeof(prowod_key0_t));
        memcpy(key_cmp, ((ProwodKey0*)key)->priv, sizeof(prowod_key0_t));
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProwodKey1Type() ) {
        key_cmp = (prowod_key1_t*)do_malloc(sizeof(prowod_key1_t));
        memcpy(key_cmp, ((ProwodKey1*)key)->priv, sizeof(prowod_key1_t));
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProwodKey2Type() ) {
        key_cmp = (prowod_key2_t*)do_malloc(sizeof(prowod_key2_t));
        memcpy(key_cmp, ((ProwodKey2*)key)->priv, sizeof(prowod_key2_t));
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProwodKey3Type() ) {
        key_cmp = (prowod_key3_t*)do_malloc(sizeof(prowod_key3_t));
        memcpy(key_cmp, ((ProwodKey3*)key)->priv, sizeof(prowod_key3_t));
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getProwodKey4Type() ) {
        key_cmp = (prowod_key4_t*)do_malloc(sizeof(prowod_key4_t));
        memcpy(key_cmp, ((ProwodKey4*)key)->priv, sizeof(prowod_key4_t));
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->division, 
                    ((ProwodKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->date, 
                    ((ProwodKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->type_doc, 
                    ((ProwodKey0*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->document, 
                    ((ProwodKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->division, 
                    ((ProwodKey1*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->type_doc, 
                    ((ProwodKey1*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->date, 
                    ((ProwodKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->document, 
                    ((ProwodKey1*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->division, 
                    ((ProwodKey2*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_account, 
                    ((ProwodKey2*)key)->priv->debit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_subaccount, 
                    ((ProwodKey2*)key)->priv->debit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_g_code, 
                    ((ProwodKey2*)key)->priv->debit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_code, 
                    ((ProwodKey2*)key)->priv->debit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_other, 
                    ((ProwodKey2*)key)->priv->debit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->date, 
                    ((ProwodKey2*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->division, 
                    ((ProwodKey3*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_account, 
                    ((ProwodKey3*)key)->priv->credit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_subaccount, 
                    ((ProwodKey3*)key)->priv->credit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_g_code, 
                    ((ProwodKey3*)key)->priv->credit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_code, 
                    ((ProwodKey3*)key)->priv->credit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_other, 
                    ((ProwodKey3*)key)->priv->credit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->date, 
                    ((ProwodKey3*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->division, 
                    ((ProwodKey4*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->document, 
                    ((ProwodKey4*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->date, 
                    ((ProwodKey4*)key)->priv->date))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
            status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
            status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
            status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
            status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
            status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Prowod_iter_le(Prowod* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwodKey0Type() ) {
        key_cmp = (prowod_key0_t*)do_malloc(sizeof(prowod_key0_t));
        memcpy(key_cmp, ((ProwodKey0*)key)->priv, sizeof(prowod_key0_t));
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProwodKey1Type() ) {
        key_cmp = (prowod_key1_t*)do_malloc(sizeof(prowod_key1_t));
        memcpy(key_cmp, ((ProwodKey1*)key)->priv, sizeof(prowod_key1_t));
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProwodKey2Type() ) {
        key_cmp = (prowod_key2_t*)do_malloc(sizeof(prowod_key2_t));
        memcpy(key_cmp, ((ProwodKey2*)key)->priv, sizeof(prowod_key2_t));
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProwodKey3Type() ) {
        key_cmp = (prowod_key3_t*)do_malloc(sizeof(prowod_key3_t));
        memcpy(key_cmp, ((ProwodKey3*)key)->priv, sizeof(prowod_key3_t));
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getProwodKey4Type() ) {
        key_cmp = (prowod_key4_t*)do_malloc(sizeof(prowod_key4_t));
        memcpy(key_cmp, ((ProwodKey4*)key)->priv, sizeof(prowod_key4_t));
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->division, 
                    ((ProwodKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->date, 
                    ((ProwodKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->type_doc, 
                    ((ProwodKey0*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->document, 
                    ((ProwodKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->division, 
                    ((ProwodKey1*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->type_doc, 
                    ((ProwodKey1*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->date, 
                    ((ProwodKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->document, 
                    ((ProwodKey1*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->division, 
                    ((ProwodKey2*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_account, 
                    ((ProwodKey2*)key)->priv->debit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_subaccount, 
                    ((ProwodKey2*)key)->priv->debit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_g_code, 
                    ((ProwodKey2*)key)->priv->debit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_code, 
                    ((ProwodKey2*)key)->priv->debit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_other, 
                    ((ProwodKey2*)key)->priv->debit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->date, 
                    ((ProwodKey2*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->division, 
                    ((ProwodKey3*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_account, 
                    ((ProwodKey3*)key)->priv->credit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_subaccount, 
                    ((ProwodKey3*)key)->priv->credit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_g_code, 
                    ((ProwodKey3*)key)->priv->credit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_code, 
                    ((ProwodKey3*)key)->priv->credit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_other, 
                    ((ProwodKey3*)key)->priv->credit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->date, 
                    ((ProwodKey3*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->division, 
                    ((ProwodKey4*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->document, 
                    ((ProwodKey4*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->date, 
                    ((ProwodKey4*)key)->priv->date))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
            status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
            status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
            status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
            status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
            status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Prowod_iter_first(Prowod* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwodKey0Type() ) {
        key_cmp = (prowod_key0_t*)do_malloc(sizeof(prowod_key0_t));
        memcpy(key_cmp, ((ProwodKey0*)key)->priv, sizeof(prowod_key0_t));
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProwodKey1Type() ) {
        key_cmp = (prowod_key1_t*)do_malloc(sizeof(prowod_key1_t));
        memcpy(key_cmp, ((ProwodKey1*)key)->priv, sizeof(prowod_key1_t));
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProwodKey2Type() ) {
        key_cmp = (prowod_key2_t*)do_malloc(sizeof(prowod_key2_t));
        memcpy(key_cmp, ((ProwodKey2*)key)->priv, sizeof(prowod_key2_t));
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProwodKey3Type() ) {
        key_cmp = (prowod_key3_t*)do_malloc(sizeof(prowod_key3_t));
        memcpy(key_cmp, ((ProwodKey3*)key)->priv, sizeof(prowod_key3_t));
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getProwodKey4Type() ) {
        key_cmp = (prowod_key4_t*)do_malloc(sizeof(prowod_key4_t));
        memcpy(key_cmp, ((ProwodKey4*)key)->priv, sizeof(prowod_key4_t));
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->division, 
                    ((ProwodKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->date, 
                    ((ProwodKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->type_doc, 
                    ((ProwodKey0*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->document, 
                    ((ProwodKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->division, 
                    ((ProwodKey1*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->type_doc, 
                    ((ProwodKey1*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->date, 
                    ((ProwodKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->document, 
                    ((ProwodKey1*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->division, 
                    ((ProwodKey2*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_account, 
                    ((ProwodKey2*)key)->priv->debit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_subaccount, 
                    ((ProwodKey2*)key)->priv->debit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_g_code, 
                    ((ProwodKey2*)key)->priv->debit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_code, 
                    ((ProwodKey2*)key)->priv->debit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_other, 
                    ((ProwodKey2*)key)->priv->debit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->date, 
                    ((ProwodKey2*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->division, 
                    ((ProwodKey3*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_account, 
                    ((ProwodKey3*)key)->priv->credit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_subaccount, 
                    ((ProwodKey3*)key)->priv->credit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_g_code, 
                    ((ProwodKey3*)key)->priv->credit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_code, 
                    ((ProwodKey3*)key)->priv->credit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_other, 
                    ((ProwodKey3*)key)->priv->credit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->date, 
                    ((ProwodKey3*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->division, 
                    ((ProwodKey4*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->document, 
                    ((ProwodKey4*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->date, 
                    ((ProwodKey4*)key)->priv->date))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
            status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
            status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
            status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
            status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
            status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_NEXT);
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

static PyObject *Prowod_iter_last(Prowod* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getProwodKey0Type() ) {
        key_cmp = (prowod_key0_t*)do_malloc(sizeof(prowod_key0_t));
        memcpy(key_cmp, ((ProwodKey0*)key)->priv, sizeof(prowod_key0_t));
        status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProwodKey1Type() ) {
        key_cmp = (prowod_key1_t*)do_malloc(sizeof(prowod_key1_t));
        memcpy(key_cmp, ((ProwodKey1*)key)->priv, sizeof(prowod_key1_t));
        status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProwodKey2Type() ) {
        key_cmp = (prowod_key2_t*)do_malloc(sizeof(prowod_key2_t));
        memcpy(key_cmp, ((ProwodKey2*)key)->priv, sizeof(prowod_key2_t));
        status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProwodKey3Type() ) {
        key_cmp = (prowod_key3_t*)do_malloc(sizeof(prowod_key3_t));
        memcpy(key_cmp, ((ProwodKey3*)key)->priv, sizeof(prowod_key3_t));
        status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getProwodKey4Type() ) {
        key_cmp = (prowod_key4_t*)do_malloc(sizeof(prowod_key4_t));
        memcpy(key_cmp, ((ProwodKey4*)key)->priv, sizeof(prowod_key4_t));
        status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->division, 
                    ((ProwodKey0*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->date, 
                    ((ProwodKey0*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->type_doc, 
                    ((ProwodKey0*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key0_t*)key_cmp)->document, 
                    ((ProwodKey0*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->division, 
                    ((ProwodKey1*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->type_doc, 
                    ((ProwodKey1*)key)->priv->type_doc))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->date, 
                    ((ProwodKey1*)key)->priv->date))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key1_t*)key_cmp)->document, 
                    ((ProwodKey1*)key)->priv->document))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->division, 
                    ((ProwodKey2*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_account, 
                    ((ProwodKey2*)key)->priv->debit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_subaccount, 
                    ((ProwodKey2*)key)->priv->debit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_g_code, 
                    ((ProwodKey2*)key)->priv->debit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_p_code, 
                    ((ProwodKey2*)key)->priv->debit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->debit_other, 
                    ((ProwodKey2*)key)->priv->debit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key2_t*)key_cmp)->date, 
                    ((ProwodKey2*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->division, 
                    ((ProwodKey3*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_account, 
                    ((ProwodKey3*)key)->priv->credit_account))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_subaccount, 
                    ((ProwodKey3*)key)->priv->credit_subaccount))
                   break;
            }
       
            if ( depth >= 4 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_g_code, 
                    ((ProwodKey3*)key)->priv->credit_p_g_code))
                   break;
            }
       
            if ( depth >= 5 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_p_code, 
                    ((ProwodKey3*)key)->priv->credit_p_code))
                   break;
            }
       
            if ( depth >= 6 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->credit_other, 
                    ((ProwodKey3*)key)->priv->credit_other))
                   break;
            }
       
            if ( depth >= 7 ) {
                if ( do_cmp(((prowod_key3_t*)key_cmp)->date, 
                    ((ProwodKey3*)key)->priv->date))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->division, 
                    ((ProwodKey4*)key)->priv->division))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->document, 
                    ((ProwodKey4*)key)->priv->document))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((prowod_key4_t*)key_cmp)->date, 
                    ((ProwodKey4*)key)->priv->date))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Prowod_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getProwodKey0Type() ) {
            status = do_prowod_get0(self->alias->alias, self->priv, ((ProwodKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey1Type() ) {
            status = do_prowod_get1(self->alias->alias, self->priv, ((ProwodKey1*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey2Type() ) {
            status = do_prowod_get2(self->alias->alias, self->priv, ((ProwodKey2*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey3Type() ) {
            status = do_prowod_get3(self->alias->alias, self->priv, ((ProwodKey3*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getProwodKey4Type() ) {
            status = do_prowod_get4(self->alias->alias, self->priv, ((ProwodKey4*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Prowod_insert(Prowod* self)
{
    int status;
    status = do_prowod_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_update(Prowod* self)
{
    int status;
    status = do_prowod_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_delete(Prowod* self)
{
    int status;
    status = do_prowod_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Prowod_set_alias(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Prowod*)self)->alias);
    Py_INCREF(obj);
    ((Prowod*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Prowod_set(Prowod* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwodType() ) {
        memcpy(self->priv, ((Prowod*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwodKey0Type() ) {

        do_cpy(self->priv->data.division, ((ProwodKey0*)obj)->priv->division);

        do_cpy(self->priv->data.date, ((ProwodKey0*)obj)->priv->date);

        do_cpy(self->priv->data.type_doc, ((ProwodKey0*)obj)->priv->type_doc);

        do_cpy(self->priv->data.document, ((ProwodKey0*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getProwodKey1Type() ) {

        do_cpy(self->priv->data.division, ((ProwodKey1*)obj)->priv->division);

        do_cpy(self->priv->data.date, ((ProwodKey1*)obj)->priv->date);

        do_cpy(self->priv->data.type_doc, ((ProwodKey1*)obj)->priv->type_doc);

        do_cpy(self->priv->data.document, ((ProwodKey1*)obj)->priv->document);

    }

    else 
    if ( Py_TYPE(obj) == getProwodKey2Type() ) {

        do_cpy(self->priv->data.division, ((ProwodKey2*)obj)->priv->division);

        do_cpy(self->priv->data.date, ((ProwodKey2*)obj)->priv->date);

        do_cpy(self->priv->data.debit_account, ((ProwodKey2*)obj)->priv->debit_account);

        do_cpy(self->priv->data.debit_subaccount, ((ProwodKey2*)obj)->priv->debit_subaccount);

        do_cpy(self->priv->data.debit_other, ((ProwodKey2*)obj)->priv->debit_other);

        do_cpy(self->priv->data.debit_p_g_code, ((ProwodKey2*)obj)->priv->debit_p_g_code);

        do_cpy(self->priv->data.debit_p_code, ((ProwodKey2*)obj)->priv->debit_p_code);

    }

    else 
    if ( Py_TYPE(obj) == getProwodKey3Type() ) {

        do_cpy(self->priv->data.division, ((ProwodKey3*)obj)->priv->division);

        do_cpy(self->priv->data.date, ((ProwodKey3*)obj)->priv->date);

        do_cpy(self->priv->data.credit_account, ((ProwodKey3*)obj)->priv->credit_account);

        do_cpy(self->priv->data.credit_subaccount, ((ProwodKey3*)obj)->priv->credit_subaccount);

        do_cpy(self->priv->data.credit_other, ((ProwodKey3*)obj)->priv->credit_other);

        do_cpy(self->priv->data.credit_p_g_code, ((ProwodKey3*)obj)->priv->credit_p_g_code);

        do_cpy(self->priv->data.credit_p_code, ((ProwodKey3*)obj)->priv->credit_p_code);

    }

    else 
    if ( Py_TYPE(obj) == getProwodKey4Type() ) {

        do_cpy(self->priv->data.division, ((ProwodKey4*)obj)->priv->division);

        do_cpy(self->priv->data.date, ((ProwodKey4*)obj)->priv->date);

        do_cpy(self->priv->data.document, ((ProwodKey4*)obj)->priv->document);

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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_account", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_account", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_other", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_other", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_partner_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_account", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_account", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_other", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_other", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_partner_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("valuta", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_valuta", "O", value) )
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

            else
            if ( !strcmp("document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("operation_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_operation_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("operation", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_operation", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("pach", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_pach", "O", value) )
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
                do_log(LOG_ERR, "Class \"Prowod\"does not contain \"%s\"", field_name);
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

static PyObject *Prowod_fields(Prowod* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Prowod");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Prowod_division(self,NULL);
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

    value =  Prowod_date(self,NULL);
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

    value =  Prowod_debit_account(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_account", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_debit_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_debit_other(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_other", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_debit_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_debit_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_credit_account(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_account", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_credit_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_credit_other(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_other", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_credit_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_credit_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_valuta(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("valuta", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Prowod_get_sums(self, NULL);
    buf = get_quants_desc(value, "sums");
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_document_type(self,NULL);
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

    value =  Prowod_operation_type(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("operation_type", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_operation(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("operation", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Prowod_document(self,NULL);
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

    value =  Prowod_pach(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("pach", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value = Prowod_get_params(self, NULL);
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

static PyGetSetDef Prowod_getset[] = {

    {"division",(getter)Prowod_division},

    {"date",(getter)Prowod_date},

    {"date_raw",(getter)Prowod_date_raw},

    {"debit_account",(getter)Prowod_debit_account},

    {"debit_subaccount",(getter)Prowod_debit_subaccount},

    {"debit_other",(getter)Prowod_debit_other},

    {"debit_partner_region_code",(getter)Prowod_debit_partner_region_code},

    {"debit_partner_code",(getter)Prowod_debit_partner_code},

    {"credit_account",(getter)Prowod_credit_account},

    {"credit_subaccount",(getter)Prowod_credit_subaccount},

    {"credit_other",(getter)Prowod_credit_other},

    {"credit_partner_region_code",(getter)Prowod_credit_partner_region_code},

    {"credit_partner_code",(getter)Prowod_credit_partner_code},

    {"valuta",(getter)Prowod_valuta},

    {"sums",(getter)Prowod_get_sums},

    {"document_type",(getter)Prowod_document_type},

    {"operation_type",(getter)Prowod_operation_type},

    {"operation",(getter)Prowod_operation},

    {"document",(getter)Prowod_document},

    {"pach",(getter)Prowod_pach},

    {"params",(getter)Prowod_get_params},

    {NULL}
};

static PyMethodDef Prowod_methods[] = {
    {"set", (PyCFunction)Prowod_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Prowod_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Prowod_clone, METH_NOARGS, "clone"},

    {"clear_sums", (PyCFunction)Prowod_sums_clear, METH_NOARGS, "do_Prowod_sums_clear"},
    {"sum", (PyCFunction)Prowod_get_sum, METH_VARARGS|METH_KEYWORDS, "do_Prowod_sum"},
    {"set_sums", (PyCFunction)Prowod_set_sums, METH_VARARGS|METH_KEYWORDS, "do_Prowod_set_sums"},

    {"set_division", (PyCFunction)Prowod_set_division, METH_VARARGS|METH_KEYWORDS, "Prowod_set_division"},

    {"set_date", (PyCFunction)Prowod_set_date, METH_VARARGS|METH_KEYWORDS, "Prowod_set_date"},

    {"set_debit_account", (PyCFunction)Prowod_set_debit_account, METH_VARARGS|METH_KEYWORDS, "Prowod_set_debit_account"},

    {"set_debit_subaccount", (PyCFunction)Prowod_set_debit_subaccount, METH_VARARGS|METH_KEYWORDS, "Prowod_set_debit_subaccount"},

    {"set_debit_other", (PyCFunction)Prowod_set_debit_other, METH_VARARGS|METH_KEYWORDS, "Prowod_set_debit_other"},

    {"set_debit_partner_region_code", (PyCFunction)Prowod_set_debit_partner_region_code, METH_VARARGS|METH_KEYWORDS, "Prowod_set_debit_partner_region_code"},

    {"set_debit_partner_code", (PyCFunction)Prowod_set_debit_partner_code, METH_VARARGS|METH_KEYWORDS, "Prowod_set_debit_partner_code"},

    {"set_credit_account", (PyCFunction)Prowod_set_credit_account, METH_VARARGS|METH_KEYWORDS, "Prowod_set_credit_account"},

    {"set_credit_subaccount", (PyCFunction)Prowod_set_credit_subaccount, METH_VARARGS|METH_KEYWORDS, "Prowod_set_credit_subaccount"},

    {"set_credit_other", (PyCFunction)Prowod_set_credit_other, METH_VARARGS|METH_KEYWORDS, "Prowod_set_credit_other"},

    {"set_credit_partner_region_code", (PyCFunction)Prowod_set_credit_partner_region_code, METH_VARARGS|METH_KEYWORDS, "Prowod_set_credit_partner_region_code"},

    {"set_credit_partner_code", (PyCFunction)Prowod_set_credit_partner_code, METH_VARARGS|METH_KEYWORDS, "Prowod_set_credit_partner_code"},

    {"set_valuta", (PyCFunction)Prowod_set_valuta, METH_VARARGS|METH_KEYWORDS, "Prowod_set_valuta"},

    {"set_sum", (PyCFunction)Prowod_set_sum, METH_VARARGS|METH_KEYWORDS, "Prowod_set_sum"},

    {"set_document_type", (PyCFunction)Prowod_set_document_type, METH_VARARGS|METH_KEYWORDS, "Prowod_set_document_type"},

    {"set_operation_type", (PyCFunction)Prowod_set_operation_type, METH_VARARGS|METH_KEYWORDS, "Prowod_set_operation_type"},

    {"set_operation", (PyCFunction)Prowod_set_operation, METH_VARARGS|METH_KEYWORDS, "Prowod_set_operation"},

    {"set_document", (PyCFunction)Prowod_set_document, METH_VARARGS|METH_KEYWORDS, "Prowod_set_document"},

    {"set_pach", (PyCFunction)Prowod_set_pach, METH_VARARGS|METH_KEYWORDS, "Prowod_set_pach"},

    {"set_param", (PyCFunction)Prowod_set_param, METH_VARARGS|METH_KEYWORDS, "do_Prowod_param_set"},
    {"param", (PyCFunction)Prowod_get_param, METH_VARARGS|METH_KEYWORDS, "do_Prowod_param"},
    {"clear_params", (PyCFunction)Prowod_params_clear, METH_NOARGS, "do_Prowod_param_clear"},
    {"set_params", (PyCFunction)Prowod_set_params, METH_VARARGS|METH_KEYWORDS, "do_Prowod_set_params"},

    {"get_equal", (PyCFunction)Prowod_equal, METH_VARARGS|METH_KEYWORDS, "Prowod_equal"},

    {"get_next", (PyCFunction)Prowod_next, METH_VARARGS|METH_KEYWORDS, "Prowod_next"},

    {"get_prev", (PyCFunction)Prowod_prev, METH_VARARGS|METH_KEYWORDS, "Prowod_prev"},

    {"get_gt", (PyCFunction)Prowod_gt, METH_VARARGS|METH_KEYWORDS, "Prowod_gt"},

    {"get_ge", (PyCFunction)Prowod_ge, METH_VARARGS|METH_KEYWORDS, "Prowod_ge"},

    {"get_lt", (PyCFunction)Prowod_lt, METH_VARARGS|METH_KEYWORDS, "Prowod_lt"},

    {"get_le", (PyCFunction)Prowod_le, METH_VARARGS|METH_KEYWORDS, "Prowod_le"},

    {"get_first", (PyCFunction)Prowod_first, METH_VARARGS|METH_KEYWORDS, "Prowod_first"},

    {"get_last", (PyCFunction)Prowod_last, METH_VARARGS|METH_KEYWORDS, "Prowod_last"},

    {"gets_equal", (PyCFunction)Prowod_iter_equal, METH_VARARGS|METH_KEYWORDS, "Prowod_iter_equal"},

    {"gets_gt", (PyCFunction)Prowod_iter_gt, METH_VARARGS|METH_KEYWORDS, "Prowod_iter_gt"},

    {"gets_ge", (PyCFunction)Prowod_iter_ge, METH_VARARGS|METH_KEYWORDS, "Prowod_iter_ge"},

    {"gets_lt", (PyCFunction)Prowod_iter_lt, METH_VARARGS|METH_KEYWORDS, "Prowod_iter_lt"},

    {"gets_le", (PyCFunction)Prowod_iter_le, METH_VARARGS|METH_KEYWORDS, "Prowod_iter_le"},

    {"gets_first", (PyCFunction)Prowod_iter_first, METH_VARARGS|METH_KEYWORDS, "Prowod_iter_first"},

    {"gets_last", (PyCFunction)Prowod_iter_last, METH_VARARGS|METH_KEYWORDS, "Prowod_iter_last"},

    {"insert", (PyCFunction)Prowod_insert, METH_VARARGS|METH_KEYWORDS, "Prowod_insert"},

    {"update", (PyCFunction)Prowod_update, METH_VARARGS|METH_KEYWORDS, "Prowod_update"},

    {"delete", (PyCFunction)Prowod_delete, METH_VARARGS|METH_KEYWORDS, "Prowod_delete"},

    {NULL}
};

static PyTypeObject ProwodType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Prowod",             /*tp_name*/
    sizeof(Prowod),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Prowod_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Prowod_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Prowod objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Prowod_methods,             /* tp_methods */
    0,
    Prowod_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Prowod_init,      /* tp_init */
    0,                         /* tp_alloc */
    Prowod_new,                 /* tp_new */
};
PyTypeObject *getProwodType()
{
    return &ProwodType_;
}

static void ProwodKey0_dealloc(PyObject* self)
{
    do_free(((ProwodKey0*)self)->priv);
    Py_DECREF(((ProwodKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProwodKey0 free\n");
}
static PyObject *ProwodKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey0 new\n");
    ProwodKey0 *self;
    self = PyObject_NEW(ProwodKey0, type);
    //self = (ProwodKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod_key0_t));
    return (PyObject *)self;
}
static int ProwodKey0_init(ProwodKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProwodKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->division);

    do_text_set_empty(self->priv->type_doc);

    do_text_set_empty(self->priv->document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProwodKey0_clone(ProwodKey0 *self)
{
    ProwodKey0 *result;
    result = (ProwodKey0*)PyObject_CallFunction((PyObject*)getProwodKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProwodKey0_division(ProwodKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey0_set_division(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set division");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->division, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey0_date(ProwodKey0* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ProwodKey0_date_raw(ProwodKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ProwodKey0_set_date(ProwodKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProwodKey0_document_type(ProwodKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->type_doc);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey0_set_document_type(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type_doc");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->type_doc, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->type_doc);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey0_document(ProwodKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey0_set_document(ProwodKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProwodKey0_equal(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_next(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_prev(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_gt(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_ge(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_lt(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_le(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_first(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_last(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey0_iter_equal(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey0_iter_gt(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey0_iter_ge(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey0_iter_lt(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey0_iter_le(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey0_iter_first(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey0_iter_last(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey0_cmp(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwodKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->division, ((ProwodKey0*)obj)->priv->division);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->date - ((ProwodKey0*)obj)->priv->date;
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->type_doc, ((ProwodKey0*)obj)->priv->type_doc);
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->document, ((ProwodKey0*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProwodKey0_set_alias(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProwodKey0*)self)->alias);
    Py_INCREF(obj);
    ((ProwodKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProwodKey0_set(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwodKey0Type() ) {
        memcpy(self->priv, ((ProwodKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwodType() ) {

        do_cpy(self->priv->division, ((Prowod*)obj)->priv->data.division);

        do_cpy(self->priv->date, ((Prowod*)obj)->priv->data.date);

        do_cpy(self->priv->type_doc, ((Prowod*)obj)->priv->data.type_doc);

        do_cpy(self->priv->document, ((Prowod*)obj)->priv->data.document);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                        return NULL;
                    break;

                case 3:
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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document_type", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProwodKey0\"does not contain \"%s\"", field_name);
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

static PyObject *ProwodKey0_fields(ProwodKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProwodKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProwodKey0_division(self,NULL);
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

    value =  ProwodKey0_date(self,NULL);
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

    value =  ProwodKey0_document_type(self,NULL);
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

    value =  ProwodKey0_document(self,NULL);
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

static PyGetSetDef ProwodKey0_getset[] = {

    {"division",(getter)ProwodKey0_division},

    {"date",(getter)ProwodKey0_date},

    {"date_raw",(getter)ProwodKey0_date_raw},

    {"document_type",(getter)ProwodKey0_document_type},

    {"document",(getter)ProwodKey0_document},

    {NULL}
};

static PyMethodDef ProwodKey0_methods[] = {
    {"set", (PyCFunction)ProwodKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProwodKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProwodKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProwodKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)ProwodKey0_set_division, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_set_division"},

    {"set_date", (PyCFunction)ProwodKey0_set_date, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_set_date"},

    {"set_document_type", (PyCFunction)ProwodKey0_set_document_type, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_set_document_type"},

    {"set_document", (PyCFunction)ProwodKey0_set_document, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_set_document"},

    {"get_equal", (PyCFunction)ProwodKey0_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_equal"},

    {"get_next", (PyCFunction)ProwodKey0_next, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_next"},

    {"get_prev", (PyCFunction)ProwodKey0_prev, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_prev"},

    {"get_gt", (PyCFunction)ProwodKey0_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_gt"},

    {"get_ge", (PyCFunction)ProwodKey0_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_ge"},

    {"get_lt", (PyCFunction)ProwodKey0_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_lt"},

    {"get_le", (PyCFunction)ProwodKey0_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_le"},

    {"get_first", (PyCFunction)ProwodKey0_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_first"},

    {"get_last", (PyCFunction)ProwodKey0_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_last"},

    {"gets_equal", (PyCFunction)ProwodKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_iter_equal"},

    {"gets_gt", (PyCFunction)ProwodKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_iter_gt"},

    {"gets_ge", (PyCFunction)ProwodKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_iter_ge"},

    {"gets_lt", (PyCFunction)ProwodKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_iter_lt"},

    {"gets_le", (PyCFunction)ProwodKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_iter_le"},

    {"gets_first", (PyCFunction)ProwodKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_iter_first"},

    {"gets_last", (PyCFunction)ProwodKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey0_iter_last"},

    {NULL}
};

static PyTypeObject ProwodKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProwodKey0",             /*tp_name*/
    sizeof(ProwodKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProwodKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey0_methods,             /* tp_methods */
    0,
    ProwodKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey0_new,                 /* tp_new */
};
PyTypeObject *getProwodKey0Type()
{
    return &ProwodKey0Type_;
}

static void ProwodKey1_dealloc(PyObject* self)
{
    do_free(((ProwodKey1*)self)->priv);
    Py_DECREF(((ProwodKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProwodKey1 free\n");
}
static PyObject *ProwodKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey1 new\n");
    ProwodKey1 *self;
    self = PyObject_NEW(ProwodKey1, type);
    //self = (ProwodKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod_key1_t));
    return (PyObject *)self;
}
static int ProwodKey1_init(ProwodKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProwodKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->division);

    do_text_set_empty(self->priv->type_doc);

    do_text_set_empty(self->priv->document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProwodKey1_clone(ProwodKey1 *self)
{
    ProwodKey1 *result;
    result = (ProwodKey1*)PyObject_CallFunction((PyObject*)getProwodKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProwodKey1_division(ProwodKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey1_set_division(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set division");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->division, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey1_document_type(ProwodKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->type_doc);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey1_set_document_type(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set type_doc");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->type_doc, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->type_doc);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey1_date(ProwodKey1* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ProwodKey1_date_raw(ProwodKey1* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ProwodKey1_set_date(ProwodKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProwodKey1_document(ProwodKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey1_set_document(ProwodKey1* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProwodKey1_equal(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_next(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_prev(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_gt(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_ge(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_lt(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_le(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_first(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_last(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey1_iter_equal(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey1_iter_gt(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey1_iter_ge(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey1_iter_lt(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey1_iter_le(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey1_iter_first(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey1_iter_last(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.type_doc, 
                 self->priv->type_doc))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

 
        item = ProwodKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey1_cmp(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwodKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->division, ((ProwodKey1*)obj)->priv->division);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->type_doc, ((ProwodKey1*)obj)->priv->type_doc);
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->date - ((ProwodKey1*)obj)->priv->date;
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->document, ((ProwodKey1*)obj)->priv->document);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProwodKey1_set_alias(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProwodKey1*)self)->alias);
    Py_INCREF(obj);
    ((ProwodKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProwodKey1_set(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwodKey1Type() ) {
        memcpy(self->priv, ((ProwodKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwodType() ) {

        do_cpy(self->priv->division, ((Prowod*)obj)->priv->data.division);

        do_cpy(self->priv->type_doc, ((Prowod*)obj)->priv->data.type_doc);

        do_cpy(self->priv->date, ((Prowod*)obj)->priv->data.date);

        do_cpy(self->priv->document, ((Prowod*)obj)->priv->data.document);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document_type", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                        return NULL;
                    break;

                case 3:
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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
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
            if ( !strcmp("document", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProwodKey1\"does not contain \"%s\"", field_name);
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

static PyObject *ProwodKey1_fields(ProwodKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProwodKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProwodKey1_division(self,NULL);
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

    value =  ProwodKey1_document_type(self,NULL);
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

    value =  ProwodKey1_date(self,NULL);
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

    value =  ProwodKey1_document(self,NULL);
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

static PyGetSetDef ProwodKey1_getset[] = {

    {"division",(getter)ProwodKey1_division},

    {"document_type",(getter)ProwodKey1_document_type},

    {"date",(getter)ProwodKey1_date},

    {"date_raw",(getter)ProwodKey1_date_raw},

    {"document",(getter)ProwodKey1_document},

    {NULL}
};

static PyMethodDef ProwodKey1_methods[] = {
    {"set", (PyCFunction)ProwodKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProwodKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProwodKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProwodKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)ProwodKey1_set_division, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_set_division"},

    {"set_document_type", (PyCFunction)ProwodKey1_set_document_type, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_set_document_type"},

    {"set_date", (PyCFunction)ProwodKey1_set_date, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_set_date"},

    {"set_document", (PyCFunction)ProwodKey1_set_document, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_set_document"},

    {"get_equal", (PyCFunction)ProwodKey1_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_equal"},

    {"get_next", (PyCFunction)ProwodKey1_next, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_next"},

    {"get_prev", (PyCFunction)ProwodKey1_prev, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_prev"},

    {"get_gt", (PyCFunction)ProwodKey1_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_gt"},

    {"get_ge", (PyCFunction)ProwodKey1_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_ge"},

    {"get_lt", (PyCFunction)ProwodKey1_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_lt"},

    {"get_le", (PyCFunction)ProwodKey1_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_le"},

    {"get_first", (PyCFunction)ProwodKey1_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_first"},

    {"get_last", (PyCFunction)ProwodKey1_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_last"},

    {"gets_equal", (PyCFunction)ProwodKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_iter_equal"},

    {"gets_gt", (PyCFunction)ProwodKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_iter_gt"},

    {"gets_ge", (PyCFunction)ProwodKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_iter_ge"},

    {"gets_lt", (PyCFunction)ProwodKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_iter_lt"},

    {"gets_le", (PyCFunction)ProwodKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_iter_le"},

    {"gets_first", (PyCFunction)ProwodKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_iter_first"},

    {"gets_last", (PyCFunction)ProwodKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey1_iter_last"},

    {NULL}
};

static PyTypeObject ProwodKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProwodKey1",             /*tp_name*/
    sizeof(ProwodKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProwodKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey1_methods,             /* tp_methods */
    0,
    ProwodKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey1_new,                 /* tp_new */
};
PyTypeObject *getProwodKey1Type()
{
    return &ProwodKey1Type_;
}

static void ProwodKey2_dealloc(PyObject* self)
{
    do_free(((ProwodKey2*)self)->priv);
    Py_DECREF(((ProwodKey2*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProwodKey2 free\n");
}
static PyObject *ProwodKey2_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey2 new\n");
    ProwodKey2 *self;
    self = PyObject_NEW(ProwodKey2, type);
    //self = (ProwodKey2 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod_key2_t));
    return (PyObject *)self;
}
static int ProwodKey2_init(ProwodKey2 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProwodKey2 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->division);

    do_text_set_empty(self->priv->debit_account);

    do_text_set_empty(self->priv->debit_subaccount);

    do_text_set_empty(self->priv->debit_p_g_code);

    do_text_set_empty(self->priv->debit_other);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProwodKey2_clone(ProwodKey2 *self)
{
    ProwodKey2 *result;
    result = (ProwodKey2*)PyObject_CallFunction((PyObject*)getProwodKey2Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProwodKey2_division(ProwodKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey2_set_division(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set division");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->division, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey2_debit_account(ProwodKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->debit_account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey2_set_debit_account(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set debit_account");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->debit_account, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->debit_account);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey2_debit_subaccount(ProwodKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->debit_subaccount);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey2_set_debit_subaccount(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set debit_subaccount");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->debit_subaccount, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->debit_subaccount);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey2_debit_partner_region_code(ProwodKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->debit_p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey2_set_debit_partner_region_code(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set debit_p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->debit_p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->debit_p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey2_debit_partner_code(ProwodKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->debit_p_code);

    return result;
}

static PyObject *ProwodKey2_set_debit_partner_code(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set debit_p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->debit_p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->debit_p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set debit_p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->debit_p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey2_debit_other(ProwodKey2* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->debit_other);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey2_set_debit_other(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set debit_other");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->debit_other, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->debit_other);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey2_date(ProwodKey2* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ProwodKey2_date_raw(ProwodKey2* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ProwodKey2_set_date(ProwodKey2* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProwodKey2_equal(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_next(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_prev(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_gt(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_ge(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_lt(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_le(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_first(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_last(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey2_iter_equal(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.debit_account, 
                 self->priv->debit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.debit_subaccount, 
                 self->priv->debit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.debit_p_g_code, 
                 self->priv->debit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.debit_p_code, 
                 self->priv->debit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.debit_other, 
                 self->priv->debit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey2_iter_gt(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.debit_account, 
                 self->priv->debit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.debit_subaccount, 
                 self->priv->debit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.debit_p_g_code, 
                 self->priv->debit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.debit_p_code, 
                 self->priv->debit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.debit_other, 
                 self->priv->debit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey2_iter_ge(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.debit_account, 
                 self->priv->debit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.debit_subaccount, 
                 self->priv->debit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.debit_p_g_code, 
                 self->priv->debit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.debit_p_code, 
                 self->priv->debit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.debit_other, 
                 self->priv->debit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey2_iter_lt(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.debit_account, 
                 self->priv->debit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.debit_subaccount, 
                 self->priv->debit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.debit_p_g_code, 
                 self->priv->debit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.debit_p_code, 
                 self->priv->debit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.debit_other, 
                 self->priv->debit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey2_iter_le(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.debit_account, 
                 self->priv->debit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.debit_subaccount, 
                 self->priv->debit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.debit_p_g_code, 
                 self->priv->debit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.debit_p_code, 
                 self->priv->debit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.debit_other, 
                 self->priv->debit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey2_iter_first(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.debit_account, 
                 self->priv->debit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.debit_subaccount, 
                 self->priv->debit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.debit_p_g_code, 
                 self->priv->debit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.debit_p_code, 
                 self->priv->debit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.debit_other, 
                 self->priv->debit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey2_iter_last(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key2_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.debit_account, 
                 self->priv->debit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.debit_subaccount, 
                 self->priv->debit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.debit_p_g_code, 
                 self->priv->debit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.debit_p_code, 
                 self->priv->debit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.debit_other, 
                 self->priv->debit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey2_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key2(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey2_cmp(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwodKey2Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->division, ((ProwodKey2*)obj)->priv->division);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->debit_account, ((ProwodKey2*)obj)->priv->debit_account);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->debit_subaccount, ((ProwodKey2*)obj)->priv->debit_subaccount);
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->debit_p_g_code, ((ProwodKey2*)obj)->priv->debit_p_g_code);
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->debit_p_code - ((ProwodKey2*)obj)->priv->debit_p_code;
       }

       if ( !res && depth >= 6 ) {
           res = do_cmp(self->priv->debit_other, ((ProwodKey2*)obj)->priv->debit_other);
       }

       if ( !res && depth >= 7 ) {
           res = self->priv->date - ((ProwodKey2*)obj)->priv->date;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProwodKey2_set_alias(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProwodKey2*)self)->alias);
    Py_INCREF(obj);
    ((ProwodKey2*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProwodKey2_set(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwodKey2Type() ) {
        memcpy(self->priv, ((ProwodKey2*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwodType() ) {

        do_cpy(self->priv->division, ((Prowod*)obj)->priv->data.division);

        do_cpy(self->priv->debit_account, ((Prowod*)obj)->priv->data.debit_account);

        do_cpy(self->priv->debit_subaccount, ((Prowod*)obj)->priv->data.debit_subaccount);

        do_cpy(self->priv->debit_p_g_code, ((Prowod*)obj)->priv->data.debit_p_g_code);

        do_cpy(self->priv->debit_p_code, ((Prowod*)obj)->priv->data.debit_p_code);

        do_cpy(self->priv->debit_other, ((Prowod*)obj)->priv->data.debit_other);

        do_cpy(self->priv->date, ((Prowod*)obj)->priv->data.date);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_debit_account", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_debit_subaccount", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_debit_partner_region_code", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_debit_partner_code", "O", value) )
                        return NULL;
                    break;

                case 5:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_debit_other", "O", value) )
                        return NULL;
                    break;

                case 6:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_account", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_account", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_partner_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit_other", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit_other", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProwodKey2\"does not contain \"%s\"", field_name);
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

static PyObject *ProwodKey2_fields(ProwodKey2* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProwodKey2");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProwodKey2_division(self,NULL);
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

    value =  ProwodKey2_debit_account(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_account", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey2_debit_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey2_debit_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey2_debit_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey2_debit_other(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit_other", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey2_date(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef ProwodKey2_getset[] = {

    {"division",(getter)ProwodKey2_division},

    {"debit_account",(getter)ProwodKey2_debit_account},

    {"debit_subaccount",(getter)ProwodKey2_debit_subaccount},

    {"debit_partner_region_code",(getter)ProwodKey2_debit_partner_region_code},

    {"debit_partner_code",(getter)ProwodKey2_debit_partner_code},

    {"debit_other",(getter)ProwodKey2_debit_other},

    {"date",(getter)ProwodKey2_date},

    {"date_raw",(getter)ProwodKey2_date_raw},

    {NULL}
};

static PyMethodDef ProwodKey2_methods[] = {
    {"set", (PyCFunction)ProwodKey2_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProwodKey2_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProwodKey2_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProwodKey2_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)ProwodKey2_set_division, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_set_division"},

    {"set_debit_account", (PyCFunction)ProwodKey2_set_debit_account, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_set_debit_account"},

    {"set_debit_subaccount", (PyCFunction)ProwodKey2_set_debit_subaccount, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_set_debit_subaccount"},

    {"set_debit_partner_region_code", (PyCFunction)ProwodKey2_set_debit_partner_region_code, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_set_debit_partner_region_code"},

    {"set_debit_partner_code", (PyCFunction)ProwodKey2_set_debit_partner_code, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_set_debit_partner_code"},

    {"set_debit_other", (PyCFunction)ProwodKey2_set_debit_other, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_set_debit_other"},

    {"set_date", (PyCFunction)ProwodKey2_set_date, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_set_date"},

    {"get_equal", (PyCFunction)ProwodKey2_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_equal"},

    {"get_next", (PyCFunction)ProwodKey2_next, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_next"},

    {"get_prev", (PyCFunction)ProwodKey2_prev, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_prev"},

    {"get_gt", (PyCFunction)ProwodKey2_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_gt"},

    {"get_ge", (PyCFunction)ProwodKey2_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_ge"},

    {"get_lt", (PyCFunction)ProwodKey2_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_lt"},

    {"get_le", (PyCFunction)ProwodKey2_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_le"},

    {"get_first", (PyCFunction)ProwodKey2_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_first"},

    {"get_last", (PyCFunction)ProwodKey2_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_last"},

    {"gets_equal", (PyCFunction)ProwodKey2_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_iter_equal"},

    {"gets_gt", (PyCFunction)ProwodKey2_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_iter_gt"},

    {"gets_ge", (PyCFunction)ProwodKey2_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_iter_ge"},

    {"gets_lt", (PyCFunction)ProwodKey2_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_iter_lt"},

    {"gets_le", (PyCFunction)ProwodKey2_iter_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_iter_le"},

    {"gets_first", (PyCFunction)ProwodKey2_iter_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_iter_first"},

    {"gets_last", (PyCFunction)ProwodKey2_iter_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey2_iter_last"},

    {NULL}
};

static PyTypeObject ProwodKey2Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProwodKey2",             /*tp_name*/
    sizeof(ProwodKey2),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey2_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProwodKey2_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey2 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey2_methods,             /* tp_methods */
    0,
    ProwodKey2_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey2_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey2_new,                 /* tp_new */
};
PyTypeObject *getProwodKey2Type()
{
    return &ProwodKey2Type_;
}

static void ProwodKey3_dealloc(PyObject* self)
{
    do_free(((ProwodKey3*)self)->priv);
    Py_DECREF(((ProwodKey3*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProwodKey3 free\n");
}
static PyObject *ProwodKey3_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey3 new\n");
    ProwodKey3 *self;
    self = PyObject_NEW(ProwodKey3, type);
    //self = (ProwodKey3 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod_key3_t));
    return (PyObject *)self;
}
static int ProwodKey3_init(ProwodKey3 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProwodKey3 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->division);

    do_text_set_empty(self->priv->credit_account);

    do_text_set_empty(self->priv->credit_subaccount);

    do_text_set_empty(self->priv->credit_p_g_code);

    do_text_set_empty(self->priv->credit_other);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProwodKey3_clone(ProwodKey3 *self)
{
    ProwodKey3 *result;
    result = (ProwodKey3*)PyObject_CallFunction((PyObject*)getProwodKey3Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProwodKey3_division(ProwodKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey3_set_division(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set division");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->division, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey3_credit_account(ProwodKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->credit_account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey3_set_credit_account(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set credit_account");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->credit_account, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->credit_account);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey3_credit_subaccount(ProwodKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->credit_subaccount);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey3_set_credit_subaccount(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set credit_subaccount");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->credit_subaccount, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->credit_subaccount);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey3_credit_partner_region_code(ProwodKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->credit_p_g_code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey3_set_credit_partner_region_code(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set credit_p_g_code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->credit_p_g_code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->credit_p_g_code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey3_credit_partner_code(ProwodKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->credit_p_code);

    return result;
}

static PyObject *ProwodKey3_set_credit_partner_code(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set credit_p_code");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->credit_p_code = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->credit_p_code = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set credit_p_code");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->credit_p_code);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey3_credit_other(ProwodKey3* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->credit_other);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey3_set_credit_other(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set credit_other");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->credit_other, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->credit_other);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey3_date(ProwodKey3* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ProwodKey3_date_raw(ProwodKey3* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ProwodKey3_set_date(ProwodKey3* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProwodKey3_equal(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_next(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_prev(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_gt(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_ge(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_lt(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_le(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_first(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_last(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey3_iter_equal(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.credit_account, 
                 self->priv->credit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.credit_subaccount, 
                 self->priv->credit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.credit_p_g_code, 
                 self->priv->credit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.credit_p_code, 
                 self->priv->credit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.credit_other, 
                 self->priv->credit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey3_iter_gt(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.credit_account, 
                 self->priv->credit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.credit_subaccount, 
                 self->priv->credit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.credit_p_g_code, 
                 self->priv->credit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.credit_p_code, 
                 self->priv->credit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.credit_other, 
                 self->priv->credit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey3_iter_ge(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.credit_account, 
                 self->priv->credit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.credit_subaccount, 
                 self->priv->credit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.credit_p_g_code, 
                 self->priv->credit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.credit_p_code, 
                 self->priv->credit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.credit_other, 
                 self->priv->credit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey3_iter_lt(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.credit_account, 
                 self->priv->credit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.credit_subaccount, 
                 self->priv->credit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.credit_p_g_code, 
                 self->priv->credit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.credit_p_code, 
                 self->priv->credit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.credit_other, 
                 self->priv->credit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey3_iter_le(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.credit_account, 
                 self->priv->credit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.credit_subaccount, 
                 self->priv->credit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.credit_p_g_code, 
                 self->priv->credit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.credit_p_code, 
                 self->priv->credit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.credit_other, 
                 self->priv->credit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey3_iter_first(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.credit_account, 
                 self->priv->credit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.credit_subaccount, 
                 self->priv->credit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.credit_p_g_code, 
                 self->priv->credit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.credit_p_code, 
                 self->priv->credit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.credit_other, 
                 self->priv->credit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey3_iter_last(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key3_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.credit_account, 
                 self->priv->credit_account))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.credit_subaccount, 
                 self->priv->credit_subaccount))
               break;
       }

       if ( depth >= 4 ) {
           if ( do_cmp(key_cmp.credit_p_g_code, 
                 self->priv->credit_p_g_code))
               break;
       }

       if ( depth >= 5 ) {
           if ( do_cmp(key_cmp.credit_p_code, 
                 self->priv->credit_p_code))
               break;
       }

       if ( depth >= 6 ) {
           if ( do_cmp(key_cmp.credit_other, 
                 self->priv->credit_other))
               break;
       }

       if ( depth >= 7 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey3_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key3(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey3_cmp(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwodKey3Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->division, ((ProwodKey3*)obj)->priv->division);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->credit_account, ((ProwodKey3*)obj)->priv->credit_account);
       }

       if ( !res && depth >= 3 ) {
           res = do_cmp(self->priv->credit_subaccount, ((ProwodKey3*)obj)->priv->credit_subaccount);
       }

       if ( !res && depth >= 4 ) {
           res = do_cmp(self->priv->credit_p_g_code, ((ProwodKey3*)obj)->priv->credit_p_g_code);
       }

       if ( !res && depth >= 5 ) {
           res = self->priv->credit_p_code - ((ProwodKey3*)obj)->priv->credit_p_code;
       }

       if ( !res && depth >= 6 ) {
           res = do_cmp(self->priv->credit_other, ((ProwodKey3*)obj)->priv->credit_other);
       }

       if ( !res && depth >= 7 ) {
           res = self->priv->date - ((ProwodKey3*)obj)->priv->date;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProwodKey3_set_alias(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProwodKey3*)self)->alias);
    Py_INCREF(obj);
    ((ProwodKey3*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProwodKey3_set(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwodKey3Type() ) {
        memcpy(self->priv, ((ProwodKey3*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwodType() ) {

        do_cpy(self->priv->division, ((Prowod*)obj)->priv->data.division);

        do_cpy(self->priv->credit_account, ((Prowod*)obj)->priv->data.credit_account);

        do_cpy(self->priv->credit_subaccount, ((Prowod*)obj)->priv->data.credit_subaccount);

        do_cpy(self->priv->credit_p_g_code, ((Prowod*)obj)->priv->data.credit_p_g_code);

        do_cpy(self->priv->credit_p_code, ((Prowod*)obj)->priv->data.credit_p_code);

        do_cpy(self->priv->credit_other, ((Prowod*)obj)->priv->data.credit_other);

        do_cpy(self->priv->date, ((Prowod*)obj)->priv->data.date);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_credit_account", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_credit_subaccount", "O", value) )
                        return NULL;
                    break;

                case 3:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_credit_partner_region_code", "O", value) )
                        return NULL;
                    break;

                case 4:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_credit_partner_code", "O", value) )
                        return NULL;
                    break;

                case 5:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_credit_other", "O", value) )
                        return NULL;
                    break;

                case 6:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_account", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_account", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_partner_region_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_partner_region_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_partner_code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_partner_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit_other", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit_other", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("date", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"ProwodKey3\"does not contain \"%s\"", field_name);
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

static PyObject *ProwodKey3_fields(ProwodKey3* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProwodKey3");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProwodKey3_division(self,NULL);
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

    value =  ProwodKey3_credit_account(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_account", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey3_credit_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey3_credit_partner_region_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_partner_region_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey3_credit_partner_code(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_partner_code", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey3_credit_other(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit_other", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  ProwodKey3_date(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef ProwodKey3_getset[] = {

    {"division",(getter)ProwodKey3_division},

    {"credit_account",(getter)ProwodKey3_credit_account},

    {"credit_subaccount",(getter)ProwodKey3_credit_subaccount},

    {"credit_partner_region_code",(getter)ProwodKey3_credit_partner_region_code},

    {"credit_partner_code",(getter)ProwodKey3_credit_partner_code},

    {"credit_other",(getter)ProwodKey3_credit_other},

    {"date",(getter)ProwodKey3_date},

    {"date_raw",(getter)ProwodKey3_date_raw},

    {NULL}
};

static PyMethodDef ProwodKey3_methods[] = {
    {"set", (PyCFunction)ProwodKey3_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProwodKey3_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProwodKey3_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProwodKey3_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)ProwodKey3_set_division, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_set_division"},

    {"set_credit_account", (PyCFunction)ProwodKey3_set_credit_account, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_set_credit_account"},

    {"set_credit_subaccount", (PyCFunction)ProwodKey3_set_credit_subaccount, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_set_credit_subaccount"},

    {"set_credit_partner_region_code", (PyCFunction)ProwodKey3_set_credit_partner_region_code, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_set_credit_partner_region_code"},

    {"set_credit_partner_code", (PyCFunction)ProwodKey3_set_credit_partner_code, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_set_credit_partner_code"},

    {"set_credit_other", (PyCFunction)ProwodKey3_set_credit_other, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_set_credit_other"},

    {"set_date", (PyCFunction)ProwodKey3_set_date, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_set_date"},

    {"get_equal", (PyCFunction)ProwodKey3_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_equal"},

    {"get_next", (PyCFunction)ProwodKey3_next, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_next"},

    {"get_prev", (PyCFunction)ProwodKey3_prev, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_prev"},

    {"get_gt", (PyCFunction)ProwodKey3_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_gt"},

    {"get_ge", (PyCFunction)ProwodKey3_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_ge"},

    {"get_lt", (PyCFunction)ProwodKey3_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_lt"},

    {"get_le", (PyCFunction)ProwodKey3_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_le"},

    {"get_first", (PyCFunction)ProwodKey3_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_first"},

    {"get_last", (PyCFunction)ProwodKey3_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_last"},

    {"gets_equal", (PyCFunction)ProwodKey3_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_iter_equal"},

    {"gets_gt", (PyCFunction)ProwodKey3_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_iter_gt"},

    {"gets_ge", (PyCFunction)ProwodKey3_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_iter_ge"},

    {"gets_lt", (PyCFunction)ProwodKey3_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_iter_lt"},

    {"gets_le", (PyCFunction)ProwodKey3_iter_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_iter_le"},

    {"gets_first", (PyCFunction)ProwodKey3_iter_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_iter_first"},

    {"gets_last", (PyCFunction)ProwodKey3_iter_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey3_iter_last"},

    {NULL}
};

static PyTypeObject ProwodKey3Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProwodKey3",             /*tp_name*/
    sizeof(ProwodKey3),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey3_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProwodKey3_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey3 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey3_methods,             /* tp_methods */
    0,
    ProwodKey3_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey3_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey3_new,                 /* tp_new */
};
PyTypeObject *getProwodKey3Type()
{
    return &ProwodKey3Type_;
}

static void ProwodKey4_dealloc(PyObject* self)
{
    do_free(((ProwodKey4*)self)->priv);
    Py_DECREF(((ProwodKey4*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("ProwodKey4 free\n");
}
static PyObject *ProwodKey4_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("ProwodKey4 new\n");
    ProwodKey4 *self;
    self = PyObject_NEW(ProwodKey4, type);
    //self = (ProwodKey4 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(prowod_key4_t));
    return (PyObject *)self;
}
static int ProwodKey4_init(ProwodKey4 *self, PyObject *args, PyObject *kwds)
{
    //LOG("ProwodKey4 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->division);

    do_text_set_empty(self->priv->document);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *ProwodKey4_clone(ProwodKey4 *self)
{
    ProwodKey4 *result;
    result = (ProwodKey4*)PyObject_CallFunction((PyObject*)getProwodKey4Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *ProwodKey4_division(ProwodKey4* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey4_set_division(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set division");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->division, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->division);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *ProwodKey4_document(ProwodKey4* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->document);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *ProwodKey4_set_document(ProwodKey4* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProwodKey4_date(ProwodKey4* self, void *unused)
{
    PyObject *result;

    struct tm tm;
    do_date(self->priv->date, &tm);
    result = PyDate_FromDate(tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);

    return result;
}

static PyObject *ProwodKey4_date_raw(ProwodKey4* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->date);

    return result;
}

static PyObject *ProwodKey4_set_date(ProwodKey4* self, PyObject *args, PyObject *kwds)
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

static PyObject *ProwodKey4_equal(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_next(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_prev(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_gt(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_ge(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_lt(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_le(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_first(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_last(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *ProwodKey4_iter_equal(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey4_iter_gt(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey4_iter_ge(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey4_iter_lt(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey4_iter_le(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey4_iter_first(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey4_iter_last(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    prowod_key4_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.division, 
                 self->priv->division))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.document, 
                 self->priv->document))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.date, 
                 self->priv->date))
               break;
       }

 
        item = ProwodKey4_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_prowod_key4(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *ProwodKey4_cmp(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getProwodKey4Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->division, ((ProwodKey4*)obj)->priv->division);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->document, ((ProwodKey4*)obj)->priv->document);
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->date - ((ProwodKey4*)obj)->priv->date;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *ProwodKey4_set_alias(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((ProwodKey4*)self)->alias);
    Py_INCREF(obj);
    ((ProwodKey4*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *ProwodKey4_set(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getProwodKey4Type() ) {
        memcpy(self->priv, ((ProwodKey4*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getProwodType() ) {

        do_cpy(self->priv->division, ((Prowod*)obj)->priv->data.division);

        do_cpy(self->priv->document, ((Prowod*)obj)->priv->data.document);

        do_cpy(self->priv->date, ((Prowod*)obj)->priv->data.date);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_document", "O", value) )
                        return NULL;
                    break;

                case 2:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_date", "O", value) )
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
            if ( !strcmp("division", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_division", "O", value) )
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

            else {
                do_log(LOG_ERR, "Class \"ProwodKey4\"does not contain \"%s\"", field_name);
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

static PyObject *ProwodKey4_fields(ProwodKey4* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class ProwodKey4");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  ProwodKey4_division(self,NULL);
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

    value =  ProwodKey4_document(self,NULL);
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

    value =  ProwodKey4_date(self,NULL);
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
    
    buf = do_strdup_printf("%s\n",str);
    retval = MyString_FromString(buf);
    do_free(str);
    do_free(buf);
    return retval;
}

static PyGetSetDef ProwodKey4_getset[] = {

    {"division",(getter)ProwodKey4_division},

    {"document",(getter)ProwodKey4_document},

    {"date",(getter)ProwodKey4_date},

    {"date_raw",(getter)ProwodKey4_date_raw},

    {NULL}
};

static PyMethodDef ProwodKey4_methods[] = {
    {"set", (PyCFunction)ProwodKey4_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)ProwodKey4_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)ProwodKey4_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)ProwodKey4_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_division", (PyCFunction)ProwodKey4_set_division, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_set_division"},

    {"set_document", (PyCFunction)ProwodKey4_set_document, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_set_document"},

    {"set_date", (PyCFunction)ProwodKey4_set_date, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_set_date"},

    {"get_equal", (PyCFunction)ProwodKey4_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_equal"},

    {"get_next", (PyCFunction)ProwodKey4_next, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_next"},

    {"get_prev", (PyCFunction)ProwodKey4_prev, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_prev"},

    {"get_gt", (PyCFunction)ProwodKey4_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_gt"},

    {"get_ge", (PyCFunction)ProwodKey4_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_ge"},

    {"get_lt", (PyCFunction)ProwodKey4_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_lt"},

    {"get_le", (PyCFunction)ProwodKey4_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_le"},

    {"get_first", (PyCFunction)ProwodKey4_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_first"},

    {"get_last", (PyCFunction)ProwodKey4_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_last"},

    {"gets_equal", (PyCFunction)ProwodKey4_iter_equal, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_iter_equal"},

    {"gets_gt", (PyCFunction)ProwodKey4_iter_gt, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_iter_gt"},

    {"gets_ge", (PyCFunction)ProwodKey4_iter_ge, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_iter_ge"},

    {"gets_lt", (PyCFunction)ProwodKey4_iter_lt, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_iter_lt"},

    {"gets_le", (PyCFunction)ProwodKey4_iter_le, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_iter_le"},

    {"gets_first", (PyCFunction)ProwodKey4_iter_first, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_iter_first"},

    {"gets_last", (PyCFunction)ProwodKey4_iter_last, METH_VARARGS|METH_KEYWORDS, "ProwodKey4_iter_last"},

    {NULL}
};

static PyTypeObject ProwodKey4Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.ProwodKey4",             /*tp_name*/
    sizeof(ProwodKey4),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)ProwodKey4_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)ProwodKey4_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "ProwodKey4 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    ProwodKey4_methods,             /* tp_methods */
    0,
    ProwodKey4_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)ProwodKey4_init,      /* tp_init */
    0,                         /* tp_alloc */
    ProwodKey4_new,                 /* tp_new */
};
PyTypeObject *getProwodKey4Type()
{
    return &ProwodKey4Type_;
}
