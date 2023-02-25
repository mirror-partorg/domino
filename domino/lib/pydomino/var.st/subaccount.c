
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "subaccount.h"

static void Subaccount_dealloc(PyObject* self)
{
    do_free(((Subaccount*)self)->priv);
    Py_DECREF(((Subaccount*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Subaccount free\n");
}
static PyObject *Subaccount_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Subaccount new\n");
    Subaccount *self;
    self = PyObject_NEW(Subaccount, type);
    //self = (Subaccount *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(subaccount_rec_t));
    return (PyObject *)self;
}
static int Subaccount_init(Subaccount *self, PyObject *args, PyObject *kwds)
{
    //LOG("Subaccount init\n");
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

    do_text_set_empty(self->priv->data.subaccount);

    do_text_set_empty(self->priv->data.ticacc);

    do_text_set_empty(self->priv->data.name);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Subaccount_clone(Subaccount *self)
{
    Subaccount *result;
    result = (Subaccount*)PyObject_CallFunction((PyObject*)getSubaccountType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Subaccount_account(Subaccount* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Subaccount_set_account(Subaccount* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subaccount_subaccount(Subaccount* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.subaccount);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Subaccount_set_subaccount(Subaccount* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.subaccount");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.subaccount, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.subaccount);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Subaccount_ticacc(Subaccount* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.ticacc);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Subaccount_set_ticacc(Subaccount* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.ticacc");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.ticacc, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.ticacc);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Subaccount_name(Subaccount* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.name);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Subaccount_set_name(Subaccount* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subaccount_debit(Subaccount* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.debit);

    return result;
}

static PyObject *Subaccount_set_debit(Subaccount* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subaccount_credit(Subaccount* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.credit);

    return result;
}

static PyObject *Subaccount_set_credit(Subaccount* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subaccount_positive_saldo(Subaccount* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.positivesaldo);

    return result;
}

static PyObject *Subaccount_set_positive_saldo(Subaccount* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subaccount_total(Subaccount* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->data.itog);

    return result;
}

static PyObject *Subaccount_set_total(Subaccount* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subaccount_sum_include(Subaccount* self, void *unused)
{
    PyObject *result;

    result = PyTuple_New(3);
    PyTuple_SET_ITEM(result, 0, MyLong_FromLong(self->priv->data.sumincl[0]));
    PyTuple_SET_ITEM(result, 1, MyLong_FromLong(self->priv->data.sumincl[1]));
    PyTuple_SET_ITEM(result, 2, MyLong_FromLong(self->priv->data.sumincl[2]));

    return result;
}

static PyObject *Subaccount_set_sum_include(Subaccount* self, PyObject *args, PyObject *kwds)
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

static PyObject *Subaccount_prev(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_gt(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_next(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_ge(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_equal(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_last(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_lt(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_le(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_first(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getSubaccountKey0Type() )
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_iter_gt(Subaccount* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
        key_cmp = (subaccount_key0_t*)do_malloc(sizeof(subaccount_key0_t));
        memcpy(key_cmp, ((SubaccountKey0*)key)->priv, sizeof(subaccount_key0_t));
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->account, 
                    ((SubaccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->subaccount, 
                    ((SubaccountKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->itog, 
                    ((SubaccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subaccount_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
            status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Subaccount_iter_ge(Subaccount* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
        key_cmp = (subaccount_key0_t*)do_malloc(sizeof(subaccount_key0_t));
        memcpy(key_cmp, ((SubaccountKey0*)key)->priv, sizeof(subaccount_key0_t));
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->account, 
                    ((SubaccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->subaccount, 
                    ((SubaccountKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->itog, 
                    ((SubaccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subaccount_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
            status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Subaccount_iter_equal(Subaccount* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
        key_cmp = (subaccount_key0_t*)do_malloc(sizeof(subaccount_key0_t));
        memcpy(key_cmp, ((SubaccountKey0*)key)->priv, sizeof(subaccount_key0_t));
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->account, 
                    ((SubaccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->subaccount, 
                    ((SubaccountKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->itog, 
                    ((SubaccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subaccount_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
            status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Subaccount_iter_last(Subaccount* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
        key_cmp = (subaccount_key0_t*)do_malloc(sizeof(subaccount_key0_t));
        memcpy(key_cmp, ((SubaccountKey0*)key)->priv, sizeof(subaccount_key0_t));
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->account, 
                    ((SubaccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->subaccount, 
                    ((SubaccountKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->itog, 
                    ((SubaccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subaccount_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
            status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Subaccount_iter_lt(Subaccount* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
        key_cmp = (subaccount_key0_t*)do_malloc(sizeof(subaccount_key0_t));
        memcpy(key_cmp, ((SubaccountKey0*)key)->priv, sizeof(subaccount_key0_t));
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->account, 
                    ((SubaccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->subaccount, 
                    ((SubaccountKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->itog, 
                    ((SubaccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subaccount_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
            status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Subaccount_iter_le(Subaccount* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
        key_cmp = (subaccount_key0_t*)do_malloc(sizeof(subaccount_key0_t));
        memcpy(key_cmp, ((SubaccountKey0*)key)->priv, sizeof(subaccount_key0_t));
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->account, 
                    ((SubaccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->subaccount, 
                    ((SubaccountKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->itog, 
                    ((SubaccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subaccount_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
            status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Subaccount_iter_first(Subaccount* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
        key_cmp = (subaccount_key0_t*)do_malloc(sizeof(subaccount_key0_t));
        memcpy(key_cmp, ((SubaccountKey0*)key)->priv, sizeof(subaccount_key0_t));
        status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->account, 
                    ((SubaccountKey0*)key)->priv->account))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->subaccount, 
                    ((SubaccountKey0*)key)->priv->subaccount))
                   break;
            }
       
            if ( depth >= 3 ) {
                if ( do_cmp(((subaccount_key0_t*)key_cmp)->itog, 
                    ((SubaccountKey0*)key)->priv->itog))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Subaccount_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getSubaccountKey0Type() ) {
            status = do_subaccount_get0(self->alias->alias, self->priv, ((SubaccountKey0*)key)->priv, DO_GET_NEXT);
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

static PyObject *Subaccount_update(Subaccount* self)
{
    int status;
    status = do_subaccount_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_insert(Subaccount* self)
{
    int status;
    status = do_subaccount_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_delete(Subaccount* self)
{
    int status;
    status = do_subaccount_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Subaccount_set_alias(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Subaccount*)self)->alias);
    Py_INCREF(obj);
    ((Subaccount*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Subaccount_set(Subaccount* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getSubaccountType() ) {
        memcpy(self->priv, ((Subaccount*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getSubaccountKey0Type() ) {

        do_cpy(self->priv->data.account, ((SubaccountKey0*)obj)->priv->account);

        do_cpy(self->priv->data.subaccount, ((SubaccountKey0*)obj)->priv->subaccount);

        do_cpy(self->priv->data.itog, ((SubaccountKey0*)obj)->priv->itog);

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
            if ( !strcmp("subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("ticacc", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_ticacc", "O", value) )
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
            if ( !strcmp("sum_include", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_sum_include", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Subaccount\"does not contain \"%s\"", field_name);
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

static PyObject *Subaccount_fields(Subaccount* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Subaccount");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Subaccount_account(self,NULL);
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

    value =  Subaccount_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Subaccount_ticacc(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("ticacc", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  Subaccount_name(self,NULL);
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

    value =  Subaccount_debit(self,NULL);
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

    value =  Subaccount_credit(self,NULL);
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

    value =  Subaccount_positive_saldo(self,NULL);
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

    value =  Subaccount_total(self,NULL);
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

static PyGetSetDef Subaccount_getset[] = {

    {"account",(getter)Subaccount_account},

    {"subaccount",(getter)Subaccount_subaccount},

    {"ticacc",(getter)Subaccount_ticacc},

    {"name",(getter)Subaccount_name},

    {"debit",(getter)Subaccount_debit},

    {"credit",(getter)Subaccount_credit},

    {"positive_saldo",(getter)Subaccount_positive_saldo},

    {"total",(getter)Subaccount_total},

    {"sum_include",(getter)Subaccount_sum_include},

    {NULL}
};

static PyMethodDef Subaccount_methods[] = {
    {"set", (PyCFunction)Subaccount_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Subaccount_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Subaccount_clone, METH_NOARGS, "clone"},

    {"set_account", (PyCFunction)Subaccount_set_account, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_account"},

    {"set_subaccount", (PyCFunction)Subaccount_set_subaccount, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_subaccount"},

    {"set_ticacc", (PyCFunction)Subaccount_set_ticacc, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_ticacc"},

    {"set_name", (PyCFunction)Subaccount_set_name, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_name"},

    {"set_debit", (PyCFunction)Subaccount_set_debit, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_debit"},

    {"set_credit", (PyCFunction)Subaccount_set_credit, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_credit"},

    {"set_positive_saldo", (PyCFunction)Subaccount_set_positive_saldo, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_positive_saldo"},

    {"set_total", (PyCFunction)Subaccount_set_total, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_total"},

    {"set_sum_include", (PyCFunction)Subaccount_set_sum_include, METH_VARARGS|METH_KEYWORDS, "Subaccount_set_sum_include"},

    {"get_prev", (PyCFunction)Subaccount_prev, METH_VARARGS|METH_KEYWORDS, "Subaccount_prev"},

    {"get_gt", (PyCFunction)Subaccount_gt, METH_VARARGS|METH_KEYWORDS, "Subaccount_gt"},

    {"get_next", (PyCFunction)Subaccount_next, METH_VARARGS|METH_KEYWORDS, "Subaccount_next"},

    {"get_ge", (PyCFunction)Subaccount_ge, METH_VARARGS|METH_KEYWORDS, "Subaccount_ge"},

    {"get_equal", (PyCFunction)Subaccount_equal, METH_VARARGS|METH_KEYWORDS, "Subaccount_equal"},

    {"get_last", (PyCFunction)Subaccount_last, METH_VARARGS|METH_KEYWORDS, "Subaccount_last"},

    {"get_lt", (PyCFunction)Subaccount_lt, METH_VARARGS|METH_KEYWORDS, "Subaccount_lt"},

    {"get_le", (PyCFunction)Subaccount_le, METH_VARARGS|METH_KEYWORDS, "Subaccount_le"},

    {"get_first", (PyCFunction)Subaccount_first, METH_VARARGS|METH_KEYWORDS, "Subaccount_first"},

    {"gets_gt", (PyCFunction)Subaccount_iter_gt, METH_VARARGS|METH_KEYWORDS, "Subaccount_iter_gt"},

    {"gets_ge", (PyCFunction)Subaccount_iter_ge, METH_VARARGS|METH_KEYWORDS, "Subaccount_iter_ge"},

    {"gets_equal", (PyCFunction)Subaccount_iter_equal, METH_VARARGS|METH_KEYWORDS, "Subaccount_iter_equal"},

    {"gets_last", (PyCFunction)Subaccount_iter_last, METH_VARARGS|METH_KEYWORDS, "Subaccount_iter_last"},

    {"gets_lt", (PyCFunction)Subaccount_iter_lt, METH_VARARGS|METH_KEYWORDS, "Subaccount_iter_lt"},

    {"gets_le", (PyCFunction)Subaccount_iter_le, METH_VARARGS|METH_KEYWORDS, "Subaccount_iter_le"},

    {"gets_first", (PyCFunction)Subaccount_iter_first, METH_VARARGS|METH_KEYWORDS, "Subaccount_iter_first"},

    {"update", (PyCFunction)Subaccount_update, METH_VARARGS|METH_KEYWORDS, "Subaccount_update"},

    {"insert", (PyCFunction)Subaccount_insert, METH_VARARGS|METH_KEYWORDS, "Subaccount_insert"},

    {"delete", (PyCFunction)Subaccount_delete, METH_VARARGS|METH_KEYWORDS, "Subaccount_delete"},

    {NULL}
};

static PyTypeObject SubaccountType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Subaccount",             /*tp_name*/
    sizeof(Subaccount),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Subaccount_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Subaccount_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Subaccount objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Subaccount_methods,             /* tp_methods */
    0,
    Subaccount_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Subaccount_init,      /* tp_init */
    0,                         /* tp_alloc */
    Subaccount_new,                 /* tp_new */
};
PyTypeObject *getSubaccountType()
{
    return &SubaccountType_;
}

static void SubaccountKey0_dealloc(PyObject* self)
{
    do_free(((SubaccountKey0*)self)->priv);
    Py_DECREF(((SubaccountKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("SubaccountKey0 free\n");
}
static PyObject *SubaccountKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("SubaccountKey0 new\n");
    SubaccountKey0 *self;
    self = PyObject_NEW(SubaccountKey0, type);
    //self = (SubaccountKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(subaccount_key0_t));
    return (PyObject *)self;
}
static int SubaccountKey0_init(SubaccountKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("SubaccountKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->account);

    do_text_set_empty(self->priv->subaccount);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *SubaccountKey0_clone(SubaccountKey0 *self)
{
    SubaccountKey0 *result;
    result = (SubaccountKey0*)PyObject_CallFunction((PyObject*)getSubaccountKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *SubaccountKey0_account(SubaccountKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->account);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SubaccountKey0_set_account(SubaccountKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *SubaccountKey0_subaccount(SubaccountKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->subaccount);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *SubaccountKey0_set_subaccount(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set subaccount");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->subaccount, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->subaccount);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *SubaccountKey0_total(SubaccountKey0* self, void *unused)
{
    PyObject *result;

    result = MyLong_FromLong(self->priv->itog);

    return result;
}

static PyObject *SubaccountKey0_set_total(SubaccountKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *SubaccountKey0_prev(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_gt(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_next(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_ge(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_equal(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_last(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_lt(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_le(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_first(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *SubaccountKey0_iter_gt(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subaccount_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = SubaccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubaccountKey0_iter_ge(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subaccount_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = SubaccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubaccountKey0_iter_equal(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subaccount_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = SubaccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubaccountKey0_iter_last(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subaccount_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = SubaccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubaccountKey0_iter_lt(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subaccount_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = SubaccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubaccountKey0_iter_le(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subaccount_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = SubaccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubaccountKey0_iter_first(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    subaccount_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.account, 
                 self->priv->account))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.subaccount, 
                 self->priv->subaccount))
               break;
       }

       if ( depth >= 3 ) {
           if ( do_cmp(key_cmp.itog, 
                 self->priv->itog))
               break;
       }

 
        item = SubaccountKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_subaccount_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *SubaccountKey0_cmp(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getSubaccountKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->account, ((SubaccountKey0*)obj)->priv->account);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->subaccount, ((SubaccountKey0*)obj)->priv->subaccount);
       }

       if ( !res && depth >= 3 ) {
           res = self->priv->itog - ((SubaccountKey0*)obj)->priv->itog;
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *SubaccountKey0_set_alias(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((SubaccountKey0*)self)->alias);
    Py_INCREF(obj);
    ((SubaccountKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *SubaccountKey0_set(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getSubaccountKey0Type() ) {
        memcpy(self->priv, ((SubaccountKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getSubaccountType() ) {

        do_cpy(self->priv->account, ((Subaccount*)obj)->priv->data.account);

        do_cpy(self->priv->subaccount, ((Subaccount*)obj)->priv->data.subaccount);

        do_cpy(self->priv->itog, ((Subaccount*)obj)->priv->data.itog);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount", "O", value) )
                        return NULL;
                    break;

                case 2:
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
            if ( !strcmp("subaccount", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_subaccount", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("total", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_total", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"SubaccountKey0\"does not contain \"%s\"", field_name);
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

static PyObject *SubaccountKey0_fields(SubaccountKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class SubaccountKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  SubaccountKey0_account(self,NULL);
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

    value =  SubaccountKey0_subaccount(self,NULL);
    if ( !value ) {
       retval = MyString_FromString(str);
       do_free(str);
       return retval;
    }
    buf = get_desc("subaccount", value);
    buf1 = do_strdup_printf("%s\n%s",str,buf);
    do_free(buf);
    do_free(str);
    str = buf1;

    value =  SubaccountKey0_total(self,NULL);
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

static PyGetSetDef SubaccountKey0_getset[] = {

    {"account",(getter)SubaccountKey0_account},

    {"subaccount",(getter)SubaccountKey0_subaccount},

    {"total",(getter)SubaccountKey0_total},

    {NULL}
};

static PyMethodDef SubaccountKey0_methods[] = {
    {"set", (PyCFunction)SubaccountKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)SubaccountKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)SubaccountKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)SubaccountKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_account", (PyCFunction)SubaccountKey0_set_account, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_set_account"},

    {"set_subaccount", (PyCFunction)SubaccountKey0_set_subaccount, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_set_subaccount"},

    {"set_total", (PyCFunction)SubaccountKey0_set_total, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_set_total"},

    {"get_prev", (PyCFunction)SubaccountKey0_prev, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_prev"},

    {"get_gt", (PyCFunction)SubaccountKey0_gt, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_gt"},

    {"get_next", (PyCFunction)SubaccountKey0_next, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_next"},

    {"get_ge", (PyCFunction)SubaccountKey0_ge, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_ge"},

    {"get_equal", (PyCFunction)SubaccountKey0_equal, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_equal"},

    {"get_last", (PyCFunction)SubaccountKey0_last, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_last"},

    {"get_lt", (PyCFunction)SubaccountKey0_lt, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_lt"},

    {"get_le", (PyCFunction)SubaccountKey0_le, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_le"},

    {"get_first", (PyCFunction)SubaccountKey0_first, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_first"},

    {"gets_gt", (PyCFunction)SubaccountKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_iter_gt"},

    {"gets_ge", (PyCFunction)SubaccountKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_iter_ge"},

    {"gets_equal", (PyCFunction)SubaccountKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_iter_equal"},

    {"gets_last", (PyCFunction)SubaccountKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_iter_last"},

    {"gets_lt", (PyCFunction)SubaccountKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_iter_lt"},

    {"gets_le", (PyCFunction)SubaccountKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_iter_le"},

    {"gets_first", (PyCFunction)SubaccountKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "SubaccountKey0_iter_first"},

    {NULL}
};

static PyTypeObject SubaccountKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.SubaccountKey0",             /*tp_name*/
    sizeof(SubaccountKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)SubaccountKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)SubaccountKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "SubaccountKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    SubaccountKey0_methods,             /* tp_methods */
    0,
    SubaccountKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)SubaccountKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    SubaccountKey0_new,                 /* tp_new */
};
PyTypeObject *getSubaccountKey0Type()
{
    return &SubaccountKey0Type_;
}
