
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "account.h"

static void Account_dealloc(PyObject* self)
{
    do_free(((Account*)self)->priv);
    Py_DECREF(((Account*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Account free\n");
}
static PyObject *Account_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Account new\n");
    Account *self;
    self = PyObject_NEW(Account, type);
    //self = (Account *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(account_rec_t));
    return (PyObject *)self;
}
static int Account_init(Account *self, PyObject *args, PyObject *kwds)
{
    //LOG("Account init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.account);

    do_text_set_empty(self->priv->data.name);

    do_text_set_empty(self->priv->data.othername);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Account_clone(Account *self)
{
    Account *result;
    result = (Account*)PyObject_CallFunction((PyObject*)getAccountType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Account_account(Account* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Account_set_account(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.account");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.account, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.account);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_name(Account* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Account_set_name(Account* self, PyObject *args, PyObject *kwds)
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

static PyObject *Account_debit(Account* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.debit);

    return result;
}

static PyObject *Account_set_debit(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.debit = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.debit = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.debit");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.debit);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_credit(Account* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.credit);

    return result;
}

static PyObject *Account_set_credit(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.credit = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.credit = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.credit");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.credit);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_positive_saldo(Account* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.positivesaldo);

    return result;
}

static PyObject *Account_set_positive_saldo(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.positivesaldo");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.positivesaldo = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.positivesaldo = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.positivesaldo");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.positivesaldo);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_total(Account* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.itog);

    return result;
}

static PyObject *Account_set_total(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.itog");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.itog = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.itog = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.itog");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.itog);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_analit_subaccount(Account* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.analitsub);

    return result;
}

static PyObject *Account_set_analit_subaccount(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.analitsub");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.analitsub = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.analitsub = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.analitsub");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.analitsub);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_analit_parnter(Account* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.analitpartner);

    return result;
}

static PyObject *Account_set_analit_parnter(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.analitpartner");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.analitpartner = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.analitpartner = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.analitpartner");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.analitpartner);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_analit_other(Account* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.analitother);

    return result;
}

