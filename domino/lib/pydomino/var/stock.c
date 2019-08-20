
#include "db.h"
#include "util.h"
#include <datetime.h>
#include "stock.h"

static void Stock_dealloc(PyObject* self)
{
    do_free(((Stock*)self)->priv);
    Py_DECREF(((Stock*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("Stock free\n");
}
static PyObject *Stock_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("Stock new\n");
    Stock *self;
    self = PyObject_NEW(Stock, type);
    //self = (Stock *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(stock_rec_t));
    return (PyObject *)self;
}
static int Stock_init(Stock *self, PyObject *args, PyObject *kwds)
{
    //LOG("Stock init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    self->priv->size=sizeof(self->priv->data);

    do_text_set_empty(self->priv->data.sklad);

    do_text_set_empty(self->priv->data.code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *Stock_clone(Stock *self)
{
    Stock *result;
    result = (Stock*)PyObject_CallFunction((PyObject*)getStockType(),
                                    "O", self->alias);
    result->priv->size = self->priv->size;
    do_cpy(result->priv->data, self->priv->data);
    return (PyObject*)result;
}

static PyObject *Stock_store(Stock* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Stock_set_store(Stock* self, PyObject *args, PyObject *kwds)
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

static PyObject *Stock_code(Stock* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *Stock_set_code(Stock* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set data.code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->data.code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->data.code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *Stock_get_quant(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *result;

    static char *kwlist[] = {"number", NULL};
    int number;
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &number) ) {
        do_log(LOG_ERR, "Invalid argument \"number\": wrong type. get quant");
        return NULL;
    }
    double quant;
    quant = do_stock_quant(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(quant);

    return result;
}

static PyObject *Stock_set_quant(Stock* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    PyObject *value=NULL;
    int number;
    static char *kwlist[] = {"number", "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|O", kwlist, &number, &value) ) {
        do_log(LOG_ERR, "Invalid arguments \"number\" \"value\": wrong type. set quant");
        return NULL;
    }
    if ( value ) {
        if ( MyLong_Check(value) )
             do_stock_quant_set(self->alias->alias,self->priv, number, MyLong_AsLong(value));
        else
        if ( PyFloat_Check(value) )
             do_stock_quant_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(value));
        else {
            do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set quant");
            return NULL;
        }    
    }
    /*double quant;
    quant = do_stock_quant(self->alias->alias, self->priv, number);
    result = PyFloat_FromDouble(quant);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}


static PyObject *Stock_quants_clear(Stock* self)
{
    do_stock_quant_clear(self->alias->alias,self->priv);
    Py_INCREF(Py_None);
    return Py_None;
}
    
static PyObject *Stock_get_quants(Stock* self, void *unused)
{   
    PyObject *result;
    PyObject *val, *key;
    result = PyDict_New();
    int i;
    int number;
    double quant;

    for ( i = 0; i < 6; i++ ) {
        number = self->priv->data.nquant[i];
        if ( !number ) break;
        quant = do_stock_quant(self->alias->alias, self->priv, number);
        val = PyFloat_FromDouble(quant);
        key = MyLong_FromLong(number);
        PyDict_SetItem(result, key, val);
    }
    return result;
}
static PyObject *Stock_set_quants(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *value = NULL;
    static char *kwlist[] = { "value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &value) )
        return NULL;
    if ( PyDict_Check(value) ) {
        do_stock_quant_clear(self->alias->alias,self->priv);
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
                 do_stock_quant_set(self->alias->alias,self->priv, number, MyLong_AsLong(pValue));
             else
             if ( PyFloat_Check(pValue) )
                 do_stock_quant_set(self->alias->alias,self->priv, number, PyFloat_AsDouble(pValue));
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

static PyObject *Stock_gt(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_GT);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_GT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_last(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_LAST);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_LAST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_next(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_NEXT);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_NEXT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_le(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_LE);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_LE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_lt(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_LT);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_LT);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_equal(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_EQUAL);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_EQUAL);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_ge(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_GE);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_GE);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_prev(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_PREVIOUS);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_PREVIOUS);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_first(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *key;

    static char *kwlist[] = {"key", NULL};
    int status;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &key) )
        return NULL;

    if ( Py_TYPE(key) == getStockKey0Type() )
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_FIRST);
    else

    if ( Py_TYPE(key) == getStockKey1Type() )
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_FIRST);
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_iter_gt(Stock* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStockKey0Type() ) {
        key_cmp = (stock_key0_t*)do_malloc(sizeof(stock_key0_t));
        memcpy(key_cmp, ((StockKey0*)key)->priv, sizeof(stock_key0_t));
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_GT);
    }
    else

    if ( Py_TYPE(key) == getStockKey1Type() ) {
        key_cmp = (stock_key1_t*)do_malloc(sizeof(stock_key1_t));
        memcpy(key_cmp, ((StockKey1*)key)->priv, sizeof(stock_key1_t));
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_GT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStockKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->sklad, 
                    ((StockKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->code, 
                    ((StockKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key1_t*)key_cmp)->code, 
                    ((StockKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Stock_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStockKey0Type() ) {
            status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
            status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Stock_iter_last(Stock* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStockKey0Type() ) {
        key_cmp = (stock_key0_t*)do_malloc(sizeof(stock_key0_t));
        memcpy(key_cmp, ((StockKey0*)key)->priv, sizeof(stock_key0_t));
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_LAST);
    }
    else

    if ( Py_TYPE(key) == getStockKey1Type() ) {
        key_cmp = (stock_key1_t*)do_malloc(sizeof(stock_key1_t));
        memcpy(key_cmp, ((StockKey1*)key)->priv, sizeof(stock_key1_t));
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_LAST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStockKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->sklad, 
                    ((StockKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->code, 
                    ((StockKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key1_t*)key_cmp)->code, 
                    ((StockKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Stock_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStockKey0Type() ) {
            status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
            status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Stock_iter_le(Stock* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStockKey0Type() ) {
        key_cmp = (stock_key0_t*)do_malloc(sizeof(stock_key0_t));
        memcpy(key_cmp, ((StockKey0*)key)->priv, sizeof(stock_key0_t));
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_LE);
    }
    else

    if ( Py_TYPE(key) == getStockKey1Type() ) {
        key_cmp = (stock_key1_t*)do_malloc(sizeof(stock_key1_t));
        memcpy(key_cmp, ((StockKey1*)key)->priv, sizeof(stock_key1_t));
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_LE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStockKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->sklad, 
                    ((StockKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->code, 
                    ((StockKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key1_t*)key_cmp)->code, 
                    ((StockKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Stock_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStockKey0Type() ) {
            status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
            status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Stock_iter_ge(Stock* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStockKey0Type() ) {
        key_cmp = (stock_key0_t*)do_malloc(sizeof(stock_key0_t));
        memcpy(key_cmp, ((StockKey0*)key)->priv, sizeof(stock_key0_t));
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_GE);
    }
    else

    if ( Py_TYPE(key) == getStockKey1Type() ) {
        key_cmp = (stock_key1_t*)do_malloc(sizeof(stock_key1_t));
        memcpy(key_cmp, ((StockKey1*)key)->priv, sizeof(stock_key1_t));
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_GE);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStockKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->sklad, 
                    ((StockKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->code, 
                    ((StockKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key1_t*)key_cmp)->code, 
                    ((StockKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Stock_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStockKey0Type() ) {
            status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
            status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Stock_iter_equal(Stock* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStockKey0Type() ) {
        key_cmp = (stock_key0_t*)do_malloc(sizeof(stock_key0_t));
        memcpy(key_cmp, ((StockKey0*)key)->priv, sizeof(stock_key0_t));
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_EQUAL);
    }
    else

    if ( Py_TYPE(key) == getStockKey1Type() ) {
        key_cmp = (stock_key1_t*)do_malloc(sizeof(stock_key1_t));
        memcpy(key_cmp, ((StockKey1*)key)->priv, sizeof(stock_key1_t));
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_EQUAL);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStockKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->sklad, 
                    ((StockKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->code, 
                    ((StockKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key1_t*)key_cmp)->code, 
                    ((StockKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Stock_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStockKey0Type() ) {
            status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
            status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Stock_iter_lt(Stock* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStockKey0Type() ) {
        key_cmp = (stock_key0_t*)do_malloc(sizeof(stock_key0_t));
        memcpy(key_cmp, ((StockKey0*)key)->priv, sizeof(stock_key0_t));
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_LT);
    }
    else

    if ( Py_TYPE(key) == getStockKey1Type() ) {
        key_cmp = (stock_key1_t*)do_malloc(sizeof(stock_key1_t));
        memcpy(key_cmp, ((StockKey1*)key)->priv, sizeof(stock_key1_t));
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_LT);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStockKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->sklad, 
                    ((StockKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->code, 
                    ((StockKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key1_t*)key_cmp)->code, 
                    ((StockKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Stock_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStockKey0Type() ) {
            status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_PREVIOUS);
        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
            status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_PREVIOUS);
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

static PyObject *Stock_iter_first(Stock* self, PyObject *args, PyObject *kwds)
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

    if ( Py_TYPE(key) == getStockKey0Type() ) {
        key_cmp = (stock_key0_t*)do_malloc(sizeof(stock_key0_t));
        memcpy(key_cmp, ((StockKey0*)key)->priv, sizeof(stock_key0_t));
        status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_FIRST);
    }
    else

    if ( Py_TYPE(key) == getStockKey1Type() ) {
        key_cmp = (stock_key1_t*)do_malloc(sizeof(stock_key1_t));
        memcpy(key_cmp, ((StockKey1*)key)->priv, sizeof(stock_key1_t));
        status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_FIRST);
    }
    else
    
    {
        do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
        return NULL;
    }

    while ( status == DO_OK ) {

        if ( Py_TYPE(key) == getStockKey0Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->sklad, 
                    ((StockKey0*)key)->priv->sklad))
                   break;
            }
       
            if ( depth >= 2 ) {
                if ( do_cmp(((stock_key0_t*)key_cmp)->code, 
                    ((StockKey0*)key)->priv->code))
                   break;
            }

        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
       
            if ( depth >= 1 ) {
                if ( do_cmp(((stock_key1_t*)key_cmp)->code, 
                    ((StockKey1*)key)->priv->code))
                   break;
            }

        }
        else
    
        {
            do_log(LOG_ERR, "Invalid argument \"key\": wrong type");
            return NULL;
        }

     item = Stock_clone(self);
     PyList_Append(retval, (PyObject*)item);
     Py_DECREF(item);        
     

        if ( Py_TYPE(key) == getStockKey0Type() ) {
            status = do_stock_get0(self->alias->alias, self->priv, ((StockKey0*)key)->priv, DO_GET_NEXT);
        }
        else

        if ( Py_TYPE(key) == getStockKey1Type() ) {
            status = do_stock_get1(self->alias->alias, self->priv, ((StockKey1*)key)->priv, DO_GET_NEXT);
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

static PyObject *Stock_insert(Stock* self)
{
    int status;
    status = do_stock_insert(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_update(Stock* self)
{
    int status;
    status = do_stock_update(self->alias->alias, self->priv);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_delete(Stock* self)
{
    int status;
    status = do_stock_delete(self->alias->alias);
    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *Stock_set_alias(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((Stock*)self)->alias);
    Py_INCREF(obj);
    ((Stock*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *Stock_set(Stock* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getStockType() ) {
        memcpy(self->priv, ((Stock*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getStockKey0Type() ) {

        do_cpy(self->priv->data.sklad, ((StockKey0*)obj)->priv->sklad);

        do_cpy(self->priv->data.code, ((StockKey0*)obj)->priv->code);

    }

    else 
    if ( Py_TYPE(obj) == getStockKey1Type() ) {

        do_cpy(self->priv->data.code, ((StockKey1*)obj)->priv->code);

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
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("quant", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_quant", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("quants", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_quants", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"Stock\"does not contain \"%s\"", field_name);
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

static PyObject *Stock_fields(Stock* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class Stock");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  Stock_store(self,NULL);
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

    value =  Stock_code(self,NULL);
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

    value = Stock_get_quants(self, NULL);
    buf = get_quants_desc(value, "quants");
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

static PyGetSetDef Stock_getset[] = {

    {"store",(getter)Stock_store},

    {"code",(getter)Stock_code},

    {"quants",(getter)Stock_get_quants},

    {NULL}
};

static PyMethodDef Stock_methods[] = {
    {"set", (PyCFunction)Stock_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)Stock_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)Stock_clone, METH_NOARGS, "clone"},

    {"clear_quants", (PyCFunction)Stock_quants_clear, METH_NOARGS, "do_Stock_quants_clear"},
    {"quant", (PyCFunction)Stock_get_quant, METH_VARARGS|METH_KEYWORDS, "do_Stock_quant"},
    {"set_quants", (PyCFunction)Stock_set_quants, METH_VARARGS|METH_KEYWORDS, "do_Stock_set_quants"},

    {"set_store", (PyCFunction)Stock_set_store, METH_VARARGS|METH_KEYWORDS, "Stock_set_store"},

    {"set_code", (PyCFunction)Stock_set_code, METH_VARARGS|METH_KEYWORDS, "Stock_set_code"},

    {"set_quant", (PyCFunction)Stock_set_quant, METH_VARARGS|METH_KEYWORDS, "Stock_set_quant"},

    {"get_gt", (PyCFunction)Stock_gt, METH_VARARGS|METH_KEYWORDS, "Stock_gt"},

    {"get_last", (PyCFunction)Stock_last, METH_VARARGS|METH_KEYWORDS, "Stock_last"},

    {"get_next", (PyCFunction)Stock_next, METH_VARARGS|METH_KEYWORDS, "Stock_next"},

    {"get_le", (PyCFunction)Stock_le, METH_VARARGS|METH_KEYWORDS, "Stock_le"},

    {"get_lt", (PyCFunction)Stock_lt, METH_VARARGS|METH_KEYWORDS, "Stock_lt"},

    {"get_equal", (PyCFunction)Stock_equal, METH_VARARGS|METH_KEYWORDS, "Stock_equal"},

    {"get_ge", (PyCFunction)Stock_ge, METH_VARARGS|METH_KEYWORDS, "Stock_ge"},

    {"get_prev", (PyCFunction)Stock_prev, METH_VARARGS|METH_KEYWORDS, "Stock_prev"},

    {"get_first", (PyCFunction)Stock_first, METH_VARARGS|METH_KEYWORDS, "Stock_first"},

    {"gets_gt", (PyCFunction)Stock_iter_gt, METH_VARARGS|METH_KEYWORDS, "Stock_iter_gt"},

    {"gets_last", (PyCFunction)Stock_iter_last, METH_VARARGS|METH_KEYWORDS, "Stock_iter_last"},

    {"gets_le", (PyCFunction)Stock_iter_le, METH_VARARGS|METH_KEYWORDS, "Stock_iter_le"},

    {"gets_ge", (PyCFunction)Stock_iter_ge, METH_VARARGS|METH_KEYWORDS, "Stock_iter_ge"},

    {"gets_equal", (PyCFunction)Stock_iter_equal, METH_VARARGS|METH_KEYWORDS, "Stock_iter_equal"},

    {"gets_lt", (PyCFunction)Stock_iter_lt, METH_VARARGS|METH_KEYWORDS, "Stock_iter_lt"},

    {"gets_first", (PyCFunction)Stock_iter_first, METH_VARARGS|METH_KEYWORDS, "Stock_iter_first"},

    {"insert", (PyCFunction)Stock_insert, METH_VARARGS|METH_KEYWORDS, "Stock_insert"},

    {"update", (PyCFunction)Stock_update, METH_VARARGS|METH_KEYWORDS, "Stock_update"},

    {"delete", (PyCFunction)Stock_delete, METH_VARARGS|METH_KEYWORDS, "Stock_delete"},

    {NULL}
};

static PyTypeObject StockType_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.Stock",             /*tp_name*/
    sizeof(Stock),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Stock_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)Stock_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Stock objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    Stock_methods,             /* tp_methods */
    0,
    Stock_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)Stock_init,      /* tp_init */
    0,                         /* tp_alloc */
    Stock_new,                 /* tp_new */
};
PyTypeObject *getStockType()
{
    return &StockType_;
}

static void StockKey0_dealloc(PyObject* self)
{
    do_free(((StockKey0*)self)->priv);
    Py_DECREF(((StockKey0*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("StockKey0 free\n");
}
static PyObject *StockKey0_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("StockKey0 new\n");
    StockKey0 *self;
    self = PyObject_NEW(StockKey0, type);
    //self = (StockKey0 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(stock_key0_t));
    return (PyObject *)self;
}
static int StockKey0_init(StockKey0 *self, PyObject *args, PyObject *kwds)
{
    //LOG("StockKey0 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->sklad);

    do_text_set_empty(self->priv->code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *StockKey0_clone(StockKey0 *self)
{
    StockKey0 *result;
    result = (StockKey0*)PyObject_CallFunction((PyObject*)getStockKey0Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *StockKey0_store(StockKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->sklad);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *StockKey0_set_store(StockKey0* self, PyObject *args, PyObject *kwds)
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

static PyObject *StockKey0_code(StockKey0* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *StockKey0_set_code(StockKey0* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *StockKey0_gt(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_last(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_next(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_le(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_lt(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_equal(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_ge(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_prev(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_first(StockKey0* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey0_iter_gt(StockKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey0_iter_last(StockKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey0_iter_le(StockKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey0_iter_ge(StockKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey0_iter_equal(StockKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey0_iter_lt(StockKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key0(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey0_iter_first(StockKey0* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key0_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key0(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.sklad, 
                 self->priv->sklad))
               break;
       }

       if ( depth >= 2 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey0_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key0(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey0_cmp(StockKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getStockKey0Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->sklad, ((StockKey0*)obj)->priv->sklad);
       }

       if ( !res && depth >= 2 ) {
           res = do_cmp(self->priv->code, ((StockKey0*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *StockKey0_set_alias(StockKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((StockKey0*)self)->alias);
    Py_INCREF(obj);
    ((StockKey0*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *StockKey0_set(StockKey0* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getStockKey0Type() ) {
        memcpy(self->priv, ((StockKey0*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getStockType() ) {

        do_cpy(self->priv->sklad, ((Stock*)obj)->priv->data.sklad);

        do_cpy(self->priv->code, ((Stock*)obj)->priv->data.code);

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
                    if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
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
            if ( !strcmp("store", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_store", "O", value) )
                    return NULL;
            }

            else
            if ( !strcmp("code", field_name) ) {
                if ( !PyObject_CallMethod((PyObject*)self, "set_code", "O", value) )
                    return NULL;
            }

            else {
                do_log(LOG_ERR, "Class \"StockKey0\"does not contain \"%s\"", field_name);
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

static PyObject *StockKey0_fields(StockKey0* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class StockKey0");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  StockKey0_store(self,NULL);
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

    value =  StockKey0_code(self,NULL);
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

static PyGetSetDef StockKey0_getset[] = {

    {"store",(getter)StockKey0_store},

    {"code",(getter)StockKey0_code},

    {NULL}
};

static PyMethodDef StockKey0_methods[] = {
    {"set", (PyCFunction)StockKey0_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)StockKey0_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)StockKey0_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)StockKey0_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_store", (PyCFunction)StockKey0_set_store, METH_VARARGS|METH_KEYWORDS, "StockKey0_set_store"},

    {"set_code", (PyCFunction)StockKey0_set_code, METH_VARARGS|METH_KEYWORDS, "StockKey0_set_code"},

    {"get_gt", (PyCFunction)StockKey0_gt, METH_VARARGS|METH_KEYWORDS, "StockKey0_gt"},

    {"get_last", (PyCFunction)StockKey0_last, METH_VARARGS|METH_KEYWORDS, "StockKey0_last"},

    {"get_next", (PyCFunction)StockKey0_next, METH_VARARGS|METH_KEYWORDS, "StockKey0_next"},

    {"get_le", (PyCFunction)StockKey0_le, METH_VARARGS|METH_KEYWORDS, "StockKey0_le"},

    {"get_lt", (PyCFunction)StockKey0_lt, METH_VARARGS|METH_KEYWORDS, "StockKey0_lt"},

    {"get_equal", (PyCFunction)StockKey0_equal, METH_VARARGS|METH_KEYWORDS, "StockKey0_equal"},

    {"get_ge", (PyCFunction)StockKey0_ge, METH_VARARGS|METH_KEYWORDS, "StockKey0_ge"},

    {"get_prev", (PyCFunction)StockKey0_prev, METH_VARARGS|METH_KEYWORDS, "StockKey0_prev"},

    {"get_first", (PyCFunction)StockKey0_first, METH_VARARGS|METH_KEYWORDS, "StockKey0_first"},

    {"gets_gt", (PyCFunction)StockKey0_iter_gt, METH_VARARGS|METH_KEYWORDS, "StockKey0_iter_gt"},

    {"gets_last", (PyCFunction)StockKey0_iter_last, METH_VARARGS|METH_KEYWORDS, "StockKey0_iter_last"},

    {"gets_le", (PyCFunction)StockKey0_iter_le, METH_VARARGS|METH_KEYWORDS, "StockKey0_iter_le"},

    {"gets_ge", (PyCFunction)StockKey0_iter_ge, METH_VARARGS|METH_KEYWORDS, "StockKey0_iter_ge"},

    {"gets_equal", (PyCFunction)StockKey0_iter_equal, METH_VARARGS|METH_KEYWORDS, "StockKey0_iter_equal"},

    {"gets_lt", (PyCFunction)StockKey0_iter_lt, METH_VARARGS|METH_KEYWORDS, "StockKey0_iter_lt"},

    {"gets_first", (PyCFunction)StockKey0_iter_first, METH_VARARGS|METH_KEYWORDS, "StockKey0_iter_first"},

    {NULL}
};

static PyTypeObject StockKey0Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.StockKey0",             /*tp_name*/
    sizeof(StockKey0),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)StockKey0_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)StockKey0_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "StockKey0 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    StockKey0_methods,             /* tp_methods */
    0,
    StockKey0_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)StockKey0_init,      /* tp_init */
    0,                         /* tp_alloc */
    StockKey0_new,                 /* tp_new */
};
PyTypeObject *getStockKey0Type()
{
    return &StockKey0Type_;
}

static void StockKey1_dealloc(PyObject* self)
{
    do_free(((StockKey1*)self)->priv);
    Py_DECREF(((StockKey1*)self)->alias);
    self->ob_type->tp_free((PyObject*)self);
    LOG("StockKey1 free\n");
}
static PyObject *StockKey1_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    LOG("StockKey1 new\n");
    StockKey1 *self;
    self = PyObject_NEW(StockKey1, type);
    //self = (StockKey1 *)type->tp_alloc(type, 0);
    if ( self == NULL )
        return NULL;
    self->priv = do_malloc0(sizeof(stock_key1_t));
    return (PyObject *)self;
}
static int StockKey1_init(StockKey1 *self, PyObject *args, PyObject *kwds)
{
    //LOG("StockKey1 init\n");
    static char *kwlist[] = {"alias", NULL};
    PyObject *alias;

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &alias) ) {
        do_log(LOG_ERR, "no param \"alias\"");
        return -1;
    }

    Py_INCREF(alias);
    self->alias = (Alias*)alias;
    
    do_text_set_empty(self->priv->code);

    PyDateTime_IMPORT;
    return 0;
}

static PyObject *StockKey1_clone(StockKey1 *self)
{
    StockKey1 *result;
    result = (StockKey1*)PyObject_CallFunction((PyObject*)getStockKey1Type(),
                                    "O", self->alias);
    memcpy(result->priv, self->priv, sizeof(*self->priv));
    return (PyObject*)result;
}

static PyObject *StockKey1_code(StockKey1* self, void *unused)
{
    PyObject *result;

    char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);

    return result;
}

static PyObject *StockKey1_set_code(StockKey1* self, PyObject *args, PyObject *kwds)
{
//    PyObject *result;

    char *value = NULL;
    static char *kwlist[] = {"value", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &value) ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set code");
        return NULL;
    }
    if ( value )
        do_text_set(self->alias->alias, self->priv->code, value);
        
    /*char *res=do_text(self->alias->alias, self->priv->code);
    result = MyString_FromString(res);
    do_free(res);*/

    Py_INCREF(Py_None);
    return Py_None;

//    return result;
}

static PyObject *StockKey1_gt(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_GT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_last(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_LAST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_next(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_NEXT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_le(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_LE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_lt(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_LT);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_equal(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_EQUAL);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_ge(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_GE);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_prev(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_first(StockKey1* self, PyObject *args, PyObject *kwds)
{
    int status;


    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_FIRST);

    if ( status == DO_ERROR )
        return NULL;
    return MyLong_FromLong((status == DO_OK) ? 1 : 0);
}

static PyObject *StockKey1_iter_gt(StockKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_GT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey1_iter_last(StockKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_LAST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey1_iter_le(StockKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_LE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey1_iter_ge(StockKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_GE);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey1_iter_equal(StockKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_EQUAL);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey1_iter_lt(StockKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_LT);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key1(self->alias->alias, self->priv, DO_GET_PREVIOUS);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey1_iter_first(StockKey1* self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"depth", NULL};
    int status;
    int depth;
    stock_key1_t key_cmp;
    PyObject *retval = NULL;
    PyObject *item;
    retval = PyList_New(0);
    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|", kwlist, &depth) ) {
        do_log(LOG_ERR, "Invalid argument");
        return NULL;
    }
    do_cpy(key_cmp, *self->priv);
    status = do_stock_key1(self->alias->alias, self->priv, DO_GET_FIRST);
    while ( status == DO_OK ) {

       if ( depth >= 1 ) {
           if ( do_cmp(key_cmp.code, 
                 self->priv->code))
               break;
       }

 
        item = StockKey1_clone(self);
        PyList_Append(retval, (PyObject*)item);
        Py_DECREF(item);        
        status = do_stock_key1(self->alias->alias, self->priv, DO_GET_NEXT);
    }
    if ( status == DO_ERROR ) {
        Py_DECREF(retval);
        return NULL;
    }
    return retval;
}

static PyObject *StockKey1_cmp(StockKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;
    int depth;
    int res = 0;

    static char *kwlist[] = {"object", "depth", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "Oi|", kwlist, &obj, &depth) ) {
        do_log(LOG_ERR, "Invalid params");
        return NULL;
    }
        
    if ( Py_TYPE(obj) == getStockKey1Type() ) {
    

       if ( !res && depth >= 1 ) {
           res = do_cmp(self->priv->code, ((StockKey1*)obj)->priv->code);
       }

       return MyLong_FromLong(res);
    }

    do_log(LOG_ERR, "Invalid argument \"object\": wrong type");
    return NULL;
}

static PyObject *StockKey1_set_alias(StockKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) != getAliasType() ) {
        do_log(LOG_ERR, "Invalid argument \"value\": wrong type. set alias");
        return NULL;
    }
    Py_DECREF(((StockKey1*)self)->alias);
    Py_INCREF(obj);
    ((StockKey1*)self)->alias = (Alias*)obj;
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *StockKey1_set(StockKey1* self, PyObject *args, PyObject *kwds)
{
    PyObject *obj;

    static char *kwlist[] = {"object", NULL};

    if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O|", kwlist, &obj) )
        return NULL;
    if ( Py_TYPE(obj) == getStockKey1Type() ) {
        memcpy(self->priv, ((StockKey1*)obj)->priv, sizeof(*self->priv));
    }

    else 
    if ( Py_TYPE(obj) == getStockType() ) {

        do_cpy(self->priv->code, ((Stock*)obj)->priv->data.code);

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
                do_log(LOG_ERR, "Class \"StockKey1\"does not contain \"%s\"", field_name);
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

static PyObject *StockKey1_fields(StockKey1* self, PyObject *args, PyObject *kwds)
{
    char *str=do_strdup("Class StockKey1");
    char *buf;
    char *buf1;
    PyObject *value = NULL;
    PyObject *retval;


    value =  StockKey1_code(self,NULL);
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

static PyGetSetDef StockKey1_getset[] = {

    {"code",(getter)StockKey1_code},

    {NULL}
};

static PyMethodDef StockKey1_methods[] = {
    {"set", (PyCFunction)StockKey1_set, METH_VARARGS|METH_KEYWORDS, "set"},
    {"set_alias", (PyCFunction)StockKey1_set_alias, METH_VARARGS|METH_KEYWORDS, "set_alias"},
    {"clone", (PyCFunction)StockKey1_clone, METH_NOARGS, "clone"},

    {"cmp", (PyCFunction)StockKey1_cmp, METH_VARARGS|METH_KEYWORDS, "cmp"},

    {"set_code", (PyCFunction)StockKey1_set_code, METH_VARARGS|METH_KEYWORDS, "StockKey1_set_code"},

    {"get_gt", (PyCFunction)StockKey1_gt, METH_VARARGS|METH_KEYWORDS, "StockKey1_gt"},

    {"get_last", (PyCFunction)StockKey1_last, METH_VARARGS|METH_KEYWORDS, "StockKey1_last"},

    {"get_next", (PyCFunction)StockKey1_next, METH_VARARGS|METH_KEYWORDS, "StockKey1_next"},

    {"get_le", (PyCFunction)StockKey1_le, METH_VARARGS|METH_KEYWORDS, "StockKey1_le"},

    {"get_lt", (PyCFunction)StockKey1_lt, METH_VARARGS|METH_KEYWORDS, "StockKey1_lt"},

    {"get_equal", (PyCFunction)StockKey1_equal, METH_VARARGS|METH_KEYWORDS, "StockKey1_equal"},

    {"get_ge", (PyCFunction)StockKey1_ge, METH_VARARGS|METH_KEYWORDS, "StockKey1_ge"},

    {"get_prev", (PyCFunction)StockKey1_prev, METH_VARARGS|METH_KEYWORDS, "StockKey1_prev"},

    {"get_first", (PyCFunction)StockKey1_first, METH_VARARGS|METH_KEYWORDS, "StockKey1_first"},

    {"gets_gt", (PyCFunction)StockKey1_iter_gt, METH_VARARGS|METH_KEYWORDS, "StockKey1_iter_gt"},

    {"gets_last", (PyCFunction)StockKey1_iter_last, METH_VARARGS|METH_KEYWORDS, "StockKey1_iter_last"},

    {"gets_le", (PyCFunction)StockKey1_iter_le, METH_VARARGS|METH_KEYWORDS, "StockKey1_iter_le"},

    {"gets_ge", (PyCFunction)StockKey1_iter_ge, METH_VARARGS|METH_KEYWORDS, "StockKey1_iter_ge"},

    {"gets_equal", (PyCFunction)StockKey1_iter_equal, METH_VARARGS|METH_KEYWORDS, "StockKey1_iter_equal"},

    {"gets_lt", (PyCFunction)StockKey1_iter_lt, METH_VARARGS|METH_KEYWORDS, "StockKey1_iter_lt"},

    {"gets_first", (PyCFunction)StockKey1_iter_first, METH_VARARGS|METH_KEYWORDS, "StockKey1_iter_first"},

    {NULL}
};

static PyTypeObject StockKey1Type_ = {
    PyVarObject_HEAD_INIT(NULL,0)
    //0,                         /*ob_size*/
    "pydomino.StockKey1",             /*tp_name*/
    sizeof(StockKey1),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)StockKey1_dealloc, /*tp_dealloc*/
    0,0,0,0,0,0,0,0,0,0,(reprfunc)StockKey1_fields,0,0,0,
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "StockKey1 objects",           /* tp_doc */
    0,0,0,0,0,0,               /* tp_iternext */
    StockKey1_methods,             /* tp_methods */
    0,
    StockKey1_getset
    ,0,0,0,0, 0,                         /* tp_dictoffset */
    (initproc)StockKey1_init,      /* tp_init */
    0,                         /* tp_alloc */
    StockKey1_new,                 /* tp_new */
};
PyTypeObject *getStockKey1Type()
{
    return &StockKey1Type_;
}