static PyObject *Account_set_analit_other(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.analitother");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.analitother = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.analitother = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.analitother");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.analitother);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_other_name(Account* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.othername);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Account_set_other_name(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.othername");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.othername, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.othername);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_sum_include(Account* self, void *unused)
{
    PyObject *result;

    result = PyTuple_New(3);
    PyTuple_SET_ITEM(result, 0, MyLong_FromLong(self->priv->data.sumincl[0]));
    PyTuple_SET_ITEM(result, 1, MyLong_FromLong(self->priv->data.sumincl[1]));
    PyTuple_SET_ITEM(result, 2, MyLong_FromLong(self->priv->data.sumincl[2]));

    return result;
}

static PyObject *Account_set_sum_include(Account* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    int number;
    static char *kwlist[] = {"number","value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set data.sumincl");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->data.sumincl[number] = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->data.sumincl[number] = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.sumincl");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->data.sumincl[number]);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Account_gt(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_last(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_next(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_le(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_lt(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_equal(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_ge(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_prev(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_first(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getAccountKey0Type() )
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_iter_gt(Account* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getAccountKey0Type() ) {
        key_cmp = (account_key0_t*)do_malloc(sizeof(account_key0_t));
        memcpy(key_cmp, ((AccountKey0*)key)->priv, sizeof(account_key0_t));
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->account, 
                    ((AccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->itog, 
                    ((AccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Account_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
            status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Account_iter_last(Account* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getAccountKey0Type() ) {
        key_cmp = (account_key0_t*)do_malloc(sizeof(account_key0_t));
        memcpy(key_cmp, ((AccountKey0*)key)->priv, sizeof(account_key0_t));
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->account, 
                    ((AccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->itog, 
                    ((AccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Account_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
            status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Account_iter_le(Account* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getAccountKey0Type() ) {
        key_cmp = (account_key0_t*)do_malloc(sizeof(account_key0_t));
        memcpy(key_cmp, ((AccountKey0*)key)->priv, sizeof(account_key0_t));
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->account, 
                    ((AccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->itog, 
                    ((AccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Account_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
            status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Account_iter_ge(Account* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getAccountKey0Type() ) {
        key_cmp = (account_key0_t*)do_malloc(sizeof(account_key0_t));
        memcpy(key_cmp, ((AccountKey0*)key)->priv, sizeof(account_key0_t));
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->account, 
                    ((AccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->itog, 
                    ((AccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Account_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
            status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Account_iter_equal(Account* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getAccountKey0Type() ) {
        key_cmp = (account_key0_t*)do_malloc(sizeof(account_key0_t));
        memcpy(key_cmp, ((AccountKey0*)key)->priv, sizeof(account_key0_t));
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->account, 
                    ((AccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->itog, 
                    ((AccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Account_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
            status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Account_iter_lt(Account* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getAccountKey0Type() ) {
        key_cmp = (account_key0_t*)do_malloc(sizeof(account_key0_t));
        memcpy(key_cmp, ((AccountKey0*)key)->priv, sizeof(account_key0_t));
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->account, 
                    ((AccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->itog, 
                    ((AccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Account_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
            status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Account_iter_first(Account* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getAccountKey0Type() ) {
        key_cmp = (account_key0_t*)do_malloc(sizeof(account_key0_t));
        memcpy(key_cmp, ((AccountKey0*)key)->priv, sizeof(account_key0_t));
        status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->account, 
                    ((AccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((account_key0_t*)key_cmp)->itog, 
                    ((AccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Account_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getAccountKey0Type() ) {
            status = do_account_get0(self->alias->alias, self->priv, ((AccountKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Account_insert(Account* self)
{
    int status;
    status = do_account_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_update(Account* self)
{
    int status;
    status = do_account_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_delete(Account* self)
{
    int status;
    status = do_account_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Account_set_alias(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Account*)self)->alias);
    Py_INCREF(obj);
    ((Account*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Account_set(Account* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getAccountType() ) {
        memcpy(self->priv, ((Account*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getAccountKey0Type() ) {

        do_cpy(self->priv->data.account, ((AccountKey0*)obj)->priv->account);

        do_cpy(self->priv->data.itog, ((AccountKey0*)obj)->priv->itog);

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
            if ( !strcmp("account", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_account", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("debit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_debit", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("credit", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_credit", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("positive_saldo", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_positive_saldo", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_total", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("analit_subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_analit_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("analit_parnter", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_analit_parnter", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("analit_other", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_analit_other", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("other_name", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_other_name", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("sum_include", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sum_include", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Account\"does not contain \"%s\"", field_name);
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

static PyObject *Account_fields(Account* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Account");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Account_account(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("account", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Account_name(self,NULL);
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

    value =  Account_debit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("debit", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Account_credit(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("credit", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Account_positive_saldo(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("positive_saldo", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Account_total(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("total", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Account_analit_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("analit_subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Account_analit_parnter(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("analit_parnter", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Account_analit_other(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("analit_other", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Account_other_name(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("other_name", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    
    buf = do_strdup_printf("SumInclude:[%d,%d,%d]", self->priv->data.sumincl[0],self->priv->data.sumincl[1],self->priv->data.sumincl[2]);
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

static PyGetSetDef Account_getset[] = {

    {"account",(getter)Account_account},

    {"name",(getter)Account_name},

    {"debit",(getter)Account_debit},

    {"credit",(getter)Account_credit},

    {"positive_saldo",(getter)Account_positive_saldo},

    {"total",(getter)Account_total},

    {"analit_subaccount",(getter)Account_analit_subaccount},

    {"analit_parnter",(getter)Account_analit_parnter},

    {"analit_other",(getter)Account_analit_other},

    {"other_name",(getter)Account_other_name},

    {"sum_include",(getter)Account_sum_include},

    {NULL}
};

static PyMethodDef Account_methods[] = {
    {"set", (PyCFunction)Account_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Account_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Account_clone, METH_NOARGS, "clone"},

    {"set_account", (PyCFunction)Account_set_account, METH_VARARGS|METH_KEYWORDS, "Account_set_account"},

    {"set_name", (PyCFunction)Account_set_name, METH_VARARGS|METH_KEYWORDS, "Account_set_name"},

    {"set_debit", (PyCFunction)Account_set_debit, METH_VARARGS|METH_KEYWORDS, "Account_set_debit"},

    {"set_credit", (PyCFunction)Account_set_credit, METH_VARARGS|METH_KEYWORDS, "Account_set_credit"},

    {"set_positive_saldo", (PyCFunction)Account_set_positive_saldo, METH_VARARGS|METH_KEYWORDS, "Account_set_positive_saldo"},

    {"set_total", (PyCFunction)Account_set_total, METH_VARARGS|METH_KEYWORDS, "Account_set_total"},

    {"set_analit_subaccount", (PyCFunction)Account_set_analit_subaccount, METH_VARARGS|METH_KEYWORDS, "Account_set_analit_subaccount"},

    {"set_analit_parnter", (PyCFunction)Account_set_analit_parnter, METH_VARARGS|METH_KEYWORDS, "Account_set_analit_parnter"},

    {"set_analit_other", (PyCFunction)Account_set_analit_other, METH_VARARGS|METH_KEYWORDS, "Account_set_analit_other"},

    {"set_other_name", (PyCFunction)Account_set_other_name, METH_VARARGS|METH_KEYWORDS, "Account_set_other_name"},

    {"set_sum_include", (PyCFunction)Account_set_sum_include, METH_VARARGS|METH_KEYWORDS, "Account_set_sum_include"},

    {"get_gt", (PyCFunction)Account_gt, METH_VARARGS|METH_KEYWORDS, "Account_gt"},

    {"get_last", (PyCFunction)Account_last, METH_VARARGS|METH_KEYWORDS, "Account_last"},

    {"get_next", (PyCFunction)Account_next, METH_VARARGS|METH_KEYWORDS, "Account_next"},

    {"get_le", (PyCFunction)Account_le, METH_VARARGS|METH_KEYWORDS, "Account_le"},

    {"get_lt", (PyCFunction)Account_lt, METH_VARARGS|METH_KEYWORDS, "Account_lt"},

    {"get_equal", (PyCFunction)Account_equal, METH_VARARGS|METH_KEYWORDS, "Account_equal"},

    {"get_ge", (PyCFunction)Account_ge, METH_VARARGS|METH_KEYWORDS, "Account_ge"},

    {"get_prev", (PyCFunction)Account_prev, METH_VARARGS|METH_KEYWORDS, "Account_prev"},

    {"get_first", (PyCFunction)Account_first, METH_VARARGS|METH_KEYWORDS, "Account_first"},

    {"gets_gt", (PyCFunction)Account_iter_gt, METH_VARARGS|METH_KEYWORDS, "Account_iter_gt"},

    {"gets_last", (PyCFunction)Account_iter_last, METH_VARARGS|METH_KEYWORDS, "Account_iter_last"},

    {"gets_le", (PyCFunction)Account_iter_le, METH_VARARGS|METH_KEYWORDS, "Account_iter_le"},

    {"gets_ge", (PyCFunction)Account_iter_ge, METH_VARARGS|METH_KEYWORDS, "Account_iter_ge"},

    {"gets_equal", (PyCFunction)Account_iter_equal, METH_VARARGS|METH_KEYWORDS, "Account_iter_equal"},

    {"gets_lt", (PyCFunction)Account_iter_lt, METH_VARARGS|METH_KEYWORDS, "Account_iter_lt"},

    {"gets_first", (PyCFunction)Account_iter_first, METH_VARARGS|METH_KEYWORDS, "Account_iter_first"},

    {"insert", (PyCFunction)Account_insert, METH_VARARGS|METH_KEYWORDS, "Account_insert"},

    {"update", (PyCFunction)Account_update, METH_VARARGS|METH_KEYWORDS, "Account_update"},

    {"delete", (PyCFunction)Account_delete, METH_VARARGS|METH_KEYWORDS, "Account_delete"},

    {NULL}
};

static PyTypeObject AccountType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Account",             /*tp_name*/
    sizeof(Account),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Account_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Account_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Account objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Account_methods,             /* tp_methods */
    0,
    Account_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Account_init,      /* tp_init */
    0,                         /* tp_alloc */
    Account_new,                 /* tp_new */
};
PyTypeObject *getAccountType()
{
    return &AccountType_;
}

static void AccountKey0_dealloc(PyObject* self)
{
    do_free(((AccountKey0*)self)->priv);
    Py_DECREF(((AccountKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("AccountKey0 free\n");
}
static PyObject *AccountKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("AccountKey0 new\n");
    AccountKey0 *self;
    self = PyObject_NEW(AccountKey0, type);
    //self = (AccountKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(account_key0_t));
    return (PyObject *)self;
}
static int AccountKey0_init(AccountKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("AccountKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->account);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *AccountKey0_clone(AccountKey0 *self)
{
    AccountKey0 *result;
    result = (AccountKey0*)PyObject_CallFunction((PyObject*)getAccountKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *AccountKey0_account(AccountKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *AccountKey0_set_account(AccountKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set account");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->account, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->account);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *AccountKey0_total(AccountKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->itog);

    return result;
}

static PyObject *AccountKey0_set_total(AccountKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set itog");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             self->priv->itog = MyLong_AsLong(value);
        else
        if ( PyFloat_Check(value) )
             self->priv->itog = PyFloat_AsDouble(value);
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set itog");
            return NULL;
        }
    }
    //result = MyLong_FromLong(self->priv->itog);

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *AccountKey0_gt(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_last(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_next(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_le(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_lt(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_equal(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_ge(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_prev(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_first(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_account_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *AccountKey0_iter_gt(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    account_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_account_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = AccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_account_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *AccountKey0_iter_last(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    account_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_account_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = AccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_account_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *AccountKey0_iter_le(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    account_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_account_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = AccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_account_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *AccountKey0_iter_ge(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    account_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_account_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = AccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_account_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *AccountKey0_iter_equal(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    account_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_account_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = AccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_account_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *AccountKey0_iter_lt(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    account_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_account_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = AccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_account_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *AccountKey0_iter_first(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    account_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_account_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = AccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_account_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *AccountKey0_cmp(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getAccountKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->account, ((AccountKey0*)obj)->priv->account);
       }

       if ( !res && depth >= 2 ) {
           res = self->priv->itog - ((AccountKey0*)obj)->priv->itog;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *AccountKey0_set_alias(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((AccountKey0*)self)->alias);
    Py_INCREF(obj);
    ((AccountKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *AccountKey0_set(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getAccountKey0Type() ) {
        memcpy(self->priv, ((AccountKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getAccountType() ) {

        do_cpy(self->priv->account, ((Account*)obj)->priv->data.account);

        do_cpy(self->priv->itog, ((Account*)obj)->priv->data.itog);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_account", "O", value) )
                        return NULL;
                    break;

                case 1:
                    if ( !PyObject_CallMethod((PyObject*)self, "set_total", "O", value) )
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
            if ( !strcmp("account", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_account", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_total", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"AccountKey0\"does not contain \"%s\"", field_name);
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

static PyObject *AccountKey0_fields(AccountKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class AccountKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  AccountKey0_account(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("account", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  AccountKey0_total(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("total", value);
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

static PyGetSetDef AccountKey0_getset[] = {

    {"account",(getter)AccountKey0_account},

    {"total",(getter)AccountKey0_total},

    {NULL}
};

static PyMethodDef AccountKey0_methods[] = {
    {"set", (PyCFunction)AccountKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)AccountKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)AccountKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)AccountKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_account", (PyCFunction)AccountKey0_set_account, METH_VARARGS|METH_KEYWORDS, "AccountKey0_set_account"},

    {"set_total", (PyCFunction)AccountKey0_set_total, METH_VARARGS|METH_KEYWORDS, "AccountKey0_set_total"},

    {"get_gt", (PyCFunction)AccountKey0_gt, METH_VARARGS|METH_KEYWORDS, "AccountKey0_gt"},

    {"get_last", (PyCFunction)AccountKey0_last, METH_VARARGS|METH_KEYWORDS, "AccountKey0_last"},

    {"get_next", (PyCFunction)AccountKey0_next, METH_VARARGS|METH_KEYWORDS, "AccountKey0_next"},

    {"get_le", (PyCFunction)AccountKey0_le, METH_VARARGS|METH_KEYWORDS, "AccountKey0_le"},

    {"get_lt", (PyCFunction)AccountKey0_lt, METH_VARARGS|METH_KEYWORDS, "AccountKey0_lt"},

    {"get_equal", (PyCFunction)AccountKey0_equal, METH_VARARGS|METH_KEYWORDS, "AccountKey0_equal"},

    {"get_ge", (PyCFunction)AccountKey0_ge, METH_VARARGS|METH_KEYWORDS, "AccountKey0_ge"},

    {"get_prev", (PyCFunction)AccountKey0_prev, METH_VARARGS|METH_KEYWORDS, "AccountKey0_prev"},

    {"get_first", (PyCFunction)AccountKey0_first, METH_VARARGS|METH_KEYWORDS, "AccountKey0_first"},

    {"gets_gt", (PyCFunction)AccountKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "AccountKey0_iter_gt"},

    {"gets_last", (PyCFunction)AccountKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "AccountKey0_iter_last"},

    {"gets_le", (PyCFunction)AccountKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "AccountKey0_iter_le"},

    {"gets_ge", (PyCFunction)AccountKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "AccountKey0_iter_ge"},

    {"gets_equal", (PyCFunction)AccountKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "AccountKey0_iter_equal"},

    {"gets_lt", (PyCFunction)AccountKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "AccountKey0_iter_lt"},

    {"gets_first", (PyCFunction)AccountKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "AccountKey0_iter_first"},

    {NULL}
};

static PyTypeObject AccountKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.AccountKey0",             /*tp_name*/
    sizeof(AccountKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)AccountKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)AccountKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "AccountKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    AccountKey0_methods,             /* tp_methods */
    0,
    AccountKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)AccountKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    AccountKey0_new,                 /* tp_new */
};
PyTypeObject *getAccountKey0Type()
{
    return &AccountKey0Type_;
}
